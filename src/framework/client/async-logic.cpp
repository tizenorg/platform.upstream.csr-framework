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

#include <utility>

#include "common/cs-detected.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Client {

AsyncLogic::AsyncLogic(HandleExt *handle, void *userdata,
					   const std::function<bool()> &isStopped) :
	m_handle(handle),
	m_ctx(new CsContext),
	m_cb(handle->m_cb),
	m_userdata(userdata),
	m_isStopped(isStopped),
	m_dispatcher(new Dispatcher(SockId::CS))
{
	// disable ask user option for async request for now
	copyKvp<int>(CsContext::Key::CoreUsage);
	copyKvp<std::string>(CsContext::Key::PopupMessage);
	copyKvp<bool>(CsContext::Key::ScanOnCloud);
}

AsyncLogic::~AsyncLogic()
{
	for (auto &resultPtr : m_results)
		m_handle->add(std::move(resultPtr));
}

AsyncLogic::Ending AsyncLogic::scanDirs(const StrSet &dirs)
{
	Ending e(Callback::Id::OnCompleted, [this] {
		if (m_cb.onCompleted)
			m_cb.onCompleted(this->m_userdata);
	});

	for (const auto &dir : dirs) {
		e = scanDir(dir);

		if (e.first != Callback::Id::OnCompleted)
			return e;
	}

	return e;
}

AsyncLogic::Ending AsyncLogic::scanDir(const std::string &dir)
{
	auto startTime = ::time(nullptr);

	// Already scanned files are included in history. it'll be skipped later
	// on server side by every single scan_file request.
	auto retFiles = m_dispatcher->methodCall<std::pair<int, std::shared_ptr<StrSet>>>(
						CommandId::GET_SCANNABLE_FILES, dir);

	if (retFiles.first != CSR_ERROR_NONE) {
		ERROR("[Error] ret: " << retFiles.first);
		auto ec = retFiles.first;
		return std::make_pair(Callback::Id::OnError, [this, ec] {
			if (this->m_cb.onError)
				this->m_cb.onError(this->m_userdata, ec);
		});
	}

#ifdef TIZEN_DEBUG_ENABLE
	DEBUG("scannable file list in dir[" << dir <<
		  "], count[" << retFiles.second->size() << "]:");
	size_t count = 0;
	for (const auto &file : *(retFiles.second))
		DEBUG(std::to_string(++count) << " : " << file);
#endif

	// Let's start scan files!
	auto task = scanFiles(*(retFiles.second));

	auto ret = m_dispatcher->methodCall<int>(CommandId::SET_DIR_TIMESTAMP, dir, startTime);
	if (ret != CSR_ERROR_NONE)
		ERROR("Failed to set dir timestamp after scan dir[" << dir << "] with "
			  "ec[" << ret << "] This is server error and not affects to "
			  "client / scan result when it doesn't comes to delta scanning... "
			  "So just ignore this error on client side.");

	return task;
}

AsyncLogic::Ending AsyncLogic::scanFiles(const StrSet &fileSet)
{
	for (const auto &file : fileSet) {
		if (m_isStopped()) {
			INFO("async operation cancelled!");
			return std::make_pair(Callback::Id::OnCancelled, [this] {
				if (this->m_cb.onCancelled)
					this->m_cb.onCancelled(this->m_userdata);
			});
		}

		auto ret = m_dispatcher->methodCall<std::pair<int, CsDetected *>>(
					   CommandId::SCAN_FILE, m_ctx, file);

		// for auto memory deleting in case of exception
		ResultPtr resultPtr(ret.second);

		if (ret.first != CSR_ERROR_NONE) {
			ERROR("[Error] ret: " << ret.first << " while scan file: " << file);
			auto ec = ret.first;
			return std::make_pair(Callback::Id::OnError, [this, ec] {
				if (this->m_cb.onError)
					this->m_cb.onError(this->m_userdata, ec);

				return;
			});
		}

		if (ret.second) {
			INFO("[Detected] file[" << file << "]");
			m_results.emplace_back(std::move(resultPtr));

			if (m_cb.onDetected)
				m_cb.onDetected(m_userdata, reinterpret_cast<csr_cs_detected_h>(ret.second));
		} else {
			DEBUG("[Scanned] file[" << file << "]");

			if (m_cb.onScanned)
				m_cb.onScanned(m_userdata, file.c_str());
		}
	}

	return std::make_pair(Callback::Id::OnCompleted, [this] {
		DEBUG("[Completed]");

		if (this->m_cb.onCompleted)
			this->m_cb.onCompleted(this->m_userdata);
	});
}

} // namespace Client
} // namespace Csr
