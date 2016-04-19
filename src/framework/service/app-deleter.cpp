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
 * @file        app-deleter.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Remove tizen package with package-manager
 */
#include "service/app-deleter.h"

#include <memory>
#include <stdexcept>
#include <cstring>

#include <glib.h>
#include <unistd.h>
#include <package-manager.h>

#include "common/audit/logger.h"

#ifdef MULTI_USER_SUPPORT
#include <sys/types.h>
#include <pwd.h>
#include <tzplatform_config.h>
#define GLOBAL_USER tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
#define MAX_PWD_LINE_SIZE 512
#endif

#define MAX_WAIT_SEC 10

namespace {

struct LoopData {
	bool isDeleted;
	GMainLoop *loop;
	const std::string &pkgid;

	explicit LoopData(GMainLoop *_loop, const std::string &_pkgid) :
		isDeleted(false), loop(_loop), pkgid(_pkgid) {}
};

void setRemoveResult(bool result, gpointer data)
{
	auto pdata = reinterpret_cast<LoopData *>(data);

	pdata->isDeleted = result;
	g_main_loop_quit(pdata->loop);
}

static int __app_uninstall_cb(
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

	DEBUG("__app_uninstall_cb called : " << pkgid << ":" << key << ":" << val);

	if (key && strncmp(key, "end", strlen("end")) == 0)
		setRemoveResult(strncmp(val, "ok", strlen("ok")) == 0, data);

	return 0;
}

static gboolean __app_uninstall_timeout(gpointer data)
{
	ERROR("Package[" << reinterpret_cast<LoopData *>(data)->pkgid
		  << "] remove timed out!");

	setRemoveResult(false, data);

	return TRUE;
}

#ifdef MULTI_USER_SUPPORT
static int __get_uid(const char* user_name, uid_t *uid)
{
	struct passwd pd;
	struct passwd* tempPwdPtr;
	char pwdbuffer[MAX_PWD_LINE_SIZE];
	int  pwdlinelen = sizeof(pwdbuffer);

	if((getpwnam_r(user_name, &pd, pwdbuffer, pwdlinelen, &tempPwdPtr))!=0)
		return -1;
	else
		*uid = pd.pw_uid;
	return 0;
}
#endif

} // namespace anonymous

namespace Csr {

bool AppDeleter::remove(const std::string &user, const std::string &pkgid)
{
	int ret;

	if (pkgid.empty())
		throw std::logic_error(FORMAT("pkgid shouldn't be empty in AppDeleter." <<user<<","<<pkgid));

	std::unique_ptr<pkgmgr_client, int(*)(pkgmgr_client *)> client(
		pkgmgr_client_new(PC_REQUEST), pkgmgr_client_free);

	std::unique_ptr<GMainLoop, void(*)(GMainLoop *)> loop(
		g_main_loop_new(nullptr, false), g_main_loop_unref);

	if (!client || !loop)
		throw std::bad_alloc();

	LoopData data(loop.get(), pkgid);

#ifdef MULTI_USER_SUPPORT
	uid_t uid = GLOBAL_USER;
	if(!user.empty())
		if( __get_uid(user.c_str(), &uid) < 0 )
			throw std::runtime_error(FORMAT("Invalid User Name. name=" << user));
	ret = pkgmgr_client_usr_uninstall(client.get(), nullptr, pkgid.c_str(), PM_QUIET,
					::__app_uninstall_cb, &data, uid);
#else
	ret = pkgmgr_client_uninstall(client.get(), nullptr, pkgid.c_str(), PM_QUIET,
					::__app_uninstall_cb, &data);
#endif
	if(ret <= PKGMGR_R_OK)
		return false;

	g_timeout_add_seconds(MAX_WAIT_SEC, __app_uninstall_timeout, &data);

	g_main_loop_run(loop.get());

	DEBUG("App Removed. pkgid: " << pkgid);

	return data.isDeleted;
}

} // namespace Csr
