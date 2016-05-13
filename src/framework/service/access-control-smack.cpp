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
 * @file        access-control-smack.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       access control with smack backend
 */
#include "service/access-control.h"

#include <sys/smack.h>

#include "common/exception.h"

namespace Csr {

bool hasPermission(const ConnShPtr &conn)
{
	return hasPermission(conn, conn->getSockId());
}

bool hasPermission(const ConnShPtr &conn, SockId sockId)
{
	const auto &cred = conn->getCredential();
	const auto &sockDesc = getSockDesc(sockId);

	auto ret = smack_have_access(cred.label.c_str(), sockDesc.label.c_str(), "w");
	if (ret < 0)
		ThrowExc(InternalError, "smack_have_access failed.");

	return ret == 1;
}

}
