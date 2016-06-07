
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
 * @file        core-usage.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/core-usage.h"

#include <sched.h>
#include <unistd.h>

#include "common/exception.h"

namespace Csr {

namespace {

const int TotalCoreNum = ::sysconf(_SC_NPROCESSORS_ONLN);

} // namespace anonymous

void CpuUsageManager::set(const csr_cs_core_usage_e &cu)
{
	switch (cu) {
	case CSR_CS_CORE_USAGE_HALF:
		CpuUsageManager::setRunningCores(TotalCoreNum / 2);
		break;

	case CSR_CS_CORE_USAGE_SINGLE:
		CpuUsageManager::setRunningCores(CpuUsageManager::MinCoreNum);
		break;

	case CSR_CS_CORE_USAGE_DEFAULT:
	case CSR_CS_CORE_USAGE_ALL:
		CpuUsageManager::reset();
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "invalid core usage param: " << static_cast<int>(cu));
	}
}

void CpuUsageManager::reset(void)
{
	if (CpuUsageManager::getRunningCores() != TotalCoreNum)
		CpuUsageManager::setRunningCores(TotalCoreNum);
}

void CpuUsageManager::setRunningCores(int num)
{
	num = (num < CpuUsageManager::MinCoreNum) ? CpuUsageManager::MinCoreNum : num;

	cpu_set_t set;

	CPU_ZERO(&set);

	for (int coreId = 0; coreId < num; ++coreId)
		CPU_SET(coreId, &set);

	if (::sched_setaffinity(0, sizeof(set), &set) == -1)
		ThrowExc(CSR_ERROR_SERVER, "sched set affinity failed. errno: " << errno);
}

int CpuUsageManager::getRunningCores(void)
{
	cpu_set_t set;

	CPU_ZERO(&set);

	if (::sched_getaffinity(0, sizeof(set), &set) == -1)
		ThrowExc(CSR_ERROR_SERVER, "sched get affinity failed. errno: " << errno);

	return CPU_COUNT(&set);
}

}
