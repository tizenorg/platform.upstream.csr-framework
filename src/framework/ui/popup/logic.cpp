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
 * @file        logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Logic for popup service
 */
#include "logic.h"

#include <functional>
#include <unordered_map>
#include <Elementary.h>

#include "common/binary-queue.h"
#include "common/audit/logger.h"
#include "common/exception.h"
#include "ui/common.h"
#include "package-info.h"
#include "popup.h"

#include <csr-content-screening-types.h>
#include <csr-web-protection-types.h>

namespace Csr {
namespace Ui {

namespace {
void split(const std::string &s, std::string &fileName, std::string &extraPath)
{
	std::string delimiter = "/";
	size_t idx = s.rfind(delimiter);

	fileName = s.substr(idx + 1);
	extraPath = s.substr(0, idx);
}
} // namespace anonymous

RawBuffer Logic::csPromptData(const std::string &message, const CsDetected &d) const
{
	std::string risk(d.severity == CSR_CS_SEVERITY_LOW ? "Low" : "Medium");
	Popup p(1);

	p.setMessage(message);
	p.setTitle("    Malware detected");
	p.setHeader("    Malware which is harm your phone is detected.");
	p.setBody(FORMAT("    - Risk : " << risk << " (" << d.malwareName << ")"));
	p.setFooter("    Processing is prohibited to protect your phone.");

	p.setText(p.m_buttons[0], "OK");

	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_DISALLOWED));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_hypertext, d.detailedUrl);

	p.run();
	return p.getResult();
}

RawBuffer Logic::csPromptFile(const std::string &message, const CsDetected &d) const
{
	std::string risk(d.severity == CSR_CS_SEVERITY_LOW ? "Low" : "Medium");
	std::string fileName, extraPath;
	split(d.targetName, fileName, extraPath);

	Popup p(3);

	p.setMessage(message);
	p.setTitle("    Malware detected");
	p.setHeader("    Malware which is harm your phone is detected.");
	p.setBody(FORMAT(
		"    - File name : " << fileName << "<br>" <<
		"    - Path : " << extraPath << "<br>" <<
		"    - Risk : " << risk << " (" << d.malwareName << ")"));
	p.setFooter(
		"    Tap Delete to delete infected files and<br>"
		"    protect your phone. If you really want to<br>"
		"    process anyway, tap Ignore.");

	p.setText(p.m_buttons[0], "OK");
	p.setText(p.m_buttons[1], "Ignore");
	p.setText(p.m_buttons[2], "Delete");

	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_DISALLOWED));
	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_ALLOWED));
	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_REMOVE));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_buttons[1], &p.m_types[1]);
	p.callbackRegister(p.m_buttons[2], &p.m_types[2]);
	p.callbackRegister(p.m_hypertext, d.detailedUrl);

	p.run();
	return p.getResult();
}

RawBuffer Logic::csPromptApp(const std::string &message, const CsDetected &d) const
{
	std::string risk(d.severity == CSR_CS_SEVERITY_LOW ? "Low" : "Medium");
	PackageInfo info(d.pkgId);

	Popup p(3);

	p.setMessage(message);
	p.setTitle("    Malware detected");
	p.setHeader("    Malware which is harm your phone is detected.");
	p.setBody(FORMAT(
		"    App name : " << info.getLabel() << "<br>" <<
		"    Version : " << info.getVersion() << "<br>" <<
		"    Risk : " << risk << " (" << d.malwareName << ")"));
	p.setIcon(info.getIconPath());
	p.setFooter(
		"    Tap Uninstall to uninstall infected<br>"
		"    application and protect your phone.<br>"
		"    If you really want to process anyway, tap Ignore.");

	p.setText(p.m_buttons[0], "OK");
	p.setText(p.m_buttons[1], "Ignore");
	p.setText(p.m_buttons[2], "Uninstall");

	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_DISALLOWED));
	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_ALLOWED));
	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_REMOVE));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_buttons[1], &p.m_types[1]);
	p.callbackRegister(p.m_buttons[2], &p.m_types[2]);
	p.callbackRegister(p.m_hypertext, d.detailedUrl);

	p.run();
	return p.getResult();
}

RawBuffer Logic::csNotifyData(const std::string &message, const CsDetected &d) const
{
	Popup p(1);

	p.setMessage(message);
	p.setTitle("    Malware detected");
	p.setHeader("    Malware which is harm your phone is detected.");
	p.setBody(FORMAT("    - Risk : " << "High" << " (" << d.malwareName << ")"));
	p.setFooter("    Processing is prohibited to protect your phone.");

	p.setText(p.m_buttons[0], "OK");

	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_DISALLOWED));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_hypertext, d.detailedUrl);

	p.run();
	return p.getResult();
}

RawBuffer Logic::csNotifyFile(const std::string &message, const CsDetected &d) const
{
	Popup p(2);
	std::string fileName, extraPath;
	split(d.targetName, fileName, extraPath);

	p.setMessage(message);
	p.setTitle("    Malware detected");
	p.setHeader("    Malware which is harm your phone is detected.");
	p.setBody(FORMAT(
		"    - File name : " << fileName << "<br>" <<
		"    - Path : " << extraPath << "<br>" <<
		"    - Risk : " << "High" << " (" << d.malwareName << ")"));
	p.setFooter(
		"    Tap Delete to delete infected files and<br>"
		"    protect your phone.");

	p.setText(p.m_buttons[0], "OK");
	p.setText(p.m_buttons[1], "Delete");

	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_DISALLOWED));
	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_REMOVE));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_buttons[1], &p.m_types[1]);
	p.callbackRegister(p.m_hypertext, d.detailedUrl);

	p.run();
	return p.getResult();
}

RawBuffer Logic::csNotifyApp(const std::string &message, const CsDetected &d) const
{
	PackageInfo info(d.pkgId);

	Popup p(2);

	p.setMessage(message);
	p.setTitle("    Malware detected");
	p.setHeader("    Malware which is harm your phone is detected.");
	p.setIcon(info.getIconPath());
	p.setBody(FORMAT(
		"    App name : " << info.getLabel() << "<br>" <<
		"    Version : " << info.getVersion() << "<br>" <<
		"    Risk : " << "High" << " (" << d.malwareName << ")"));
	p.setFooter(
		"    Tap Uninstall to uninstall infected<br>"
		"    application and protect your phone.");

	p.setText(p.m_buttons[0], "OK");
	p.setText(p.m_buttons[1], "Uninstall");

	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_DISALLOWED));
	p.m_types.emplace_back(
		static_cast<int>(CSR_CS_USER_RESPONSE_PROCESSING_ALLOWED));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_buttons[1], &p.m_types[1]);
	p.callbackRegister(p.m_hypertext, d.detailedUrl);

	p.run();
	return p.getResult();
}

RawBuffer Logic::wpPrompt(const std::string &message, const UrlItem &item) const
{
	std::string risk(item.risk == CSR_WP_RISK_LOW ? "Low" : "Medium");

	Popup p(1);

	p.setMessage(message);
	p.setTitle("Block malicious URL");
	p.setHeader("    This website may harm your phone.");
	p.setBody(FORMAT(
		"    - URL : " << item.url << "<br>" <<
		"    - Risk : " << risk));
	p.setFooter(
		"    Accessing to this URL is prohibited to<br>"
		"    protect your phone.");

	p.setText(p.m_buttons[0], "OK");

	p.m_types.emplace_back(static_cast<int>(CSR_WP_USER_RESPONSE_PROCESSING_DISALLOWED));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_hypertext, item.url);

	p.run();
	return p.getResult();
}

RawBuffer Logic::wpNotify(const std::string &message, const UrlItem &item) const
{
	Popup p(2);

	p.setMessage(message);
	p.setTitle("Block malicious URL");
	p.setHeader("   This website may harm your phone.");
	p.setBody(FORMAT(
		"    - URL : " << item.url << "<br>" <<
		"    - Risk : " << "High"));
	p.setFooter(
		"   If you really want to process anyway,<br>"
		"   tap View");

	p.setText(p.m_buttons[0], "OK");
	p.setText(p.m_buttons[1], "View");

	p.m_types.emplace_back(
		static_cast<int>(CSR_WP_USER_RESPONSE_PROCESSING_DISALLOWED));
	p.m_types.emplace_back(
		static_cast<int>(CSR_WP_USER_RESPONSE_PROCESSING_ALLOWED));

	p.callbackRegister(p.m_buttons[0], &p.m_types[0]);
	p.callbackRegister(p.m_buttons[1], &p.m_types[1]);
	p.callbackRegister(p.m_hypertext, item.url);

	p.run();
	return p.getResult();
}
} // namespace Ui
} // namespace Csr
