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
 * @file        access-control-cynara.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       access control with cynara backend
 */
#include "service/access-control.h"

#include <mutex>

#include <cynara-client.h>

#include "service/exception.h"

namespace Csr {

namespace {

class Cynara {
public:
	Cynara() : m_isInitialized(false), m_cynara(nullptr) {}

	~Cynara()
	{
		std::lock_guard<std::mutex> l(this->m_mutex);

		if (!this->m_isInitialized)
			return;

		cynara_finish(this->m_cynara);

		this->m_isInitialized = false;
	}

	void initialize()
	{
		std::lock_guard<std::mutex> l(this->m_mutex);

		if (this->m_isInitialized)
			return;

		auto ret = cynara_initialize(&(this->m_cynara), nullptr);

		if (ret != CYNARA_API_SUCCESS)
			ThrowExc(InternalError, "Failed to cynara_async_initialize. ret: " << ret);

		this->m_isInitialized = true;
	}

	bool request(const std::string &user, const std::string &client,
				 const std::string &session, const std::string &privilege)
	{
		auto ret = cynara_check(this->m_cynara, client.c_str(), session.c_str(),
								user.c_str(), privilege.c_str());

		switch (ret) {
		case CYNARA_API_ACCESS_ALLOWED: return true;
		case CYNARA_API_ACCESS_DENIED:  return false;
		default:
			ThrowExc(InternalError, "cynara_check failed. ret: " << ret);
		}
	}

private:
	bool m_isInitialized;
	cynara *m_cynara;
	std::mutex m_mutex;

};

Cynara g_cynara;

} // namespace anonymous

bool hasPermission(const ConnShPtr &conn)
{
	return hasPermission(conn, conn->getSockId());
}

bool hasPermission(const ConnShPtr &conn, SockId sockId)
{
	const auto &c = conn->getCredential();
	const auto &d = getSockDesc(sockId);

	g_cynara.initialize();

	return g_cynara.request(c.user, c.client, std::to_string(conn->getFd()), d.privilege);
}

}
