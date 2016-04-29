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

bool isCsCommand(const CommandId &cid)
{
	switch (cid) {
	case CommandId::CS_PROMPT_DATA:
	case CommandId::CS_PROMPT_APP:
	case CommandId::CS_PROMPT_FILE:
	case CommandId::CS_NOTIFY_DATA:
	case CommandId::CS_NOTIFY_APP:
	case CommandId::CS_NOTIFY_FILE:
		return true;

	case CommandId::WP_PROMPT:
	case CommandId::WP_NOTIFY:
		return false;

	default:
		ThrowExc(InternalError, "Protocol error. unknown popup-service command id.");
	}
}

void addButton(int *rp, const std::string &buttonPart,
			   const std::string &buttonText,
			   Popup &popup, RawBuffer &result)
{
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

RawBuffer Logic::dispatch(const RawBuffer &in)
{
	auto info = getRequestInfo(in);
	INFO("Request dispatch on popup-service. CommandId: " << static_cast<int>
		 (info.first));

	if (isCsCommand(info.first)) {
		std::string message;
		CsDetected d;
		info.second.Deserialize(message, d);

		switch (info.first) {
		case CommandId::CS_PROMPT_DATA:
			return csPromptData(message, d);

		case CommandId::CS_PROMPT_APP:
			return csPromptApp(message, d);

		case CommandId::CS_PROMPT_FILE:
			return csPromptFile(message, d);

		case CommandId::CS_NOTIFY_DATA:
			return csNotifyData(message, d);

		case CommandId::CS_NOTIFY_APP:
			return csNotifyApp(message, d);

		case CommandId::CS_NOTIFY_FILE:
			return csNotifyFile(message, d);

		default:
			ThrowExc(InternalError, "protocol error. invalid ui command id.");
		}
	} else {
		std::string message;
		UrlItem item;
		info.second.Deserialize(message, item);

		switch (info.first) {
		case CommandId::WP_PROMPT:
			return wpPrompt(message, item);

		case CommandId::WP_NOTIFY:
			return wpNotify(message, item);

		default:
			ThrowExc(InternalError, "protocol error. invalid ui command id.");
		}
	}
}

std::pair<CommandId, BinaryQueue> Logic::getRequestInfo(const RawBuffer &data)
{
	BinaryQueue q;
	q.push(data);

	int int_id;
	q.Deserialize(int_id);

	return std::make_pair(static_cast<CommandId>(int_id), std::move(q));
}

RawBuffer Logic::csPromptData(const std::string &message,
							  const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected on data", FORMAT(
					   "Malware: " << d.malwareName << "\n" <<
					   "Severity: " << static_cast<int>(d.severity) << "\n" <<
					   "Do you want to stop processing?" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);
	resps[0] = static_cast<int>(CSR_CS_PROCESSING_ALLOWED);
	resps[1] = static_cast<int>(CSR_CS_PROCESSING_DISALLOWED);

	addButton(&resps[0], "button1", "yes", popup, result);
	addButton(&resps[1], "button2", "no", popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csPromptFile(const std::string &message,
							  const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "File: " << d.targetName << "\n" <<
					   "Malware: " << d.malwareName << "\n" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[3]);
	resps[0] = static_cast<int>(CSR_CS_REMOVE);
	resps[1] = static_cast<int>(CSR_CS_IGNORE);
	resps[2] = static_cast<int>(CSR_CS_SKIP);

	addButton(&resps[0], "button1", "remove", popup, result);
	addButton(&resps[1], "button2", "ignore", popup, result);
	addButton(&resps[2], "button3", "skip", popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csPromptApp(const std::string &message,
							 const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "App: " << d.targetName << "\n" <<
					   "Malware: " << d.malwareName << "\n" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[3]);
	resps[0] = static_cast<int>(CSR_CS_REMOVE);
	resps[1] = static_cast<int>(CSR_CS_IGNORE);
	resps[2] = static_cast<int>(CSR_CS_SKIP);

	addButton(&resps[0], "button1", "remove", popup, result);
	addButton(&resps[1], "button2", "ignore", popup, result);
	addButton(&resps[2], "button3", "skip", popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csNotifyData(const std::string &message,
							  const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected on data", FORMAT(
					   "Malware: " << d.malwareName << "\n" <<
					   "Severity: " << static_cast<int>(d.severity) << "\n" <<
					   "Do you want to stop processing?" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);
	resps[0] = static_cast<int>(CSR_CS_PROCESSING_ALLOWED);
	resps[1] = static_cast<int>(CSR_CS_PROCESSING_DISALLOWED);

	addButton(&resps[0], "button1", "yes", popup, result);
	addButton(&resps[1], "button2", "no", popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csNotifyFile(const std::string &message,
							  const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "File: " << d.targetName << "\n" <<
					   "Malware: " << d.malwareName << "\n" <<
					   message.c_str()));

	std::unique_ptr<int[]> resps(new int[2]);
	resps[0] = static_cast<int>(CSR_CS_REMOVE);
	resps[1] = static_cast<int>(CSR_CS_SKIP);

	RawBuffer result;
	addButton(&resps[0], "button1", "remove", popup, result);
	addButton(&resps[1], "button2", "skip", popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::csNotifyApp(const std::string &message,
							 const CsDetected &d) const
{
	Popup popup;

	popup.fillText("Virus detected", FORMAT(
					   "App: " << d.targetName << "\n" <<
					   "Malware: " << d.malwareName << "\n" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);
	resps[0] = static_cast<int>(CSR_CS_REMOVE);
	resps[1] = static_cast<int>(CSR_CS_SKIP);

	addButton(&resps[0], "button1", "remove", popup, result);
	addButton(&resps[1], "button2", "skip", popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::wpPrompt(const std::string &message, const UrlItem &item) const
{
	Popup popup;

	popup.fillText("Danger URL", FORMAT(
					   "URL: " << item.url << "\n" <<
					   "Risky: " << (item.risk == CSR_WP_RISK_HIGH
									 ? "High" : "Medium") << "\n" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int[]> resps(new int[2]);
	resps[0] = static_cast<int>(CSR_WP_PROCESSING_ALLOWED);
	resps[1] = static_cast<int>(CSR_WP_PROCESSING_DISALLOWED);

	addButton(&resps[0], "button1", "allow", popup, result);
	addButton(&resps[1], "button2", "deny", popup, result);

	popup.start();

	return result;
}

RawBuffer Logic::wpNotify(const std::string &message, const UrlItem &item) const
{
	Popup popup;

	popup.fillText("Danger URL", FORMAT(
					   "URL: " << item.url << "\n" <<
					   "Risky: " << (item.risk == CSR_WP_RISK_HIGH
									 ? "High" : "Medium") << "\n" <<
					   message.c_str()));

	RawBuffer result;

	std::unique_ptr<int> resp(new int);
	*resp = static_cast<int>(CSR_WP_PROCESSING_DISALLOWED);

	addButton(resp.get(), "button1", "confirm", popup, result);

	popup.start();

	return result;
}

}
}
