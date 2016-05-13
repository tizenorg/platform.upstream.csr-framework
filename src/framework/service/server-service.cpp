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

namespace {

inline CommandId extractCommandId(BinaryQueue &q)
{
	CommandId id;

	q.Deserialize(id);

	return id;
}

}

ServerService::ServerService() : Service(), m_workqueue(2, 10)
{
	this->add(SockId::CS);
	this->add(SockId::WP);
	this->add(SockId::ADMIN);
}

ServerService::~ServerService()
{
}

RawBuffer ServerService::processCs(const ConnShPtr &, RawBuffer &data)
{
	BinaryQueue q;
	q.push(data);

	switch (extractCommandId(q)) {
	case CommandId::SCAN_DATA: {
		CsContextShPtr cptr;
		RawBuffer data;
		q.Deserialize(cptr, data);

		return m_cslogic.scanData(*cptr, data);
	}

	case CommandId::SCAN_FILE: {
		CsContextShPtr cptr;
		std::string filepath;
		q.Deserialize(cptr, filepath);

		return m_cslogic.scanFile(*cptr, filepath);
	}

	case CommandId::GET_SCANNABLE_FILES: {
		std::string dir;
		q.Deserialize(dir);

		return m_cslogic.getScannableFiles(dir);
	}

	case CommandId::JUDGE_STATUS: {
		std::string filepath;
		int intAction;
		q.Deserialize(filepath, intAction);

		return m_cslogic.judgeStatus(filepath, static_cast<csr_cs_action_e>(intAction));
	}

	case CommandId::GET_DETECTED: {
		std::string filepath;
		q.Deserialize(filepath);

		return m_cslogic.getDetected(filepath);
	}

	case CommandId::GET_DETECTED_LIST: {
		StrSet dirSet;
		q.Deserialize(dirSet);

		return m_cslogic.getDetectedList(dirSet);
	}

	case CommandId::GET_IGNORED: {
		std::string filepath;
		q.Deserialize(filepath);

		return m_cslogic.getIgnored(filepath);
	}

	case CommandId::GET_IGNORED_LIST: {
		StrSet dirSet;
		q.Deserialize(dirSet);

		return m_cslogic.getIgnoredList(dirSet);
	}

	default:
		ThrowExc(InternalError, "CS Command isn't in range");
	}
}

RawBuffer ServerService::processWp(const ConnShPtr &, RawBuffer &data)
{
	BinaryQueue q;
	q.push(data);

	switch (extractCommandId(q)) {
	case CommandId::CHECK_URL: {
		WpContextShPtr cptr;
		std::string url;
		q.Deserialize(cptr, url);

		return m_wplogic.checkUrl(*cptr, url);
	}

	default:
		ThrowExc(InternalError, "WP Command isn't in range");
	}
}

RawBuffer ServerService::processAdmin(const ConnShPtr &, RawBuffer &data)
{
	BinaryQueue q;
	q.push(data);

	switch (extractCommandId(q)) {
	case CommandId::EM_GET_NAME: {
		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineName(*cptr);
	}

	case CommandId::EM_GET_VENDOR: {
		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineVendor(*cptr);
	}

	case CommandId::EM_GET_VERSION: {
		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineVersion(*cptr);
	}

	case CommandId::EM_GET_DATA_VERSION: {
		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineDataVersion(*cptr);
	}

	case CommandId::EM_GET_UPDATED_TIME: {
		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineUpdatedTime(*cptr);
	}

	case CommandId::EM_GET_ACTIVATED: {
		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineActivated(*cptr);
	}

	case CommandId::EM_GET_STATE: {
		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineState(*cptr);
	}

	case CommandId::EM_SET_STATE: {
		EmContextShPtr cptr;
		int intState;
		q.Deserialize(cptr, intState);

		return m_emlogic.setEngineState(*cptr, static_cast<csr_state_e>(intState));
	}

	default:
		ThrowExc(InternalError, "ADMIN Command isn't in range");
	}
}

void ServerService::onMessageProcess(const ConnShPtr &connection)
{
	DEBUG("let's dispatch it to worker threads.");

	using LogicMapper = std::function<RawBuffer(const ConnShPtr &, RawBuffer &)>;

	auto process(std::make_shared<LogicMapper>());

	switch (connection->getSockId()) {
	case SockId::CS:
		*process = [this](const ConnShPtr &c, RawBuffer &b) {
			return this->processCs(c, b);
		};
		break;

	case SockId::WP:
		*process = [this](const ConnShPtr &c, RawBuffer &b) {
			return this->processWp(c, b);
		};
		break;

	case SockId::ADMIN:
		*process = [this](const ConnShPtr &c, RawBuffer &b) {
			return this->processAdmin(c, b);
		};
		break;

	default:
		ThrowExc(InternalError, "Message from unknown sock id");
	}

	m_workqueue.submit(std::bind([this, &connection, process](RawBuffer &buffer) {
		connection->send((*process)(connection, buffer));
	}, connection->receive()));
}

}
