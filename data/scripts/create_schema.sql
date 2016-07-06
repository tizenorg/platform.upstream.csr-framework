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
 * @file        create_schema.sql
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     2.0
 * @brief       DB schema
 */
CREATE TABLE IF NOT EXISTS SCHEMA_INFO (
	name TEXT NOT NULL,
	value TEXT NOT NULL,

	PRIMARY KEY(name)
);

CREATE TABLE IF NOT EXISTS ENGINE_STATE (
	id INTEGER NOT NULL,
	state INTEGER NOT NULL,

	PRIMARY KEY(id)
);

CREATE TABLE IF NOT EXISTS SCAN_REQUEST (
	dir TEXT NOT NULL,
	last_scan INTEGER NOT NULL,
	data_version TEXT NOT NULL,

	PRIMARY KEY(dir)
);

CREATE TABLE IF NOT EXISTS NAMES (
	name TEXT NOT NULL,
	is_ignored INTEGER DEFAULT 0,
	idx INTEGER PRIMARY KEY AUTOINCREMENT,

	UNIQUE(name)
);

CREATE TABLE IF NOT EXISTS DETECTED_MALWARE (
	filepath_idx INTEGER PRIMARY KEY AUTOINCREMENT,
	file_path TEXT NOT NULL,
	idx INTEGER NOT NULL,
	data_version TEXT NOT NULL,
	malware_name TEXT NOT NULL,
	detailed_url TEXT NOT NULL,
	severity INTEGER NOT NULL,
	detected_time INTEGER NOT NULL,

	FOREIGN KEY(idx) REFERENCES NAMES(idx) ON DELETE CASCADE,
	UNIQUE(file_path)
);

CREATE TABLE IF NOT EXISTS DETECTED_MALWARE_CLOUD (
	idx INTEGER NOT NULL,
	pkg_id TEXT NOT NULL,
	data_version TEXT NOT NULL,
	malware_name TEXT NOT NULL,
	detailed_url TEXT NOT NULL,
	severity INTEGER NOT NULL,
	detected_time INTEGER NOT NULL,

	FOREIGN KEY(idx) REFERENCES NAMES(idx) ON DELETE CASCADE,
	UNIQUE(idx)
);

CREATE TABLE IF NOT EXISTS PACKAGE_INFO (
	pkg_id TEXT NOT NULL,
	idx INTEGER NOT NULL,
	worst_filepath_idx TEXT NOT NULL,
	i INTEGER PRIMARY KEY AUTOINCREMENT,

	FOREIGN KEY(idx) REFERENCES NAMES(idx) ON DELETE CASCADE,
	FOREIGN KEY(worst_filepath_idx) REFERENCES DETECTED_MALWARE(filepath_idx) ON DELETE CASCADE
);

CREATE VIEW IF NOT EXISTS [join_detecteds_cloud_by_name] AS
	SELECT N.name, D.data_version, D.malware_name, D.detailed_url, D.severity,
			D.detected_time, D.pkg_id, N.is_ignored
		FROM NAMES AS N INNER JOIN DETECTED_MALWARE_CLOUD AS D ON N.idx = D.idx;

CREATE VIEW IF NOT EXISTS [join_p_d] AS
	SELECT N.idx, N.name, D.file_path, D.data_version, D.malware_name, D.detailed_url,
			D.severity, D.detected_time, P.pkg_id
		FROM (PACKAGE_INFO AS P INNER JOIN DETECTED_MALWARE AS D ON P.worst_filepath_idx = D.filepath_idx)
			AS J INNER JOIN NAMES AS N ON J.idx = N.idx;

CREATE VIEW IF NOT EXISTS [join_detecteds_by_name] AS
	SELECT N.name, J.file_path, J.data_version, J.malware_name, J.detailed_url,
			J.severity, J.detected_time, J.pkg_id, N.is_ignored
		FROM (
			SELECT D.idx, D.file_path, D.data_version, D.malware_name, D.detailed_url,
					D.severity, D.detected_time, P.pkg_id
				FROM DETECTED_MALWARE AS D LEFT JOIN PACKAGE_INFO AS P ON D.idx = P.idx
				WHERE NOT EXISTS (SELECT * FROM join_p_d WHERE idx = D.idx)
			UNION
			SELECT idx, file_path, data_version, malware_name, detailed_url,
					severity, detected_time, pkg_id
				FROM join_p_d)
			AS J INNER JOIN NAMES AS N ON J.idx = N.idx;

CREATE VIEW IF NOT EXISTS [join_detecteds_by_file_path] AS
	SELECT N.name, D.file_path, D.data_version, D.malware_name, D.detailed_url,
			D.severity, D.detected_time, P.pkg_id, N.is_ignored
		FROM (DETECTED_MALWARE AS D LEFT JOIN PACKAGE_INFO AS P ON D.idx = P.idx)
			AS J INNER JOIN NAMES AS N ON J.idx = N.idx;

CREATE INDEX IF NOT EXISTS d_name_index ON DETECTED_MALWARE(idx);
CREATE INDEX IF NOT EXISTS c_name_index ON DETECTED_MALWARE_CLOUD(idx);
CREATE INDEX IF NOT EXISTS p_name_index ON PACKAGE_INFO(idx);
CREATE INDEX IF NOT EXISTS p_filepath_index ON PACKAGE_INFO(worst_filepath_idx);
