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

#include "db/connection.h"
#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {
namespace Db {

namespace {

const int BindingStartIndex = 0;
const int ColumnStartIndex = -1;

} // namespace anonymous

Statement::Statement(const Connection &db, const std::string &query) :
	m_stmt(nullptr), m_bindingIndex(BindingStartIndex)
{
	switch (sqlite3_prepare_v2(db.get(), query.c_str(), query.size(), &m_stmt,
							   nullptr)) {
	case SQLITE_OK:
		m_bindParamCount = ::sqlite3_bind_parameter_count(m_stmt);
		m_columnCount = ::sqlite3_column_count(m_stmt);

		// column index should be initialized after step(), so make it invalid here
		m_columnIndex = m_columnCount + 1;
		break;

	default:
		ThrowExc(DbFailed, db.getErrorMessage());
	}

}

Statement::~Statement()
{
	if (SQLITE_OK != ::sqlite3_finalize(m_stmt))
		ThrowExc(DbFailed, getErrorMessage());
}

void Statement::reset()
{
	clearBindings();

	if (::sqlite3_reset(m_stmt) != SQLITE_OK)
		ThrowExc(DbFailed, getErrorMessage());

	m_columnIndex = m_columnCount + 1;
}

void Statement::clearBindings() const
{
	if (::sqlite3_clear_bindings(m_stmt) != SQLITE_OK)
		ThrowExc(DbFailed, getErrorMessage());

	m_bindingIndex = BindingStartIndex;
}

std::string Statement::getErrorMessage() const noexcept
{
	return ::sqlite3_errmsg(::sqlite3_db_handle(m_stmt));
}

bool Statement::step()
{
	switch (::sqlite3_step(m_stmt)) {
	case SQLITE_ROW:
		m_columnIndex = ColumnStartIndex;
		return true;

	case SQLITE_DONE:
		// column cannot be 'get' after sqlite done, so make index overflow.
		m_columnIndex = m_columnCount + 1;
		return false;

	default:
		ThrowExc(DbFailed, getErrorMessage());
	}
}

int Statement::exec()
{
	if (::sqlite3_step(m_stmt) != SQLITE_DONE)
		ThrowExc(DbFailed, getErrorMessage());

	// column cannot be 'get' after sqlite done, so make index overflow.
	m_columnIndex = m_columnCount + 1;
	return sqlite3_changes(sqlite3_db_handle(m_stmt));
}

void Statement::bind(int value) const
{
	if (!isBindingIndexValid())
		ThrowExc(DbFailed, "index overflowed when binding int to stmt.");

	if (SQLITE_OK != ::sqlite3_bind_int(m_stmt, ++m_bindingIndex, value))
		ThrowExc(DbFailed, getErrorMessage());
}

void Statement::bind(sqlite3_int64 value) const
{
	if (!isBindingIndexValid())
		ThrowExc(DbFailed, "index overflowed when binding int64 to stmt.");

	if (SQLITE_OK != ::sqlite3_bind_int64(m_stmt, ++m_bindingIndex, value))
		ThrowExc(DbFailed, getErrorMessage());
}

void Statement::bind(const std::string &value) const
{
	if (!isBindingIndexValid())
		ThrowExc(DbFailed, "index overflowed when binding string to stmt.");

	if (SQLITE_OK != ::sqlite3_bind_text(m_stmt, ++m_bindingIndex, value.c_str(),
										 -1,
										 SQLITE_STATIC))
		ThrowExc(DbFailed, getErrorMessage());
}

void Statement::bind(void) const
{
	if (!isBindingIndexValid())
		ThrowExc(DbFailed, "index overflowed when binding fields from row");

	if (SQLITE_OK != ::sqlite3_bind_null(m_stmt, ++m_bindingIndex))
		ThrowExc(DbFailed, getErrorMessage());
}

bool Statement::isNullColumn() const
{
	if (!isColumnIndexValid())
		ThrowExc(DbFailed, "column isn't valud for index: " << m_columnIndex);

	return SQLITE_NULL == sqlite3_column_type(m_stmt, (m_columnIndex + 1));
}

int Statement::getInt() const
{
	if (!isColumnIndexValid())
		ThrowExc(DbFailed, "index overflowed when getting fields from row");

	return sqlite3_column_int(m_stmt, ++m_columnIndex);
}

sqlite3_int64 Statement::getInt64() const
{
	if (!isColumnIndexValid())
		ThrowExc(DbFailed, "index overflowed when getting fields from row");

	return sqlite3_column_int64(m_stmt, ++m_columnIndex);
}

const char *Statement::getText() const
{
	if (!isColumnIndexValid())
		ThrowExc(DbFailed, "index overflowed when getting fields from row");

	return reinterpret_cast<const char *>(sqlite3_column_text(m_stmt,
										  ++m_columnIndex));
}

} // namespace Db
} // namespace Csr
