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
 * @file        test-api.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#define BOOST_TEST_MODULE CSR_API_TEST
#include <csr/api.h>

#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(API_TEST)

BOOST_AUTO_TEST_CASE(FILE_SCAN)
{
	std::string filepath = "this is not real file path";
	int ret = CSR_ERROR_UNKNOWN;

	BOOST_REQUIRE_NO_THROW(ret = csr_file_scan(filepath.c_str()));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
}

BOOST_AUTO_TEST_CASE(FILE_JUDGE)
{
	std::string filepath = "this is not real file path";
	int ret = CSR_ERROR_UNKNOWN;
	int judge = 1;

	BOOST_REQUIRE_NO_THROW(ret = csr_file_judge(filepath.c_str(), judge));
	BOOST_REQUIRE(ret == CSR_ERROR_NONE);
}

BOOST_AUTO_TEST_SUITE_END()
