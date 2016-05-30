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
						  const char *expected_detailed_url,
						  time_t expected_timestamp)
{
	EXCEPTION_GUARD_START

	CHECK_IS_NOT_NULL(detected);

	csre_cs_severity_level_e severity;
	ASSERT_IF(loader.getSeverity(detected, &severity), CSRE_ERROR_NONE);
	ASSERT_IF(severity, expected_severity);

	std::string malware_name;
	ASSERT_IF(loader.getMalwareName(detected, malware_name), CSRE_ERROR_NONE);

	if (expected_malware_name != nullptr)
		ASSERT_IF(malware_name, expected_malware_name);

	std::string detailed_url;
	ASSERT_IF(loader.getDetailedUrl(detected, detailed_url), CSRE_ERROR_NONE);

	if (expected_detailed_url != nullptr)
		ASSERT_IF(detailed_url, expected_detailed_url);

	time_t timestamp;
	ASSERT_IF(loader.getTimestamp(detected, &timestamp), CSRE_ERROR_NONE);

	if (expected_timestamp != 0)
		ASSERT_IF(timestamp, expected_timestamp);

	EXCEPTION_GUARD_END
}

template <typename T>
struct Handle {
	Handle()
	{
		BOOST_REQUIRE_MESSAGE(0, "Not specialized for handle template");
	}

	~Handle()
	{
		BOOST_REQUIRE_MESSAGE(0, "Not specialized for handle template");
	}

	Csr::CsLoader loader;
	T context;
};

template <>
struct Handle<csre_cs_context_h> {
	Handle() :
		loader(SAMPLE_ENGINE_DIR "/libcsr-cs-engine.so",
			   SAMPLE_ENGINE_RO_RES_DIR,
			   SAMPLE_ENGINE_RW_WORKING_DIR)
	{
		ASSERT_IF(loader.contextCreate(context), CSRE_ERROR_NONE);
	}

	~Handle()
	{
		ASSERT_IF(loader.contextDestroy(context), CSRE_ERROR_NONE);
	}

	Csr::CsLoader loader;
	csre_cs_context_h context;
};

template <>
struct Handle<csre_cs_engine_h> {
	Handle() :
		loader(SAMPLE_ENGINE_DIR "/libcsr-cs-engine.so",
			   SAMPLE_ENGINE_RO_RES_DIR,
			   SAMPLE_ENGINE_RW_WORKING_DIR)
	{
		ASSERT_IF(loader.getEngineInfo(context), CSRE_ERROR_NONE);
	}

	~Handle()
	{
		ASSERT_IF(loader.destroyEngine(context), CSRE_ERROR_NONE);
	}

	Csr::CsLoader loader;
	csre_cs_engine_h context;
};

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(CS_LOADER)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;
	(void) h;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_clear)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	const char *cdata =
		"abcd1234dfdfdf334dfdi8ffndsfdfdsfdasfagdfvdfdfafadfasdfsdfe";

	csre_cs_detected_h detected;
	std::vector<unsigned char> data(cdata, cdata + strlen(cdata));
	ASSERT_IF(h.loader.scanData(h.context, data, &detected), CSRE_ERROR_NONE);

	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_high)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	const char *cdata =
		"aabbccX5O!P%@AP[4\\PZX54(P^)7CC)7}$"
		"EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*112233";

	csre_cs_detected_h detected;
	std::vector<unsigned char> data(cdata, cdata + strlen(cdata));
	ASSERT_IF(h.loader.scanData(h.context, data, &detected), CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_HIGH,
				  "test_malware",
				  "http://high.malware.com",
				  0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_data_medium)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	const char *cdata = "aabbccRISKY_MALWARE112233";

	csre_cs_detected_h detected;
	std::vector<unsigned char> data(cdata, cdata + strlen(cdata));
	ASSERT_IF(h.loader.scanData(h.context, data, &detected), CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_MEDIUM,
				  "test_risk",
				  nullptr,
				  0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_normal)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	csre_cs_detected_h detected;
	ASSERT_IF(h.loader.scanFile(h.context, TEST_FILE_NORMAL, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NULL(detected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_malware)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	csre_cs_detected_h detected;
	ASSERT_IF(h.loader.scanFile(h.context, TEST_FILE_MALWARE, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_HIGH,
				  "test_malware",
				  "http://high.malware.com",
				  0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_file_risky)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	csre_cs_detected_h detected;
	ASSERT_IF(h.loader.scanFile(h.context, TEST_FILE_RISKY, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_MEDIUM,
				  "test_risk",
				  "http://medium.malware.com",
				  0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_app_on_cloud)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	csre_cs_detected_h detected;
	ASSERT_IF(h.loader.scanAppOnCloud(h.context, TEST_APP_ROOT, &detected),
			  CSRE_ERROR_NONE);

	CHECK_IS_NOT_NULL(detected);

	checkDetected(h.loader,
				  detected,
				  CSRE_CS_SEVERITY_HIGH,
				  "test_malware",
				  "http://high.malware.com",
				  0);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(error_string_positive)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_context_h> h;

	std::string str;

	ASSERT_IF(h.loader.getErrorString(CSRE_ERROR_UNKNOWN, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_engine_info)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;
	(void) h;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineVendor(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor_logo)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;

	std::vector<unsigned char> logo;
	ASSERT_IF(h.loader.getEngineVendorLogo(h.context, logo), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_version)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineVersion(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_data_version)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineDataVersion(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_latest_update_time)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;

	time_t time = 0;
	ASSERT_IF(h.loader.getEngineLatestUpdateTime(h.context, &time),
			  CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_engine_activated)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;

	csre_cs_activated_e activated;
	ASSERT_IF(h.loader.getEngineActivated(h.context, &activated), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_api_version)
{
	EXCEPTION_GUARD_START

	Handle<csre_cs_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineApiVersion(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str == CSRE_CS_API_VERSION, true);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
