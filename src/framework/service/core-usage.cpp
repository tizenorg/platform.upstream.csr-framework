
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

#include <stdexcept>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "service/core-usage.h"


namespace Csr {

void CpuUsageManager::initialize()
{
	m_cores = getUsedCnt();
}

bool CpuUsageManager::setThreadCoreUsage(int percent)
{
	if (percent > 100 || percent <= 0)
		throw std::runtime_error("invalid core usage percent.");

	cpu_set_t set;
	CPU_ZERO(&set);
	int cnt = (m_cores * percent) / 100;

	if (cnt < MIN_CORE_USED)
		cnt = MIN_CORE_USED;

	for (int cpuid = m_cores - 1; cpuid >= (m_cores - cnt); cpuid--)
		CPU_SET(cpuid, &set);

	if (sched_setaffinity(0, sizeof(set), &set) != 0) {
		return false;
	}

	return true;
}

int CpuUsageManager::getCoreUsage() const
{
	return (getUsedCnt() * 100) / m_cores;
}

int CpuUsageManager::getCoreCnt() const
{
	return m_cores;
}

int CpuUsageManager::getUsedCnt()
{
	cpu_set_t usedCore;
	CPU_ZERO(&usedCore);

	if (sched_getaffinity(0, sizeof(cpu_set_t), &usedCore) != 0)
		throw std::runtime_error("sched_getaffinity failed.");

	return CPU_COUNT(&usedCore);
}

}// end of namespace Csr

