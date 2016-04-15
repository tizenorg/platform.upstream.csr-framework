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

namespace Csr {
namespace Db {

struct RowDetected {
	std::string path;
	std::string dataVersion; // engine's data version
	std::string name;
	std::string detailedUrl;
	int severityLevel;
	int threatType;
	int ignored;
	long detected_time;
	long modified_time;

	RowDetected() :
		severityLevel(-1),
		threatType(-1),
		ignored(-1),
		detected_time(-1),
		modified_time(-1) {}

	virtual ~RowDetected() {}
};

using DetectedShrPtr = std::shared_ptr<RowDetected>;
using DetectedListShrPtr = std::shared_ptr<std::vector<DetectedShrPtr>>;

class Manager {
public:
	Manager(const std::string &dbfile, const std::string &scriptsDir);
	virtual ~Manager();

	// SCHEMA_INFO
	int getSchemaVersion();

	// ENGINE_STATE
	int getEngineState(int engineId) noexcept;
	bool setEngineState(int engineId, int state) noexcept;

	// SCAN_REQUEST
	long getLastScanTime(const std::string &dir,
						 const std::string &dataVersion)  noexcept;
	bool insertLastScanTime(const std::string &dir, long scanTime,
							const std::string &dataVersion)  noexcept;
	bool deleteLastScanTime(const std::string &dir)  noexcept;
	bool cleanLastScanTime()  noexcept;

	// DETECTED_MALWARE_FILE & USER_RESPONSE
	DetectedListShrPtr getDetectedMalwares(const std::string &dir)  noexcept;
	DetectedShrPtr getDetectedMalware(const std::string &path)  noexcept;
	bool insertDetectedMalware(const RowDetected &malware)  noexcept;
	bool setDetectedMalwareIgnored(const std::string &path,
								   int userResponse)  noexcept;
	bool deleteDetecedMalware(const std::string &path) noexcept;
	bool deleteDeprecatedDetecedMalwares(const std::string &dir,
										 const std::string &dataVersion)  noexcept;

private:
	void initDatabase();
	void resetDatabase();
	std::string getScript(const std::string &scriptName);
	std::string getMigrationScript(int schemaVersion);

	void setSchemaVersion(int schemaVersion);

	Csr::Db::Connection m_conn;
	std::string m_scriptsDir;
};

} // namespace Db
} // namespace Csr
