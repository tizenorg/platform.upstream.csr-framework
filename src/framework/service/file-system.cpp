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

bool isModifiedSince(const std::string &path, time_t since)
{
	struct stat s;
	memset(&s, 0x00, sizeof(s));

	if (stat(path.c_str(), &s) != 0)
		ThrowExc(InternalError, "Failed to stat() on file: " << path <<
				 ". errno: " << errno);

	DEBUG("Modified since called with file: " << path <<
		  " file mtime: " << s.st_mtim.tv_sec <<
		  " since: " << since);

	return s.st_mtim.tv_sec > since;
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

File::File(const std::string &fpath, time_t modifiedSince) :
	m_path(fpath), m_inApp(false),
	m_isModified((modifiedSince != -1 && isModifiedSince(fpath, modifiedSince)))
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

bool File::isModified() const
{
	return m_isModified;
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
	return FilePtr(new File(fpath, modifiedSince));
}

FsVisitor::DirPtr FsVisitor::openDir(const std::string &dir)
{
	return std::unique_ptr<DIR, int(*)(DIR *)>(::opendir(dir.c_str()), ::closedir);
}

FsVisitorPtr FsVisitor::create(const std::string &dirpath, time_t modifiedSince)
{
	struct stat s;
	memset(&s, 0x00, sizeof(s));

	if (stat(dirpath.c_str(), &s) != 0) {
		if (errno == ENOENT) {
			WARN("directory not exist: " << dirpath);
		} else {
			// TODO: throw exception? can we trust errno value?
			ERROR("stat() failed with file[" << dirpath << "]. errno: " << errno);
		}

		return nullptr;
	} else if (!S_ISDIR(s.st_mode)) {
		INFO("File type is not directory: " << dirpath);
		return nullptr;
	} else {
		DEBUG("File type is directory: " << dirpath);
		return FsVisitorPtr(new FsVisitor(dirpath, modifiedSince));
	}
}

FsVisitor::FsVisitor(const std::string &dirpath, time_t modifiedSince) :
	m_since(modifiedSince),
	m_currDir(openDir(dirpath)),
	m_currEntry(static_cast<struct dirent *>(::malloc(
			offsetof(struct dirent, d_name) + NAME_MAX + 1)), ::free)
{
	if (!m_currDir)
		ThrowExc(InternalError, "Failed to open dir: " << dirpath);

	m_dirs.push((dirpath.back() == '/') ? dirpath : (dirpath + '/'));
}

FilePtr FsVisitor::next()
{
	struct dirent *result = nullptr;
	while (readdir_r(m_currDir.get(), m_currEntry.get(), &result) == 0) {
		if (result == nullptr) { // end of dir stream
			m_dirs.pop();
			while (!m_dirs.empty() && !(m_currDir = openDir(m_dirs.front()))) {
				m_dirs.pop();
			}

			if (m_dirs.empty())
				return nullptr;
			else
				continue;
		}

		auto &dir = m_dirs.front();
		std::string filepath(result->d_name);

		if (result->d_type == DT_DIR) {
			if (filepath.compare(".") == 0 || filepath.compare("..") == 0)
				continue;
			else
				m_dirs.emplace((filepath.back() == '/') ?
					(dir + filepath) : (dir + filepath + '/'));
		} else if (result->d_type == DT_REG) {
			auto fileptr = File::create(dir + filepath, m_since);

			if (!fileptr)
				continue;
			else
				return fileptr;
		}
	}

	throw std::system_error(std::error_code(), FORMAT("reading dir: " << m_dirs.front()));
}

} // namespace Csr
