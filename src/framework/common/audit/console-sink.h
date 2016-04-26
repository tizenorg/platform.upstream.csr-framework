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
 * @file        console-sink.h
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>

#include "common/audit/log-sink.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Audit {

class ConsoleLogSink : public LogSink {
public:
	void sink(LogLevel level, const std::string &tag,
			  const std::string &message) override;
};

}
}
