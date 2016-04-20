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
 * @file        types.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR internal serializable types
 */
#include "common/types.h"

#include <stdexcept>
#include <utility>

#include "common/audit/logger.h"

namespace Csr {

Context::Context()
{
}

Context::~Context()
{
}

// don't copy results.. context copy operation only should be used for option copy
Context::Context(const Context &) :
	ISerializable(),
	m_results()
{
}

Context &Context::operator=(const Context &)
{
	return *this;
}

Context::Context(Context &&other) :
	m_results(std::move(other.m_results))
{
}

Context &Context::operator=(Context &&other)
{
	if (this == &other)
		return *this;

	m_results = std::move(other.m_results);

	return *this;
}

void Context::add(ResultPtr &&item)
{
	std::lock_guard<std::mutex> l(m_mutex);
	m_results.emplace_back(std::forward<ResultPtr>(item));
}

void Context::add(Result *item)
{
	std::lock_guard<std::mutex> l(m_mutex);
	m_results.emplace_back(item);
}

void Context::add(ResultListPtr &&item)
{
	std::lock_guard<std::mutex> l(m_mutex);
	m_resultLists.emplace_back(std::forward<ResultListPtr>(item));
}

size_t Context::size() const
{
	std::lock_guard<std::mutex> l(m_mutex);
	return m_results.size();
}

Result::Result() : m_hasVal(false)
{
}

Result::~Result()
{
}

Result::Result(IStream &stream)
{
	Deserializer<bool>::Deserialize(stream, m_hasVal);
}

void Result::Serialize(IStream &stream) const
{
	Serializer<bool>::Serialize(stream, m_hasVal);
}

Result::Result(Result &&)
{
}

Result &Result::operator=(Result &&)
{
	return *this;
}

bool Result::hasValue() const
{
	return m_hasVal;
}

} // namespace Csr
