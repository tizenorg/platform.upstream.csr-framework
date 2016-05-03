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
 * @file        access-control.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/access-control.h"

#include <memory>
#include <cstring>
#include <sys/smack.h>

#include "common/audit/logger.h"
#include "common/exception.h"
#include "service/fs-utils.h"

namespace Csr {

namespace {

bool hasPermToWriteDac(const Credential &cred, const std::string &filepath)
{
	auto statptr = getStat(filepath);
	return (cred.uid == statptr->st_uid && (statptr->st_mode & S_IWUSR)) ||
		   (cred.gid == statptr->st_gid && (statptr->st_mode & S_IWGRP)) ||
		   (statptr->st_mode & S_IWOTH);
}

bool hasPermToWriteMac(const Credential &cred, const std::string &filepath)
{
	char *label = nullptr;
	int ret = smack_getlabel(filepath.c_str(), &label, SMACK_LABEL_ACCESS);
	if (ret != 0)
		ThrowExc(InternalError, "get smack label failed from file: " << filepath <<
				 " ret: " << ret);

	std::unique_ptr<char, void(*)(void *)> labelptr(label, ::free);

	ret = smack_have_access(cred.label.c_str(), label, "w");
	if (ret == -1)
		ThrowExc(InternalError, "smack_have_access err on file: " << filepath <<
				 " errno: " << errno);

	return ret == 1;
}

} // namespace anonymous

bool hasPermToRemove(const Credential &cred, const std::string &filepath)
{
	auto parent = filepath.substr(0, filepath.find_last_of('/'));

	return hasPermToWriteDac(cred, parent) && hasPermToWriteMac(cred, parent);

}

}
