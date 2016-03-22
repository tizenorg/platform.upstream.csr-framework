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

#include <sys/types.h>
#include <sys/epoll.h>

#include "common/audit/logger.h"

namespace Csr {

Service::Service(const std::string &address) : m_address(address)
{
	DEBUG("Service constructed with address[" << address << "]");

	setNewConnectionCallback(nullptr);
	setCloseConnectionCallback(nullptr);
}

Service::~Service()
{
}

void Service::start(int timeout)
{
	INFO("Service start!");

	Socket socket(m_address);

	DEBUG("Get systemd socket[" << socket.getFd()
		<< "] with address[" << m_address << "]");

	m_loop.addEventSource(socket.getFd(), EPOLLIN | EPOLLHUP | EPOLLRDHUP,
		[&](uint32_t event) {
			if (event != EPOLLIN)
				return;

			m_onNewConnection(std::make_shared<Connection>(socket.accept()));
		});

	m_loop.run(timeout);
}

void Service::stop()
{
	INFO("Service stop!");
}

void Service::setNewConnectionCallback(const ConnCallback &/*callback*/)
{
	/* TODO: scoped-lock */
	m_onNewConnection = [&](const ConnShPtr &connection) {
		if (!connection)
			throw std::logic_error("onNewConnection called but ConnShPtr is nullptr.");

		int fd = connection->getFd();

		INFO("welcome! accepted client socket fd[" << fd << "]");

/*
		// TODO: disable temporarily
		if (callback)
			callback(connection);
*/

		m_loop.addEventSource(fd, EPOLLIN | EPOLLHUP | EPOLLRDHUP,
			[&, fd](uint32_t event) {
				DEBUG("read event comes in to fd[" << fd << "]");

				if (m_connectionRegistry.count(fd) == 0)
					throw std::logic_error(FORMAT("get event on fd[" << fd
						<< "] but no associated connection exist"));

				auto &conn = m_connectionRegistry[fd];

				if (event & (EPOLLHUP | EPOLLRDHUP)) {
					DEBUG("event of epoll hup. close connection on fd[" << fd << "]");
					m_onCloseConnection(conn);
					return;
				}

				DEBUG("Start message process on fd[" << fd << "]");

				onMessageProcess(conn);
			});

		m_connectionRegistry[fd] = connection;
	};
}

void Service::setCloseConnectionCallback(const ConnCallback &/*callback*/)
{
	/* TODO: scoped-lock */
	m_onCloseConnection = [&](const ConnShPtr &connection) {
		if (!connection)
			throw std::logic_error("no connection to close");

		int fd = connection->getFd();

		if (m_connectionRegistry.count(fd) == 0)
			throw std::logic_error(FORMAT("no connection in registry to remove "
				"associated to fd[" << fd << "]"));

		INFO("good-bye! close socket fd[" << fd << "]");

		m_loop.removeEventSource(fd);
		m_connectionRegistry.erase(fd); /* scoped-lock needed? */

/*
		// TODO: disable temporarily
		if (callback)
			callback(connection);
*/
	};
}

}
