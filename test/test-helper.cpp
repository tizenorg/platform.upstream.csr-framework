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

void ASSERT_DETECTED(csr_cs_malware_h detected, const char *e_malware_name,
					 csr_cs_severity_level_e e_severity, const char *e_detailed_url)
{
	csr_cs_severity_level_e a_severity;
	Test::ScopedCstr a_malware_name, a_detailed_url;

	ASSERT_SUCCESS(csr_cs_malware_get_severity(detected, &a_severity));
	ASSERT_SUCCESS(csr_cs_malware_get_name(detected, &a_malware_name.ptr));
	ASSERT_SUCCESS(csr_cs_malware_get_detailed_url(detected, &a_detailed_url.ptr));

	ASSERT_IF(a_severity, e_severity);
	ASSERT_IF(a_malware_name.ptr, e_malware_name);
	ASSERT_IF(a_detailed_url.ptr, e_detailed_url);
}

void ASSERT_DETECTED_EXT(csr_cs_malware_h detected, time_t e_timestamp,
						 const char *e_file_name, bool e_is_app, const char *e_pkg_id)
{
	time_t a_timestamp;
	bool a_is_app;
	Test::ScopedCstr a_file_name, a_pkg_id;

	ASSERT_SUCCESS(csr_cs_malware_get_timestamp(detected, &a_timestamp));
	ASSERT_SUCCESS(csr_cs_malware_get_file_name(detected, &a_file_name.ptr));
	ASSERT_SUCCESS(csr_cs_malware_is_app(detected, &a_is_app));
	ASSERT_SUCCESS(csr_cs_malware_get_pkg_id(detected, &a_pkg_id.ptr));

	ASSERT_IF(a_file_name.ptr, e_file_name);
	ASSERT_IF(a_is_app, e_is_app);
	ASSERT_IF(a_pkg_id.ptr, e_pkg_id);

	BOOST_WARN_MESSAGE(e_timestamp <= a_timestamp,
					   "Actual detected item's time stamp is later than expected time "
					   "stamp (which is start time before scan_file maybe..). this case "
					   "should be the returned detected item comes from history which is "
					   "scanned in the past. actual time: " << a_timestamp <<
					   " expected(started) time: " << e_timestamp);
}

void ASSERT_DETECTED_HANDLE(csr_cs_malware_h expected, csr_cs_malware_h actual)
{
	csr_cs_severity_level_e e_severity, a_severity;
	Test::ScopedCstr e_malware_name, e_detailed_url, e_file_name, e_pkg_id;
	Test::ScopedCstr a_malware_name, a_detailed_url, a_file_name, a_pkg_id;
	time_t e_timestamp, a_timestamp;
	bool e_is_app, a_is_app;

	ASSERT_SUCCESS(csr_cs_malware_get_severity(expected, &e_severity));
	ASSERT_SUCCESS(csr_cs_malware_get_name(expected, &e_malware_name.ptr));
	ASSERT_SUCCESS(csr_cs_malware_get_detailed_url(expected, &e_detailed_url.ptr));
	ASSERT_SUCCESS(csr_cs_malware_get_timestamp(expected, &e_timestamp));
	ASSERT_SUCCESS(csr_cs_malware_get_file_name(expected, &e_file_name.ptr));
	ASSERT_SUCCESS(csr_cs_malware_is_app(expected, &e_is_app));
	ASSERT_SUCCESS(csr_cs_malware_get_pkg_id(expected, &e_pkg_id.ptr));

	ASSERT_SUCCESS(csr_cs_malware_get_severity(actual, &a_severity));
	ASSERT_SUCCESS(csr_cs_malware_get_name(actual, &a_malware_name.ptr));
	ASSERT_SUCCESS(csr_cs_malware_get_detailed_url(actual, &a_detailed_url.ptr));
	ASSERT_SUCCESS(csr_cs_malware_get_timestamp(actual, &a_timestamp));
	ASSERT_SUCCESS(csr_cs_malware_get_file_name(actual, &a_file_name.ptr));
	ASSERT_SUCCESS(csr_cs_malware_is_app(actual, &a_is_app));
	ASSERT_SUCCESS(csr_cs_malware_get_pkg_id(actual, &a_pkg_id.ptr));

	ASSERT_IF(a_severity, e_severity);
	ASSERT_IF(a_malware_name.ptr, e_malware_name.ptr);
	ASSERT_IF(a_detailed_url.ptr, e_detailed_url.ptr);
	ASSERT_IF(a_file_name.ptr, e_file_name.ptr);
	ASSERT_IF(a_is_app, e_is_app);
	ASSERT_IF(a_pkg_id.ptr, e_pkg_id.ptr);
	ASSERT_IF(a_timestamp, e_timestamp);
}

void ASSERT_DETECTED_IN_LIST(const std::vector<csr_cs_malware_h> &detectedList,
							 const char *file_name, const char *name,
							 csr_cs_severity_level_e severity, const char *detailed_url)
{
	csr_cs_severity_level_e a_severity;
	Test::ScopedCstr a_file_name, a_name, a_detailed_url;

	std::vector<csr_cs_malware_h>::iterator iter;
	for (auto &d : detectedList) {
		ASSERT_IF(csr_cs_malware_get_file_name(d, &a_file_name.ptr), CSR_ERROR_NONE);
		if (strcmp(file_name, a_file_name.ptr) != 0)
			continue;

		ASSERT_SUCCESS(csr_cs_malware_get_name(d, &a_name.ptr));
		ASSERT_SUCCESS(csr_cs_malware_get_severity(d, &a_severity));
		ASSERT_SUCCESS(csr_cs_malware_get_detailed_url(d, &a_detailed_url.ptr));

		ASSERT_IF(name, a_name.ptr);
		ASSERT_IF(severity, a_severity);
		ASSERT_IF(detailed_url, a_detailed_url.ptr);

		return;
	}

	BOOST_REQUIRE_MESSAGE(false,
			"Cannot find the file[" << file_name << "] in detected list.");
}

void ASSERT_DETECTED_IN_LIST_EXT(const std::vector<csr_cs_malware_h> &detectedList,
								 const char *file_name, bool is_app, const char *pkg_id)
{
	bool a_is_app;
	Test::ScopedCstr a_file_name, a_pkg_id;

	for (auto &d : detectedList) {
		ASSERT_SUCCESS(csr_cs_malware_get_file_name(d, &a_file_name.ptr));
		if (strcmp(file_name, a_file_name.ptr) != 0)
			continue;

		ASSERT_SUCCESS(csr_cs_malware_is_app(d, &a_is_app));
		ASSERT_SUCCESS(csr_cs_malware_get_pkg_id(d, &a_pkg_id.ptr));

		ASSERT_IF(is_app, a_is_app);
		ASSERT_IF(pkg_id, a_pkg_id.ptr);

		return;
	}

	BOOST_REQUIRE_MESSAGE(false,
			"Cannot find the file[" << file_name << "] in detected list.");
}
