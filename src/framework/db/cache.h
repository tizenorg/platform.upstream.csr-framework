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

#include "common/cs-detected.h"

namespace Csr {
namespace Db {

class Cache {
public:
	Cache(const std::string &pkgPath, const std::string &pkgId);
	virtual ~Cache() = default;

	// TODO implement below
	Cache(const Cache &) = delete;
	Cache &operator=(const Cache &) = delete;
	Cache(Cache &&) = delete;
	Cache &operator=(Cache &&) = delete;

/*
	void setLastScanTime(const time_t &scanTime) noexcept;
	void setDataVersion(const std::string &dataVersion) noexcept;
	void setDetectedFile(const std::string &filepath,
						 const CsDetected &d) noexcept;
	void setRiskiest(const CsDetectedPtr &riskiest) noexcept;
	void setRiskiestPath(const std::string &path) noexcept
	CsDectectedPtr& getRiskiest() noexcept;
*/
	friend class CsLogic;

private:
	std::string m_pkgPath;
	std::string m_pkgId;
	std::string m_dataVersion;
	std::string m_riskiestPath;
	std::vector<std::string> m_filePaths;
	std::vector<CsDetected> m_detecteds;
	time_t m_scanTime;
....CsDetectedPtr m_riskiest;
};

} // namespace Db
} // namespace Csr
