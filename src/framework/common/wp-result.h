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
 * @file        wp-result.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Web protection result
 */
#pragma once

#include <string>

#include "common/types.h"
#include "csr/web-protection-types.h"

namespace Csr {

class WpResult : public Result {
public:
	// key for set/get
	enum class Key : int {
		DetailedUrl  = 0x01, // string

		RiskLevel    = 0x10, // int
		UserResponse = 0x11  // int
	};

	WpResult();
	virtual ~WpResult();

	WpResult(IStream &);
	virtual void Serialize(IStream &) const override;

	WpResult(WpResult &&);
	WpResult &operator=(WpResult &&);

	virtual void set(int, int) override;
	virtual void set(int, const std::string &) override;
	virtual void set(int, const char *) override;

	virtual void get(int, int &) const override;
	virtual void get(int, const char **) const override;

private:
	csr_wp_risk_level_e m_riskLevel;
	std::string m_detailedUrl;
	csr_wp_user_response_e m_response;
};

}
