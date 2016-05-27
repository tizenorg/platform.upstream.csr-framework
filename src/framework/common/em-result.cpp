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
 * @file        em-result.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Engine manager result set
 */
#include "common/em-result.h"

namespace Csr {

EmString::EmString() {}
EmString::~EmString() {}

EmString::EmString(IStream &stream)
{
	Deserializer<std::string>::Deserialize(stream, this->value);
}

void EmString::Serialize(IStream &stream) const
{
	Serializer<std::string>::Serialize(stream, this->value);
}

}