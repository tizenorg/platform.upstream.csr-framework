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
#include "common/command-id.h"
#include "common/audit/logger.h"

using namespace Csr;

API
int csr_cs_context_create(csr_cs_context_h* phandle)
{
	EXCEPTION_SAFE_START

	if (phandle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*phandle = reinterpret_cast<csr_cs_context_h>(new Client::HandleExt());

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
	(void) handle;
	(void) ask_user;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_set_popup_message(csr_cs_context_h handle, const char* message)
{
	(void) handle;
	(void) message;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_set_core_usage(csr_cs_context_h handle, csr_cs_core_usage_e usage)
{
	(void) handle;
	(void) usage;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_set_scan_on_cloud(csr_cs_context_h handle)
{
	(void) handle;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_scan_data(csr_cs_context_h handle, const unsigned char *data, unsigned int length, csr_cs_detected_h *pdetected)
{
	(void) handle;
	(void) data;
	(void) length;
	(void) pdetected;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_scan_file(csr_cs_context_h handle, const char *file_path, csr_cs_detected_h *pdetected)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr || pdetected == nullptr
		|| file_path == nullptr || file_path[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);
	auto ret = hExt->dispatch<std::pair<int, Result *>>(
		CommandId::SCAN_FILE,
		hExt->getContext(),
		std::string(file_path));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	hExt->add(ret.second);
	*pdetected = reinterpret_cast<csr_cs_detected_h>(ret.second);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_cs_set_callback_on_file_scanned(csr_cs_context_h handle, csr_cs_on_file_scanned_cb callback)
{
	EXCEPTION_SAFE_START

	auto hExt = reinterpret_cast<Client::HandleExt *>(handle);

	if (hExt == nullptr)
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

	if (hExt == nullptr)
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

	if (hExt == nullptr)
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

	if (hExt == nullptr)
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

	if (hExt == nullptr)
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
	(void) detected;
	(void) pseverity;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_detected_get_threat_type(csr_cs_detected_h detected, csr_cs_threat_type_e* pthreat_type)
{
	(void) detected;
	(void) pthreat_type;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_detected_get_malware_name(csr_cs_detected_h detected, char** malware_name)
{
	(void) detected;
	(void) malware_name;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_detected_get_detailed_url(csr_cs_detected_h detected, char** detailed_url)
{
	(void) detected;
	(void) detailed_url;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_detected_get_timestamp(csr_cs_detected_h detected, time_t* timestamp)
{
	(void) detected;
	(void) timestamp;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_detected_get_file_name(csr_cs_detected_h detected, char** file_name)
{
	(void) detected;
	(void) file_name;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_detected_get_user_response(csr_cs_detected_h detected, csr_cs_user_response_e* presponse)
{
	(void) detected;
	(void) detected;
	(void) presponse;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_judge_detected_malware(csr_cs_context_h handle, const char *file_path, csr_cs_action_e action)
{
	(void) handle;
	(void) file_path;
	(void) action;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_get_detected_malware(csr_cs_context_h handle, const char *file_path, csr_cs_detected_h *pdetected)
{
	(void) handle;
	(void) file_path;
	(void) pdetected;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_get_detected_malwares(csr_cs_context_h handle, const char *dir, csr_cs_detected_list_h *plist, int *pcount)
{
	(void) handle;
	(void) dir;
	(void) plist;
	(void) pcount;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_get_ignored_malware(csr_cs_context_h handle, const char *file_path, csr_cs_detected_h *pdetected)
{
	(void) handle;
	(void) file_path;
	(void) pdetected;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_get_ignored_malwares(csr_cs_context_h handle, const char *dir, csr_cs_detected_list_h *plist, int *pcount)
{
	(void) handle;
	(void) dir;
	(void) plist;
	(void) pcount;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_dlist_get_detected(csr_cs_detected_list_h list, int index, csr_cs_detected_h *pdetected)
{
	(void) list;
	(void) index;
	(void) pdetected;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}
