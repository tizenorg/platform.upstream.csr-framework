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
 * @file        manager.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       DB manager to maintain scanning history
 */
#include "db/manager.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "db/query.h"
#include "db/statement.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Db {

namespace {

enum SchemaVersion : int {
	NOT_EXIST = 0,
	_1        = 1,
	LATEST    = _1
};

const std::string SCRIPT_CREATE_SCHEMA  = "create_schema";
const std::string SCRIPT_DROP_ALL_ITEMS = "drop_all";
const std::string SCRIPT_MIGRATE        = "migrate_";

const std::string DB_VERSION_STR  = "DB_VERSION";

} // namespace anonymous

Manager::Manager(const std::string &dbfile, const std::string &scriptsDir) :
	m_conn(dbfile, Connection::Create | Connection::ReadWrite |
		   Connection::Serialized),
	m_scriptsDir(scriptsDir)
{
	initDatabase();
	m_conn.exec("VACUUM;");
}

Manager::~Manager() {}

void Manager::initDatabase()
{
	// run migration if old database is present
	auto sv = getSchemaVersion();

	if (sv < SchemaVersion::NOT_EXIST || sv > SchemaVersion::LATEST) {
		ERROR("Database corrupted! invalid db version returned! : " << sv);
		resetDatabase();
		return;
	} else if (sv == SchemaVersion::LATEST) {
		DEBUG("Database version is latest");
		return;
	}

	if (sv == SchemaVersion::NOT_EXIST) {
		INFO("Database initializing!");
		resetDatabase();
	} else if (sv < SchemaVersion::LATEST) {
		INFO("Database migration! from[" << sv <<
			 "] to[" << SchemaVersion::LATEST << "]");

		for (int vi = sv; vi < SchemaVersion::LATEST; ++vi)
			m_conn.exec(getMigrationScript(vi).c_str());

		setSchemaVersion(SchemaVersion::LATEST);
	}
}

void Manager::resetDatabase()
{
	m_conn.exec(getScript(SCRIPT_DROP_ALL_ITEMS).c_str());
	m_conn.exec(getScript(SCRIPT_CREATE_SCHEMA).c_str());
	setSchemaVersion(SchemaVersion::LATEST);
}

std::string Manager::getMigrationScript(int sv)
{
	return getScript(SCRIPT_MIGRATE + std::to_string(sv));
}

std::string Manager::getScript(const std::string &scriptName)
{
	auto scriptPath = m_scriptsDir + std::string("/") + scriptName + ".sql";
	std::ifstream is(scriptPath);

	if (is.fail())
		throw std::runtime_error(FORMAT("Cannot open script: " << scriptPath));

	std::istreambuf_iterator<char> begin(is), end;
	auto str = std::string(begin, end);

	if (str.empty())
		throw std::runtime_error(FORMAT("Script file empty: " << scriptPath));

	return str;
}

int Manager::getSchemaVersion()
{
	Statement stmt(m_conn, Query::SEL_SCHEMA_INFO);

	int idx = 0;
	stmt.bind(++idx, DB_VERSION_STR);

	if (!stmt.step()) // Tables don't exist yet
		return SchemaVersion::NOT_EXIST;

	return stmt.getInt(0);
}

void Manager::setSchemaVersion(int sv)
{
	Statement stmt(m_conn, Query::INS_SCHEMA_INFO);

	int idx = 0;
	stmt.bind(++idx, DB_VERSION_STR);
	stmt.bind(++idx, sv);

	if (stmt.exec() == 0)
		throw std::runtime_error(FORMAT("Failed to set schema version!"));
}

//===========================================================================
// ENGINE_STATE table
//===========================================================================
int Manager::getEngineState(int engineId) noexcept
{
	try {
		Statement stmt(m_conn, Query::SEL_ENGINE_STATE);

		int idx = 0;
		stmt.bind(++idx, engineId);

		if (!stmt.step())
			return -1;

		return stmt.getInt(0);
	} catch (const std::exception &e) {
		ERROR("getEngineState failed. error_msg=" << e.what());
		return -1;
	}
}

bool Manager::setEngineState(int engineId, int state) noexcept
{
	try {
		Statement stmt(m_conn, Query::INS_ENGINE_STATE);

		int idx = 0;
		stmt.bind(++idx, engineId);
		stmt.bind(++idx, state);
		return stmt.exec();
	} catch (const std::exception &e) {
		ERROR("setEngineState failed. error_msg=" << e.what());
		return false;
	}
}

//===========================================================================
// SCAN_REQUEST table
//===========================================================================

long Manager::getLastScanTime(const std::string &dir,
							  const std::string &dataVersion) noexcept
{
	try {
		Statement stmt(m_conn, Query::SEL_SCAN_REQUEST);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.bind(++idx, dataVersion);

		if (!stmt.step())
			return -1;

		return static_cast<long>(stmt.getInt64(1));
	} catch (const std::exception &e) {
		ERROR("getLastScanTime failed. error_msg=" << e.what());
		return -1;
	}
}

bool Manager::insertLastScanTime(const std::string &dir, long scanTime,
								 const std::string &dataVersion) noexcept
{
	try {
		Statement stmt(m_conn, Query::INS_SCAN_REQUEST);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.bind(++idx, static_cast<sqlite3_int64>(scanTime));
		stmt.bind(++idx, dataVersion);
		return stmt.exec();
	} catch (const std::exception &e) {
		ERROR("insertLastScanTime failed. error_msg=" << e.what());
		return false;
	}
}

bool Manager::deleteLastScanTime(const std::string &dir) noexcept
{
	try {
		Statement stmt(m_conn, Query::DEL_SCAN_REQUEST_BY_DIR);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (const std::exception &e) {
		ERROR("deleteLastScanTime failed. error_msg=" << e.what());
		return false;
	}
}

bool Manager::cleanLastScanTime() noexcept
{
	try {
		Statement stmt(m_conn, Query::DEL_SCAN_REQUEST);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (const std::exception &e) {
		ERROR("cleanLastScanTime failed. error_msg=" << e.what());
		return false;
	}
}

//===========================================================================
// DETECTED_MALWARE_FILE table
//===========================================================================
DetectedListShrPtr Manager::getDetectedMalwares(const std::string &dir) noexcept
{
	try {
		DetectedListShrPtr detectedList =
			std::make_shared<std::vector<DetectedShrPtr>>();

		Statement stmt(m_conn, Query::SEL_DETECTED_BY_DIR);

		int idx = 0;
		stmt.bind(++idx, dir);

		while (stmt.step()) {
			DetectedShrPtr detected = std::make_shared<RowDetected>();
			idx = -1;
			detected->path = stmt.getText(++idx);
			detected->dataVersion = stmt.getText(++idx);
			detected->severityLevel = stmt.getInt(++idx);
			detected->threatType = stmt.getInt(++idx);
			detected->name = stmt.getText(++idx);
			detected->detailedUrl = stmt.getText(++idx);
			detected->detected_time = static_cast<long>(stmt.getInt64(++idx));
			detected->modified_time = static_cast<long>(stmt.getInt64(++idx));
			detected->ignored = stmt.getInt(++idx);

			detectedList->push_back(detected);
		}

		return detectedList;
	} catch (const std::exception &e) {
		ERROR("getDetectedMalwares failed. error_msg=" << e.what());
		return nullptr;
	}

}

DetectedShrPtr Manager::getDetectedMalware(const std::string &path) noexcept
{
	try {
		DetectedShrPtr detected = std::make_shared<RowDetected>();
		Statement stmt(m_conn, Query::SEL_DETECTED_BY_PATH);

		int idx = 0;
		stmt.bind(++idx, path);

		if (!stmt.step())
			return nullptr;

		idx = -1;
		detected->path = stmt.getText(++idx);
		detected->dataVersion = stmt.getText(++idx);
		detected->severityLevel = stmt.getInt(++idx);
		detected->threatType = stmt.getInt(++idx);
		detected->name = stmt.getText(++idx);
		detected->detailedUrl = stmt.getText(++idx);
		detected->detected_time = static_cast<long>(stmt.getInt64(++idx));
		detected->modified_time = static_cast<long>(stmt.getInt64(++idx));
		detected->ignored = stmt.getInt(++idx);

		return detected;
	} catch (const std::exception &e) {
		ERROR("getDetectedMalware failed. error_msg=" << e.what());
		return nullptr;
	}
}

bool Manager::insertDetectedMalware(const RowDetected &malware) noexcept
{
	try {
		Statement stmt(m_conn, Query::INS_DETECTED);

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
	} catch (const std::exception &e) {
		ERROR("insertDetectedMalware failed. error_msg=" << e.what());
		return false;
	}
}

bool Manager::setDetectedMalwareIgnored(const std::string &path,
										int ignored) noexcept
{
	try {
		Statement stmt(m_conn, Query::UPD_DETECTED_INGNORED);

		int idx = 0;
		stmt.bind(++idx, ignored);
		stmt.bind(++idx, path);

		return stmt.exec();
	} catch (const std::exception &e) {
		ERROR("setDetectedMalwareIgnored failed. error_msg=" << e.what());
		return false;
	}
}

bool Manager::deleteDetecedMalware(const std::string &path) noexcept
{
	try {
		Statement stmt(m_conn, Query::DEL_DETECTED_BY_PATH);

		int idx = 0;
		stmt.bind(++idx, path);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (const std::exception &e) {
		ERROR("deleteDetecedMalware failed.error_msg=" << e.what());
		return false;
	}
}

bool Manager::deleteDeprecatedDetecedMalwares(const std::string &dir,
		const std::string &dataVersion) noexcept
{
	try {
		Statement stmt(m_conn, Query::DEL_DETECTED_DEPRECATED);

		int idx = 0;
		stmt.bind(++idx, dir);
		stmt.bind(++idx, dataVersion);
		stmt.exec();
		return true; // even if no rows are deleted
	} catch (const std::exception &e) {
		ERROR("deleteDeprecatedDetecedMalwares failed.error_msg=" << e.what());
		return false;
	}
}

} // namespace Db
} // namespace Csr
