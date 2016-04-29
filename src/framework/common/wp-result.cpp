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

namespace Csr {

WpResult::WpResult() :
	riskLevel(CSR_WP_RISK_LOW),
	detailedUrl(),
	response(CSR_WP_NO_ASK_USER)
{
}

WpResult::~WpResult()
{
}

WpResult::WpResult(IStream &stream)
{
	int intRiskLevel;
	int intResponse;
	Deserializer<int, std::string, int>::Deserialize(stream,
			intRiskLevel, detailedUrl, intResponse);

	riskLevel = static_cast<csr_wp_risk_level_e>(intRiskLevel);
	response = static_cast<csr_wp_user_response_e>(intResponse);
}

void WpResult::Serialize(IStream &stream) const
{
	Serializer<int, std::string, int>::Serialize(stream,
			static_cast<int>(riskLevel), detailedUrl, static_cast<int>(response));
}

WpResult::WpResult(WpResult &&other) :
	riskLevel(other.riskLevel),
	detailedUrl(std::move(other.detailedUrl)),
	response(other.response)
{
}

WpResult &WpResult::operator=(WpResult &&other)
{
	if (this == &other)
		return *this;

	detailedUrl = std::move(other.detailedUrl);
	riskLevel = other.riskLevel;
	response = other.response;

	return *this;
}

}
