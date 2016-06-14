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

const std::string SEL_ENGINE_STATE_ALL =
	"select id, state from ENGINE_STATE";

const std::string SEL_SCAN_REQUEST =
	"select last_scan from SCAN_REQUEST where dir = ? and data_version = ?";

const std::string INS_SCAN_REQUEST =
	"insert or replace into SCAN_REQUEST (dir, last_scan, data_version)"
	" values (?, ?, ?)";

const std::string DEL_SCAN_REQUEST_BY_DIR =
	"delete from SCAN_REQUEST where dir = ?";

const std::string DEL_SCAN_REQUEST =
	"delete from SCAN_REQUEST";

const std::string SEL_DETECTED_CLOUD_BY_NAME_ON_PATH =
	"select name, data_version, malware_name, detailed_url, severity, detected_time,"
	"       pkg_id, is_ignored"
	" from join_detecteds_cloud_by_name"
	" where name = ?";

const std::string SEL_DETECTED_BY_NAME_ON_PATH =
	"select name, file_path, data_version, malware_name, detailed_url, severity,"
	"       detected_time, pkg_id, is_ignored"
	" from join_detecteds_by_name"
	" where name = ?";

const std::string SEL_DETECTED_CLOUD_BY_NAME_ON_DIR =
	"select name, data_version, malware_name, detailed_url, severity, detected_time,"
	"       pkg_id, is_ignored"
	" from join_detecteds_cloud_by_name"
	" where name like ? || '%'";

const std::string SEL_DETECTED_BY_NAME_ON_DIR =
	"select name, file_path, data_version, malware_name, detailed_url, severity,"
	"       detected_time, pkg_id, is_ignored"
	" from join_detecteds_by_name"
	" where name like ? || '%'";

const std::string SEL_DETECTED_BY_FILEPATH_ON_DIR =
	"select name, file_path, data_version, malware_name, detailed_url, severity,"
	"       detected_time, pkg_id, is_ignored"
	" from join_detecteds_by_file_path"
	" where file_path like ? || '%'";

const std::string SEL_WORST_BY_PKGID =
	"select name, file_path, data_version, malware_name, detailed_url, severity,"
	"       detected_time"
	" from join_p_d"
	" where pkg_id = ?";

const std::string INS_NAME =
	"insert or replace into NAMES(name) values(?)";

const std::string INS_DETECTED_CLOUD =
	"insert or replace into DETECTED_MALWARE_CLOUD(idx, pkg_id, data_version,"
	"                                              malware_name, detailed_url, severity,"
	"                                              detected_time)"
	" values((select idx from NAMES where name = ?), ?, ?, ?, ?, ?, ?)";

const std::string INS_DETECTED =
	"insert or replace into DETECTED_MALWARE(file_path, idx, data_version, malware_name,"
	"                                        detailed_url, severity, detected_time)"
	" values(?, (select idx from NAMES where name = ?), ?, ?, ?, ?, ?)";

const std::string INS_WORST =
	"insert or replace into PACKAGE_INFO(pkg_id, idx, worst_filepath_idx)"
	" values(?,"
	"  (select idx from NAMES where name = ?),"
	"  (select filepath_idx from DETECTED_MALWARE where file_path = ?))";

const std::string UPD_IGNORE =
	"update NAMES set is_ignored = ? where name = ?";

const std::string DEL_DETECTED_BY_NAME_ON_PATH =
	"delete from NAMES where name = ?";

const std::string DEL_DETECTED_BY_FILEPATH_ON_PATH =
	"delete from DETECTED_MALWARE where file_path = ?";

const std::string DEL_DETECTED_DEPRECATED_ON_DIR =
	"delete from DETECTED_MALWARE where file_path like ? || '%' "
	" and data_version != ?";

} // namespace Query
} // namespace Db
} // namespace Csr
