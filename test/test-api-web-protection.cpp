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
 * @file        test-api-web-protection.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Web protection API test
 */
#include <csr/web-protection.h>

#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(API_WEB_PROTECTION)

BOOST_AUTO_TEST_CASE(context_create_destroy)
{
	csr_wp_context_h handle;
	int ret = CSR_ERROR_UNKNOWN;

	BOOST_REQUIRE_NO_THROW(ret = csr_wp_context_create(&handle));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);

	BOOST_REQUIRE_NO_THROW(ret = csr_wp_context_destroy(handle));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(check_url)
{
	csr_wp_context_h handle;
	int ret = CSR_ERROR_UNKNOWN;

	BOOST_REQUIRE_NO_THROW(ret = csr_wp_context_create(&handle));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);

	csr_wp_check_result_h result;
	BOOST_REQUIRE_NO_THROW(ret = csr_wp_check_url(handle, "dummy/url/test", &result));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);

	BOOST_REQUIRE_NO_THROW(ret = csr_wp_context_destroy(handle));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
}

BOOST_AUTO_TEST_SUITE_END()
