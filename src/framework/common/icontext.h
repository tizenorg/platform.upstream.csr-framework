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
 * @file        icontext.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Abstract class of context for both of cs / wp
 */
#pragma once

#include <vector>
#include <memory>
#include <mutex>

#include "common/dispatcher.h"
#include "common/serialization.h"
#include "common/kvp-container.h"
#include "common/iresult.h"

namespace Csr {

class IContext;
using ContextPtr = std::unique_ptr<IContext>;
using ContextShPtr = std::shared_ptr<IContext>;

class IContext : public ISerializable, public KvpContainer {
public:
	IContext();
	virtual ~IContext();

	IContext(IContext &&) = delete;
	IContext &operator=(IContext &&) = delete;
	IContext(const IContext &) = delete;
	IContext &operator=(const IContext &) = delete;

	void add(ResultPtr &&);
	void add(IResult *);
	void add(ResultListPtr &&);
	size_t size(void) const;
	// for destroying with context
	std::vector<ResultPtr> m_results;
	std::vector<ResultListPtr> m_resultLists;

private:
	mutable std::mutex m_mutex;
};

} // namespace Csr
