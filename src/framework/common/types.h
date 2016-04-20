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
 * @file        types.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR internal serializable types
 */
#pragma once

#include <vector>
#include <memory>
#include <mutex>

#include "common/dispatcher.h"
#include "common/serialization.h"
#include "common/kvp-container.h"

namespace Csr {

using Task = std::function<void()>;
using StrSet = std::set<std::string>;

class Result : public ISerializable, public KvpContainer {
public:
	Result();
	virtual ~Result();

	Result(Result &&);
	Result &operator=(Result &&);

	bool hasValue(void) const;

protected:
	Result(IStream &);
	virtual void Serialize(IStream &) const override;

	inline void setValueFlag(void)
	{
		m_hasVal = true;
	}

private:
	bool m_hasVal;
};

using ResultPtr = std::unique_ptr<Result>;
using ResultList = std::vector<ResultPtr>;
using ResultListPtr = std::unique_ptr<ResultList>;

class Context : public ISerializable, public KvpContainer {
public:
	Context();
	virtual ~Context();

	Context(Context &&);
	Context &operator=(Context &&);

	// TODO: Handling results vector between contexts should be refined..
	//       copy ctor/assignments for serializing and results vector isn't included here.
	Context(const Context &);
	Context &operator=(const Context &);

	void add(ResultPtr &&);
	void add(Result *);
	void add(ResultListPtr &&);
	size_t size(void) const;
	// for destroying with context
	std::vector<ResultPtr> m_results;
	std::vector<ResultListPtr> m_resultLists;

private:
	mutable std::mutex m_mutex;
};

} // namespace Csr
