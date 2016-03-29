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

#include <functional>

#include "common/message-buffer.h"
#include "common/command-id.h"
#include "common/audit/logger.h"
#include "client/client-common.h"

#define API __attribute__((visibility("default")))

API
int csr_cs_context_create(csr_cs_context_h* phandle)
{
	(void) phandle;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_context_destroy(csr_cs_context_h handle)
{
	(void) handle;

	DEBUG("start!");
	return CSR_ERROR_NONE;
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
	(void) handle;
	(void) pdetected;

	DEBUG("start!");
	// Request to server sample with arguments
	return Csr::Client::post([&]() {
			// TODO: options in handle should be serialized and send to server
			return Csr::MessageBuffer::Serialize(
				Csr::CommandId::SCAN_FILE,
				Csr::Client::toStlString(file_path));
		});
	// TODO: Deserialize detected item and give it out
}

API
int csr_cs_set_callback_on_detected(csr_cs_context_h handle, csr_cs_on_detected_cb callback)
{
	(void) handle;
	(void) callback;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_set_callback_on_completed(csr_cs_context_h handle, csr_cs_on_completed_cb callback)
{
	(void) handle;
	(void) callback;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_set_callback_on_cancelled(csr_cs_context_h handle, csr_cs_on_cancelled_cb callback)
{
	(void) handle;
	(void) callback;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_set_callback_on_error(csr_cs_context_h handle, csr_cs_on_error_cb callback)
{
	(void) handle;
	(void) callback;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_set_callback_on_file_scanned(csr_cs_context_h handle, csr_cs_on_file_scanned_cb callback)
{
	(void) handle;
	(void) callback;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_scan_files_async(csr_cs_context_h handle, const char **file_paths, unsigned int count,  void *user_data)
{
	(void) handle;
	(void) file_paths;
	(void) count;
	(void) user_data;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_scan_dir_async(csr_cs_context_h handle, const char *dir_path)
{
	(void) handle;
	(void) dir_path;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_scan_dirs_async(csr_cs_context_h handle, const char **file_paths, unsigned int count, void *user_data)
{
	(void) handle;
	(void) file_paths;
	(void) count;
	(void) user_data;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}

API
int csr_cs_scan_cancel(csr_cs_context_h handle)
{
	(void) handle;

	DEBUG("start!");
	return CSR_ERROR_NONE;
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
int csr_cs_dlist_get_detected(csr_cs_detected_list_h list, int index, csr_cs_detected_h *pdetected)
{
	(void) list;
	(void) index;
	(void) pdetected;

	DEBUG("start!");
	return CSR_ERROR_NONE;
}
