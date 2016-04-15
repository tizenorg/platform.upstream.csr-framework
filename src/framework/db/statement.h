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
#include <map>

#include <sqlite3.h>

#include "db/connection.h"

namespace Csr {
namespace Db {

class Statement {
public:
	Statement(const Connection &db, const std::string &query);
	virtual ~Statement();

	int exec();
	bool step();

	void reset();
	void clearBindings();

	// bind values to query
	void bind(int index, int value);
	void bind(int index, sqlite3_int64 value);
	void bind(int index, double value);
	void bind(int index, const std::string &value);
	void bind(int index, const char *value);
	void bind(int index, const void *value, int size);
	void bind(int index);

	void bind(const std::string &name, int value);
	void bind(const std::string &name, sqlite3_int64 value);
	void bind(const std::string &name, double value);
	void bind(const std::string &name, const std::string &value);
	void bind(const std::string &name, const char *value);
	void bind(const std::string &name, const void *value, int size);
	void bind(const std::string &name);

	// get column values
	std::string getColumnName(int index) const;
	bool isNullColumn(int index) const;

	int getInt(int index) const;
	sqlite3_int64 getInt64(int index) const;
	double getDouble(int index) const;
	const char *getText(int index) const;
	const void *getBlob(int index) const;
	int getType(int index) const;
	int getBytes(int index) const;

private:
	std::string getErrorMessage() const;
	std::string getErrorMessage(int errorCode) const;
	bool isColumnValid(int index) const noexcept;

	sqlite3_stmt *m_stmt;
	int m_columnCount;
	bool m_validRow;
};

} // namespace Db
} // namespace Csr
