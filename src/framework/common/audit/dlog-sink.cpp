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
 * @file        dlog-sink.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "dlog-sink.h"

#include <iostream>
#include <dlog.h>

namespace Csr {
namespace Audit {

void DlogLogSink::sink(LogLevel level, const std::string &tag,
					   const std::string &message)
{
	switch (level) {
	case LogLevel::Error:
		SLOG(LOG_ERROR, tag.c_str(), "%s", message.c_str());
		return;

	case LogLevel::Warning:
		SLOG(LOG_WARN, tag.c_str(), "%s", message.c_str());
		return;

	case LogLevel::Debug:
		SLOG(LOG_DEBUG, tag.c_str(), "%s", message.c_str());
		return;

	case LogLevel::Info:
		SLOG(LOG_INFO, tag.c_str(), "%s", message.c_str());
		return;

	case LogLevel::Trace:
		SLOG(LOG_VERBOSE, tag.c_str(), "%s", message.c_str());
		return;

	default:
		SLOG(LOG_SILENT, tag.c_str(), "%s", message.c_str());
		return;
	}
}

}
}
