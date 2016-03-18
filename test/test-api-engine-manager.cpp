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

BOOST_AUTO_TEST_SUITE(API_ENGINE_MANAGER)

BOOST_AUTO_TEST_CASE(get_selected_engine)
{
	int ret = CSR_ERROR_UNKNOWN;

	csr_engine_h handle;
	BOOST_REQUIRE_NO_THROW(ret = csr_get_selected_engine(CSR_ENGINE_CS, &handle));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(get_name)
{
	int ret = CSR_ERROR_UNKNOWN;

	csr_engine_h handle;
	BOOST_REQUIRE_NO_THROW(ret = csr_get_selected_engine(CSR_ENGINE_CS, &handle));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);

	char *name = nullptr;
	BOOST_REQUIRE_NO_THROW(ret = csr_engine_get_name(handle, &name));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
}

BOOST_AUTO_TEST_SUITE_END()
