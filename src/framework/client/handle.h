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
#include "common/socket-descriptor.h"

namespace Csr {
namespace Client {

class Handle {
public:
	explicit Handle(SockId, ContextShPtr &&);
	virtual ~Handle();

	template<typename Type, typename ...Args>
	Type dispatch(Args &&...);

	template<typename ...Args>
	void ping(Args &&...);

	virtual void add(ResultPtr &&);
	virtual void add(ResultListPtr &&);

	ContextShPtr &getContext(void) noexcept;

protected:
	// for destroying with context
	std::vector<ResultPtr> m_results;
	std::vector<ResultListPtr> m_resultLists;

private:
	SockId m_sockId;
	ContextShPtr m_ctx;
	std::unique_ptr<Dispatcher> m_dispatcher;
};

template<typename Type, typename ...Args>
Type Handle::dispatch(Args &&...args)
{
	return this->m_dispatcher->methodCall<Type>(std::forward<Args>(args)...);
}

template<typename ...Args>
void Handle::ping(Args &&...args)
{
	this->m_dispatcher->methodPing(std::forward<Args>(args)...);
}

} // namespace Client
} // namespace Csr
