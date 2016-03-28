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
 * @file        utils.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Utils
 */
#pragma once

#include <string>
#include <functional>

#include "common/audit/logger.h"

#define API __attribute__((visibility("default")))

#define EXCEPTION_SAFE_START return Csr::Client::exceptionGuard([&]()->int {
#define EXCEPTION_SAFE_END });

namespace Csr {
namespace Client {

inline std::string toStlString(const char *cstr)
{
	return (cstr == nullptr) ? std::string() : std::string(cstr);
}

int exceptionGuard(const std::function<int()> &);

} // namespace Client
} // namespace Csr
