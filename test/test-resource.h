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
 * @file        test-resource.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Test resoure name and paths
 */
#pragma once

#include <string>

// Test data for content screening
#define TEST_FILE_NORMAL   TEST_DIR "/test_normal_file"
#define TEST_FILE_HIGH     TEST_DIR "/test_malware_file"
#define TEST_FILE_MEDIUM   TEST_DIR "/test_risky_file"
#define TEST_FILE_LOW      TEST_DIR "/test_generic_file"

#define TEST_DIR_MALWARES  TEST_DIR "/test_dir"
#define TEST_DIR_ROOT      "/"
#define TEST_DIR_MEDIA     "/opt/usr/media"
#define TEST_DIR_TMP       "/tmp"
#define TEST_DIR_APPS      "/opt/usr/apps"

#define TEST_FILE_MEDIA               TEST_DIR_MEDIA "/test_malware_file"
#define TEST_FILE_TMP_IN_DIR_MALWARES TEST_DIR_MALWARES "/tmp_malware_file"
#define TEST_FILE_TMP                 TEST_DIR_TMP "/test_malware_file"
#define TEST_FILE_NO_EXIST            TEST_DIR_TMP "/not_existing_file"

#define TEST_WGT_PKG_ID    "hFhcNcbE6K"
#define TEST_WGT_TYPE      "wgt"
#define TEST_WGT_PATH      TEST_RES_DIR "/" TEST_TARGET "/MaliciousWgt.wgt"
const std::string &TEST_WGT_APP_ROOT();
const std::string &TEST_WGT_MAL_FILE();

#define TEST_SAFE_WGT_PKG_ID   "q3JfX6RA5q"
#define TEST_SAFE_WGT_TYPE     "wgt"
#define TEST_SAFE_WGT_PATH     TEST_RES_DIR "/" TEST_TARGET "/non-malware.wgt"

#define TEST_TPK_PKG_ID    "org.example.malicioustpk"
#define TEST_TPK_TYPE      "tpk"
#define TEST_TPK_PATH      TEST_RES_DIR "/" TEST_TARGET "/MaliciousTpk.tpk"
const std::string &TEST_TPK_APP_ROOT();
const std::string &TEST_TPK_MAL_FILE();

#define TEST_FAKE_APP_ROOT TEST_DIR_APPS "/fake_app"
#define TEST_FAKE_APP_FILE TEST_FAKE_APP_ROOT "/malicious.txt"

#define MALWARE_HIGH_NAME            "test_malware"
#define MALWARE_HIGH_SEVERITY        CSR_CS_SEVERITY_HIGH
#define MALWARE_HIGH_DETAILED_URL    "http://high.malware.com"
#define MALWARE_HIGH_SIGNATURE       "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*"

#define MALWARE_MEDIUM_NAME          "test_risk"
#define MALWARE_MEDIUM_SEVERITY      CSR_CS_SEVERITY_MEDIUM
#define MALWARE_MEDIUM_DETAILED_URL  "http://medium.malware.com"
#define MALWARE_MEDIUM_SIGNATURE     "RISKY_MALWARE"

#define MALWARE_LOW_NAME             "test_generic"
#define MALWARE_LOW_SEVERITY         CSR_CS_SEVERITY_LOW
#define MALWARE_LOW_DETAILED_URL     "http://low.malware.com"
#define MALWARE_LOW_SIGNATURE        "GENERIC_MALWARE"

// Test data for web protection
#define RISK_HIGH_RISK               CSR_WP_RISK_HIGH
#define RISK_HIGH_URL                "http://highrisky.test.com/abc/def"
#define RISK_HIGH_DETAILED_URL       "http://high.risky.com"

#define RISK_MEDIUM_RISK             CSR_WP_RISK_MEDIUM
#define RISK_MEDIUM_URL              "https://mediumrisky.test.com:80/abc/def"
#define RISK_MEDIUM_DETAILED_URL     "http://medium.risky.com"

#define RISK_LOW_RISK                CSR_WP_RISK_LOW
#define RISK_LOW_URL                 "lowrisky.test.com:8080/abc/def"
#define RISK_LOW_DETAILED_URL        "http://low.risky.com"

#define RISK_UNVERIFIED_RISK         CSR_WP_RISK_UNVERIFIED
#define RISK_UNVERIFIED_URL          "http://unverified.test.com:8080/abc/def"
#define RISK_UNVERIFIED_DETAILED_URL (nullptr)

