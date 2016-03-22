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
 * @file        client-common.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       client common for both of cs / wp
 */
#pragma once

#include <functional>
#include <string>

#include "csr/error.h"
#include "common/message-buffer.h"


namespace Csr {
namespace Client {

/* Encode parameters which is Command ID specific */
using Encoder = std::function<Csr::MessageBuffer(void)>;

/* Decode parameters which is Command ID specific */
using Decoder = std::function<csr_error_e(Csr::MessageBuffer &&)>;

inline std::string toStlString(const char *cstr)
{
	return (cstr == nullptr) ? std::string() : std::string(cstr);
}

/* post request to server with given encoder/decoder
 * If decoder isn't assigned, default decoder do only deserialize
 * return code(csr_error_e) from message buffer and return it */
int post(Encoder &&encoder, Decoder &&decoder = nullptr);

}
}
