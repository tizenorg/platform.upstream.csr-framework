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
 * @file        eventfd.cpp
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 * @version     1.0
 * @brief
 */
#include "client/eventfd.h"

#include <sys/types.h>
#include <unistd.h>
#include <cstdint>

#include "common/exception.h"

namespace Csr {
namespace Client {

EventFd::EventFd(unsigned int initval, int flags) :
	m_fd(::eventfd(initval, flags))
{
	if (this->m_fd == -1)
		ThrowExc(CSR_ERROR_SERVER, "Eventfd from constructor is failed!");
}

EventFd::~EventFd()
{
	if (this->m_fd != -1)
		::close(this->m_fd);
}

void EventFd::send()
{
	const std::uint64_t val = 1;
	if (::write(this->m_fd, &val, sizeof(val)) == -1)
		ThrowExc(CSR_ERROR_SOCKET, "EventFd send to fd[" << this->m_fd << "] is failed!");
}

void EventFd::receive()
{
	std::uint64_t val;
	if (::read(this->m_fd, &val, sizeof(val)) == -1)
		ThrowExc(CSR_ERROR_SOCKET, "EventFd receive from fd[" << this->m_fd << "] is failed!");
}

}
}
