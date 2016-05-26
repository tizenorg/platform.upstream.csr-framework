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
#include "test-helper.h"
#include "test-resource.h"

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

	Test::ScopedCstr vendor;
	ASSERT_IF(csr_engine_get_vendor(e.get(), &vendor.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_VENDOR"), vendor.ptr);

	Test::ScopedCstr name;
	ASSERT_IF(csr_engine_get_name(e.get(), &name.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_LOCAL_TCS_ENGINE"), name.ptr);

	Test::ScopedCstr version;
	ASSERT_IF(csr_engine_get_version(e.get(), &version.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), version.ptr);

	Test::ScopedCstr dataVersion;
	ASSERT_IF(csr_engine_get_version(e.get(), &dataVersion.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), dataVersion.ptr);

	csr_activated_e activated;
	ASSERT_IF(csr_engine_get_activated(e.get(), &activated), CSR_ERROR_NONE);
	ASSERT_IF(activated, CSR_ACTIVATED);

	csr_state_e state = CSR_ENABLE;
	ASSERT_IF(csr_engine_set_state(e.get(), state), CSR_ERROR_NONE);

	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_ENABLE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_state)
{
	EXCEPTION_GUARD_START

	Engine e(CSR_ENGINE_CS);

	csr_state_e state = CSR_ENABLE;

	// enable
	ASSERT_IF(csr_engine_set_state(e.get(), CSR_ENABLE), CSR_ERROR_NONE);
	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_ENABLE);

	// prepare data
	auto c = Test::Context<csr_cs_context_h>();
	auto context = c.get();
	csr_cs_detected_h detected;
	unsigned char data[100] = {0, };
	//const char *files[1] = { TEST_FILE_NORMAL };
	//const char *dirs[1] = { TEST_DIR };

	// check if engine is working well
	memcpy(data, MALWARE_HIGH_SIGNATURE, strlen(MALWARE_HIGH_SIGNATURE));
	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected), CSR_ERROR_NONE);

	// disable
	ASSERT_IF(csr_engine_set_state(e.get(), CSR_DISABLE), CSR_ERROR_NONE);
	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_DISABLE);

	// test operation
	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected), CSR_ERROR_ENGINE_DISABLED);
	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_NORMAL, &detected), CSR_ERROR_ENGINE_DISABLED);
	//ASSERT_IF(csr_cs_scan_files_async(context, files, sizeof(files) / sizeof(const char *), nullptr),
	//	CSR_ERROR_ENGINE_DISABLED);
	//ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR, nullptr), CSR_ERROR_ENGINE_DISABLED);
	//ASSERT_IF(csr_cs_scan_dirs_async(context, dirs, sizeof(dirs) / sizeof(const char *), nullptr),
	//	CSR_ERROR_ENGINE_DISABLED);

	// enable
	ASSERT_IF(csr_engine_set_state(e.get(), CSR_ENABLE), CSR_ERROR_NONE);
	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_ENABLE);

	// test operation
	ASSERT_IF(csr_cs_scan_data(context, data, sizeof(data), &detected), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_scan_file(context, TEST_FILE_NORMAL, &detected), CSR_ERROR_NONE);
	//ASSERT_IF(csr_cs_scan_files_async(context, files, sizeof(files) / sizeof(const char *), nullptr),
	//	CSR_ERROR_NONE);
	//ASSERT_IF(csr_cs_scan_dir_async(context, TEST_DIR, nullptr), CSR_ERROR_NONE);
	//ASSERT_IF(csr_cs_scan_dirs_async(context, dirs, sizeof(dirs) / sizeof(const char *), nullptr),
	//	CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END() // CS

BOOST_AUTO_TEST_SUITE(WP)

BOOST_AUTO_TEST_CASE(fields_getters)
{
	EXCEPTION_GUARD_START

	Engine e(CSR_ENGINE_WP);

	Test::ScopedCstr vendor;
	ASSERT_IF(csr_engine_get_vendor(e.get(), &vendor.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_VENDOR"), vendor.ptr);

	Test::ScopedCstr name;
	ASSERT_IF(csr_engine_get_name(e.get(), &name.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("TEST_LOCAL_TWP_ENGINE"), name.ptr);

	Test::ScopedCstr version;
	ASSERT_IF(csr_engine_get_version(e.get(), &version.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), version.ptr);

	Test::ScopedCstr dataVersion;
	ASSERT_IF(csr_engine_get_version(e.get(), &dataVersion.ptr), CSR_ERROR_NONE);
	ASSERT_IF(std::string("0.0.1"), dataVersion.ptr);

	csr_activated_e activated;
	ASSERT_IF(csr_engine_get_activated(e.get(), &activated), CSR_ERROR_NONE);
	ASSERT_IF(activated, CSR_ACTIVATED);

	csr_state_e state = CSR_ENABLE;
	ASSERT_IF(csr_engine_set_state(e.get(), state), CSR_ERROR_NONE);

	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_ENABLE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(set_state)
{
	EXCEPTION_GUARD_START

	Engine e(CSR_ENGINE_WP);

	csr_state_e state = CSR_ENABLE;

	// enable
	ASSERT_IF(csr_engine_set_state(e.get(), CSR_ENABLE), CSR_ERROR_NONE);
	ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
	ASSERT_IF(state, CSR_ENABLE);

        // prepare data
	auto c = Test::Context<csr_wp_context_h>();
	auto context = c.get();
	csr_wp_check_result_h result;

        // check if engine is working well
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_NONE);

        // disable
        ASSERT_IF(csr_engine_set_state(e.get(), CSR_DISABLE), CSR_ERROR_NONE);
        ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
        ASSERT_IF(state, CSR_DISABLE);

        // test operation
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_ENGINE_DISABLED);

        // enable
        ASSERT_IF(csr_engine_set_state(e.get(), CSR_ENABLE), CSR_ERROR_NONE);
        ASSERT_IF(csr_engine_get_state(e.get(), &state), CSR_ERROR_NONE);
        ASSERT_IF(state, CSR_ENABLE);

        // test operation
	ASSERT_IF(csr_wp_check_url(context, RISK_HIGH_URL, &result), CSR_ERROR_NONE);

	EXCEPTION_GUARD_END
}


BOOST_AUTO_TEST_SUITE_END() // WP
BOOST_AUTO_TEST_SUITE_END() // API_ENGINE_MANAGER
