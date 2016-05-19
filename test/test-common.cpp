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
 * @file        test-common.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Common utilities for test
 */
#include "test-common.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <utime.h>

#include <package-manager.h>
#include <pkgmgr-info.h>

namespace Test {

namespace {

bool installed;
GMainLoop *installMainLoop;
int __app_install_cb(int req_id, const char *pkg_type, const char *pkgid,
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
			g_main_loop_quit(installMainLoop);
			g_main_loop_unref(installMainLoop);
		}
	}

	return 0;
}

gboolean __app_install_timeout(gpointer)
{
	installed = false;
	return TRUE;
}

bool uninstalled;
GMainLoop *uninstallMainLoop;
int __app_uninstall_cb(int req_id, const char *pkg_type, const char *pkgid,
					   const char *key, const char *val, const void *pmsg, void *data)
{
	(void) req_id;
	(void) pkg_type;
	(void) pkgid;
	(void) pmsg;
	(void) data;

	uninstalled = false;

	if (key && strncmp(key, "end", strlen("end")) == 0) {
		if (strncmp(val, "ok", strlen("ok")) == 0) {
			uninstalled = true;
			g_main_loop_quit(uninstallMainLoop);
			g_main_loop_unref(uninstallMainLoop);
		}
	}

	return 0;
}

gboolean __app_uninstall_timeout(gpointer)
{
	uninstalled = false;
	return TRUE;
}

#define ERRORDESCRIBE(name) case name: return #name
std::string capi_ec_to_string(csr_error_e ec)
{
	switch (ec) {
	ERRORDESCRIBE(CSR_ERROR_NONE);
	ERRORDESCRIBE(CSR_ERROR_INVALID_PARAMETER);
	ERRORDESCRIBE(CSR_ERROR_OUT_OF_MEMORY);
	ERRORDESCRIBE(CSR_ERROR_PERMISSION_DENIED);
	ERRORDESCRIBE(CSR_ERROR_SOCKET);
	ERRORDESCRIBE(CSR_ERROR_INVALID_HANDLE);
	ERRORDESCRIBE(CSR_ERROR_SERVER);
	ERRORDESCRIBE(CSR_ERROR_NO_TASK);
	ERRORDESCRIBE(CSR_ERROR_DB);
	ERRORDESCRIBE(CSR_ERROR_REMOVE_FAILED);
	ERRORDESCRIBE(CSR_ERROR_FILE_DO_NOT_EXIST);
	ERRORDESCRIBE(CSR_ERROR_FILE_SYSTEM);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_PERMISSION);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_NOT_EXIST);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_DIASABLED);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_NOT_ACTIVATED);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_INTERNAL);
	ERRORDESCRIBE(CSR_ERROR_UNKNOWN);
	default: return std::string("Undefined capi error code: ")
						+ std::to_string(static_cast<int>(ec));
	}
}
#undef ERRORDESCRIBE

} // namespace anonymous

template <>
void _assert<csr_error_e, csr_error_e>(const csr_error_e &value,
									   const csr_error_e &expected,
									   const std::string &filename,
									   const std::string &funcname,
									   unsigned int line)
{
	BOOST_REQUIRE_MESSAGE(value == expected,
						  "[" << filename << " > " << funcname << " : " << line << "]" <<
						  " returned[" << capi_ec_to_string(value) <<
						  "] expected[" << capi_ec_to_string(expected) << "]");
}

template <>
void _assert<csr_error_e, int>(const csr_error_e &value,
							   const int &expected,
							   const std::string &filename,
							   const std::string &funcname,
							   unsigned int line)
{
	BOOST_REQUIRE_MESSAGE(value == expected,
						  "[" << filename << " > " << funcname << " : " << line << "]" <<
						  " returned[" << capi_ec_to_string(value) << "] expected[" <<
						  capi_ec_to_string(static_cast<csr_error_e>(expected)) << "]");
}

template <>
void _assert<int, csr_error_e>(const int &value,
							   const csr_error_e &expected,
							   const std::string &filename,
							   const std::string &funcname,
							   unsigned int line)
{
	BOOST_REQUIRE_MESSAGE(value == expected,
						  "[" << filename << " > " << funcname << " : " << line << "]" <<
						  " returned[" <<
						  capi_ec_to_string(static_cast<csr_error_e>(value)) <<
						  "] expected[" << capi_ec_to_string(expected) << "]");
}

void _assert(const char *value, const char *expected, const std::string &filename,
			 const std::string &funcname, unsigned int line)
{
	if (value == nullptr && expected == nullptr)
		BOOST_REQUIRE(true);
	else if (value != nullptr && expected != nullptr)
		_assert(std::string(value), expected, filename, funcname, line);
	else if (value == nullptr && expected != nullptr)
		BOOST_REQUIRE_MESSAGE(std::string(expected).empty(),
							  "[" << filename << " > " << funcname << " : " << line <<
							  "] returned[nullptr] expected[" << expected << "]");
	else
		BOOST_REQUIRE_MESSAGE(std::string(value).empty(),
							  "[" << filename << " > " << funcname << " : " << line <<
							  "] returned[" << value << "] expected[nullptr]");
}

void _assert(const char *value, const std::string &expected, const std::string &filename,
			 const std::string &funcname, unsigned int line)
{
	_assert((value == nullptr) ? std::string() : std::string(value),
			expected, filename, funcname, line);
}

void exceptionGuard(const std::function<void()> &f)
{
	try {
		f();
	} catch (const std::exception &e) {
		BOOST_REQUIRE_MESSAGE(0, "std::exception caught: " << e.what());
	} catch (...) {
		BOOST_REQUIRE_MESSAGE(0, "Unknown exception caught");
	}
}

void copy_file(const char *src_file, const char *dest_file)
{
	std::ifstream srce(src_file, std::ios::binary);
	std::ofstream dest(dest_file, std::ios::binary);
	dest << srce.rdbuf();
}

void touch_file(const char *file)
{
	struct utimbuf new_times;
	time_t now = time(nullptr);

	new_times.actime = now;
	new_times.modtime = now;

	utime(file, &new_times);
}

void remove_file(const char *file)
{
	unlink(file);
}

bool is_file_exist(const char *file)
{
	return (access(file, F_OK) != -1);
}

bool uninstall_app(const char *pkg_id)
{
	auto pkgmgr = pkgmgr_client_new(PC_REQUEST);
	CHECK_IS_NOT_NULL(pkgmgr);

	uninstalled = false;
	int ret = pkgmgr_client_uninstall(pkgmgr, nullptr, pkg_id, PM_QUIET,
									  __app_uninstall_cb, nullptr);
	if (ret <= PKGMGR_R_OK)
		return false;

	g_timeout_add_seconds(30, __app_uninstall_timeout, nullptr);
	uninstallMainLoop = g_main_loop_new(nullptr, false);
	g_main_loop_run(uninstallMainLoop);
	pkgmgr_client_free(pkgmgr);

	return uninstalled;
}

bool is_app_exist(const char *pkg_id)
{
	pkgmgrinfo_pkginfo_h handle;

	if (pkgmgrinfo_pkginfo_get_pkginfo(pkg_id, &handle) != PMINFO_R_OK)
		return false;

	pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
	return true;
}

bool install_app(const char *app_path, const char *pkg_type)
{
	auto pkgmgr = pkgmgr_client_new(PC_REQUEST);
	CHECK_IS_NOT_NULL(pkgmgr);

	installed = false;
	int ret = pkgmgr_client_install(pkgmgr, pkg_type, nullptr, app_path, nullptr, PM_QUIET,
									__app_install_cb, nullptr);
	if (ret <= PKGMGR_R_OK)
		return false;

	g_timeout_add_seconds(30, __app_install_timeout, nullptr);
	installMainLoop = g_main_loop_new(nullptr, false);
	g_main_loop_run(installMainLoop);
	pkgmgr_client_free(pkgmgr);

	return installed;
}

} // namespace Test
