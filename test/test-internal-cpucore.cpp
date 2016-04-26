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
 * @file        test-internal-database.cpp
 * @author      Dongsun Lee(ds73.lee@samsung.com)
 * @version     1.0
 * @brief       CSR Content screening DB internal test
 */

#include "service/core-usage.h"
#include <iostream>
#include <fstream>
#include <string>

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(INTERNAL_CORE_USAGE)

BOOST_AUTO_TEST_CASE(set_core_usage)
{
	Csr::CpuUsageManager *inst = Csr::CpuUsageManager::getInstance();

	int total = inst->getCoreCnt();

	inst->setThreadCoreUsage(Csr::USAGE_SINGLE);
	BOOST_REQUIRE_MESSAGE(total == inst->getCoreCnt(),
						  "Failed. setThreadCoreUsage. Total=" << inst->getCoreCnt() << ", expectd=" <<
						  total);
	BOOST_REQUIRE_MESSAGE((1 * 100) / total == inst->getCoreUsage(),
						  "Failed. setThreadCoreUsage. Usage=" << inst->getCoreUsage() << ", expectd="
						  << (1 * 100) / total);

	inst->setThreadCoreUsage(Csr::USAGE_HALF);
	BOOST_REQUIRE_MESSAGE(total == inst->getCoreCnt(),
						  "Failed. setThreadCoreUsage. Total=" << inst->getCoreCnt() << ", expectd=" <<
						  total);
	BOOST_REQUIRE_MESSAGE(((total > 1) ? total / 2 : 1) * 100 / total ==
						  inst->getCoreUsage(),
						  "Failed. setThreadCoreUsage. Usage=" << inst->getCoreUsage()
						  << ", expectd=" << ((total > 1) ? total / 2 : 1) * 100 / total);

	inst->setThreadCoreUsage(Csr::USAGE_FULL);
	BOOST_REQUIRE_MESSAGE(total == inst->getCoreCnt(),
						  "Failed. setThreadCoreUsage. Total=" << inst->getCoreCnt() << ", expectd=" <<
						  total);
	BOOST_REQUIRE_MESSAGE(100 == inst->getCoreUsage(),
						  "Failed. setThreadCoreUsage. Usage=" << inst->getCoreUsage() << ", expectd=" <<
						  100);

	inst->setThreadCoreUsage(Csr::USAGE_DEFAULT);
	BOOST_REQUIRE_MESSAGE(total == inst->getCoreCnt(),
						  "Failed. setThreadCoreUsage. Total=" << inst->getCoreCnt() << ", expectd=" <<
						  total);
	BOOST_REQUIRE_MESSAGE(100 == inst->getCoreUsage(),
						  "Failed. setThreadCoreUsage. Usage=" << inst->getCoreUsage() << ", expectd=" <<
						  100);
}

BOOST_AUTO_TEST_SUITE_END()
