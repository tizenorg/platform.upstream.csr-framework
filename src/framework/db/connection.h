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
#pragma once

#include <string>

#include <sqlite3.h>

namespace Csr {
namespace Db {

class Connection {
public:
	enum Mode {
		Create = SQLITE_OPEN_CREATE,
		ReadWrite = SQLITE_OPEN_READWRITE,
		MultiThread = SQLITE_OPEN_NOMUTEX,
		Serialized = SQLITE_OPEN_FULLMUTEX
	};

	Connection(const std::string &name, const int flags);
	virtual ~Connection();

	int exec(const std::string &query);

	inline long long getLastInsertRowId() const noexcept
	{
		return sqlite3_last_insert_rowid(m_handle);
	}

	inline const std::string &getName() const noexcept
	{
		return m_filename;
	}

	inline int getErrorCode() const
	{
		return sqlite3_errcode(m_handle);
	}

	inline int getExtendedErrorCode() const
	{
		return sqlite3_extended_errcode(m_handle);
	}

	inline std::string getErrorMessage() const
	{
		return sqlite3_errmsg(m_handle);
	}

	inline sqlite3 *get() const noexcept
	{
		return m_handle;
	}

private:
	sqlite3 *m_handle;
	std::string m_filename;
};

} // namespace Db
} // namespace Csr
