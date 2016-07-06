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
 * @file        migrate_1.sql
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       DB migration from schema version 1 to 2
 */

-- isolate old data
DROP INDEX c_name_index;
DROP VIEW join_detecteds_cloud_by_name;
ALTER TABLE DETECTED_MALWARE_CLOUD RENAME TO OLD_DETECTED_MALWARE_CLOUD;

-- create new structure
CREATE TABLE DETECTED_MALWARE_CLOUD (
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

CREATE VIEW [join_detecteds_cloud_by_name] AS
	SELECT N.name, D.data_version, D.malware_name, D.detailed_url, D.severity,
			D.detected_time, D.pkg_id, N.is_ignored
		FROM NAMES AS N INNER JOIN DETECTED_MALWARE_CLOUD AS D ON N.idx = D.idx;
CREATE INDEX c_name_index ON DETECTED_MALWARE_CLOUD(idx);

-- move data
INSERT INTO DETECTED_MALWARE_CLOUD(idx, pkg_id, data_version, malware_name, detailed_url, severity, detected_time)
	SELECT idx, pkg_id, data_version, malware_name, detailed_url, severity, MAX(detected_time) FROM OLD_DETECTED_MALWARE_CLOUD GROUP BY idx;

-- cleanup
DROP TABLE OLD_DETECTED_MALWARE_CLOUD;
