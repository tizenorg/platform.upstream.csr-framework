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
 * @file        async-logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "client/async-logic.h"

#include <cstdint>
#include <utility>
#include <sys/epoll.h>

#include "common/exception.h"
#include "common/cs-detected.h"
#include "common/connection.h"
#include "common/async-protocol.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Client {

AsyncLogic::AsyncLogic(HandleExt *handle, void *userdata) :
	m_handle(handle), m_userdata(userdata), m_dispatcherAsync(new Dispatcher(SockId::CS))
{
}

void AsyncLogic::stop(void)
{
	INFO("async logic stop called! Let's send cancel signal to loop");
	this->m_cancelSignal.send();
}

void AsyncLogic::scanDirs(const StrSet &dirs)
{
	this->scanHelper(CommandId::SCAN_DIRS_ASYNC, dirs);
}

void AsyncLogic::scanFiles(const StrSet &files)
{
	this->scanHelper(CommandId::SCAN_FILES_ASYNC, files);
}

void AsyncLogic::scanHelper(const CommandId &id, const StrSet &s)
{
	this->m_dispatcherAsync->methodPing(id, *(this->m_handle->getContext()), s);

	auto fd = this->m_dispatcherAsync->getFd();
	auto cancelEventFd = this->m_cancelSignal.getFd();

	this->m_loop.addEventSource(cancelEventFd, EPOLLIN,
	[&](uint32_t) {
		this->m_cancelSignal.receive();
		ThrowExcInfo(ASYNC_EVENT_CANCEL, "Async event cancelled on fd: " << fd);
	});

	this->m_loop.addEventSource(fd, EPOLLIN | EPOLLHUP | EPOLLRDHUP,
	[&](uint32_t e) {
		if (e & (EPOLLHUP | EPOLLRDHUP))
			ThrowExc(CSR_ERROR_SOCKET, "csr-server might be crashed. Finish async client loop");

		// read event
		auto event = this->m_dispatcherAsync->receiveEvent<int>();

		DEBUG("event received: " << event);

		switch (event) {
		case ASYNC_EVENT_MALWARE_NONE: {
			DEBUG("ASYNC_EVENT_MALWARE_NONE comes in!");
			auto targetName = this->m_dispatcherAsync->receiveEvent<std::string>();

			if (targetName.empty()) {
				ERROR("scanned event received but target name is empty");
				break;
			}

			if (this->m_handle->m_cb.onScanned != nullptr)
				this->m_handle->m_cb.onScanned(targetName.c_str(), this->m_userdata);

			break;
		}

		case ASYNC_EVENT_MALWARE_DETECTED: {
			DEBUG("ASYNC_EVENT_MALWARE_DETECTED comes in!");
			auto malware = this->m_dispatcherAsync->receiveEvent<CsDetected *>();

			if (malware == nullptr) {
				ERROR("malware detected event received but handle is null");
				break;
			}

			ResultPtr resultPtr(malware);

			if (this->m_handle->m_cb.onDetected != nullptr) {
				this->m_handle->add(std::move(resultPtr));
				this->m_handle->m_cb.onDetected(
					reinterpret_cast<csr_cs_malware_h>(malware), this->m_userdata);
			}

			break;
		}

		default:
			ThrowExcInfo(event, "Async event loop terminated by event: " << event);
		}
	});

	try {
		while (true)
			this->m_loop.dispatch(-1);
	} catch (const Exception &e) {
		switch (e.error()) {
			case ASYNC_EVENT_COMPLETE:
				INFO("Async operation completed");
				break;

			default:
				throw;
		}
	}
}

} // namespace Client
} // namespace Csr
