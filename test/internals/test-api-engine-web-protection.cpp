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
#include <csre-web-protection.h>
#include <csre-web-protection-engine-info.h>

#include <unordered_map>
#include <string>

#include <boost/test/unit_test.hpp>

#include "test-common.h"

namespace {

struct Result {
	csre_wp_risk_level_e risk_level;
	std::string detailed_url;

	Result(csre_wp_risk_level_e r, const char *durl) :
		risk_level(r),
		detailed_url(durl ? durl : std::string()) {}
};

std::unordered_map<std::string, Result> ExpectedResult = {
	{"http://normal.test.com",      Result(CSRE_WP_RISK_UNVERIFIED, nullptr)},
	{"http://highrisky.test.com",   Result(CSRE_WP_RISK_HIGH, "http://high.risky.com")},
	{"http://mediumrisky.test.com", Result(CSRE_WP_RISK_MEDIUM, "http://medium.risky.com")},
	{"http://lowrisky.test.com",    Result(CSRE_WP_RISK_LOW, "http://low.risky.com")}
};

inline void checkResult(csre_wp_check_result_h &result, const Result &expected)
{
	EXCEPTION_GUARD_START

	CHECK_IS_NOT_NULL(result);

	csre_wp_risk_level_e risk_level;
	ASSERT_IF(csre_wp_result_get_risk_level(result, &risk_level), CSRE_ERROR_NONE);
	ASSERT_IF(risk_level, expected.risk_level);

	const char *detailed_url = nullptr;
	ASSERT_IF(csre_wp_result_get_detailed_url(result, &detailed_url), CSRE_ERROR_NONE);
	ASSERT_IF(detailed_url, expected.detailed_url);

	EXCEPTION_GUARD_END
}

}

BOOST_AUTO_TEST_SUITE(API_ENGINE_WEB_PROTECTION)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Test::Context<csre_wp_context_h>();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_context_h>();
	auto context = c.get();

	for (const auto &pair : ExpectedResult) {
		csre_wp_check_result_h result;
		ASSERT_IF(csre_wp_check_url(context, pair.first.c_str(), &result),
				  CSRE_ERROR_NONE);
		BOOST_MESSAGE("check result from url: " << pair.first);
		checkResult(result, pair.second);
	}

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_WEB_PROTECTION_ERR_STRING)

BOOST_AUTO_TEST_CASE(positive)
{
	EXCEPTION_GUARD_START

	int ret = CSRE_ERROR_UNKNOWN;

	const char *string = nullptr;

	ASSERT_IF(csre_wp_get_error_string(ret, &string), CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(string);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_WEB_PROTECTION_ENGINE_INFO)

BOOST_AUTO_TEST_CASE(get_engine_info)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	(void) c;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	auto handle = c.get();

	const char *vendor = nullptr;
	ASSERT_IF(csre_wp_engine_get_vendor(handle, &vendor), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor_logo)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	auto handle = c.get();

	unsigned char *vendor_logo_image = nullptr;
	unsigned int size = 0;
	ASSERT_IF(csre_wp_engine_get_vendor_logo(handle, &vendor_logo_image, &size),
			  CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_version)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	auto handle = c.get();

	const char *version = nullptr;
	ASSERT_IF(csre_wp_engine_get_version(handle, &version), CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_data_version)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	auto handle = c.get();

	const char *version = nullptr;
	ASSERT_IF(csre_wp_engine_get_data_version(handle, &version), CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_latest_update_time)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	auto handle = c.get();

	time_t time = 0;
	ASSERT_IF(csre_wp_engine_get_latest_update_time(handle, &time),
			  CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_engine_activated)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	auto handle = c.get();

	csre_wp_activated_e activated;
	ASSERT_IF(csre_wp_engine_get_activated(handle, &activated), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_api_version)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_wp_engine_h>();
	auto handle = c.get();

	const char *version = nullptr;
	ASSERT_IF(csre_wp_engine_get_api_version(handle, &version), CSRE_ERROR_NONE);
	ASSERT_IF(memcmp(version, CSRE_WP_API_VERSION, strlen(version)), 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
