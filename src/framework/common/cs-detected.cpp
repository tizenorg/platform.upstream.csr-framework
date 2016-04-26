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
 * @file        cs-detected.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Content screening detected result
 */
#include "common/cs-detected.h"

#include <stdexcept>

#include "common/audit/logger.h"

namespace Csr {

CsDetected::CsDetected() :
	m_targetName(),
	m_malwareName(),
	m_detailedUrl(),
	m_severity(CSR_CS_SEVERITY_LOW),
	m_threat(CSR_CS_THREAT_GENERIC),
	m_response(CSR_CS_NO_ASK_USER),
	m_isApp(false),
	m_ts(0)
{
}

CsDetected::~CsDetected()
{
}

CsDetected::CsDetected(IStream &stream) : Result(stream)
{
	if (!hasValue())
		return;

	int intSeverity;
	int intThreat;
	int intResponse;
	Deserializer<std::string, std::string, std::string, int, int, int, time_t>::Deserialize(
		stream, m_targetName, m_malwareName, m_detailedUrl, intSeverity, intThreat,
		intResponse, m_ts);

	m_severity = static_cast<csr_cs_severity_level_e>(intSeverity);
	m_threat = static_cast<csr_cs_threat_type_e>(intThreat);
	m_response = static_cast<csr_cs_user_response_e>(intResponse);
}

void CsDetected::Serialize(IStream &stream) const
{
	Result::Serialize(stream);

	if (!hasValue())
		return;

	Serializer<std::string, std::string, std::string, int, int, int, time_t>::Serialize(
		stream, m_targetName, m_malwareName, m_detailedUrl,
		static_cast<int>(m_severity), static_cast<int>(m_threat),
		static_cast<int>(m_response), m_ts);
}

CsDetected::CsDetected(CsDetected &&other) :
	Result(std::move(other)),
	m_targetName(std::move(other.m_targetName)),
	m_malwareName(std::move(other.m_malwareName)),
	m_detailedUrl(std::move(other.m_detailedUrl)),
	m_severity(other.m_severity),
	m_threat(other.m_threat),
	m_response(other.m_response),
	m_ts(other.m_ts)
{
}

CsDetected &CsDetected::operator=(CsDetected &&other)
{
	if (this == &other)
		return *this;

	Result::operator=(std::move(other));

	m_targetName = std::move(other.m_targetName);
	m_malwareName = std::move(other.m_malwareName);
	m_detailedUrl = std::move(other.m_detailedUrl);
	m_severity = other.m_severity;
	m_threat = other.m_threat;
	m_response = other.m_response;
	m_ts = other.m_ts;

	return *this;
}

void CsDetected::set(int key, int value)
{
	switch (static_cast<Key>(key)) {
	case Key::Severity:
		m_severity = static_cast<csr_cs_severity_level_e>(value);
		break;

	case Key::Threat:
		m_threat = static_cast<csr_cs_threat_type_e>(value);
		break;

	case Key::UserResponse:
		m_response = static_cast<csr_cs_user_response_e>(value);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as int."));
	}

	setValueFlag();
}

void CsDetected::set(int key, bool value)
{
	switch (static_cast<Key>(key)) {
	case Key::IsApp:
		m_isApp = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as bool."));
	}

	setValueFlag();
}

void CsDetected::set(int key, const std::string &value)
{
	switch (static_cast<Key>(key)) {
	case Key::TargetName:
		m_targetName = value;
		break;

	case Key::MalwareName:
		m_malwareName = value;
		break;

	case Key::DetailedUrl:
		m_detailedUrl = value;
		break;

	case Key::PkgId:
		m_pkgId = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as string."));
	}

	setValueFlag();
}

void CsDetected::set(int key, const char *value)
{
	switch (static_cast<Key>(key)) {
	case Key::TargetName:
		m_targetName = value;
		break;

	case Key::MalwareName:
		m_malwareName = value;
		break;

	case Key::DetailedUrl:
		m_detailedUrl = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as string."));
	}

	setValueFlag();
}

void CsDetected::set(int key, time_t value)
{
	switch (static_cast<Key>(key)) {
	case Key::TimeStamp:
		m_ts = value;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to set as time_t."));
	}

	setValueFlag();
}

void CsDetected::get(int key, int &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::Severity:
		value = static_cast<int>(m_severity);
		break;

	case Key::Threat:
		value = static_cast<int>(m_threat);
		break;

	case Key::UserResponse:
		value = static_cast<int>(m_response);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as int."));
	}
}

void CsDetected::get(int key, bool &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::IsApp:
		value = m_isApp;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as bool."));
	}
}

void CsDetected::get(int key, const char **value) const
{
	if (value == nullptr)
		throw std::logic_error("invalud argument. output storage pointer is null.");

	switch (static_cast<Key>(key)) {
	case Key::TargetName:
		*value = m_targetName.c_str();
		break;

	case Key::MalwareName:
		*value = m_malwareName.c_str();
		break;

	case Key::DetailedUrl:
		*value = m_detailedUrl.c_str();
		break;

	case Key::PkgId:
		*value = m_pkgId.c_str();
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as string."));
	}
}

void CsDetected::get(int key, time_t &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::TimeStamp:
		value = m_ts;
		break;

	default:
		throw std::logic_error(FORMAT("Invalid key[" << key <<
									  "] comes in to get as time_t."));
	}
}

}
