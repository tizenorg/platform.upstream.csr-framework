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
#include "common/raw-buffer.h"
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

} // end of namespace

API
int csr_cs_context_create(csr_cs_context_h* phandle)
{
	EXCEPTION_SAFE_START

	if (phandle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*phandle = reinterpret_cast<csr_cs_context_h>(
		new Client::HandleExt(std::shared_ptr<Context>(new CsContext())));

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
int csr_cs_set_popup_message(csr_cs_context_h handle, const char* message)
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
int csr_cs_scan_data(csr_cs_context_h handle, const unsigned char *data, unsigned int length, csr_cs_detected_h *pdetected)
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
		hExt->add(ret.second);
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_file(csr_cs_context_h handle, const char *file_path, csr_cs_detected_h *pdetected)
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

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ret.second);
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_callback_on_file_scanned(csr_cs_context_h handle, csr_cs_on_file_scanned_cb callback)
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
int csr_cs_set_callback_on_detected(csr_cs_context_h handle, csr_cs_on_detected_cb callback)
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
int csr_cs_set_callback_on_completed(csr_cs_context_h handle, csr_cs_on_completed_cb callback)
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
int csr_cs_set_callback_on_cancelled(csr_cs_context_h handle, csr_cs_on_cancelled_cb callback)
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
int csr_cs_set_callback_on_error(csr_cs_context_h handle, csr_cs_on_error_cb callback)
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
int csr_cs_scan_files_async(csr_cs_context_h handle, const char **file_paths, unsigned int count, void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || file_paths == nullptr || count == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	auto fileSet(std::make_shared<StrSet>());
	for (unsigned int i = 0; i < count; i++) {
		if (file_paths[i] == nullptr)
			return CSR_ERROR_INVALID_PARAMETER;

		fileSet->emplace(file_paths[i]);
	}

	hExt->dispatchAsync([hExt, user_data, fileSet] {
		Client::AsyncLogic l(hExt->getContext(), hExt->m_cb, user_data,
			[&hExt] { return hExt->isStopped(); });

		l.scanFiles(fileSet).second();
	});

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_dir_async(csr_cs_context_h handle, const char *dir_path, void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir_path == nullptr || dir_path[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	hExt->dispatchAsync([hExt, user_data, dir_path] {
		Client::AsyncLogic l(hExt->getContext(), hExt->m_cb, user_data,
			[&hExt] { return hExt->isStopped(); });

		l.scanDir(dir_path).second();
	});

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_scan_dirs_async(csr_cs_context_h handle, const char **dir_paths, unsigned int count, void *user_data)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir_paths == nullptr || count == 0)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	auto dirSet(std::make_shared<StrSet>());
	for (unsigned int i = 0; i < count; i++) {
		if (dir_paths[i] == nullptr)
			return CSR_ERROR_INVALID_PARAMETER;

		dirSet->emplace(dir_paths[i]);
	}

	hExt->dispatchAsync([hExt, user_data, dirSet] {
		Client::AsyncLogic l(hExt->getContext(), hExt->m_cb, user_data,
			[&hExt] { return hExt->isStopped(); });

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
int csr_cs_detected_get_severity(csr_cs_detected_h detected, csr_cs_severity_level_e* pseverity)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pseverity == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	int intSeverity;
	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::Severity), intSeverity);
	*pseverity = static_cast<csr_cs_severity_level_e>(intSeverity);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_threat_type(csr_cs_detected_h detected, csr_cs_threat_type_e* pthreat_type)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pthreat_type == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	int intThreat;
	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::Threat), intThreat);
	*pthreat_type = static_cast<csr_cs_threat_type_e>(intThreat);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_malware_name(csr_cs_detected_h detected, const char** pmalware_name)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pmalware_name == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::MalwareName), pmalware_name);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_detailed_url(csr_cs_detected_h detected, const char** pdetailed_url)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pdetailed_url == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::DetailedUrl), pdetailed_url);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_timestamp(csr_cs_detected_h detected, time_t* ptimestamp)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || ptimestamp == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::TimeStamp), *ptimestamp);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_file_name(csr_cs_detected_h detected, const char** pfile_name)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pfile_name == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::TargetName), pfile_name);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_user_response(csr_cs_detected_h detected, csr_cs_user_response_e* presponse)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || presponse == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	int intResponse;
	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::UserResponse), intResponse);
	*presponse = static_cast<csr_cs_user_response_e>(intResponse);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_is_app(csr_cs_detected_h detected, csr_cs_bool_e* pis_app )
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || pis_app == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	int is_app;
	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::IsApp), is_app);
	*pis_app = static_cast<csr_cs_bool_e>(is_app);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_pkg_id(csr_cs_detected_h detected, const char** ppkg_id)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || ppkg_id == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::PkgId), ppkg_id);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_pkg_name(csr_cs_detected_h detected, const char** ppkg_name)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || ppkg_name == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::PkgName), ppkg_name);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_detected_get_pkg_version(csr_cs_detected_h detected, const char** ppkg_version)
{
	EXCEPTION_SAFE_START

	if (detected == nullptr || ppkg_version == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::PkgVersion), ppkg_version);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_judge_detected_malware(csr_cs_context_h handle, csr_cs_detected_h detected, csr_cs_action_e action)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || detected == nullptr ||  !_isValid(action))
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	const char* file_path;
	reinterpret_cast<Result *>(detected)->get(
		static_cast<int>(CsDetected::Key::TargetName), &file_path);
	auto ret = hExt->dispatch<int>(
		CommandId::JUDGE_STATUS,
		hExt->getContext(),
		std::string(file_path),
		static_cast<int>(action));

	if (ret != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret);
		return ret;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_detected_malware(csr_cs_context_h handle, const char *file_path, csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || file_path == nullptr ||  pdetected == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
		CommandId::GET_DETECTED,
		hExt->getContext(),
		std::string(file_path));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ret.second);
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_detected_malwares(csr_cs_context_h handle, const char *dir, csr_cs_detected_list_h *plist, int *pcount)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir == nullptr
		||  plist == nullptr || pcount == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetectedList *>>(
		CommandId::GET_DETECTED_LIST,
		hExt->getContext(),
		std::string(dir));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ret.second);
		*plist = reinterpret_cast<csr_cs_detected_list_h>(ret.second);
		*pcount = ret.second->size();
	} else {
		*plist = nullptr;
		*pcount = 0;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_ignored_malware(csr_cs_context_h handle, const char *file_path, csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || file_path == nullptr ||  pdetected == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetected *>>(
		CommandId::GET_IGNORED,
		hExt->getContext(),
		std::string(file_path));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ret.second);
		*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);
	} else {
		*pdetected = nullptr;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_get_ignored_malwares(csr_cs_context_h handle, const char *dir, csr_cs_detected_list_h *plist, int *pcount)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || dir == nullptr
		||  plist == nullptr || pcount == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, CsDetectedList *>>(
		CommandId::GET_IGNORED_LIST,
		hExt->getContext(),
		std::string(dir));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	if (ret.second == nullptr)
		return CSR_ERROR_UNKNOWN; // deserialization logic error

	if (ret.second->hasValue()) {
		hExt->add(ret.second);
		*plist = reinterpret_cast<csr_cs_detected_list_h>(ret.second);
		*pcount = ret.second->size();
	} else {
		*plist = nullptr;
		*pcount = 0;
	}

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_dlist_get_detected(csr_cs_detected_list_h list, int index, csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (list == nullptr || index < 0 || pdetected == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	CsDetectedList *detectedList = reinterpret_cast<CsDetectedList *>(list);
	*pdetected = reinterpret_cast<csr_cs_detected_h>(detectedList->get(index));
	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}