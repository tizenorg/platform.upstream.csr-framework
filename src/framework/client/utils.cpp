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
 * @file        utils.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Utils
 */
#include "client/utils.h"

#include <exception>

#include "common/audit/logger.h"
#include "common/exception.h"
#include <csr-error.h>

__attribute__((constructor))
static void init_lib(void)
{
	Csr::Audit::Logger::setTag("CSR_CLIENT");
}

namespace Csr {
namespace Client {

int exceptionGuard(const std::function<int()> &func)
{
	try {
		return func();
	} catch (const Exception &e) {
		ERROR("Exception caught. code: " << e.error() << " message: " << e.what());
		return e.error();
	} catch (const std::invalid_argument &e) {
		ERROR("invalid argument: " << e.what());
		return CSR_ERROR_INVALID_PARAMETER;
	} catch (const std::bad_alloc &e) {
		ERROR("memory allocation failed: " << e.what());
		return CSR_ERROR_OUT_OF_MEMORY;
	} catch (const std::exception &e) {
		ERROR("std exception: " << e.what());
		return CSR_ERROR_UNKNOWN;
	} catch (...) {
		ERROR("Unknown exception occured!");
		return CSR_ERROR_UNKNOWN;
	}
}

}
}
