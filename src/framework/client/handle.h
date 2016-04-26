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
 * @file        handle.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Client request handle with dispatcher in it
 */
#pragma once

#include <utility>
#include <memory>
#include <vector>

#include "common/icontext.h"
#include "common/dispatcher.h"

namespace Csr {
namespace Client {

class Handle {
public:
	explicit Handle(ContextShPtr &&);
	virtual ~Handle();

	template<typename Type, typename ...Args>
	Type dispatch(Args &&...);

	virtual void add(ResultPtr &&);
	virtual void add(ResultListPtr &&);

	ContextShPtr &getContext(void) noexcept;

protected:
	// for destroying with context
	std::vector<ResultPtr> m_results;
	std::vector<ResultListPtr> m_resultLists;

private:
	std::unique_ptr<Dispatcher> m_dispatcher;
	ContextShPtr m_ctx;
};

template<typename Type, typename ...Args>
Type Handle::dispatch(Args &&...args)
{
	if (m_dispatcher == nullptr)
		m_dispatcher.reset(new Dispatcher("/tmp/." SERVICE_NAME ".socket"));

	return m_dispatcher->methodCall<Type>(std::forward<Args>(args)...);
}

} // namespace Client
} // namespace Csr