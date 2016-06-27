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
 * @file        row.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       db row
 */
#pragma once

#include <memory>

#include "common/cs-detected.h"

namespace Csr {
namespace Db {

class Row;
using RowShPtr = std::shared_ptr<Row>;
using RowShPtrs = std::vector<RowShPtr>;

struct Row : public Csr::CsDetected {
	std::string fileInAppPath; // for case of file in app
	bool isIgnored;

	Row() : isIgnored(false) {}
	virtual ~Row() {}
};

} // namespace Db
} // namespace Csr
