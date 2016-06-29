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
#include <functional>

#include <cstddef>
#include <ctime>
#include <dirent.h>
#include <sys/stat.h>

namespace Csr {

class File;
using FilePtr = std::unique_ptr<File>;

class File {
public:
	File() = delete;

	inline bool isInApp() const noexcept
	{
		return this->isPackage() && !this->isPreloaded();
	}

	inline bool isPackage() const noexcept
	{
		return this->m_type & static_cast<int>(Type::Package);
	}

	inline bool isPreloaded() const noexcept
	{
		return this->m_type & static_cast<int>(Type::PreLoaded);
	}

	inline bool isModified() const noexcept
	{
		return this->m_type & static_cast<int>(Type::Modified);
	}

	inline bool isModifiedSince(time_t since) const noexcept
	{
		return this->m_statptr->st_ctime > since;
	}

	inline bool isDir() const noexcept
	{
		return this->m_type & static_cast<int>(Type::Directory);
	}

	inline const std::string &getName() const noexcept
	{
		return (this->isInApp()) ? this->m_appPkgPath : this->m_path;
	}

	inline const std::string &getPath() const noexcept
	{
		return this->m_path;
	}

	inline const std::string &getAppPkgId() const noexcept
	{
		return this->m_appPkgId;
	}

	inline const std::string &getAppUser() const noexcept
	{
		return this->m_appUser;
	}

	inline const std::string &getAppPkgPath() const noexcept
	{
		return this->m_appPkgPath;
	}

	void remove() const;

	// throws FileNotExist and FileSystemError
	static FilePtr create(const std::string &fpath, const FilePtr &parentdir,
						  time_t modifiedSince = -1);
	static FilePtr createIfModified(const std::string &fpath, const FilePtr &parentdir,
									time_t modifiedSince = -1);

	static std::string getPkgPath(const std::string &path);

private:
	enum class Type : int {
		Modified  = (1 << 0),
		Package   = (1 << 1),
		PreLoaded = (1 << 2),
		File      = (1 << 3),
		Directory = (1 << 4)
	};

	static FilePtr createInternal(const std::string &fpath, const FilePtr &parentdir,
								  time_t modifiedSince, bool isModifiedOnly);
	static int getPkgTypes(const std::string &user, const std::string &pkgid);

	explicit File(const std::string &fpath, const FilePtr &parentdir, int type,
				  std::unique_ptr<struct stat> &&statptr);

	std::string m_path;
	int m_type;
	std::unique_ptr<struct stat> m_statptr;
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
	using TargetHandler = std::function<void(FilePtr &)>;

	~FsVisitor();

	void run();

	// throws FileNotExist and FileSystemError
	static FsVisitorPtr create(TargetHandler &&targetHandler,
							   const std::string &dirpath, bool isBasedOnName,
							   time_t modifiedSince = -1);

private:
	using DirPtr = std::unique_ptr<DIR, int(*)(DIR *)>;

	void run(const DirPtr &dirptr, const FilePtr &currentdir);

	static DirPtr openDir(const std::string &);

	FsVisitor(TargetHandler &&targetHandler, const std::string &dirpath,
			  bool isBasedOnName, time_t modifiedSince = -1);

	TargetHandler m_targetHandler;
	std::string m_path;
	time_t m_since;
	bool m_isDone;
	bool m_isBasedOnName;
	struct dirent *m_entryBuf;
};

} // namespace Csr
