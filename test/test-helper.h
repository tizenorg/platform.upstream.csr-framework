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
 * @file        test-helper.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       CSR API test helper
 */
#include <csr/content-screening.h>

void ASSERT_DETECTED(csr_cs_detected_h detected, const char *name, int severity, const char *detailed_url);

void ASSERT_DETECTED_EXT(csr_cs_detected_h detected, time_t time, const char *file_name, bool is_app, const char *pkg_id);

void ASSERT_DETECTED_HANDLE(csr_cs_detected_h expected, csr_cs_detected_h actual);