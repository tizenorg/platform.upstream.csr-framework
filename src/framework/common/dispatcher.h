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

#include "common/connection.h"
#include "common/message-buffer.h"

namespace Csr {

class Dispatcher {
public:
	Dispatcher(const std::string &path);
	virtual ~Dispatcher();

	Dispatcher(const Dispatcher &) = delete;
	Dispatcher &operator=(const Dispatcher &) = delete;
	Dispatcher(Dispatcher &&) = delete;
	Dispatcher &operator=(Dispatcher &&) = delete;

	template<typename Type, typename ...Args>
	Type methodCall(Args &&...args);

private:
	bool isConnected(void);
	void connect(void);
	void disconnect(void);

	std::string m_address;
	ConnShPtr m_connection;
};

template<typename Type, typename ...Args>
Type Dispatcher::methodCall(Args &&...args)
{
	if (!isConnected())
		connect();

	m_connection->send(MessageBuffer::Serialize(std::forward<Args>(args)...).pop());

	auto reply = m_connection->receive();
	MessageBuffer msg;
	msg.push(reply);

	Type response;
	msg.Deserialize(response);

	return response;
}

}
