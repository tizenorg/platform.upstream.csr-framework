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
 * @file        server-service.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/server-service.h"

#include <string>
#include <vector>

#include "common/command-id.h"
#include "common/audit/logger.h"
#include "common/exception.h"
#include "common/cs-context.h"
#include "common/wp-context.h"

#include "csr/content-screening-types.h"

namespace Csr {

ServerService::ServerService() : Service(), m_workqueue(2, 10)
{
	this->add(SockId::CS);
	this->add(SockId::WP);
	this->add(SockId::ADMIN);
}

ServerService::~ServerService()
{
}

RawBuffer ServerService::process(const ConnShPtr &, RawBuffer &data)
{
	CommandId id;

	BinaryQueue q;
	q.push(data);
	q.Deserialize(id);

	switch (id) {
	case CommandId::SCAN_DATA: {
		CsContextShPtr cptr;
		RawBuffer data;
		q.Deserialize(cptr, data);

		return m_logic.scanData(*cptr, data);
	}

	case CommandId::SCAN_FILE: {
		CsContextShPtr cptr;
		std::string filepath;
		q.Deserialize(cptr, filepath);

		return m_logic.scanFile(*cptr, filepath);
	}

	case CommandId::GET_SCANNABLE_FILES: {
		std::string dir;
		q.Deserialize(dir);

		return m_logic.getScannableFiles(dir);
	}

	case CommandId::JUDGE_STATUS: {
		std::string filepath;
		int intAction;
		q.Deserialize(filepath, intAction);

		return m_logic.judgeStatus(filepath, static_cast<csr_cs_action_e>(intAction));
	}

	case CommandId::GET_DETECTED: {
		std::string filepath;
		q.Deserialize(filepath);

		return m_logic.getDetected(filepath);
	}

	case CommandId::GET_DETECTED_LIST: {
		StrSet dirSet;
		q.Deserialize(dirSet);

		return m_logic.getDetectedList(dirSet);
	}

	case CommandId::GET_IGNORED: {
		std::string filepath;
		q.Deserialize(filepath);

		return m_logic.getIgnored(filepath);
	}

	case CommandId::GET_IGNORED_LIST: {
		StrSet dirSet;
		q.Deserialize(dirSet);

		return m_logic.getIgnoredList(dirSet);
	}

	// Web protection
	/* TODO: should we separate command->logic mapping of CS and WP ? */
	case CommandId::CHECK_URL: {
		WpContextShPtr cptr;
		std::string url;
		q.Deserialize(cptr, url);

		return m_logic.checkUrl(*cptr, url);
	}

	default:
		ThrowExc(InternalError, "Command isn't in range: " << static_cast<int>(id));
	}
}

void ServerService::onMessageProcess(const ConnShPtr &connection)
{
	DEBUG("let's dispatch it to worker threads.");

	auto process = [&](RawBuffer & buffer) {
		connection->send(this->process(connection, buffer));
	};

	m_workqueue.submit(std::bind(process, connection->receive()));
}

}
