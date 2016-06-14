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
 * @file        exception.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       custom exception derived from std
 */
#include "common/exception.h"

#include "common/audit/logger.h"

namespace Csr {

Exception::Exception(int ec, const char *file, const char *function, unsigned int line,
					 const std::string &message) noexcept :
	m_ec(ec),
	m_message(FORMAT("[" << file << ":" << line << " " << function << "()]" << message))
{
	ERROR(this->m_message);
}

const char *Exception::what() const noexcept
{
	return this->m_message.c_str();
}

int Exception::error(void) const noexcept
{
	return this->m_ec;
}

}
