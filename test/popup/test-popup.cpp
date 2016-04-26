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
 * @file        test-popup.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR popup service test
 */
#define BOOST_TEST_MODULE CSR_POPUP_TEST
#include "ui/askuser.h"
#include "ui/common.h"

#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

using namespace Csr::Ui;

namespace {

void printPressed(CsResponse response)
{
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;

	bool isValid = true;

	switch (response) {
	case CsResponse::REMOVE:
		std::cout << "############## REMOVE BUTTON PRESSED  #############" << std::endl;
		break;

	case CsResponse::IGNORE:
		std::cout << "############## IGNORE BUTTON PRESSED  #############" << std::endl;
		break;

	case CsResponse::SKIP:
		std::cout << "##############  SKIP  BUTTON PRESSED  #############" << std::endl;
		break;

	default:
		std::cout << "############## Invalid Response!!!    #############" << std::endl;
		isValid = false;
		break;
	}

	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;

	BOOST_REQUIRE_MESSAGE(isValid, "invalid response from csr-popup!");
}

void printPressed(WpResponse response)
{
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;

	bool isValid = true;

	switch (response) {
	case WpResponse::ALLOW:
		std::cout << "##############  ALLOW BUTTON PRESSED  #############" << std::endl;
		break;

	case WpResponse::DENY:
		std::cout << "##############  DENY  BUTTON PRESSED  #############" << std::endl;
		break;

	case WpResponse::CONFIRM:
		std::cout << "############## CONFIRM BUTTON PRESSED #############" << std::endl;
		break;

	default:
		std::cout << "############## Invalid Response!!!    #############" << std::endl;
		isValid = false;
		break;
	}

	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;

	BOOST_REQUIRE_MESSAGE(isValid, "invalid response from csr-popup!");
}

}

BOOST_AUTO_TEST_SUITE(POPUP)

BOOST_AUTO_TEST_CASE(file_single)
{
	try {
		AskUser askuser;

		FileItem item;
		item.severity = CSR_CS_SEVERITY_MEDIUM;
		item.threat = CSR_CS_THREAT_RISKY;
		item.filepath = "/opt/apps/csr/test/dummyfile";

		auto response = askuser.fileSingle("Message for file_single tc", item);
		printPressed(response);
	} catch (...) {
		BOOST_REQUIRE_MESSAGE(0, "exception shouldn't be thrown.");
	}
}

BOOST_AUTO_TEST_CASE(wp_ask_permission)
{
	try {
		AskUser askuser;

		UrlItem item;
		item.risk = CSR_WP_RISK_MEDIUM;
		item.url = "http://csr.test.dummyurl.com";

		auto response = askuser.wpAskPermission("Message for wp_ask_permission tc",
												item);
		printPressed(response);
	} catch (...) {
		BOOST_REQUIRE_MESSAGE(0, "exception shouldn't be thrown.");
	}
}

BOOST_AUTO_TEST_CASE(wp_notify)
{
	try {
		AskUser askuser;

		UrlItem item;
		item.risk = CSR_WP_RISK_HIGH;
		item.url = "http://csr.test.dummyurl.com";

		auto response = askuser.wpNotify("Message for wp_ask_permission tc", item);
		printPressed(response);
	} catch (...) {
		BOOST_REQUIRE_MESSAGE(0, "exception shouldn't be thrown.");
	}
}

BOOST_AUTO_TEST_SUITE_END()
