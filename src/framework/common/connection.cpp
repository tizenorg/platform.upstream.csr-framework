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
 * @file        connection.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "common/connection.h"

#include <utility>

namespace Csr {

Connection::Connection(Socket &&socket) noexcept :
	m_socket(std::move(socket))
{
}

Connection::~Connection() noexcept
{
}

Connection::Connection(Connection &&other) noexcept :
	m_socket(std::move(other.m_socket))
{
}

Connection &Connection::operator=(Connection &&other) noexcept
{
	if (this == &other)
		return *this;

	this->m_socket = std::move(other.m_socket);
	return *this;
}

void Connection::send(const RawBuffer &buf) const
{
	std::lock_guard<std::mutex> lock(this->m_mSend);
	this->m_socket.write(buf);
}

RawBuffer Connection::receive() const
{
	std::lock_guard<std::mutex> lock(this->m_mRecv);
	return this->m_socket.read();
}

SockId Connection::getSockId() const noexcept
{
	return this->m_socket.getSockId();
}

int Connection::getFd() const noexcept
{
	return this->m_socket.getFd();
}

const Credential &Connection::getCredential()
{
	if (this->m_cred)
		return *this->m_cred;

	this->m_cred = Credential::get(getFd());

	return *this->m_cred;
}

}
