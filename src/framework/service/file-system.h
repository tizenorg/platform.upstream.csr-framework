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

#include <cstddef>
#include <ctime>
#include <dirent.h>

namespace Csr {

class File;
using FilePtr = std::unique_ptr<File>;

class File {
public:
	const std::string &getPath() const;
	bool isInApp() const;
	const std::string &getAppPkgId() const;
	const std::string &getAppUser() const;
	const std::string &getAppPkgPath() const;
	bool isModified() const;

	bool remove();

	static FilePtr create(const std::string &fpath, time_t modifiedSince = -1);

private:
	static void initRegex();

	static std::vector<std::regex> m_regexprs;

	File(const std::string &fpath, time_t modifiedSince = -1);

	std::string m_path;
	bool m_inApp;              // in app or not
	std::string m_appPkgId;    // meaningful only if inApp == true
	std::string m_appUser;     // meaningful only if inApp == true
	std::string m_appPkgPath;  // meaningful only if inApp == true
	bool m_isModified;
};

// visitor traverses file which is modified since the time of 'modifiedSince'
// if modifiedSince is -1, traverse regardless of time
class FsVisitor;
using FsVisitorPtr = std::unique_ptr<FsVisitor>;

class FsVisitor {
public:
	FilePtr next();

	static FsVisitorPtr create(const std::string &dirpath, time_t modifiedSince = -1);

private:
	using DirPtr = std::unique_ptr<DIR, int(*)(DIR *)>;
	using EntryPtr = std::unique_ptr<struct dirent, void(*)(void *)>;

	static DirPtr openDir(const std::string &);

	FsVisitor(const std::string &dirpath, time_t modifiedSince = -1);

	time_t m_since;
	std::queue<std::string> m_dirs;
	DirPtr m_currDir;
	EntryPtr m_currEntry;
};

} // namespace Csr
