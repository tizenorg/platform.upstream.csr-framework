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

#define TEST_FILE_NORMAL   TEST_DIR "/test_normal_file"
#define TEST_FILE_HIGH     TEST_DIR "/test_malware_file"
#define TEST_FILE_MEDIUM   TEST_DIR "/test_risky_file"
#define TEST_FILE_LOW      TEST_DIR "/test_generic_file"

#define TEST_FILE_MEDIA    "/opt/usr/media/test_malware_file"
#define TEST_FILE_TMP      "/tmp/test_malware_file"
#define TEST_FILE_APP_ROOT "/opt/usr/apps"
#define TEST_FILE_NO_EXIST "/tmp/not_existing_file"

#define TEST_WGT_PKG_ID    "hFhcNcbE6K"
#define TEST_WGT_APP_ROOT  TEST_FILE_APP_ROOT "/" TEST_WGT_PKG_ID
#define TEST_WGT_MAL_FILE  TEST_WGT_APP_ROOT "/res/wgt/data/malicious.txt"
#define TEST_WGT_PATH      TEST_DIR "/" TEST_TARGET "/MaliciousWgt.wgt"
#define TEST_WGT_TYPE      "WGT"

#define TEST_TPK_PKG_ID    "org.example.malicioustpk"
#define TEST_TPK_APP_ROOT  TEST_FILE_APP_ROOT "/" TEST_TPK_PKG_ID
#define TEST_TPK_MAL_FILE  TEST_TPK_APP_ROOT "/shared/res/malicious.txt"
#define TEST_TPK_PATH      TEST_DIR "/" TEST_TARGET "/MaliciousTpk.tpk"
#define TEST_TPK_TYPE      "TPK"

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
