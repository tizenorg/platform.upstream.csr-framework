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
#include "ui/askuser.h"
#include "ui/common.h"
#include "common/cs-detected.h"

#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "test-resource.h"
#include "test-common.h"

using namespace Csr;

namespace {

void printPressed(csr_cs_user_response_e response)
{
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;

	bool isValid = true;

	switch (response) {
	case CSR_CS_USER_RESPONSE_REMOVE:
		std::cout << "############## REMOVE BUTTON PRESSED  #############" << std::endl;
		break;

	case CSR_CS_USER_RESPONSE_PROCESSING_ALLOWED:
		std::cout << "############## ALLOW  BUTTON PRESSED  #############" << std::endl;
		break;

	case CSR_CS_USER_RESPONSE_PROCESSING_DISALLOWED:
		std::cout << "############ DISALLOW BUTTON PRESSED ##############" << std::endl;
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

void printPressed(csr_wp_user_response_e response)
{
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;
	std::cout << "###################################################" << std::endl;

	bool isValid = true;

	switch (response) {
	case CSR_WP_USER_RESPONSE_PROCESSING_ALLOWED:
		std::cout << "##############  ALLOW BUTTON PRESSED  #############" << std::endl;
		break;

	case CSR_WP_USER_RESPONSE_PROCESSING_DISALLOWED:
		std::cout << "############ DENY/CONFIRM BUTTON PRESSED ##########" << std::endl;
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

BOOST_AUTO_TEST_SUITE(CS)

BOOST_AUTO_TEST_CASE(prompt_data)
{
	EXCEPTION_GUARD_START

	CsDetected d;
	d.targetName.clear(); // data's target name should be empty
	d.malwareName = "dummy malware";
	d.detailedUrl = "http://detailedurl/cs_prompt_data";
	d.severity = CSR_CS_SEVERITY_MEDIUM;

	Ui::AskUser askuser;
	printPressed(askuser.cs(Ui::CommandId::CS_PROMPT_DATA,
							"Message for prompt data tc", d));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(prompt_app)
{
	EXCEPTION_GUARD_START

	CsDetected d;

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	// message pkg is for get icon temporary.
	d.pkgId = TEST_TPK_PKG_ID;
	d.malwareName = "dummy malware";
	d.detailedUrl = "http://detailedurl/cs_prompt_app";
	d.severity = CSR_CS_SEVERITY_MEDIUM;

	Ui::AskUser askuser;
	printPressed(askuser.cs(Ui::CommandId::CS_PROMPT_APP,
							"Message for prompt app tc", d));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(prompt_file)
{
	EXCEPTION_GUARD_START

	CsDetected d;
	d.targetName = "/opt/apps/csr/test/dummyfile";
	d.malwareName = "dummy malware";
	d.detailedUrl = "http://detailedurl/cs_prompt_file";
	d.severity = CSR_CS_SEVERITY_MEDIUM;

	Ui::AskUser askuser;
	printPressed(askuser.cs(Ui::CommandId::CS_PROMPT_FILE,
							"Message for prompt file tc", d));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(notify_data)
{
	EXCEPTION_GUARD_START

	CsDetected d;
	d.targetName.clear(); // data's target name should be empty
	d.malwareName = "dummy malware";
	d.detailedUrl = "http://detailedurl/cs_notify_data";
	d.severity = CSR_CS_SEVERITY_HIGH;

	Ui::AskUser askuser;
	printPressed(askuser.cs(Ui::CommandId::CS_NOTIFY_DATA,
							"Message for notify data tc", d));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(notify_app)
{
	EXCEPTION_GUARD_START

	CsDetected d;

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	d.pkgId = TEST_WGT_PKG_ID;
	d.malwareName = "dummy malware";
	d.detailedUrl = "http://detailedurl/cs_notify_app";
	d.severity = CSR_CS_SEVERITY_HIGH;

	Ui::AskUser askuser;
	printPressed(askuser.cs(Ui::CommandId::CS_NOTIFY_APP,
							"Message for notify app tc", d));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(notify_file)
{
	EXCEPTION_GUARD_START

	CsDetected d;
	d.targetName = "/opt/apps/csr/test/dummyfile";
	d.malwareName = "dummy malware";
	d.detailedUrl = "http://detailedurl/cs_notify_file";
	d.severity = CSR_CS_SEVERITY_HIGH;

	Ui::AskUser askuser;
	auto response = askuser.cs(Ui::CommandId::CS_NOTIFY_FILE,
							   "Message for notify file tc", d);
	printPressed(response);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END() // CS

BOOST_AUTO_TEST_SUITE(WP)

BOOST_AUTO_TEST_CASE(prompt)
{
	EXCEPTION_GUARD_START

	Ui::UrlItem item;
	item.risk = CSR_WP_RISK_MEDIUM;
	item.url = "http://csr.test.dummyurl.com";

	Ui::AskUser askuser;
	printPressed(askuser.wp(Ui::CommandId::WP_PROMPT, "Message for wp_prompt tc",
							item));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(notify)
{
	EXCEPTION_GUARD_START

	Ui::UrlItem item;
	item.risk = CSR_WP_RISK_HIGH;
	item.url = "http://csr.test.dummyurl.com";

	Ui::AskUser askuser;
	printPressed(askuser.wp(Ui::CommandId::WP_NOTIFY, "Message for wp_notify tc",
							item));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END() // WP

BOOST_AUTO_TEST_SUITE_END() // POPUP
