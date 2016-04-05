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
#define BOOST_TEST_MODULE CSR_API_TEST
#include <csr/content-screening.h>

#include <string>
#include <memory>
#include <new>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <boost/test/unit_test.hpp>

namespace {

class ContextPtr {
public:
	ContextPtr() : m_context(nullptr) {}
	ContextPtr(csr_cs_context_h context) : m_context(context) {}
	virtual ~ContextPtr()
	{
		BOOST_REQUIRE(csr_cs_context_destroy(m_context) == CSR_ERROR_NONE);
	}

	inline csr_cs_context_h get(void)
	{
		return m_context;
	}

private:
	csr_cs_context_h m_context;
};

using ScopedContext = std::unique_ptr<ContextPtr>;

inline ScopedContext makeScopedContext(csr_cs_context_h context)
{
	return ScopedContext(new ContextPtr(context));
}

inline ScopedContext getContextHandle(void)
{
	csr_cs_context_h context;
	int ret = CSR_ERROR_UNKNOWN;
	BOOST_REQUIRE_NO_THROW(ret = csr_cs_context_create(&context));
	BOOST_REQUIRE_MESSAGE(ret == CSR_ERROR_NONE,
						  "Failed to create context handle. ret: " << ret);
	BOOST_REQUIRE(context != nullptr);
	return makeScopedContext(context);
}

}

BOOST_AUTO_TEST_SUITE(API_CONTENT_SCREENING)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	auto contextPtr = getContextHandle();
	(void) contextPtr;
}

BOOST_AUTO_TEST_CASE(scan_file)
{
	int ret = CSR_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();
	csr_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csr_cs_scan_file(context, "dummy_file_path",
								 &detected));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
}

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
	BOOST_MESSAGE("on_scanned called. file[" << file << "] scanned!");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->scannedCnt++;
}

void on_error(void *userdata, int ec)
{
	BOOST_MESSAGE("on_error called. async request done with error code[" << ec <<
				  "]");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->errorCnt++;
}

void on_detected(void *userdata, csr_cs_detected_h detected)
{
	(void) detected;
	BOOST_MESSAGE("on_detected called.");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->detectedCnt++;
}

void on_completed(void *userdata)
{
	BOOST_MESSAGE("on_completed called. async request completed succesfully.");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->completedCnt++;
	ctx->cv.notify_one();
}

void on_cancelled(void *userdata)
{
	BOOST_MESSAGE("on_cancelled called. async request canceled!");
	auto ctx = reinterpret_cast<AsyncTestContext *>(userdata);
	ctx->cancelledCnt++;
}

BOOST_AUTO_TEST_CASE(scan_files_async)
{
	int ret = CSR_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();
	BOOST_REQUIRE_NO_THROW(ret = csr_cs_set_callback_on_completed(context,
								 on_completed));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
	BOOST_REQUIRE_NO_THROW(ret = csr_cs_set_callback_on_error(context, on_error));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
	BOOST_REQUIRE_NO_THROW(ret = csr_cs_set_callback_on_cancelled(context,
								 on_cancelled));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
	BOOST_REQUIRE_NO_THROW(ret = csr_cs_set_callback_on_detected(context,
								 on_detected));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
	BOOST_REQUIRE_NO_THROW(ret = csr_cs_set_callback_on_file_scanned(context,
								 on_scanned));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
	const char *files[3] = {
		TEST_DIR "/test_malware_file",
		TEST_DIR "/test_normal_file",
		TEST_DIR "/test_risky_file"
	};
	AsyncTestContext testCtx;
	BOOST_REQUIRE_NO_THROW(ret =
							   csr_cs_scan_files_async(context, files, sizeof(files) / sizeof(const char *),
									   &testCtx));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
	std::unique_lock<std::mutex> l(testCtx.m);
	testCtx.cv.wait(l);
	l.unlock();
	BOOST_REQUIRE_MESSAGE(testCtx.completedCnt == 1 && testCtx.scannedCnt == 3 &&
						  testCtx.detectedCnt == 0 && testCtx.cancelledCnt == 0 && testCtx.errorCnt == 0,
						  "Async request result isn't expected.");
}

BOOST_AUTO_TEST_SUITE_END()
