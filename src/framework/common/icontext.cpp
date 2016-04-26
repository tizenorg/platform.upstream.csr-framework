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
 * @file        icontext.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Abstract class of context for both of cs / wp
 */
#include "common/icontext.h"

#include <stdexcept>
#include <utility>

#include "common/audit/logger.h"

namespace Csr {

IContext::IContext()
{
}

IContext::~IContext()
{
}

void IContext::add(ResultPtr &&item)
{
	std::lock_guard<std::mutex> l(m_mutex);
	m_results.emplace_back(std::forward<ResultPtr>(item));
}

void IContext::add(IResult *item)
{
	std::lock_guard<std::mutex> l(m_mutex);
	m_results.emplace_back(item);
}

void IContext::add(ResultListPtr &&item)
{
	std::lock_guard<std::mutex> l(m_mutex);
	m_resultLists.emplace_back(std::forward<ResultListPtr>(item));
}

size_t IContext::size() const
{
	std::lock_guard<std::mutex> l(m_mutex);
	return m_results.size();
}

} // namespace Csr
