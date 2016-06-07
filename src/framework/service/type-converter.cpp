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
 * @file        type-converter.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Converts types of engine interface to capi interface
 */
#include "service/type-converter.h"

#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {

csr_cs_severity_level_e convert(const csre_cs_severity_level_e &e)
{
	switch (e) {
	case CSRE_CS_SEVERITY_LOW:
		return CSR_CS_SEVERITY_LOW;

	case CSRE_CS_SEVERITY_MEDIUM:
		return CSR_CS_SEVERITY_MEDIUM;

	case CSRE_CS_SEVERITY_HIGH:
		return CSR_CS_SEVERITY_HIGH;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid eseverity: " << static_cast<int>(e));
	}
}

csr_wp_risk_level_e convert(const csre_wp_risk_level_e &e)
{
	switch (e) {
	case CSRE_WP_RISK_LOW:
		return CSR_WP_RISK_LOW;

	case CSRE_WP_RISK_UNVERIFIED:
		return CSR_WP_RISK_UNVERIFIED;

	case CSRE_WP_RISK_MEDIUM:
		return CSR_WP_RISK_MEDIUM;

	case CSRE_WP_RISK_HIGH:
		return CSR_WP_RISK_HIGH;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid elevel: " << static_cast<int>(e));
	}
}

}
