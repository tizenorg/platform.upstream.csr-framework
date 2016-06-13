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
 * @file       iloader.cpp
 * @author     Kyungwook Tak (k.tak@samsung.com)
 * @version    1.0
 * @brief      engine loader abstract class
 */
#include "service/iloader.h"

#include <stdexcept>

#include <csre-error.h>

#include "common/exception.h"

namespace Csr {

void ILoader::toException(int ee, bool internalErrorToString)
{
	switch (ee) {
	case CSRE_ERROR_NONE:
		return;

	case CSRE_ERROR_OUT_OF_MEMORY:
		throw std::bad_alloc();

	case CSRE_ERROR_PERMISSION_DENIED:
		ThrowExc(CSR_ERROR_ENGINE_PERMISSION, "access denied related to engine");

	case CSRE_ERROR_FILE_NOT_FOUND:
		ThrowExc(CSR_ERROR_FILE_DO_NOT_EXIST, "file not found.");

	case CSRE_ERROR_ENGINE_NOT_ACTIVATED:
		ThrowExc(CSR_ERROR_ENGINE_NOT_ACTIVATED, "engine is not activated yet");

	default:
		if (internalErrorToString) {
			std::string errstr;
			try {
				errstr = this->getErrorString(ee);
			} catch (...) {
				ERROR("exception from engine loader's getErrorString.");
				ThrowExc(CSR_ERROR_ENGINE_INTERNAL, "engine internal error. Failed to "
						 "get errno string... engine error code: " << ee);
			}

			ThrowExc(CSR_ERROR_ENGINE_INTERNAL, "engine internal error: " << errstr);
		} else {
			ThrowExc(CSR_ERROR_ENGINE_INTERNAL, "engine internal error. ec: " << ee);
		}
	}
}

}
