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
 * @file        cs-context.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Content screening context with dependent options
 */
#include "common/cs-context.h"

#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {

CsContext::CsContext() noexcept :
	askUser(CSR_CS_ASK_USER_NO),
	coreUsage(CSR_CS_CORE_USAGE_DEFAULT),
	isScanOnCloud(false),
	isScannedCbRegistered(false)
{
}

CsContext::CsContext(IStream &stream)
{
	int intAskUser;
	int intCoreUsage;
	Deserializer<std::string, int, int, bool, bool>::Deserialize(stream,
			this->popupMessage, intAskUser, intCoreUsage, this->isScanOnCloud,
			this->isScannedCbRegistered);

	this->askUser = static_cast<csr_cs_ask_user_e>(intAskUser);
	this->coreUsage = static_cast<csr_cs_core_usage_e>(intCoreUsage);
}

void CsContext::Serialize(IStream &stream) const
{
	Serializer<std::string, int, int, bool, bool>::Serialize(stream,
			this->popupMessage, static_cast<int>(this->askUser),
			static_cast<int>(this->coreUsage), this->isScanOnCloud,
			this->isScannedCbRegistered);
}

void CsContext::set(int key, int value)
{
	switch (static_cast<Key>(key)) {
	case Key::AskUser:
		this->askUser = static_cast<csr_cs_ask_user_e>(value);
		break;

	case Key::CoreUsage:
		this->coreUsage = static_cast<csr_cs_core_usage_e>(value);
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to set as int.");
	}
}

void CsContext::set(int key, const std::string &value)
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		this->popupMessage = value;
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to set as string.");
	}
}

void CsContext::set(int key, const char *value)
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		this->popupMessage = value;
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to set as string.");
	}
}

void CsContext::set(int key, bool value)
{
	switch (static_cast<Key>(key)) {
	case Key::ScanOnCloud:
		this->isScanOnCloud = value;
		break;

	case Key::ScannedCbRegistered:
		this->isScannedCbRegistered = value;
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to set as bool.");
	}
}

void CsContext::get(int key, int &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::AskUser:
		value = static_cast<int>(this->askUser);
		break;

	case Key::CoreUsage:
		value = static_cast<int>(this->coreUsage);
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to get as int.");
	}
}

void CsContext::get(int key, std::string &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		value = this->popupMessage;
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to get as string.");
	}
}

void CsContext::get(int key, const char **value) const
{
	if (value == nullptr)
		throw std::invalid_argument("output storage pointer is null.");

	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		*value = this->popupMessage.c_str();
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to get as string.");
	}
}

void CsContext::get(int key, bool &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::ScanOnCloud:
		value = this->isScanOnCloud;
		break;

	case Key::ScannedCbRegistered:
		value = this->isScannedCbRegistered;
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid key[" << key << "] comes in to get as bool.");
	}
}

}
