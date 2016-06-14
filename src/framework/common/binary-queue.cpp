/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        binary-queue.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 *              Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "common/binary-queue.h"

#include <cstring>
#include <stdexcept>
#include <new>

#include "common/exception.h"

namespace Csr {

BinaryQueue::BinaryQueue() noexcept : m_size(0)
{
}

RawBuffer BinaryQueue::pop()
{
	RawBuffer buf(this->m_size);

	this->read(this->m_size, buf.data());

	return buf;
}

void BinaryQueue::push(const RawBuffer &data)
{
	this->write(data.size(), data.data());
}

void BinaryQueue::write(size_t size, const void *bytes)
{
	size_t idx = 0;
	while (size > 0) {
		auto s = (size > BinaryQueue::MaxBucketSize) ? MaxBucketSize : size;
		auto b = new unsigned char[s];
		memcpy(b, static_cast<const unsigned char *>(bytes) + idx, s);
		this->m_buckets.emplace(new Bucket(b, s));
		this->m_size += s;
		idx += s;
		size -= s;
	}
}

void BinaryQueue::read(size_t size, void *bytes)
{
	if (size == 0)
		return;

	if (size > this->m_size)
		ThrowExc(CSR_ERROR_SOCKET, "protocol broken. no more binary to flatten in queue");

	void *cur = bytes;

	while (size > 0) {
		if (this->m_buckets.empty())
			ThrowExc(CSR_ERROR_SOCKET, "protocol broken. no more buckets to extract");

		size_t count = std::min(size, this->m_buckets.front()->left);
		cur = this->m_buckets.front()->extractTo(cur, count);

		size -= count;
		this->m_size -= count;

		if (this->m_buckets.front()->left == 0)
			this->m_buckets.pop();
	}
}

bool BinaryQueue::empty(void) const noexcept
{
	return this->m_size == 0;
}

BinaryQueue::Bucket::Bucket(unsigned char *_data, size_t _size) :
	data(_data), cur(_data), left(_size)
{
	if (_data == nullptr || _size == 0)
		ThrowExc(CSR_ERROR_SERVER, "Bucket construct failed.");
}

BinaryQueue::Bucket::~Bucket() noexcept
{
	delete []this->data;
}

void *BinaryQueue::Bucket::extractTo(void *dest, size_t size)
{
	if (dest == nullptr || size == 0)
		ThrowExc(CSR_ERROR_SERVER, "logic error. invalid input to Bucket::extractTo.");

	memcpy(dest, this->cur, size);

	this->cur += size;
	this->left -= size;

	return static_cast<unsigned char *>(dest) + size;
}

}
