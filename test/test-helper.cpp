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
 * @file        test-helper.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       CSR API test helper
 */
#include <string>

#include <time.h>
#include <utime.h>
#include <unistd.h>

#include <boost/test/unit_test.hpp>

#include "test-common.h"

void ASSERT_DETECTED(csr_cs_detected_h detected, const char *e_malware_name,
					 int e_severity, const char *e_detailed_url)
{
	csr_cs_severity_level_e a_severity;
	const char *a_malware_name;
	const char *a_detailed_url;

	ASSERT_IF(csr_cs_detected_get_severity(detected, &a_severity), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_malware_name(detected, &a_malware_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_detailed_url(detected, &a_detailed_url), CSR_ERROR_NONE);

	ASSERT_IF(a_severity, e_severity);
	ASSERT_IF(a_malware_name, e_malware_name);
	ASSERT_IF(a_detailed_url, e_detailed_url);
}

void ASSERT_DETECTED_EXT(csr_cs_detected_h detected, time_t e_timestamp,
						 const char *e_file_name, bool e_is_app, const char *e_pkg_id)
{
	time_t a_timestamp;
	const char *a_file_name;
	bool a_is_app;
	const char *a_pkg_id;

	ASSERT_IF(csr_cs_detected_get_timestamp(detected, &a_timestamp), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_file_name(detected, &a_file_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_is_app(detected, &a_is_app), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_pkg_id(detected, &a_pkg_id), CSR_ERROR_NONE);

	ASSERT_IF(a_file_name, e_file_name);
	ASSERT_IF(a_is_app, e_is_app);
	ASSERT_IF(a_pkg_id, e_pkg_id);

	BOOST_WARN_MESSAGE(e_timestamp <= a_timestamp,
					   "Actual detected item's time stamp is later than expected time "
					   "stamp (which is start time before scan_file maybe..). this case "
					   "should be the returned detected item comes from history which is "
					   "scanned in the past. actual time: " << a_timestamp <<
					   " expected(started) time: " << e_timestamp);
}

void ASSERT_DETECTED_HANDLE(csr_cs_detected_h expected, csr_cs_detected_h actual)
{
	csr_cs_severity_level_e e_severity, a_severity;
	const char *e_malware_name, *a_malware_name;
	const char *e_detailed_url, *a_detailed_url;
	time_t e_timestamp, a_timestamp;
	const char *e_file_name, *a_file_name;
	bool e_is_app, a_is_app;
	const char *e_pkg_id, *a_pkg_id;

	ASSERT_IF(csr_cs_detected_get_severity(expected, &e_severity), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_malware_name(expected, &e_malware_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_detailed_url(expected, &e_detailed_url), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_timestamp(expected, &e_timestamp), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_file_name(expected, &e_file_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_is_app(expected, &e_is_app), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_pkg_id(expected, &e_pkg_id), CSR_ERROR_NONE);

	ASSERT_IF(csr_cs_detected_get_severity(actual, &a_severity), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_malware_name(actual, &a_malware_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_detailed_url(actual, &a_detailed_url), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_timestamp(actual, &a_timestamp), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_file_name(actual, &a_file_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_is_app(actual, &a_is_app), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_pkg_id(actual, &a_pkg_id), CSR_ERROR_NONE);

	ASSERT_IF(a_severity, e_severity);
	ASSERT_IF(a_malware_name, e_malware_name);
	ASSERT_IF(a_detailed_url, e_detailed_url);
	ASSERT_IF(a_file_name, e_file_name);
	ASSERT_IF(a_is_app, e_is_app);
	ASSERT_IF(a_pkg_id, e_pkg_id);
	ASSERT_IF(a_timestamp, e_timestamp);
}
