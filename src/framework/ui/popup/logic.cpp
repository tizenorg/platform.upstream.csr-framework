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
#include "popup.h"

#include "csr/content-screening-types.h"
#include "csr/web-protection-types.h"

namespace Csr {
namespace Ui {

namespace {

std::unordered_map<int, std::function<void()>> g_callbackRegistry;

void evasCbWrapper(void *data, Evas_Object *, void *)
{
	auto response = reinterpret_cast<int *>(data);
	g_callbackRegistry[*response]();
	g_callbackRegistry.clear();
}

void registerCb(Evas_Object *button, int *rp, std::function<void()> &&func)
{
	evas_object_smart_callback_add(button, "clicked", evasCbWrapper, rp);
	g_callbackRegistry[*rp] = std::move(func);
}

void addButton(int response, int *rp, const std::string &buttonPart,
			   const std::string &buttonText, Popup &popup, RawBuffer &result)
{
	*rp = response;

	auto button = popup.addButton(buttonPart);
	elm_object_text_set(button, buttonText.c_str());

	registerCb(button, rp, [rp, &result, &popup]() {
		DEBUG("Button for response[" << *rp << "] clicked!");
		result = BinaryQueue::Serialize(rp).pop();
		popup.stop();
	});
}

} // namespace anonymous

Logic::Logic()
{
}

Logic::~Logic()
{
}

RawBuffer Logic::csPromptData(const std::string &message,
							  const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected on data", FORMAT(
					   "Malware: " << d.malwareName << "<br>" <<
					   "Severity: " << static_cast<int>(d.severity) << "<br>" <<
					   "Do you want to stop processing?" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);

	addButton(static_cast<int>(CSR_CS_PROCESSING_ALLOWED), &resps[0], "button1", "yes",
			  popup, result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED), &resps[1], "button2", "no",
			  popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csPromptFile(const std::string &message, const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "File: " << d.targetName << "<br>" <<
					   "Malware: " << d.malwareName << "<br>" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[3]);

	addButton(static_cast<int>(CSR_CS_REMOVE), &resps[0], "button1", "remove", popup,
			  result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_ALLOWED), &resps[1], "button2", "allow",
			  popup, result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED), &resps[2], "button3", "deny",
			  popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csPromptApp(const std::string &message, const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "App: " << d.targetName << "<br>" <<
					   "Malware: " << d.malwareName << "<br>" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[3]);

	addButton(static_cast<int>(CSR_CS_REMOVE), &resps[0], "button1", "remove", popup,
			  result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_ALLOWED), &resps[1], "button2", "allow",
			  popup, result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED), &resps[2], "button3", "deny",
			  popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csNotifyData(const std::string &message, const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected on data", FORMAT(
					   "Malware: " << d.malwareName << "<br>" <<
					   "Severity: " << static_cast<int>(d.severity) << "<br>" <<
					   "Do you want to stop processing?" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);

	addButton(static_cast<int>(CSR_CS_PROCESSING_ALLOWED), &resps[0], "button1", "yes",
			  popup, result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED), &resps[1], "button2", "no",
			  popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csNotifyFile(const std::string &message, const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "File: " << d.targetName << "<br>" <<
					   "Malware: " << d.malwareName << "<br>" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);

	addButton(static_cast<int>(CSR_CS_REMOVE), &resps[0], "button1", "remove", popup,
			  result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED), &resps[1], "button2", "deny",
			  popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csNotifyApp(const std::string &message, const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "App: " << d.targetName << "<br>" <<
					   "Malware: " << d.malwareName << "<br>" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);

	addButton(static_cast<int>(CSR_CS_REMOVE), &resps[0], "button1", "remove", popup,
			  result);
	addButton(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED), &resps[1], "button2", "deny",
			  popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::wpPrompt(const std::string &message, const UrlItem &item) const
{
	Popup popup;

	popup.fillText("Danger URL", FORMAT(
					   "URL: " << item.url << "<br>" <<
					   "Risky: " << (item.risk == CSR_WP_RISK_HIGH
									 ? "High" : "Medium") << "<br>" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);

	addButton(static_cast<int>(CSR_WP_PROCESSING_ALLOWED), &resps[0], "button1", "allow",
			  popup, result);
	addButton(static_cast<int>(CSR_WP_PROCESSING_DISALLOWED), &resps[1], "button2", "deny",
			  popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::wpNotify(const std::string &message, const UrlItem &item) const
{
	Popup popup;

	popup.fillText("Danger URL", FORMAT(
					   "URL: " << item.url << "<br>" <<
					   "Risky: " << (item.risk == CSR_WP_RISK_HIGH
									 ? "High" : "Medium") << "<br>" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int> resp(new int);

	addButton(static_cast<int>(CSR_WP_PROCESSING_DISALLOWED), resp.get(), "button1",
			  "confirm", popup, result);

	popup.start();

	return result;
}

}
}
