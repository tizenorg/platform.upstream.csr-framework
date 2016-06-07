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
 * @file        connection.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <memory>
#include <mutex>

#include "common/macros.h"
#include "common/socket.h"
#include "common/types.h"
#include "common/credential.h"

namespace Csr {

class API Connection {
public:
	explicit Connection(Socket &&socket) noexcept;
	virtual ~Connection() = default;

	Connection(const Connection &) = delete;
	Connection &operator=(const Connection &) = delete;

	Connection(Connection &&) noexcept;
	Connection &operator=(Connection &&) noexcept;

	void send(const RawBuffer &) const;
	RawBuffer receive(void) const;

	SockId getSockId(void) const noexcept;
	int getFd(void) const noexcept;
	const Credential &getCredential();

private:
	Socket m_socket;
	mutable std::mutex m_mSend;
	mutable std::mutex m_mRecv;

	std::unique_ptr<Credential> m_cred;
};

using ConnShPtr = std::shared_ptr<Connection>;

}
