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
 * @file        engine-error-converter.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       convert engine error to custom exception
 */
#include "service/engine-error-converter.h"

#include "csre/error.h"

namespace Csr {

void toException(int ee)
{
	switch (ee) {
	case CSRE_ERROR_NONE:
		return;

	case CSRE_ERROR_OUT_OF_MEMORY:
		throw std::bad_alloc();

	case CSRE_ERROR_PERMISSION_DENIED:
		ThrowExc(PermDenied, "access denied related to engine");

	case CSRE_ERROR_FILE_NOT_FOUND:
		ThrowExc(FileDoNotExist, "file not found.");

	case CSRE_ERROR_ENGINE_NOT_ACTIVATED:
		ThrowExc(EngineNotActivated, "engine is not activated yet");

	default:
		ThrowExc(EngineError, "engine internal error. ec: " << ee);
	}
}

}
