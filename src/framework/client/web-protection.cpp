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

#include <new>

#include "client/utils.h"
#include "client/handle.h"
#include "common/types.h"
#include "common/command-id.h"
#include "common/audit/logger.h"

using namespace Csr;

API
int csr_wp_context_create(csr_wp_context_h* phandle)
{
	EXCEPTION_SAFE_START

	if (phandle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	*phandle = reinterpret_cast<csr_wp_context_h>(
		new Client::Handle(std::make_shared<Context>()));

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_wp_context_destroy(csr_wp_context_h handle)
{
	EXCEPTION_SAFE_START

	if (handle == nullptr)
		return CSR_ERROR_INVALID_PARAMETER;

	delete reinterpret_cast<Client::Handle *>(handle);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
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
	EXCEPTION_SAFE_START

	if (handle == nullptr || presult == nullptr
		|| url == nullptr || url[0] == '\0')
		return CSR_ERROR_INVALID_PARAMETER;

	auto h = reinterpret_cast<Client::Handle *>(handle);
	auto ret = h->dispatch<std::pair<int, Result *>>(
		CommandId::CHECK_URL,
		h->getContext(),
		std::string(url));

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error! ret: " << ret.first);
		return ret.first;
	}

	h->add(ret.second);
	*presult = reinterpret_cast<csr_wp_check_result_h>(ret.second);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
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
