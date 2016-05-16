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
#include <map>
#include <mutex>

#include "db/connection.h"
#include "db/row.h"
#include "common/cs-detected.h"

#include "csr/engine-manager.h"

namespace Csr {
namespace Db {

class Manager {
public:
	Manager(const std::string &dbfile, const std::string &scriptsDir);
	virtual ~Manager();

	// SCHEMA_INFO. it's public only for testing for now...
	int getSchemaVersion();

	// ENGINE_STATE
	csr_state_e getEngineState(csr_engine_id_e);
	void setEngineState(csr_engine_id_e, csr_state_e);

	// SCAN_REQUEST
	time_t getLastScanTime(const std::string &dir, const std::string &dataVersion);
	void insertLastScanTime(const std::string &dir, time_t scanTime,
							const std::string &dataVersion);
	void deleteLastScanTime(const std::string &dir);
	void cleanLastScanTime();

	// DETECTED_MALWARE_FILE & USER_RESPONSE
	RowShPtrs getDetectedMalwares(const std::string &dirpath);
	RowShPtr getDetectedMalware(const std::string &filepath);
	void insertDetectedMalware(const CsDetected &, const std::string &dataVersion);
	void setDetectedMalwareIgnored(const std::string &path, bool flag);
	void deleteDetectedMalware(const std::string &path);
	void deleteDeprecatedDetectedMalwares(const std::string &dir,
										  const std::string &dataVersion);

private:
	void resetDatabase();
	bool isTableExist(const std::string &name);
	std::string getScript(const std::string &scriptName);
	std::string getMigrationScript(int schemaVersion);

	void setSchemaVersion(int schemaVersion);

	Csr::Db::Connection m_conn;
	std::string m_scriptsDir;

	std::map<csr_engine_id_e, csr_state_e> m_stateMap;
	std::mutex m_mutex;
};

} // namespace Db
} // namespace Csr
