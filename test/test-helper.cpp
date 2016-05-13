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
 * @file        test-helper.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       CSR API test helper
 */

#include <string>
#include <iostream>
#include <fstream>
#include <boost/test/unit_test.hpp>

#include <time.h>
#include <utime.h>

#include <package-manager.h>
#include <pkgmgr-info.h>

#include "test-common.h"

void ASSERT_STRING(const char *expected, const char *actual, const char *msg)
{
	if( expected == actual ) // true including nullptr
		return;

	// null string and empty string are same
	if( (expected == nullptr) && (actual != nullptr) && (strlen(actual) == 0) )
		return;
	if( (actual == nullptr) && (expected != nullptr) && (strlen(expected) == 0) )
		return;

	BOOST_REQUIRE_MESSAGE( (expected != nullptr) && (actual != nullptr) && (strcmp(expected, actual) == 0),
	 	std::string((msg == nullptr) ? "NULL" : msg)
		<< ", EXPECTED=" << std::string((expected == nullptr) ? "NULL" : expected)
		<< ", ACTUAL=" << std::string((actual == nullptr) ? "NULL" : actual) );
}

void ASSERT_DETECTED(csr_cs_detected_h detected, const char* name, int severity, const char* detailed_url)
{
	csr_cs_severity_level_e d_severity;
	const char *d_malware_name;
	const char *d_detailed_url;

	ASSERT_IF(csr_cs_detected_get_severity(detected, &d_severity), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_malware_name(detected, &d_malware_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_detailed_url(detected, &d_detailed_url), CSR_ERROR_NONE);

	BOOST_REQUIRE_MESSAGE(severity == d_severity,
		"EXPECTED=" << severity << ", ACTUAL=" << d_severity);
	ASSERT_STRING(name, d_malware_name, "MALWARE NAME CMP FAIL");
	ASSERT_STRING(detailed_url, d_detailed_url, "DETAILED ULR CMP FAIL");
}

void ASSERT_DETECTED_EXT(csr_cs_detected_h detected, time_t time, const char* file_name, bool is_app, const char* pkg_id)
{
	time_t d_timestamp;
	const char *d_file_name;
	bool d_is_app;
	const char *d_pkg_id;

	ASSERT_IF(csr_cs_detected_get_timestamp(detected, &d_timestamp), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_file_name(detected, &d_file_name), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_is_app(detected, &d_is_app), CSR_ERROR_NONE);
	ASSERT_IF(csr_cs_detected_get_pkg_id(detected, &d_pkg_id), CSR_ERROR_NONE);

	BOOST_REQUIRE_MESSAGE(time <= d_timestamp,
		"TIMESTAMP CMP FAIL. EXPECTED should be smaller than ACTUAL, EXPECTED="
			<< time << ", ACTUAL=" << d_timestamp);
	ASSERT_STRING(file_name, d_file_name, "NAME CMP FAIL");
	BOOST_REQUIRE_MESSAGE(is_app == d_is_app,
		"IS_APP CMP FAIL. EXPECTED=" << is_app << ", ACTUAL=" << d_is_app);
	ASSERT_STRING(pkg_id, d_pkg_id, "PKGID CMP FAIL");
}


void copy_file(const char* src_file, const char* dest_file )
{
	std::ifstream srce( src_file, std::ios::binary ) ;
	std::ofstream dest( dest_file, std::ios::binary ) ;
	dest << srce.rdbuf() ;
}

void touch_file(const char *file)
{
	struct utimbuf new_times;
	time_t now = time(nullptr);

	new_times.actime = now;
	new_times.modtime = now;

	utime(file, &new_times);
}

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

bool install_app(const char *app_path, const char *pkg_type)
{
	auto pkgmgr = pkgmgr_client_new(PC_REQUEST);
	CHECK_IS_NOT_NULL(pkgmgr);

	installed = false;
	int ret = pkgmgr_client_install(pkgmgr, pkg_type, nullptr, app_path, nullptr, PM_QUIET,
					__app_install_cb, nullptr);
	if(ret <= PKGMGR_R_OK)
		return false;

	g_timeout_add_seconds(30, __app_install_timeout, nullptr);
	installMainLoop = g_main_loop_new(nullptr, false);
	g_main_loop_run(installMainLoop);
	pkgmgr_client_free(pkgmgr);

	return installed;
}


bool uninstalled;
GMainLoop *uninstallMainLoop;
int __app_uninstall_cb(int req_id, const char *pkg_type, const char *pkgid, const char *key,
		const char *val, const void *pmsg, void *data)
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

bool uninstall_app(const char *pkg_id)
{
	auto pkgmgr = pkgmgr_client_new(PC_REQUEST);
	CHECK_IS_NOT_NULL(pkgmgr);

	uninstalled = false;
	int ret = pkgmgr_client_uninstall(pkgmgr, nullptr, pkg_id, PM_QUIET, __app_uninstall_cb, nullptr);
	if(ret <= PKGMGR_R_OK)
		return false;

	g_timeout_add_seconds(30, __app_uninstall_timeout, nullptr);
	uninstallMainLoop = g_main_loop_new(nullptr, false);
	g_main_loop_run(uninstallMainLoop);
	pkgmgr_client_free(pkgmgr);

	return uninstalled;
}
