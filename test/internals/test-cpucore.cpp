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
 * @file        test-cpucore.cpp
 * @author      Dongsun Lee(ds73.lee@samsung.com)
 * @version     1.0
 * @brief       cpu core internal test
 */
#include "service/core-usage.h"

#include <iostream>
#include <fstream>
#include <string>
#include <boost/test/unit_test.hpp>

#include "test-common.h"

BOOST_AUTO_TEST_SUITE(CPU_CORE)

BOOST_AUTO_TEST_CASE(set_core_usage)
{
	EXCEPTION_GUARD_START

	auto inst = Csr::CpuUsageManager::getInstance();

	int total = inst->getCoreCnt();

	ASSERT_IF(inst->setThreadCoreUsage(Csr::USAGE_SINGLE), true);
	ASSERT_IF(total, inst->getCoreCnt());
	ASSERT_IF((1 * 100) / total, inst->getCoreUsage());

	ASSERT_IF(inst->setThreadCoreUsage(Csr::USAGE_HALF), true);
	ASSERT_IF(total, inst->getCoreCnt());
	ASSERT_IF(((total > 1) ? (total / 2) : 1) * 100 / total, inst->getCoreUsage());

	ASSERT_IF(inst->setThreadCoreUsage(Csr::USAGE_FULL), true);
	ASSERT_IF(total, inst->getCoreCnt());
	ASSERT_IF(100, inst->getCoreUsage());

	ASSERT_IF(inst->setThreadCoreUsage(Csr::USAGE_DEFAULT), true);
	ASSERT_IF(total, inst->getCoreCnt());
	ASSERT_IF(100, inst->getCoreUsage());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
