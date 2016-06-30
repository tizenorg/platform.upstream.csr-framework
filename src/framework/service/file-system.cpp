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
	makeRegexpr("^(/opt/home/([^/]+)/apps_rw/([^/]+))"),   // /opt/home/{user}/apps_rw/{pkgid}/
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

int File::getPkgTypes(const std::string &user, const std::string &pkgid)
{
	pkgmgrinfo_pkginfo_h handle;

#ifdef PLATFORM_VERSION_3
	int ret = -1;
	if (user.empty())
		ret = ::pkgmgrinfo_pkginfo_get_pkginfo(pkgid.c_str(), &handle);
	else
		ret = ::pkgmgrinfo_pkginfo_get_usr_pkginfo(pkgid.c_str(), getUid(user), &handle);
#else
	(void) user;
	auto ret = ::pkgmgrinfo_pkginfo_get_pkginfo(pkgid.c_str(), &handle);
#endif

	if (ret != PMINFO_R_OK) {
		INFO("Extracted pkgid[" << pkgid << "] from filepath isn't pkg id. "
			 "It's not package.");
		return 0;
	}

	auto type = static_cast<int>(Type::Package);

	bool isPreloaded = false;
	ret = ::pkgmgrinfo_pkginfo_is_preload(handle, &isPreloaded);

	if (ret != PMINFO_R_OK)
		ERROR("Failed to ::pkgmgrinfo_pkginfo_is_preload: " << ret);

	if (isPreloaded)
		type |= static_cast<int>(Type::PreLoaded);

	::pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

	return type;
}

std::string File::getPkgPath(const std::string &path)
{
	std::smatch matched;

	for (const auto &rege : g_regexprs) {
		if (!std::regex_search(path, matched, rege))
			continue;

		std::string pkgPath;
		std::string pkgUser;
		std::string pkgId;

		if (matched.size() == 3) {
			pkgPath = matched[1];
			pkgId = matched[2];
		} else if (matched.size() == 4) {
			pkgPath = matched[1];
			pkgUser = matched[2];
			pkgId = matched[3];
		} else {
			continue;
		}

		auto type = File::getPkgTypes(pkgUser, pkgId);

		return ((type & static_cast<int>(Type::Package)) &&
			   (!(type & static_cast<int>(Type::PreLoaded)))) ? pkgPath : path;
	}

	return path;
}

File::File(const std::string &fpath, const FilePtr &parentdir, int type,
		   std::unique_ptr<struct stat> &&statptr) :
	m_path(fpath), m_type(type), m_statptr(std::move(statptr))
{
	if (parentdir != nullptr) {
		if (parentdir->isPackage()) {
			this->m_appPkgPath = parentdir->getAppPkgPath();
			this->m_appPkgId = parentdir->getAppPkgId();
			this->m_appUser = parentdir->getAppUser();

			this->m_type |= static_cast<int>(File::Type::Package);

			if (parentdir->isPreloaded())
				this->m_type |= static_cast<int>(File::Type::PreLoaded);

			return;
		} else if (!this->isDir()) {
			this->m_type &= ~(static_cast<int>(File::Type::Package) |
							  static_cast<int>(File::Type::PreLoaded));
		}
	}

	std::smatch matched;

	for (const auto &rege : g_regexprs) {
		if (!std::regex_search(this->m_path, matched, rege))
			continue;

		if (matched.size() == 3) {
			this->m_appPkgPath = matched[1];
			this->m_appPkgId = matched[2];
			this->m_appUser.clear();
		} else if (matched.size() == 4) {
			this->m_appPkgPath = matched[1];
			this->m_appUser = matched[2];
			this->m_appPkgId = matched[3];
		} else {
			continue;
		}

		this->m_type |= File::getPkgTypes(this->m_appUser, this->m_appPkgId);

		break;
	}
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

FilePtr File::createIfModified(const std::string &fpath, const FilePtr &parentdir, time_t modifiedSince)
{
	return File::createInternal(fpath, parentdir, modifiedSince, true);
}

FilePtr File::create(const std::string &fpath, const FilePtr &parentdir, time_t modifiedSince)
{
	return File::createInternal(fpath, parentdir, modifiedSince, false);
}

FilePtr File::createInternal(const std::string &fpath, const FilePtr &parentdir,
							 time_t modifiedSince, bool isModifiedOnly)
{
	auto statptr = getStat(fpath);

	if (statptr == nullptr)
		ThrowExcWarn(CSR_ERROR_FILE_DO_NOT_EXIST, "file not exist or no permission: " <<
					 fpath);
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
		return FilePtr(new File(fpath, parentdir, type, std::move(statptr)));
}

FsVisitor::DirPtr FsVisitor::openDir(const std::string &dir)
{
	return std::unique_ptr<DIR, int(*)(DIR *)>(
		(isInBlackList(dir) ? nullptr : ::opendir(dir.c_str())), ::closedir);
}

FsVisitorPtr FsVisitor::create(TargetHandler &&targetHandler,
							   const std::string &dirpath, bool isBasedOnName,
							   time_t modifiedSince)
{
	auto statptr = getStat(dirpath);
	if (statptr == nullptr)
		ThrowExcWarn(CSR_ERROR_FILE_DO_NOT_EXIST, "directory not exist or no "
					 "permission: " << dirpath);
	else if (!S_ISDIR(statptr->st_mode))
		ThrowExc(CSR_ERROR_FILE_SYSTEM, "file type is not directory: " << dirpath);
	else
		return FsVisitorPtr(new FsVisitor(std::move(targetHandler), dirpath,
										  isBasedOnName, modifiedSince));
}

FsVisitor::FsVisitor(TargetHandler &&targetHandler,
					 const std::string &dirpath, bool isBasedOnName, time_t modifiedSince) :
	m_targetHandler(std::move(targetHandler)), m_path(dirpath),
	m_since(modifiedSince), m_isDone(true), m_isBasedOnName(isBasedOnName),
	m_entryBuf(static_cast<struct dirent *>(::malloc(offsetof(struct dirent, d_name) + NAME_MAX + 1)))
{
	if (this->m_entryBuf == nullptr)
		throw std::bad_alloc();
}

FsVisitor::~FsVisitor()
{
	::free(this->m_entryBuf);
}

void FsVisitor::run(const DirPtr &dirptr, const FilePtr &currentdir)
{
	struct dirent *result = nullptr;
	while (true) {
		auto ret = ::readdir_r(dirptr.get(), this->m_entryBuf, &result);
		if (ret != 0) {
			WARN("readdir_r error on dir: " << currentdir->getPath() <<
				 " with errno: " << errno << ". Silently ignore this error & dir stream"
				 " to reduce side-effect of traversing all the other file systems.");
			break;
		} else if (result == nullptr) {
			DEBUG("End of stream of dir: " << currentdir->getPath());
			break;
		}

		const auto &name = result->d_name;
		auto name_size = ::strlen(name);

		if (name_size == 0)
			continue;

		const auto &parent_dirpath = currentdir->getPath();
		auto fullpath = (parent_dirpath.back() == '/') ?
				(parent_dirpath + name) : (parent_dirpath + "/" + name);

		if (result->d_type == DT_DIR) {
			if ((name_size == 1 && name[0] == '.') ||
				(name_size == 2 && name[0] == '.' && name[1] == '.'))
				continue;

			auto ndirptr = openDir(fullpath);
			if (ndirptr == nullptr) {
				WARN("Failed to open dir: " << fullpath);
				continue;
			}

			FilePtr ncurrentdir;
			try {
				ncurrentdir = File::create(fullpath, currentdir);
			} catch (const Exception &e) {
				if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST) {
					WARN("Perm denied to create file on pkg path: " << fullpath);
					continue;
				} else {
					throw;
				}
			}

			if (this->m_isBasedOnName && ncurrentdir->isInApp())
				this->m_targetHandler(ncurrentdir);

			DEBUG("recurse dir : " << fullpath);
			this->run(ndirptr, ncurrentdir);
		} else if (result->d_type == DT_REG) {
			try {
				auto fileptr = File::createIfModified(fullpath, currentdir, this->m_since);

				if (fileptr)
					this->m_targetHandler(fileptr);
			} catch (const Exception &e) {
				if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST)
					WARN("file not exist: " << fullpath << " msg: " << e.what());
				else if (e.error() == CSR_ERROR_FILE_SYSTEM)
					WARN("file type is not regular...? can it be happened?"
						 " :" << fullpath << " msg: " << e.what());
				else
					throw;
			}
		}
	}
}

void FsVisitor::run()
{
	auto dirptr = openDir(this->m_path);
	auto currentdir = File::create(this->m_path, nullptr);

	INFO("Visiting files start from dir: " << this->m_path);

	this->run(dirptr, currentdir);
}

} // namespace Csr
