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
 * @file        message-buffer.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Zofia Abramowska (z.abramowska@samsung.com)
 * @version     1.0
 * @brief       Implementation of MessageBuffer.
 */
#include "common/message-buffer.h"

#include <exception>
#include <cstring>

namespace Csr {

MessageBuffer::MessageBuffer() : m_bytesLeft(0)
{
}

void MessageBuffer::push(const RawBuffer &data)
{
	m_buffer.appendCopy(data.data(), data.size());
}

RawBuffer MessageBuffer::pop()
{
	size_t size = m_buffer.size();

	RawBuffer buffer(size + sizeof(size_t));

	memcpy(buffer.data(), &size, sizeof(size_t));
	m_buffer.flattenConsume(buffer.data() + sizeof(size_t), size);

	return buffer;
}

bool MessageBuffer::ready()
{
	countBytesLeft();

	return (m_bytesLeft != 0 && m_bytesLeft <= m_buffer.size());
}

void MessageBuffer::read(size_t num, void *bytes)
{
	countBytesLeft();
	if (num > m_bytesLeft)
		throw std::range_error("protocol broken. message buffer overreaded");

	m_buffer.flattenConsume(bytes, num);
	m_bytesLeft -= num;
}

void MessageBuffer::write(size_t num, const void *bytes)
{
	m_buffer.appendCopy(bytes, num);
}

void MessageBuffer::countBytesLeft()
{
	if (m_bytesLeft > 0 || m_buffer.size() < sizeof(size_t))
		return;

	m_buffer.flattenConsume(&m_bytesLeft, sizeof(size_t));
}

}
