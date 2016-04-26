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

#include <stdexcept>
#include <utility>

namespace Csr {
namespace Client {

Handle::Handle(ContextShPtr &&context) :
	m_ctx(std::move(context))
{
	if (!m_ctx)
		throw std::logic_error("context shouldn't be null");
}

Handle::~Handle()
{
}

ContextShPtr &Handle::getContext() noexcept
{
	return m_ctx;
}

void Handle::add(IResult *result)
{
	if (result == nullptr)
		throw std::logic_error("result shouldn't be null");

	m_ctx->add(result);
}

void Handle::add(ResultListPtr &&resultListPtr)
{
	if (resultListPtr == nullptr)
		throw std::logic_error("result list pointer shouldn't be null");

	m_ctx->add(std::forward<ResultListPtr>(resultListPtr));
}

} // namespace Client
} // namespace Csr
