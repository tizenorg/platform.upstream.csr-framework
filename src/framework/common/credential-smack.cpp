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
 * @file        credential-smack.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "common/credential.h"

#include <vector>
#include <sys/socket.h>
#include <sys/smack.h>

#include "common/exception.h"

namespace Csr {

Credential::Credential(uid_t _uid, gid_t _gid, const std::string &_label) :
	uid(_uid), gid(_gid), label(_label) {}

Credential::Credential(const std::string &, const std::string &)
{
	ThrowExc(InternalError, "Invalid credential ctor called which is for cynara backend.");
}

std::unique_ptr<Credential> Credential::get(int sockfd)
{
	struct ucred cred;
	socklen_t lenCred = sizeof(ucred);

	if (getsockopt(sockfd, SOL_SOCKET, SO_PEERCRED, &cred, &lenCred) != 0)
		ThrowExc(InternalError, "getsockopt peercred failed. errno: " << errno);

	std::vector<char> label(SMACK_LABEL_LEN + 1, '0');
	socklen_t lenLabel = SMACK_LABEL_LEN;

	if (getsockopt(sockfd, SOL_SOCKET, SO_PEERSEC, label.data(), &lenLabel) != 0)
		ThrowExc(InternalError, "getsockopt peersec failed. errno: " << errno);

	return std::unique_ptr<Credential>(new Credential(cred.uid, cred.gid,
									   std::string(label.data(), lenLabel)));

}

}
