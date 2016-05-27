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
 * @file        logger.cpp
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 * @version     1.0
 * @brief
 */
#include "common/audit/logger.h"

#include <sys/types.h>
#include <unistd.h>

#include <stdexcept>

#include "common/audit/log-sink.h"
#include "common/audit/console-sink.h"
#include "common/audit/dlog-sink.h"

namespace Csr {
namespace Audit {

//std::unique_ptr<LogSink> g_backend(new ConsoleLogSink());
std::unique_ptr<LogSink> g_backend(new DlogLogSink());
std::string g_tag("CSR");

namespace {

std::string toString(LogLevel level)
{
	switch (level) {
	case LogLevel::Error:
		return "ERROR";

	case LogLevel::Warning:
		return "WARN";

	case LogLevel::Debug:
		return "DEBUG";

	case LogLevel::Info:
		return "INFO";

	case LogLevel::Trace:
		return "TRACE";

	default:
		return "UNKNOWN";
	}
}

} // namespace anonymous

void Logger::log(LogLevel level,
				 const std::string &file,
				 unsigned int line,
				 const std::string &func,
				 const std::string &message)
{
	std::ostringstream buffer;

	buffer << toString(level)
		   << "<" << ::getpid() << ">:"
		   << "[" << file << ":" << line << "]"
		   << " " << func << "() " << message
		   << std::endl;

	g_backend->sink(level, g_tag, buffer.str());
}

void Logger::setTag(const std::string &tag) noexcept
{
	g_tag = tag;
}

}
}
