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

#include "common/audit/logger.h"
#include "csr/error.h"

namespace Csr {

class Exception : public std::exception {
public:
	Exception(const char *file, const char *function, unsigned int line,
			  const std::string &message);
	virtual ~Exception() noexcept;

	virtual std::string message(void) const noexcept;

	virtual int error(void) const = 0;

protected:
	std::string m_message;
};

template <int Error = 0, Audit::LogLevel level = Audit::LogLevel::Error>
class DerivedException : public Exception {
public:
	DerivedException(const char *file, const char *function, unsigned int line,
					 const std::string &message) :
		Exception(file, function, line, message)
	{
		switch (level) {
		case Audit::LogLevel::Error:
			ERROR(message);
			break;

		case Audit::LogLevel::Warning:
			WARN(message);
			break;

		case Audit::LogLevel::Info:
			INFO(message);
			break;

		default:
			DEBUG(message);
			break;
		}
	}

	virtual ~DerivedException() noexcept {}

	virtual int error(void) const
	{
		return Error;
	}
};

using DbFailed      = DerivedException<CSR_ERROR_DB>;
using InvalidParam  = DerivedException<CSR_ERROR_INVALID_PARAMETER>;
using InternalError = DerivedException<CSR_ERROR_SERVER>;
using EngineError   = DerivedException<CSR_ERROR_ENGINE_INTERNAL>;
using SocketError   = DerivedException<CSR_ERROR_SOCKET>;

} // namespace Csr

#define ThrowExc(name, MESSAGE) \
	throw name(__FILE__, __FUNCTION__, __LINE__, FORMAT(MESSAGE))
