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
 * @file        test-api-web-protection.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Web protection API test
 */
#include <csr/web-protection.h>

#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "test-common.h"
#include "test-helper.h"

#define RISK_HIGH_RISK              CSR_WP_RISK_HIGH
#define RISK_HIGH_URL               "http://highrisky.test.com/abc/def"
#define RISK_HIGH_DETAILED_URL      "http://high.risky.com"

#define RISK_MEDIUM_RISK            CSR_WP_RISK_MEDIUM
#define RISK_MEDIUM_URL             "https://mediumrisky.test.com:80/abc/def"
#define RISK_MEDIUM_DETAILED_URL    "http://medium.risky.com"

#define RISK_LOW_RISK               CSR_WP_RISK_LOW
#define RISK_LOW_URL                "lowrisky.test.com:8080/abc/def"
#define RISK_LOW_DETAILED_URL       "http://low.risky.com"

#define RISK_UNVERIFIED_RISK         CSR_WP_RISK_UNVERIFIED
#define RISK_UNVERIFIED_URL          "http://unverified.test.com:8080/abc/def"
#define RISK_UNVERIFIED_DETAILED_URL (nullptr)


void ASSERT_RESULT(csr_wp_check_result_h result,
	csr_wp_risk_level_e risk, const char * detailed_url, csr_wp_user_response_e response)
{
	csr_wp_risk_level_e a_risk;
	const char *a_detailed_url;
	csr_wp_user_response_e a_response;

	BOOST_REQUIRE_MESSAGE(result != nullptr, "Result handle is null");

	ASSERT_IF(csr_wp_result_get_risk_level(result, &a_risk), CSR_ERROR_NONE);
	ASSERT_IF(csr_wp_result_get_detailed_url(result, &a_detailed_url), CSR_ERROR_NONE);
	ASSERT_IF(csr_wp_result_get_user_response(result, &a_response), CSR_ERROR_NONE);

	BOOST_REQUIRE_MESSAGE(risk == a_risk,
		"RISK LEVEL CMP FAIL. EXP=" << risk <<", ACT=" << a_risk);
	ASSERT_STRING(detailed_url, a_detailed_url, "DETAIL URL CMP FAIL.");
	BOOST_REQUIRE_MESSAGE(response == a_response,
		"USER RESPONSE CMP FAIL. EXP=" << response <<", ACT=" << a_response);
}

BOOST_AUTO_TEST_SUITE(API_WEB_PROTECTION)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	(void) c;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_ask_user)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_wp_set_ask_user(context, CSR_WP_ASK_USER), CSR_ERROR_NONE);

	ASSERT_IF(csr_wp_set_ask_user(context, CSR_WP_NOT_ASK_USER), CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_ask_user_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_wp_set_ask_user(nullptr, CSR_WP_ASK_USER), CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(csr_wp_set_ask_user(context, static_cast<csr_wp_ask_user_e>(-1)),
			CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_popup_message)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	const char *msg = "test popup message";

	ASSERT_IF(csr_wp_set_popup_message(context, msg), CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_popup_message_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	const char *msg = "test popup message";

	ASSERT_IF(csr_wp_set_popup_message(nullptr, msg), CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(csr_wp_set_popup_message(context, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url_high)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(result);

	ASSERT_RESULT(result, RISK_HIGH_RISK, RISK_HIGH_DETAILED_URL, CSR_WP_NO_ASK_USER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url_medium)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_MEDIUM_URL, &result), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(result);

	ASSERT_RESULT(result, RISK_MEDIUM_RISK, RISK_MEDIUM_DETAILED_URL, CSR_WP_NO_ASK_USER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url_low)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_LOW_URL, &result), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(result);

	ASSERT_RESULT(result, RISK_LOW_RISK, RISK_LOW_DETAILED_URL, CSR_WP_NO_ASK_USER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url_unverified)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_UNVERIFIED_URL, &result), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(result);

	ASSERT_RESULT(result, RISK_UNVERIFIED_RISK, RISK_UNVERIFIED_DETAILED_URL, CSR_WP_NO_ASK_USER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(nullptr, RISK_HIGH_URL, &result), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_wp_check_url(context, nullptr, &result), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_risk_level)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	csr_wp_risk_level_e risk_level;

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_NONE);

	ASSERT_IF(csr_wp_result_get_risk_level(result, &risk_level), CSR_ERROR_NONE);

	BOOST_REQUIRE_MESSAGE( RISK_HIGH_RISK == risk_level,
		"RISK LEVEL CMP FAIL. EXP=" << RISK_HIGH_RISK <<", ACT=" << risk_level);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_risk_level_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	csr_wp_risk_level_e risk_level;

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_NONE);

	ASSERT_IF(csr_wp_result_get_risk_level(nullptr, &risk_level), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_wp_result_get_risk_level(result, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detailed_url)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	const char *detailed_url;

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_NONE);

	ASSERT_IF(csr_wp_result_get_detailed_url(result, &detailed_url), CSR_ERROR_NONE);
	ASSERT_STRING(RISK_HIGH_DETAILED_URL, detailed_url, "DETAILED URL not same");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detailed_url_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	const char *detailed_url;

	csr_wp_check_result_h result;
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_NONE);

	ASSERT_IF(csr_wp_result_get_detailed_url(nullptr, &detailed_url), CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_wp_result_get_detailed_url(result, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
