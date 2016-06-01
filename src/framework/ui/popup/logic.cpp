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

#include "popup-wp-prompt.h"
#include "popup-wp-notify.h"
#include "popup-cs-prompt-app.h"
#include "popup-cs-notify-app.h"
#include "popup-cs-prompt-file.h"
#include "popup-cs-notify-file.h"
#include "popup-cs-prompt-data.h"
#include "popup-cs-notify-data.h"

#include <csr-content-screening-types.h>
#include <csr-web-protection-types.h>

namespace Csr {
namespace Ui {

RawBuffer Logic::csPromptData(const std::string &message, const CsDetected &d) const
{
	PopupCsPromptData popup("High", d.malwareName);
	popup.setMessage(message);
	popup.run();
	DEBUG(popup.getResult().data());
	return popup.getResult();
}

RawBuffer Logic::csPromptFile(const std::string &message, const CsDetected &d) const
{
	PopupCsPromptFile popup(d.targetName, "path", "High", d.malwareName);
	popup.setMessage(message);
	popup.run();
	return popup.getResult();
}

RawBuffer Logic::csPromptApp(const std::string &message, const CsDetected &d) const
{
	PopupCsPromptApp popup(d.targetName, "1.0", "High", d.malwareName);
	popup.setMessage(message);
	popup.run();
	return popup.getResult();
}

RawBuffer Logic::csNotifyData(const std::string &message, const CsDetected &d) const
{
	std::string risk(d.severity == CSR_CS_SEVERITY_LOW ? "Low" : "Medium");
	PopupCsNotifyData popup(risk, d.malwareName);
	popup.setMessage(message);
	popup.run();
	return popup.getResult();
}

RawBuffer Logic::csNotifyFile(const std::string &message, const CsDetected &d) const
{
	std::string risk(d.severity == CSR_CS_SEVERITY_LOW ? "Low" : "Medium");
	PopupCsNotifyFile popup(d.targetName, "path", risk, d.malwareName);
	popup.setMessage(message);
	popup.run();
	return popup.getResult();
}

RawBuffer Logic::csNotifyApp(const std::string &message, const CsDetected &d) const
{
	std::string risk(d.severity == CSR_CS_SEVERITY_LOW ? "Low" : "Medium");
	PopupCsNotifyApp popup(d.targetName, "1.0", risk, d.malwareName);
	popup.setMessage(message);
	popup.run();
	return popup.getResult();
}

RawBuffer Logic::wpPrompt(const std::string &message, const UrlItem &item) const
{
	PopupWpPrompt popup(item.url, "High");
	popup.setMessage(message);
	popup.run();
	return popup.getResult();
}

RawBuffer Logic::wpNotify(const std::string &message, const UrlItem &item) const
{
	std::string risk(item.risk == CSR_WP_RISK_LOW ? "Low" : "Medium");
	PopupWpNotify popup(item.url, risk);
	popup.setMessage(message);
	popup.run();
	return popup.getResult();
}
} // namespace Ui
} // namespace Csr
