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
 * @file        dispatcher.h
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>

#include "common/macros.h"
#include "common/connection.h"
#include "common/binary-queue.h"

namespace Csr {

class API Dispatcher {
public:
	Dispatcher(SockId) noexcept;
	virtual ~Dispatcher() = default;

	Dispatcher(const Dispatcher &) = delete;
	Dispatcher &operator=(const Dispatcher &) = delete;
	Dispatcher(Dispatcher &&) = delete;
	Dispatcher &operator=(Dispatcher &&) = delete;

	template<typename Type, typename ...Args>
	Type methodCall(Args &&...args);

	template<typename ...Args>
	void methodPing(Args &&...args);

private:
	bool isConnected(void) const noexcept;
	void connect(void);

	SockId m_sockId;
	ConnShPtr m_connection;
};

template<typename Type, typename ...Args>
Type Dispatcher::methodCall(Args &&...args)
{
	if (!this->isConnected())
		this->connect();

	this->m_connection->send(BinaryQueue::Serialize(std::forward<Args>(args)...).pop());

	BinaryQueue q;
	q.push(this->m_connection->receive());

	Type response;
	q.Deserialize(response);

	return response;
}

template<typename ...Args>
void Dispatcher::methodPing(Args &&...args)
{
	if (!this->isConnected())
		this->connect();

	this->m_connection->send(BinaryQueue::Serialize(std::forward<Args>(args)...).pop());
}

}
