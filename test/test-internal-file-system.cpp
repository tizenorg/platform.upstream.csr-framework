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
 * @file        test-internal-file-system.cpp
 * @author      Dongsun Lee(ds73.lee@samsung.com)
 * @version     1.0
 * @brief       File system releated class test
 */
#include <service/file-system.h>

#include <string>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <boost/test/unit_test.hpp>
#include <unistd.h>
#include <time.h>
#include <glib.h>

#include <package-manager.h>
#include <pkgmgr-info.h>

#include "test-common.h"

#define TEST_APP_PKG      TEST_DIR "/maliciousapp.tpk"
#define TEST_DIR_VISIT    TEST_DIR "/test_dir"
#define TEST_WRITE_FILE   TEST_DIR_VISIT "/testwritefile.txt"

namespace {

bool installed;
GMainLoop *mainLoop;

int __app_install_cb(
#ifdef MULTI_USER_SUPPORT
	uid_t, // target_uid
#endif
	int req_id, const char *pkg_type, const char *pkgid,
	const char *key, const char *val, const void *pmsg, void *data)
{
	(void) req_id;
	(void) pkg_type;
	(void) pkgid;
	(void) pmsg;
	(void) data;

	installed = false;
	if (key && strncmp(key, "end", strlen("end")) == 0) {
		if (strncmp(val, "ok", strlen("ok")) == 0) {
			installed = true;
			g_main_loop_quit(mainLoop);
			g_main_loop_unref(mainLoop);
		}
	}
	return 0;
}

gboolean __app_uninstall_timeout(gpointer)
{
	installed = false;

	return TRUE;
}

void __assertFile(const Csr::File &file, const std::string &path, const std::string &user,
				  const std::string &pkgId, const std::string &pkgPath, bool inApp)
{
	BOOST_REQUIRE_MESSAGE(file.getPath() == path,
						  "expected=" << path << ",actual=" << file.getPath());
	BOOST_REQUIRE_MESSAGE(file.getAppUser() == user,
						  "expected=" << user << ",actual=" << file.getAppUser());
	BOOST_REQUIRE_MESSAGE(file.getAppPkgId() == pkgId,
						  "expected=" << pkgId << ",actual=" << file.getAppPkgId());
	BOOST_REQUIRE_MESSAGE(file.getAppPkgPath() == pkgPath,
						  "expected=" << pkgPath << ",actual=" << file.getAppPkgPath());
	BOOST_REQUIRE_MESSAGE(file.isInApp() == inApp,
						  "expected=" << ((inApp) ? "APP" : "FILE") << ",actual=" << ((file.isInApp()) ? "APP" : "FILE"));
}

void __createFile(const std::string &path)
{
	if (access(path.c_str(), R_OK | W_OK) == 0)
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
	BOOST_REQUIRE(fp != nullptr);
	char buffer[] = { 't', 'e', 's', 't' };
	fwrite(buffer , sizeof(char), sizeof(buffer), fp);
	fclose(fp);
}

}

BOOST_AUTO_TEST_SUITE(INTERNAL_FILE_SYSTEM)

BOOST_AUTO_TEST_CASE(check_in_app)
{
	std::string path1("/sdcard/text.txt");
	Csr::File file1(path1);
	__assertFile(file1, path1, "", "", "", false);

	std::string path2("/usr/apps1/testpkg/test.txt");
	Csr::File file2(path2);
	__assertFile(file2, path2, "", "", "", false);

	std::string path3("/opt/usr/apps1/testpkg/test.txt");
	Csr::File file3(path3);
	__assertFile(file3, path3, "", "", "", false);

	std::string path4("/sdcard1/apps/testpkg/test.txt");
	Csr::File file4(path4);
	__assertFile(file4, path4, "", "", "", false);

	std::string pkgid("testpkg");
	std::string appPath1("/usr/apps/" + pkgid);
	std::string appFilePath1(appPath1 + "/test.txt");
	Csr::File app1(appFilePath1);
	__assertFile(app1, appFilePath1,  "", pkgid, appPath1, true);

	std::string appPath2("/opt/usr/apps/" + pkgid);
	std::string appFilePath2(appPath2 + "/test.txt");
	Csr::File app2(appFilePath2);
	__assertFile(app2, appFilePath2,  "", pkgid, appPath2, true);

	std::string appPath3("/sdcard/apps/" + pkgid);
	std::string appFilePath3(appPath3 + "/test.txt");
	Csr::File app3(appFilePath3);
	__assertFile(app3, appFilePath3,  "", pkgid, appPath3, true);

	std::string appPath4("/sdcard/app2sd/" + pkgid);
	std::string appFilePath4(appPath4 + "/test.txt");
	Csr::File app4(appFilePath4);
	__assertFile(app4, appFilePath4,  "", pkgid, appPath4, true);
}

BOOST_AUTO_TEST_CASE(remove_file)
{
	std::string fpath = "/tmp/test.txt";

	__createFile(fpath);

	Csr::File file(fpath);
	BOOST_REQUIRE_MESSAGE(file.remove(), "Faile to remove file. path=" << fpath);

	bool isRemoved = access(fpath.c_str(), F_OK) != 0 && errno == ENOENT;
	__removeFile(fpath);

	BOOST_REQUIRE_MESSAGE(isRemoved, "file remove done but file still exist...");
}

BOOST_AUTO_TEST_CASE(remove_app)
{
#ifdef MULTI_USER_SUPPORT
	std::string fpath = "/opt/usr/apps/org.example.maliciousapp/shared/res/malicious.txt";
#else
	std::string fpath = "/home/owner/apps_rw/org.example.maliciousapp/shared/res/malicious.txt";
#endif
	std::string appPath = TEST_APP_PKG;

	// install the test app
	auto pkgmgr = pkgmgr_client_new(PC_REQUEST);
	BOOST_REQUIRE(pkgmgr != nullptr);

	int ret = pkgmgr_client_install(pkgmgr, nullptr, nullptr,
								appPath.c_str(), nullptr, PM_QUIET, ::__app_install_cb, nullptr);
	BOOST_REQUIRE_MESSAGE(ret > PKGMGR_R_OK, std::string("expected>") << PKGMGR_R_OK << ", actual=" << ret);
	g_timeout_add_seconds(30, __app_uninstall_timeout, this);
	mainLoop = g_main_loop_new(nullptr, false);
	g_main_loop_run(mainLoop);
	pkgmgr_client_free(pkgmgr);
#ifdef MULTI_USER_SUPPORT
	BOOST_REQUIRE_MESSAGE(installed == true, "fail to install test app. check if you logged in as owner.");
#else
	BOOST_REQUIRE_MESSAGE(installed == true, "fail to install test app");
#endif

	// remove the app
	Csr::File app(fpath);
	BOOST_REQUIRE_MESSAGE(app.remove(), "Faile to remove app. path=" << fpath);

	// check if the app still exists
	pkgmgrinfo_pkginfo_h handle;
	ret = pkgmgrinfo_pkginfo_get_pkginfo(app.getAppPkgId().c_str(), &handle);
	BOOST_REQUIRE(ret < PKGMGR_R_OK);
}

BOOST_AUTO_TEST_CASE(file_visitor_positive_existing)
{
	std::string file = TEST_WRITE_FILE;

	auto visitor = Csr::createVisitor(file, 0);
	CHECK_IS_NOT_NULL(visitor);

	int cnt = 0;
	while (visitor->next())
		cnt++;

	ASSERT_IF(cnt, 1);
}

BOOST_AUTO_TEST_CASE(file_visitor_positive_modified)
{
	std::string file = TEST_WRITE_FILE;

	__writeFile(file);

	auto visitor = Csr::createVisitor(file, time(0) - 1);
	CHECK_IS_NOT_NULL(visitor);

	int cnt = 0;
	while (visitor->next())
		cnt++;

	ASSERT_IF(cnt, 1);
}

BOOST_AUTO_TEST_CASE(file_visitor_negative_non_existing)
{
	std::string file = TEST_DIR "/non_existing_file";

	BOOST_REQUIRE(!Csr::createVisitor(file, 0));
}

BOOST_AUTO_TEST_CASE(directory_visitor_positive_existing)
{
	std::string dir(TEST_DIR_VISIT);

	// test for existing dir
	auto visitor = Csr::createVisitor(dir, 0);
	CHECK_IS_NOT_NULL(visitor);

	int cnt = 0;
	while (visitor->next())
		cnt++;

	ASSERT_IF(cnt, 8);
}

BOOST_AUTO_TEST_CASE(directory_visitor_positive_modified)
{
	std::string dir(TEST_DIR_VISIT);
	std::string file(TEST_WRITE_FILE);

	__writeFile(file);

	auto visitor = Csr::createVisitor(dir, time(0) - 1);
	CHECK_IS_NOT_NULL(visitor);

	int cnt = 0;
	while (visitor->next())
		cnt++;

	ASSERT_IF(cnt, 1);
}

BOOST_AUTO_TEST_SUITE_END()
