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
 * @file        file-system.cpp
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/file-system.h"

#include <regex>
#include <system_error>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>

#include "common/audit/logger.h"
#include "service/exception.h"
#include "service/app-deleter.h"
#include "service/fs-utils.h"

#include <pkgmgr-info.h>

namespace Csr {

namespace {

inline std::regex makeRegexpr(const char *str)
{
	return std::regex(str, std::regex_constants::extended);
}

std::vector<std::regex> g_regexprs{
	// Tizen 2.4 app directories
	makeRegexpr("^(/usr/apps/([^/]+))"),                   // /usr/apps/{pkgid}/
	makeRegexpr("^(/opt/usr/apps/([^/]+))"),               // /opt/usr/apps/{pkgid}/
	makeRegexpr("^(/sdcard/apps/([^/]+))"),                // /sdcard/apps/{pkgid}/
	makeRegexpr("^(/sdcard/app2sd/([^/]+))"),              // /sdcard/app2sd/{pkgid}/
	// Tizen 3.0 app directories
	//makeRegexpr("^(/opt/usr/apps/([^/]+))"),               // /opt/usr/apps/{pkgid}/
	//makeRegexpr("^(/home/([^/]+)/apps_rw/([^/]+))"),       // /home/{user}/apps_rw/{pkgid}/
	//makeRegexpr("^(/sdcard/app2sd/([^/]+)/([^/]+))"),      // /sdcard/app2sd/{user}/{pkgid}/
	//makeRegexpr("^(/sdcard/app2sd/([^/]+))"),              // /sdcard/app2sd/{pkgid}/
	//makeRegexpr("^(/sdcard/apps/([^/]+)/apps_rw/([^/]+))") // /sdcard/apps/{user}/apps_rw/{pkgid}/
};

int getPkgTypes(const std::string &pkgid)
{
	pkgmgrinfo_pkginfo_h handle;
	auto ret = ::pkgmgrinfo_pkginfo_get_pkginfo(pkgid.c_str(), &handle);
	if (ret != PMINFO_R_OK)
		return 0;

	auto type = static_cast<File::Type>(File::Type::Package);

	bool isRemovable = false;
	ret = ::pkgmgrinfo_pkginfo_is_removable(handle, &isRemovable);
	if (ret != PMINFO_R_OK) {
		ERROR("Failed to pkgmgrinfo_pkginfo_is_removable. ret: " << ret);
		return type;
	}

	::pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

	if (isRemovable)
		type |= File::Type::Removable;

	return type;
}

} // namespace anonymous

bool hasPermToRemove(const std::string &filepath)
{
	auto parent = filepath.substr(0, filepath.find_last_of('/'));
	return ::access(parent.c_str(), W_OK) == 0;
}

bool File::isInApp(const std::string &path)
{
	std::smatch matched;

	for (const auto &rege : g_regexprs) {
		if (!std::regex_search(path, matched, rege))
			continue;

		std::string pkgId;

		if (matched.size() == 3)
			pkgId = matched[2];
		else if (matched.size() == 4)
			pkgId = matched[3];
		else
			continue;

		auto types = getPkgTypes(pkgId);
		if (types & Type::Package)
			return true;
		else
			break;
	}

	return false;
}

std::string File::getPkgPath(const std::string &path)
{
	std::smatch matched;

	for (const auto &rege : g_regexprs) {
		if (!std::regex_search(path, matched, rege))
			continue;

		std::string pkgPath;
		std::string pkgId;

		if (matched.size() == 3) {
			pkgPath = matched[1];
			pkgId = matched[2];
		} else if (matched.size() == 4) {
			pkgPath = matched[1];
			pkgId = matched[3];
		} else {
			continue;
		}

		auto types = getPkgTypes(pkgId);
		if (types & Type::Package)
			return pkgPath;
		else
			break;
	}

	return path;
}

File::File(const std::string &fpath, int type) :
	m_path(fpath), m_inApp(false), m_type(type)
{
	std::smatch matched;

	for (const auto &rege : g_regexprs) {
		if (!std::regex_search(m_path, matched, rege))
			continue;

		if (matched.size() == 3) {
			m_appPkgPath = matched[1];
			m_appPkgId = matched[2];
		} else if (matched.size() == 4) {
			m_appPkgPath = matched[1];
			m_appUser = matched[2];
			m_appPkgId = matched[3];
		} else {
			continue;
		}

		m_type |= getPkgTypes(m_appPkgId);

		break;
	}
}

const std::string &File::getPath() const noexcept
{
	return m_path;
}

bool File::isInApp() const noexcept
{
	return m_type & Type::Package;
}

bool File::isDir() const noexcept
{
	return m_type & Type::Directory;
}

bool File::isModified() const noexcept
{
	return m_type & Type::Modified;
}

bool File::isRemovable() const noexcept
{
	return m_type & Type::Removable;
}

bool File::isModified() const noexcept
{
	return m_type & Type::Modified;
}

const std::string &File::getAppPkgId() const
{
	return m_appPkgId;
}

const std::string &File::getAppUser() const
{
	return m_appUser;
}

const std::string &File::getAppPkgPath() const
{
	return m_appPkgPath;
}

void File::remove() const
{
	if (m_inApp) {
		DEBUG("remove app: " << m_appPkgId);
		AppDeleter::remove(m_appPkgId);
	} else {
		DEBUG("remove file: " << m_path);
		if (::remove(m_path.c_str()) != 0)
			ThrowExc(RemoveFailed, "Failed to remove file: " << m_path);
	}
}

FilePtr File::create(const std::string &fpath, time_t modifiedSince)
{
	auto statptr = getStat(fpath);

	if (statptr == nullptr)
		ThrowExc(FileDoNotExist, "file not exist: " << fpath);
	else if (!S_ISREG(statptr->st_mode) && !S_ISDIR(statptr->st_mode))
		ThrowExc(FileSystemError, "file type is not reguler or dir: " << fpath);

	auto type = static_cast<int>(S_ISREG(statptr->st_mode) ? Type::File : Type::Directory);

	if (hasPermToRemove(fpath))
		type |= Type::Removable;

	if (modifiedSince == -1 && statptr->st_ctime > modifiedSince) {
		DEBUG("file[" << fpath << "] is changed since[" << modifiedSince << "]");
		type |= Type::Modified;
	}

	return FilePtr(new File(fpath, type));
}

FsVisitor::DirPtr FsVisitor::openDir(const std::string &dir)
{
	return std::unique_ptr<DIR, int(*)(DIR *)>(::opendir(dir.c_str()), ::closedir);
}

FsVisitorPtr FsVisitor::create(const std::string &dirpath, time_t modifiedSince)
{
	auto statptr = getStat(dirpath);
	if (statptr == nullptr)
		ThrowExc(FileDoNotExist, "directory not exist: " << dirpath);
	else if (!S_ISDIR(statptr->st_mode))
		ThrowExc(FileSystemError, "file type is not directory: " << dirpath);
	else
		return FsVisitorPtr(new FsVisitor(dirpath, modifiedSince));
}

FsVisitor::FsVisitor(const std::string &dirpath, time_t modifiedSince) :
	m_since(modifiedSince), m_dirptr(openDir(dirpath)),
	m_entryBuf(static_cast<struct dirent *>(::malloc(
			offsetof(struct dirent, d_name) + NAME_MAX + 1)))
{
	if (!m_dirptr)
		ThrowExc(InternalError, "Failed to open dir: " << dirpath);

	m_dirs.push((dirpath.back() == '/') ? dirpath : (dirpath + '/'));
}

FsVisitor::~FsVisitor()
{
	::free(m_entryBuf);
}

FilePtr FsVisitor::next()
{
	struct dirent *result = nullptr;
	while (readdir_r(m_dirptr.get(), m_entryBuf, &result) == 0) {
		if (result == nullptr) { // end of dir stream
			m_dirs.pop();
			while (!m_dirs.empty() && !(m_dirptr = openDir(m_dirs.front())))
				m_dirs.pop();

			if (m_dirs.empty())
				return nullptr;
			else
				continue;
		}

		auto &dir = m_dirs.front();
		std::string filepath(result->d_name);

		if (result->d_type == DT_DIR) {
			if (filepath.compare(".") != 0 && filepath.compare("..") != 0)
				m_dirs.emplace(
					dir + ((filepath.back() == '/') ? filepath : (filepath + '/')));
		} else if (result->d_type == DT_REG) {
			try {
				auto fileptr = File::create(dir + filepath, m_since);

				if (fileptr->isModified())
					return fileptr;
			} catch (const FileDoNotExist &e) {
				WARN("file not exist: " << dir << filepath <<
					 " msg: " << e.what());
			} catch (const FileSystemError &e) {
				WARN("file type is not regular...? can it be happened?"
					 " :" << dir << filepath << " msg: " << e.what());
			}
		}
	}

	throw std::system_error(std::error_code(), FORMAT("reading dir: " << m_dirs.front()));
}

} // namespace Csr
