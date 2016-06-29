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
 * @file        test-file-system.cpp
 * @author      Dongsun Lee(ds73.lee@samsung.com)
 * @version     1.0
 * @brief       filesystem class unit test
 */
#include <service/file-system.h>
#include <common/exception.h>

#include <string>
#include <iostream>
#include <climits>
#include <ctime>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <package-manager.h>
#include <pkgmgr-info.h>

#include <boost/test/unit_test.hpp>

#include "test-common.h"
#include "test-resource.h"

#define TEST_DIR_VISIT    TEST_DIR "/test_dir"
#define TEST_WRITE_FILE   TEST_DIR_VISIT "/testwritefile.txt"

using namespace Csr;

namespace {

void __assertFile(const File &file, const std::string &path,
				  const std::string &user,
				  const std::string &pkgId, const std::string &pkgPath, bool inApp)
{
	ASSERT_IF(file.getPath(), path);
	ASSERT_IF(file.getAppUser(), user);
	ASSERT_IF(file.getAppPkgId(), pkgId);
	ASSERT_IF(file.getName(), pkgPath);
	ASSERT_IF(file.isInApp(), inApp);
}

void __createFile(const std::string &path)
{
	if (::access(path.c_str(), R_OK | W_OK) == 0)
		return; // already exist

	int fd = creat(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	BOOST_REQUIRE_MESSAGE(fd > 0, "Failed to create file: " << path);
	close(fd);
}

void __removeFile(const std::string &path)
{
	remove(path.c_str());
}

void __writeFile(const std::string &path)
{
	auto fp = fopen(path.c_str(), "w");
	CHECK_IS_NOT_NULL(fp);
	char buffer[] = { 't', 'e', 's', 't' };
	fwrite(buffer , sizeof(char), sizeof(buffer), fp);
	fclose(fp);
}

}

BOOST_AUTO_TEST_SUITE(FILE_SYSTEM)

// File::create returns null when file doesn't exist.
// So these test case will restored with well-defined resources later
/*
BOOST_AUTO_TEST_CASE(check_in_app)
{
	std::string path1("/sdcard/text.txt");
	auto file1 = File::create(path1);
	__assertFile(*file1, path1, std::string(), std::string(), std::string(), false);

	std::string path2("/usr/apps1/testpkg/test.txt");
	auto file2 = File::create(path2);
	__assertFile(*file2, path2, std::string(), std::string(), std::string(), false);

	std::string path3("/opt/usr/apps1/testpkg/test.txt");
	auto file3 = File::create(path3);
	__assertFile(*file3, path3, std::string(), std::string(), std::string(), false);

	std::string path4("/sdcard1/apps/testpkg/test.txt");
	auto file4 = File::create(path4);
	__assertFile(*file4, path4, std::string(), std::string(), std::string(), false);

	std::string pkgid("testpkg");
	std::string appPath1("/usr/apps/" + pkgid);
	std::string appFilePath1(appPath1 + "/test.txt");
	auto app1 = File::create(appFilePath1);
	__assertFile(*app1, appFilePath1, std::string(), pkgid, appPath1, true);

	std::string appPath2("/opt/usr/apps/" + pkgid);
	std::string appFilePath2(appPath2 + "/test.txt");
	auto app2 = File::create(appFilePath2);
	__assertFile(*app2, appFilePath2, std::string(), pkgid, appPath2, true);

	std::string appPath3("/sdcard/apps/" + pkgid);
	std::string appFilePath3(appPath3 + "/test.txt");
	auto app3 = File::create(appFilePath3);
	__assertFile(*app3, appFilePath3, std::string(), pkgid, appPath3, true);

	std::string appPath4("/sdcard/app2sd/" + pkgid);
	std::string appFilePath4(appPath4 + "/test.txt");
	auto app4 = File::create(appFilePath4);
	__assertFile(*app4, appFilePath4, std::string(), pkgid, appPath4, true);
}
*/

BOOST_AUTO_TEST_CASE(remove_file)
{
	EXCEPTION_GUARD_START

	std::string fpath = "/tmp/test.txt";

	__createFile(fpath);

	auto file = File::create(fpath, nullptr);
	BOOST_REQUIRE_NO_THROW(file->remove());

	bool isRemoved = access(fpath.c_str(), F_OK) != 0 && errno == ENOENT;
	__removeFile(fpath);

	BOOST_REQUIRE_MESSAGE(isRemoved, "file remove done but file still exist...");

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(remove_app)
{
	EXCEPTION_GUARD_START

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	auto app = File::create(TEST_TPK_MAL_FILE(), nullptr);
	CHECK_IS_NOT_NULL(app);
	app->remove();

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(file_visitor_positive_existing)
{
	CHECK_IS_NOT_NULL(File::create(TEST_WRITE_FILE, nullptr));
}

BOOST_AUTO_TEST_CASE(file_visitor_positive_modified)
{
	std::string file = TEST_WRITE_FILE;

	auto beforeWrite = time(nullptr) - 1;

	__writeFile(file);

	auto afterWrite = time(nullptr) + 1;

	// before write time < modified time < after write time
	// if modifiedSince is same to modified time in file stat,
	// the file will be regarded as ''not modified'' no File::create returns null.

	CHECK_IS_NOT_NULL(File::createIfModified(file, nullptr, beforeWrite));
	CHECK_IS_NULL(File::createIfModified(file, nullptr, afterWrite));
}

BOOST_AUTO_TEST_CASE(file_visitor_negative_non_existing)
{
	BOOST_REQUIRE_THROW(File::create(TEST_DIR "/non_existing_file", nullptr), Csr::Exception);
}

BOOST_AUTO_TEST_CASE(directory_visitor_positive_existing)
{
	std::string dir(TEST_DIR_VISIT);

	int cnt = 0;

	// test for existing dir
	auto visitor = FsVisitor::create([&](const FilePtr &) {
		++cnt;
	}, dir, true);
	CHECK_IS_NOT_NULL(visitor);

	visitor->run();

	ASSERT_IF(cnt, 9);
}

BOOST_AUTO_TEST_CASE(directory_visitor_positive_modified)
{
	std::string dir(TEST_DIR_VISIT);
	std::string file(TEST_WRITE_FILE);

	auto beforeWrite = time(nullptr) - 1;

	__writeFile(file);

	int cnt = 0;

	auto visitor = FsVisitor::create([&](const FilePtr &) {
		++cnt;
	}, dir, true, beforeWrite);
	CHECK_IS_NOT_NULL(visitor);

	visitor->run();

	ASSERT_IF(cnt, 1);
}

BOOST_AUTO_TEST_CASE(app_directory_visitor_positive)
{
	auto visitor = FsVisitor::create([&](const FilePtr &file) {
		BOOST_MESSAGE("visit target name: " << file->getPath());
	}, TEST_DIR_APPS(), true);
	CHECK_IS_NOT_NULL(visitor);

	visitor->run();
}

BOOST_AUTO_TEST_SUITE_END()
