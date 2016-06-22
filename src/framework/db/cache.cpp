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
#include "db/Cache.h"

namespace Csr {
namespace Db {

Cache::Cache(const std::string &pkgPath) :
	m_pkgPath(pkgPath), m_pkgId(pkgId), m_riskiest(nullptr)
{
}

void Cache::setLastScanTime(time_t scanTime) noexcept
{
	m_scanTime = scanTime;
}

void Cache::setDataVersion(const std::string &dataVersion) noexcept
{
	m_dataVersion = dataVersion;
}

void Cache::setDetectedFile(const std::string &filepath,
							const CsDetected &d) noexcept
{
	m_filePaths.emplace_back(std::move(filepath));
	m_detecteds.emplace_back(std::move(d));
}

void Cache::setRiskiest(const CsDetectedPtr &riskiest) noexcept
{
	m_riskiest = riskiest;
}

void Cache::setRiskiestPath(const std::string &path) noexcept
{
	m_riskiestPath = path;
}

} // namespace Db
} // namespace Csr
