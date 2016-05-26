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
 */
#define BOOST_TEST_MODULE CSR_API_TEST
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/results_reporter.hpp>
#include <colour_log_formatter.h>

#include <csr-engine-manager.h>

namespace {

csr_state_e setEngineState(csr_engine_id_e id, csr_state_e state)
{
	csr_engine_h handle;
	auto ret = csr_get_current_engine(id, &handle);
	if (ret != CSR_ERROR_NONE)
		throw std::logic_error("Failed to csr_get_current_engine.");

	csr_state_e current;
	ret = csr_engine_get_state(handle, &current);
	if (ret != CSR_ERROR_NONE)
		throw std::logic_error("Failed to csr_get_state.");

	if (current == state)
		return current;

	ret = csr_engine_set_state(handle, state);
	if (ret != CSR_ERROR_NONE)
		throw std::logic_error("Failed to csr_engine_set_state.");

	return current;
}

}

struct TestConfig {
	TestConfig()
	{
		boost::unit_test::unit_test_log.set_threshold_level(
			boost::unit_test::log_test_units);
		boost::unit_test::results_reporter::set_level(boost::unit_test::SHORT_REPORT);
		boost::unit_test::unit_test_log.set_formatter(new Csr::Test::colour_log_formatter);
	}
};

bool isEngineInitialized = false;
struct Initializer {
	Initializer()
	{
		m_oldCsState = setEngineState(CSR_ENGINE_CS, CSR_ENABLE);
		m_oldWpState = setEngineState(CSR_ENGINE_WP, CSR_ENABLE);
	}

	~Initializer()
	{
		setEngineState(CSR_ENGINE_CS, m_oldCsState);
		setEngineState(CSR_ENGINE_WP, m_oldWpState);
	}

	csr_state_e m_oldCsState;
	csr_state_e m_oldWpState;
};

BOOST_GLOBAL_FIXTURE(TestConfig)
BOOST_GLOBAL_FIXTURE(Initializer)
