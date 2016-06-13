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
#include "common/exception.h"
#include "service/app-deleter.h"
#include "service/fs-utils.h"
#include "service/dir-blacklist.h"

#include <pkgmgr-info.h>

namespace Csr {

namespace {

inline std::regex makeRegexpr(const char *str)
{
	return std::regex(str, std::regex_constants::extended);
}

std::vector<std::regex> g_regexprs{
#ifdef PLATFORM_VERSION_3
	makeRegexpr("^(/opt/usr/apps/([^/]+))"),               // /opt/usr/apps/{pkgid}/
	makeRegexpr("^(/home/([^/]+)/apps_rw/([^/]+))"),       // /home/{user}/apps_rw/{pkgid}/
	makeRegexpr("^(/sdcard/app2sd/([^/]+)/([^/]+))"),      // /sdcard/app2sd/{user}/{pkgid}/
	makeRegexpr("^(/sdcard/app2sd/([^/]+))"),              // /sdcard/app2sd/{pkgid}/
	makeRegexpr("^(/sdcard/apps/([^/]+)/apps_rw/([^/]+))") // /sdcard/apps/{user}/apps_rw/{pkgid}/
#else
	makeRegexpr("^(/usr/apps/([^/]+))"),                   // /usr/apps/{pkgid}/
	makeRegexpr("^(/opt/usr/apps/([^/]+))"),               // /opt/usr/apps/{pkgid}/
	makeRegexpr("^(/sdcard/apps/([^/]+))"),                // /sdcard/apps/{pkgid}/
	makeRegexpr("^(/sdcard/app2sd/([^/]+))"),              // /sdcard/app2sd/{pkgid}/
#endif
};

} // namespace anonymous

int File::getPkgTypes(const std::string &pkgid)
{
	pkgmgrinfo_pkginfo_h handle;
	auto ret = ::pkgmgrinfo_pkginfo_get_pkginfo(pkgid.c_str(), &handle);
	if (ret != PMINFO_R_OK)
		return 0;

	auto type = static_cast<int>(Type::Package);

	::pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

	return type;
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

		auto types = File::getPkgTypes(pkgId);
		if (types & static_cast<int>(Type::Package))
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

		auto types = File::getPkgTypes(pkgId);
		if (types & static_cast<int>(Type::Package))
			return pkgPath;
		else
			break;
	}

	return path;
}

File::File(const std::string &fpath, int type) : m_path(fpath), m_type(type)
{
	std::smatch matched;

	for (const auto &rege : g_regexprs) {
		if (!std::regex_search(this->m_path, matched, rege))
			continue;

		if (matched.size() == 3) {
			this->m_appPkgPath = matched[1];
			this->m_appPkgId = matched[2];
		} else if (matched.size() == 4) {
			this->m_appPkgPath = matched[1];
			this->m_appUser = matched[2];
			this->m_appPkgId = matched[3];
		} else {
			continue;
		}

		this->m_type |= File::getPkgTypes(this->m_appPkgId);

		break;
	}
}

const std::string &File::getPath() const noexcept
{
	return this->m_path;
}

bool File::isInApp() const noexcept
{
	return this->m_type & static_cast<int>(Type::Package);
}

bool File::isDir() const noexcept
{
	return this->m_type & static_cast<int>(Type::Directory);
}

bool File::isModified() const noexcept
{
	return this->m_type & static_cast<int>(Type::Modified);
}

const std::string &File::getAppPkgId() const noexcept
{
	return this->m_appPkgId;
}

const std::string &File::getAppUser() const noexcept
{
	return this->m_appUser;
}

const std::string &File::getAppPkgPath() const noexcept
{
	return this->m_appPkgPath;
}

void File::remove() const
{
	if (this->isInApp()) {
		DEBUG("remove app: " << this->m_appPkgId);
		AppDeleter::remove(this->m_appPkgId, this->m_appUser);
	} else {
		DEBUG("remove file: " << this->m_path);
		if (::remove(this->m_path.c_str()) != 0)
			ThrowExc(CSR_ERROR_REMOVE_FAILED,
					 "Failed to remove file: " << this->m_path << " with errno: " << errno);
	}
}

FilePtr File::createIfModified(const std::string &fpath, time_t modifiedSince)
{
	return File::createInternal(fpath, modifiedSince, true);
}

FilePtr File::create(const std::string &fpath, time_t modifiedSince)
{
	return File::createInternal(fpath, modifiedSince, false);
}

FilePtr File::createInternal(const std::string &fpath, time_t modifiedSince,
							 bool isModifiedOnly)
{
	auto statptr = getStat(fpath);

	if (statptr == nullptr)
		ThrowExc(CSR_ERROR_FILE_DO_NOT_EXIST, "file not exist or no permission: " << fpath);
	else if (!S_ISREG(statptr->st_mode) && !S_ISDIR(statptr->st_mode))
		ThrowExc(CSR_ERROR_FILE_SYSTEM, "file type is not reguler or dir: " << fpath);

	auto type = static_cast<int>(S_ISREG(statptr->st_mode) ? Type::File : Type::Directory);

	if (modifiedSince == -1 || statptr->st_ctime > modifiedSince) {
		DEBUG("file[" << fpath << "] is changed since[" << modifiedSince << "]");
		type |= static_cast<int>(Type::Modified);
	}

	if (isModifiedOnly && !(type & static_cast<int>(Type::Modified)))
		return nullptr;
	else
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
		ThrowExc(CSR_ERROR_FILE_DO_NOT_EXIST, "directory not exist or no permission: " << dirpath);
	else if (!S_ISDIR(statptr->st_mode))
		ThrowExc(CSR_ERROR_FILE_SYSTEM, "file type is not directory: " << dirpath);
	else
		return FsVisitorPtr(new FsVisitor(dirpath, modifiedSince));
}

FsVisitor::FsVisitor(const std::string &dirpath, time_t modifiedSince) :
	m_since(modifiedSince), m_dirptr(openDir(dirpath)),
	m_entryBuf(static_cast<struct dirent *>(::malloc(
			offsetof(struct dirent, d_name) + NAME_MAX + 1)))
{
	if (!this->m_dirptr)
		ThrowExc(CSR_ERROR_SERVER, "Failed to open dir: " << dirpath);

	this->m_dirs.push((dirpath.back() == '/') ? dirpath : (dirpath + '/'));
}

FsVisitor::~FsVisitor()
{
	::free(this->m_entryBuf);
}

FilePtr FsVisitor::next()
{
	struct dirent *result = nullptr;
	while (true) {
		bool isDone = false;

		if (readdir_r(this->m_dirptr.get(), this->m_entryBuf, &result) != 0) {
			ERROR("readdir_r error on dir: " << this->m_dirs.front() <<
				  " with errno: " << errno << ". Silently ignore this error & dir stream"
				  " to reduce side-effect of traversing all the other file systems.");
			isDone = true;
		} else if (result == nullptr) {
			DEBUG("End of stream of dir: " << this->m_dirs.front());
			isDone = true;
		} else if (isInBlackList(this->m_dirs.front())) {
			DEBUG("dir[" << this->m_dirs.front() << "] is in black list.");
			isDone = true;
		}

		if (isDone) {
			this->m_dirs.pop();
			while (!this->m_dirs.empty() &&
					!(this->m_dirptr = openDir(this->m_dirs.front())))
				this->m_dirs.pop();

			if (this->m_dirs.empty())
				return nullptr;
			else
				continue;
		}

		auto &dir = this->m_dirs.front();
		std::string filepath(result->d_name);

		if (result->d_type == DT_DIR) {
			if (filepath.compare(".") != 0 && filepath.compare("..") != 0)
				this->m_dirs.emplace(
					dir + ((filepath.back() == '/') ? filepath : (filepath + '/')));
		} else if (result->d_type == DT_REG) {
			try {
				auto fileptr = File::createIfModified(dir + filepath, this->m_since);

				if (fileptr)
					return fileptr;
			} catch (const Exception &e) {
				if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST)
					WARN("file not exist: " << dir << filepath << " msg: " << e.what());
				else if (e.error() == CSR_ERROR_FILE_SYSTEM)
					WARN("file type is not regular...? can it be happened?"
						 " :" << dir << filepath << " msg: " << e.what());
				else
					throw;
			}
		}
	}

	ThrowExc(CSR_ERROR_FILE_SYSTEM, "readdir_r error on dir: " << this->m_dirs.front());
}

} // namespace Csr
