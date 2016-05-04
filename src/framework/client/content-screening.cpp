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
#include "csr/content-screening.h"

#include <new>

#include "client/utils.h"
#include "client/handle-ext.h"
#include "client/async-logic.h"
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
	case CSR_CS_USE_CORE_DEFAULT:
	case CSR_CS_USE_CORE_ALL:
	case CSR_CS_USE_CORE_HALF:
	case CSR_CS_USE_CORE_SINGLE:
		return true;

	default:
		return false;
	}
}

bool _isValid(const csr_cs_ask_user_e &value)
{
	switch (value) {
	case CSR_CS_NOT_ASK_USER:
	case CSR_CS_ASK_USER:
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
	CptrList(std::vector<T *> &_l) : l(_l) {}

	~CptrList()
	{
		for (auto &item : l)
			delete item;
	}

	T *pop(void)
	{
		if (l.empty())
			return nullptr;

		auto iter = l.begin();
		auto item = *iter;
		l.erase(iter);

		return item;
	}

private:
	std::vector<T *> &l;
};

} // end of namespace

API
int csr_cs_context_create(csr_cs_context_h *phandle)
{
	EXCEPTION_SAFE_START

	if (phandle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*phandle = reinterpret_cast<csr_cs_context_h>(
				   new Client::HandleExt(ContextShPtr(new CsContext())));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_context_destroy(csr_cs_context_h handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	delete reinterpret_cast<Client::HandleExt *>(handle);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_ask_user(csr_cs_context_h handle, csr_cs_ask_user_e ask_user)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || !_isValid(ask_user))
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

	if (handle == nullptr || message == nullptr || message[0] == '\0')
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

	if (handle == nullptr || !_isValid(usage))
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Client::HandleExt *>(handle)->getContext()->set(
		static_cast<int>(CsContext::Key::CoreUsage), static_cast<int>(usage));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

// TODO: API which unset this option isn't needed?
API
int csr_cs_set_scan_on_cloud(csr_cs_context_h handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Client::HandleExt *>(handle)->getContext()->set(
		static_cast<int>(CsContext::Key::ScanOnCloud), true);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_data(csr_cs_context_h handle, const unsigned char *data,
					 size_t length, csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || pdetected == nullptr || data == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				   CommandId::SCAN_DATA,
				   hExt->getContext(),
				   RawBuffer(data, data + length));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ResultPtr(ret.second));
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
		delete ret.second;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_file(csr_cs_context_h handle, const char *file_path,
					 csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || pdetected == nullptr
			|| file_path == nullptr || file_path[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				   CommandId::SCAN_FILE,
				   hExt->getContext(),
				   std::string(file_path));

	if (ret.first != CSR_ERROR_NONE && ret.first != CSR_ERROR_REMOVE_FAILED) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ResultPtr(ret.second));
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
		delete ret.second;
	}

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_callback_on_file_scanned(csr_cs_context_h handle,
										csr_cs_on_file_scanned_cb callback)
{
	EXCEPTION_SAFE_START

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt == nullptr || callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	hExt->m_cb.onScanned = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_callback_on_detected(csr_cs_context_h handle,
									csr_cs_on_detected_cb callback)
{
	EXCEPTION_SAFE_START

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt == nullptr || callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	hExt->m_cb.onDetected = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_callback_on_completed(csr_cs_context_h handle,
									 csr_cs_on_completed_cb callback)
{
	EXCEPTION_SAFE_START

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt == nullptr || callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	hExt->m_cb.onCompleted = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_callback_on_cancelled(csr_cs_context_h handle,
									 csr_cs_on_cancelled_cb callback)
{
	EXCEPTION_SAFE_START

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt == nullptr || callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	hExt->m_cb.onCancelled = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_callback_on_error(csr_cs_context_h handle,
								 csr_cs_on_error_cb callback)
{
	EXCEPTION_SAFE_START

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt == nullptr || callback == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	hExt->m_cb.onError = callback;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_files_async(csr_cs_context_h handle, const char *file_paths[],
							size_t count, void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || file_paths == nullptr || count == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	auto fileSet(std::make_shared<StrSet>());

	for (size_t i = 0; i < count; i++) {
		if (file_paths[i] == nullptr)
			return CSR_ERROR_INVALID_PARAMETER;

		fileSet->emplace(file_paths[i]);
	}

	hExt->dispatchAsync([hExt, user_data, fileSet] {
		Client::AsyncLogic l(hExt, user_data, [&hExt] { return hExt->isStopped(); });

		l.scanFiles(fileSet).second();
	});

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_dir_async(csr_cs_context_h handle, const char *dir_path,
						  void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir_path == nullptr || dir_path[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	hExt->dispatchAsync([hExt, user_data, dir_path] {
		Client::AsyncLogic l(hExt, user_data, [&hExt] { return hExt->isStopped(); });

		l.scanDir(dir_path).second();
	});

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_dirs_async(csr_cs_context_h handle, const char *dir_paths[],
						   size_t count, void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir_paths == nullptr || count == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	auto dirSet(std::make_shared<StrSet>());

	for (size_t i = 0; i < count; i++) {
		if (dir_paths[i] == nullptr)
			return CSR_ERROR_INVALID_PARAMETER;

		dirSet->emplace(dir_paths[i]);
	}

	hExt->dispatchAsync([hExt, user_data, dirSet] {
		Client::AsyncLogic l(hExt, user_data, [&hExt] { return hExt->isStopped(); });

		l.scanDirs(dirSet).second();
	});

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_cancel(csr_cs_context_h handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt->isStopped())
		return CSR_ERROR_NONE;

	hExt->stop();

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_severity(csr_cs_detected_h detected,
								 csr_cs_severity_level_e *pseverity)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pseverity == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*pseverity = reinterpret_cast<CsDetected *>(detected)->severity;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_threat_type(csr_cs_detected_h detected,
									csr_cs_threat_type_e *pthreat_type)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pthreat_type == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*pthreat_type = reinterpret_cast<CsDetected *>(detected)->threat;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_malware_name(csr_cs_detected_h detected,
									 const char **pmalware_name)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pmalware_name == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*pmalware_name = reinterpret_cast<CsDetected *>(detected)->malwareName.c_str();

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_detailed_url(csr_cs_detected_h detected,
									 const char **pdetailed_url)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pdetailed_url == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*pdetailed_url = reinterpret_cast<CsDetected *>(detected)->detailedUrl.c_str();

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_timestamp(csr_cs_detected_h detected,
								  time_t *ptimestamp)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || ptimestamp == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*ptimestamp = reinterpret_cast<CsDetected *>(detected)->ts;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_file_name(csr_cs_detected_h detected,
								  const char **pfile_name)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pfile_name == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*pfile_name = reinterpret_cast<CsDetected *>(detected)->targetName.c_str();

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_user_response(csr_cs_detected_h detected,
									  csr_cs_user_response_e *presponse)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || presponse == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*presponse = reinterpret_cast<CsDetected *>(detected)->response;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_is_app(csr_cs_detected_h detected, bool *pis_app)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pis_app == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*pis_app = reinterpret_cast<CsDetected *>(detected)->isApp;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_pkg_id(csr_cs_detected_h detected, const char **ppkg_id)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || ppkg_id == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*ppkg_id = reinterpret_cast<CsDetected *>(detected)->pkgId.c_str();

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_judge_detected_malware(csr_cs_context_h handle,
								  csr_cs_detected_h detected, csr_cs_action_e action)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || detected == nullptr || !_isValid(action))
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<int>(
				   CommandId::JUDGE_STATUS,
				   reinterpret_cast<CsDetected *>(detected)->targetName,
				   static_cast<int>(action));

	if (ret != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret);
		return ret;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_detected_malware(csr_cs_context_h handle, const char *file_path,
								csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || file_path == nullptr || pdetected == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				   CommandId::GET_DETECTED, std::string(file_path));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ResultPtr(ret.second));
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
		delete ret.second;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_detected_malwares(csr_cs_context_h handle,
								 const char *dir_paths[], size_t count,
								 csr_cs_detected_list_h *plist, size_t *pcount)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir_paths == nullptr || count == 0
			|| plist == nullptr || pcount == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	StrSet dirSet;

	for (size_t i = 0; i < count; i++) {
		if (dir_paths[i] == nullptr || dir_paths[i][0] == '\0')
			return CSR_ERROR_INVALID_PARAMETER;

		dirSet.emplace(dir_paths[i]);
	}

	if (dirSet.size() == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto ret = hExt->dispatch<std::pair<int, std::vector<CsDetected *>>>(
				   CommandId::GET_DETECTED_LIST, dirSet);

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second.empty()) {
		*plist = nullptr;
		*pcount = 0;
		return CSR_ERROR_NONE;
	}

	CptrList<CsDetected> cptrList(ret.second);

	ResultListPtr resultListPtr(new ResultList);

	while (auto dptr = cptrList.pop())
		resultListPtr->emplace_back(dptr);

	*plist = reinterpret_cast<csr_cs_detected_list_h>(resultListPtr.get());
	*pcount = resultListPtr->size();

	hExt->add(std::move(resultListPtr));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_ignored_malware(csr_cs_context_h handle, const char *file_path,
							   csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || file_path == nullptr || pdetected == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
				   CommandId::GET_IGNORED, std::string(file_path));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ResultPtr(ret.second));
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
		delete ret.second;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_ignored_malwares(csr_cs_context_h handle,
								const char *dir_paths[], size_t count,
								csr_cs_detected_list_h *plist, size_t *pcount)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir_paths == nullptr || count == 0
			|| plist == nullptr || pcount == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	StrSet dirSet;

	for (size_t i = 0; i < count; i++) {
		if (dir_paths[i] == nullptr || dir_paths[i][0] == '\0')
			return CSR_ERROR_INVALID_PARAMETER;

		dirSet.emplace(dir_paths[i]);
	}

	if (dirSet.size() == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto ret = hExt->dispatch<std::pair<int, std::vector<CsDetected *>>>(
				   CommandId::GET_IGNORED_LIST, dirSet);

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second.empty()) {
		*plist = nullptr;
		*pcount = 0;
		return CSR_ERROR_NONE;
	}

	CptrList<CsDetected> cptrList(ret.second);

	ResultListPtr resultListPtr(new ResultList);

	while (auto dptr = cptrList.pop())
		resultListPtr->emplace_back(dptr);

	*plist = reinterpret_cast<csr_cs_detected_list_h>(resultListPtr.get());
	*pcount = resultListPtr->size();

	hExt->add(std::move(resultListPtr));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_dlist_get_detected(csr_cs_detected_list_h list, size_t index,
							  csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (list == nullptr || pdetected == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto dListPtr = reinterpret_cast<ResultList *>(list);

	if (index >= dListPtr->size())
		return CSR_ERROR_INVALID_PARAMETER;

	*pdetected = reinterpret_cast<csr_cs_detected_h>(dListPtr->at(index).get());

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}
