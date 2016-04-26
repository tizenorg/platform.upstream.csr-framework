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
 * @file        askuser.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Ask user and get response to handle screening result
 */
#include "ui/askuser.h"

#include <new>

namespace Csr {
namespace Ui {

AskUser::AskUser() : m_address("/tmp/." SERVICE_NAME "-popup.socket")
{
	m_dispatcher.reset(new Dispatcher(m_address));
}

AskUser::~AskUser()
{
}

csr_cs_user_response_e AskUser::cs(const CommandId &cid, const std::string &message,
					   const CsDetected &d) const
{
	return static_cast<csr_cs_user_response_e>(m_dispatcher->methodCall<int>(
			static_cast<int>(cid), message, d));
}

csr_wp_user_response_e AskUser::wp(const CommandId &cid, const std::string &message,
					   const UrlItem &item) const
{
	return static_cast<csr_wp_user_response_e>(m_dispatcher->methodCall<int>(
			static_cast<int>(cid), message, item));
}

} // namespace Ui
} // namespace Csr
