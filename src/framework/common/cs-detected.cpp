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
	targetName(),
	malwareName(),
	detailedUrl(),
	severity(CSR_CS_SEVERITY_LOW),
	threat(CSR_CS_THREAT_GENERIC),
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
	int intSeverity;
	int intThreat;
	int intResponse;
	Deserializer<std::string, std::string, std::string, int, int, int, time_t>::Deserialize(
		stream, targetName, malwareName, detailedUrl, intSeverity, intThreat,
		intResponse, ts);

	severity = static_cast<csr_cs_severity_level_e>(intSeverity);
	threat = static_cast<csr_cs_threat_type_e>(intThreat);
	response = static_cast<csr_cs_user_response_e>(intResponse);
}

void CsDetected::Serialize(IStream &stream) const
{
	Serializer<std::string, std::string, std::string, int, int, int, time_t>::Serialize(
		stream, targetName, malwareName, detailedUrl,
		static_cast<int>(severity), static_cast<int>(threat),
		static_cast<int>(response), ts);
}

CsDetected::CsDetected(CsDetected &&other) :
	targetName(std::move(other.targetName)),
	malwareName(std::move(other.malwareName)),
	detailedUrl(std::move(other.detailedUrl)),
	severity(other.severity),
	threat(other.threat),
	response(other.response),
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
	severity = other.severity;
	threat = other.threat;
	response = other.response;
	ts = other.ts;

	return *this;
}

bool CsDetected::hasValue(void) const noexcept
{
	return !targetName.empty();
}

}
