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

#include <csr-error.h>

#include "common/audit/logger.h"
#include "common/exception.h"

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
		ThrowExc(CSR_ERROR_DB, "DB init failed");
}

RawBuffer EmLogic::getEngineName(const EmContext &context)
{
	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineContext engineContext(this->m_cs);
		auto &c = engineContext.get();

		std::string value;
		this->m_cs->getEngineName(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineContext engineContext(this->m_wp);
		auto &c = engineContext.get();

		std::string value;
		this->m_wp->getEngineName(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}
}

RawBuffer EmLogic::getEngineVendor(const EmContext &context)
{
	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineContext engineContext(this->m_cs);
		auto &c = engineContext.get();

		std::string value;
		this->m_cs->getEngineVendor(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineContext engineContext(this->m_wp);
		auto &c = engineContext.get();

		std::string value;
		this->m_wp->getEngineVendor(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}
}

RawBuffer EmLogic::getEngineVersion(const EmContext &context)
{
	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineContext engineContext(this->m_cs);
		auto &c = engineContext.get();

		std::string value;
		this->m_cs->getEngineVersion(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineContext engineContext(this->m_wp);
		auto &c = engineContext.get();

		std::string value;
		this->m_wp->getEngineVersion(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}
}

RawBuffer EmLogic::getEngineDataVersion(const EmContext &context)
{
	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineContext engineContext(this->m_cs);
		auto &c = engineContext.get();

		std::string value;
		this->m_cs->getEngineDataVersion(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineContext engineContext(this->m_wp);
		auto &c = engineContext.get();

		std::string value;
		this->m_wp->getEngineDataVersion(c, value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}
}

RawBuffer EmLogic::getEngineUpdatedTime(const EmContext &context)
{
	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineContext engineContext(this->m_cs);
		auto &c = engineContext.get();

		time_t value;
		this->m_cs->getEngineLatestUpdateTime(c, &value);

		int64_t ts64 = static_cast<int64_t>(value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, ts64).pop();
	} else {
		WpEngineContext engineContext(this->m_wp);
		auto &c = engineContext.get();

		time_t value;
		this->m_wp->getEngineLatestUpdateTime(c, &value);

		int64_t ts64 = static_cast<int64_t>(value);

		return BinaryQueue::Serialize(CSR_ERROR_NONE, ts64).pop();
	}
}

RawBuffer EmLogic::getEngineActivated(const EmContext &context)
{
	csr_activated_e activated = CSR_NOT_ACTIVATED;

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineContext engineContext(this->m_cs);
		auto &c = engineContext.get();

		csre_cs_activated_e value;
		this->m_cs->getEngineActivated(c, &value);

		if (value == CSRE_CS_ACTIVATED)
			activated = CSR_ACTIVATED;
		else if (value == CSRE_CS_NOT_ACTIVATED)
			activated = CSR_NOT_ACTIVATED;
		else
			ThrowExc(CSR_ERROR_ENGINE_INTERNAL, "Invalid returned activated val: " <<
					 static_cast<int>(value));
	} else {
		WpEngineContext engineContext(this->m_wp);
		auto &c = engineContext.get();

		csre_wp_activated_e value;
		this->m_wp->getEngineActivated(c, &value);

		if (value == CSRE_WP_ACTIVATED)
			activated = CSR_ACTIVATED;
		else if (value == CSRE_WP_NOT_ACTIVATED)
			activated = CSR_NOT_ACTIVATED;
		else
			ThrowExc(CSR_ERROR_ENGINE_INTERNAL, "Invalid returned activated val: " <<
					 static_cast<int>(value));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, static_cast<int>(activated)).pop();
}

RawBuffer EmLogic::getEngineState(const EmContext &context)
{
	if (!_isValid(context.engineId))
		ThrowExc(CSR_ERROR_SERVER, "invalid engine id comes to get engine state.");

	auto state = this->m_db->getEngineState(context.engineId);

	return BinaryQueue::Serialize(CSR_ERROR_NONE,
			static_cast<int>(state) == -1 ? static_cast<int>(CSR_STATE_DISABLE) :
											static_cast<int>(state)).pop();
}

RawBuffer EmLogic::setEngineState(const EmContext &context, csr_state_e state)
{
	if (!_isValid(context.engineId) || !_isValid(state))
		ThrowExc(CSR_ERROR_SERVER, "invalid argument comes to set engine state.");

	this->m_db->setEngineState(context.engineId, state);

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
}

}
