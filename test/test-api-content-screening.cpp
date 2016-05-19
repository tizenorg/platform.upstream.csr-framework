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
#include <fstream>
#include <boost/test/unit_test.hpp>

#include "test-common.h"
#include "test-helper.h"


BOOST_AUTO_TEST_SUITE(API_CONTENT_SCREENING)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Test::Context<csr_cs_context_h>();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_values_to_context_positive)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();

	ASSERT_SUCCESS(csr_cs_set_ask_user(context, CSR_CS_NOT_ASK_USER));
	ASSERT_SUCCESS(csr_cs_set_ask_user(context, CSR_CS_ASK_USER));

	ASSERT_SUCCESS(csr_cs_set_popup_message(context, "Test popup message"));

	ASSERT_SUCCESS(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_DEFAULT));
	ASSERT_SUCCESS(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_ALL));
	ASSERT_SUCCESS(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_HALF));
	ASSERT_SUCCESS(csr_cs_set_core_usage(context, CSR_CS_USE_CORE_SINGLE));

	ASSERT_SUCCESS(csr_cs_set_scan_on_cloud(context));

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
	ASSERT_SUCCESS(csr_cs_scan_data(context, data, sizeof(data), &detected));

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
	ASSERT_SUCCESS(csr_cs_scan_data(context, data, sizeof(data), &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
					MALWARE_HIGH_DETAILED_URL);
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
	ASSERT_SUCCESS(csr_cs_scan_data(context, data, sizeof(data), &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_MEDIUM_NAME, MALWARE_MEDIUM_SEVERITY,
					MALWARE_MEDIUM_DETAILED_URL);
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
	ASSERT_SUCCESS(csr_cs_scan_data(context, data, sizeof(data), &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_LOW_NAME, MALWARE_LOW_SEVERITY,
					MALWARE_LOW_DETAILED_URL);
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

	ASSERT_IF(csr_cs_scan_data(nullptr, data, sizeof(data), &detected),
			  CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(csr_cs_scan_data(context, nullptr, sizeof(data), &detected),
			  CSR_ERROR_INVALID_PARAMETER);

	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_normal)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_NORMAL, &detected));

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

	Test::touch_file(TEST_FILE_HIGH);

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
					MALWARE_HIGH_DETAILED_URL);
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

	Test::touch_file(TEST_FILE_MEDIUM);

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_MEDIUM_NAME, MALWARE_MEDIUM_SEVERITY,
					MALWARE_MEDIUM_DETAILED_URL);
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

	Test::touch_file(TEST_FILE_LOW);

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_LOW, &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_LOW_NAME, MALWARE_LOW_SEVERITY,
					MALWARE_LOW_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_FILE_LOW, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_media_dir)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	time_t start_time = time(nullptr);

	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
					MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_FILE_MEDIA, false, nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_tmp_dir)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	time_t start_time = time(nullptr);

	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_TMP);

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_TMP, &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
					MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_FILE_TMP, false, nullptr);

	EXCEPTION_GUARD_END
}

// TODO : Add TC for different directories
BOOST_AUTO_TEST_CASE(scan_file_wgt_dir)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	time_t start_time = time(nullptr);

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_WGT_MAL_FILE, &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
					MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_WGT_APP_ROOT, true, TEST_WGT_PKG_ID);

	ASSERT_UNINSTALL_APP(TEST_WGT_PKG_ID);

	EXCEPTION_GUARD_END
}

// TODO : Add TC for different directories
BOOST_AUTO_TEST_CASE(scan_file_tpk_dir)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	time_t start_time = time(nullptr);

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_TPK_MAL_FILE, &detected));

	CHECK_IS_NOT_NULL(detected);
	ASSERT_DETECTED(detected, MALWARE_HIGH_NAME, MALWARE_HIGH_SEVERITY,
					MALWARE_HIGH_DETAILED_URL);
	ASSERT_DETECTED_EXT(detected, start_time, TEST_TPK_APP_ROOT, true, TEST_TPK_PKG_ID);

	ASSERT_UNINSTALL_APP(TEST_TPK_PKG_ID);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_invalid_params)
{

	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(nullptr, TEST_FILE_NORMAL, &detected),
			  CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_scan_file(context, nullptr, &detected), CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_NORMAL, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_not_existing_file)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_NO_EXIST, &detected),
			  CSR_ERROR_FILE_DO_NOT_EXIST);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	csr_cs_detected_h stored;

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected));
	CHECK_IS_NOT_NULL(detected);

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIUM, &stored));
	CHECK_IS_NOT_NULL(stored);

	ASSERT_DETECTED_HANDLE(detected, stored);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malware_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h stored;

	ASSERT_IF(csr_cs_get_detected_malware(nullptr, TEST_FILE_MEDIUM, &stored),
			  CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_get_detected_malware(context, nullptr, &stored),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_get_detected_malware(context, TEST_FILE_MEDIUM, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END;
}

BOOST_AUTO_TEST_CASE(get_detected_malware_not_existing_file)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h stored;

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_NO_EXIST, &stored));
	CHECK_IS_NULL(stored);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malwares)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h detected_list;
	csr_cs_detected_h detected_high, detected_medium;
	csr_cs_detected_h stored;
	const char *file_path_actual;
	size_t cnt = 0;
	size_t idx = 0;
	size_t compared_cnt = 0;

	const char *dirs[1] = {
		TEST_DIR
	};

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected_high));
	CHECK_IS_NOT_NULL(detected_high);
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected_medium));
	CHECK_IS_NOT_NULL(detected_medium);

	ASSERT_SUCCESS(csr_cs_get_detected_malwares(context, dirs,
												sizeof(dirs) / sizeof(const char *),
												&detected_list, &cnt));
	CHECK_IS_NOT_NULL(detected_list);

	for (idx = 0; idx < cnt; idx++) {
		ASSERT_SUCCESS(csr_cs_dlist_get_detected(detected_list, idx, &stored));
		CHECK_IS_NOT_NULL(stored);
		ASSERT_SUCCESS(csr_cs_detected_get_file_name(stored, &file_path_actual));
		if (strcmp(TEST_FILE_HIGH, file_path_actual) == 0) {
			ASSERT_DETECTED_HANDLE(detected_high, stored);
			compared_cnt++;
		} else if (strcmp(TEST_FILE_MEDIUM, file_path_actual) == 0) {
			ASSERT_DETECTED_HANDLE(detected_medium, stored);
			compared_cnt++;
		} else {
			continue;
		}
	}

	BOOST_REQUIRE_MESSAGE(compared_cnt == 2, "Detected malware count is invalid. "
						  "expected: == 2, cnt: " << compared_cnt);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malwares_invalid_params)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h detected_list;
	size_t cnt = 0;

	const char *dirs[1] = {
		TEST_DIR
	};

	ASSERT_IF(csr_cs_get_detected_malwares(nullptr, dirs,
										   sizeof(dirs) / sizeof(const char *),
										   &detected_list, &cnt),
			  CSR_ERROR_INVALID_HANDLE);

	ASSERT_IF(csr_cs_get_detected_malwares(context, nullptr,
										   sizeof(dirs) / sizeof(const char *),
										   &detected_list, &cnt),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_get_detected_malwares(context, dirs,
										   sizeof(dirs) / sizeof(const char *),
										   nullptr, &cnt),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_get_detected_malwares(context, dirs,
										   sizeof(dirs) / sizeof(const char *),
										   &detected_list, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_detected_malwares_not_existing_dir)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h detected_list;
	size_t cnt = 0;

	const char *dirs[1] = {
		TEST_FILE_NO_EXIST
	};

	ASSERT_SUCCESS(csr_cs_get_detected_malwares(context, dirs,
												sizeof(dirs) / sizeof(const char *),
												&detected_list, &cnt));

	ASSERT_IF(cnt, static_cast<size_t>(0));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected, stored;

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected));
	CHECK_IS_NOT_NULL(detected);

	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_IGNORE));

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_HIGH, &stored));

	CHECK_IS_NOT_NULL(stored);

	ASSERT_DETECTED_HANDLE(detected, stored);

	// rollback to unignore
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_UNIGNORE));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malware_invalid_param)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h stored;

	ASSERT_IF(csr_cs_get_ignored_malware(nullptr, TEST_FILE_MEDIUM, &stored),
			  CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_get_ignored_malware(context, nullptr, &stored),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIUM, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malware_not_existing_file)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h stored;

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_NO_EXIST, &stored));
	CHECK_IS_NULL(stored);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malwares)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h ignored_list;
	csr_cs_detected_h detected_high, detected_medium, stored;
	size_t cnt = 0;
	size_t idx = 0;
	size_t compared_cnt = 0;
	const char *file_path_actual;

	const char *dirs[1] = {
		TEST_DIR
	};

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected_high));
	CHECK_IS_NOT_NULL(detected_high);
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected_high,
												 CSR_CS_ACTION_IGNORE));

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIUM, &detected_medium));
	CHECK_IS_NOT_NULL(detected_medium);
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected_medium,
												 CSR_CS_ACTION_IGNORE));

	ASSERT_SUCCESS(csr_cs_get_ignored_malwares(context, dirs,
												sizeof(dirs) / sizeof(const char *),
												&ignored_list, &cnt));
	CHECK_IS_NOT_NULL(ignored_list);

	for (idx = 0; idx < cnt; idx++) {
		ASSERT_SUCCESS(csr_cs_dlist_get_detected(ignored_list, idx, &stored));
		CHECK_IS_NOT_NULL(stored);
		ASSERT_SUCCESS(csr_cs_detected_get_file_name(stored, &file_path_actual));
		if (strcmp(TEST_FILE_HIGH, file_path_actual) == 0) {
			ASSERT_DETECTED_HANDLE(detected_high, stored);
			compared_cnt++;
		} else if (strcmp(TEST_FILE_MEDIUM, file_path_actual) == 0) {
			ASSERT_DETECTED_HANDLE(detected_medium, stored);
			compared_cnt++;
		} else {
			continue;
		}
	}

	ASSERT_IF(compared_cnt, static_cast<size_t>(2));

	// rollback to unignore
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected_high,
												 CSR_CS_ACTION_UNIGNORE));
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected_medium,
												 CSR_CS_ACTION_UNIGNORE));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malwares_invalid_params)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h ignored_list;
	size_t cnt = 0;

	const char *dirs[1] = {
		TEST_DIR
	};

	ASSERT_IF(csr_cs_get_ignored_malwares(nullptr, dirs,
										  sizeof(dirs) / sizeof(const char *),
										  &ignored_list, &cnt),
			  CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_get_ignored_malwares(context, nullptr,
										  sizeof(dirs) / sizeof(const char *),
										  &ignored_list, &cnt),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_get_ignored_malwares(context, dirs,
										  sizeof(dirs) / sizeof(const char *),
										  nullptr, &cnt),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_get_ignored_malwares(context, dirs,
										  sizeof(dirs) / sizeof(const char *),
										  &ignored_list, nullptr),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_ignored_malwares_not_existing_dir)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_list_h ignored_list;
	size_t cnt = 0;

	const char *dirs[1] = {
		TEST_FILE_NO_EXIST
	};

	ASSERT_SUCCESS(csr_cs_get_ignored_malwares(context, dirs,
											   sizeof(dirs) / sizeof(const char *),
											   &ignored_list, &cnt));

	ASSERT_IF(cnt, static_cast<size_t>(0));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected, stored;

	// remove earlier test results
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE));
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);

	// store detected malware
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	CHECK_IS_NOT_NULL(detected);

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NOT_NULL(stored);

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	// IGNORE
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_IGNORE));

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NOT_NULL(stored);

	// UNIGNORE
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_UNIGNORE));

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NOT_NULL(stored);

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	// REMOVE
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE));

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware_ignored_rescan)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected, stored;

	// remove earlier test results
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE);
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);

	// store detected malware
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	CHECK_IS_NOT_NULL(detected);

	// ignore
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_IGNORE));

	// rescan
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	CHECK_IS_NULL(detected);

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NOT_NULL(stored);

	// rollback to unignore
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, stored, CSR_CS_ACTION_UNIGNORE));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware_remove_file)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected, stored;

	// remove earlier test results
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE);
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);

	// store detected malware
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	CHECK_IS_NOT_NULL(detected);

	// remove detected malware file
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE));

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	ASSERT_IF(Test::is_file_exist(TEST_FILE_MEDIA), false);

	// make ignored
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_IGNORE));

	// remove ignored malware file
	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE));

	ASSERT_SUCCESS(csr_cs_get_detected_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	ASSERT_SUCCESS(csr_cs_get_ignored_malware(context, TEST_FILE_MEDIA, &stored));
	CHECK_IS_NULL(stored);

	ASSERT_IF(Test::is_file_exist(TEST_FILE_MEDIA), false);

	// remove not existing file
	Test::copy_file(TEST_FILE_HIGH, TEST_FILE_MEDIA);
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_MEDIA, &detected));
	CHECK_IS_NOT_NULL(detected);

	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE));

	ASSERT_IF(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE),
			  CSR_ERROR_FILE_DO_NOT_EXIST);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware_remove_wgt)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	// remove old test data
	Test::uninstall_app(TEST_WGT_PKG_ID);

	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	// remove detected malware file
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_WGT_MAL_FILE, &detected));
	CHECK_IS_NOT_NULL(detected);

	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE));

	ASSERT_IF(Test::is_app_exist(TEST_WGT_PKG_ID), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware_remove_tpk)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	// remove old test data
	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	// remove detected malware file
	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_TPK_MAL_FILE, &detected));
	CHECK_IS_NOT_NULL(detected);

	ASSERT_SUCCESS(csr_cs_judge_detected_malware(context, detected, CSR_CS_ACTION_REMOVE));

	ASSERT_IF(Test::is_app_exist(TEST_TPK_PKG_ID), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(judge_detected_malware_invalid_params)
{
	EXCEPTION_GUARD_START

	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;

	ASSERT_SUCCESS(csr_cs_scan_file(context, TEST_FILE_HIGH, &detected));
	CHECK_IS_NOT_NULL(detected);

	ASSERT_IF(csr_cs_judge_detected_malware(nullptr, detected, CSR_CS_ACTION_REMOVE),
			  CSR_ERROR_INVALID_HANDLE);
	ASSERT_IF(csr_cs_judge_detected_malware(context, nullptr, CSR_CS_ACTION_REMOVE),
			  CSR_ERROR_INVALID_PARAMETER);
	ASSERT_IF(csr_cs_judge_detected_malware(context, detected,
											static_cast<csr_cs_action_e>(-1)),
			  CSR_ERROR_INVALID_PARAMETER);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
