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
 * @file        test-db.cpp
 * @author      Dongsun Lee(ds73.lee@samsung.com)
 * @version     1.0
 * @brief       database module unit test
 */
#include "db/manager.h"

#include <iostream>
#include <fstream>
#include <string>

#include <boost/test/unit_test.hpp>

#include "test-common.h"

#define TEST_DB_FILE     TEST_DIR "/test.db"
#define TEST_DB_SCRIPTS  RO_DBSPACE

using namespace Csr;

namespace {

void checkSameMalware(const CsDetected &d, const Db::Row &r)
{
	ASSERT_IF(d.targetName,  r.targetName);
	ASSERT_IF(d.severity,    r.severity);
	ASSERT_IF(d.malwareName, r.malwareName);
	ASSERT_IF(d.detailedUrl, r.detailedUrl);
	ASSERT_IF(d.ts,          r.ts);
}

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(DB)

BOOST_AUTO_TEST_CASE(schema_info)
{
	EXCEPTION_GUARD_START

	Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	ASSERT_IF(db.getSchemaVersion(), 1); // latest version is 1

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(engine_state)
{
	EXCEPTION_GUARD_START

	Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	db.setEngineState(CSR_ENGINE_CS, CSR_STATE_ENABLE);
	db.setEngineState(CSR_ENGINE_WP, CSR_STATE_DISABLE);

	ASSERT_IF(db.getEngineState(CSR_ENGINE_CS), CSR_STATE_ENABLE);
	ASSERT_IF(db.getEngineState(CSR_ENGINE_WP), CSR_STATE_DISABLE);

	db.setEngineState(CSR_ENGINE_CS, CSR_STATE_DISABLE);
	ASSERT_IF(db.getEngineState(CSR_ENGINE_CS), CSR_STATE_DISABLE);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(scan_time)
{
	EXCEPTION_GUARD_START

	Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	std::string dataVersion = "1.0.0";

	auto starttime = ::time(nullptr);

	db.cleanLastScanTime();
	ASSERT_IF(db.getLastScanTime("/opt", starttime - 1), -1);

	db.insertLastScanTime("/opt", dataVersion, starttime + 1);
	db.insertLastScanTime("/opt/data", dataVersion, starttime + 3);
	db.insertLastScanTime("/opt/data/etc", dataVersion, starttime + 2);

	ASSERT_IF(db.getLastScanTime("/opt/data/etc", starttime), starttime + 3);
	ASSERT_IF(db.getLastScanTime("/opt/data", starttime), starttime + 3);
	ASSERT_IF(db.getLastScanTime("/opt", starttime), starttime + 1);

	ASSERT_IF(db.getLastScanTime("/opt/data/etc", starttime + 4), -1);
	ASSERT_IF(db.getLastScanTime("/opt/data", starttime + 4), -1);
	ASSERT_IF(db.getLastScanTime("/opt", starttime + 2), -1);

	db.cleanLastScanTime();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(detected_malware_file)
{
	EXCEPTION_GUARD_START

	Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	std::string initDataVersion = "1.0.0";
	std::string changedDataVersion = "2.0.0";

	auto starttime = ::time(nullptr);

	// insert
	CsDetected malware1;
	malware1.targetName = "/opt/testmalware1";
	malware1.severity = CSR_CS_SEVERITY_MEDIUM;
	malware1.malwareName = "testmalware1";
	malware1.detailedUrl = "http://detailed.malware.com";
	malware1.ts = 1;

	CsDetected malware2;
	malware2.targetName = "/opt/testmalware2";
	malware2.severity = CSR_CS_SEVERITY_HIGH;
	malware2.malwareName = "testmalware2";
	malware2.detailedUrl = "http://detailed2.malware.com";
	malware2.ts = 2;

	CsDetected malware3;
	malware3.targetName = "/opt/testmalware3";
	malware3.severity = CSR_CS_SEVERITY_LOW;
	malware3.malwareName = "testmalware3";
	malware3.detailedUrl = "http://detailed3.malware.com";
	malware3.ts = starttime;

	CsDetected malware4;
	malware4.targetName = "/opt/testmalware4";
	malware4.severity = CSR_CS_SEVERITY_LOW;
	malware4.malwareName = "testmalware4";
	malware4.detailedUrl = "http://detailed4.malware.com";
	malware4.ts = starttime;

	// select test with vacant data
	auto detected = db.getDetectedAllByNameOnPath(malware1.targetName, 0);
	CHECK_IS_NULL(detected);

	auto detectedList = db.getDetectedAllByNameOnDir("/opt", 0);
	ASSERT_IF(detectedList.empty(), true);

	db.insertDetectedFile(malware1, initDataVersion);
	detected = db.getDetectedAllByNameOnPath(malware1.targetName, 0);
	CHECK_IS_NOT_NULL(detected);
	checkSameMalware(malware1, *detected);
	ASSERT_IF(detected->dataVersion, initDataVersion);
	ASSERT_IF(detected->isIgnored, false);

	db.insertDetectedFile(malware2, initDataVersion);
	db.updateIgnoreFlag(malware2.targetName, true);
	detected = db.getDetectedAllByNameOnPath(malware2.targetName, 0);
	CHECK_IS_NOT_NULL(detected);
	checkSameMalware(malware2, *detected);
	ASSERT_IF(detected->dataVersion, initDataVersion);
	ASSERT_IF(detected->isIgnored, true);

	db.insertDetectedFile(malware3, initDataVersion);
	db.updateIgnoreFlag(malware3.targetName, true);
	detected = db.getDetectedAllByNameOnPath(malware3.targetName, 0);
	CHECK_IS_NOT_NULL(detected);
	checkSameMalware(malware3, *detected);
	ASSERT_IF(detected->dataVersion, initDataVersion);
	ASSERT_IF(detected->isIgnored, true);

	// getDetectedMalwares test
	detectedList = db.getDetectedAllByNameOnDir("/opt", 0);
	ASSERT_IF(detectedList.size(), static_cast<size_t>(3));

	for (auto &item : detectedList) {
		if (malware1.targetName == item->targetName)
			checkSameMalware(malware1, *item);
		else if (malware2.targetName == item->targetName)
			checkSameMalware(malware2, *item);
		else if (malware3.targetName == item->targetName)
			checkSameMalware(malware3, *item);
		else
			BOOST_REQUIRE_MESSAGE(false, "Failed. getDetectedMalwares");
	}

	// setDetectedMalwareIgnored test
	db.updateIgnoreFlag(malware1.targetName, true);
	detected = db.getDetectedAllByNameOnPath(malware1.targetName, 0);
	checkSameMalware(malware1, *detected);
	ASSERT_IF(detected->isIgnored, true);

	// deleteDeprecatedDetectedMalwares test
	db.insertDetectedFile(malware4, changedDataVersion);
	db.deleteDetectedDeprecated(3);
	detected = db.getDetectedAllByNameOnPath(malware4.targetName, 0);
	CHECK_IS_NOT_NULL(detected);
	checkSameMalware(malware4, *detected);
	ASSERT_IF(detected->dataVersion, changedDataVersion);
	ASSERT_IF(detected->isIgnored, false);

	CHECK_IS_NULL(db.getDetectedAllByNameOnPath(malware1.targetName, 0));
	CHECK_IS_NULL(db.getDetectedAllByNameOnPath(malware2.targetName, 0));
	CHECK_IS_NOT_NULL(db.getDetectedAllByNameOnPath(malware3.targetName, 0));

	// deleteDetectedMalware test
	db.deleteDetectedByNameOnPath(malware3.targetName);
	db.deleteDetectedByNameOnPath(malware4.targetName);
	CHECK_IS_NULL(db.getDetectedAllByNameOnPath(malware3.targetName, 0));
	CHECK_IS_NULL(db.getDetectedAllByNameOnPath(malware4.targetName, 0));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
