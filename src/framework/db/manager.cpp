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
	m_conn.exec("VACUUM;");
}

Manager::~Manager()
{
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
	try {
		// statement ctor throw exception if DB file isn't exist
		Statement stmt(m_conn, Query::SEL_SCHEMA_INFO);

		stmt.bind(DB_VERSION_STR);

		// step false if any row don't exist
		return stmt.step() ? stmt.getInt() : SchemaVersion::NOT_EXIST;
	} catch (const std::runtime_error &e) {
		INFO("Database doesn't exist.");
		return SchemaVersion::NOT_EXIST; // DB not exist!
	}
}

void Manager::setSchemaVersion(int sv)
{
	Statement stmt(m_conn, Query::INS_SCHEMA_INFO);

	stmt.bind(DB_VERSION_STR);
	stmt.bind(sv);

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

		stmt.bind(engineId);

		return stmt.step() ? stmt.getInt() : -1;
	} catch (const std::exception &e) {
		ERROR("getEngineState failed. error_msg=" << e.what());
		return -1;
	}
}

bool Manager::setEngineState(int engineId, int state) noexcept
{
	try {
		Statement stmt(m_conn, Query::INS_ENGINE_STATE);

		stmt.bind(engineId);
		stmt.bind(state);

		return stmt.exec() != 0;
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

		stmt.bind(dir);
		stmt.bind(dataVersion);

		return stmt.step() ? static_cast<time_t>(stmt.getInt64()) : -1;
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

		stmt.bind(dir);
		stmt.bind(static_cast<sqlite3_int64>(scanTime));
		stmt.bind(dataVersion);

		return stmt.exec() != 0;
	} catch (const std::exception &e) {
		ERROR("insertLastScanTime failed. error_msg=" << e.what());
		return false;
	}
}

bool Manager::deleteLastScanTime(const std::string &dir) noexcept
{
	try {
		Statement stmt(m_conn, Query::DEL_SCAN_REQUEST_BY_DIR);

		stmt.bind(dir);

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
RowsShPtr Manager::getDetectedMalwares(const std::string &dir) noexcept
{
	try {
		Statement stmt(m_conn, Query::SEL_DETECTED_BY_DIR);
		stmt.bind(dir);

		RowsShPtr rows = std::make_shared<std::vector<RowShPtr>>();
		while (stmt.step()) {
			RowShPtr row = std::make_shared<Row>();

			row->targetName = stmt.getText();
			row->dataVersion = stmt.getText();
			row->severity = static_cast<csr_cs_severity_level_e>(stmt.getInt());
			row->threat = static_cast<csr_cs_threat_type_e>(stmt.getInt());
			row->malwareName = stmt.getText();
			row->detailedUrl = stmt.getText();
			row->ts = static_cast<time_t>(stmt.getInt64());
			row->isIgnored = static_cast<bool>(stmt.getInt());

			rows->emplace_back(std::move(row));
		}

		return rows;
	} catch (const std::exception &e) {
		ERROR("getDetectedMalwares failed. error_msg=" << e.what());
		return nullptr;
	}
}

RowShPtr Manager::getDetectedMalware(const std::string &path) noexcept
{
	try {
		Statement stmt(m_conn, Query::SEL_DETECTED_BY_PATH);
		stmt.bind(path);

		if (!stmt.step())
			return nullptr;

		RowShPtr row = std::make_shared<Row>();
		row->targetName = stmt.getText();
		row->dataVersion = stmt.getText();
		row->severity = static_cast<csr_cs_severity_level_e>(stmt.getInt());
		row->threat = static_cast<csr_cs_threat_type_e>(stmt.getInt());
		row->malwareName = stmt.getText();
		row->detailedUrl = stmt.getText();
		row->ts = static_cast<time_t>(stmt.getInt64());
		row->isIgnored = static_cast<bool>(stmt.getInt());

		return row;
	} catch (const std::exception &e) {
		ERROR("getDetectedMalware failed. error_msg=" << e.what());
		return nullptr;
	}
}

bool Manager::insertDetectedMalware(const CsDetected &d,
									const std::string &dataVersion,
									bool isIgnored) noexcept
{
	try {
		Statement stmt(m_conn, Query::INS_DETECTED);

		stmt.bind(d.targetName);
		stmt.bind(dataVersion);
		stmt.bind(static_cast<int>(d.severity));
		stmt.bind(static_cast<int>(d.threat));
		stmt.bind(d.malwareName);
		stmt.bind(d.detailedUrl);
		stmt.bind(static_cast<sqlite3_int64>(d.ts));
		stmt.bind(static_cast<int>(isIgnored));

		return stmt.exec() == 1;
	} catch (const std::exception &e) {
		ERROR("insertDetectedMalware failed. error_msg=" << e.what());
		return false;
	}
}

bool Manager::setDetectedMalwareIgnored(const std::string &path,
										bool flag) noexcept
{
	try {
		Statement stmt(m_conn, Query::UPD_DETECTED_INGNORED);

		stmt.bind(flag);
		stmt.bind(path);

		return stmt.exec() == 1;
	} catch (const std::exception &e) {
		ERROR("setDetectedMalwareIgnored failed. error_msg=" << e.what());
		return false;
	}
}

bool Manager::deleteDetectedMalware(const std::string &path) noexcept
{
	try {
		Statement stmt(m_conn, Query::DEL_DETECTED_BY_PATH);

		stmt.bind(path);

		stmt.exec();

		return true; // even if no rows are deleted
	} catch (const std::exception &e) {
		ERROR("deleteDetectedMalware failed.error_msg=" << e.what());
		return false;
	}
}

bool Manager::deleteDeprecatedDetecedMalwares(const std::string &dir,
		const std::string &dataVersion) noexcept
{
	try {
		Statement stmt(m_conn, Query::DEL_DETECTED_DEPRECATED);

		stmt.bind(dir);
		stmt.bind(dataVersion);

		stmt.exec();

		return true; // even if no rows are deleted
	} catch (const std::exception &e) {
		ERROR("deleteDeprecatedDetecedMalwares failed.error_msg=" << e.what());
		return false;
	}
}

} // namespace Db
} // namespace Csr
