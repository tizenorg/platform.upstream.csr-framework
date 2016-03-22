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
 * @file        message-buffer.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Zofia Abramowska (z.abramowska@samsung.com)
 * @version     1.0
 * @brief       Implementatin of MessageBuffer.
 */
#pragma once

#include "common/raw-buffer.h"
#include "common/binary-queue.h"
#include "common/serialization.h"

namespace Csr {

class MessageBuffer : public IStream {
public:
	MessageBuffer();

	MessageBuffer(MessageBuffer&&) = default;
	MessageBuffer &operator=(MessageBuffer&&) = default;

	void push(const RawBuffer &data);
	RawBuffer pop();

	bool ready();

	virtual void read(size_t num, void *bytes);
	virtual void write(size_t num, const void *bytes);

	// generic serialization
	template <typename... Args>
	static MessageBuffer Serialize(const Args&... args)
	{
		MessageBuffer buffer;
		Serializer<Args...>::Serialize(buffer, args...);
		return buffer;
	}

	// generic deserialization
	template <typename... Args>
	void Deserialize(Args&... args)
	{
		Deserializer<Args...>::Deserialize(*this, args...);
	}

protected:
	void countBytesLeft(void);

	size_t m_bytesLeft;
	BinaryQueue m_buffer;
};

}
