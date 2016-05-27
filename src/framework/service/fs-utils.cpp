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

#include <cstring>
#include <cerrno>

#include "common/audit/logger.h"

namespace Csr {

std::unique_ptr<struct stat> getStat(const std::string &target)
{
	std::unique_ptr<struct stat> statptr(new struct stat);
	memset(statptr.get(), 0x00, sizeof(struct stat));

	if (stat(target.c_str(), statptr.get()) != 0) {
		const int err = errno;

		if (err == ENOENT)
			WARN("target not exist: " << target);
		else if (err == EACCES)
			WARN("no permission to read target: " << target);
		else
			ERROR("stat() failed on target: " << target << " errno: " << err);

		return nullptr;
	}

	return statptr;
}

}
