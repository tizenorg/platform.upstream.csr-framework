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
 * @file        test-api-engine-web-protection.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Content screening Engine API test
 */
#include <csre/web-protection.h>
#include <csre/web-protection-engine-info.h>

#include <unordered_map>
#include <string>

#include <boost/test/unit_test.hpp>

#define CHECK_IS_NULL(ptr)     BOOST_REQUIRE(ptr == nullptr)
#define CHECK_IS_NOT_NULL(ptr) BOOST_REQUIRE(ptr != nullptr)

namespace {

struct Result {
	csre_wp_risk_level_e risk_level;
	std::string detailed_url;

	Result(csre_wp_risk_level_e r, const char* durl) : risk_level(r), detailed_url(durl) {}
};

std::unordered_map<std::string, Result> ExpectedResult = {
	{"http://normal.test.com",      Result(CSRE_WP_RISK_UNVERIFIED, "")},
	{"http://highrisky.test.com",   Result(CSRE_WP_RISK_HIGH, "http://high.risky.com")},
	{"http://mediumrisky.test.com", Result(CSRE_WP_RISK_MEDIUM, "http://medium.risky.com")},
	{"http://lowrisky.test.com",    Result(CSRE_WP_RISK_LOW, "")}
};

inline void checkResult(csre_wp_check_result_h &result, const Result &expected)
{
	int ret = CSRE_ERROR_UNKNOWN;
	CHECK_IS_NOT_NULL(result);

	csre_wp_risk_level_e risk_level;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_result_get_risk_level(result, &risk_level));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE_MESSAGE(risk_level == expected.risk_level,
		"risk level isn't expected value. "
			"val: " << risk_level << " expected: " << expected.risk_level);

	const char *detailed_url = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_result_get_detailed_url(result, &detailed_url));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE_MESSAGE(expected.detailed_url.compare(detailed_url) == 0,
		"detailed url isn't expected value. "
			"val: " << detailed_url <<" expected: " << expected.detailed_url);
}

class ContextPtr {
public:
	ContextPtr() : m_context(nullptr) {}
	ContextPtr(csre_wp_context_h context) : m_context(context) {}
	virtual ~ContextPtr()
	{
		int ret = csre_wp_context_destroy(m_context);
		BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	}

	csre_wp_context_h get(void)
	{
		return m_context;
	}

private:
	csre_wp_context_h m_context;
};

using ScopedContext = std::unique_ptr<ContextPtr>;

inline ScopedContext makeScopedContext(csre_wp_context_h context)
{
	return ScopedContext(new ContextPtr(context));
}

inline csre_wp_engine_h getEngineHandle(void)
{
	csre_wp_engine_h engine;
	int ret = CSRE_ERROR_UNKNOWN;

	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_info(&engine));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(engine);

	return engine;
}

inline ScopedContext getContextHandleWithDir(const char *dir)
{
	csre_wp_context_h context;
	int ret = CSRE_ERROR_UNKNOWN;

	BOOST_REQUIRE_NO_THROW(ret = csre_wp_context_create(dir, &context));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(context);

	return makeScopedContext(context);
}

inline ScopedContext getContextHandle(void)
{
	return getContextHandleWithDir(SAMPLE_ENGINE_WORKING_DIR);
}

}

BOOST_AUTO_TEST_SUITE(API_ENGINE_WEB_PROTECTION)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	auto handle = getContextHandle();
	(void)handle;
}

BOOST_AUTO_TEST_CASE(check_url)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	for (const auto &pair : ExpectedResult) {
		csre_wp_check_result_h result;
		BOOST_REQUIRE_NO_THROW(ret = csre_wp_check_url(context, pair.first.c_str(), &result));

		BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
		checkResult(result, pair.second);
	}
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_WEB_PROTECTION_ERR_STRING)

BOOST_AUTO_TEST_CASE(positive)
{
	int ret = CSRE_ERROR_UNKNOWN;

	const char *string = nullptr;

	BOOST_REQUIRE_NO_THROW(ret = csre_wp_get_error_string(ret, &string));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(string);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_WEB_PROTECTION_ENGINE_INFO)

BOOST_AUTO_TEST_CASE(get_engine_info)
{
	auto handle = getEngineHandle();
	(void)handle;
}

BOOST_AUTO_TEST_CASE(get_vendor)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *vendor = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_vendor(handle, &vendor));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(get_vendor_logo)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	unsigned char *vendor_logo_image = nullptr;
	unsigned int size = 0;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_vendor_logo(handle, &vendor_logo_image, &size));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(get_version)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *version = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_version(handle, &version));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);
}

BOOST_AUTO_TEST_CASE(get_data_version)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *version = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_data_version(handle, &version));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);
}

BOOST_AUTO_TEST_CASE(get_latest_update_time)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	time_t time = 0;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_latest_update_time(handle, &time));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE(time > 0);
}

BOOST_AUTO_TEST_CASE(get_engine_activated)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	csre_wp_activated_e activated;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_activated(handle, &activated));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(get_api_version)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *version = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_wp_engine_get_api_version(handle, &version));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE(memcmp(version, CSRE_WP_API_VERSION, strlen(version)) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
