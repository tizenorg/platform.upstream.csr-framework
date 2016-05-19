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
 * @file        test-api-content-screening.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Content screening API test
 */
#include <csr/content-screening.h>

#include <condition_variable>
#include <thread>
#include <mutex>
#include <boost/test/unit_test.hpp>

#include "test-common.h"
#include "test-helper.h"
#include "test-resource.h"

namespace {

struct AsyncTestContext {
	std::mutex m;
	std::condition_variable cv;
	int scannedCnt;
	int detectedCnt;
	int completedCnt;
	int cancelledCnt;
	int errorCnt;
	std::vector<csr_cs_detected_h> detectedList;
	int errorCode;

	AsyncTestContext() :
		scannedCnt(0),
		detectedCnt(0),
		completedCnt(0),
		cancelledCnt(0),
		errorCnt(0) {}
};

void on_scanned(void *userdata, const char *file)
{
	BOOST_MESSAGE("on_scanned. file[" << file << "] scanned!");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->scannedCnt++;
}

void on_detected(void *userdata, csr_cs_detected_h detected)
{
	const char *file_name;
	ASSERT_IF(csr_cs_detected_get_file_name(detected, &file_name), CSR_ERROR_NONE);
	BOOST_MESSAGE("on_detected. file[" << file_name << "] detected!");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->detectedCnt++;
	ctx->detectedList.push_back(detected);
}

void on_error(void *userdata, int ec)
{
	BOOST_MESSAGE("on_error. async request done with error code[" << ec << "]");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->errorCnt++;
	ctx->errorCode = ec;
	ctx->cv.notify_one();
}

void on_completed(void *userdata)
{
	BOOST_MESSAGE("on_completed. async request completed succesfully.");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->completedCnt++;
	ctx->cv.notify_one();
}

void on_cancelled(void *userdata)
{
	BOOST_MESSAGE("on_cancelled. async request canceled!");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->cancelledCnt++;
	ctx->cv.notify_one();
}

}

void set_default_callback(csr_cs_context_h context)
{
	ASSERT_IF(csr_cs_set_callback_on_detected(context, on_detected), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_completed(context, on_completed), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(context, on_cancelled), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_error(context, on_error), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(context, on_scanned), CSR_ERROR_NONE);
}

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

	ASSERT_IF(csr_cs_set_callback_on_detected(nullptr, on_detected), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_callback_on_detected(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_completed(nullptr, on_completed), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_callback_on_completed(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(nullptr, on_cancelled), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_error(nullptr, on_error), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_callback_on_error(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(nullptr, on_scanned), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(context, nullptr), CSR_ERROR_INVALID_PARAMETER);

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

	ASSERT_IF(
		csr_cs_scan_files_async(context, files, sizeof(files) / sizeof(const char *),
								&testCtx),
		CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.scannedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 3);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_HIGH, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_HIGH, false, nullptr);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_MEDIUM, MALWARE_MEDIUM_NAME, MALWARE_MEDIUM_SEVERITY, MALWARE_MEDIUM_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_MEDIUM, false, nullptr);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_LOW, MALWARE_LOW_NAME, MALWARE_LOW_SEVERITY, MALWARE_LOW_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_LOW, false, nullptr);

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

	ASSERT_IF(
		csr_cs_scan_files_async(nullptr, files, sizeof(files) / sizeof(const char *),
								nullptr),
		CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(
		csr_cs_scan_files_async(context, nullptr, sizeof(files) / sizeof(const char *),
								nullptr),
		CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

// TODO: directory scanning integarted testing will be meaningful
//       after delta management by logic(+ db) is integrated.
BOOST_AUTO_TEST_CASE(scan_dir_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR_MALWARES, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 0);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);
	ASSERT_IF(testCtx.scannedCnt == 8 || testCtx.scannedCnt == 0, true); // in case of delta

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(nullptr, TEST_DIR_MALWARES, nullptr),
		CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_scan_dir_async(context, nullptr, nullptr),
		CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_root)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_TMP);
	
	Test::make_dir(TEST_FAKE_APP_ROOT);
	Test::copy_file(TEST_FILE_HIGH, TEST_FAKE_APP_FILE);

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR_ROOT, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	Test::uninstall_app(TEST_WGT_PKG_ID);
	Test::uninstall_app(TEST_TPK_PKG_ID);

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 8);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_HIGH, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_HIGH, false, nullptr);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_MEDIUM, MALWARE_MEDIUM_NAME, MALWARE_MEDIUM_SEVERITY, MALWARE_MEDIUM_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_MEDIUM, false, nullptr);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_LOW, MALWARE_LOW_NAME, MALWARE_LOW_SEVERITY, MALWARE_LOW_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_LOW, false, nullptr);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_MEDIA, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_MEDIA, false, nullptr);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_TMP, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_TMP, false, nullptr);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_WGT_APP_ROOT, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_WGT_APP_ROOT, true, TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_TPK_APP_ROOT, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_TPK_APP_ROOT, true, TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FAKE_APP_FILE, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FAKE_APP_FILE, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_media)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR_MEDIA, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 1);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_MEDIA, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_MEDIA, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_tmp)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_TMP);

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR_TMP, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 1);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FILE_TMP, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FILE_TMP, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dir_apps)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	Test::make_dir(TEST_FAKE_APP_ROOT);
	Test::copy_file(TEST_FILE_HIGH, TEST_FAKE_APP_FILE);

	set_default_callback(context);

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR_APPS, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 3);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_WGT_APP_ROOT, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_WGT_APP_ROOT, true, TEST_WGT_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_TPK_APP_ROOT, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_TPK_APP_ROOT, true, TEST_TPK_PKG_ID);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_FAKE_APP_FILE, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_FAKE_APP_FILE, false, nullptr);

	Test::uninstall_app(TEST_WGT_PKG_ID);
	Test::uninstall_app(TEST_TPK_PKG_ID);

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

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_WGT_APP_ROOT, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 1);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_WGT_APP_ROOT, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_WGT_APP_ROOT, true, TEST_WGT_PKG_ID);

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

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_TPK_APP_ROOT, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 1);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	ASSERT_DETECTED_IN_LIST(testCtx.detectedList,
		TEST_TPK_APP_ROOT, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_IN_LIST_EXT(testCtx.detectedList,
		TEST_TPK_APP_ROOT, true, TEST_TPK_PKG_ID);

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

	ASSERT_IF(
		csr_cs_scan_dirs_async(context, dirs, sizeof(dirs) / sizeof(const char *),
							   &testCtx),
		CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.detectedCnt, 0);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);
	ASSERT_IF(testCtx.scannedCnt == 8 || testCtx.scannedCnt == 0, true); // in case of delta

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

	ASSERT_IF(
		csr_cs_scan_dirs_async(nullptr, dirs, sizeof(dirs) / sizeof(const char *),
							   nullptr),
		CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(
		csr_cs_scan_dirs_async(context, nullptr, sizeof(dirs) / sizeof(const char *),
							   nullptr),
		CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}


BOOST_AUTO_TEST_CASE(scan_cancel_positiive)
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

	ASSERT_IF(csr_cs_scan_dirs_async(context, dirs, sizeof(dirs) / sizeof(const char *), &testCtx),
			CSR_ERROR_NONE);

	// TODO: check the reason
	// Without sleep, sometimes the first run of this TC fails with core dump.
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	ASSERT_IF(csr_cs_scan_cancel(context), CSR_ERROR_NONE);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	ASSERT_IF(testCtx.completedCnt, 0);
	ASSERT_IF(testCtx.cancelledCnt, 1);
	ASSERT_IF(testCtx.errorCnt, 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_cancel_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_scan_cancel(nullptr), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_scan_cancel(context), CSR_ERROR_NO_TASK);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
