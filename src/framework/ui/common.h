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

#include "csr/content-screening-types.h"
#include "csr/web-protection-types.h"

namespace Csr {
namespace Ui {

enum class CommandId : int {
	FILE_SINGLE   = 0x01,
	FILE_MULTIPLE = 0x02,
	URL_SINGLE    = 0x03,
	URL_MULTIPLE  = 0x04
};

enum class Response : int {
	/* For url access on web protection */
	ALLOW   = 0x01,
	DENY    = 0x02,
	CONFIRM = 0x03,

	/* For content screening detected item handling */
	REMOVE  = 0x04,
	IGNORE  = 0x05,
	SKIP    = 0x06
};

struct FileItem : public ISerializable {
	FileItem() {}
	virtual ~FileItem() {}
	FileItem(IStream &);
	virtual void Serialize(IStream &) const override;

	csr_cs_severity_level_e severity;
	csr_cs_threat_type_e threat;
	std::string filepath;
};

struct UrlItem : public ISerializable {
	UrlItem() {}
	virtual ~UrlItem() {}
	UrlItem(IStream &);
	virtual void Serialize(IStream &) const override;

	csr_wp_risk_level_e risk;
	std::string url;
};

using FileItems = std::vector<FileItem>;
using UrlItems = std::vector<UrlItem>;
using ResponseMap = std::unordered_map<std::string, int>;

}
}
