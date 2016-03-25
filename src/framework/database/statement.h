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

#ifndef __STATEMENT__
#define __STATEMENT__

#include <string>
#include <map>

#include <sqlite3.h>

namespace Csr {

namespace Database {

class Column;
class Connection;

class Statement {
public:
	Statement(const Connection &db, const std::string &query);
	virtual ~Statement();

	int exec();
	bool step();

	void reset();
	void clearBindings();
	std::string getErrorMessage() const;
	std::string getErrorMessage(int errorCode) const;

	Column getColumn(const int index);
	std::string getColumnName(const int index) const;
	bool isNullColumn(const int index) const;

	void bind(const int index, const int &value);
	void bind(const int index, const sqlite3_int64 &value);
	void bind(const int index, const double &value);
	void bind(const int index, const std::string &value);
	void bind(const int index, const char *value);
	void bind(const int index, const void *value, const int size);
	void bind(const int index);

	void bind(const std::string &name, const int &value);
	void bind(const std::string &name, const sqlite3_int64 &value);
	void bind(const std::string &name, const double &value);
	void bind(const std::string &name, const std::string &value);
	void bind(const std::string &name, const char *value);
	void bind(const std::string &name, const void *value, const int size);
	void bind(const std::string &name);

	inline int getColumnCount() const {
		return columnCount;
	}

	inline sqlite3_stmt *get() const noexcept {
		return statement;
	}

private:
	typedef std::map<std::string, int> ColumnMap;

	sqlite3_stmt *statement;
	int columnCount;
	int validRow;
	ColumnMap columnNames;
};

} // namespace Database

} // namespace Csr
#endif //__STATEMENT__
