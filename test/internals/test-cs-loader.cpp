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
 * @file        test-cs-loader.cpp
 * @author      Sangsu Choi (sangsu.choi@samsung.com)
 * @version     1.0
 * @brief       Content screening engine dynamic loading test
 */
#include "service/cs-loader.h"

#include <cstring>
#include <cstdio>
#include <ctime>
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

inline void checkDetected(Csr::CsLoader &loader,
						  csre_cs_detected_h detected,
						  csre_cs_severity_level_e expected_severity,
						  const char *expected_malware_name,
						  const char *expected_detailed_url)
{
	EXCEPTION_GUARD_START

	CHECK_IS_NOT_NULL(detected);

	csre_cs_severity_level_e severity;
	loader.getSeverity(detected, &severity);
	ASSERT_IF(severity, expected_severity);

	std::string malware_name;
	loader.getMalwareName(detected, malware_name);

	if (expected_malware_name != nullptr)
		ASSERT_IF(malware_name, expected_malware_name);

	std::string detailed_url;
	loader.getDetailedUrl(detected, detailed_url);

	if (expected_detailed_url != nullptr)
		ASSERT_IF(detailed_url, expected_detailed_url);

	EXCEPTION_GUARD_END
}

struct Handle {
	Handle() :
		loader(ENGINE_DIR "/libcsr-cs-engine.so",
			   ENGINE_DIR,
			   ENGINE_RW_WORKING_DIR)
	{
		loader.contextCreate(context);
	}

	~Handle()
	{
		loader.contextDestroy(context);
	}

	Csr::CsLoader loader;
	csre_cs_context_h context;
};

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(CS_LOADER)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Handle h;
	(void) h;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_clear)
{
	EXCEPTION_GUARD_START

	Handle h;

	const char *cdata =
		"abcd1234dfdfdf334dfdi8ffndsfdfdsfdasfagdfvdfdfafadfasdfsdfe";

	csre_cs_detected_h detected;
	std::vector<unsigned char> data(cdata, cdata + strlen(cdata));
	h.loader.scanData(h.context, data, &detected);

	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_high)
{
	EXCEPTION_GUARD_START

	Handle h;

	const char *cdata =
		"aabbccX5O!P%@AP[4\\PZX54(P^)7CC)7}$"
		"EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*112233";

	csre_cs_detected_h detected;
	std::vector<unsigned char> data(cdata, cdata + strlen(cdata));
	h.loader.scanData(h.context, data, &detected);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_HIGH,
				  "test_malware",
				  "http://high.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_medium)
{
	EXCEPTION_GUARD_START

	Handle h;

	const char *cdata = "aabbccRISKY_MALWARE112233";

	csre_cs_detected_h detected;
	std::vector<unsigned char> data(cdata, cdata + strlen(cdata));
	h.loader.scanData(h.context, data, &detected);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_MEDIUM,
				  "test_risk",
				  nullptr);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_normal)
{
	EXCEPTION_GUARD_START

	Handle h;

	csre_cs_detected_h detected;
	h.loader.scanFile(h.context, TEST_FILE_NORMAL, &detected);

	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_malware)
{
	EXCEPTION_GUARD_START

	Handle h;

	csre_cs_detected_h detected;
	h.loader.scanFile(h.context, TEST_FILE_MALWARE, &detected);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_HIGH,
				  "test_malware",
				  "http://high.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_risky)
{
	EXCEPTION_GUARD_START

	Handle h;

	csre_cs_detected_h detected;
	h.loader.scanFile(h.context, TEST_FILE_RISKY, &detected);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_MEDIUM,
				  "test_risk",
				  "http://medium.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_app_on_cloud)
{
	EXCEPTION_GUARD_START

	Handle h;

	csre_cs_detected_h detected;
	h.loader.scanAppOnCloud(h.context, TEST_APP_ROOT, &detected);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_HIGH,
				  "test_malware",
				  "http://high.malware.com");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_app_on_cloud_supported)
{
	EXCEPTION_GUARD_START

	Handle h;

	BOOST_MESSAGE("scanAppOnCloudSupported: " << h.loader.scanAppOnCloudSupported());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(error_string_positive)
{
	EXCEPTION_GUARD_START

	Handle h;

	std::string str = h.loader.getErrorString(CSRE_ERROR_UNKNOWN);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor)
{
	EXCEPTION_GUARD_START

	Handle h;

	std::string str;
	h.loader.getEngineVendor(h.context, str);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor_logo)
{
	EXCEPTION_GUARD_START

	Handle h;

	std::vector<unsigned char> logo;
	h.loader.getEngineVendorLogo(h.context, logo);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_version)
{
	EXCEPTION_GUARD_START

	Handle h;

	std::string str;
	h.loader.getEngineVersion(h.context, str);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_data_version)
{
	EXCEPTION_GUARD_START

	Handle h;

	std::string str;
	h.loader.getEngineDataVersion(h.context, str);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_latest_update_time)
{
	EXCEPTION_GUARD_START

	Handle h;

	time_t time = 0;
	h.loader.getEngineLatestUpdateTime(h.context, &time);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_engine_activated)
{
	EXCEPTION_GUARD_START

	Handle h;

	csre_cs_activated_e activated;
	h.loader.getEngineActivated(h.context, &activated);;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_api_version)
{
	EXCEPTION_GUARD_START

	Handle h;

	std::string str;
	h.loader.getEngineApiVersion(h.context, str);
	ASSERT_IF(str == CSRE_CS_API_VERSION, true);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
