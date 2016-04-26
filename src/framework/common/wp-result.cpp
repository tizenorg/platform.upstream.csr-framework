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
 * @file        wp-result.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Web protection result
 */
#include "common/wp-result.h"

#include <stdexcept>

#include "common/audit/logger.h"

namespace Csr {

WpResult::WpResult() :
	m_riskLevel(CSR_WP_RISK_LOW),
	m_detailedUrl(),
	m_response(CSR_WP_NO_ASK_USER)
{
}

WpResult::~WpResult()
{
}

WpResult::WpResult(IStream &stream) : Result(stream)
{
	if (!hasValue())
		return;

	int intRiskLevel;
	int intResponse;
	Deserializer<int, std::string, int>::Deserialize(stream,
			intRiskLevel, m_detailedUrl, intResponse);

	m_riskLevel = static_cast<csr_wp_risk_level_e>(intRiskLevel);
	m_response = static_cast<csr_wp_user_response_e>(intResponse);
}

void WpResult::Serialize(IStream &stream) const
{
	Result::Serialize(stream);

	if (!hasValue())
		return;

	Serializer<int, std::string, int>::Serialize(stream,
			static_cast<int>(m_riskLevel), m_detailedUrl, static_cast<int>(m_response));
}

WpResult::WpResult(WpResult &&other) :
	Result(std::move(other)),
	m_riskLevel(other.m_riskLevel),
	m_detailedUrl(std::move(other.m_detailedUrl)),
	m_response(other.m_response)
{
}

WpResult &WpResult::operator=(WpResult &&other)
{
	if (this == &other)
		return *this;

	Result::operator=(std::move(other));

	m_detailedUrl = std::move(other.m_detailedUrl);
	m_riskLevel = other.m_riskLevel;
	m_response = other.m_response;

	return *this;
}

void WpResult::set(int key, int value)
{
	switch (static_cast<Key>(key)) {
	case Key::RiskLevel:
		m_riskLevel = static_cast<csr_wp_risk_level_e>(value);
		break;

	case Key::UserResponse:
		m_response = static_cast<csr_wp_user_response_e>(value);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key
									  << "] comes in to set as int. value=" << value));
	}

	setValueFlag();
}

void WpResult::set(int key, const std::string &value)
{
	switch (static_cast<Key>(key)) {
	case Key::DetailedUrl:
		m_detailedUrl = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key
									  << "] comes in to set as std::string. value=" << value));
	}

	setValueFlag();
}

void WpResult::set(int key, const char *value)
{
	switch (static_cast<Key>(key)) {
	case Key::DetailedUrl:
		m_detailedUrl = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key
									  << "] comes in to set as char*. value=" << value));
	}

	setValueFlag();
}

void WpResult::get(int key, int &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::RiskLevel:
		value = static_cast<int>(m_riskLevel);
		break;

	case Key::UserResponse:
		value = static_cast<int>(m_response);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key
									  << "] comes in to set as int. value=" << value));
	}
}

void WpResult::get(int key, const char **value) const
{
	if (value == nullptr)
		throw std::logic_error("invalid argument. output storage pointer is null.");

	switch (static_cast<Key>(key)) {
	case Key::DetailedUrl:
		*value = m_detailedUrl.c_str();
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key
									  << "] comes in to set as char**. value=" << value));
	}
}

}
