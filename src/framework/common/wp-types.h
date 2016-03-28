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
 * @file        wp-types.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Web Protection internal types
 */
#pragma once

#include <set>
#include <memory>
#include <utility>

#include "common/dispatcher.h"
#include "common/serialization.h"

namespace Csr {
namespace Wp {

class Result : public ISerializable {
public:
	Result();
	virtual ~Result();
	Result(IStream &);
	virtual void Serialize(IStream &) const;

	Result(Result &&);
	Result &operator=(Result &&);
};

class Context : public ISerializable {
public:
	Context();
	virtual ~Context();
	Context(IStream &);
	virtual void Serialize(IStream &) const;

	template<typename Type, typename  ...Args>
	Type dispatch(Args &&...);

	Context(Context &&);
	Context &operator=(Context &&);

	void addResult(Result *);

private:
	std::unique_ptr<Dispatcher> m_dispatcher;
	std::set<std::unique_ptr<Result>> m_results;
};

template<typename Type, typename ...Args>
Type Context::dispatch(Args &&...args)
{
	if (m_dispatcher == nullptr)
		m_dispatcher.reset(new Dispatcher("/tmp/." SERVICE_NAME ".socket"));

	return m_dispatcher->methodCall<Type>(std::forward<Args>(args)...);
}

} // namespace Wp
} // namespace Csr
