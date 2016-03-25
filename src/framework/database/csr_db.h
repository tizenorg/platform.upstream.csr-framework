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

#ifndef CONTENT_SCANNING_DB_HXX_
#define CONTENT_SCANNING_DB_HXX_

#include <string>
#include <vector>
#include <memory>

#include "database/connection.h"

namespace Csr {

namespace Database {

struct RowDetected {
	std::string path;
	std::string dataVersion; // engine's data version
	int severityLevel;
	int threatType;
	std::string name;
	std::string detailedUrl;
	long detected_time;
	long modified_time;
	int ignored;

	RowDetected() : path(""), dataVersion(""),
		severityLevel(-1), threatType(-1),
		name(""), detailedUrl(""), detected_time(-1),
		modified_time(-1), ignored(-1) {};
	~RowDetected() {};
};

typedef std::shared_ptr<RowDetected> DetectedShrPtr;
typedef std::shared_ptr<std::vector<DetectedShrPtr>> DetectedListShrPtr;

class CsrDB {
public:
	CsrDB(const std::string &dbfile, const std::string &scriptsDir);
	virtual ~CsrDB();

	// SCHEMA_INFO
	bool initDatabase()  noexcept;
	bool createSchema()  noexcept;
	bool resetDatabase() noexcept;
	std::string getMigrationScript(int dbVersion);
	std::string getScript(std::string scriptName);
	int getDbVersion() noexcept;
	bool setDbVersion(int version) noexcept;

	// ENGINE_STATE
	int getEngineState(int engineId) noexcept;
	bool setEngineState(int engineId, int state) noexcept;

	// SCAN_REQUEST
	long getLastScanTime(const std::string &dir, const std::string &dataVersion)  noexcept;
	bool insertLastScanTime(const std::string &dir, long scanTime, const std::string &dataVersion)  noexcept;
	bool deleteLastScanTime(const std::string &dir)  noexcept;
	bool cleanLastScanTime()  noexcept;

	// DETECTED_MALWARE_FILE & USER_RESPONSE
	DetectedListShrPtr getDetectedMalwares(const std::string &dir)  noexcept;
	DetectedShrPtr getDetectedMalware(const std::string &path)  noexcept;
	bool insertDetectedMalware(const RowDetected &malware)  noexcept;
	bool setDetectedMalwareIgnored(const std::string &path, int userResponse)  noexcept;
	bool deleteDetecedMalware(const std::string &path) noexcept;
	bool deleteDeprecatedDetecedMalwares(const std::string &dir, const std::string &dataVersion)  noexcept;

private:
	Connection conn;
	std::string scriptsDir;
};

}// namespace Database


}// namespace Csr
#endif /* CONTENT_SCANNING_DB_HXX_ */
