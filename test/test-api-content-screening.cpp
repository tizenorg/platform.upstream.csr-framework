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

#include <string>
#include <memory>
#include <new>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "test-common.h"

#define TEST_FILE_NORMAL   TEST_DIR "/test_normal_file"
#define TEST_FILE_HIGH     TEST_DIR "/test_malware_file"
#define TEST_FILE_MEDIUM   TEST_DIR "/test_risky_file"
#define TEST_FILE_LOW      TEST_DIR "/test_generic_file"
#define TEST_APP_ROOT      TEST_DIR "/test_app"

#define MALWARE_HIGH_NAME           "test_malware"
#define MALWARE_HIGH_SEVERITY       CSR_CS_SEVERITY_HIGH
#define MALWARE_HIGH_DETAILED_URL   "http://high.malware.com"
#define MALWARE_HIGH_SIGNATURE      "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*"

#define MALWARE_MEDIUM_NAME         "test_risk"
#define MALWARE_MEDIUM_SEVERITY     CSR_CS_SEVERITY_MEDIUM
#define MALWARE_MEDIUM_DETAILED_URL "http://medium.malware.com"
#define MALWARE_MEDIUM_SIGNATURE    "RISKY_MALWARE"

#define MALWARE_LOW_NAME            "test_generic"
#define MALWARE_LOW_SEVERITY        CSR_CS_SEVERITY_LOW
#define MALWARE_LOW_DETAILED_URL    "http://low.malware.com"
#define MALWARE_LOW_SIGNATURE       "GENERIC_MALWARE"



void ASSERT_STRING(const char *expected, const char *actual, const char *msg)
{
	if( expected == actual ) // true including nullptr
		return;

	// null string and empty string are same
	if( (expected == nullptr) && (actual != nullptr) && (strlen(actual) == 0) )
		return;
	if( (actual == nullptr) && (expected != nullptr) && (strlen(expected) == 0) )
		return;

	BOOST_REQUIRE_MESSAGE( (expected != nullptr) && (actual != nullptr) && (strcmp(expected, actual) == 0),
	 	std::string((msg == nullptr) ? "NULL" : msg)
		<< ", EXPECTED=" << std::string((expected == nullptr) ? "NULL" : expected)
		<< ", ACTUAL=" << std::string((actual == nullptr) ? "NULL" : actual) );
}

void ASSERT_DETECTED(csr_cs_detected_h detected, const char* name, int severity, const char* detailed_url)
{
	csr_cs_severity_level_e d_severity;
	const char *d_malware_name;
	const char *d_detailed_url;

	ASSERT_IF(csr_cs_detected_get_severity(detected, &d_severity), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_malware_name(detected, &d_malware_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_detailed_url(detected, &d_detailed_url), CSR_ERROR_NONE);

	BOOST_REQUIRE_MESSAGE(severity == d_severity,
		"EXPECTED=" << severity << ", ACTUAL=" << d_severity);
	ASSERT_STRING(name, d_malware_name, "MALWARE NAME CMP FAIL");
	ASSERT_STRING(detailed_url, d_detailed_url, "DETAILED ULR CMP FAIL");
}

void ASSERT_DETECTED_EXT(csr_cs_detected_h detected, time_t time, const char* file_name, bool is_app, const char* pkg_id)
{
	time_t d_timestamp;
	const char *d_file_name;
	bool d_is_app;
	const char *d_pkg_id;

	ASSERT_IF(csr_cs_detected_get_timestamp(detected, &d_timestamp), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_file_name(detected, &d_file_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_is_app(detected, &d_is_app), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_pkg_id(detected, &d_pkg_id), CSR_ERROR_NONE);

	BOOST_REQUIRE_MESSAGE(time <= d_timestamp,
		"TIMESTAMP CMP FAIL. EXPECTED should be smaller than ACTUAL, EXPECTED="
			<< time << ", ACTUAL=" << d_timestamp);
	ASSERT_STRING(file_name, d_file_name, "NAME CMP FAIL");
	BOOST_REQUIRE_MESSAGE(is_app == d_is_app,
		"IS_APP CMP FAIL. EXPECTED=" << is_app << ", ACTUAL=" << d_is_app);
	ASSERT_STRING(pkg_id, d_pkg_id, "PKGID CMP FAIL");
}

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

	ASSERT_IF(csr_cs_set_popup_message(context, "Test popup message"), CSR_ERROR_NONE);

	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_DEFAULT), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_ALL), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_HALF), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_SINGLE), CSR_ERROR_NONE);

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

	ASSERT_IF(csr_cs_set_popup_message(context, nullptr), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_set_popup_message(context, ""), CSR_ERROR_INVALID_PARAMETER);

	ASSERT_IF(csr_cs_set_core_usage(context,
									static_cast<csr_cs_core_usage_e>(0x882a2)),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_normal)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	unsigned char data[100] = {0, };

	// no malware detected
	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected), CSR_ERROR_NONE);

	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_high)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	unsigned char data[100] = {0, };
	time_t start_time = time(nullptr);

	// severity high detected
	memcpy(data, MALWARE_HIGH_SIGNATURE, strlen(MALWARE_HIGH_SIGNATURE));
	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, nullptr, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_medium)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	unsigned char data[100] = {0, };
	time_t start_time = time(nullptr);

	// severity medium detected
	memcpy(data, MALWARE_MEDIUM_SIGNATURE, strlen(MALWARE_MEDIUM_SIGNATURE));
	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_MEDIUM_NAME, MALWARE_MEDIUM_SEVERITY, MALWARE_MEDIUM_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, nullptr, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_low)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	unsigned char data[100] = {0, };
	time_t start_time = time(nullptr);

	// severity low detected
	memcpy(data, MALWARE_LOW_SIGNATURE, strlen(MALWARE_LOW_SIGNATURE));
	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_LOW_NAME, MALWARE_LOW_SEVERITY, MALWARE_LOW_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, nullptr, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_negative)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	unsigned char data[100] = {0, };

	ASSERT_IF(csr_cs_scan_data(nullptr, data, sizeof(data), &detected), CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(csr_cs_scan_data(context, nullptr, sizeof(data), &detected), CSR_ERROR_INVALID_PARAMETER);

	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), nullptr),  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_normal)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_NORMAL, &detected), CSR_ERROR_NONE);

	// no malware detected
	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_high)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	time_t start_time = time(nullptr);

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY, MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_FILE_HIGH, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_medium)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	time_t start_time = time(nullptr);

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_MEDIUM_NAME, MALWARE_MEDIUM_SEVERITY, MALWARE_MEDIUM_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_FILE_MEDIUM, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_low)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	time_t start_time = time(nullptr);

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_LOW, &detected), CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_LOW_NAME, MALWARE_LOW_SEVERITY, MALWARE_LOW_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_FILE_LOW, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_negative)
{

	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(nullptr, TEST_FILE_NORMAL, &detected), CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(csr_cs_scan_file(context, nullptr, &detected), CSR_ERROR_INVALID_PARAMETER);

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_NORMAL, nullptr), CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected), CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	ASSERT_IF(csr_cs_get_detected_malware(context, TEST_FILE_MEDIUM, &detected),
			  CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malwares)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h detected_list;
	csr_cs_detected_h detected;
	size_t cnt = 0;

	const char *dirs[1] = {
		TEST_DIR
	};

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected), CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);
	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected), CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	ASSERT_IF(csr_cs_get_detected_malwares(context, dirs,
										   sizeof(dirs) / sizeof(const char *),
										   &detected_list, &cnt), CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected_list);

	BOOST_REQUIRE_MESSAGE(cnt >= 2, "Detected malware count is invalid. "
									"expected: >= 2, cnt: " << cnt);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected), CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	ASSERT_IF(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_IGNORE),
			  CSR_ERROR_NONE);

	ASSERT_IF(csr_cs_get_ignored_malware(context, TEST_FILE_HIGH, &detected),
			  CSR_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malwares)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h ignored_list;
	csr_cs_detected_h detected;
	size_t cnt = 0;

	const char *dirs[1] = {
		TEST_DIR
	};

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected),
			  CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);
	ASSERT_IF(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_IGNORE),
			  CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected),
			  CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);
	ASSERT_IF(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_IGNORE),
			  CSR_ERROR_NONE);

	ASSERT_IF(csr_cs_get_ignored_malwares(context, dirs,
										  sizeof(dirs) / sizeof(const char *),
										  &ignored_list, &cnt), CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(ignored_list);
	ASSERT_IF(cnt, static_cast<size_t>(2));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected), CSR_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);
	ASSERT_IF(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_UNIGNORE),
			  CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
