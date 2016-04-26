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
 * @file        server-service.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/server-service.h"

#include "common/audit/logger.h"

namespace Csr {

ServerService::ServerService(const std::string &address) :
	Service(address),
	m_workqueue(2, 10)
{
}

ServerService::~ServerService()
{
}

void ServerService::onMessageProcess(const ConnShPtr &connection)
{
	DEBUG("let's dispatch it to worker threads.");

	auto process = [&](RawBuffer & buffer) {
		connection->send(m_logic.dispatch(buffer));
	};

	m_workqueue.submit(std::bind(process, connection->receive()));
}

}
