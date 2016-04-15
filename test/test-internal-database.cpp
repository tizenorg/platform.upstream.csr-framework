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
 * @file        test-internal-database.cpp
 * @author      Dongsun Lee(ds73.lee@samsung.com)
 * @version     1.0
 * @brief       CSR Content screening DB internal test
 */

#include "db/manager.h"

#include <iostream>
#include <fstream>
#include <string>

#include <boost/test/unit_test.hpp>

#include "test-common.h"

#define TEST_DB_FILE     TEST_DIR "/test.db"
#define TEST_DB_SCRIPTS  RO_DBSPACE

namespace {

void checkSameMalware(Csr::Db::RowDetected &malware1,
					  Csr::Db::RowDetected &malware2)
{
	ASSERT_IF(malware1.path,          malware2.path);
	ASSERT_IF(malware1.dataVersion,   malware2.dataVersion);
	ASSERT_IF(malware1.severityLevel, malware2.severityLevel);
	ASSERT_IF(malware1.threatType,    malware2.threatType);
	ASSERT_IF(malware1.name,          malware2.name);
	ASSERT_IF(malware1.detailedUrl,   malware2.detailedUrl);
	ASSERT_IF(malware1.detected_time, malware2.detected_time);
	ASSERT_IF(malware1.modified_time, malware2.modified_time);
	ASSERT_IF(malware1.ignored,       malware2.ignored);
}

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(INTERNAL_DATABASE)

BOOST_AUTO_TEST_CASE(schema_info)
{
	EXCEPTION_GUARD_START

	Csr::Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	ASSERT_IF(db.getSchemaVersion(), 1); // latest version is 1

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(engine_state)
{
	EXCEPTION_GUARD_START

	Csr::Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	ASSERT_IF(db.setEngineState(1, 1), true);
	ASSERT_IF(db.setEngineState(2, 2), true);

	ASSERT_IF(db.getEngineState(1), 1);
	ASSERT_IF(db.getEngineState(2), 2);

	ASSERT_IF(db.setEngineState(1, 2), true);
	ASSERT_IF(db.getEngineState(1), 2);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_time)
{
	EXCEPTION_GUARD_START

	Csr::Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	std::string dir = "/opt";
	long scantime = 100;
	std::string dataVersion = "1.0.0";

	ASSERT_IF(db.cleanLastScanTime(), true);
	ASSERT_IF(db.getLastScanTime(dir, dataVersion), -1);
	ASSERT_IF(db.insertLastScanTime(dir, scantime, dataVersion), true);

	ASSERT_IF(db.insertLastScanTime("/opt/data", scantime + 100, dataVersion),
			  true);
	ASSERT_IF(db.insertLastScanTime("/opt/data/etc", scantime + 200, dataVersion),
			  true);

	ASSERT_IF(db.getLastScanTime(dir, dataVersion), scantime);
	ASSERT_IF(db.cleanLastScanTime(), true);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(detected_malware_file)
{
	EXCEPTION_GUARD_START

	Csr::Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	std::string initDataVersion = "1.0.0";
	std::string changedDataVersion = "2.0.0";

	// insert
	Csr::Db::RowDetected malware1;
	malware1.path = "/opt/testmalware1";
	malware1.dataVersion = initDataVersion;
	malware1.severityLevel = 1;
	malware1.threatType = 1;
	malware1.name = "testmalware1";
	malware1.detailedUrl = "http://detailed.malware.com";
	malware1.detected_time = 100;
	malware1.modified_time = 100;
	malware1.ignored = 1;

	Csr::Db::RowDetected malware2;
	malware2.path = "/opt/testmalware2";
	malware2.dataVersion = initDataVersion;
	malware2.severityLevel = 2;
	malware2.threatType = 2;
	malware2.name = "testmalware2";
	malware2.detailedUrl = "http://detailed2.malware.com";
	malware2.detected_time = 210;
	malware2.modified_time = 210;
	malware2.ignored = 2;

	Csr::Db::RowDetected malware3;
	malware3.path = "/opt/testmalware3";
	malware3.dataVersion = changedDataVersion;
	malware3.severityLevel = 3;
	malware3.threatType = 3;
	malware3.name = "testmalware2";
	malware3.detailedUrl = "http://detailed2.malware.com";
	malware3.detected_time = 310;
	malware3.modified_time = 310;
	malware3.ignored = 3;

	// select test with vacant data
	auto detected = db.getDetectedMalware(malware1.path);
	CHECK_IS_NULL(detected);

	auto detectedList = db.getDetectedMalwares("/opt");
	ASSERT_IF(detectedList->empty(), true);

	// insertDetectedMalware test
	ASSERT_IF(db.insertDetectedMalware(malware1), true);
	ASSERT_IF(db.insertDetectedMalware(malware2), true);

	// getDetectedMalware test
	detected = db.getDetectedMalware(malware1.path);
	checkSameMalware(malware1, *detected);
	detected = db.getDetectedMalware(malware2.path);
	checkSameMalware(malware2, *detected);

	// getDetectedMalwares test
	detectedList = db.getDetectedMalwares("/opt");
	ASSERT_IF(detectedList->size(), static_cast<size_t>(2));

	for (auto &item : *detectedList) {
		if (malware1.path == item->path)
			checkSameMalware(malware1, *item);
		else if (malware2.path == item->path)
			checkSameMalware(malware2, *item);
		else
			BOOST_REQUIRE_MESSAGE(false, "Failed. getDetectedMalwares");
	}

	// setDetectedMalwareIgnored test
	ASSERT_IF(db.setDetectedMalwareIgnored(malware1.path, 1), true);

	malware1.ignored = 1;
	detected = db.getDetectedMalware(malware1.path);
	checkSameMalware(malware1, *detected);

	// deleteDeprecatedDetecedMalwares test
	ASSERT_IF(db.insertDetectedMalware(malware3), true);

	ASSERT_IF(db.deleteDeprecatedDetecedMalwares("/opt", changedDataVersion), true);

	detected = db.getDetectedMalware(malware3.path);
	checkSameMalware(malware3, *detected);

	detected = db.getDetectedMalware(malware1.path);
	CHECK_IS_NULL(detected);
	detected = db.getDetectedMalware(malware2.path);
	CHECK_IS_NULL(detected);

	// deleteDetecedMalware test
	ASSERT_IF(db.deleteDetecedMalware(malware3.path), true);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
