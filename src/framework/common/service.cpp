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
 * @file        service.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "common/service.h"

#include <utility>
#include <sys/types.h>
#include <sys/epoll.h>

#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {

Service::Service()
{
	this->setNewConnectionCallback(nullptr);
	this->setCloseConnectionCallback(nullptr);

	// set default idle checker which returns always true
	this->m_loop.setIdleChecker([]() { return true; });
}

void Service::setIdleChecker(std::function<bool()> &&idleChecker)
{
	this->m_loop.setIdleChecker(std::move(idleChecker));
}

void Service::add(const SockId &id)
{
	this->m_sockIds.insert(id);
}

void Service::start(int timeout)
{
	INFO("Service start!");

	for (const auto &id : this->m_sockIds) {
		auto socket = std::make_shared<Socket>(Socket::create(id, Socket::Type::SERVER));

		DEBUG("Get systemd socket[" << socket->getFd() <<
			  "] for sock id: " << static_cast<int>(id));

		this->m_loop.addEventSource(socket->getFd(), EPOLLIN | EPOLLHUP | EPOLLRDHUP,
		[this, socket](uint32_t event) {
			if (event != EPOLLIN)
				return;

			this->m_onNewConnection(std::make_shared<Connection>(socket->accept()));
		});
	}

	this->m_loop.run(timeout);
}

void Service::setNewConnectionCallback(const ConnCallback &callback)
{
	this->m_onNewConnection = [this, &callback](const ConnShPtr &connection) {
		if (!connection)
			ThrowExc(CSR_ERROR_SERVER, "onNewConnection called but ConnShPtr is nullptr.");

		int fd = connection->getFd();

		INFO("welcome! accepted client socket fd[" << fd << "]");

		if (callback)
			callback(connection);

		this->m_loop.addEventSource(fd, EPOLLIN | EPOLLHUP | EPOLLRDHUP,
		[&, fd](uint32_t event) {
			DEBUG("read event comes in to fd[" << fd << "]");

			if (this->m_connectionRegistry.count(fd) == 0)
				ThrowExc(CSR_ERROR_SERVER, "get event on fd[" << fd <<
						 "] but no associated connection exist");

			auto &conn = this->m_connectionRegistry[fd];

			if (event & (EPOLLHUP | EPOLLRDHUP)) {
				DEBUG("event of epoll hup. close connection on fd[" << fd << "]");
				this->m_onCloseConnection(conn);
				return;
			}

			DEBUG("Start message process on fd[" << fd << "]");

			onMessageProcess(conn);
		});

		this->m_connectionRegistry[fd] = connection;
	};
}

void Service::setCloseConnectionCallback(const ConnCallback &callback)
{
	this->m_onCloseConnection = [this, &callback](const ConnShPtr &connection) {
		if (!connection)
			ThrowExc(CSR_ERROR_SERVER, "no connection to close");

		int fd = connection->getFd();

		if (this->m_connectionRegistry.count(fd) == 0)
			ThrowExc(CSR_ERROR_SERVER, "no connection in registry to remove "
					 "associated to fd[" << fd << "]");

		INFO("good-bye! close socket fd[" << fd << "]");

		this->m_loop.removeEventSource(fd);
		this->m_connectionRegistry.erase(fd);

		if (callback)
			callback(connection);
	};
}

}
