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
 * @file        service.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>
#include <functional>
#include <set>
#include <mutex>

#include "common/macros.h"
#include "common/connection.h"
#include "common/mainloop.h"
#include "common/socket-descriptor.h"

namespace Csr {

class API Service {
public:
	Service() noexcept;
	virtual ~Service();

	Service(const Service &) = delete;
	Service &operator=(const Service &) = delete;
	Service(Service &&) = delete;
	Service &operator=(Service &&) = delete;

	virtual void add(const SockId &) final;
	virtual void start(int timeout) final;

protected:
	void setIdleChecker(std::function<bool()> &&idleChecker);

private:
	virtual void onMessageProcess(const ConnShPtr &) = 0;

	void onNewConnection(ConnShPtr &&);
	void onCloseConnection(const ConnShPtr &);

	mutable std::mutex m_crMtx;
	std::unordered_map<int, ConnShPtr> m_connectionRegistry;
	Mainloop m_loop;

	std::set<SockId> m_sockIds;
};

}
