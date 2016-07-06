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
 * @file        exception.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       exception guard and custom exceptions which are thrown
 *              only on server side
 */
#include "service/exception.h"

#include <exception>

#include "common/audit/logger.h"
#include "common/binary-queue.h"

#include <csr-error.h>

namespace Csr {

RawBuffer exceptionGuard(const std::function<RawBuffer()> &func)
{
	try {
		return func();
	} catch (const Exception &e) {
		if (e.error() == CSR_ERROR_SOCKET)
			WARN("Socket error. Client might cancel async scan or crashed: " << e.what());
		else
			ERROR("Exception caught. code: " << e.error() << " message: " << e.what());
		return BinaryQueue::Serialize(e.error()).pop();
	} catch (const std::invalid_argument &e) {
		ERROR("Invalid argument: " << e.what());
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER).pop();
	} catch (const std::bad_alloc &e) {
		ERROR("memory alloc failed: " << e.what());
		return BinaryQueue::Serialize(CSR_ERROR_OUT_OF_MEMORY).pop();
	} catch (const std::exception &e) {
		ERROR("std exception: " << e.what());
		return BinaryQueue::Serialize(CSR_ERROR_SYSTEM).pop();
	} catch (...) {
		ERROR("Unknown exception occured in logic");
		return BinaryQueue::Serialize(CSR_ERROR_SYSTEM).pop();
	}
}

}
