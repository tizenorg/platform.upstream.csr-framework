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
 * @file        askuser.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Ask user and get response to handle screening result
 */
#pragma once

#include <string>
#include <memory>

#include "ui/common.h"
#include "common/dispatcher.h"
#include "common/cs-detected.h"

#include "csr/content-screening-types.h"
#include "csr/web-protection-types.h"

namespace Csr {
namespace Ui {

class AskUser {
public:
	AskUser();
	virtual ~AskUser();

	AskUser(const AskUser &) = delete;
	AskUser &operator=(const AskUser &) = delete;

	// ui command id, message, data for filling up popup window
	csr_cs_user_response_e cs(const CommandId &, const std::string &,
							  const CsDetected &) const;
	csr_wp_user_response_e wp(const CommandId &, const std::string &,
							  const UrlItem &) const;

private:
	std::string m_address;
	std::unique_ptr<Dispatcher> m_dispatcher;
};

}
}
