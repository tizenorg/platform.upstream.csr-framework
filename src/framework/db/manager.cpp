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

#include "db/query.h"
#include "db/statement.h"
#include "common/audit/logger.h"
#include "common/exception.h"

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

const std::string DB_VERSION_STR    = "DB_VERSION";
const std::string SCHEMA_INFO_TABLE = "SCHEMA_INFO";

RowShPtr extractRow(Statement &stmt)
{
	RowShPtr row = std::make_shared<Row>();

	row->targetName = stmt.getText(); // name.
	row->fileInAppPath = stmt.getText(); // file_path
	row->dataVersion = stmt.getText(); // data_version
	row->malwareName = stmt.getText(); // malware_name
	row->detailedUrl = stmt.getText(); // detailed_url
	row->severity = static_cast<csr_cs_severity_level_e>(stmt.getInt()); // severity
	row->ts = static_cast<time_t>(stmt.getInt64()); // detected_time
	row->pkgId = stmt.getText(); // pkg_id
	row->isApp = !row->pkgId.empty();
	row->isIgnored = static_cast<bool>(stmt.getInt());

	return row;
}

RowShPtr extractRowCloud(Statement &stmt)
{
	RowShPtr row = std::make_shared<Row>();

	row->targetName = stmt.getText(); // name.
	row->fileInAppPath.clear();
	row->dataVersion = stmt.getText(); // data_version
	row->malwareName = stmt.getText(); // malware_name
	row->detailedUrl = stmt.getText(); // detailed_url
	row->severity = static_cast<csr_cs_severity_level_e>(stmt.getInt()); // severity
	row->ts = static_cast<time_t>(stmt.getInt64()); // detected_time
	row->pkgId = stmt.getText(); // pkg_id
	row->isApp = true;
	row->isIgnored = static_cast<bool>(stmt.getInt());

	return row;
}

} // namespace anonymous

Manager::Manager(const std::string &dbfile, const std::string &scriptsDir) :
	m_conn(dbfile, Connection::Create | Connection::ReadWrite |
		   Connection::Serialized),
	m_scriptsDir(scriptsDir)
{
	this->m_conn.exec("PRAGMA foreign_keys = ON;");

	// run migration if old database is present
	auto sv = this->getSchemaVersion();

	if (sv < SchemaVersion::NOT_EXIST || sv > SchemaVersion::LATEST) {
		ERROR("Database corrupted! invalid db version returned! : " << sv);
		this->resetDatabase();
		return;
	} else if (sv == SchemaVersion::LATEST) {
		DEBUG("Database version is latest");
		return;
	}

	if (sv == SchemaVersion::NOT_EXIST) {
		INFO("Database initializing!");
		this->resetDatabase();
	} else if (sv < SchemaVersion::LATEST) {
		INFO("Database migration! from[" << sv <<
			 "] to[" << SchemaVersion::LATEST << "]");

		for (int vi = sv; vi < SchemaVersion::LATEST; ++vi)
			this->m_conn.exec(this->getMigrationScript(vi).c_str());

		this->setSchemaVersion(SchemaVersion::LATEST);
	}

	this->m_conn.exec("VACUUM;");
}

Manager::~Manager()
{
}

void Manager::resetDatabase()
{
	this->m_conn.exec(this->getScript(SCRIPT_DROP_ALL_ITEMS).c_str());
	this->m_conn.exec(this->getScript(SCRIPT_CREATE_SCHEMA).c_str());
	this->setSchemaVersion(SchemaVersion::LATEST);
}

std::string Manager::getMigrationScript(int sv)
{
	return this->getScript(SCRIPT_MIGRATE + std::to_string(sv));
}

std::string Manager::getScript(const std::string &scriptName)
{
	auto scriptPath = this->m_scriptsDir + std::string("/") + scriptName + ".sql";
	std::ifstream is(scriptPath);

	if (is.fail())
		ThrowExc(CSR_ERROR_DB, "Cannot open script: " << scriptPath);

	std::istreambuf_iterator<char> begin(is), end;
	auto str = std::string(begin, end);

	if (str.empty())
		ThrowExc(CSR_ERROR_DB, "Script file empty: " << scriptPath);

	return str;
}

bool Manager::isTableExist(const std::string &name)
{
	Statement stmt(this->m_conn, Query::CHK_TABLE);

	stmt.bind(name);

	return stmt.step();
}

int Manager::getSchemaVersion()
{
	if (!this->isTableExist(SCHEMA_INFO_TABLE)) {
		WARN("Schema table doesn't exist. This case would be the first time of "
			 "db manager instantiated in target");
		return SchemaVersion::NOT_EXIST;
	}

	Statement stmt(this->m_conn, Query::SEL_SCHEMA_INFO);

	stmt.bind(DB_VERSION_STR);

	return stmt.step() ? stmt.getInt() : SchemaVersion::NOT_EXIST;
}

void Manager::setSchemaVersion(int sv)
{
	Statement stmt(this->m_conn, Query::INS_SCHEMA_INFO);

	stmt.bind(DB_VERSION_STR);
	stmt.bind(sv);

	if (stmt.exec() == 0)
		ThrowExc(CSR_ERROR_DB, "Failed to set schema version to: " << sv);
}

//===========================================================================
// ENGINE_STATE table
//===========================================================================
csr_state_e Manager::getEngineState(csr_engine_id_e id)
{
	std::lock_guard<std::mutex> ll(this->m_mutex);

	{
		std::lock_guard<std::mutex> l(this->m_stateMutex);

		if (this->m_stateMap.size() == 0) {
			Statement stmt(this->m_conn, Query::SEL_ENGINE_STATE_ALL);

			while (stmt.step()) {
				auto _id = static_cast<csr_engine_id_e>(stmt.getInt());
				auto _state = static_cast<csr_state_e>(stmt.getInt());

				this->m_stateMap[_id] = _state;
			}
		}

		return (this->m_stateMap.count(id) == 0) ? CSR_STATE_ENABLE : this->m_stateMap[id];
	}
}

void Manager::setEngineState(csr_engine_id_e id, csr_state_e state)
{
	std::lock_guard<std::mutex> ll(this->m_mutex);

	{
		std::lock_guard<std::mutex> l(this->m_stateMutex);

		Statement stmt(this->m_conn, Query::INS_ENGINE_STATE);

		stmt.bind(static_cast<int>(id));
		stmt.bind(static_cast<int>(state));

		stmt.exec();

		this->m_stateMap[id] = state;
	}
}

//===========================================================================
// SCAN_REQUEST table
//===========================================================================

time_t Manager::getLastScanTime(const std::string &dir,
								const std::string &dataVersion)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	time_t latest = -1;
	std::string current = dir;
	Statement stmt(this->m_conn, Query::SEL_SCAN_REQUEST);

	while (true) {
		stmt.bind(current);
		stmt.bind(dataVersion);

		if (stmt.step()) {
			auto candidate = static_cast<time_t>(stmt.getInt64());
			if (latest < candidate)
				latest = candidate;
		}

		if (current == "/")
			break;

		auto pos = current.find_last_of('/');
		current = (pos == 0) ? "/" : current.substr(0, pos);

		stmt.reset();
	}

	return latest;
}

void Manager::insertCache(const Cache &c, bool isIgnored,
						  const std::string &filePath)
{
	// TODO Apply database transaction.
	for(std::vector<int>::size_type i = 0; i < c.detecteds.size(); i++)
		this->insertDetectedFileInApp(
			c.pkgPath, c.filePaths[i], c.detecteds[i], c.dataVersion);

	if(isIgnored)
		this->updateIgnoreFlag(c.pkgPath, false);

	this->insertLastScanTime(c.pkgPath, c.scanTime, c.dataVersion);
	this->insertWorst(c.pkgId, c.pkgPath, filePath);
}

void Manager::insertLastScanTime(const std::string &dir, time_t scanTime,
								 const std::string &dataVersion)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::INS_SCAN_REQUEST);

	stmt.bind(dir);
	stmt.bind(static_cast<sqlite3_int64>(scanTime));
	stmt.bind(dataVersion);
	stmt.exec();
}

void Manager::deleteLastScanTime(const std::string &dir)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::DEL_SCAN_REQUEST_BY_DIR);

	stmt.bind(dir);
	stmt.exec();
}

void Manager::cleanLastScanTime()
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::DEL_SCAN_REQUEST);

	stmt.exec();
}

//===========================================================================
// DETECTED_MALWARE_FILE table
//===========================================================================
RowShPtr Manager::getDetectedByNameOnPath(const std::string &path)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::SEL_DETECTED_BY_NAME_ON_PATH);
	stmt.bind(path);

	if (!stmt.step())
		return nullptr;

	return extractRow(stmt);
}

RowShPtr Manager::getDetectedCloudByNameOnPath(const std::string &path)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::SEL_DETECTED_CLOUD_BY_NAME_ON_PATH);
	stmt.bind(path);

	if (!stmt.step())
		return nullptr;

	return extractRowCloud(stmt);
}

RowShPtr Manager::getDetectedAllByNameOnPath(const std::string &path, bool *isByCloud)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::SEL_DETECTED_BY_NAME_ON_PATH);
	stmt.bind(path);

	if (stmt.step()) {
		if (isByCloud)
			*isByCloud = false;
		return extractRow(stmt);
	}

	Statement stmt2(this->m_conn, Query::SEL_DETECTED_CLOUD_BY_NAME_ON_PATH);
	stmt.bind(path);

	if (stmt2.step()) {
		if (isByCloud)
			*isByCloud = true;
		return extractRow(stmt);
	}

	return nullptr;
}

RowShPtrs Manager::getDetectedByNameOnDir(const std::string &dir)
{
	Statement stmt(this->m_conn, Query::SEL_DETECTED_BY_NAME_ON_DIR);
	stmt.bind(dir);

	RowShPtrs rows;

	while (stmt.step())
		rows.emplace_back(extractRow(stmt));

	return rows;
}

RowShPtrs Manager::getDetectedCloudByNameOnDir(const std::string &dir)
{
	Statement stmt(this->m_conn, Query::SEL_DETECTED_CLOUD_BY_NAME_ON_DIR);
	stmt.bind(dir);

	RowShPtrs rows;
	while (stmt.step())
		rows.emplace_back(extractRowCloud(stmt));

	return rows;
}

RowShPtrs Manager::getDetectedAllByNameOnDir(const std::string &dir)
{
	RowShPtrs normals;
	RowShPtrs clouds;

	{
		std::lock_guard<std::mutex> l(this->m_mutex);
		normals = this->getDetectedByNameOnDir(dir);
		clouds = this->getDetectedCloudByNameOnDir(dir);
	}

	if (clouds.empty())
		return normals;

	RowShPtrs rows;

	for (const auto &cloud : clouds) {
		bool found = false;
		auto it = normals.begin();

		while (it != normals.end()) {
			if ((*it)->targetName == cloud->targetName) {
				rows.emplace_back(std::move(*it));
				it = normals.erase(it);
				found = true;
				break;
			} else {
				++it;
			}
		}

		if (!found)
			rows.push_back(cloud);
	}

	for (auto &&normal : normals)
		rows.emplace_back(std::move(normal));

	return rows;
}

RowShPtrs Manager::getDetectedByFilepathOnDir(const std::string &dir)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::SEL_DETECTED_BY_FILEPATH_ON_DIR);
	stmt.bind(dir);

	RowShPtrs rows;

	while (stmt.step())
		rows.emplace_back(extractRow(stmt));

	return rows;
}

RowShPtr Manager::getWorstByPkgPath(const std::string &pkgPath)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::SEL_WORST_BY_PKGPATH);
	stmt.bind(pkgPath);

	if (!stmt.step())
		return nullptr;

	RowShPtr row = std::make_shared<Row>();

	row->targetName = stmt.getText(); // name
	row->fileInAppPath = stmt.getText(); // file_path
	row->dataVersion = stmt.getText(); // data_version
	row->malwareName = stmt.getText(); // malware_name
	row->detailedUrl = stmt.getText(); // detailed_url
	row->severity = static_cast<csr_cs_severity_level_e>(stmt.getInt()); // severity
	row->ts = static_cast<time_t>(stmt.getInt64()); // detected_time
	row->pkgId = stmt.getText(); // pkg_id
	row->isApp = true;

	return row;
}

void Manager::insertDetectedFile(const std::string &filepath, const CsDetected &d,
								 const std::string &dataVersion)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	this->insertName(filepath);
	this->insertDetected(d, filepath, dataVersion);
}

void Manager::insertDetectedFileInApp(const std::string &pkgpath, const std::string &filepath,
									  const CsDetected &d, const std::string &dataVersion)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	this->insertName(pkgpath);
	this->insertDetected(d, filepath, dataVersion);
}

void Manager::insertDetectedAppByCloud(const std::string &name, const std::string &pkgId,
									   const CsDetected &d, const std::string &dataVersion)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	this->insertName(name);
	this->insertDetectedCloud(d, pkgId, name, dataVersion);
}

void Manager::insertName(const std::string &name)
{
	Statement stmt(this->m_conn, Query::INS_NAME);

	stmt.bind(name);
	stmt.exec();
}

void Manager::insertDetected(const CsDetected &d, const std::string &filepath,
							 const std::string &dataVersion)
{
	Statement stmt(this->m_conn, Query::INS_DETECTED);

	stmt.bind(filepath);
	stmt.bind(d.targetName);
	stmt.bind(dataVersion);
	stmt.bind(d.malwareName);
	stmt.bind(d.detailedUrl);
	stmt.bind(static_cast<int>(d.severity));
	stmt.bind(static_cast<sqlite3_int64>(d.ts));
	stmt.exec();
}

void Manager::insertDetectedCloud(const CsDetected &d, const std::string &pkgId,
								  const std::string &name, const std::string &dataVersion)
{
	Statement stmt(this->m_conn, Query::INS_DETECTED_CLOUD);

	stmt.bind(name);
	stmt.bind(pkgId);
	stmt.bind(dataVersion);
	stmt.bind(d.malwareName);
	stmt.bind(d.detailedUrl);
	stmt.bind(static_cast<int>(d.severity));
	stmt.bind(static_cast<sqlite3_int64>(d.ts));
	stmt.exec();
}

void Manager::insertWorst(const std::string &pkgId, const std::string &name,
						  const std::string &filepath)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::INS_WORST);

	stmt.bind(pkgId);
	stmt.bind(name);
	stmt.bind(filepath);
	stmt.exec();
}

void Manager::updateIgnoreFlag(const std::string &name, bool flag)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::UPD_IGNORE);

	stmt.bind((flag ? 1 : 0));
	stmt.bind(name);
	stmt.exec();
}

void Manager::deleteDetectedByNameOnPath(const std::string &path)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::DEL_DETECTED_BY_NAME_ON_PATH);

	stmt.bind(path);
	stmt.exec();
}

void Manager::deleteDetectedByFilepathOnPath(const std::string &path)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::DEL_DETECTED_BY_FILEPATH_ON_PATH);

	stmt.bind(path);
	stmt.exec();
}

void Manager::deleteDetectedDeprecatedOnDir(const std::string &dir,
											const std::string &dataVersion)
{
	std::lock_guard<std::mutex> l(this->m_mutex);

	Statement stmt(this->m_conn, Query::DEL_DETECTED_DEPRECATED_ON_DIR);

	stmt.bind(dir);
	stmt.bind(dataVersion);
	stmt.exec();
}

} // namespace Db
} // namespace Csr
