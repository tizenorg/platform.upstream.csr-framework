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

	Result(csre_wp_risk_level_e r, const char *durl) :
		risk_level(r),
		detailed_url(durl ? durl : std::string()) {}
};

std::unordered_map<std::string, Result> ExpectedResult = {
	{"http://normal.test.com",      Result(CSRE_WP_RISK_UNVERIFIED, nullptr)},
	{"http://highrisky.test.com",   Result(CSRE_WP_RISK_HIGH, "http://high.risky.com")},
	{"http://mediumrisky.test.com", Result(CSRE_WP_RISK_MEDIUM, "http://medium.risky.com")},
	{"http://lowrisky.test.com",    Result(CSRE_WP_RISK_LOW, "http://low.risky.com")}
};

inline void checkResult(Csr::WpLoader &loader,
						csre_wp_check_result_h &result,
						const std::pair<const std::string, Result> &expected)
{
	EXCEPTION_GUARD_START

	CHECK_IS_NOT_NULL(result);

	csre_wp_risk_level_e risk_level;
	loader.getRiskLevel(result, &risk_level);
	ASSERT_IF(risk_level, expected.second.risk_level);

	std::string detailed_url;
	loader.getDetailedUrl(result, detailed_url);
	ASSERT_IF(detailed_url, expected.second.detailed_url);

	EXCEPTION_GUARD_END
}

struct Handle {
	Handle() :
		loader(ENGINE_DIR "/libcsr-wp-engine.so",
			   ENGINE_DIR,
			   ENGINE_RW_WORKING_DIR)
	{
		loader.contextCreate(context);
	}

	~Handle()
	{
		loader.contextDestroy(context);
	}

	Csr::WpLoader loader;
	csre_wp_context_h context;
};

} // namespace anonymous


BOOST_AUTO_TEST_SUITE(WP_LOADER)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	EXCEPTION_GUARD_START

	Handle h;
	(void) h;

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(check_url)
{
	EXCEPTION_GUARD_START

	Handle h;

	for (const auto &expected : ExpectedResult) {
		csre_wp_check_result_h result;
		h.loader.checkUrl(h.context, expected.first.c_str(), &result);
		checkResult(h.loader, result, expected);
	}

	EXCEPTION_GUARD_END
}


BOOST_AUTO_TEST_CASE(error_string)
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

	csre_wp_activated_e activated;
	h.loader.getEngineActivated(h.context, &activated);

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
