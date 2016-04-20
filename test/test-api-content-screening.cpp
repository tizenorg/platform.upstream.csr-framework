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
#include <boost/test/unit_test.hpp>

#include "test-common.h"

BOOST_AUTO_TEST_SUITE(API_CONTENT_SCREENING)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	(void) c;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_values_to_context_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_ask_user(context, CSR_CS_NOT_ASK_USER), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_ask_user(context, CSR_CS_ASK_USER), CSR_ERROR_NONE);

	ASSERT_IF(csr_cs_set_popup_message(context, "Test popup message"),
			  CSR_ERROR_NONE);

	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_DEFAULT),
			  CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_ALL), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_HALF), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_SINGLE),
			  CSR_ERROR_NONE);

	ASSERT_IF(csr_cs_set_scan_on_cloud(context), CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_values_to_context_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_IF(csr_cs_set_ask_user(context, static_cast<csr_cs_ask_user_e>(0x926ce)),
			  CSR_ERROR_INVALID_PARAMETER);

	ASSERT_IF(csr_cs_set_popup_message(context, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_popup_message(context, ""), CSR_ERROR_INVALID_PARAMETER);

	ASSERT_IF(csr_cs_set_core_usage(context,
									static_cast<csr_cs_core_usage_e>(0x882a2)),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	unsigned char data[100] = {0, };

	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected),
			  CSR_ERROR_NONE);

	// no malware detected
	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(context, "dummy_file_path", &detected),
			  CSR_ERROR_NONE);

	// no malware detected
	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_get_detected_malware(context, "dummy_file_path", &detected),
			  CSR_ERROR_NONE);

	// no malware detected
	CHECK_IS_NOT_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malwares)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h detected_list;
	size_t cnt = 0;

	ASSERT_IF(csr_cs_get_detected_malwares(context, "dummy_dir_path", &detected_list, &cnt),
			  CSR_ERROR_NONE);

	// no malware detected
	CHECK_IS_NOT_NULL(detected_list);

	ASSERT_IF(cnt, static_cast<size_t>(1));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h ignored;

	ASSERT_IF(csr_cs_get_ignored_malware(context, "dummy_file_path", &ignored),
			  CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(ignored);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malwares)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h ignored_list;
	size_t cnt = 0;

	ASSERT_IF(csr_cs_get_ignored_malwares(context, "dummy_dir_path", &ignored_list, &cnt),
			  CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(ignored_list);

	ASSERT_IF(cnt, static_cast<size_t>(1));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_get_detected_malware(context, "dummy_file_path", &detected),
			  CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	ASSERT_IF(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE),
			  CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
