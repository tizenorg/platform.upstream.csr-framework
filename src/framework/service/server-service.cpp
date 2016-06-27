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
#include "common/exception.h"
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
	CID_TOSTRING(SCAN_DIRS_ASYNC);
	CID_TOSTRING(SCAN_FILES_ASYNC);
	CID_TOSTRING(CANCEL_OPERATION);
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

} // namespace anonymous

ServerService::ServerService() : Service(), m_workqueue(5)
{
	this->m_db = std::make_shared<Db::Manager>(RW_DBSPACE "/.csr.db", RO_DBSPACE);

	try {
		this->m_cs = std::make_shared<CsLoader>(CS_ENGINE_PATH, ENGINE_DIR,
												ENGINE_RW_WORKING_DIR);
	} catch (const Exception &e) {
		WARN("Excetpion in content screening loader: " << e.what() <<
			 " error: " << e.error() << " treat it as ENGINE_NOT_EXIST.");
	}

	try {
		this->m_wp = std::make_shared<WpLoader>(WP_ENGINE_PATH, ENGINE_DIR,
												ENGINE_RW_WORKING_DIR);
	} catch (const Exception &e) {
		WARN("Exception in web protection loader: " << e.what() <<
			 " error: " << e.error() << " treat it as ENGINE_NOT_EXIST.");
	}

	this->m_cslogic.reset(new CsLogic(this->m_cs, this->m_db));
	this->m_wplogic.reset(new WpLogic(this->m_wp, this->m_db));
	this->m_emlogic.reset(new EmLogic(this->m_cs, this->m_wp, this->m_db));

	this->add(SockId::CS);
	this->add(SockId::WP);
	this->add(SockId::ADMIN);

	// if task is not running in workqueue, it's idle.
	this->m_loop.setIdleChecker([this]()->bool {
		return (!this->m_workqueue.isTaskRunning() && this->m_loop.countEventSource() == 3);
	});
}

RawBuffer ServerService::processCs(const ConnShPtr &conn, RawBuffer &data)
{
	EXCEPTION_GUARD_START

	// need not to try resetting engine because the engine is pre-loaded (RO) and it
	// cannot be fixed in dynamically except platform-development time.
	if (!this->m_cs)
		ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "Content screening engine is not exist!");

	BinaryQueue q;
	q.push(data);

	auto cid = extractCommandId(q);

	INFO("Content scanning request process. command id: " << cidToString(cid));

	switch (cid) {
	case CommandId::SCAN_DATA: {
		hasPermission(conn);

		CsContextShPtr cptr;
		RawBuffer data;
		q.Deserialize(cptr, data);

		return this->m_cslogic->scanData(*cptr, data);
	}

	case CommandId::SCAN_FILE: {
		hasPermission(conn);

		CsContextShPtr cptr;
		std::string filepath;
		q.Deserialize(cptr, filepath);

		return this->m_cslogic->scanFile(*cptr, filepath);
	}

	case CommandId::SCAN_FILES_ASYNC: {
		hasPermission(conn);

		CsContextShPtr cptr;
		StrSet paths;
		q.Deserialize(cptr, paths);

		auto fd = conn->getFd();

		{
			std::lock_guard<std::mutex> l(this->m_cancelledMutex);
			this->m_isCancelled[fd] = false;
			INFO("Turn off cancelled flag before start async. fd: " << fd);
		}

		Closer closer([this, fd]() {
			std::lock_guard<std::mutex> l(this->m_cancelledMutex);
			this->m_isCancelled.erase(fd);
			INFO("Erase cancelled flag in closer on fd: " << fd);
		});

		return this->m_cslogic->scanFilesAsync(conn, *cptr, paths, [this, fd]() {
			std::lock_guard<std::mutex> l(this->m_cancelledMutex);
			if (this->m_isCancelled.count(fd) == 1 && this->m_isCancelled[fd])
				ThrowExcInfo(-999, "operation cancelled on fd: " << fd);
		});
	}

	case CommandId::SCAN_DIRS_ASYNC: {
		hasPermission(conn);

		CsContextShPtr cptr;
		StrSet paths;
		q.Deserialize(cptr, paths);

		auto fd = conn->getFd();

		{
			std::lock_guard<std::mutex> l(this->m_cancelledMutex);
			this->m_isCancelled[fd] = false;
			INFO("Turn off cancelled flag before start async. fd: " << fd);
		}

		Closer closer([this, fd]() {
			std::lock_guard<std::mutex> l(this->m_cancelledMutex);
			this->m_isCancelled.erase(fd);
			INFO("Erase cancelled flag in closer on fd: " << fd);
		});

		return this->m_cslogic->scanDirsAsync(conn, *cptr, paths, [this, fd]() {
			std::lock_guard<std::mutex> l(this->m_cancelledMutex);
			if (this->m_isCancelled.count(fd) == 1 && this->m_isCancelled[fd])
				ThrowExcInfo(-999, "operation cancelled on fd: " << fd);
		});
	}

	case CommandId::CANCEL_OPERATION: {
		std::lock_guard<std::mutex> l(this->m_cancelledMutex);
		auto fd = conn->getFd();
		if (this->m_isCancelled.count(fd) == 1) {
			this->m_isCancelled[fd] = true;
			INFO("Turn on cancelled flag of fd: " << fd);
		} else {
			WARN("Nothing to cancel... fd: " << fd);
		}

		return RawBuffer();
	}

	case CommandId::JUDGE_STATUS: {
		// judge status needs admin privilege
		hasPermission(conn, SockId::ADMIN);

		std::string filepath;
		int intAction;
		q.Deserialize(filepath, intAction);

		return this->m_cslogic->judgeStatus(filepath,
											static_cast<csr_cs_action_e>(intAction));
	}

	case CommandId::GET_DETECTED: {
		hasPermission(conn);

		std::string filepath;
		q.Deserialize(filepath);

		return this->m_cslogic->getDetected(filepath);
	}

	case CommandId::GET_DETECTED_LIST: {
		hasPermission(conn);

		StrSet dirSet;
		q.Deserialize(dirSet);

		return this->m_cslogic->getDetectedList(dirSet);
	}

	case CommandId::GET_IGNORED: {
		hasPermission(conn);

		std::string filepath;
		q.Deserialize(filepath);

		return this->m_cslogic->getIgnored(filepath);
	}

	case CommandId::GET_IGNORED_LIST: {
		hasPermission(conn);

		StrSet dirSet;
		q.Deserialize(dirSet);

		return this->m_cslogic->getIgnoredList(dirSet);
	}

	default:
		ThrowExc(CSR_ERROR_SERVER, "CS Command isn't in range");
	}

	EXCEPTION_GUARD_END
}

RawBuffer ServerService::processWp(const ConnShPtr &conn, RawBuffer &data)
{
	EXCEPTION_GUARD_START

	if (!this->m_wp)
		ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "Web protection engine is not exist!");

	hasPermission(conn);

	BinaryQueue q;
	q.push(data);

	auto cid = extractCommandId(q);

	INFO("Web protection request process. command id: " << cidToString(cid));

	switch (cid) {
	case CommandId::CHECK_URL: {
		WpContextShPtr cptr;
		std::string url;
		q.Deserialize(cptr, url);

		return this->m_wplogic->checkUrl(*cptr, url);
	}

	default:
		ThrowExc(CSR_ERROR_SERVER, "WP Command isn't in range");
	}

	EXCEPTION_GUARD_END
}

RawBuffer ServerService::processAdmin(const ConnShPtr &conn, RawBuffer &data)
{
	EXCEPTION_GUARD_START

	BinaryQueue q;
	q.push(data);

	auto cid = extractCommandId(q);

	EmContextShPtr cptr;
	q.Deserialize(cptr);

	if (!cptr)
		ThrowExc(CSR_ERROR_SOCKET, "engine context for processAdmin should be exist!");

	switch (cptr->engineId) {
	case CSR_ENGINE_CS:
		if (!this->m_cs)
			ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "Content screening engine is not exist!");
		break;
	case CSR_ENGINE_WP:
		if (!this->m_wp)
			ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "Web protection engine is not exist!");
		break;
	default:
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "context engine Id is invalid for "
				 "processAdmin!");
	}

	hasPermission(conn);

	INFO("Admin request process. command id: " << cidToString(cid));

	switch (cid) {
	case CommandId::EM_GET_NAME: {
		return this->m_emlogic->getEngineName(*cptr);
	}

	case CommandId::EM_GET_VENDOR: {
		return this->m_emlogic->getEngineVendor(*cptr);
	}

	case CommandId::EM_GET_VERSION: {
		return this->m_emlogic->getEngineVersion(*cptr);
	}

	case CommandId::EM_GET_DATA_VERSION: {
		return this->m_emlogic->getEngineDataVersion(*cptr);
	}

	case CommandId::EM_GET_UPDATED_TIME: {
		return this->m_emlogic->getEngineUpdatedTime(*cptr);
	}

	case CommandId::EM_GET_ACTIVATED: {
		return this->m_emlogic->getEngineActivated(*cptr);
	}

	case CommandId::EM_GET_STATE: {
		return this->m_emlogic->getEngineState(*cptr);
	}

	case CommandId::EM_SET_STATE: {
		int intState;
		q.Deserialize(intState);

		return this->m_emlogic->setEngineState(*cptr, static_cast<csr_state_e>(intState));
	}

	default:
		ThrowExc(CSR_ERROR_SERVER, "ADMIN Command isn't in range");
	}

	EXCEPTION_GUARD_END
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
		ThrowExc(CSR_ERROR_SERVER, "Message from unknown sock id");
	}

	auto inbufPtr = std::make_shared<RawBuffer>(connection->receive());

	this->m_workqueue.submit([this, connection, process, inbufPtr]() {
		try {
			auto outbuf = (*process)(connection, *inbufPtr);

			CpuUsageManager::reset();

			if (!outbuf.empty())
				connection->send(outbuf);
		} catch (const std::exception &e) {
			ERROR("exception on workqueue task: " << e.what());
			try {
				connection->send(BinaryQueue::Serialize(CSR_ERROR_SYSTEM).pop());
			} catch (const std::exception &e) {
				ERROR("The connection is abnormally closed by the peer: " << e.what());
			}
		}
	});
}

}
