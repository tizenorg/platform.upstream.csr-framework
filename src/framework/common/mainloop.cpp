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

#include <system_error>
#include <sys/epoll.h>
#include <unistd.h>

#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {

Mainloop::Mainloop() :
	m_isTimedOut(false),
	m_pollfd(::epoll_create1(EPOLL_CLOEXEC))
{
	if (this->m_pollfd == -1)
		throw std::system_error(
			std::error_code(errno, std::generic_category()),
			"Failed to epoll_create1");
}

Mainloop::~Mainloop()
{
	if (!this->m_isTimedOut && !this->m_callbacks.empty())
		ERROR("mainloop registered callbacks should be empty except timed out case");

	::close(m_pollfd);
}

void Mainloop::run(int timeout)
{
	this->m_isTimedOut = false;

	while (!this->m_isTimedOut) {
		this->dispatch(timeout);
	}

	DEBUG("Mainloop run stopped");
}

void Mainloop::addEventSource(int fd, uint32_t event, Callback &&callback)
{
	if (this->m_callbacks.count(fd) != 0)
		ThrowExc(CSR_ERROR_SERVER, "event source on fd[" << fd << "] already added!");

	DEBUG("Add event[" << event << "] source on fd[" << fd << "]");

	epoll_event e;

	e.events = event;
	e.data.fd = fd;

	if (::epoll_ctl(m_pollfd, EPOLL_CTL_ADD, fd, &e) == -1)
		throw std::system_error(
			std::error_code(errno, std::generic_category()),
			"epoll_ctl failed to EPOLL_CTL_ADD.");

	this->m_callbacks[fd] = std::move(callback);
}

void Mainloop::removeEventSource(int fd)
{
	if (this->m_callbacks.count(fd) == 0)
		ThrowExc(CSR_ERROR_SERVER, "event source on fd[" << fd << "] isn't added at all");

	DEBUG("Remove event source on fd[" << fd << "]");

	{
		this->m_callbacks.erase(fd);

		if (::epoll_ctl(m_pollfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
			if (errno == ENOENT)
				ThrowExc(CSR_ERROR_SERVER, "Tried to delete epoll item which wasn't added");
			else
				throw std::system_error(
					std::error_code(errno, std::generic_category()),
					"epoll_ctl failed to EPOLL_CTL_DEL.");
		}
	}
}

void Mainloop::dispatch(int timeout)
{
	int nfds = -1;
	epoll_event event[MAX_EPOLL_EVENTS];

	DEBUG("Mainloop dispatched with timeout: " << timeout);

	do {
		nfds = ::epoll_wait(this->m_pollfd, event, MAX_EPOLL_EVENTS,
							((timeout < 0) ? -1 : (timeout * 1000)));
	} while ((nfds == -1) && (errno == EINTR));

	if (nfds < 0)
		throw std::system_error(
			std::error_code(errno, std::generic_category()),
			"epoll_wait failed!");

	if (nfds == 0) {
		DEBUG("Mainloop timed out!");
		if (this->m_isIdle && !this->m_isIdle()) {
			INFO("Mainloop timed out but there's running task on upper layer. "
				 "Re-dispatch.");
			this->m_isTimedOut = false;
		} else {
			INFO("Mainloop timed out! stop the loop!");
			this->m_isTimedOut = true;
		}

		return;
	}

	for (int i = 0; i < nfds; i++) {
		int fd = event[i].data.fd;

		if (this->m_callbacks.count(fd) == 0)
			ThrowExc(CSR_ERROR_SERVER, "event in on fd[" << fd <<
					 "] but associated callback isn't exist!");

		if (event[i].events & (EPOLLHUP | EPOLLRDHUP)) {
			INFO("peer connection closed on fd[" << fd << "]");
			event[i].events &= ~EPOLLIN;
		}

		DEBUG("event[" << event[i].events << "] polled on fd[" << fd << "]");

		this->m_callbacks[fd](event[i].events);
	}
}

void Mainloop::setIdleChecker(std::function<bool()> &&idleChecker)
{
	this->m_isIdle = std::move(idleChecker);
}

}
