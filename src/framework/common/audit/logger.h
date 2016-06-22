/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        logger.h
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <ios>

#include <cstring>

#include "common/macros.h"

namespace Csr {
namespace Audit {

enum class LogLevel : int {
	Error,
	Warning,
	Info,
	Debug,
	Trace
};

class API Logger {
public:
	static void log(LogLevel level,
					const std::string &file,
					unsigned int line,
					const std::string &func,
					const std::string &message);
	static void setTag(const std::string &tag) noexcept;
};

}
}

#ifndef __FILENAME__
#define __FILENAME__ (::strrchr(__FILE__, '/') ? ::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define FORMAT(ITEMS) \
	(static_cast<std::ostringstream &>(std::ostringstream() << ITEMS)).str()

#define LOG(LEVEL, MESSAGE) Csr::Audit::Logger::log(              \
		LEVEL, __FILENAME__, __LINE__, __func__, FORMAT(MESSAGE)) \

#define ERROR(MESSAGE) LOG(Csr::Audit::LogLevel::Error, MESSAGE)
#define WARN(MESSAGE)  LOG(Csr::Audit::LogLevel::Warning, MESSAGE)
#define INFO(MESSAGE)  LOG(Csr::Audit::LogLevel::Info, MESSAGE)

#if !defined(NDEBUG)
#define DEBUG(MESSAGE) LOG(Csr::Audit::LogLevel::Debug, MESSAGE)
#define TRACE(MESSAGE) LOG(Csr::Audit::LogLevel::Trace, MESSAGE)
#else
#define DEBUG(MESSAGE) do {} while (false)
#define TRACE(MESSAGE) do {} while (false)
#endif //NDEBUG
