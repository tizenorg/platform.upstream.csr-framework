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
 * @file        em-logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Engine management logic
 */
#include "service/em-logic.h"

#include <string>
#include <utility>
#include <cstdint>

#include "common/audit/logger.h"
#include "service/engine-error-converter.h"
#include "service/exception.h"
#include <csr-error.h>

namespace Csr {

namespace {

bool _isValid(const csr_engine_id_e &id)
{
	switch (id) {
	case CSR_ENGINE_CS:
	case CSR_ENGINE_WP:
		return true;

	default:
		return false;
	}
}

bool _isValid(const csr_state_e &state)
{
	switch (state) {
	case CSR_STATE_ENABLE:
	case CSR_STATE_DISABLE:
		return true;

	default:
		return false;
	}
}

} // namespace anonymous

EmLogic::EmLogic(const std::shared_ptr<CsLoader> &cs, const std::shared_ptr<WpLoader> &wp,
				 const std::shared_ptr<Db::Manager> &db) : m_cs(cs), m_wp(wp), m_db(db)
{
	if (!this->m_db)
		ThrowExc(DbFailed, "DB init failed");
}

RawBuffer EmLogic::getEngineName(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs->getEngineName(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp->getEngineName(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineVendor(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs->getEngineVendor(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp->getEngineVendor(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineVersion(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs->getEngineVersion(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp->getEngineVersion(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineDataVersion(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs->getEngineDataVersion(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp->getEngineDataVersion(c, value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineUpdatedTime(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		time_t value;
		toException(this->m_cs->getEngineLatestUpdateTime(c, &value));

		int64_t ts64 = static_cast<int64_t>(value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, ts64).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		time_t value;
		toException(this->m_wp->getEngineLatestUpdateTime(c, &value));

		int64_t ts64 = static_cast<int64_t>(value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, ts64).pop();
	}

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineActivated(const EmContext &context)
{
	EXCEPTION_GUARD_START

	csr_activated_e activated = CSR_NOT_ACTIVATED;

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		csre_cs_activated_e value;
		toException(this->m_cs->getEngineActivated(c, &value));

		if (value == CSRE_CS_ACTIVATED)
			activated = CSR_ACTIVATED;
		else if (value == CSRE_CS_NOT_ACTIVATED)
			activated = CSR_NOT_ACTIVATED;
		else
			ThrowExc(EngineError, "Invalid returned activated val: " <<
					 static_cast<int>(value));
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		csre_wp_activated_e value;
		toException(this->m_wp->getEngineActivated(c, &value));

		if (value == CSRE_WP_ACTIVATED)
			activated = CSR_ACTIVATED;
		else if (value == CSRE_WP_NOT_ACTIVATED)
			activated = CSR_NOT_ACTIVATED;
		else
			ThrowExc(EngineError, "Invalid returned activated val: " <<
					 static_cast<int>(value));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, static_cast<int>(activated)).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineState(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (!_isValid(context.engineId))
		ThrowExc(InternalError, "invalid engine id comes to get engine state.");

	auto state = this->m_db->getEngineState(context.engineId);

	return BinaryQueue::Serialize(CSR_ERROR_NONE,
			static_cast<int>(state) == -1 ? static_cast<int>(CSR_STATE_DISABLE) :
											static_cast<int>(state)).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::setEngineState(const EmContext &context, csr_state_e state)
{
	EXCEPTION_GUARD_START

	if (!_isValid(context.engineId) || !_isValid(state))
		ThrowExc(InternalError, "invalid argument comes to set engine state.");

	this->m_db->setEngineState(context.engineId, state);

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_END
}

}
