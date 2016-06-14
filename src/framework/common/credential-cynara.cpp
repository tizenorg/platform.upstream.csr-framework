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
 * @file        credential-cynara.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Get credential by cynara backend
 */
#include "common/credential.h"

#include <string>

#include <cynara-creds-socket.h>

#include "common/exception.h"

namespace Csr {

Credential::Credential(const std::string &_user, const std::string &_client) :
	user(_user), client(_client) {}

Credential::Credential(uid_t, gid_t, const std::string &)
{
	ThrowExc(CSR_ERROR_SERVER, "Invalid credential ctor called which is for smack backend.");
}

std::unique_ptr<Credential> Credential::get(int sockfd)
{
	char *userptr = nullptr;
	auto ret = cynara_creds_socket_get_user(sockfd, USER_METHOD_DEFAULT, &userptr);

	if (ret != CYNARA_API_SUCCESS)
		ThrowExc(CSR_ERROR_SERVER, "Failed to cynara_creds_socket_get_user. ret: " << ret);

	char *clientptr = nullptr;
	ret = cynara_creds_socket_get_client(sockfd, CLIENT_METHOD_DEFAULT, &clientptr);

	if (ret != CYNARA_API_SUCCESS) {
		free(userptr);
		ThrowExc(CSR_ERROR_SERVER, "Failed to cynara_creds_socket_get_client. ret: " << ret);
	}

	std::unique_ptr<Credential> cred(new Credential(userptr, clientptr));

	free(userptr);
	free(clientptr);

	return cred;
}

}
