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
#pragma once

namespace Csr {
namespace Db {
namespace Query {

const std::string CHK_TABLE =
	"select name from sqlite_master where type = 'table' and name = ?";

const std::string SEL_SCHEMA_INFO =
	"select value from SCHEMA_INFO where name = ?";

const std::string INS_SCHEMA_INFO =
	"insert or replace into SCHEMA_INFO (name, value) values (?, ?)";

const std::string SEL_ENGINE_STATE =
	"select state from ENGINE_STATE where id = ?";

const std::string INS_ENGINE_STATE =
	"insert or replace into ENGINE_STATE (id, state) values (?, ?)";

const std::string SEL_SCAN_REQUEST =
	"select last_scan from SCAN_REQUEST where dir = ? and data_version = ?";

const std::string INS_SCAN_REQUEST =
	"insert or replace into SCAN_REQUEST (dir, last_scan, data_version) "
	"values (?, ?, ?)";

const std::string DEL_SCAN_REQUEST_BY_DIR =
	"delete from SCAN_REQUEST where dir = ?";

const std::string DEL_SCAN_REQUEST =
	"delete from SCAN_REQUEST ";


const std::string SEL_DETECTED_BY_DIR =
	"SELECT path, data_version, "
	" severity, threat, malware_name, "
	" detailed_url, detected_time, ignored "
	" FROM detected_malware_file where path like ? || '%' ";

const std::string SEL_DETECTED_BY_PATH =
	"SELECT path, data_version, "
	" severity, threat, malware_name, "
	" detailed_url, detected_time, ignored "
	" FROM detected_malware_file where path = ? ";

const std::string INS_DETECTED =
	"insert or replace into DETECTED_MALWARE_FILE "
	" (path, data_version, severity, threat, malware_name, "
	" detailed_url, detected_time, ignored) "
	" values (?, ?, ?, ?, ?, ?, ?, ?)";

const std::string UPD_DETECTED_INGNORED =
	"update DETECTED_MALWARE_FILE set ignored = ? where path = ?";

const std::string DEL_DETECTED_BY_PATH =
	"delete from DETECTED_MALWARE_FILE where path = ?";

const std::string DEL_DETECTED_DEPRECATED =
	"delete from DETECTED_MALWARE_FILE where path like ? || '%' "
	" and data_version != ?";

} // namespace Query
} // namespace Db
} // namespace Csr
