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
 * @file        package-info.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Package info getter for displaying detail of package on popup.
 */
#include "package-info.h"

#include "common/exception.h"

namespace Csr {
namespace Ui {

PackageInfo::PackageInfo(const std::string &pkgid)
{
	auto ret = ::pkgmgrinfo_pkginfo_get_pkginfo(pkgid.c_str(), &this->m_handle);
	if (ret != PMINFO_R_OK)
		ThrowExc(CSR_ERROR_SERVER, "Invalid package id: " << pkgid << " ret: " << ret);
}

PackageInfo::~PackageInfo()
{
	::pkgmgrinfo_pkginfo_destroy_pkginfo(this->m_handle);
}

const std::string PackageInfo::UNKNOWN = "Unknown";

std::string PackageInfo::getIconPath(void)
{
	char *icon = nullptr;
	auto ret = ::pkgmgrinfo_pkginfo_get_icon(this->m_handle, &icon);
	if (ret != PMINFO_R_OK || icon == nullptr) {
		WARN("Failed to get icon with pkginfo. ret: " << ret);
		return this->UNKNOWN;
	}

	return std::string(icon);
}

std::string PackageInfo::getVersion(void)
{
	char *version = nullptr;
	auto ret = ::pkgmgrinfo_pkginfo_get_version(this->m_handle, &version);
	if (ret != PMINFO_R_OK || version == nullptr) {
		WARN("Failed to get version with pkginfo. ret: " << ret);
		return this->UNKNOWN;
	}

	return std::string(version);
}

std::string PackageInfo::getLabel(void)
{
	char *label = nullptr;
	auto ret = ::pkgmgrinfo_pkginfo_get_label(this->m_handle, &label);
	if (ret != PMINFO_R_OK || label == nullptr) {
		WARN("Failed to get label with pkginfo. ret: " << ret);
		return this->UNKNOWN;
	}

	return std::string(label);
}

} // namespace Ui
} // namespace Csr
