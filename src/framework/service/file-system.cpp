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

#include <system_error>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>

#include "service/app-deleter.h"
#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {

namespace {

std::unique_ptr<struct stat> getStat(const std::string &target)
{
	std::unique_ptr<struct stat> statptr(new struct stat);
	memset(statptr.get(), 0x00, sizeof(struct stat));

	if (stat(target.c_str(), statptr.get()) != 0) {
		if (errno == ENOENT) {
			WARN("target not exist: " << target);
		} else {
			ERROR("stat() failed on target: " << target << " errno: " << errno);
		}

		return nullptr;
	}

	return statptr;
}

} // namespace anonymous

const char *APP_DIRS[4] = {
	// Tizen 2.4 app directories
	"^(/usr/apps/([^/]+))",                      // /usr/apps/{pkgid}/
	"^(/opt/usr/apps/([^/]+))",                  // /opt/usr/apps/{pkgid}/
	"^(/sdcard/apps/([^/]+))",                   // /sdcard/apps/{pkgid}/
	"^(/sdcard/app2sd/([^/]+))",                 // /sdcard/app2sd/{pkgid}/
	// Tizen 3.0 app directories
	//"^(/opt/usr/apps/([^/]+))",                  // /opt/usr/apps/{pkgid}/
	//"^(/home/([^/]+)/apps_rw/([^/]+))",          // /home/{user}/apps_rw/{pkgid}/
	//"^(/sdcard/app2sd/([^/]+)/([^/]+))",         // /sdcard/app2sd/{user}/{pkgid}/
	//"^(/sdcard/app2sd/([^/]+))",                 // /sdcard/app2sd/{pkgid}/
	//"^(/sdcard/apps/([^/]+)/apps_rw/([^/]+))"    // /sdcard/apps/{user}/apps_rw/{pkgid}/
};

//===========================================================================
// File
//===========================================================================
std::vector<std::regex> File::m_regexprs;

File::File(const std::string &fpath) : m_path(fpath), m_inApp(false)
{
	if (m_regexprs.size() == 0)
		initRegex();

	std::smatch matched;

	for (const auto &rege : m_regexprs) {
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

		m_inApp = true;
	}
}

const std::string &File::getPath() const
{
	return m_path;
}

bool File::isInApp() const
{
	return m_inApp;
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

void File::initRegex()
{
	for (unsigned int i = 0; i < sizeof(APP_DIRS) / sizeof(char *); i++) {
		std::regex regexpr(APP_DIRS[i], std::regex_constants::extended);
		m_regexprs.emplace_back(std::move(regexpr));
	}
}

bool File::remove()
{
	if (m_inApp)
		return AppDeleter::remove(m_appPkgId);
	else
		return ::remove(m_path.c_str()) == 0;
}

FilePtr File::create(const std::string &fpath, time_t modifiedSince)
{
	auto statptr = getStat(fpath);
	if (statptr == nullptr) {
		ThrowExc(FileDoNotExist, "file not exist: " << fpath);
	} else if (!S_ISREG(statptr->st_mode)) {
		ThrowExc(FileSystemError, "file type is not reguler: " << fpath);
	} else if (modifiedSince != -1 && statptr->st_mtim.tv_sec < modifiedSince) {
		DEBUG("file[" << fpath << "] isn't modified since[" << modifiedSince << "]");
		return nullptr;
	} else {
		return FilePtr(new File(fpath));
	}
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

				if (fileptr)
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
