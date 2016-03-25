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


#include "database/connection.h"

namespace Csr {

namespace Database {

Connection::Connection(const std::string &name, const int flags)
	: handle(nullptr),
	  filename(name)
{
	if (::sqlite3_open_v2(filename.c_str(), &handle, flags, NULL)) {
		throw std::runtime_error(getErrorMessage());
	}
}

Connection::~Connection()
{
	sqlite3_close(handle);
}

int Connection::exec(const std::string &query)
{
	if (::sqlite3_exec(handle, query.c_str(), NULL, NULL, NULL) != SQLITE_OK) {
		throw std::runtime_error(getErrorMessage());
	}

	return sqlite3_changes(handle);
}

} // namespace Database

} // namespace Csr
