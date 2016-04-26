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
 * @file        mainloop.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Mainloop of csr-server with epoll
 */
#include "common/mainloop.h"

#include <exception>
#include <stdexcept>
#include <system_error>
#include <sys/epoll.h>
#include <unistd.h>

#include "common/audit/logger.h"

namespace Csr {

Mainloop::Mainloop() :
	m_isTimedOut(false),
	m_pollfd(::epoll_create1(EPOLL_CLOEXEC))
{
	if (m_pollfd == -1)
		throw std::system_error(
			std::error_code(errno, std::generic_category()),
			"Failed to epoll_create1");
}

Mainloop::~Mainloop()
{
	if (!m_isTimedOut && !m_callbacks.empty())
		throw std::logic_error("mainloop registered callbacks should be empty "
							   "except timed out case");

	::close(m_pollfd);
}

void Mainloop::run(int timeout)
{
	m_isTimedOut = false;

	while (!m_isTimedOut) {
		dispatch(timeout);
	}

	DEBUG("Mainloop run stopped");
}

void Mainloop::addEventSource(int fd, uint32_t event, Callback &&callback)
{
	/* TODO: use scoped-lock to thread safe on member variables */
	if (m_callbacks.count(fd) != 0)
		throw std::logic_error(FORMAT("event source on fd[" << fd <<
									  "] already added!"));

	DEBUG("Add event[" << event << "] source on fd[" << fd << "]");

	epoll_event e;

	e.events = event;
	e.data.fd = fd;

	if (::epoll_ctl(m_pollfd, EPOLL_CTL_ADD, fd, &e) == -1)
		throw std::system_error(
			std::error_code(errno, std::generic_category()),
			"epoll_ctl failed to EPOLL_CTL_ADD.");

	m_callbacks[fd] = std::move(callback);
}

void Mainloop::removeEventSource(int fd)
{
	/* TODO: use scoped-lock to thread safe on member variables */
	if (m_callbacks.count(fd) == 0)
		throw std::logic_error(FORMAT("event source on fd[" << fd <<
									  "] isn't added at all"));

	DEBUG("Remove event source on fd[" << fd << "]");

	do {
		m_callbacks.erase(fd);

		if (::epoll_ctl(m_pollfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
			if (errno == ENOENT)
				throw std::logic_error("Tried to delete epoll item which wasn't added");
			else
				throw std::system_error(
					std::error_code(errno, std::generic_category()),
					"epoll_ctl failed to EPOLL_CTL_DEL.");
		}
	} while (false);
}

void Mainloop::dispatch(int timeout)
{
	int nfds = -1;
	epoll_event event[MAX_EPOLL_EVENTS];

	DEBUG("Mainloop dispatched with timeout: " << timeout);

	do {
		nfds = ::epoll_wait(m_pollfd, event, MAX_EPOLL_EVENTS, timeout);
	} while ((nfds == -1) && (errno == EINTR));

	if (nfds < 0)
		throw std::system_error(
			std::error_code(errno, std::generic_category()),
			"epoll_wait failed!");

	if (nfds == 0) {
		DEBUG("Mainloop timed out!");
		m_isTimedOut = true;
		return;
	}

	for (int i = 0; i < nfds; i++) {
		/* TODO: use scoped-lock to thread safe on member variables */
		int fd = event[i].data.fd;

		if (m_callbacks.count(fd) == 0)
			throw std::logic_error(FORMAT(
									   "event in on fd[" << fd << "] but associated callback isn't exist!"));

		if (event[i].events & (EPOLLHUP | EPOLLRDHUP)) {
			INFO("peer connection closed on fd[" << fd << "]");
			event[i].events &= ~EPOLLIN;
		}

		DEBUG("event[" << event[i].events << "] polled on fd[" << fd << "]");

		m_callbacks[fd](event[i].events);
	}
}

}
