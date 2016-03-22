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

#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <Elementary.h>

#include "common/message-buffer.h"
#include "common/raw-buffer.h"
#include "common/audit/logger.h"
#include "ui/common.h"

namespace Csr {
namespace Ui {

namespace {

std::unordered_map<int, std::function<void()>> g_callbackRegistry;

void evasCbWrapper(void *data, Evas_Object *, void *)
{
	auto response = reinterpret_cast<int *>(data);
	g_callbackRegistry[*response]();
}

void registerCb(Evas_Object *button, int *response, std::function<void()> &&func)
{
	evas_object_smart_callback_add(button, "clicked", evasCbWrapper, response);
	g_callbackRegistry[*response] = std::move(func);
}

void unregisterCb(int response)
{
	g_callbackRegistry.erase(response);
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
	INFO("Request dispatch on popup-service. CommandId: " << static_cast<int>(info.first));

	switch (info.first) {
	case CommandId::FILE_SINGLE: {
		std::string message;
		FileItem item;
		info.second.Deserialize(message, item);

		return fileSingle(message, item);
	}

	case CommandId::FILE_MULTIPLE: {
		std::string message;
		FileItems items;
		info.second.Deserialize(message, items);

		return fileMultiple(message, items);
	}

	case CommandId::URL_SINGLE: {
		std::string message;
		UrlItem item;
		info.second.Deserialize(message, item);

		return urlSingle(message, item);
	}

	case CommandId::URL_MULTIPLE: {
		std::string message;
		UrlItems items;
		info.second.Deserialize(message, items);

		return urlMultiple(message, items);
	}

	default:
		throw std::logic_error("Protocol error. unknown popup-service command id.");
	}
}

std::pair<CommandId, MessageBuffer> Logic::getRequestInfo(const RawBuffer &data)
{
	int int_id;
	MessageBuffer msg;
	msg.push(data);
	msg.Deserialize(int_id);

	return std::make_pair(static_cast<CommandId>(int_id), std::move(msg));
}

RawBuffer Logic::fileSingle(const std::string &message, const FileItem &item) const
{
	INFO("fileSingle start with param.. "
		"message[" << message << "] "
		"filepath[" << item.filepath << "] "
		"severity[" << static_cast<int>(item.severity) << "] "
		"threat[" << static_cast<int>(item.threat) << "]");

	Popup popup;

	popup.fillText("FileSingle title", message.c_str());

	MessageBuffer msg;

	auto respRemove = static_cast<int>(Response::REMOVE);
	Evas_Object *remove = popup.addButton("button1");
	elm_object_text_set(remove, "remove");
	registerCb(remove, &respRemove, [&msg, &respRemove, &popup]() {
		DEBUG("Remove button clicked!");
		msg = MessageBuffer::Serialize(respRemove);
		popup.stop();
	});

	auto respIgnore = static_cast<int>(Response::IGNORE);
	Evas_Object *ignore = popup.addButton("button2");
	elm_object_text_set(ignore, "ignore");
	registerCb(ignore, &respIgnore, [&msg, &respIgnore, &popup]() {
		DEBUG("Ignore button clicked!");
		msg = MessageBuffer::Serialize(respIgnore);
		popup.stop();
	});

	auto respSkip = static_cast<int>(Response::SKIP);
	Evas_Object *skip = popup.addButton("button3");
	elm_object_text_set(skip, "skip");
	registerCb(skip, &respSkip, [&msg, &respSkip, &popup]() {
		DEBUG("Skip button clicked!");
		msg = MessageBuffer::Serialize(respSkip);
		popup.stop();
	});

	popup.start();

	DEBUG("fileSingle done!");

	unregisterCb(respRemove);
	unregisterCb(respIgnore);
	unregisterCb(respSkip);

	return msg.pop();
}

RawBuffer Logic::fileMultiple(const std::string &message, const FileItems &items) const
{
	(void) message;
	(void) items;
	return RawBuffer();
}

RawBuffer Logic::urlSingle(const std::string &message, const UrlItem &item) const
{
	INFO("urlSingle start with param.. "
		"message[" << message << "] "
		"url[" << item.url << "] "
		"risk level[" << static_cast<int>(item.risk) << "]");

	Popup popup;

	popup.fillText("FileSingle title", message.c_str());

	MessageBuffer msg;

	auto respAllow = static_cast<int>(Response::ALLOW);
	Evas_Object *allow = popup.addButton("button1");
	elm_object_text_set(allow, "allow");
	registerCb(allow, &respAllow, [&msg, &respAllow, &popup]() {
		DEBUG("Allow button clicked!");
		msg = MessageBuffer::Serialize(respAllow);
		popup.stop();
	});

	auto respDeny = static_cast<int>(Response::DENY);
	Evas_Object *deny = popup.addButton("button2");
	elm_object_text_set(deny, "deny");
	registerCb(deny, &respDeny, [&msg, &respDeny, &popup]() {
		DEBUG("Deny button clicked!");
		msg = MessageBuffer::Serialize(respDeny);
		popup.stop();
	});

	popup.start();

	DEBUG("urlSingle done!");

	unregisterCb(respAllow);
	unregisterCb(respDeny);

	return msg.pop();
}

RawBuffer Logic::urlMultiple(const std::string &message, const UrlItems &items) const
{
	(void) message;
	(void) items;
	return RawBuffer();
}

}
}
