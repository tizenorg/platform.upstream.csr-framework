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

#include <algorithm>
#include <system_error>
#include <boost/test/unit_test.hpp>

#include "test-common.h"

namespace {

int getRunningCores(void)
{
	cpu_set_t cores;

	CPU_ZERO(&cores);

	if (sched_getaffinity(0, sizeof(cores), &cores) == -1)
		throw std::system_error(errno, std::generic_category(),
								"failed to sched_getaffinity");

	return CPU_COUNT(&cores);
}

void coreNumTest(const csr_cs_core_usage_e &cu, int expected)
{
	int total = ::getRunningCores();

	Csr::CpuUsageManager::set(cu);
	ASSERT_IF(::getRunningCores(), expected);
	Csr::CpuUsageManager::reset();
	ASSERT_IF(::getRunningCores(), total);
}

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(CPU_CORE)

BOOST_AUTO_TEST_CASE(single)
{
	EXCEPTION_GUARD_START

	coreNumTest(CSR_CS_CORE_USAGE_SINGLE, Csr::CpuUsageManager::MinCoreNum);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(half)
{
	EXCEPTION_GUARD_START

	int expected = std::max(::getRunningCores() / 2, Csr::CpuUsageManager::MinCoreNum);

	coreNumTest(CSR_CS_CORE_USAGE_HALF, expected);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(default_)
{
	EXCEPTION_GUARD_START

	int total = ::getRunningCores();

	coreNumTest(CSR_CS_CORE_USAGE_DEFAULT, total);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(all)
{
	EXCEPTION_GUARD_START

	int total = ::getRunningCores();

	coreNumTest(CSR_CS_CORE_USAGE_ALL, total);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
