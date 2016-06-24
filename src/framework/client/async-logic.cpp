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
#include "common/audit/logger.h"

namespace Csr {
namespace Client {

AsyncLogic::AsyncLogic(HandleExt *handle, void *userdata) :
	m_handle(handle),
	m_ctx(new CsContext),
	m_cb(handle->m_cb),
	m_userdata(userdata)
{
	// disable ask user option for async request for now
	copyKvp<int>(CsContext::Key::CoreUsage);
	copyKvp<std::string>(CsContext::Key::PopupMessage);
	copyKvp<bool>(CsContext::Key::ScanOnCloud);
}

AsyncLogic::~AsyncLogic()
{
	for (auto &resultPtr : this->m_results)
		this->m_handle->add(std::move(resultPtr));
}

void AsyncLogic::scanDirs(const StrSet &dirs)
{
	for (const auto &dir : dirs)
		this->scanDir(dir);
}

void AsyncLogic::scanDir(const std::string &dir)
{
	auto startTime = ::time(nullptr);

	if (this->m_handle->isStopped())
		ThrowExcInfo(-999, "Async operation cancelled!");

	// Already scanned files are included in history. it'll be skipped later
	// on server side by every single scan_file request.
	auto retFiles = this->m_handle->dispatch<std::pair<int, std::shared_ptr<StrSet>>>(
						CommandId::GET_SCANNABLE_FILES, dir);

	if (retFiles.first == -999) {
		ThrowExcInfo(-999, "Async op cancelled!");
	} else if (retFiles.first != CSR_ERROR_NONE) {
		ThrowExc(retFiles.first, "Error to get scannalbe files. "
				 "dir: " << dir << " ret: " << retFiles.first);
	}

	if (retFiles.second == nullptr) {
		INFO("No scannable file exist on dir: " << dir);
		return;
	}

#ifdef TIZEN_DEBUG_ENABLE
	DEBUG("scannable file list in dir[" << dir <<
		  "], count[" << retFiles.second->size() << "]:");
	size_t count = 0;
	for (const auto &file : *(retFiles.second))
		DEBUG(std::to_string(++count) << " : " << file);
#endif

	// Let's start scan files!
	this->scanFiles(*(retFiles.second));

	auto ts64 = static_cast<int64_t>(startTime);

	auto ret = this->m_handle->dispatch<int>(CommandId::SET_DIR_TIMESTAMP, dir, ts64);
	if (ret != CSR_ERROR_NONE)
		ERROR("Failed to set dir timestamp after scan dir[" << dir << "] with "
			  "ec[" << ret << "] This is server error and not affects to "
			  "client / scan result when it doesn't comes to delta scanning... "
			  "So just ignore this error on client side.");
}

void AsyncLogic::scanFiles(const StrSet &fileSet)
{
	for (const auto &file : fileSet) {
		if (this->m_handle->isStopped())
			ThrowExcInfo(-999, "Async op cancelled!");

		auto ret = this->m_handle->dispatch<std::pair<int, CsDetected *>>(
					   CommandId::SCAN_FILE, this->m_ctx, file);

		// for auto memory deleting in case of exception
		ResultPtr resultPtr(ret.second);

		// ignore all file-system related error in async operation.
		if (ret.first == CSR_ERROR_FILE_DO_NOT_EXIST ||
			ret.first == CSR_ERROR_FILE_CHANGED ||
			ret.first == CSR_ERROR_FILE_SYSTEM) {
			WARN("File system related error code returned when scan files async."
				 " Ignore all file-system related error in async operation because"
				 " scan file list has been provided by server."
				 " file: " << file << " ret: " << ret.first);
			continue;
		}

		if (ret.first != CSR_ERROR_NONE)
			ThrowExc(ret.first, "Error on async scan. ret: " << ret.first <<
					 " while scan file: " << file);

		if (ret.second) {
			INFO("[Detected] file[" << file << "]");
			this->m_results.emplace_back(std::move(resultPtr));

			if (this->m_cb.onDetected != nullptr)
				this->m_cb.onDetected(reinterpret_cast<csr_cs_malware_h>(ret.second),
									  this->m_userdata);
		} else {
			DEBUG("[Scanned] file[" << file << "]");

			if (this->m_cb.onScanned != nullptr)
				this->m_cb.onScanned(file.c_str(), this->m_userdata);
		}
	}
}

} // namespace Client
} // namespace Csr
