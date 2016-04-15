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
#include "db/statement.h"

#include <string>
#include <stdexcept>

#include "db/connection.h"

namespace Csr {
namespace Db {

Statement::Statement(const Connection &db, const std::string &query) :
	m_stmt(nullptr),
	m_columnCount(0),
	m_validRow(false)
{
	if (SQLITE_OK != ::sqlite3_prepare_v2(db.get(),
										  query.c_str(),
										  query.size(),
										  &m_stmt,
										  nullptr))
		throw std::runtime_error(db.getErrorMessage());

	m_columnCount = sqlite3_column_count(m_stmt);
}

Statement::~Statement()
{
	if (::sqlite3_finalize(m_stmt) != SQLITE_OK)
		throw std::runtime_error(getErrorMessage());
}

void Statement::reset()
{
	if (::sqlite3_clear_bindings(m_stmt) != SQLITE_OK)
		throw std::runtime_error(getErrorMessage());

	if (::sqlite3_reset(m_stmt) != SQLITE_OK)
		throw std::runtime_error(getErrorMessage());
}

void Statement::clearBindings()
{
	if (::sqlite3_clear_bindings(m_stmt) != SQLITE_OK)
		throw std::runtime_error(getErrorMessage());
}

std::string Statement::getErrorMessage() const
{
	return ::sqlite3_errmsg(::sqlite3_db_handle(m_stmt));
}

std::string Statement::getErrorMessage(int errorCode) const
{
	return ::sqlite3_errstr(errorCode);
}

bool Statement::step()
{
	return (m_validRow = (SQLITE_ROW == ::sqlite3_step(m_stmt)));
}

int Statement::exec()
{
	if (SQLITE_DONE == ::sqlite3_step(m_stmt))
		m_validRow = false;

	return sqlite3_changes(sqlite3_db_handle(m_stmt));
}


void Statement::bind(int index, int value)
{
	if (SQLITE_OK != ::sqlite3_bind_int(m_stmt, index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(int index, sqlite3_int64 value)
{
	if (SQLITE_OK != ::sqlite3_bind_int64(m_stmt, index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(int index, double value)
{
	if (SQLITE_OK != ::sqlite3_bind_double(m_stmt, index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(int index, const char *value)
{
	if (SQLITE_OK != ::sqlite3_bind_text(m_stmt, index, value, -1,
										 SQLITE_TRANSIENT))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(int index, const std::string &value)
{
	if (SQLITE_OK != ::sqlite3_bind_text(m_stmt, index, value.c_str(),
										 static_cast<int>(value.size()), SQLITE_TRANSIENT))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(int index, const void *value, int size)
{
	if (SQLITE_OK != ::sqlite3_bind_blob(m_stmt, index, value, size,
										 SQLITE_TRANSIENT))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(int index)
{
	if (SQLITE_OK != ::sqlite3_bind_null(m_stmt, index))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &name, int value)
{
	int index = sqlite3_bind_parameter_index(m_stmt, name.c_str());

	if (SQLITE_OK != sqlite3_bind_int(m_stmt, index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &name, sqlite3_int64 value)
{
	int index = sqlite3_bind_parameter_index(m_stmt, name.c_str());

	if (SQLITE_OK != ::sqlite3_bind_int64(m_stmt, index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &name, double value)
{
	int index = sqlite3_bind_parameter_index(m_stmt, name.c_str());

	if (SQLITE_OK != ::sqlite3_bind_double(m_stmt, index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &name, const std::string &value)
{
	int index = sqlite3_bind_parameter_index(m_stmt, name.c_str());

	if (SQLITE_OK != ::sqlite3_bind_text(m_stmt, index, value.c_str(),
										 static_cast<int>(value.size()), SQLITE_TRANSIENT))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &name, const char *value)
{
	int index = sqlite3_bind_parameter_index(m_stmt, name.c_str());

	if (SQLITE_OK != ::sqlite3_bind_text(m_stmt, index, value, -1,
										 SQLITE_TRANSIENT))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &name, const void *value, int size)
{
	int index = sqlite3_bind_parameter_index(m_stmt, name.c_str());

	if (SQLITE_OK != ::sqlite3_bind_blob(m_stmt, index, value, size,
										 SQLITE_TRANSIENT))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &name)
{
	int index = sqlite3_bind_parameter_index(m_stmt, name.c_str());

	if (SQLITE_OK != ::sqlite3_bind_null(m_stmt, index))
		throw std::runtime_error(getErrorMessage());
}


bool Statement::isColumnValid(int index) const noexcept
{
	return m_validRow && index < m_columnCount;
}

bool Statement::isNullColumn(int index) const
{
	if (!isColumnValid(index))
		throw std::runtime_error(getErrorMessage(SQLITE_RANGE));

	return SQLITE_NULL == sqlite3_column_type(m_stmt, index);
}

std::string Statement::getColumnName(int index) const
{
	if (!isColumnValid(index))
		throw std::runtime_error(getErrorMessage(SQLITE_RANGE));

	return sqlite3_column_name(m_stmt, index);
}

int Statement::getInt(int index) const
{
	return sqlite3_column_int(m_stmt, index);
}

sqlite3_int64 Statement::getInt64(int index) const
{
	return sqlite3_column_int64(m_stmt, index);
}

double Statement::getDouble(int index) const
{
	return sqlite3_column_double(m_stmt, index);
}

const char *Statement::getText(int index) const
{
	return reinterpret_cast<const char *>(sqlite3_column_text(m_stmt, index));
}

const void *Statement::getBlob(int index) const
{
	return sqlite3_column_blob(m_stmt, index);
}

int Statement::getType(int index) const
{
	return sqlite3_column_type(m_stmt, index);
}

int Statement::getBytes(int index) const
{
	return sqlite3_column_bytes(m_stmt, index);
}

} // namespace Db
} // namespace Csr
