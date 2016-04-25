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
 * @file        test-wp-loader.cpp
 * @author      Sangsu Choi (sangsu.choi@samsung.com)
 * @version     1.0
 * @brief       web protection Engine loading test
 */
#include "service/wp-loader.h"

#include <ctime>
#include <cstring>
#include <unordered_map>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/test/unit_test.hpp>

#include "test-common.h"

namespace {

struct Result {
	csre_wp_risk_level_e risk_level;
	std::string detailed_url;

	Result(csre_wp_risk_level_e r, const char* durl) : risk_level(r), detailed_url(durl) {}
};

std::unordered_map<std::string, Result> ExpectedResult = {
	{"http://normal.test.com",      Result(CSRE_WP_RISK_UNVERIFIED, "")},
	{"http://highrisky.test.com",   Result(CSRE_WP_RISK_HIGH, "http://high.risky.com")},
	{"http://mediumrisky.test.com", Result(CSRE_WP_RISK_MEDIUM, "http://medium.risky.com")},
	{"http://lowrisky.test.com",    Result(CSRE_WP_RISK_LOW, "")}
};

inline void checkResult(const std::string &url, csre_wp_check_result_h &result, const Result &expected)
{
	EXCEPTION_GUARD_START

	CHECK_IS_NOT_NULL(result);

	csre_wp_risk_level_e risk_level;
	ASSERT_IF(csre_wp_result_get_risk_level(result, &risk_level), CSRE_ERROR_NONE);
	BOOST_REQUIRE_MESSAGE(risk_level == expected.risk_level,
		"url[" << url << "] risk level isn't expected value. "
			"val: " << risk_level << " expected: " << expected.risk_level);

	const char *detailed_url = nullptr;
	ASSERT_IF(csre_wp_result_get_detailed_url(result, &detailed_url), CSRE_ERROR_NONE);
	BOOST_REQUIRE_MESSAGE(expected.detailed_url.compare(detailed_url) == 0,
		"url[" << url << "] detailed url isn't expected value. "
			"val: " << detailed_url <<" expected: " << expected.detailed_url);


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

	Csr::WpLoader loader;
	T context;
};

template <>
struct Handle<csre_wp_context_h> {
	Handle() : loader(SAMPLE_ENGINE_DIR "/libcsr-wp-engine.so")
	{
		ASSERT_IF(
			loader.globalInit(SAMPLE_ENGINE_RO_RES_DIR, SAMPLE_ENGINE_RW_WORKING_DIR),
			CSRE_ERROR_NONE);
		ASSERT_IF(loader.contextCreate(context), CSRE_ERROR_NONE);
	}

	~Handle()
	{
		ASSERT_IF(loader.contextDestroy(context), CSRE_ERROR_NONE);
		ASSERT_IF(loader.globalDeinit(), CSRE_ERROR_NONE);
	}

	Csr::WpLoader loader;
	csre_wp_context_h context;
};

template <>
struct Handle<csre_wp_engine_h> {
	Handle() : loader(SAMPLE_ENGINE_DIR "/libcsr-wp-engine.so")
	{
		ASSERT_IF(
			loader.globalInit(SAMPLE_ENGINE_RO_RES_DIR, SAMPLE_ENGINE_RW_WORKING_DIR),
			CSRE_ERROR_NONE);
		ASSERT_IF(loader.getEngineInfo(context), CSRE_ERROR_NONE);
	}

	~Handle()
	{
		ASSERT_IF(loader.destroyEngine(context), CSRE_ERROR_NONE);
		ASSERT_IF(loader.globalDeinit(), CSRE_ERROR_NONE);
	}

	Csr::WpLoader loader;
	csre_wp_engine_h context;
};

} // namespace anonymous



BOOST_AUTO_TEST_SUITE(WP_LOADER)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_context_h> h;
	(void) h;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_context_h> h;

	for (const auto &pair : ExpectedResult) {
		csre_wp_check_result_h result;
		ASSERT_IF(h.loader.checkUrl(h.context, pair.first.c_str(), &result), CSRE_ERROR_NONE);
		checkResult(pair.first, result, pair.second);
	}

	EXCEPTION_GUARD_END
}


BOOST_AUTO_TEST_CASE(error_string)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	std::string str;

	ASSERT_IF(h.loader.getErrorString(CSRE_ERROR_UNKNOWN, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}


BOOST_AUTO_TEST_CASE(get_engine_info)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;
	(void) h;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineVendor(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_vendor_logo)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	std::vector<unsigned char> logo;
	ASSERT_IF(h.loader.getEngineVendorLogo(h.context, logo), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_version)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineVersion(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_data_version)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineDataVersion(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str.empty(), false);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_latest_update_time)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	time_t time = 0;
	ASSERT_IF(h.loader.getEngineLatestUpdateTime(h.context, &time),
			  CSRE_ERROR_NONE);

	struct tm t;
	BOOST_MESSAGE(asctime(gmtime_r(&time, &t)));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_engine_activated)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	csre_wp_activated_e activated;
	ASSERT_IF(h.loader.getEngineActivated(h.context, &activated), CSRE_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_api_version)
{
	EXCEPTION_GUARD_START

	Handle<csre_wp_engine_h> h;

	std::string str;
	ASSERT_IF(h.loader.getEngineApiVersion(h.context, str), CSRE_ERROR_NONE);
	ASSERT_IF(str == CSRE_CS_API_VERSION, true);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
