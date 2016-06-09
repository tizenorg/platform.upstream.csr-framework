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
 * @file       test-main.cpp
 * @author     Kyungwook Tak(k.tak@samsung.com)
 * @version    1.0
 * @brief      internal test main
 */
#define BOOST_TEST_MODULE CSR_INTERNAL_TEST
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/results_reporter.hpp>
#include <colour_log_formatter.h>

#include <csre-content-screening.h>
#include <csre-web-protection.h>

struct TestConfig {
	TestConfig()
	{
		boost::unit_test::unit_test_log.set_threshold_level(
			boost::unit_test::log_test_units);
		boost::unit_test::results_reporter::set_level(boost::unit_test::SHORT_REPORT);
		boost::unit_test::unit_test_log.set_formatter(new
				Csr::Test::colour_log_formatter);
	}
};

BOOST_GLOBAL_FIXTURE(TestConfig)

#ifdef WITH_SAMPLE_ENGINE
bool isEngineInitialized = false;
struct Initializer {
	Initializer()
	{
		if (!isEngineInitialized) {
			int ret = csre_cs_global_initialize(ENGINE_DIR, ENGINE_RW_WORKING_DIR);

			if (ret != CSRE_ERROR_NONE)
				throw std::logic_error("Failed to init content screening engine.");

			ret = csre_wp_global_initialize(ENGINE_DIR, ENGINE_RW_WORKING_DIR);

			if (ret != CSRE_ERROR_NONE)
				throw std::logic_error("Failed to init web protection engine.");

			isEngineInitialized = true;

			BOOST_MESSAGE("Initialize engines");
		}
	}

	~Initializer()
	{
		if (!isEngineInitialized)
			return;

		int ret = csre_cs_global_deinitialize();

		if (ret != CSRE_ERROR_NONE)
			throw std::logic_error("Failed to deinit content screening engine.");

		ret = csre_wp_global_deinitialize();

		if (ret != CSRE_ERROR_NONE)
			throw std::logic_error("Failed to deinit web protection engine.");

		BOOST_MESSAGE("Deinitialize engines");
	}
};

BOOST_GLOBAL_FIXTURE(Initializer)
#endif
