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

namespace {

struct AsyncTestContext {
	std::mutex m;
	std::condition_variable cv;
	int scannedCnt;
	int detectedCnt;
	int completedCnt;
	int cancelledCnt;
	int errorCnt;

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

void on_error(void *userdata, int ec)
{
	BOOST_MESSAGE("on_error. async request done with error code[" << ec << "]");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->errorCnt++;
}

void on_detected(void *userdata, csr_cs_detected_h detected)
{
	(void) detected;
	BOOST_MESSAGE("on_detected.");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->detectedCnt++;
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
}

}

BOOST_AUTO_TEST_SUITE(API_CONTENT_SCREENING_ASYNC)

BOOST_AUTO_TEST_CASE(set_callbacks_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_callback_on_detected(context, on_detected), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_completed(context, on_completed), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(context, on_cancelled), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_error(context, on_error), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(context, on_scanned), CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_callbacks_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_callback_on_detected(context, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_completed(context, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(context, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_error(context, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(context, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_files_async_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_callback_on_completed(context, on_completed), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_error(context, on_error), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(context, on_cancelled), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_detected(context, on_detected), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(context, on_scanned), CSR_ERROR_NONE);

	const char *files[3] = {
		TEST_DIR "/test_malware_file",
		TEST_DIR "/test_normal_file",
		TEST_DIR "/test_risky_file"
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
	ASSERT_IF(testCtx.detectedCnt, 2);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	EXCEPTION_GUARD_END
}

// TODO: directory scanning integarted testing will be meaningful
//       after delta management by logic(+ db) is integrated.
BOOST_AUTO_TEST_CASE(scan_dir_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_callback_on_completed(context, on_completed), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_error(context, on_error), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(context, on_cancelled), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_detected(context, on_detected), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(context, on_scanned), CSR_ERROR_NONE);

	AsyncTestContext testCtx;

	ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR, &testCtx), CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.scannedCnt, 0); // should be 1 after dir_get_files implemented
	ASSERT_IF(testCtx.detectedCnt, 2);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_dirs_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_callback_on_completed(context, on_completed), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_error(context, on_error), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_cancelled(context, on_cancelled), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_detected(context, on_detected), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_callback_on_file_scanned(context, on_scanned), CSR_ERROR_NONE);

	AsyncTestContext testCtx;

	const char *dirs[1] = {
		TEST_DIR
	};

	ASSERT_IF(
		csr_cs_scan_dirs_async(context, dirs, sizeof(dirs) / sizeof(const char *),
							   &testCtx),
		CSR_ERROR_NONE);

	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();

	ASSERT_IF(testCtx.completedCnt, 1);
	ASSERT_IF(testCtx.scannedCnt, 0); // should be 1 after dir_get_files implemented
	ASSERT_IF(testCtx.detectedCnt, 2);
	ASSERT_IF(testCtx.cancelledCnt, 0);
	ASSERT_IF(testCtx.errorCnt, 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
