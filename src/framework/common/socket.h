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
 * @file        socket.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>

#include "common/macros.h"
#include "common/types.h"
#include "common/socket-descriptor.h"

namespace Csr {

class API Socket {
public:
	enum class Type : int {
		SERVER = 0x01,
		CLIENT = 0x02
	};

	static Socket create(SockId, Socket::Type);

	Socket(const Socket &) = delete;
	Socket &operator=(const Socket &) = delete;

	Socket(Socket &&) noexcept;
	Socket &operator=(Socket &&) noexcept;

	virtual ~Socket();

	Socket accept(void) const;

	SockId getSockId(void) const noexcept;
	int getFd(void) const noexcept;

	RawBuffer read(void) const;
	void write(const RawBuffer &data) const;

private:
	static Socket connect(SockId);
	Socket(SockId sockId, int fd);
	Socket(SockId sockId);

	SockId m_sockId;
	int m_fd;
};

}
