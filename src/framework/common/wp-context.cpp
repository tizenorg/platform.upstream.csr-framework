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
 * @file        wp-context.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Wep protection context with dependent options
 */
#include "common/wp-context.h"

#include <stdexcept>

#include "common/audit/logger.h"

namespace Csr {

WpContext::WpContext() :
	m_popupMessage(),
	m_askUser(CSR_WP_NOT_ASK_USER)
{
}

WpContext::~WpContext()
{
}

WpContext::WpContext(IStream &stream)
{
	int intAskUser;
	Deserializer<std::string, int>::Deserialize(stream,
			m_popupMessage, intAskUser);

	m_askUser = static_cast<csr_wp_ask_user_e>(intAskUser);
}

void WpContext::Serialize(IStream &stream) const
{
	Serializer<std::string, int>::Serialize(stream, m_popupMessage,
											static_cast<int>(m_askUser));
}

WpContext::WpContext(WpContext &&other) :
	Context(std::move(other)),
	m_popupMessage(std::move(other.m_popupMessage)),
	m_askUser(other.m_askUser)
{
}

WpContext &WpContext::operator=(WpContext &&other)
{
	if (this == &other)
		return *this;

	Context::operator=(std::move(other));

	m_popupMessage = std::move(other.m_popupMessage);
	m_askUser = other.m_askUser;

	return *this;
}

WpContext::WpContext(const WpContext &other) :
	Context(other),
	m_popupMessage(other.m_popupMessage),
	m_askUser(other.m_askUser)
{
}

WpContext &WpContext::operator=(const WpContext &other)
{
	Context::operator=(other);

	m_popupMessage = other.m_popupMessage;
	m_askUser = other.m_askUser;

	return *this;
}

void WpContext::set(int key, int value)
{
	switch (static_cast<Key>(key)) {
	case Key::AskUser:
		m_askUser = static_cast<csr_wp_ask_user_e>(value);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as int. value=" << value));
	}
}

void WpContext::set(int key, const std::string &value)
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		m_popupMessage = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as std::string. value=" << value));
	}
}

void WpContext::set(int key, const char *value)
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		m_popupMessage = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as char*. value=" << value));
	}
}

void WpContext::get(int key, int &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::AskUser:
		value = static_cast<int>(m_askUser);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as int. value=" << value));
	}
}

void WpContext::get(int key, std::string &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		value = m_popupMessage;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as std::string. value=" << value));
	}
}

void WpContext::get(int key, const char **value) const
{
	if (value == nullptr)
		throw std::logic_error("invalud argument. output storage pointer is null.");

	switch (static_cast<Key>(key)) {
	case Key::PopupMessage:
		*value = m_popupMessage.c_str();
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as char*. value=" << value));
	}
}

}
