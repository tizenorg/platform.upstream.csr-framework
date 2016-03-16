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
#include <memory>
#include <functional>

#include "connection.h"
#include "mainloop.h"
#include "logic.h"

namespace Csr {

using ConnCallback = std::function<void(const ConnShPtr &)>;

class Service {
public:
	Service(const std::string &address);
	virtual ~Service();

	void start(void);
	void stop(void);

	/* ConnCallback param should throw exception to handle error */
	void setNewConnectionCallback(const ConnCallback &);
	void setCloseConnectionCallback(const ConnCallback &);

private:
	void onMessageProcess(const ConnShPtr &);

	ConnCallback m_onNewConnection;
	ConnCallback m_onCloseConnection;

	std::unordered_map<int, ConnShPtr> m_connectionRegistry;
	Mainloop m_loop;
	std::string m_address;

	Logic m_logic;
};

}
