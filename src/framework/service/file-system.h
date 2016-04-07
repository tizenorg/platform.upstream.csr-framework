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
 * @file        file-system.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>
#include <memory>
#include <regex>
#include <queue>

#include <dirent.h>
#include <stddef.h>
#include <time.h>

namespace Csr {

class File {
public:
	File(const std::string &fpath);
	File(const std::string &fpath, bool belongToApp,
		 const std::string &pkgId, const std::string &user, const std::string &pkgPath);
	virtual ~File();

	const std::string &getPath() const;
	bool isInApp() const;
	const std::string &getAppPkgId() const;
	const std::string &getAppUser() const;
	const std::string &getAppPkgPath() const;

	bool remove();

private:
	static void initRegex();

	static std::vector<std::regex> m_regexprs;

	std::string m_path;
	bool m_inApp;              // in app or not
	std::string m_appPkgId;    // meaningful only if inApp == true
	std::string m_appUser;     // meaningful only if inApp == true
	std::string m_appPkgPath;  // meaningful only if inApp == true
};

class FileSystemVisitor;
using FileShrPtr = std::shared_ptr<File>;
using FsVisitorShrPtr = std::shared_ptr<FileSystemVisitor>;

FsVisitorShrPtr createVisitor(const std::string &fpath, time_t modifiedSince);

class FileSystemVisitor {
public:
	virtual ~FileSystemVisitor() = 0;

	// returns nullprt when there is no next element.
	virtual FileShrPtr next() = 0;

protected:
	FileSystemVisitor();

	// return true if a file was modified since "since" parameter.
	static bool isModifiedSince(const std::string &path, time_t since);
};

class FileVisitor : public FileSystemVisitor {
public:
	virtual ~FileVisitor();
	virtual FileShrPtr next() override;

private:
	friend FsVisitorShrPtr createVisitor(const std::string &, time_t);

	FileVisitor(const std::string &fpath, time_t modifiedSince);

	std::string m_path;
	time_t m_since;
	FileShrPtr m_nextItem;
};

class DirVisitor : public FileSystemVisitor {
public:
	virtual ~DirVisitor();
	virtual FileShrPtr next() override;

private:
	friend FsVisitorShrPtr createVisitor(const std::string &, time_t);

	DirVisitor(const std::string &fpath, time_t modifiedSince);

	std::string m_path;
	time_t m_since;
	std::queue<std::string> m_dirs;
	std::unique_ptr<DIR, std::function<int(DIR *)>> m_currDir;
	std::unique_ptr<struct dirent, std::function<void(void *)>> m_currEntry;
};

} // namespace Csr
