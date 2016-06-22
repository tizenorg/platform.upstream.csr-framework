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
#include "common/audit/logger.h"

namespace Csr {
namespace Db {

Cache::Cache(Cache &&rhs) noexcept :
	pkgPath(std::move(rhs.pkgPath)),
	pkgId(std::move(rhs.pkgId)),
	dataVersion(std::move(rhs.dataVersion)),
	riskiestPath(std::move(rhs.riskiestPath)),
	filePaths(std::move(rhs.filePaths)),
	detecteds(std::move(rhs.detecteds)),
	riskiest(std::move(rhs.riskiest)),
	scanTime(rhs.scanTime)
{
	DEBUG("move constructor ---------------------");
	DEBUG(pkgPath);
	DEBUG(pkgId);
	DEBUG(dataVersion);
	DEBUG(riskiestPath);
	for(auto &p : filePaths)
		DEBUG(p);
	for(auto &d : detecteds)
		DEBUG(d.pkgId);
	DEBUG((*riskiest).pkgId);
}

Cache &Cache::operator=(Cache &&rhs) noexcept
{
	DEBUG("move assign ---------------------");
	if (this == &rhs)
		return *this;

	this->pkgPath = std::move(rhs.pkgPath);
	this->pkgId = std::move(rhs.pkgId);
	this->dataVersion = std::move(rhs.dataVersion);
	this->riskiestPath = std::move(rhs.riskiestPath);
	this->filePaths = std::move(rhs.filePaths);
	this->detecteds = std::move(rhs.detecteds);
	this->riskiest = std::move(rhs.riskiest);
	this->scanTime = rhs.scanTime;

	return *this;
}

} // namespace Db
} // namespace Csr
