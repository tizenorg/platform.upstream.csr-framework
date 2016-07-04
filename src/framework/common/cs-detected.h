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
#pragma once

#include <string>
#include <ctime>

#include <csr-content-screening-types.h>

#include "common/macros.h"
#include "common/iresult.h"

namespace Csr {

class CsDetected;
using CsDetectedPtr = std::unique_ptr<CsDetected>;

struct API CsDetected : public IResult {
	CsDetected() noexcept;
	virtual ~CsDetected() = default;

	CsDetected(IStream &);
	virtual void Serialize(IStream &) const override;

	CsDetected(const CsDetected &) noexcept;
	CsDetected &operator=(const CsDetected &) noexcept;
	CsDetected(CsDetected &&) noexcept;
	CsDetected &operator=(CsDetected &&) noexcept;

	bool operator>(const CsDetected &other) const noexcept;
	bool operator<(const CsDetected &other) const noexcept;

	std::string targetName; // file(app) path which contains malware

	std::string malwareName;
	std::string detailedUrl;
	std::string pkgId;
	csr_cs_severity_level_e severity;
	csr_cs_user_response_e response;
	bool isApp;
	time_t ts;
};

}
