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
#include <iostream>
#include <stdexcept>

#include "db/connection.h"

namespace Csr {
namespace Db {

Connection::Connection(const std::string &name, const int flags) :
	m_handle(nullptr),
	m_filename(name)
{
	if (::sqlite3_open_v2(m_filename.c_str(), &m_handle, flags, nullptr))
		throw std::runtime_error(getErrorMessage());
}

Connection::~Connection()
{
	sqlite3_close(m_handle);
}

int Connection::exec(const std::string &query)
{
	if (::sqlite3_exec(m_handle, query.c_str(), nullptr, nullptr,
					   nullptr) != SQLITE_OK)
		throw std::runtime_error(getErrorMessage());

	return sqlite3_changes(m_handle);
}

} // namespace Db
} // namespace Csr
