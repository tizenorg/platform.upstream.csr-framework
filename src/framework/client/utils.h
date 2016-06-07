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

#include <functional>

#include "client/callback.h"

#define EXCEPTION_SAFE_START return Csr::Client::exceptionGuard([&]()->int {
#define EXCEPTION_SAFE_END });

#define EXCEPTION_ASYNC_SAFE_START(callbacks, userdata) \
	return Csr::Client::exceptionGuardAsync(callbacks, userdata, [&]() {

namespace Csr {
namespace Client {

int exceptionGuard(const std::function<int()> &);

void exceptionGuardAsync(const Callback &callbacks, void *userdata,
						 const std::function<void()> &);

} // namespace Client
} // namespace Csr
