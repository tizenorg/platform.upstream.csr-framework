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
#include <queue>

#include <cstddef>
#include <ctime>
#include <dirent.h>

namespace Csr {

bool hasPermToRemove(const std::string &filepath);

class File;
using FilePtr = std::unique_ptr<File>;

class File {
public:
	enum class Type : int {
		DIRECTORY,
		FILE_REMOVABLE,
		FILE_UNREMOVABLE,
		PACKAGE_REMOVABLE,
		PACKAGE_UNREMOVABLE,
	};

	File() = delete;

	const std::string &getPath() const;
	bool isInApp() const;
	bool isDir() const;
	const std::string &getAppPkgId() const;
	const std::string &getAppUser() const;
	const std::string &getAppPkgPath() const;

	void remove() const;

	// throws FileNotExist and FileSystemError
	static FilePtr create(const std::string &fpath, time_t modifiedSince = -1);

	static bool isInApp(const std::string &path);
	static std::string getPkgPath(const std::string &path);

private:
	explicit File(const std::string &fpath, bool isDir);

	std::string m_path;
	bool m_inApp;              // in app or not
	bool m_isDir;
	Type m_type;
	std::string m_appPkgId;    // meaningful only if inApp == true
	std::string m_appUser;     // meaningful only if inApp == true
	std::string m_appPkgPath;  // meaningful only if inApp == true
};

// visitor traverses file which is modified since the time of 'modifiedSince'
// if modifiedSince is -1, traverse regardless of time
class FsVisitor;
using FsVisitorPtr = std::unique_ptr<FsVisitor>;

class FsVisitor {
public:
	~FsVisitor();

	FilePtr next();

	// throws FileNotExist and FileSystemError
	static FsVisitorPtr create(const std::string &dirpath, time_t modifiedSince = -1);

private:
	using DirPtr = std::unique_ptr<DIR, int(*)(DIR *)>;

	static DirPtr openDir(const std::string &);

	FsVisitor(const std::string &dirpath, time_t modifiedSince = -1);

	time_t m_since;
	std::queue<std::string> m_dirs;
	DirPtr m_dirptr;
	struct dirent *m_entryBuf;
};

} // namespace Csr
