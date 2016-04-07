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

#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "service/app-deleter.h"
#include "common/audit/logger.h"

namespace Csr {

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

File::File(const std::string &fpath)
	: m_path(fpath), m_inApp(false)
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

File::File(const std::string &fpath, bool belongToApp,
		   const std::string &pkgId, const std::string &user, const std::string &pkgPath)
	: m_path(fpath), m_inApp(belongToApp), m_appPkgId(pkgId), m_appUser(user),
	  m_appPkgPath(pkgPath)
{
}

File::~File()
{
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

//===========================================================================
// FileVisitor
//===========================================================================
FsVisitorShrPtr createVisitor(const std::string &fpath, time_t modifiedSince)
{
	struct stat s;
	memset(&s, 0x00, sizeof(s));
	int ret = stat(fpath.c_str(), &s);

	if (ret != 0) {
		if (errno == ENOENT)
			WARN("file[" << fpath << "] not exist!");
		else {
			// TODO: throw exception? can we trust errno value?
			ERROR("stat() failed with file[" << fpath << "]. errno: " << errno);
		}

		return nullptr;
	}

	if (S_ISDIR(s.st_mode)) {
		DEBUG("File type is directory[" << fpath << "]");
		return FsVisitorShrPtr(new DirVisitor(fpath, modifiedSince));
	} else if (S_ISREG(s.st_mode)) {
		DEBUG("File type is regular[" << fpath << "]");
		return FsVisitorShrPtr(new FileVisitor(fpath, modifiedSince));
	} else {
		INFO("File type is unknown[" << fpath << "]");
		return nullptr;
	}
}

FileSystemVisitor::FileSystemVisitor()
{
}

FileSystemVisitor::~FileSystemVisitor()
{
}

bool FileSystemVisitor::isModifiedSince(const std::string &path, time_t since)
{
	struct stat s;

	if (stat(path.c_str(), &s) != 0)
		return false;
	else
		return s.st_mtim.tv_sec >= since;
}

FileVisitor::FileVisitor(const std::string &fpath, time_t modifiedSince) :
	m_path(fpath), m_since(modifiedSince), m_nextItem(std::make_shared<File>(fpath))
{
}

FileVisitor::~FileVisitor()
{
}

FileShrPtr FileVisitor::next()
{
	if (m_nextItem && !FileSystemVisitor::isModifiedSince(m_path, m_since))
		m_nextItem.reset();

	FileShrPtr item = m_nextItem;
	m_nextItem.reset();

	return item;
}

DirVisitor::DirVisitor(const std::string &fpath, time_t modifiedSince)
	: m_path(fpath), m_since(modifiedSince), m_currDir(nullptr),
	  m_currEntry(nullptr)
{
	m_dirs.push(m_path);
}


DirVisitor::~DirVisitor()
{
}

FileShrPtr DirVisitor::next()
{
	struct dirent *result;

	if (!m_currDir) { // no current dir set
		if (m_dirs.empty()) // traversed all directories.
			return nullptr;

		std::unique_ptr<DIR, std::function<int(DIR *)>> dirp(
					::opendir(m_dirs.front().c_str()), ::closedir);

		if (!dirp) { // fail to open due to no permission
			DEBUG("DirVisitor: cannot visit(may be due to permission). dir=" <<
				  m_dirs.front());
			m_dirs.pop(); // remove front
			return next();
		}

		size_t len = offsetof(struct dirent, d_name) + NAME_MAX + 1;
		std::unique_ptr<struct dirent, std::function<void(void *)>> pEntry(
					static_cast<struct dirent *>(::malloc(len)), ::free);

		m_currDir = std::move(dirp);
		m_currEntry = std::move(pEntry);
		DEBUG("DirVisitor: start visiting. dir=" << m_dirs.front());
	}

	while (true) {
		if (readdir_r(m_currDir.get(), m_currEntry.get(), &result) != 0)
			throw std::runtime_error("exception occurred. reading dir = " + m_dirs.front());

		if (result == nullptr) { // end of dir stream
			DEBUG("DirVisitor: end visiting. dir=" << m_dirs.front());
			m_currDir.reset();
			m_currEntry.reset();
			m_dirs.pop(); // remove front
			return next();
		}

		std::string fullPath;

		if (m_dirs.front().size() > 0 &&
				m_dirs.front().at(m_dirs.front().size() - 1) == '/') {
			fullPath = m_dirs.front() + result->d_name;
		} else {
			fullPath = m_dirs.front() + "/" + result->d_name;
		}

		if (result->d_type ==  DT_DIR) {
			if (strcmp(result->d_name, ".") != 0 && strcmp(result->d_name, "..") != 0) {
				m_dirs.push(fullPath);
			}

			continue;
		}

		if (result->d_type ==  DT_REG) {
			// check modified time
			if (!FileSystemVisitor::isModifiedSince(fullPath, m_since))
				continue;

			return std::make_shared<File>(fullPath);
		}
	}

	return nullptr;
}

} // namespace Csr
