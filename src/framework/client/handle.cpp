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
 * @file        handle.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Client request handle with dispatcher in it
 */
#include "client/handle.h"

#include <utility>

#include "common/exception.h"

namespace Csr {
namespace Client {

Handle::Handle(SockId id, ContextShPtr &&context) :
	m_sockId(id), m_ctx(std::forward<ContextShPtr>(context))
{
	if (!m_ctx)
		ThrowExc(InvalidParam, "context shouldn't be null");
}

Handle::~Handle()
{
}

ContextShPtr &Handle::getContext() noexcept
{
	return m_ctx;
}

void Handle::add(ResultPtr &&ptr)
{
	m_results.emplace_back(std::forward<ResultPtr>(ptr));
}

void Handle::add(ResultListPtr &&ptr)
{
	m_resultLists.emplace_back(std::forward<ResultListPtr>(ptr));
}

} // namespace Client
} // namespace Csr
