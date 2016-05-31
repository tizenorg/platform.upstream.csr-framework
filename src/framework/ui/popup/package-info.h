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
 * @file        package-info.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Package info getter for displaying detail of package on popup.
 */
#pragma once

#include <pkgmgr-info.h>

#include <string>

namespace Csr {
namespace Ui {

class PackageInfo {
public:
	PackageInfo(const std::string &pkgid);
	virtual ~PackageInfo();

	PackageInfo(const PackageInfo &) = delete;
	PackageInfo &operator=(const PackageInfo &) = delete;
	PackageInfo(PackageInfo &&) = delete;
	PackageInfo &operator=(PackageInfo &&) = delete;

	std::string getIconPath(void);

private:
	pkgmgrinfo_pkginfo_h m_handle;

};


} // namespace Ui
} // namespace Csr
