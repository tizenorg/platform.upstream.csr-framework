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

#include "db/connection.h"

namespace Csr {
namespace Db {

class Statement {
public:
	Statement() = delete;
	explicit Statement(const Connection &db, const std::string &query);
	virtual ~Statement();

	int exec();
	bool step();

	void reset();
	void clearBindings() const;

	// bind values to query. index of column auto-incremented
	void bind(int value) const;
	void bind(sqlite3_int64 value) const;
	void bind(double value) const;
	void bind(const std::string &value) const;
	void bind() const;

	// get column values. index of column auto-incremented
	int getInt() const;
	sqlite3_int64 getInt64() const;
	const char *getText() const;

	bool isNullColumn() const; // it's checking func. not auto incremented.

private:
	inline bool isBindingIndexValid() const noexcept
	{
		return m_bindingIndex <= m_bindParamCount;
	}

	inline bool isColumnIndexValid() const noexcept
	{
		return m_columnIndex < m_columnCount;
	}

	std::string getErrorMessage() const noexcept;

	sqlite3_stmt *m_stmt;
	int m_bindParamCount;
	int m_columnCount;
	mutable int m_bindingIndex;
	mutable int m_columnIndex;
};

} // namespace Db
} // namespace Csr
