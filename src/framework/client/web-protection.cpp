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
#include <csr-web-protection.h>

#include <new>

#include "client/utils.h"
#include "client/handle.h"
#include "common/types.h"
#include "common/command-id.h"
#include "common/wp-context.h"
#include "common/wp-result.h"
#include "common/audit/logger.h"

using namespace Csr;

API
int csr_wp_context_create(csr_wp_context_h *handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;

	*handle = reinterpret_cast<csr_wp_context_h>(
				   new Client::Handle(SockId::WP, std::make_shared<WpContext>()));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_wp_context_destroy(csr_wp_context_h handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;

	delete reinterpret_cast<Client::Handle *>(handle);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_wp_set_ask_user(csr_wp_context_h handle, csr_wp_ask_user_e ask_user)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (ask_user != CSR_WP_NOT_ASK_USER && ask_user != CSR_WP_ASK_USER)
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Client::Handle *>(handle)->getContext()->set(
		static_cast<int>(WpContext::Key::AskUser), static_cast<int>(ask_user));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_wp_set_popup_message(csr_wp_context_h handle, const char *message)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (message == nullptr || message[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	reinterpret_cast<Client::Handle *>(handle)->getContext()->set(
		static_cast<int>(WpContext::Key::PopupMessage), message);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_wp_check_url(csr_wp_context_h handle, const char *url,
					 csr_wp_check_result_h *result)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (result == nullptr || url == nullptr || url[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto h = reinterpret_cast<Client::Handle *>(handle);
	auto ret = h->dispatch<std::pair<int, WpResult *>>(
				   CommandId::CHECK_URL,
				   h->getContext(),
				   std::string(url));

	if (ret.second)
		h->add(ResultPtr(ret.second));

	*result = reinterpret_cast<csr_wp_check_result_h>(ret.second);

	return ret.first;

	EXCEPTION_SAFE_END
}

API
int csr_wp_result_get_risk_level(csr_wp_check_result_h result,
								 csr_wp_risk_level_e *level)
{
	EXCEPTION_SAFE_START

	if (result == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (level == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*level = reinterpret_cast<WpResult *>(result)->riskLevel;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_wp_result_get_detailed_url(csr_wp_check_result_h result, char **detailed_url)
{
	EXCEPTION_SAFE_START

	if (result == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (detailed_url == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	auto url = strdup(reinterpret_cast<WpResult *>(result)->detailedUrl.c_str());

	if (url == nullptr)
		return CSR_ERROR_OUT_OF_MEMORY;

	*detailed_url = url;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}


API
int csr_wp_result_get_user_response(csr_wp_check_result_h result,
									csr_wp_user_response_e *response)
{
	EXCEPTION_SAFE_START

	if (result == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (response == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*response = reinterpret_cast<WpResult *>(result)->response;

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}
