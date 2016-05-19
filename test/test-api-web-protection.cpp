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


void ASSERT_RESULT(csr_wp_check_result_h result,
	csr_wp_risk_level_e risk, const char * detailed_url, csr_wp_user_response_e response)
{
	csr_wp_risk_level_e a_risk;
	const char *a_detailed_url;
	csr_wp_user_response_e a_response;

	CHECK_IS_NOT_NULL(result);

	ASSERT_SUCCESS(csr_wp_result_get_risk_level(result, &a_risk));
	ASSERT_SUCCESS(csr_wp_result_get_detailed_url(result, &a_detailed_url));
	ASSERT_SUCCESS(csr_wp_result_get_user_response(result, &a_response));

	ASSERT_IF(a_risk, risk);
	ASSERT_IF(a_detailed_url, detailed_url);
	ASSERT_IF(a_response, response);
}

BOOST_AUTO_TEST_SUITE(API_WEB_PROTECTION)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Test::Context<csr_wp_context_h>();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_ask_user)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();

	ASSERT_SUCCESS(csr_wp_set_ask_user(context, CSR_WP_ASK_USER));
	ASSERT_SUCCESS(csr_wp_set_ask_user(context, CSR_WP_NOT_ASK_USER));

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

	ASSERT_SUCCESS(csr_wp_set_popup_message(context, msg));

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
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_HIGH_URL, &result));

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
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_MEDIUM_URL, &result));

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
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_LOW_URL, &result));

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
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_UNVERIFIED_URL, &result));

	CHECK_IS_NOT_NULL(result);

	ASSERT_RESULT(result, RISK_UNVERIFIED_RISK, RISK_UNVERIFIED_DETAILED_URL,
				  CSR_WP_NO_ASK_USER);

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
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_risk_level)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	csr_wp_risk_level_e risk_level;

	csr_wp_check_result_h result;
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_HIGH_URL, &result));
	ASSERT_SUCCESS(csr_wp_result_get_risk_level(result, &risk_level));

	ASSERT_IF(risk_level, RISK_HIGH_RISK);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_risk_level_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	csr_wp_risk_level_e risk_level;

	csr_wp_check_result_h result;
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_HIGH_URL, &result));

	ASSERT_IF(csr_wp_result_get_risk_level(result, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_wp_result_get_risk_level(nullptr, &risk_level),
			  CSR_ERROR_INVALID_HANDLE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detailed_url)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	const char *detailed_url;

	csr_wp_check_result_h result;
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_HIGH_URL, &result));

	ASSERT_SUCCESS(csr_wp_result_get_detailed_url(result, &detailed_url));
	ASSERT_IF(detailed_url, RISK_HIGH_DETAILED_URL);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detailed_url_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	const char *detailed_url;

	csr_wp_check_result_h result;
	ASSERT_SUCCESS(csr_wp_check_url(context, RISK_HIGH_URL, &result));

	ASSERT_IF(csr_wp_result_get_detailed_url(nullptr, &detailed_url),
			  CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_wp_result_get_detailed_url(result, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
