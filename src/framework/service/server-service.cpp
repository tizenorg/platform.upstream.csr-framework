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
#include <cstdint>

#include "common/binary-queue.h"
#include "common/command-id.h"
#include "common/audit/logger.h"
#include "common/cs-context.h"
#include "common/wp-context.h"
#include "common/cs-detected.h"
#include "common/wp-result.h"
#include "service/exception.h"
#include "service/access-control.h"
#include "service/core-usage.h"

#include <csr-content-screening-types.h>

namespace Csr {

namespace {

#define CID_TOSTRING(name) case CommandId::name: return #name
std::string cidToString(const CommandId &cid)
{
	switch (cid) {
	CID_TOSTRING(SCAN_DATA);
	CID_TOSTRING(SCAN_FILE);
	CID_TOSTRING(GET_DETECTED);
	CID_TOSTRING(GET_DETECTED_LIST);
	CID_TOSTRING(GET_IGNORED);
	CID_TOSTRING(GET_IGNORED_LIST);
	CID_TOSTRING(GET_SCANNABLE_FILES);
	CID_TOSTRING(SET_DIR_TIMESTAMP);
	CID_TOSTRING(JUDGE_STATUS);

	CID_TOSTRING(CHECK_URL);

	CID_TOSTRING(EM_GET_VENDOR);
	CID_TOSTRING(EM_GET_NAME);
	CID_TOSTRING(EM_GET_DATA_VERSION);
	CID_TOSTRING(EM_GET_UPDATED_TIME);
	CID_TOSTRING(EM_GET_ACTIVATED);
	CID_TOSTRING(EM_GET_STATE);
	CID_TOSTRING(EM_SET_STATE);
	default: return FORMAT("Undefined command id: " << static_cast<int>(cid));
	}
}
#undef CID_TOSTRING

inline CommandId extractCommandId(BinaryQueue &q)
{
	CommandId id;

	q.Deserialize(id);

	return id;
}

}

ServerService::ServerService() :
	Service(),
	m_workqueue(2, 10),
	m_cs(new CsLoader(CS_ENGINE_PATH, SAMPLE_ENGINE_RO_RES_DIR, SAMPLE_ENGINE_RW_WORKING_DIR)),
	m_wp(new WpLoader(WP_ENGINE_PATH, SAMPLE_ENGINE_RO_RES_DIR, SAMPLE_ENGINE_RW_WORKING_DIR)),
	m_db(new Db::Manager(RW_DBSPACE "/.csr.db", RO_DBSPACE)),
	m_cslogic(*m_cs, *m_db),
	m_wplogic(*m_wp, *m_db),
	m_emlogic(*m_cs, *m_wp, *m_db)
{
	this->add(SockId::CS);
	this->add(SockId::WP);
	this->add(SockId::ADMIN);
}

RawBuffer ServerService::processCs(const ConnShPtr &conn, RawBuffer &data)
{
	BinaryQueue q;
	q.push(data);

	auto cid = extractCommandId(q);

	INFO("Content scanning request process. command id: " << cidToString(cid));

	switch (cid) {
	case CommandId::SCAN_DATA: {
		if (!hasPermission(conn))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		CsContextShPtr cptr;
		RawBuffer data;
		q.Deserialize(cptr, data);

		return m_cslogic.scanData(*cptr, data);
	}

	case CommandId::SCAN_FILE: {
		if (!hasPermission(conn))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		CsContextShPtr cptr;
		std::string filepath;
		q.Deserialize(cptr, filepath);

		return m_cslogic.scanFile(*cptr, filepath);
	}

	case CommandId::GET_SCANNABLE_FILES: {
		if (!hasPermission(conn))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		std::string dir;
		q.Deserialize(dir);

		return m_cslogic.getScannableFiles(dir);
	}

	case CommandId::SET_DIR_TIMESTAMP: {
		std::string dir;
		int64_t ts64 = 0;
		q.Deserialize(dir, ts64);

		return m_cslogic.setDirTimestamp(dir, static_cast<time_t>(ts64));
	}

	case CommandId::JUDGE_STATUS: {
		// judge status needs admin privilege
		if (!hasPermission(conn, SockId::ADMIN))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		std::string filepath;
		int intAction;
		q.Deserialize(filepath, intAction);

		return m_cslogic.judgeStatus(filepath, static_cast<csr_cs_action_e>(intAction));
	}

	case CommandId::GET_DETECTED: {
		if (!hasPermission(conn))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		std::string filepath;
		q.Deserialize(filepath);

		return m_cslogic.getDetected(filepath);
	}

	case CommandId::GET_DETECTED_LIST: {
		if (!hasPermission(conn))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		StrSet dirSet;
		q.Deserialize(dirSet);

		return m_cslogic.getDetectedList(dirSet);
	}

	case CommandId::GET_IGNORED: {
		if (!hasPermission(conn))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		std::string filepath;
		q.Deserialize(filepath);

		return m_cslogic.getIgnored(filepath);
	}

	case CommandId::GET_IGNORED_LIST: {
		if (!hasPermission(conn))
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		StrSet dirSet;
		q.Deserialize(dirSet);

		return m_cslogic.getIgnoredList(dirSet);
	}

	default:
		ThrowExc(InternalError, "CS Command isn't in range");
	}
}

RawBuffer ServerService::processWp(const ConnShPtr &conn, RawBuffer &data)
{
	if (!hasPermission(conn))
		return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

	BinaryQueue q;
	q.push(data);

	auto cid = extractCommandId(q);

	INFO("Web protection request process. command id: " << cidToString(cid));

	switch (cid) {
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

RawBuffer ServerService::processAdmin(const ConnShPtr &conn, RawBuffer &data)
{
	BinaryQueue q;
	q.push(data);

	bool hasPerm = hasPermission(conn);

	auto cid = extractCommandId(q);

	INFO("Admin request process. command id: " << cidToString(cid));

	switch (cid) {
	case CommandId::EM_GET_NAME: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineName(*cptr);
	}

	case CommandId::EM_GET_VENDOR: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineVendor(*cptr);
	}

	case CommandId::EM_GET_VERSION: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineVersion(*cptr);
	}

	case CommandId::EM_GET_DATA_VERSION: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineDataVersion(*cptr);
	}

	case CommandId::EM_GET_UPDATED_TIME: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineUpdatedTime(*cptr);
	}

	case CommandId::EM_GET_ACTIVATED: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineActivated(*cptr);
	}

	case CommandId::EM_GET_STATE: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

		EmContextShPtr cptr;
		q.Deserialize(cptr);

		return m_emlogic.getEngineState(*cptr);
	}

	case CommandId::EM_SET_STATE: {
		if (!hasPerm)
			return BinaryQueue::Serialize(CSR_ERROR_PERMISSION_DENIED).pop();

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

	auto inbufPtr = std::make_shared<RawBuffer>(connection->receive());

	m_workqueue.submit([this, &connection, process, inbufPtr]() {
		auto outbuf = (*process)(connection, *inbufPtr);

		connection->send(outbuf);

		CpuUsageManager::reset();
	});
}

}
