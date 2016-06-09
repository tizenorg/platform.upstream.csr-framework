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
 * @file        test-resource.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Test resoure name and paths
 */
#include "test-resource.h"

#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "test-common.h"

namespace {

std::string s_wgtAppRootCache;
std::string s_wgtMalFileCache;
std::string s_tpkAppRootCache;
std::string s_tpkMalFileCache;

#ifdef PLATFORM_VERSION_3
std::string getUsername(void)
{
	struct passwd pwd;
	struct passwd *result = nullptr;

	auto bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	bufsize = (bufsize == -1) ? 16384 : bufsize;

	std::vector<char> buf(bufsize, 0);

	::getpwuid_r(::getuid(), &pwd, buf.data(), buf.size(), &result);
	if (result == nullptr)
		throw std::logic_error("Failed to getpwuid_r with errno: " + errno);

	return std::string(pwd.pw_name);
}
#endif

} // namespace anonymous

const std::string &TEST_WGT_APP_ROOT(void)
{
	if (s_wgtAppRootCache.empty())
#ifdef PLATFORM_VERSION_3
		s_wgtAppRootCache = "/home/" + ::getUsername() + "/apps_rw/" TEST_WGT_PKG_ID;
#else
		s_wgtAppRootCache = TEST_DIR_APPS "/" TEST_WGT_PKG_ID;
#endif

	return s_wgtAppRootCache;
}

const std::string &TEST_WGT_MAL_FILE(void)
{
	if (s_wgtMalFileCache.empty())
		s_wgtMalFileCache = TEST_WGT_APP_ROOT() + "/res/wgt/data/malicious.txt";

	return s_wgtMalFileCache;
}

const std::string &TEST_TPK_APP_ROOT(void)
{
	if (s_tpkAppRootCache.empty())
#ifdef PLATFORM_VERSION_3
		s_tpkAppRootCache = "/home/" + ::getUsername() + "/apps_rw/" TEST_TPK_PKG_ID;
#else
		s_tpkAppRootCache = TEST_DIR_APPS "/" TEST_TPK_PKG_ID;
#endif

	return s_tpkAppRootCache;
}

const std::string &TEST_TPK_MAL_FILE(void)
{
	if (s_tpkMalFileCache.empty())
		s_tpkMalFileCache = TEST_TPK_APP_ROOT() + "/shared/res/malicious.txt";

	return s_tpkMalFileCache;
}
