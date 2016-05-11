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
 * @file        test-api-engine-manager.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Engine manager API test
 */
#include <csr/engine-manager.h>

#include <string>
#include <boost/test/unit_test.hpp>

#include "test-common.h"

namespace {

class Engine {
public:
	Engine(csr_engine_id_e id)
	{
		ASSERT_IF(csr_get_current_engine(id, &m_engine), CSR_ERROR_NONE);
	}

	~Engine()
	{
		ASSERT_IF(csr_engine_destroy(m_engine), CSR_ERROR_NONE);
	}

	csr_engine_h &get(void)
	{
		return m_engine;
	}

private:
	csr_engine_h m_engine;
};

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(API_ENGINE_MANAGER)
BOOST_AUTO_TEST_SUITE(CS)

BOOST_AUTO_TEST_CASE(fields_getters)
{
	EXCEPTION_GUARD_START

	Engine e(CSR_ENGINE_CS);

	const char *vendor = nullptr;
	ASSERT_IF(csr_engine_get_vendor(e.get(), &vendor), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_VENDOR"), vendor);

	const char *name = nullptr;
	ASSERT_IF(csr_engine_get_name(e.get(), &name), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_LOCAL_TCS_ENGINE"), name);

	const char *version = nullptr;
	ASSERT_IF(csr_engine_get_version(e.get(), &version), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), version);

	const char *dataVersion = nullptr;
	ASSERT_IF(csr_engine_get_version(e.get(), &dataVersion), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), dataVersion);

	csr_activated_e activated;
	ASSERT_IF(csr_engine_get_activated(e.get(), &activated), CSR_ERROR_NONE);
	ASSERT_IF(activated, CSR_ACTIVATED);

	csr_state_e state = CSR_ENABLE;
	ASSERT_IF(csr_engine_set_state(e.get(), state), CSR_ERROR_NONE);

	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_ENABLE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END() // CS

BOOST_AUTO_TEST_SUITE(WP)

BOOST_AUTO_TEST_CASE(fields_getters)
{
	EXCEPTION_GUARD_START

	Engine e(CSR_ENGINE_WP);

	const char *vendor = nullptr;
	ASSERT_IF(csr_engine_get_vendor(e.get(), &vendor), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_VENDOR"), vendor);

	const char *name = nullptr;
	ASSERT_IF(csr_engine_get_name(e.get(), &name), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_LOCAL_TWP_ENGINE"), name);

	const char *version = nullptr;
	ASSERT_IF(csr_engine_get_version(e.get(), &version), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), version);

	const char *dataVersion = nullptr;
	ASSERT_IF(csr_engine_get_version(e.get(), &dataVersion), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), dataVersion);

	csr_activated_e activated;
	ASSERT_IF(csr_engine_get_activated(e.get(), &activated), CSR_ERROR_NONE);
	ASSERT_IF(activated, CSR_ACTIVATED);

	csr_state_e state = CSR_ENABLE;
	ASSERT_IF(csr_engine_set_state(e.get(), state), CSR_ERROR_NONE);

	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_ENABLE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END() // WP
BOOST_AUTO_TEST_SUITE_END() // API_ENGINE_MANAGER
