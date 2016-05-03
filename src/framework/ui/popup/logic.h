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
 * @file        logic.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Logic for popup service
 */
#pragma once

#include <string>

#include "common/types.h"
#include "common/cs-detected.h"
#include "ui/common.h"

namespace Csr {
namespace Ui {

class Logic {
public:
	Logic();
	virtual ~Logic();

	RawBuffer csPromptData(const std::string &, const CsDetected &) const;
	RawBuffer csPromptApp(const std::string &, const CsDetected &) const;
	RawBuffer csPromptFile(const std::string &, const CsDetected &) const;
	RawBuffer csNotifyData(const std::string &, const CsDetected &) const;
	RawBuffer csNotifyApp(const std::string &, const CsDetected &) const;
	RawBuffer csNotifyFile(const std::string &, const CsDetected &) const;

	RawBuffer wpPrompt(const std::string &, const UrlItem &) const;
	RawBuffer wpNotify(const std::string &, const UrlItem &) const;
};

}
}
