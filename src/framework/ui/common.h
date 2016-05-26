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
 * @file        common.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Common data structs for popup-service and csr-server
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "common/serialization.h"

#include <csr-content-screening-types.h>
#include <csr-web-protection-types.h>

namespace Csr {
namespace Ui {

enum class CommandId : int {
	CS_PROMPT_DATA = 0x01,
	CS_PROMPT_APP  = 0x02,
	CS_PROMPT_FILE = 0x03,
	CS_NOTIFY_DATA = 0x04,
	CS_NOTIFY_APP  = 0x05,
	CS_NOTIFY_FILE = 0x06,

	WP_PROMPT      = 0x10,
	WP_NOTIFY      = 0x11
};

struct UrlItem : public ISerializable {
	UrlItem() {}
	virtual ~UrlItem() {}
	UrlItem(IStream &);
	virtual void Serialize(IStream &) const override;

	csr_wp_risk_level_e risk;
	std::string url;
};

}
}
