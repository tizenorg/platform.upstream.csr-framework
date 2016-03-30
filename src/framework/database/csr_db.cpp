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

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "database/csr_db.h"
#include "database/connection.h"
#include "database/statement.h"
#include "database/column.h"

namespace Csr {

namespace Database {

enum DBVersion : int {
	DB_VERSION_1                   = 1,
	DB_VERSION_CURRENT             = 1
};

const char *SCRIPT_CREATE_SCHEMA                = "create_schema";
const char *SCRIPT_DROP_ALL_ITEMS               = "drop_all";
const char *SCRIPT_MIGRATE                      = "migrate_";

const std::string DB_VERSION_STR = "DB_VERSION";

//===========================================================================
// Queries
//===========================================================================

const std::string QUERY_SEL_SCHEMA_INFO =
	"select value from SCHEMA_INFO where name = ?";

const std::string QUERY_INS_SCHEMA_INFO =
	"insert or replace into SCHEMA_INFO (name, value) values (?, ?)";

const std::string QUERY_SEL_ENGINE_STATE =
	"select state from ENGINE_STATE where engine_id = ?";

const std::string QUERY_INS_ENGINE_STATE =
	"insert or replace into ENGINE_STATE (engine_id, state) values (?, ?)";

const std::string QUERY_SEL_SCAN_REQUEST =
	"select dir, last_scan from SCAN_REQUEST where dir = ? and data_version = ?";

const std::string QUERY_INS_SCAN_REQUEST =
	"insert or replace into SCAN_REQUEST (dir, last_scan, data_version) "
	"values (?, ?, ?)";

const std::string QUERY_DEL_SCAN_REQUEST_BY_DIR =
	"delete from SCAN_REQUEST where dir = ?";

const std::string QUERY_DEL_SCAN_REQUEST =
	"delete from SCAN_REQUEST ";


const std::string QUERY_SEL_DETECTED_BY_DIR =
	"SELECT path, data_version, "
	" severity_level, threat_type, malware_name, "
	" detailed_url, detected_time, modified_time, ignored "
	" FROM detected_malware_file where path like ? || '%' ";

const std::string QUERY_SEL_DETECTED_BY_PATH =
	"SELECT path, data_version, "
	" severity_level, threat_type, malware_name, "
	" detailed_url, detected_time, modified_time, ignored "
	" FROM detected_malware_file where path = ? ";

const std::string QUERY_INS_DETECTED =
	"insert or replace into DETECTED_MALWARE_FILE "
	" (path, data_version, severity_level, threat_type, malware_name, "
	" detailed_url, detected_time, modified_time, ignored) "
	" values (?, ?, ?, ?, ?, ?, ?, ?, ?)";

const std::string QUERY_UPD_DETECTED_INGNORED =
	"update DETECTED_MALWARE_FILE set ignored = ? where path = ?";

const std::string QUERY_DEL_DETECTED_BY_PATH =
	"delete from DETECTED_MALWARE_FILE where path = ?";

const std::string QUERY_DEL_DETECTED_DEPRECATED =
	"delete from DETECTED_MALWARE_FILE where path like ? || '%' "
	" and data_version != ?";


//===========================================================================
// Constructor
//===========================================================================

CsrDB::CsrDB(const std::string &dbfile, const std::string &scriptsDirectory)
	: conn(dbfile, Connection::Create | Connection::ReadWrite | Connection::Serialized),
		scriptsDir(scriptsDirectory)
{
	initDatabase();
	conn.exec("VACUUM;");
}

CsrDB::~CsrDB()
{
}

//===========================================================================
// SCHEMA_INFO table
//===========================================================================


bool CsrDB::initDatabase() noexcept {
	// run migration if old database is present
	int schemaVersion;
	if ((schemaVersion = getDbVersion()) <= 0  || schemaVersion > DB_VERSION_CURRENT) {
		// DB empty or corrupted or too new scheme
		// LogDebug("no database or database corrupted, initializing the DB");
		resetDatabase();
	} else if (schemaVersion < DB_VERSION_CURRENT) {
		// migration needed
		//LogDebug("DB migration from version " << schemaVersion << " to version " << DB_VERSION_CURRENT << " started.");
		for (int vi = schemaVersion; vi < DB_VERSION_CURRENT; vi++) {
			std::string script = getMigrationScript(vi);
			if (script.size() <= 0) {
				//LogWarn("script to migrate database from version: " << vi <<
				//		" to version: " << vi+1 << " not available, resetting the DB");
				resetDatabase();
				break;
			}

			//LogInfo("migrating from version " << vi << " to version " << vi+1);
			if (!conn.exec(script.c_str())) {
				//LogWarn("Error, migration database from version: " << vi <<
				//		" to version: " << vi+1 << " failed, resetting the DB");
				resetDatabase();
				break;
			}
		}
		// update DB version info
		if (!setDbVersion(DB_VERSION_CURRENT)) {
			//LogWarn("setting db version failed. version=: " << DB_VERSION_CURRENT <<", resetting the DB");
			resetDatabase();
		}
	}

	return true;
}

bool CsrDB::createSchema() noexcept {
	try {
		std::string script = getScript(SCRIPT_CREATE_SCHEMA);
		if (script.size() <= 0) {
			std::string errmsg = "Can not create the database schema: no initialization script";
			throw std::runtime_error(errmsg);
		}
		return conn.exec(script.c_str());
	} catch (std::exception &e) {
		//LogWarn("createSchema failed.error_msg=" << e.what());
		return false;
	}
}

bool CsrDB::resetDatabase() noexcept {
	try {
		std::string script = getScript(SCRIPT_DROP_ALL_ITEMS);
		if (script.size() <= 0) {
			std::string errmsg = "Can not clear the database: no clearing script";
			throw std::runtime_error(errmsg);
		}
		conn.exec(script.c_str());
		return createSchema();
	} catch (std::exception &e) {
		//LogWarn("resetDatabase failed.error_msg=" << e.what());
		return false;
	}
}

std::string CsrDB::getMigrationScript(int dbVersion)
{
	std::string scriptPath = std::string(SCRIPT_MIGRATE) + std::to_string(dbVersion);
	return getScript(scriptPath);
}

std::string CsrDB::getScript(std::string scriptName)
{
	std::string scriptPath = scriptsDir + std::string("/") + scriptName + std::string(".sql");
	std::ifstream is(scriptPath);
	if (is.fail()) {
		return std::string();
	}
	std::istreambuf_iterator<char> begin(is), end;
	return std::string(begin, end);
}

int CsrDB::getDbVersion() noexcept {
	try{
		Statement stmt(conn, QUERY_SEL_SCHEMA_INFO);

		int idx = 0;
		stmt.bind(++idx, DB_VERSION_STR);

		if (!stmt.step())
			return -1;

		return stmt.getColumn(0).getInt();
	} catch (std::exception &e) {
		//LogWarn("getDbVersion failed.error_msg=" << e.what());
		return -1;
	}
}

bool CsrDB::setDbVersion(int version) noexcept {
	try {
		Statement stmt(conn, QUERY_INS_SCHEMA_INFO);

		int idx = 0;
		stmt.bind(++idx, DB_VERSION_STR);
		stmt.bind(++idx, version);
		return stmt.exec();
	} catch (std::exception &e) {
		//LogWarn("setDbVersion failed.error_msg=" << e.what());
		return false;
	}
}

//===========================================================================
// ENGINE_STATE table
//===========================================================================

int CsrDB::getEngineState(int engineId) noexcept {
	try{
		Statement stmt(conn, QUERY_SEL_ENGINE_STATE);

		int idx = 0;
		stmt.bind(++idx, engineId);

		if (!stmt.step())
			return -1;

		return stmt.getColumn(0).getInt();
	} catch (std::exception &e) {
		//LogWarn("getEngineState failed.error_msg=" << e.what());
		return -1;
	}
}

bool CsrDB::setEngineState(int engineId, int state) noexcept {
	try {
		Statement stmt(conn, QUERY_INS_ENGINE_STATE);

		int idx = 0;
		stmt.bind(++idx, engineId);
		stmt.bind(++idx, state);
		return stmt.exec();
	} catch (std::exception &e) {
		//LogWarn("setEngineState failed.error_msg=" << e.what());
		return false;
	}
}

//===========================================================================
// SCAN_REQUEST table
//===========================================================================

long CsrDB::getLastScanTime(const std::string &dir, const std::string &dataVersion) noexcept {
	try{
		Statement stmt(conn, QUERY_SEL_SCAN_REQUEST);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.bind(++idx, dataVersion);

		if (!stmt.step())
			return -1;

		return static_cast<long>(stmt.getColumn(1).getInt64());
	} catch (std::exception &e) {
		//LogWarn("getLastScanTime failed.error_msg=" << e.what());
		return -1;
	}
}

bool CsrDB::insertLastScanTime(const std::string &dir, long scanTime, const std::string &dataVersion) noexcept {
	try {
		Statement stmt(conn, QUERY_INS_SCAN_REQUEST);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.bind(++idx, static_cast<sqlite3_int64>(scanTime));
		stmt.bind(++idx, dataVersion);
		return stmt.exec();
	} catch (std::exception &e) {
		//LogWarn("insertLastScanTime failed.error_msg=" << e.what());
		return false;
	}
}

bool CsrDB::deleteLastScanTime(const std::string &dir) noexcept {
	try {
		Statement stmt(conn, QUERY_DEL_SCAN_REQUEST_BY_DIR);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (std::exception &e) {
		//LogWarn("deleteLastScanTime failed.error_msg=" << e.what());
		return false;
	}
}

bool CsrDB::cleanLastScanTime() noexcept {
	try {
		Statement stmt(conn, QUERY_DEL_SCAN_REQUEST);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (std::exception &e) {
		//LogWarn("cleanLastScanTime failed.error_msg=" << e.what());
		return false;
	}
}

//===========================================================================
// DETECTED_MALWARE_FILE table
//===========================================================================
DetectedListShrPtr CsrDB::getDetectedMalwares(const std::string &dir) noexcept {
	DetectedListShrPtr detectedList = std::make_shared<std::vector<DetectedShrPtr>>();

	try {
		Statement stmt(conn, QUERY_SEL_DETECTED_BY_DIR);

		int idx = 0;
		stmt.bind(++idx, dir);

		while (stmt.step()) {
			DetectedShrPtr detected = std::make_shared<RowDetected>();
			idx = -1;
			detected->path = std::string(stmt.getColumn(++idx).getText());
			detected->dataVersion = std::string(stmt.getColumn(++idx).getText());
			detected->severityLevel = stmt.getColumn(++idx).getInt();
			detected->threatType = stmt.getColumn(++idx).getInt();
			detected->name = std::string(stmt.getColumn(++idx).getText());
			detected->detailedUrl = std::string(stmt.getColumn(++idx).getText());
			detected->detected_time = static_cast<long>(stmt.getColumn(++idx).getInt64());
			detected->modified_time = static_cast<long>(stmt.getColumn(++idx).getInt64());
			detected->ignored = stmt.getColumn(++idx).getInt();

			detectedList->push_back(detected);
		}
	} catch (std::exception &e) {
		//LogWarn("getDetectedMalwares failed.error_msg=" << e.what());
	}
	return detectedList;
}

DetectedShrPtr CsrDB::getDetectedMalware(const std::string &path) noexcept {
	DetectedShrPtr detected = std::make_shared<RowDetected>();

	try {
		Statement stmt(conn, QUERY_SEL_DETECTED_BY_PATH);

		int idx = 0;
		stmt.bind(++idx, path);

		if (!stmt.step())
			return nullptr;

		idx = -1;
		detected->path = std::string(stmt.getColumn(++idx).getText());
		detected->dataVersion = std::string(stmt.getColumn(++idx).getText());
		detected->severityLevel = stmt.getColumn(++idx).getInt();
		detected->threatType = stmt.getColumn(++idx).getInt();
		detected->name = std::string(stmt.getColumn(++idx).getText());
		detected->detailedUrl = std::string(stmt.getColumn(++idx).getText());
		detected->detected_time = static_cast<long>(stmt.getColumn(++idx).getInt64());
		detected->modified_time = static_cast<long>(stmt.getColumn(++idx).getInt64());
		detected->ignored = stmt.getColumn(++idx).getInt();

	} catch (std::exception &e) {
		//LogWarn("getDetectedMalware failed.error_msg=" << e.what());
		return nullptr;
	}
	return detected;
}

bool CsrDB::insertDetectedMalware(const RowDetected &malware) noexcept {
	try {
		Statement stmt(conn, QUERY_INS_DETECTED);

		int idx = 0;
		stmt.bind(++idx, malware.path);
		stmt.bind(++idx, malware.dataVersion);
		stmt.bind(++idx, malware.severityLevel);
		stmt.bind(++idx, malware.threatType);
		stmt.bind(++idx, malware.name);
		stmt.bind(++idx, malware.detailedUrl);
		stmt.bind(++idx, static_cast<sqlite3_int64>(malware.detected_time));
		stmt.bind(++idx, static_cast<sqlite3_int64>(malware.modified_time));
		stmt.bind(++idx, malware.ignored);

		return stmt.exec();
	} catch (std::exception &e) {
		//LogWarn("insertDetectedMalware failed.error_msg=" << e.what());
		return false;
	}
}

bool CsrDB::setDetectedMalwareIgnored(const std::string &path, int ignored) noexcept {
	try {
		Statement stmt(conn, QUERY_UPD_DETECTED_INGNORED);

		int idx = 0;
		stmt.bind(++idx, ignored);
		stmt.bind(++idx, path);

		return stmt.exec();
	} catch (std::exception &e) {
		//LogWarn("setDetectedMalwareIgnored failed.error_msg=" << e.what());
		return false;
	}
}

bool CsrDB::deleteDetecedMalware(const std::string &path) noexcept {
	try {
		Statement stmt(conn, QUERY_DEL_DETECTED_BY_PATH);

		int idx = 0;
		stmt.bind(++idx, path);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (std::exception &e) {
		//LogWarn("deleteDetecedMalware failed.error_msg=" << e.what());
		return false;
	}
}

bool CsrDB::deleteDeprecatedDetecedMalwares(const std::string &dir, const std::string &dataVersion) noexcept {
	try {
		Statement stmt(conn, QUERY_DEL_DETECTED_DEPRECATED);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.bind(++idx, dataVersion);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (std::exception &e) {
		//LogWarn("deleteDeprecatedDetecedMalwares failed.error_msg=" << e.what());
		return false;
	}
}


}// namespace Database

}// namespace Csr
