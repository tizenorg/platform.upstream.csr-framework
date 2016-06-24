/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        test-api-content-screening-async.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Content screening async API test
 */
#include <csr-content-screening.h>

#include <condition_variable>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <boost/test/unit_test.hpp>

#include "test-common.h"
#include "test-helper.h"
#include "test-resource.h"

#define ASSERT_CALLBACK(actual, scan, detect, complete, cancel, error)                  \
	do {                                                                                \
		std::unique_lock<std::mutex> l(actual.m);                                       \
		if (!actual.isDone()) {                                                         \
			actual.cv.wait(l);                                                          \
			l.unlock();                                                                 \
		}                                                                               \
		if (scan >= 0)                                                                  \
			ASSERT_IF_MSG(actual.scannedCnt, scan, "scanned count mismatch.");          \
		if (detect >= 0)                                                                \
			ASSERT_IF_MSG(actual.detectedCnt, detect, "detected count mismatch.");      \
		if (complete >= 0)                                                              \
			ASSERT_IF_MSG(actual.completedCnt, complete, "completed count mismatch.");  \
		if (cancel >= 0)                                                                \
			ASSERT_IF_MSG(actual.cancelledCnt, cancel, "cancelled count mismatch.");    \
		if (error >= 0)                                                                 \
			ASSERT_IF_MSG(actual.errorCnt, error, "error count mismatch.");             \
		break;                                                                          \
	} while (false)

namespace {

struct AsyncTestContext {
	std::mutex m;
	std::mutex m_vec;
	std::condition_variable cv;
	int scannedCnt;
	int detectedCnt;
	int completedCnt;
	int cancelledCnt;
	int errorCnt;
	std::vector<std::string> scannedList;
	std::vector<csr_cs_malware_h> detectedList;
	int errorCode;
	bool apiReturned;

	AsyncTestContext(bool raceTest) :
		scannedCnt(0),
		detectedCnt(0),
		completedCnt(0),
		cancelledCnt(0),
		errorCnt(0),
		apiReturned(!raceTest) {}

	AsyncTestContext() :
		scannedCnt(0),
		detectedCnt(0),
		completedCnt(0),
		cancelledCnt(0),
		errorCnt(0),
		apiReturned(true) {}

	bool isDone(void) const {
		return this->completedCnt != 0 || this->cancelledCnt != 0 || this->errorCnt != 0;
	}
};

void on_scanned(const char *file, void *userdata)
{
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);

	BOOST_REQUIRE_MESSAGE(ctx->apiReturned,
		"API not returned yet but scanned callback called on file: " << file);

	BOOST_MESSAGE("on_scanned. file[" << file << "] scanned!");

	std::lock_guard<std::mutex> l(ctx->m_vec);

	ctx->scannedCnt++;
	ctx->scannedList.push_back(file);
}

void on_detected(csr_cs_malware_h detected, void *userdata)
{
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);

	BOOST_REQUIRE_MESSAGE(ctx->apiReturned,
		"API not returned yet but detected callback called");

	Test::ScopedCstr file_name;
	ASSERT_SUCCESS(csr_cs_malware_get_file_name(detected, &file_name.ptr));
	BOOST_MESSAGE("on_detected. file[" << file_name.ptr << "] detected!");

	std::lock_guard<std::mutex> l(ctx->m_vec);

	ctx->detectedCnt++;
	ctx->detectedList.push_back(detected);
}

void on_error(int ec, void *userdata)
{
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);

	BOOST_REQUIRE_MESSAGE(ctx->apiReturned,
		"API not returned yet but error callback called with error: " <<
		Test::capi_ec_to_string(ec));

	BOOST_MESSAGE("on_error. async request done with error: " <<
				  Test::capi_ec_to_string(ec));

	ctx->errorCnt++;
	ctx->errorCode = ec;
	ctx->cv.notify_one();
}

void on_completed(void *userdata)
{
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);

	BOOST_REQUIRE_MESSAGE(ctx->apiReturned,
		"API not returned yet but completed callback called");

	BOOST_MESSAGE("on_completed. async request completed succesfully.");
	ctx->completedCnt++;
	ctx->cv.notify_one();
}

void on_cancelled(void *userdata)
{
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);

	BOOST_REQUIRE_MESSAGE(ctx->apiReturned,
		"API not returned yet but cancelled callback called");

	BOOST_MESSAGE("on_cancelled. async request canceled!");
	ctx->cancelledCnt++;
	ctx->cv.notify_one();
}

void set_default_callback(csr_cs_context_h context)
{
	ASSERT_SUCCESS(csr_cs_set_detected_cb(context, on_detected));
	ASSERT_SUCCESS(csr_cs_set_completed_cb(context, on_completed));
	ASSERT_SUCCESS(csr_cs_set_cancelled_cb(context, on_cancelled));
	ASSERT_SUCCESS(csr_cs_set_error_cb(context, on_error));
	ASSERT_SUCCESS(csr_cs_set_file_scanned_cb(context, on_scanned));
}

void install_test_files()
{
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA());
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_TMP);

	Test::make_dir(TEST_FAKE_APP_ROOT());
	Test::copy_file(TEST_FILE_HIGH, TEST_FAKE_APP_FILE());
}

void uninstall_test_files()
{
	Test::remove_file(TEST_FILE_MEDIA());
	Test::remove_file(TEST_FILE_TMP);
	Test::remove_file(TEST_FAKE_APP_FILE());
	Test::remove_file(TEST_FAKE_APP_ROOT());
}

void uninstall_test_apps()
{
	Test::uninstall_app(TEST_WGT_PKG_ID);
	Test::uninstall_app(TEST_TPK_PKG_ID);
	Test::uninstall_app(TEST_SAFE_WGT_PKG_ID);
}

void install_test_apps()
{
	uninstall_test_apps();

	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);
	ASSERT_INSTALL_APP(TEST_SAFE_WGT_PATH, TEST_SAFE_WGT_TYPE);
}

} // end of namespace

BOOST_AUTO_TEST_SUITE(API_CONTENT_SCREENING_ASYNC)

BOOST_AUTO_TEST_CASE(set_callbacks_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_callbacks_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_detected_cb(nullptr, on_detected), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_detected_cb(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_completed_cb(nullptr, on_completed), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_completed_cb(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_cancelled_cb(nullptr, on_cancelled), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_cancelled_cb(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_error_cb(nullptr, on_error), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_error_cb(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_file_scanned_cb(nullptr, on_scanned), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_file_scanned_cb(context, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_files_async_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	const char *files[4] = {
		TEST_FILE_HIGH,
		TEST_FILE_MEDIUM,
		TEST_FILE_LOW,
		TEST_FILE_NORMAL
	};

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_files_async(context, files, 4, &testCtx));

	ASSERT_CALLBACK(testCtx, -1, 3, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_HIGH, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_HIGH, false, "");

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_MEDIUM, MALWARE_MEDIUM_NAME,
							MALWARE_MEDIUM_SEVERITY, MALWARE_MEDIUM_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_MEDIUM, false, "");

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_LOW, MALWARE_LOW_NAME,
							MALWARE_LOW_SEVERITY, MALWARE_LOW_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_LOW, false, "");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_files_async_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	const char *files[3] = {
		TEST_FILE_HIGH,
		TEST_FILE_NORMAL,
		TEST_FILE_MEDIUM
	};

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_files_async(nullptr, files, 3, nullptr), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_scan_files_async(context, nullptr, 3, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_MALWARES, &testCtx));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, -1, 1, 0, 0);
	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_UNREMOVABLE,
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_UNREMOVABLE, false, "");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(nullptr, TEST_DIR_MALWARES, nullptr), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_scan_dir_async(context, nullptr, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_root)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();
	install_test_apps();

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_ROOT, &testCtx));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, -1, 1, 0, 0);

	Test::uninstall_app(TEST_WGT_PKG_ID);
	Test::uninstall_app(TEST_TPK_PKG_ID);

	std::string homeDirPrefix;
#ifdef PLATFORM_VERSION_3
	// "/home" is symlinked of "/opt/home" so in root directory scanning,
	// user directory prefix("/opt") is additionally needed to check file_name field
	// in malware handle
	homeDirPrefix = "/opt";
#endif

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_HIGH, MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_HIGH, false, "");

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, homeDirPrefix + TEST_FILE_MEDIA(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, homeDirPrefix + TEST_FILE_MEDIA(),
								false, "");

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_TMP, MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_TMP, false, "");

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, homeDirPrefix + TEST_WGT_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, homeDirPrefix + TEST_WGT_APP_ROOT(),
								true, TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, homeDirPrefix + TEST_TPK_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, homeDirPrefix + TEST_TPK_APP_ROOT(),
								true, TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, homeDirPrefix + TEST_FAKE_APP_FILE(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, homeDirPrefix + TEST_FAKE_APP_FILE(),
								false, "");

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_MEDIUM, MALWARE_MEDIUM_NAME,
							MALWARE_MEDIUM_SEVERITY, MALWARE_MEDIUM_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_MEDIUM, false, "");

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_LOW, MALWARE_LOW_NAME,
							MALWARE_LOW_SEVERITY, MALWARE_LOW_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_LOW, false, "");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_media)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_MEDIA(), &testCtx));

	// scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, 1, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_MEDIA(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_MEDIA(), false, "");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_tmp)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_TMP, &testCtx));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, 1, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_TMP, MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_TMP, false, "");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_apps)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();
	install_test_apps();

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testCtx));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, 3, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_WGT_APP_ROOT(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_WGT_APP_ROOT(), true,
								TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_TPK_APP_ROOT(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_TPK_APP_ROOT(), true,
								TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FAKE_APP_FILE(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FAKE_APP_FILE(), false, "");

	uninstall_test_apps();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_wgt)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_WGT_APP_ROOT(), &testCtx));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, 1, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_WGT_APP_ROOT(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_WGT_APP_ROOT(), true,
								TEST_WGT_PKG_ID);

	Test::uninstall_app(TEST_WGT_PKG_ID);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_tpk)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_TPK_APP_ROOT(), &testCtx));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, 1, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_TPK_APP_ROOT(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_TPK_APP_ROOT(), true,
								TEST_TPK_PKG_ID);

	Test::uninstall_app(TEST_TPK_PKG_ID);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dirs_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	AsyncTestContext testCtx;

	const char *dirs[1] = {
		TEST_DIR_MALWARES
	};

	ASSERT_SUCCESS(csr_cs_scan_dirs_async(context, dirs, 1, &testCtx));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, -1, 1, 0, 0);
	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FILE_UNREMOVABLE,
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FILE_UNREMOVABLE, false, "");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dirs_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	const char *dirs[1] = {
		TEST_DIR_MALWARES
	};

	ASSERT_IF(csr_cs_scan_dirs_async(nullptr, dirs, 1, nullptr), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_scan_dirs_async(context, nullptr, 1, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_cancel_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	AsyncTestContext testCtx;

	const char *dirs[1] = {
		"/"
	};

	// touch a file : in case of no target file to scan, we cannot cancel to scan.
	Test::touch_file(TEST_FILE_HIGH);

	ASSERT_SUCCESS(csr_cs_scan_dirs_async(context, dirs, 1, &testCtx));
	ASSERT_SUCCESS(csr_cs_cancel_scanning(context));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, -1, 0, 1, 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_cancel_positive_100)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	AsyncTestContext testCtx;

	const char *dirs[1] = {
		"/"
	};

	// touch a file : in case of no target file to scan, we cannot cancel to scan.
	Test::touch_file(TEST_FILE_HIGH);

	ASSERT_SUCCESS(csr_cs_scan_dirs_async(context, dirs, 1, &testCtx));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	ASSERT_SUCCESS(csr_cs_cancel_scanning(context));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, -1, 0, 1, 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_cancel_positive_500)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	AsyncTestContext testCtx;

	const char *dirs[1] = {
		"/"
	};

	// touch a file : in case of no target file to scan, we cannot cancel to scan.
	Test::touch_file(TEST_FILE_HIGH);

	ASSERT_SUCCESS(csr_cs_scan_dirs_async(context, dirs, 1, &testCtx));

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	ASSERT_SUCCESS(csr_cs_cancel_scanning(context));

	//scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testCtx, -1, -1, 0, 1, 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_cancel_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_cancel_scanning(nullptr), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_cancel_scanning(context), CSR_ERROR_NO_TASK);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(delta_scan_basic)
{
	EXCEPTION_GUARD_START

	Test::initialize_db();

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();
	install_test_apps();

	set_default_callback(context);

	// Base Scan
	AsyncTestContext testBaseCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testBaseCtx));

	// scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testBaseCtx, -1, -1, 1, 0, 0);
	BOOST_REQUIRE_MESSAGE(testBaseCtx.scannedCnt > 0,
			"Base Scan count should not be zero");

	ASSERT_DETECTED_IN_LIST(testBaseCtx.detectedList, TEST_WGT_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testBaseCtx.detectedList, TEST_WGT_APP_ROOT(), true,
								TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testBaseCtx.detectedList, TEST_TPK_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testBaseCtx.detectedList, TEST_TPK_APP_ROOT(), true,
								TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testBaseCtx.detectedList, TEST_FAKE_APP_FILE(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testBaseCtx.detectedList, TEST_FAKE_APP_FILE(), false, "");

	// Rescan the same dir
	AsyncTestContext testRescanCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testRescanCtx));

	// scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testRescanCtx, -1, -1, 1, 0, 0);
	BOOST_REQUIRE_MESSAGE(testBaseCtx.scannedCnt > testRescanCtx.scannedCnt,
			"Scan count of the base[" << testBaseCtx.scannedCnt <<
			"] should be bigger than the delta-scan[" << testRescanCtx.scannedCnt << "]");

	ASSERT_DETECTED_IN_LIST(testRescanCtx.detectedList, TEST_WGT_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testRescanCtx.detectedList, TEST_WGT_APP_ROOT(), true,
								TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testRescanCtx.detectedList, TEST_TPK_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testRescanCtx.detectedList, TEST_TPK_APP_ROOT(), true,
								TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testRescanCtx.detectedList, TEST_FAKE_APP_FILE(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testRescanCtx.detectedList, TEST_FAKE_APP_FILE(), false,
								"");

	// Rescan the sub dir
	AsyncTestContext testRescanSubCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_WGT_APP_ROOT(), &testRescanSubCtx));

	// scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testRescanSubCtx, 0, 1, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testRescanSubCtx.detectedList, TEST_WGT_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testRescanSubCtx.detectedList, TEST_WGT_APP_ROOT(), true,
								TEST_WGT_PKG_ID);

	uninstall_test_apps();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(delta_scan_changed_after_scan)
{
	EXCEPTION_GUARD_START

	Test::initialize_db();

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();
	install_test_apps();

	set_default_callback(context);

	// Base Scan
	AsyncTestContext testBaseCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testBaseCtx));

	// scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testBaseCtx, -1, 3, 1, 0, 0);
	BOOST_REQUIRE_MESSAGE(testBaseCtx.scannedCnt != 0,
			"Base Scan count should not be zero");

	// changed
	Test::uninstall_app(TEST_SAFE_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_SAFE_WGT_PATH, TEST_SAFE_WGT_TYPE);

	// Rescan the same dir
	AsyncTestContext testRescanCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testRescanCtx));

	// scanned, detected, completed, cancelled, error
	ASSERT_CALLBACK(testRescanCtx, -1, -1, 1, 0, 0);
	BOOST_REQUIRE_MESSAGE(testRescanCtx.scannedCnt >= 1,
			"reinstalled app(non-malicious wgt) should be counted in scanned.");

	ASSERT_DETECTED_IN_LIST(testRescanCtx.detectedList, TEST_WGT_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testRescanCtx.detectedList, TEST_WGT_APP_ROOT(), true,
								TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testRescanCtx.detectedList, TEST_TPK_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testRescanCtx.detectedList, TEST_TPK_APP_ROOT(), true,
								TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testRescanCtx.detectedList, TEST_FAKE_APP_FILE(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testRescanCtx.detectedList, TEST_FAKE_APP_FILE(), false,
								"");

	uninstall_test_apps();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(canonicalize_files_absolute_path)
{
	EXCEPTION_GUARD_START

	Test::initialize_db();

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();

	set_default_callback(context);

	// four files are all same as realpath.
	const char *files[4] = {
		TEST_FILE_NORMAL,
		TEST_DIR "/./test_normal_file",
		TEST_DIR "/.././././csr-test/test_normal_file",
		TEST_DIR "/././.././csr-test/test_normal_file"
	};

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_files_async(context, files, 4, &testCtx));

	ASSERT_CALLBACK(testCtx, 1, 0, 1, 0, 0);
	ASSERT_IF(testCtx.scannedList.size(), static_cast<std::size_t>(1));
	ASSERT_IF(testCtx.scannedList.front(), static_cast<const char *>(TEST_FILE_NORMAL));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(canonicalize_files_relative_path)
{
	EXCEPTION_GUARD_START

	Test::initialize_db();

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();

	set_default_callback(context);

	// four files are all same as realpath.
	const char *files[4] = {
		"test_normal_file",
		"./test_normal_file",
		".././././csr-test/test_normal_file",
		"././.././csr-test/test_normal_file"
	};

	AsyncTestContext testCtx;

	Test::ScopedChDir scopedCd(TEST_DIR);

	ASSERT_SUCCESS(csr_cs_scan_files_async(context, files, 4, &testCtx));

	ASSERT_CALLBACK(testCtx, 1, 0, 1, 0, 0);
	ASSERT_IF(testCtx.scannedList.size(), static_cast<std::size_t>(1));
	ASSERT_IF(testCtx.scannedList.front(), static_cast<const char *>(TEST_FILE_NORMAL));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(multiple_async_dispatch_negative)
{
	EXCEPTION_GUARD_START

	Test::initialize_db();

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();
	install_test_apps();

	set_default_callback(context);

	AsyncTestContext testCtx;

	// first call. it'll running in background asynchronously.
	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testCtx));

	AsyncTestContext testCtx2;

	// second call while first call is running in background. It should blocked because
	// only one operation can be running asynchronously on one handle.
	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testCtx2), CSR_ERROR_BUSY);

	ASSERT_CALLBACK(testCtx, -1, 3, 1, 0, 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_malware_after_async)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	const char *dirs[4] = {
		"/opt/usr/media/",
		"/opt/usr/apps/",
		"/tmp/",
		"/sdcard/"
	};

	csr_cs_malware_list_h malwares = nullptr;
	csr_cs_malware_h malware = nullptr;
	size_t count = 0;

	ASSERT_SUCCESS(csr_cs_get_detected_malwares(context, dirs, 4, &malwares, &count));
	BOOST_MESSAGE("detected malware exist count before scanning: " << count);

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dirs_async(context, dirs, 4, &testCtx));

	ASSERT_CALLBACK(testCtx, -1, -1, -1, -1, -1);

	BOOST_MESSAGE("scanned count: " << testCtx.scannedCnt);
	BOOST_MESSAGE("detected count: " << testCtx.detectedCnt);
	BOOST_MESSAGE("completed count: " << testCtx.completedCnt);
	BOOST_MESSAGE("cancelled count: " << testCtx.cancelledCnt);
	BOOST_MESSAGE("error count: " << testCtx.errorCnt);

	ASSERT_SUCCESS(csr_cs_get_detected_malwares(context, dirs, 4, &malwares, &count));

	CHECK_IS_NOT_NULL(malwares);

	for (size_t i = 0; i < count; ++i) {
		malware = nullptr;
		Test::ScopedCstr filepath;
		ASSERT_SUCCESS(csr_cs_malware_list_get_malware(malwares, i, &malware));
		CHECK_IS_NOT_NULL(malware);
		ASSERT_SUCCESS(csr_cs_malware_get_file_name(malware, &filepath.ptr));
		CHECK_IS_NOT_NULL(filepath.ptr);
		BOOST_MESSAGE("detect malware from file: " << filepath.ptr);
	}

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(async_api_returning_and_callback_race)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();

	set_default_callback(context);

	const char *files[2] = {
		TEST_FILE_NORMAL,
		TEST_FILE_HIGH
	};

	AsyncTestContext testCtx(true);

	ASSERT_SUCCESS(csr_cs_scan_files_async(context, files, 2, &testCtx));
	testCtx.apiReturned = true;

	ASSERT_CALLBACK(testCtx, -1, -1, -1, -1, -1);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_app_on_cloud)
{
	EXCEPTION_GUARD_START

	BOOST_MESSAGE("Only valid for engines which supports scan on cloud feature");

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	install_test_files();
	install_test_apps();

	set_default_callback(context);
	ASSERT_SUCCESS(csr_cs_set_scan_on_cloud(context, true));

	AsyncTestContext testCtx;

	ASSERT_SUCCESS(csr_cs_scan_dir_async(context, TEST_DIR_APPS(), &testCtx));

	ASSERT_CALLBACK(testCtx, -1, -1, 1, 0, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_WGT_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_WGT_APP_ROOT(), true,
								TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_TPK_APP_ROOT(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_TPK_APP_ROOT(), true,
								TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList, TEST_FAKE_APP_FILE(),
							MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
							MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList, TEST_FAKE_APP_FILE(), false, "");

	const char *dirs[1] = {
		TEST_DIR_APPS()
	};
	csr_cs_malware_list_h malwares = nullptr;
	size_t count = 0;

	ASSERT_SUCCESS(csr_cs_get_detected_malwares(context, dirs, 1, &malwares, &count));
	CHECK_IS_NOT_NULL(malwares);

	std::vector<csr_cs_malware_h> malware_vec;
	for (size_t i = 0; i < count; ++i) {
		csr_cs_malware_h malware = nullptr;
		ASSERT_SUCCESS(csr_cs_malware_list_get_malware(malwares, i, &malware));

		malware_vec.push_back(malware);
	}

	ASSERT_DETECTED_IN_LIST(malware_vec, TEST_WGT_APP_ROOT(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(malware_vec, TEST_WGT_APP_ROOT(), true, TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(malware_vec, TEST_TPK_APP_ROOT(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(malware_vec, TEST_TPK_APP_ROOT(), true, TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(malware_vec, TEST_FAKE_APP_FILE(), MALWARE_HIGH_NAME,
							MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(malware_vec, TEST_FAKE_APP_FILE(), false, "");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_async_multiple)
{
	EXCEPTION_GUARD_START

	constexpr size_t NUM = 3;

	install_test_files();
	install_test_apps();

	std::vector<Test::Context<csr_cs_context_h>> cs(NUM);
	std::vector<csr_cs_context_h> contexts(NUM);
	std::vector<AsyncTestContext> testCtxs(NUM);

	for (size_t i = 0; i < NUM; ++i) {
		contexts[i] = cs[i].get();

		set_default_callback(contexts[i]);

		ASSERT_SUCCESS(csr_cs_scan_dir_async(contexts[i], TEST_DIR_ROOT, &testCtxs[i]));
	}

	for (size_t i = 0; i < NUM; ++i)
		ASSERT_CALLBACK(testCtxs[i], -1, -1, 1, 0, 0);

	std::string homeDirPrefix;
#ifdef PLATFORM_VERSION_3
	// "/home" is symlinked of "/opt/home" so in root directory scanning,
	// user directory prefix("/opt") is additionally needed to check file_name field
	// in malware handle
	homeDirPrefix = "/opt";
#endif

	for (size_t i = 0; i < NUM; ++i) {
		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, TEST_FILE_HIGH, MALWARE_HIGH_NAME,
								MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, TEST_FILE_HIGH, false, "");

		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, homeDirPrefix + TEST_FILE_MEDIA(),
								MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
								MALWARE_HIGH_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, homeDirPrefix + TEST_FILE_MEDIA(),
									false, "");

		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, TEST_FILE_TMP, MALWARE_HIGH_NAME,
								MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, TEST_FILE_TMP, false, "");

		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, homeDirPrefix + TEST_WGT_APP_ROOT(),
								MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
								MALWARE_HIGH_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, homeDirPrefix + TEST_WGT_APP_ROOT(),
									true, TEST_WGT_PKG_ID);

		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, homeDirPrefix + TEST_TPK_APP_ROOT(),
								MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
								MALWARE_HIGH_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, homeDirPrefix + TEST_TPK_APP_ROOT(),
									true, TEST_TPK_PKG_ID);

		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, homeDirPrefix + TEST_FAKE_APP_FILE(),
								MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
								MALWARE_HIGH_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, homeDirPrefix + TEST_FAKE_APP_FILE(),
									false, "");

		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, TEST_FILE_MEDIUM, MALWARE_MEDIUM_NAME,
								MALWARE_MEDIUM_SEVERITY, MALWARE_MEDIUM_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, TEST_FILE_MEDIUM, false, "");

		ASSERT_DETECTED_IN_LIST(testCtxs[i].detectedList, TEST_FILE_LOW, MALWARE_LOW_NAME,
								MALWARE_LOW_SEVERITY, MALWARE_LOW_DETAILED_URL);
		ASSERT_DETECTED_IN_LIST_EXT(testCtxs[i].detectedList, TEST_FILE_LOW, false, "");

	}

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
