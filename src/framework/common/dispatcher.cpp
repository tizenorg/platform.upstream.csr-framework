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
 * @file        dispatcher.cpp
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 * @version     1.0
 * @brief
 */
#include "common/dispatcher.h"

#include <utility>

#include "common/socket.h"

namespace Csr {

Dispatcher::Dispatcher(SockId sockId) noexcept : m_sockId(sockId)
{
}

void Dispatcher::connect()
{
	std::lock_guard<std::mutex> l(this->m_connMutex);
	if (this->m_connection == nullptr)
		this->m_connection = std::make_shared<Connection>(
				Socket::create(this->m_sockId, Socket::Type::CLIENT));
}

} // namespace Csr
