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

using namespace Csr;

namespace {

void checkSameMalware(const CsDetected &d, const Db::Row &r)
{
	ASSERT_IF(d.targetName,  r.targetName);
	ASSERT_IF(d.severity,    r.severity);
	ASSERT_IF(d.threat,      r.threat);
	ASSERT_IF(d.malwareName, r.malwareName);
	ASSERT_IF(d.detailedUrl, r.detailedUrl);
	ASSERT_IF(d.ts,          r.ts);
}

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(INTERNAL_DATABASE)

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

	Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

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

	Db::Manager db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	std::string initDataVersion = "1.0.0";
	std::string changedDataVersion = "2.0.0";

	// insert
	CsDetected malware1;
	malware1.targetName = "/opt/testmalware1";
	malware1.severity = CSR_CS_SEVERITY_MEDIUM;
	malware1.threat = CSR_CS_THREAT_MALWARE;
	malware1.malwareName = "testmalware1";
	malware1.detailedUrl = "http://detailed.malware.com";
	malware1.ts = 100;

	CsDetected malware2;
	malware2.targetName = "/opt/testmalware2";
	malware2.severity = CSR_CS_SEVERITY_HIGH;
	malware2.threat = CSR_CS_THREAT_RISKY;
	malware2.malwareName = "testmalware2";
	malware2.detailedUrl = "http://detailed2.malware.com";
	malware2.ts = 210;

	CsDetected malware3;
	malware3.targetName = "/opt/testmalware3";
	malware3.severity = CSR_CS_SEVERITY_LOW;
	malware3.threat = CSR_CS_THREAT_GENERIC;
	malware3.malwareName = "testmalware3";
	malware3.detailedUrl = "http://detailed3.malware.com";
	malware3.ts = 310;

	// select test with vacant data
	auto detected = db.getDetectedMalware(malware1.targetName);
	CHECK_IS_NULL(detected);

	auto detectedList = db.getDetectedMalwares("/opt");
	ASSERT_IF(detectedList->empty(), true);

	ASSERT_IF(db.insertDetectedMalware(malware1, initDataVersion, false), true);
	detected = db.getDetectedMalware(malware1.targetName);
	checkSameMalware(malware1, *detected);
	ASSERT_IF(detected->dataVersion, initDataVersion);
	ASSERT_IF(detected->isIgnored, false);

	ASSERT_IF(db.insertDetectedMalware(malware2, initDataVersion, true), true);
	detected = db.getDetectedMalware(malware2.targetName);
	checkSameMalware(malware2, *detected);
	ASSERT_IF(detected->dataVersion, initDataVersion);
	ASSERT_IF(detected->isIgnored, true);

	// getDetectedMalwares test
	detectedList = db.getDetectedMalwares("/opt");
	ASSERT_IF(detectedList->size(), static_cast<size_t>(2));

	for (auto &item : *detectedList) {
		if (malware1.targetName == item->targetName)
			checkSameMalware(malware1, *item);
		else if (malware2.targetName == item->targetName)
			checkSameMalware(malware2, *item);
		else
			BOOST_REQUIRE_MESSAGE(false, "Failed. getDetectedMalwares");
	}

	// setDetectedMalwareIgnored test
	ASSERT_IF(db.setDetectedMalwareIgnored(malware1.targetName, true), true);
	detected = db.getDetectedMalware(malware1.targetName);
	checkSameMalware(malware1, *detected);
	ASSERT_IF(detected->isIgnored, true);

	// deleteDeprecatedDetecedMalwares test
	ASSERT_IF(db.insertDetectedMalware(malware3, changedDataVersion, false), true);
	ASSERT_IF(db.deleteDeprecatedDetecedMalwares("/opt", changedDataVersion), true);
	detected = db.getDetectedMalware(malware3.targetName);
	checkSameMalware(malware3, *detected);
	ASSERT_IF(detected->dataVersion, changedDataVersion);
	ASSERT_IF(detected->isIgnored, false);

	CHECK_IS_NULL(db.getDetectedMalware(malware1.targetName));
	CHECK_IS_NULL(db.getDetectedMalware(malware2.targetName));

	// deleteDetectedMalware test
	ASSERT_IF(db.deleteDetectedMalware(malware3.targetName), true);
	CHECK_IS_NULL(db.getDetectedMalware(malware3.targetName));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
