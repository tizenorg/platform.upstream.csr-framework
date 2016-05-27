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
#include <cstdint>

#include "common/audit/logger.h"

namespace Csr {

CsDetected::CsDetected() noexcept :
	severity(CSR_CS_SEVERITY_LOW),
	response(CSR_CS_NO_ASK_USER),
	isApp(false),
	ts(0)
{
}

CsDetected::CsDetected(IStream &stream)
{
	Deserializer<std::string, std::string, std::string, std::string>::Deserialize(
		stream, this->targetName, this->malwareName, this->detailedUrl, this->pkgId);

	int intSeverity;
	int intResponse;
	int64_t ts64;
	Deserializer<int, int, bool, int64_t>::Deserialize(
		stream, intSeverity, intResponse, isApp, ts64);
	this->severity = static_cast<csr_cs_severity_level_e>(intSeverity);
	this->response = static_cast<csr_cs_user_response_e>(intResponse);
	this->ts = static_cast<time_t>(ts64);
}

void CsDetected::Serialize(IStream &stream) const
{
	Serializer<std::string, std::string, std::string, std::string>::Serialize(
		stream, this->targetName, this->malwareName, this->detailedUrl, this->pkgId);

	auto ts64 = static_cast<int64_t>(this->ts);

	Serializer<int, int, bool, int64_t>::Serialize(
		stream, static_cast<int>(this->severity), static_cast<int>(this->response),
		this->isApp, ts64);
}

CsDetected::CsDetected(CsDetected &&other) noexcept :
	targetName(std::move(other.targetName)),
	malwareName(std::move(other.malwareName)),
	detailedUrl(std::move(other.detailedUrl)),
	pkgId(std::move(other.pkgId)),
	severity(other.severity),
	response(other.response),
	isApp(other.isApp),
	ts(other.ts)
{
}

CsDetected &CsDetected::operator=(CsDetected &&other) noexcept
{
	if (this == &other)
		return *this;

	this->targetName = std::move(other.targetName);
	this->malwareName = std::move(other.malwareName);
	this->detailedUrl = std::move(other.detailedUrl);
	this->pkgId = std::move(other.pkgId);
	this->severity = other.severity;
	this->response = other.response;
	this->isApp = other.isApp;
	this->ts = other.ts;

	return *this;
}

bool CsDetected::operator>(const CsDetected &other) const noexcept
{
	return this->severity > other.severity;
}

bool CsDetected::operator<(const CsDetected &other) const noexcept
{
	return this->severity < other.severity;
}

}
