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
 * @file        web-protection.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Web Protection APIs
 */
#include "csr/web-protection.h"

#include <functional>

#include "common/message-buffer.h"
#include "common/command-id.h"
#include "common/audit/logger.h"
#include "client/client-common.h"

#define API __attribute__((visibility("default")))

API
int csr_wp_context_create(csr_wp_context_h* phandle)
{
	(void) phandle;

	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_wp_context_destroy(csr_wp_context_h handle)
{
	(void) handle;

	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_wp_set_ask_user(csr_wp_context_h handle, csr_wp_ask_user_e ask_user)
{
	(void) handle;
	(void) ask_user;

	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_wp_set_popup_message(csr_wp_context_h handle, const char* message)
{
	(void) handle;
	(void) message;

	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_wp_check_url(csr_wp_context_h handle, const char *url, csr_wp_check_result_h *presult)
{
	(void) handle;
	(void) presult;

	DEBUG("start");
	// Request to server sample with arguments
	return Csr::Client::post([&]() {
			// TODO: options in handle should be serialized and send to server
			return Csr::MessageBuffer::Serialize(
				Csr::CommandId::CHECK_URL,
				Csr::Client::toStlString(url));
		});
	// TODO: Deserialize result and give it out
}

API
int csr_wp_result_get_risk_level(csr_wp_check_result_h result, csr_wp_risk_level_e* plevel)
{
	(void) result;
	(void) plevel;

	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_wp_result_get_user_response(csr_wp_check_result_h result, csr_wp_user_response_e* presponse)
{
	(void) result;
	(void) presponse;

	DEBUG("start");
	return CSR_ERROR_NONE;
}
