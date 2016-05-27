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
 * @file        em-context.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Engine manager context
 */
#include "common/em-context.h"

#include "common/exception.h"

namespace Csr {

EmContext::EmContext(IStream &stream)
{
	int intEngineId;
	Deserializer<int>::Deserialize(stream, intEngineId);

	this->engineId = static_cast<csr_engine_id_e>(intEngineId);
}

void EmContext::Serialize(IStream &stream) const
{
	Serializer<int>::Serialize(stream, static_cast<int>(this->engineId));
}

void EmContext::set(int key, int value)
{
	switch (static_cast<Key>(key)) {
	case Key::EngineId:
		this->engineId = static_cast<csr_engine_id_e>(value);
		break;

	default:
		ThrowExc(InternalError, "Invalid key[" << key <<
				 "] comes in to set as int. value=" << value);
	}
}

void EmContext::get(int key, int &value) const
{
	switch (static_cast<Key>(key)) {
	case Key::EngineId:
		value = static_cast<int>(this->engineId);
		break;

	default:
		ThrowExc(InternalError, "Invalid key[" << key <<
				 "] comes in to get int. value=" << value);
	}
}

}
