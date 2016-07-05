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
 * @file        cache.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Cache for hold the data temporally before insert to DB
 */
#include "db/cache.h"

namespace Csr {
namespace Db {

Cache::Cache() noexcept : riskiestIndex(-1)
{
}

Cache::Cache(Cache &&rhs) noexcept :
	pkgPath(std::move(rhs.pkgPath)),
	pkgId(std::move(rhs.pkgId)),
	dataVersion(std::move(rhs.dataVersion)),
	detecteds(std::move(rhs.detecteds)),
	riskiestIndex(rhs.riskiestIndex),
	scanTime(rhs.scanTime)
{
}

Cache &Cache::operator=(Cache &&rhs) noexcept
{
	if (this == &rhs)
		return *this;

	this->pkgPath = std::move(rhs.pkgPath);
	this->pkgId = std::move(rhs.pkgId);
	this->dataVersion = std::move(rhs.dataVersion);
	this->detecteds = std::move(rhs.detecteds);
	this->riskiestIndex = rhs.riskiestIndex;
	this->scanTime = rhs.scanTime;

	return *this;
}

} // namespace Db
} // namespace Csr
