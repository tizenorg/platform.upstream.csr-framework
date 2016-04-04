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
 * @file        core-usage.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */

#pragma once

#define MIN_CORE_USED 1

namespace Csr {

enum CORE_USAGE : int {
	USAGE_SINGLE  = 1,
	USAGE_HALF    = 50,
	USAGE_FULL    = 100,
	USAGE_DEFAULT = 100
};


class CpuUsageManager {
public:
	static CpuUsageManager *getInstance() {
		static CpuUsageManager instance;
		return &instance;
	}

	bool setThreadCoreUsage(int percent);

	int getCoreUsage() const;
	int getCoreCnt() const;
private:
	int m_cores;

	CpuUsageManager() : m_cores(1) {
		initialize();
	};
	~CpuUsageManager() {};

	void initialize();
	static int getUsedCnt();
};

} // end of namespace Csr


