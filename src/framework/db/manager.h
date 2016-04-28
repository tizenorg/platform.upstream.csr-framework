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
 * @file        manager.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       DB manager to maintain scanning history
 */
#pragma once

#include <string>
#include <vector>
#include <memory>

#include "db/connection.h"
#include "db/row.h"
#include "common/cs-detected.h"

namespace Csr {
namespace Db {

class Manager {
public:
	Manager(const std::string &dbfile, const std::string &scriptsDir);
	virtual ~Manager();

	// SCHEMA_INFO. it's public only for testing for now...
	int getSchemaVersion();

	// ENGINE_STATE
	int getEngineState(int engineId) noexcept;
	bool setEngineState(int engineId, int state) noexcept;

	// SCAN_REQUEST
	long getLastScanTime(const std::string &dir,
						 const std::string &dataVersion) noexcept;
	bool insertLastScanTime(const std::string &dir, long scanTime,
							const std::string &dataVersion) noexcept;
	bool deleteLastScanTime(const std::string &dir) noexcept;
	bool cleanLastScanTime() noexcept;

	// DETECTED_MALWARE_FILE & USER_RESPONSE
	RowsShPtr getDetectedMalwares(const std::string &dirpath) noexcept;
	RowShPtr getDetectedMalware(const std::string &filepath) noexcept;
	bool insertDetectedMalware(const CsDetected &, const std::string &dataVersion,
							   bool isIgnored) noexcept;
	bool setDetectedMalwareIgnored(const std::string &path, bool flag) noexcept;
	bool deleteDetectedMalware(const std::string &path) noexcept;
	bool deleteDeprecatedDetectedMalwares(const std::string &dir,
										  const std::string &dataVersion) noexcept;

private:
	void resetDatabase();
	bool isTableExist(const std::string &name);
	std::string getScript(const std::string &scriptName);
	std::string getMigrationScript(int schemaVersion);

	void setSchemaVersion(int schemaVersion);

	Csr::Db::Connection m_conn;
	std::string m_scriptsDir;
};

} // namespace Db
} // namespace Csr
