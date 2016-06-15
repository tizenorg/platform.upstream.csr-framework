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
#include "popup-string.h"

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
	std::string risk(d.severity ==
		CSR_CS_SEVERITY_LOW ? LABEL_RISK_LEVEL_LOW : LABEL_RISK_LEVEL_MEDIUM);

	Popup p(1);

	p.setMessage(message);
	p.setTitle(CS_TITLE);
	p.setHeader(CS_PROMPT_DATA_HEADER);
	p.setBody(FORMAT(
		"- " << LABEL_RISK << risk << " (" << d.malwareName << ")"));
	p.setFooter(CS_PROMPT_DATA_FOOTER);

	p.setText(p.m_buttons[0], BTN_OK);
	p.setText(p.m_buttons[1], BTN_IGNORE);

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

RawBuffer Logic::csPromptFile(const std::string &message, const CsDetected &d) const
{
	std::string risk(d.severity ==
		CSR_CS_SEVERITY_LOW ? LABEL_RISK_LEVEL_LOW : LABEL_RISK_LEVEL_MEDIUM);
	std::string fileName, extraPath;
	split(d.targetName, fileName, extraPath);

	Popup p(3);

	p.setMessage(message);
	p.setTitle(CS_TITLE);
	p.setHeader(CS_PROMPT_FILE_HEADER);
	p.setBody(FORMAT(
		"- " << LABEL_FILE_NAME << fileName << "<br>" <<
		"- " << LABEL_FILE_PATH << extraPath << "<br>" <<
		"- " << LABEL_RISK << risk << " (" << d.malwareName << ")"));
	std::string prefix(CS_NOTIFY_FILE_FOOTER);
	p.setFooter(FORMAT(prefix << "<br>" << CS_PROMPT_FILE_FOOTER));

	p.setText(p.m_buttons[0], BTN_OK);
	p.setText(p.m_buttons[1], BTN_IGNORE);
	p.setText(p.m_buttons[2], BTN_DELETE);

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
	std::string risk(d.severity ==
		CSR_CS_SEVERITY_LOW ? LABEL_RISK_LEVEL_LOW : LABEL_RISK_LEVEL_MEDIUM);
	PackageInfo info(d.pkgId);

	Popup p(3);

	p.setMessage(message);
	p.setTitle(CS_TITLE);
	p.setHeader(CS_PROMPT_APP_HEADER);
	p.setBody(FORMAT(
		LABEL_APP_NAME << info.getLabel() << "<br>" <<
		LABEL_VERSION << info.getVersion() << "<br>" <<
		LABEL_RISK << risk << " (" << d.malwareName << ")"));
	p.setIcon(info.getIconPath());
	std::string prefix(CS_NOTIFY_APP_FOOTER);
	p.setFooter(FORMAT(prefix << "<br>" << CS_PROMPT_APP_FOOTER));

	p.setText(p.m_buttons[0], BTN_OK);
	p.setText(p.m_buttons[1], BTN_IGNORE);
	p.setText(p.m_buttons[2], BTN_UNINSTALL);

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
	p.setTitle(CS_TITLE);
	p.setHeader(CS_NOTIFY_DATA_HEADER);
	p.setBody(FORMAT(
		"- " << LABEL_RISK << LABEL_RISK_LEVEL_HIGH <<
		" (" << d.malwareName << ")"));
	p.setFooter(CS_NOTIFY_DATA_FOOTER);

	p.setText(p.m_buttons[0], BTN_OK);

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
	p.setTitle(CS_TITLE);
	p.setHeader(CS_NOTIFY_FILE_HEADER);
	p.setBody(FORMAT(
		"- " << LABEL_FILE_NAME << fileName << "<br>" <<
		"- " << LABEL_FILE_PATH << extraPath << "<br>" <<
		"- " << LABEL_RISK << LABEL_RISK_LEVEL_HIGH <<
		" (" << d.malwareName << ")"));
	p.setFooter(CS_NOTIFY_FILE_FOOTER);

	p.setText(p.m_buttons[0], BTN_OK);
	p.setText(p.m_buttons[1], BTN_DELETE);

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
	p.setTitle(CS_TITLE);
	p.setHeader(CS_NOTIFY_APP_HEADER);
	p.setIcon(info.getIconPath());
	p.setBody(FORMAT(
		LABEL_APP_NAME << info.getLabel() << "<br>" <<
		LABEL_VERSION << info.getVersion() << "<br>" <<
		LABEL_RISK << LABEL_RISK_LEVEL_HIGH << " (" << d.malwareName << ")"));
	p.setFooter(CS_NOTIFY_APP_FOOTER);

	p.setText(p.m_buttons[0], BTN_OK);
	p.setText(p.m_buttons[1], BTN_UNINSTALL);

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
	std::string risk(item.risk ==
		CSR_WP_RISK_LOW ? LABEL_RISK_LEVEL_LOW : LABEL_RISK_LEVEL_MEDIUM);

	Popup p(1);

	p.setMessage(message);
	p.setTitle(WP_TITLE);
	p.setHeader(WP_PROMPT_HEADER);
	p.setBody(FORMAT(
		"- " << LABEL_URL << item.url << "<br>" <<
		"- " << LABEL_RISK << risk));
	p.setFooter(WP_PROMPT_FOOTER);

	p.setText(p.m_buttons[0], BTN_OK);

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
	p.setTitle(WP_TITLE);
	p.setHeader(WP_NOTIFY_HEADER);
	p.setBody(FORMAT(
		"- " << LABEL_URL << item.url << "<br>" <<
		"- " << LABEL_RISK << LABEL_RISK_LEVEL_HIGH));
	p.setFooter(WP_NOTIFY_FOOTER);

	p.setText(p.m_buttons[0], BTN_OK);
	p.setText(p.m_buttons[1], BTN_VIEW);

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
