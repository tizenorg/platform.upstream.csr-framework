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
#include <cstring>

#include <package-manager.h>

#include "common/audit/logger.h"
#include "service/exception.h"

namespace Csr {

void AppDeleter::remove(const std::string &pkgid)
{
	if (pkgid.empty())
		ThrowExc(InternalError, "pkgid shouldn't be empty in AppDeleter");

	std::unique_ptr<pkgmgr_client, int(*)(pkgmgr_client *)> client(
		pkgmgr_client_new(PC_REQUEST), pkgmgr_client_free);

	if (!client)
		throw std::bad_alloc();

	auto ret = pkgmgr_client_uninstall(client.get(), nullptr, pkgid.c_str(), PM_QUIET,
									   nullptr, nullptr);
	if (ret < PKGMGR_R_OK)
		ThrowExc(RemoveFailed, "Failed to pkgmgr_client_uninstall for pkg: " << pkgid <<
				 " ret: " << ret);

	DEBUG("App Removed. pkgid: " << pkgid);
}

} // namespace Csr
