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

#include <stdexcept>

#include "common/audit/logger.h"

namespace Csr {

CsContext::CsContext() :
	IContext(),
	popupMessage(),
	askUser(CSR_CS_NOT_ASK_USER),
	coreUsage(CSR_CS_USE_CORE_DEFAULT),
	isScanOnCloud(false)
{
}

CsContext::~CsContext()
{
}

CsContext::CsContext(IStream &stream)
{
	int intAskUser;
	int intCoreUsage;
	Deserializer<std::string, int, int, bool>::Deserialize(stream,
			popupMessage, intAskUser, intCoreUsage, isScanOnCloud);

	askUser = static_cast<csr_cs_ask_user_e>(intAskUser);
	coreUsage = static_cast<csr_cs_core_usage_e>(intCoreUsage);
}

void CsContext::Serialize(IStream &stream) const
{
	Serializer<std::string, int, int, bool>::Serialize(stream,
			popupMessage, static_cast<int>(askUser), static_cast<int>(coreUsage),
			isScanOnCloud);
}

void CsContext::set(int key, int value)
{
	switch (static_cast<Key>(key)) {
	case Key::AskUser:
		askUser = static_cast<csr_cs_ask_user_e>(value);
		break;

	case Key::CoreUsage:
		coreUsage = static_cast<csr_cs_core_usage_e>(value);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as int."));
	}
}

void CsContext::set(int key, const std::string &value)
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		popupMessage = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as string."));
	}
}

void CsContext::set(int key, const char *value)
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		popupMessage = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as string."));
	}
}

void CsContext::set(int key, bool value)
{
	switch (static_cast<Key>(key)) {
	case Key::ScanOnCloud:
		isScanOnCloud = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as bool."));
	}
}

void CsContext::get(int key, int &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::AskUser:
		value = static_cast<int>(askUser);
		break;

	case Key::CoreUsage:
		value = static_cast<int>(coreUsage);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as int."));
	}
}

void CsContext::get(int key, std::string &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		value = popupMessage;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as string."));
	}
}

void CsContext::get(int key, const char **value) const
{
	if (value == nullptr)
		throw std::logic_error("invalud argument. output storage pointer is null.");

	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		*value = popupMessage.c_str();
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as string."));
	}
}

void CsContext::get(int key, bool &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::ScanOnCloud:
		value = isScanOnCloud;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as bool."));
	}
}

}
