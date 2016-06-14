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
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 *              Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       custom exception derived from std
 */
#pragma once

#include <exception>
#include <string>

#include <csr-error.h>

#include "common/audit/logger.h"
#include "common/macros.h"

namespace Csr {

class API Exception : public std::exception {
public:
	Exception(int ec, const char *file, const char *function, unsigned int line,
			  const std::string &message) noexcept;
	virtual ~Exception() = default;
	virtual const char *what() const noexcept final;

	virtual int error(void) const noexcept;

protected:
	int m_ec;
	std::string m_message;
};

} // namespace Csr

#define ThrowExc(ec, MESSAGE) \
	throw Csr::Exception(ec, __FILE__, __FUNCTION__, __LINE__, FORMAT(MESSAGE))
