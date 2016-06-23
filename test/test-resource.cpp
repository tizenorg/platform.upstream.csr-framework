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

std::string s_testDirMedia;
std::string s_testDirApps;

std::string s_testFileMedia;
std::string s_testFileInDirMalwares;

std::string s_wgtAppRoot;
std::string s_wgtMalFile;
std::string s_tpkAppRoot;
std::string s_tpkMalFile;
std::string s_fakeAppRoot;
std::string s_fakeAppFile;

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

const char *TEST_DIR_MEDIA(void)
{
	if (s_testDirMedia.empty())
#ifdef PLATFORM_VERSION_3
		s_testDirMedia = "/home/" + ::getUsername() + "/content";
#else
		s_testDirMedia = "/opt/usr/media";
#endif

	return s_testDirMedia.c_str();
}

const char *TEST_DIR_APPS(void)
{
	if (s_testDirApps.empty())
#ifdef PLATFORM_VERSION_3
		s_testDirApps = "/home/" + ::getUsername() + "/apps_rw";
#else
		s_testDirApps = "/opt/usr/apps";
#endif

	return s_testDirApps.c_str();
}

const char *TEST_FILE_MEDIA(void)
{
	if (s_testFileMedia.empty())
		s_testFileMedia = std::string(TEST_DIR_MEDIA()) + "/test_malware_file";

	return s_testFileMedia.c_str();
}

const char *TEST_WGT_APP_ROOT(void)
{
	if (s_wgtAppRoot.empty())
		s_wgtAppRoot = std::string(TEST_DIR_APPS()) + "/" TEST_WGT_PKG_ID;

	return s_wgtAppRoot.c_str();
}

const char *TEST_WGT_MAL_FILE(void)
{
	if (s_wgtMalFile.empty())
		s_wgtMalFile = std::string(TEST_WGT_APP_ROOT()) + "/res/wgt/data/eicar.txt";

	return s_wgtMalFile.c_str();
}

const char *TEST_TPK_APP_ROOT(void)
{
	if (s_tpkAppRoot.empty())
		s_tpkAppRoot = std::string(TEST_DIR_APPS()) + "/" TEST_TPK_PKG_ID;

	return s_tpkAppRoot.c_str();
}

const char *TEST_TPK_MAL_FILE(void)
{
	if (s_tpkMalFile.empty())
		s_tpkMalFile = std::string(TEST_TPK_APP_ROOT()) + "/shared/res/eicar.txt";

	return s_tpkMalFile.c_str();
}

const char *TEST_FAKE_APP_ROOT(void)
{
	if (s_fakeAppRoot.empty())
		s_fakeAppRoot = std::string(TEST_DIR_APPS()) + "/fake_app";

	return s_fakeAppRoot.c_str();
}

const char *TEST_FAKE_APP_FILE(void)
{
	if (s_fakeAppFile.empty())
		s_fakeAppFile = std::string(TEST_FAKE_APP_ROOT()) + "/malicious.txt";

	return s_fakeAppFile.c_str();
}
