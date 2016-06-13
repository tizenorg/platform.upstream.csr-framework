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
 * @file        content-screening.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Content Screening APIs
 */
#include <csr-content-screening.h>

#include <new>
#include <cstring>

#include "client/utils.h"
#include "client/handle-ext.h"
#include "client/async-logic.h"
#include "client/canonicalize.h"
#include "common/macros.h"
#include "common/exception.h"
#include "common/types.h"
#include "common/cs-context.h"
#include "common/cs-detected.h"
#include "common/command-id.h"
#include "common/audit/logger.h"

using namespace Csr;

namespace {

bool _isValid(const csr_cs_core_usage_e &value)
{
	switch (value) {
	case CSR_CS_CORE_USAGE_DEFAULT:
	case CSR_CS_CORE_USAGE_ALL:
	case CSR_CS_CORE_USAGE_HALF:
	case CSR_CS_CORE_USAGE_SINGLE:
		return true;

	default:
		return false;
	}
}

bool _isValid(const csr_cs_ask_user_e &value)
{
	switch (value) {
	case CSR_CS_ASK_USER_NO:
	case CSR_CS_ASK_USER_YES:
		return true;

	default:
		return false;
	}
}

bool _isValid(const csr_cs_action_e &value)
{
	switch (value) {
	case CSR_CS_ACTION_REMOVE:
	case CSR_CS_ACTION_IGNORE:
	case CSR_CS_ACTION_UNIGNORE:
		return true;

	default:
		return false;
	}
}

template <class T>
class CptrList {
public:
	CptrList(std::vector<T *> *_l) : l(_l) {}

	~CptrList()
	{
		for (auto &item : *l)
			delete item;

		delete l;
	}

	T *pop(void)
	{
		if (l->empty())
			return nullptr;

		auto iter = l->begin();
		auto item = *iter;
		l->erase(iter);

		return item;
	}

private:
	std::vector<T *> *l;
};

} // end of namespace

API
int csr_cs_context_create(csr_cs_context_h *handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;

	*handle = reinterpret_cast<csr_cs_context_h>(
				   new Client::HandleExt(SockId::CS, ContextShPtr(new CsContext())));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_context_destroy(csr_cs_context_h handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;

	delete reinterpret_cast<Client::HandleExt *>(handle);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_ask_user(csr_cs_context_h handle, csr_cs_ask_user_e ask_user)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (!_isValid(ask_user))
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Client::HandleExt *>(handle)->getContext()->set(
		static_cast<int>(CsContext::Key::AskUser), static_cast<int>(ask_user));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_popup_message(csr_cs_context_h handle, const char *message)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (message == nullptr || message[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Client::HandleExt *>(handle)->getContext()->set(
		static_cast<int>(CsContext::Key::PopupMessage), message);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_core_usage(csr_cs_context_h handle, csr_cs_core_usage_e usage)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (!_isValid(usage))
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Client::HandleExt *>(handle)->getContext()->set(
		static_cast<int>(CsContext::Key::CoreUsage), static_cast<int>(usage));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_scan_on_cloud(csr_cs_context_h handle, bool scan_on_cloud)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;

	reinterpret_cast<Client::HandleExt *>(handle)->getContext()->set(
		static_cast<int>(CsContext::Key::ScanOnCloud), scan_on_cloud);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_data(csr_cs_context_h handle, const unsigned char *data,
					 size_t length, csr_cs_malware_h *malware)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (malware == nullptr || data == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				   CommandId::SCAN_DATA,
				   hExt->getContext(),
				   RawBuffer(data, data + length));

	if (ret.second)
		hExt->add(ResultPtr(ret.second));

	*malware = reinterpret_cast<csr_cs_malware_h>(ret.second);

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_file(csr_cs_context_h handle, const char *file_path,
					 csr_cs_malware_h *malware)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (malware == nullptr || file_path == nullptr || file_path[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				   CommandId::SCAN_FILE,
				   hExt->getContext(),
				   Client::getAbsolutePath(file_path));

	if (ret.second)
		hExt->add(ResultPtr(ret.second));

	*malware = reinterpret_cast<csr_cs_malware_h>(ret.second);

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_file_scanned_cb(csr_cs_context_h handle, csr_cs_file_scanned_cb callback)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	hExt->m_cb.onScanned = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_detected_cb(csr_cs_context_h handle, csr_cs_detected_cb callback)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	hExt->m_cb.onDetected = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_completed_cb(csr_cs_context_h handle, csr_cs_completed_cb callback)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	hExt->m_cb.onCompleted = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_cancelled_cb(csr_cs_context_h handle, csr_cs_cancelled_cb callback)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	hExt->m_cb.onCancelled = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_error_cb(csr_cs_context_h handle, csr_cs_error_cb callback)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	hExt->m_cb.onError = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_files_async(csr_cs_context_h handle, const char *file_paths[],
							size_t count, void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (file_paths == nullptr || count == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt->isRunning()) {
		ERROR("Async scanning already running with this handle.");
		return CSR_ERROR_BUSY;
	}

	auto fileSet(std::make_shared<StrSet>());

	for (size_t i = 0; i < count; i++) {
		if (file_paths[i] == nullptr)
			return CSR_ERROR_INVALID_PARAMETER;

		fileSet->emplace(Client::getAbsolutePath(file_paths[i]));
	}

	auto task = std::make_shared<Task>([hExt, user_data, fileSet] {
		EXCEPTION_ASYNC_SAFE_START(hExt->m_cb, user_data)

		auto ret = hExt->dispatch<std::pair<int, std::shared_ptr<StrSet>>>(
					CommandId::CANONICALIZE_PATHS, *fileSet);

		if (ret.first != CSR_ERROR_NONE)
			ThrowExc(ret.first, "Error on getting canonicalized paths in subthread. "
					 "ret: " << ret.first);

		std::shared_ptr<StrSet> canonicalizedFiles;

		if (ret.second == nullptr)
			canonicalizedFiles = std::make_shared<StrSet>();
		else
			canonicalizedFiles = std::move(ret.second);

		Client::AsyncLogic l(hExt, user_data, [&hExt] { return hExt->isStopped(); });

		l.scanFiles(*canonicalizedFiles);

		EXCEPTION_SAFE_END
	});

	std::lock_guard<std::mutex> l(hExt->m_dispatchMutex);

	hExt->dispatchAsync(task);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_dir_async(csr_cs_context_h handle, const char *dir_path,
						  void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (dir_path == nullptr || dir_path[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt->isRunning()) {
		ERROR("Async scanning already running with this handle.");
		return CSR_ERROR_BUSY;
	}

	auto dir = std::make_shared<std::string>(Client::getAbsolutePath(dir_path));

	auto task = std::make_shared<Task>([hExt, user_data, dir] {
		EXCEPTION_ASYNC_SAFE_START(hExt->m_cb, user_data)

		Client::AsyncLogic l(hExt, user_data, [&hExt] { return hExt->isStopped(); });

		l.scanDir(*dir);

		EXCEPTION_SAFE_END
	});

	std::lock_guard<std::mutex> l(hExt->m_dispatchMutex);

	hExt->dispatchAsync(task);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_dirs_async(csr_cs_context_h handle, const char *dir_paths[],
						   size_t count, void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (dir_paths == nullptr || count == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt->isRunning()) {
		ERROR("Async scanning already running with this handle.");
		return CSR_ERROR_BUSY;
	}

	auto dirSet(std::make_shared<StrSet>());

	for (size_t i = 0; i < count; i++) {
		if (dir_paths[i] == nullptr)
			return CSR_ERROR_INVALID_PARAMETER;

		dirSet->emplace(Client::getAbsolutePath(dir_paths[i]));
	}

	auto task = std::make_shared<Task>([hExt, user_data, dirSet] {
		EXCEPTION_ASYNC_SAFE_START(hExt->m_cb, user_data)

		auto ret = hExt->dispatch<std::pair<int, std::shared_ptr<StrSet>>>(
					CommandId::CANONICALIZE_PATHS, *dirSet);

		if (ret.first != CSR_ERROR_NONE)
			ThrowExc(ret.first, "Error on getting canonicalized paths in subthread. "
					 "ret: " << ret.first);

		std::shared_ptr<StrSet> canonicalizedDirs;

		if (ret.second == nullptr)
			canonicalizedDirs = std::make_shared<StrSet>();
		else
			canonicalizedDirs = std::move(ret.second);

		Client::eraseSubdirectories(*canonicalizedDirs);

		Client::AsyncLogic l(hExt, user_data, [&hExt] { return hExt->isStopped(); });

		l.scanDirs(*canonicalizedDirs);

		EXCEPTION_SAFE_END
	});

	std::lock_guard<std::mutex> l(hExt->m_dispatchMutex);

	hExt->dispatchAsync(task);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_cancel_scanning(csr_cs_context_h handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (!hExt->isRunning() || hExt->isStopped())
		return CSR_ERROR_NO_TASK;

	hExt->stop();

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_get_severity(csr_cs_malware_h malware, csr_cs_severity_level_e *severity)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (severity == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*severity = reinterpret_cast<CsDetected *>(malware)->severity;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_get_name(csr_cs_malware_h malware, char **name)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (name == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto _name = strdup(reinterpret_cast<CsDetected *>(malware)->malwareName.c_str());

	if (_name == nullptr)
		return CSR_ERROR_OUT_OF_MEMORY;

	*name = _name;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_get_detailed_url(csr_cs_malware_h malware, char **detailed_url)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (detailed_url == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto url = strdup(reinterpret_cast<CsDetected *>(malware)->detailedUrl.c_str());

	if (url == nullptr)
		return CSR_ERROR_OUT_OF_MEMORY;

	*detailed_url = url;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_get_timestamp(csr_cs_malware_h malware, time_t *timestamp)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (timestamp == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*timestamp = reinterpret_cast<CsDetected *>(malware)->ts;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_get_file_name(csr_cs_malware_h malware, char **file_name)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (file_name == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto fname = strdup(reinterpret_cast<CsDetected *>(malware)->targetName.c_str());

	if (fname == nullptr)
		return CSR_ERROR_OUT_OF_MEMORY;

	*file_name = fname;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_get_user_response(csr_cs_malware_h malware, csr_cs_user_response_e *response)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (response == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*response = reinterpret_cast<CsDetected *>(malware)->response;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_is_app(csr_cs_malware_h malware, bool *is_app)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (is_app == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*is_app = reinterpret_cast<CsDetected *>(malware)->isApp;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_get_pkg_id(csr_cs_malware_h malware, char **pkg_id)
{
	EXCEPTION_SAFE_START

	if (malware == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (pkg_id == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto pakage_id = strdup(
			reinterpret_cast<CsDetected *>(malware)->pkgId.c_str());

	if (pakage_id == nullptr)
		return CSR_ERROR_OUT_OF_MEMORY;

	*pkg_id = pakage_id;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_judge_detected_malware(csr_cs_context_h handle,
								  csr_cs_malware_h malware, csr_cs_action_e action)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (malware == nullptr || !_isValid(action))
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	return hExt->dispatch<int>(CommandId::JUDGE_STATUS,
							   reinterpret_cast<CsDetected *>(malware)->targetName,
							   static_cast<int>(action));

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_detected_malware(csr_cs_context_h handle, const char *file_path,
								csr_cs_malware_h *malware)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (file_path == nullptr || malware == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				CommandId::GET_DETECTED, Client::getAbsolutePath(file_path));

	if (ret.second)
		hExt->add(ResultPtr(ret.second));

	*malware = reinterpret_cast<csr_cs_malware_h>(ret.second);

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_detected_malwares(csr_cs_context_h handle,
								 const char *dir_paths[], size_t count,
								 csr_cs_malware_list_h *list, size_t *list_count)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (dir_paths == nullptr || count == 0 || list == nullptr || list_count == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	StrSet dirSet;

	for (size_t i = 0; i < count; i++) {
		if (dir_paths[i] == nullptr || dir_paths[i][0] == '\0')
			return CSR_ERROR_INVALID_PARAMETER;

		dirSet.emplace(Client::getAbsolutePath(dir_paths[i]));
	}

	if (dirSet.size() == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto ret = hExt->dispatch<std::pair<int, std::vector<CsDetected *> *>>(
				   CommandId::GET_DETECTED_LIST, dirSet);

	if (ret.second) {
		CptrList<CsDetected> cptrList(ret.second);

		ResultListPtr resultListPtr(new ResultList);

		while (auto dptr = cptrList.pop())
			resultListPtr->emplace_back(dptr);

		*list = reinterpret_cast<csr_cs_malware_list_h>(resultListPtr.get());
		*list_count = resultListPtr->size();

		hExt->add(std::move(resultListPtr));
	} else {
		*list = nullptr;
		*list_count = 0;
	}

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_ignored_malware(csr_cs_context_h handle, const char *file_path,
							   csr_cs_malware_h *malware)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (file_path == nullptr || malware == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				CommandId::GET_IGNORED, Client::getAbsolutePath(file_path));

	if (ret.second)
		hExt->add(ResultPtr(ret.second));

	*malware = reinterpret_cast<csr_cs_malware_h>(ret.second);

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_ignored_malwares(csr_cs_context_h handle,
								const char *dir_paths[], size_t count,
								csr_cs_malware_list_h *list, size_t *list_count)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (dir_paths == nullptr || count == 0 || list == nullptr || list_count == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	StrSet dirSet;

	for (size_t i = 0; i < count; i++) {
		if (dir_paths[i] == nullptr || dir_paths[i][0] == '\0')
			return CSR_ERROR_INVALID_PARAMETER;

		dirSet.emplace(Client::getAbsolutePath(dir_paths[i]));
	}

	if (dirSet.size() == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto ret = hExt->dispatch<std::pair<int, std::vector<CsDetected *> *>>(
				   CommandId::GET_IGNORED_LIST, dirSet);

	if (ret.second) {
		CptrList<CsDetected> cptrList(ret.second);

		ResultListPtr resultListPtr(new ResultList);

		while (auto dptr = cptrList.pop())
			resultListPtr->emplace_back(dptr);

		*list = reinterpret_cast<csr_cs_malware_list_h>(resultListPtr.get());
		*list_count = resultListPtr->size();

		hExt->add(std::move(resultListPtr));
	} else {
		*list = nullptr;
		*list_count = 0;
	}

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_cs_malware_list_get_malware(csr_cs_malware_list_h list, size_t index,
							  csr_cs_malware_h *malware)
{
	EXCEPTION_SAFE_START

	if (list == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (malware == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto dListPtr = reinterpret_cast<ResultList *>(list);

	if (index >= dListPtr->size())
		return CSR_ERROR_INVALID_PARAMETER;

	*malware = reinterpret_cast<csr_cs_malware_h>(dListPtr->at(index).get());

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}
