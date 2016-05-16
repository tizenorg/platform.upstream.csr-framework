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

#include "common/audit/logger.h"
#include "service/engine-error-converter.h"
#include "csr/error.h"

namespace Csr {

EmLogic::EmLogic(CsLoader &cs, WpLoader &wp, Db::Manager &db) :
	m_cs(cs), m_wp(wp), m_db(db)
{
}

EmLogic::~EmLogic()
{
}

RawBuffer EmLogic::getEngineName(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs.getEngineName(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp.getEngineName(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	}

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, EmString()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineVendor(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs.getEngineVendor(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp.getEngineVendor(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	}

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, EmString()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineVersion(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs.getEngineVersion(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp.getEngineVersion(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	}

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, EmString()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineDataVersion(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_cs.getEngineDataVersion(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		std::string value;
		toException(this->m_wp.getEngineDataVersion(c, value));

		EmString emString;
		emString.value = std::move(value);
		return BinaryQueue::Serialize(CSR_ERROR_NONE, emString).pop();
	}

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, EmString()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineUpdatedTime(const EmContext &context)
{
	EXCEPTION_GUARD_START

	if (context.engineId == CSR_ENGINE_CS) {
		CsEngineInfo engineInfo(this->m_cs);
		auto &c = engineInfo.get();

		time_t value;
		toException(this->m_cs.getEngineLatestUpdateTime(c, &value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	} else {
		WpEngineInfo engineInfo(this->m_wp);
		auto &c = engineInfo.get();

		time_t value;
		toException(this->m_wp.getEngineLatestUpdateTime(c, &value));

		return BinaryQueue::Serialize(CSR_ERROR_NONE, value).pop();
	}

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, -1).pop();

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
		toException(this->m_cs.getEngineActivated(c, &value));

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
		toException(this->m_wp.getEngineActivated(c, &value));

		if (value == CSRE_WP_ACTIVATED)
			activated = CSR_ACTIVATED;
		else if (value == CSRE_WP_NOT_ACTIVATED)
			activated = CSR_NOT_ACTIVATED;
		else
			ThrowExc(EngineError, "Invalid returned activated val: " <<
					 static_cast<int>(value));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, static_cast<int>(activated)).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, -1).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::getEngineState(const EmContext &context)
{
	EXCEPTION_GUARD_START

	auto state = this->m_db.getEngineState(static_cast<int>(context.engineId));

	if (state == -1)
		ThrowExc(DbFailed, "No engine state exist...");

	return BinaryQueue::Serialize(CSR_ERROR_NONE, state).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, -1).pop();

	EXCEPTION_GUARD_END
}

RawBuffer EmLogic::setEngineState(const EmContext &context, csr_state_e state)
{
	EXCEPTION_GUARD_START

	this->m_db.setEngineState(static_cast<int>(context.engineId), static_cast<int>(state));

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

	EXCEPTION_GUARD_END
}

}
