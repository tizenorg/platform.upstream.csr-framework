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
#include <csre/content-screening.h>
#include <csre/content-screening-engine-info.h>

#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/test/unit_test.hpp>

#define CHECK_IS_NULL(ptr)     BOOST_REQUIRE(ptr == nullptr)
#define CHECK_IS_NOT_NULL(ptr) BOOST_REQUIRE(ptr != nullptr)

#define TEST_FILE_NORMAL   TEST_DIR "/test_normal_file"
#define TEST_FILE_MALWARE  TEST_DIR "/test_malware_file"
#define TEST_FILE_RISKY    TEST_DIR "/test_risky_file"
#define TEST_APP_ROOT      TEST_DIR "/test_app"

namespace {

inline void checkDetected(csre_cs_detected_h detected,
		csre_cs_severity_level_e expected_severity,
		csre_cs_threat_type_e expected_threat_type,
		const char *expected_malware_name,
		const char *expected_detailed_url,
		long expected_timestamp)
{
	int ret = CSRE_ERROR_UNKNOWN;
	CHECK_IS_NOT_NULL(detected);

	csre_cs_severity_level_e severity;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_detected_get_severity(detected, &severity));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE_MESSAGE(severity == expected_severity,
		"severity isn't expected value. "
		"val: " << severity << " expected: " << expected_severity);

	csre_cs_threat_type_e threat_type;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_detected_get_threat_type(detected, &threat_type));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE_MESSAGE(threat_type == expected_threat_type,
		"threat type isn't expected value. "
		"val: " << threat_type << " expected: " << expected_threat_type);

	const char *malware_name = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_detected_get_malware_name(detected, &malware_name));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);

	if (expected_malware_name != nullptr) {
		CHECK_IS_NOT_NULL(malware_name);
		BOOST_REQUIRE_MESSAGE(
			(strlen(malware_name) == strlen(expected_malware_name) &&
				memcmp(malware_name, expected_malware_name, strlen(malware_name)) == 0),
			"malware_name isn't expected value. "
				"val: " << malware_name << " expected: " << expected_malware_name);
	}

	const char *detailed_url = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_detected_get_detailed_url(detected, &detailed_url));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);

	if (expected_detailed_url != nullptr) {
		CHECK_IS_NOT_NULL(detailed_url);
		BOOST_REQUIRE_MESSAGE(
			(strlen(detailed_url) == strlen(expected_detailed_url) &&
				memcmp(detailed_url, expected_detailed_url, strlen(detailed_url)) == 0),
			"detailed_url isn't expected value. "
				"val: " << detailed_url << " expected: " << expected_detailed_url);

	}

	long timestamp;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_detected_get_timestamp(detected, &timestamp));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);

	if (expected_timestamp != 0)
		BOOST_REQUIRE_MESSAGE(timestamp == expected_timestamp,
			"timestamp isn't expected value. "
				"val: " << timestamp << " expected: " << expected_timestamp);
}

class ContextPtr {
public:
	ContextPtr() : m_context(nullptr) {}
	ContextPtr(csre_cs_context_h context) : m_context(context) {}
	virtual ~ContextPtr()
	{
		int ret = csre_cs_context_destroy(m_context);
		BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	}

	csre_cs_context_h get(void)
	{
		return m_context;
	}

private:
	csre_cs_context_h m_context;
};

class ScopedFile {
public:
	explicit ScopedFile(const std::string &file)
	{
		int fd = open(file.c_str(), O_RDONLY);
		BOOST_REQUIRE_MESSAGE(fd > 0,
			"Cannot open file: " << file << " with errno: " << errno);

		m_fd = fd;
	}

	virtual ~ScopedFile()
	{
		close(m_fd);
	}

	int getFd(void)
	{
		return m_fd;
	}

private:
	int m_fd;
};

using ScopedContext = std::unique_ptr<ContextPtr>;

inline ScopedContext makeScopedContext(csre_cs_context_h context)
{
	return ScopedContext(new ContextPtr(context));
}

inline csre_cs_engine_h getEngineHandle(void)
{
	csre_cs_engine_h engine;
	int ret = CSRE_ERROR_UNKNOWN;

	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_info(&engine));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(engine);

	return engine;
}

inline ScopedContext getContextHandleWithDir(const char *dir)
{
	csre_cs_context_h context;
	int ret = CSRE_ERROR_UNKNOWN;

	BOOST_REQUIRE_NO_THROW(ret = csre_cs_context_create(dir, &context));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(context);

	return makeScopedContext(context);
}

inline ScopedContext getContextHandle(void)
{
	return getContextHandleWithDir(SAMPLE_ENGINE_WORKING_DIR);
}

}

BOOST_AUTO_TEST_SUITE(API_ENGINE_CONTENT_SCREENING)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	auto context = getContextHandle();
	(void)context;
}

BOOST_AUTO_TEST_CASE(scan_data_clear)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	const char *data = "abcd1234dfdfdf334dfdi8ffndsfdfdsfdasfagdfvdfdfafadfasdfsdfe";

	csre_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_scan_data(
		context,
		reinterpret_cast<const unsigned char *>(data),
		strlen(data),
		&detected));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NULL(detected);
}

BOOST_AUTO_TEST_CASE(scan_data_high)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	const char *data =
		"aabbccX5O!P%@AP[4\\PZX54(P^)7CC)7}$"
		"EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*112233";

	csre_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_scan_data(
		context,
		reinterpret_cast<const unsigned char *>(data),
		strlen(data),
		&detected));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected,
		CSRE_CS_SEVERITY_HIGH,
		CSRE_CS_THREAT_MALWARE,
		"test_malware",
		"http://high.malware.com",
		0);
}

BOOST_AUTO_TEST_CASE(scan_data_medium)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	const char *data = "aabbccRISKY_MALWARE112233";

	csre_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_scan_data(
		context,
		reinterpret_cast<const unsigned char *>(data),
		strlen(data),
		&detected));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected,
		CSRE_CS_SEVERITY_MEDIUM,
		CSRE_CS_THREAT_RISKY,
		"test_risk",
		nullptr,
		0);
}

BOOST_AUTO_TEST_CASE(scan_file_normal)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	csre_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_scan_file(context, TEST_FILE_NORMAL, &detected));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NULL(detected);
}

BOOST_AUTO_TEST_CASE(scan_file_malware)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	csre_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_scan_file(context, TEST_FILE_MALWARE, &detected));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected,
		CSRE_CS_SEVERITY_HIGH,
		CSRE_CS_THREAT_MALWARE,
		"test_malware",
		"http://high.malware.com",
		0);
}

BOOST_AUTO_TEST_CASE(scan_file_risky)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	csre_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_scan_file(context, TEST_FILE_RISKY, &detected));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected,
		CSRE_CS_SEVERITY_MEDIUM,
		CSRE_CS_THREAT_RISKY,
		"test_risk",
		"http://medium.malware.com",
		0);
}

BOOST_AUTO_TEST_CASE(scan_app_on_cloud)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto contextPtr = getContextHandle();
	auto context = contextPtr->get();

	csre_cs_detected_h detected;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_scan_app_on_cloud(context, TEST_APP_ROOT,&detected));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(detected);

	checkDetected(detected,
		CSRE_CS_SEVERITY_HIGH,
		CSRE_CS_THREAT_MALWARE,
		"test_malware",
		"http://high.malware.com",
		0);
}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_CONTENT_SCREENING_ERR_STRING)

BOOST_AUTO_TEST_CASE(positive)
{
	int ret = CSRE_ERROR_UNKNOWN;

	const char *string = nullptr;

	BOOST_REQUIRE_NO_THROW(ret = csre_cs_get_error_string(ret, &string));

	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(string);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(API_ENGINE_CONTENT_SCREENING_ENGINE_INFO)

BOOST_AUTO_TEST_CASE(get_engine_info)
{
	auto handle = getEngineHandle();
	(void)handle;
}

BOOST_AUTO_TEST_CASE(get_vendor)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *vendor = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_vendor(handle, &vendor));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(get_vendor_logo)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	unsigned char *vendor_logo_image = nullptr;
	unsigned int size = 0;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_vendor_logo(handle, &vendor_logo_image, &size));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(get_version)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *version = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_version(handle, &version));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);
}

BOOST_AUTO_TEST_CASE(get_data_version)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *version = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_data_version(handle, &version));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	CHECK_IS_NOT_NULL(version);
}

BOOST_AUTO_TEST_CASE(get_latest_update_time)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	time_t time = 0;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_latest_update_time(handle, &time));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE(time > 0);
}

BOOST_AUTO_TEST_CASE(get_engine_activated)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	csre_cs_activated_e activated;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_activated(handle, &activated));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(get_api_version)
{
	int ret = CSRE_ERROR_UNKNOWN;
	auto handle = getEngineHandle();

	const char *version = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csre_cs_engine_get_api_version(handle, &version));
	BOOST_REQUIRE(ret == CSRE_ERROR_NONE);
	BOOST_REQUIRE(memcmp(version, CSRE_CS_API_VERSION, strlen(version)) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
