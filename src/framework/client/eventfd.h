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
 * @file        eventfd.h
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <sys/eventfd.h>

namespace Csr {
namespace Client {

class EventFd {
public:
	EventFd(unsigned int initval = 0, int flags = EFD_SEMAPHORE | EFD_CLOEXEC);
	~EventFd();

	EventFd(const EventFd &) = delete;
	EventFd &operator=(const EventFd &) = delete;

	void send();
	void receive();

	inline int getFd() const noexcept
	{
		return this->m_fd;
	}

private:
	int m_fd;
};

}
}
