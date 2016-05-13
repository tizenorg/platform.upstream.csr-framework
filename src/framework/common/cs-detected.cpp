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
	severity(CSR_CS_SEVERITY_LOW),
	response(CSR_CS_NO_ASK_USER),
	isApp(false),
	ts(0)
{
}

CsDetected::~CsDetected()
{
}

CsDetected::CsDetected(IStream &stream)
{
	Deserializer<std::string, std::string, std::string, std::string>::Deserialize(
		stream, targetName, malwareName, detailedUrl, pkgId);

	int intSeverity;
	int intResponse;
	Deserializer<int, int, bool, time_t>::Deserialize(
		stream, intSeverity, intResponse, isApp, ts);
	severity = static_cast<csr_cs_severity_level_e>(intSeverity);
	response = static_cast<csr_cs_user_response_e>(intResponse);
}

void CsDetected::Serialize(IStream &stream) const
{
	Serializer<std::string, std::string, std::string, std::string>::Serialize(
		stream, targetName, malwareName, detailedUrl, pkgId);

	Serializer<int, int, bool, time_t>::Serialize(
		stream, static_cast<int>(severity), static_cast<int>(response), isApp, ts);
}

CsDetected::CsDetected(CsDetected &&other) :
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

CsDetected &CsDetected::operator=(CsDetected &&other)
{
	if (this == &other)
		return *this;

	targetName = std::move(other.targetName);
	malwareName = std::move(other.malwareName);
	detailedUrl = std::move(other.detailedUrl);
	pkgId = std::move(other.pkgId);
	severity = other.severity;
	response = other.response;
	isApp = other.isApp;
	ts = other.ts;

	return *this;
}

bool CsDetected::hasValue(void) const noexcept
{
	// malware name field is mandatory for detected item.
	// so we could use it for validating detected instance
	return !malwareName.empty();
}

}
