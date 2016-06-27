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

#include "common/exception.h"
#include "common/cs-detected.h"
#include "common/async-protocol.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Client {

AsyncLogic::AsyncLogic(HandleExt *handle, void *userdata) :
	m_handle(handle), m_userdata(userdata)
{
}

AsyncLogic::~AsyncLogic()
{
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
	auto ret = this->m_handle->dispatch<int>(id, *(this->m_handle->getContext()), s);

	if (ret != ASYNC_EVENT_START)
		ThrowExc(ret, "Error on async scan. ret: " << ret);

	bool isDone = false;

	DEBUG("loop for waiting server event start!!");

	while (!isDone) {
		auto event = this->m_handle->revent<int>();

		DEBUG("event received: " << event);

		switch (event) {
		case ASYNC_EVENT_MALWARE_NONE: {
			DEBUG("ASYNC_EVENT_MALWARE_NONE comes in!");
			auto targetName = this->m_handle->revent<std::string>();

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
			auto malware = this->m_handle->revent<CsDetected *>();

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

		case ASYNC_EVENT_COMPLETE: {
			DEBUG("Async operation completed");
			isDone = true;
			break;
		}

		default:
			ThrowExc(event, "Error on async scan! ec: " << event);
		}

		if (this->m_handle->isStopped())
			ThrowExcInfo(ASYNC_EVENT_CANCEL, "Async op cancelled!");
	}
}

} // namespace Client
} // namespace Csr
