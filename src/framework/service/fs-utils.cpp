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
 * @file        fs-utils.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/fs-utils.h"

#include <vector>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <csr-error.h>

#include "common/exception.h"
#include "common/audit/logger.h"

namespace Csr {

uid_t getUid(const std::string &username)
{
	auto bufsize = ::sysconf(_SC_GETPW_R_SIZE_MAX);
	bufsize = (bufsize == -1) ? 16384 : bufsize;

	std::vector<char> buf(bufsize, 0);

	struct passwd pwd;
	struct passwd *result = nullptr;

	auto ret = ::getpwnam_r(username.c_str(), &pwd, buf.data(), buf.size(), &result);

	if (result == nullptr) {
		if (ret == 0)
			ThrowExc(CSR_ERROR_SERVER, "Uid not found by username: " << username);
		else
			ThrowExc(CSR_ERROR_SERVER, "Failed to getpwnam_r with errno: " << errno);
	}

	return pwd.pw_uid;
}

std::unique_ptr<struct stat> getStat(const std::string &target)
{
	std::unique_ptr<struct stat> statptr(new struct stat);
	::memset(statptr.get(), 0x00, sizeof(struct stat));

	if (::stat(target.c_str(), statptr.get()) != 0) {
		const int err = errno;

		if (err == ENOENT)
			WARN("target not exist: " << target);
		else if (err == EACCES)
			WARN("no permission to read path: " << target);
		else
			ERROR("stat() failed on target: " << target << " errno: " << err);

		return nullptr;
	}

	// if no permission to read, return nullptr
	if (::access(target.c_str(), R_OK) != 0)
		return nullptr;
	else
		return statptr;
}

}
