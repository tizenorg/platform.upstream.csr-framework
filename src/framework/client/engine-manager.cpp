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
 * @file        engine-manager.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Engine manager APIs
 */
#include <csr-engine-manager.h>

#include <new>
#include <utility>
#include <cstring>

#include "client/utils.h"
#include "client/handle.h"
#include "common/command-id.h"
#include "common/em-context.h"
#include "common/em-result.h"
#include "common/audit/logger.h"

using namespace Csr;

#define GETTER_PARAM_CHECK(engine, poutput) \
	if (engine == nullptr)                         \
		return CSR_ERROR_INVALID_HANDLE;           \
	else if (poutput == nullptr)                   \
		return CSR_ERROR_INVALID_PARAMETER

#define STRING_GETTER(engine, cid, poutput)                                \
	do {                                                                   \
		GETTER_PARAM_CHECK(engine, poutput);                               \
		auto h = reinterpret_cast<Csr::Client::Handle *>(engine);          \
		auto ret = h->dispatch<std::pair<int, EmString *>>(                \
				Csr::CommandId::cid, h->getContext());                     \
		if (ret.first != CSR_ERROR_NONE) {                                 \
			ERROR("Error to get string about engine. ret: " << ret.first); \
			return ret.first;                                              \
		} else if (ret.second == nullptr) {                                \
			ERROR("Serialization error. should have value.");              \
			return CSR_ERROR_UNKNOWN;                                      \
		}                                                                  \
		h->add(Csr::ResultPtr(ret.second));                                \
		auto output = strdup(                                              \
				ret.second->value.c_str());                                \
		if (output == nullptr)                                             \
			return CSR_ERROR_OUT_OF_MEMORY;                                \
		*poutput = output;                                                 \
		return CSR_ERROR_NONE;                                             \
	} while (false)

namespace {

bool _isValid(const csr_engine_id_e &id) noexcept
{
	switch (id) {
	case CSR_ENGINE_CS:
	case CSR_ENGINE_WP:
		return true;

	default:
		return false;
	}
}

bool _isValid(const csr_state_e &state) noexcept
{
	switch (state) {
	case CSR_ENABLE:
	case CSR_DISABLE:
		return true;

	default:
		return false;
	}
}

}

API
int csr_get_current_engine(csr_engine_id_e id, csr_engine_h *pengine)
{
	EXCEPTION_SAFE_START

	if (pengine == nullptr || !_isValid(id))
		return CSR_ERROR_INVALID_PARAMETER;

	auto handle = new Csr::Client::Handle(SockId::ADMIN, std::make_shared<EmContext>());

	handle->getContext()->set(static_cast<int>(EmContext::Key::EngineId),
							  static_cast<int>(id));

	*pengine = reinterpret_cast<csr_engine_h>(handle);

	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_engine_get_vendor(csr_engine_h engine, char **pvendor)
{
	EXCEPTION_SAFE_START

	STRING_GETTER(engine, EM_GET_VENDOR, pvendor);

	EXCEPTION_SAFE_END
}

API
int csr_engine_get_name(csr_engine_h engine, char **pname)
{
	EXCEPTION_SAFE_START

	STRING_GETTER(engine, EM_GET_NAME, pname);

	EXCEPTION_SAFE_END
}

API
int csr_engine_get_version(csr_engine_h engine, char **pversion)
{
	EXCEPTION_SAFE_START

	STRING_GETTER(engine, EM_GET_VERSION, pversion);

	EXCEPTION_SAFE_END
}

API
int csr_engine_get_data_version(csr_engine_h engine, char **pversion)
{
	EXCEPTION_SAFE_START

	STRING_GETTER(engine, EM_GET_DATA_VERSION, pversion);

	EXCEPTION_SAFE_END
}

API
int csr_engine_get_latest_update_time(csr_engine_h engine, time_t *ptime)
{
	EXCEPTION_SAFE_START

	GETTER_PARAM_CHECK(engine, ptime);

	auto h = reinterpret_cast<Csr::Client::Handle *>(engine);
	auto ret = h->dispatch<std::pair<int, int64_t>>(
			Csr::CommandId::EM_GET_UPDATED_TIME, h->getContext());

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error to get updated time. ret: " << ret.first);
		return ret.first;
	}

	*ptime = static_cast<time_t>(ret.second);
	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_engine_get_activated(csr_engine_h engine, csr_activated_e *pactivated)
{
	EXCEPTION_SAFE_START

	GETTER_PARAM_CHECK(engine, pactivated);

	auto h = reinterpret_cast<Csr::Client::Handle *>(engine);
	auto ret = h->dispatch<std::pair<int, int>>(
			Csr::CommandId::EM_GET_ACTIVATED, h->getContext());

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error to get activated. ret: " << ret.first);
		return ret.first;
	}

	*pactivated = static_cast<csr_activated_e>(ret.second);
	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_engine_get_state(csr_engine_h engine, csr_state_e *pstate)
{
	EXCEPTION_SAFE_START

	GETTER_PARAM_CHECK(engine, pstate);

	auto h = reinterpret_cast<Csr::Client::Handle *>(engine);
	auto ret = h->dispatch<std::pair<int, int>>(
			Csr::CommandId::EM_GET_STATE, h->getContext());

	if (ret.first != CSR_ERROR_NONE) {
		ERROR("Error to get state. ret: " << ret.first);
		return ret.first;
	}

	*pstate = static_cast<csr_state_e>(ret.second);
	return CSR_ERROR_NONE;

	EXCEPTION_SAFE_END
}

API
int csr_engine_set_state(csr_engine_h engine, csr_state_e state)
{
	EXCEPTION_SAFE_START

	if (engine == nullptr)
		return CSR_ERROR_INVALID_HANDLE;
	else if (!_isValid(state))
		return CSR_ERROR_INVALID_PARAMETER;

	auto h = reinterpret_cast<Csr::Client::Handle *>(engine);
	auto ret = h->dispatch<int>(
			Csr::CommandId::EM_SET_STATE, h->getContext(), static_cast<int>(state));

	if (ret != CSR_ERROR_NONE)
		ERROR("Failed to set state. ret: " << ret);

	return ret;

	EXCEPTION_SAFE_END
}

API
int csr_engine_destroy(csr_engine_h engine)
{
	if (engine == nullptr)
		return CSR_ERROR_INVALID_HANDLE;

	delete reinterpret_cast<Csr::Client::Handle *>(engine);
	return CSR_ERROR_NONE;
}
