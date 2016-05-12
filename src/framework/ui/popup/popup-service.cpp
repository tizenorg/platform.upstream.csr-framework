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
 * @file        popup-service.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup-service.h"

#include "common/binary-queue.h"
#include "common/audit/logger.h"
#include "common/exception.h"
#include "common/cs-detected.h"

namespace Csr {
namespace Ui {

namespace {

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

} // namespace nonymous

PopupService::PopupService() : Service()
{
	this->add(SockId::POPUP);
}

PopupService::~PopupService()
{
}

RawBuffer PopupService::process(const ConnShPtr &, RawBuffer &data)
{
	BinaryQueue q;
	q.push(data);

	int intCid;
	q.Deserialize(intCid);

	INFO("Request dispatch on popup-service. CommandId: " << static_cast<int>(intCid));

	if (isCsCommand(static_cast<Ui::CommandId>(intCid))) {
		std::string message;
		CsDetected d;
		q.Deserialize(message, d);

		switch (static_cast<Ui::CommandId>(intCid)) {
		case CommandId::CS_PROMPT_DATA:
			return m_logic.csPromptData(message, d);

		case CommandId::CS_PROMPT_APP:
			return m_logic.csPromptApp(message, d);

		case CommandId::CS_PROMPT_FILE:
			return m_logic.csPromptFile(message, d);

		case CommandId::CS_NOTIFY_DATA:
			return m_logic.csNotifyData(message, d);

		case CommandId::CS_NOTIFY_APP:
			return m_logic.csNotifyApp(message, d);

		case CommandId::CS_NOTIFY_FILE:
			return m_logic.csNotifyFile(message, d);

		default:
			ThrowExc(InternalError, "protocol error. invalid ui command id.");
		}
	} else {
		std::string message;
		UrlItem item;
		q.Deserialize(message, item);

		switch (static_cast<Ui::CommandId>(intCid)) {
		case CommandId::WP_PROMPT:
			return m_logic.wpPrompt(message, item);

		case CommandId::WP_NOTIFY:
			return m_logic.wpNotify(message, item);

		default:
			ThrowExc(InternalError, "protocol error. invalid ui command id.");
		}
	}
}

void PopupService::onMessageProcess(const ConnShPtr &connection)
{
	DEBUG("process message on popup service");

	auto in = connection->receive();
	connection->send(this->process(connection, in));

	DEBUG("process done on popup service");
}

} // namespace Ui
} // namespace Csr
