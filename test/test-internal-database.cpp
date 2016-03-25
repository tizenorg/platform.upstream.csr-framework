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

#include "database/csr_db.h"
#include <iostream>
#include <fstream>
#include <string>

#include <boost/test/unit_test.hpp>

#define TEST_DB_FILE     TEST_DIR "/test.db"
#define TEST_DB_SCRIPTS  RO_DBSPACE

namespace {

inline void checkSameMalware(Csr::Database::RowDetected &malware1, Csr::Database::RowDetected &malware2)
{
	BOOST_REQUIRE_MESSAGE(malware1.path.compare(malware2.path) == 0,
						  "Failed. checkSameMalware. path1=" << malware1.path << ", path2=" << malware2.path);

	BOOST_REQUIRE_MESSAGE(malware1.dataVersion.compare(malware2.dataVersion) == 0,
						  "Failed. checkSameMalware. dataVersion1=" << malware1.dataVersion
						  << ", dataVersion2=" << malware2.dataVersion);

	BOOST_REQUIRE_MESSAGE(malware1.severityLevel == malware2.severityLevel,
						  "Failed. checkSameMalware. severityLevel1=" << malware1.severityLevel
						  << ", severityLevel2=" << malware2.severityLevel);

	BOOST_REQUIRE_MESSAGE(malware1.threatType == malware2.threatType,
						  "Failed. checkSameMalware. threatType1=" << malware1.threatType
						  << ", threatType2=" << malware2.threatType);

	BOOST_REQUIRE_MESSAGE(malware1.name.compare(malware2.name) == 0,
						  "Failed. checkSameMalware. name1=" << malware1.name
						  << ", name2=" << malware2.name);

	BOOST_REQUIRE_MESSAGE(malware1.detailedUrl.compare(malware2.detailedUrl) == 0,
						  "Failed. checkSameMalware. detailedUrl1=" << malware1.detailedUrl
						  << ", detailedUrl2=" << malware2.detailedUrl);

	BOOST_REQUIRE_MESSAGE(malware1.detected_time == malware2.detected_time,
						  "Failed. checkSameMalware. detected_time1=" << malware1.detected_time
						  << ", detected_time2=" << malware2.detected_time);

	BOOST_REQUIRE_MESSAGE(malware1.modified_time == malware2.modified_time,
						  "Failed. checkSameMalware. modified_time1=" << malware1.modified_time
						  << ", modified_time2=" << malware2.modified_time);

	BOOST_REQUIRE_MESSAGE(malware1.ignored == malware2.ignored,
						  "Failed. checkSameMalware. ignored1=" << malware1.ignored
						  << ", ignored2=" << malware2.ignored);
}

} // end of namespace


BOOST_AUTO_TEST_SUITE(INTERNAL_DATABASE)

BOOST_AUTO_TEST_CASE(schema_info)
{
	bool result;
	Csr::Database::CsrDB db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	int version = 1;
	result = db.setDbVersion(version);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. setDbVersion");

	version = db.getDbVersion();
	BOOST_REQUIRE_MESSAGE(version == 1, "Failed. getDbVersion. expected=1, actual=" << version);
}

BOOST_AUTO_TEST_CASE(engine_state)
{
	bool result;
	Csr::Database::CsrDB db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	int state = -1;
	result = db.setEngineState(1, 1);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. setEngineState");

	result = db.setEngineState(2, 2);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. setEngineState");

	state = db.getEngineState(1);
	BOOST_REQUIRE_MESSAGE(state == 1, "Failed. getEngineState. expected=1, actual=" << state);

	state = db.getEngineState(2);
	BOOST_REQUIRE_MESSAGE(state == 2, "Failed. getEngineState. expected=2, actual=" << state);

	result = db.setEngineState(1, 2);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. setEngineState");

	state = db.getEngineState(1);
	BOOST_REQUIRE_MESSAGE(state == 2, "Failed. getEngineState. expected=2, actual=" << state);
}

BOOST_AUTO_TEST_CASE(scan_time)
{
	bool result;
	Csr::Database::CsrDB db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	std::string dir = "/opt";
	long scantime = 100;
	long retrievedTime;
	std::string dataVersion = "1.0.0";

	result = db.cleanLastScanTime();
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. cleanLastScanTime");

	retrievedTime = db.getLastScanTime(dir, dataVersion);
	BOOST_REQUIRE_MESSAGE(retrievedTime == -1, "Failed. getLastScanTime."
						  "expected=-1, actual=" << retrievedTime);

	result = db.insertLastScanTime(dir, scantime, dataVersion);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. insertLastScanTime");

	result = db.insertLastScanTime(std::string("/opt/data"), scantime + 100, dataVersion);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. insertLastScanTime");

	result = db.insertLastScanTime(std::string("/opt/data/etc"), scantime + 200, dataVersion);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. insertLastScanTime");

	retrievedTime = db.getLastScanTime(dir, dataVersion);
	BOOST_REQUIRE_MESSAGE(retrievedTime == scantime, "Failed. getLastScanTime."
						  "expected=" << scantime << ", actual=" << retrievedTime);

	result = db.cleanLastScanTime();
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. cleanLastScanTime");
}

BOOST_AUTO_TEST_CASE(detected_malware_file)
{
	bool result;
	Csr::Database::CsrDB db(TEST_DB_FILE, TEST_DB_SCRIPTS);

	std::string initDataVersion = "1.0.0";
	std::string changedDataVersion = "2.0.0";

	// insert
	Csr::Database::RowDetected malware1;
	malware1.path = "/opt/testmalware1";
	malware1.dataVersion = initDataVersion;
	malware1.severityLevel = 1;
	malware1.threatType = 1;
	malware1.name = "testmalware1";
	malware1.detailedUrl = "http://detailed.malware.com";
	malware1.detected_time = 100;
	malware1.modified_time = 100;
	malware1.ignored = 1;

	Csr::Database::RowDetected malware2;
	malware2.path = "/opt/testmalware2";
	malware2.dataVersion = initDataVersion;
	malware2.severityLevel = 2;
	malware2.threatType = 2;
	malware2.name = "testmalware2";
	malware2.detailedUrl = "http://detailed2.malware.com";
	malware2.detected_time = 210;
	malware2.modified_time = 210;
	malware2.ignored = 2;

	Csr::Database::RowDetected malware3;
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
	Csr::Database::DetectedShrPtr detected = db.getDetectedMalware(malware1.path);
	BOOST_REQUIRE_MESSAGE(detected == nullptr, "Failed. getDetectedMalware for no data");

	Csr::Database::DetectedListShrPtr detectedList = db.getDetectedMalwares(std::string("/opt"));
	BOOST_REQUIRE_MESSAGE(detectedList->size() == 0, "Failed. getDetectedMalwares for no data. size="
						  <<  detectedList->size());

	// insertDetectedMalware test
	result = db.insertDetectedMalware(malware1);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. insertDetectedMalware");

	result = db.insertDetectedMalware(malware2);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. insertDetectedMalware");

	// getDetectedMalware test
	detected = db.getDetectedMalware(malware1.path);
	checkSameMalware(malware1, *detected.get());

	detected = db.getDetectedMalware(malware2.path);
	checkSameMalware(malware2, *detected.get());

	// getDetectedMalwares test
	detectedList = db.getDetectedMalwares(std::string("/opt"));
	BOOST_REQUIRE_MESSAGE(detectedList->size() == 2, "Failed. getDetectedMalwares. Size="
						  << detectedList->size());
	std::vector<Csr::Database::DetectedShrPtr>::iterator iter;
	for (iter = detectedList->begin(); iter != detectedList->end(); iter++) {
		if (malware1.path.compare((*iter)->path) == 0) {
			checkSameMalware(malware1, **iter);
		} else if (malware2.path.compare((*iter)->path) == 0) {
			checkSameMalware(malware2, **iter);
		} else {
			BOOST_REQUIRE_MESSAGE(false, "Failed. getDetectedMalwares");
		}
	}

	// setDetectedMalwareIgnored test
	result = db.setDetectedMalwareIgnored(malware1.path, 1);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. setDetectedMalwareIgnored");

	malware1.ignored = 1;
	detected = db.getDetectedMalware(malware1.path);
	checkSameMalware(malware1, *detected.get());

	// deleteDeprecatedDetecedMalwares test
	result = db.insertDetectedMalware(malware3);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. insertDetectedMalware");

	result = db.deleteDeprecatedDetecedMalwares(std::string("/opt"), changedDataVersion);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. deleteDeprecatedDetecedMalwares");

	detected = db.getDetectedMalware(malware3.path);
	checkSameMalware(malware3, *detected.get());

	detected = db.getDetectedMalware(malware1.path);
	BOOST_REQUIRE_MESSAGE(detected.get() == nullptr,
						  "Failed. deleteDeprecatedDetecedMalwares:getDetectedMalware");
	detected = db.getDetectedMalware(malware2.path);
	BOOST_REQUIRE_MESSAGE(detected.get() == nullptr,
						  "Failed. deleteDeprecatedDetecedMalwares:getDetectedMalware");

	// deleteDetecedMalware test
	result = db.deleteDetecedMalware(malware3.path);
	BOOST_REQUIRE_MESSAGE(result == true, "Failed. deleteDetecedMalware");
}


BOOST_AUTO_TEST_SUITE_END()
