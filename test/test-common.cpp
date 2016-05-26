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
#include <functional>
#include <unistd.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <glib.h>

#include <package-manager.h>

namespace Test {

namespace {

struct PkgEventData {
	bool isSuccess;
	GMainLoop *loop;

	PkgEventData() : isSuccess(false), loop(nullptr) {}
};

#ifdef PLATFORM_VERSION_3
int __quit_loop_on_end_cb(uid_t, int req_id, const char *pkg_type, const char *pkgid,
						  const char *key, const char *val, const void *pmsg, void *data)
#else
int __quit_loop_on_end_cb(int req_id, const char *pkg_type, const char *pkgid,
						  const char *key, const char *val, const void *pmsg, void *data)
#endif
{
	(void) req_id;
	(void) pkg_type;
	(void) pkgid;
	(void) pmsg;

	auto eventData = reinterpret_cast<PkgEventData *>(data);

	if (key && strncmp(key, "end", strlen("end")) == 0) {
		eventData->isSuccess = (strncmp(val, "ok", strlen("ok")) == 0);

		g_main_loop_quit(eventData->loop);
	}

	return 0;
}

gboolean __quit_loop_on_timeout_cb(gpointer data)
{
	auto eventData = reinterpret_cast<PkgEventData *>(data);

	eventData->isSuccess = false;
	g_main_loop_quit(eventData->loop);

	return FALSE;
}

bool pkgmgr_request(const std::function<int(pkgmgr_client *, PkgEventData *)> &request)
{
	auto pkgmgr = pkgmgr_client_new(PC_REQUEST);
	CHECK_IS_NOT_NULL(pkgmgr);

	PkgEventData data;
	auto ret = request(pkgmgr, &data);
	if (ret <= PKGMGR_R_OK)
		return false;

	auto id = g_timeout_add_seconds(10, __quit_loop_on_timeout_cb, &data);
	data.loop = g_main_loop_new(nullptr, false);
	g_main_loop_run(data.loop);
	BOOST_REQUIRE_MESSAGE(g_source_remove(id),
			"Failed to remove timeout event source from main loop.");
	g_main_loop_unref(data.loop);
	pkgmgr_client_free(pkgmgr);

	return data.isSuccess;
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
	ERRORDESCRIBE(CSR_ERROR_FILE_CHANGED);
	ERRORDESCRIBE(CSR_ERROR_FILE_SYSTEM);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_PERMISSION);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_NOT_EXIST);
	ERRORDESCRIBE(CSR_ERROR_ENGINE_DISABLED);
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
									   unsigned int line,
									   const std::string &msg)
{
	BOOST_REQUIRE_MESSAGE(value == expected,
						  "[" << filename << " > " << funcname << " : " << line << "]" <<
						  " returned[" << capi_ec_to_string(value) <<
						  "] expected[" << capi_ec_to_string(expected) << "] " << msg);
}

template <>
void _assert<csr_error_e, int>(const csr_error_e &value,
							   const int &expected,
							   const std::string &filename,
							   const std::string &funcname,
							   unsigned int line,
							   const std::string &msg)
{
	BOOST_REQUIRE_MESSAGE(value == expected,
						  "[" << filename << " > " << funcname << " : " << line << "]" <<
						  " returned[" << capi_ec_to_string(value) << "] expected[" <<
						  capi_ec_to_string(static_cast<csr_error_e>(expected)) << "]" <<
						  " " << msg);
}

template <>
void _assert<int, csr_error_e>(const int &value,
							   const csr_error_e &expected,
							   const std::string &filename,
							   const std::string &funcname,
							   unsigned int line,
							   const std::string &msg)
{
	BOOST_REQUIRE_MESSAGE(value == expected,
						  "[" << filename << " > " << funcname << " : " << line << "]" <<
						  " returned[" <<
						  capi_ec_to_string(static_cast<csr_error_e>(value)) <<
						  "] expected[" << capi_ec_to_string(expected) << "] " << msg);
}

template <>
void _assert<const char *, const char *>(const char * const &value,
										 const char * const &expected,
										 const std::string &filename,
										 const std::string &funcname,
										 unsigned int line,
										 const std::string &msg)
{
	if (value == nullptr && expected == nullptr)
		BOOST_REQUIRE(true);
	else if (value != nullptr && expected != nullptr)
		_assert<std::string, const char *>(std::string(value), expected, filename,
										   funcname, line, msg);
	else if (value == nullptr && expected != nullptr)
		BOOST_REQUIRE_MESSAGE(std::string(expected).empty(),
							  "[" << filename << " > " << funcname << " : " << line <<
							  "] returned[nullptr] expected[" << expected << "] " << msg);
	else
		BOOST_REQUIRE_MESSAGE(std::string(value).empty(),
							  "[" << filename << " > " << funcname << " : " << line <<
							  "] returned[" << value << "] expected[nullptr] " << msg);
}

template <>
void _assert<char *, const char *>(char * const &value,
								   const char * const &expected,
								   const std::string &filename,
								   const std::string &funcname,
								   unsigned int line,
								   const std::string &msg)
{
	_assert<const char *, const char *>(value, expected, filename, funcname, line, msg);
}

template <>
void _assert<const char *, char *>(const char * const &value,
								   char * const &expected,
								   const std::string &filename,
								   const std::string &funcname,
								   unsigned int line,
								   const std::string &msg)
{
	_assert<const char *, const char *>(value, expected, filename, funcname, line, msg);
}

template <>
void _assert<char *, char *>(char * const &value,
							 char * const &expected,
							 const std::string &filename,
							 const std::string &funcname,
							 unsigned int line,
							 const std::string &msg)
{
	_assert<const char *, const char *>(value, expected, filename, funcname, line, msg);
}

template <>
void _assert<const char *, std::string>(const char * const &value,
										const std::string &expected,
										const std::string &filename,
										const std::string &funcname,
										unsigned int line,
										const std::string &msg)
{
	_assert<std::string, std::string>(
			(value == nullptr) ? std::string() : std::string(value),
			expected, filename, funcname, line, msg);
}

template <>
void _assert<char *, std::string>(char * const &value,
								  const std::string &expected,
								  const std::string &filename,
								  const std::string &funcname,
								  unsigned int line,
								  const std::string &msg)
{
	_assert<const char *, std::string>(value, expected, filename, funcname, line, msg);
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

void make_dir(const char *dir)
{
	mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO);
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
	return pkgmgr_request([&](pkgmgr_client *pc, PkgEventData *data) {
		return pkgmgr_client_uninstall(pc, nullptr, pkg_id, PM_QUIET,
									   __quit_loop_on_end_cb, data);
	});
}

bool install_app(const char *app_path, const char *pkg_type)
{
	return pkgmgr_request([&](pkgmgr_client *pc, PkgEventData *data) {
		return pkgmgr_client_install(pc, pkg_type, nullptr, app_path, nullptr, PM_QUIET,
									 __quit_loop_on_end_cb, data);
	});
}

void initialize_db()
{
	remove_file(RW_DBSPACE ".csr.db");
	remove_file(RW_DBSPACE ".csr.db-journal");

	int ret = system("systemctl restart csr.service");
	BOOST_MESSAGE("CSR DB Initalization & Daemon Restart. Result=" << ret);
}

} // namespace Test
