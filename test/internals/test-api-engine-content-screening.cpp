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
 * @file        test-api-engine-content-screening.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Content screening Engine API test
 */
#include <csre-content-screening.h>
#include <csre-content-screening-engine-info.h>

#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/test/unit_test.hpp>

#include "test-common.h"

#define TEST_FILE_NORMAL   TEST_DIR "/test_normal_file"
#define TEST_FILE_MALWARE  TEST_DIR "/test_malware_file"
#define TEST_FILE_RISKY    TEST_DIR "/test_risky_file"
#define TEST_APP_ROOT      TEST_DIR "/test_app"

namespace {

inline void checkDetected(csre_cs_detected_h detected,
						  csre_cs_severity_level_e expected_severity,
						  const char *expected_malware_name,
						  const char *expected_detailed_url)
{
	EXCEPTION_GUARD_START

	CHECK_IS_NOT_NULL(detected);

	csre_cs_severity_level_e severity;
	ASSERT_IF(csre_cs_detected_get_severity(detected, &severity), CSRE_ERROR_NONE);
	BOOST_REQUIRE_MESSAGE(severity == expected_severity,
						  "severity isn't expected value. "
						  "val: " << severity << " expected: " << expected_severity);

	const char *malware_name = nullptr;
	ASSERT_IF(csre_cs_detected_get_malware_name(detected, &malware_name),
			  CSRE_ERROR_NONE);

	if (expected_malware_name != nullptr) {
		CHECK_IS_NOT_NULL(malware_name);
		BOOST_REQUIRE_MESSAGE(
			(strlen(malware_name) == strlen(expected_malware_name) &&
			 memcmp(malware_name, expected_malware_name, strlen(malware_name)) == 0),
			"malware_name isn't expected value. "
			"val: " << malware_name << " expected: " << expected_malware_name);
	}

	const char *detailed_url = nullptr;
	ASSERT_IF(csre_cs_detected_get_detailed_url(detected, &detailed_url),
			  CSRE_ERROR_NONE);

	if (expected_detailed_url != nullptr) {
		CHECK_IS_NOT_NULL(detailed_url);
		BOOST_REQUIRE_MESSAGE(
			(strlen(detailed_url) == strlen(expected_detailed_url) &&
			 memcmp(detailed_url, expected_detailed_url, strlen(detailed_url)) == 0),
			"detailed_url isn't expected value. "
			"val: " << detailed_url << " expected: " << expected_detailed_url);
	}

	EXCEPTION_GUARD_END
}

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(API_ENGINE_CONTENT_SCREENING)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Test::Context<csre_cs_context_h>();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_clear)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_context_h>();
	auto context = c.get();

	const char *data =
		"abcd1234dfdfdf334dfdi8ffndsfdfdsfdasfagdfvdfdfafadfasdfsdfe";

	csre_cs_detected_h detected;
	ASSERT_IF(csre_cs_scan_data(
				  context,
				  reinterpret_cast<const unsigned char *>(data),
				  strlen(data),
				  &detected), CSRE_ERROR_NONE);

	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_high)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_context_h>();
	auto context = c.get();

	const char *data =
		"aabbccX5O!P%@AP[4\\PZX54(P^)7CC)7}$"
		"EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*112233";

	csre_cs_detected_h detected;
	ASSERT_IF(csre_cs_scan_data(
				  context,
				  reinterpret_cast<const unsigned char *>(data),
				  strlen(data),
				  &detected), CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected, CSRE_CS_SEVERITY_HIGH, "test_malware",
				  "http://high.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_medium)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_context_h>();
	auto context = c.get();

	const char *data = "aabbccRISKY_MALWARE112233";

	csre_cs_detected_h detected;
	ASSERT_IF(csre_cs_scan_data(
				  context,
				  reinterpret_cast<const unsigned char *>(data),
				  strlen(data),
				  &detected), CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected, CSRE_CS_SEVERITY_MEDIUM, "test_risk", nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_normal)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_context_h>();
	auto context = c.get();

	csre_cs_detected_h detected;
	ASSERT_IF(csre_cs_scan_file(context, TEST_FILE_NORMAL, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_context_h>();
	auto context = c.get();

	csre_cs_detected_h detected;
	ASSERT_IF(csre_cs_scan_file(context, TEST_FILE_MALWARE, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected, CSRE_CS_SEVERITY_HIGH, "test_malware",
				  "http://high.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_risky)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_context_h>();
	auto context = c.get();

	csre_cs_detected_h detected;
	ASSERT_IF(csre_cs_scan_file(context, TEST_FILE_RISKY, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected, CSRE_CS_SEVERITY_MEDIUM, "test_risk",
				  "http://medium.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_app_on_cloud)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_context_h>();
	auto context = c.get();

	csre_cs_detected_h detected;
	ASSERT_IF(csre_cs_scan_app_on_cloud(context, TEST_APP_ROOT, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected, CSRE_CS_SEVERITY_HIGH, "test_malware",
				  "http://high.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_CONTENT_SCREENING_ERR_STRING)

BOOST_AUTO_TEST_CASE(positive)
{
	EXCEPTION_GUARD_START

	const char *string = nullptr;

	ASSERT_IF(csre_cs_get_error_string(CSRE_ERROR_UNKNOWN, &string),
			  CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(string);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_CONTENT_SCREENING_ENGINE_INFO)

BOOST_AUTO_TEST_CASE(get_engine_info)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	(void) c;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	auto handle = c.get();

	const char *vendor = nullptr;
	ASSERT_IF(csre_cs_engine_get_vendor(handle, &vendor), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor_logo)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	auto handle = c.get();

	unsigned char *vendor_logo_image = nullptr;
	unsigned int size = 0;
	ASSERT_IF(csre_cs_engine_get_vendor_logo(handle, &vendor_logo_image, &size),
			  CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_version)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	auto handle = c.get();

	const char *version = nullptr;
	ASSERT_IF(csre_cs_engine_get_version(handle, &version), CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_data_version)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	auto handle = c.get();

	const char *version = nullptr;
	ASSERT_IF(csre_cs_engine_get_data_version(handle, &version), CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_latest_update_time)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	auto handle = c.get();

	time_t time = 0;
	ASSERT_IF(csre_cs_engine_get_latest_update_time(handle, &time),
			  CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_engine_activated)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	auto handle = c.get();

	csre_cs_activated_e activated;
	ASSERT_IF(csre_cs_engine_get_activated(handle, &activated), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_api_version)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csre_cs_engine_h>();
	auto handle = c.get();

	const char *version = nullptr;
	ASSERT_IF(csre_cs_engine_get_api_version(handle, &version), CSRE_ERROR_NONE);
	ASSERT_IF(memcmp(version, CSRE_CS_API_VERSION, strlen(version)), 0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
