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
#include "common/audit/logger.h"

namespace Csr {
namespace Db {

Statement::Statement(const Connection &db, const std::string &query) :
	m_stmt(nullptr), m_index(0), m_validRow(false)
{
	// m_index from 1 when bind (0 because ++m_index at first)

	if (SQLITE_OK != ::sqlite3_prepare_v2(db.get(),
										  query.c_str(),
										  query.size(),
										  &m_stmt,
										  nullptr))
		throw std::runtime_error(db.getErrorMessage());

	m_count = ::sqlite3_bind_parameter_count(m_stmt);
}

Statement::~Statement()
{
	if (SQLITE_OK != ::sqlite3_finalize(m_stmt))
		throw std::runtime_error(getErrorMessage());
}

void Statement::reset()
{
	clearBindings();
	m_index = 0;

	if (::sqlite3_reset(m_stmt) != SQLITE_OK)
		throw std::runtime_error(getErrorMessage());
}

void Statement::clearBindings() const
{
	if (::sqlite3_clear_bindings(m_stmt) != SQLITE_OK)
		throw std::runtime_error(getErrorMessage());
}

std::string Statement::getErrorMessage() const
{
	return ::sqlite3_errmsg(::sqlite3_db_handle(m_stmt));
}

bool Statement::step()
{
	auto ret = ::sqlite3_step(m_stmt);

	if (ret != SQLITE_ROW && ret != SQLITE_DONE)
		throw std::runtime_error(getErrorMessage());

	// index from 0 when get (-1 because ++m_index at first)
	m_index = -1;
	m_count = ::sqlite3_column_count(m_stmt);

	return (m_validRow = (ret == SQLITE_ROW));
}

int Statement::exec()
{
	if (::sqlite3_step(m_stmt) == SQLITE_DONE) {
		m_validRow = false;
		m_index = 0;
	}

	return sqlite3_changes(sqlite3_db_handle(m_stmt));
}

void Statement::bind(int value) const
{
	if (m_index > m_count)
		throw std::logic_error("index overflowed when binding int to stmt.");

	if (SQLITE_OK != ::sqlite3_bind_int(m_stmt, ++m_index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(sqlite3_int64 value) const
{
	if (m_index > m_count)
		throw std::logic_error("index overflowed when binding int64 to stmt.");

	if (SQLITE_OK != ::sqlite3_bind_int64(m_stmt, ++m_index, value))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(const std::string &value) const
{
	if (m_index > m_count)
		throw std::logic_error("index overflowed when binding string to stmt.");

	if (SQLITE_OK != ::sqlite3_bind_text(m_stmt, ++m_index, value.c_str(), -1,
										 SQLITE_STATIC))
		throw std::runtime_error(getErrorMessage());
}

void Statement::bind(void) const
{
	if (m_index > m_count)
		throw std::logic_error("index overflowed when binding fields from row");

	if (SQLITE_OK != ::sqlite3_bind_null(m_stmt, ++m_index))
		throw std::runtime_error(getErrorMessage());
}

bool Statement::isColumnValid() const noexcept
{
	return m_validRow && m_index < m_count;
}

bool Statement::isNullColumn() const
{
	if (!isColumnValid())
		throw std::runtime_error(FORMAT("column isn't valud for index: " << m_index));

	return SQLITE_NULL == sqlite3_column_type(m_stmt, (m_index + 1));
}

int Statement::getInt() const
{
	if (!isColumnValid())
		throw std::logic_error("index overflowed when getting fields from row");

	return sqlite3_column_int(m_stmt, ++m_index);
}

sqlite3_int64 Statement::getInt64() const
{
	if (!isColumnValid())
		throw std::logic_error("index overflowed when getting fields from row");

	return sqlite3_column_int64(m_stmt, ++m_index);
}

const char *Statement::getText() const
{
	if (!isColumnValid())
		throw std::logic_error("index overflowed when getting fields from row");

	return reinterpret_cast<const char *>(sqlite3_column_text(m_stmt, ++m_index));
}

} // namespace Db
} // namespace Csr
