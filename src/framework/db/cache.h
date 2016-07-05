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
 * @file        cache.h
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Cache for hold the data temporally before insert to DB
 */
#pragma once

#include <string>
#include <vector>
#include <utility>
#include <ctime>

#include "common/cs-detected.h"

namespace Csr {
namespace Db {

struct Cache {
public:
	// fileInAppPath and CsDetected struct
	using MalwareInApp = std::pair<std::string, CsDetectedPtr>;

	Cache() = default;
	virtual ~Cache() = default;

	Cache(const Cache &) = delete;
	Cache &operator=(const Cache &) = delete;
	Cache(Cache &&) noexcept;
	Cache &operator=(Cache &&) noexcept;

	std::string pkgPath;
	std::string pkgId;
	std::string dataVersion;
	std::vector<MalwareInApp> detecteds;
	int riskiestIndex;
	time_t scanTime;
};

} // namespace Db
} // namespace Csr
