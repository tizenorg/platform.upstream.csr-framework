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
 * @file        async-logic.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <memory>
#include <atomic>

#include "common/types.h"
#include "common/cs-context.h"
#include "common/dispatcher.h"
#include "client/callback.h"

namespace Csr {
namespace Client {

class AsyncLogic {
public:
	AsyncLogic(std::shared_ptr<Context> &context, const Callback &cb,
			   void *userdata,
			   const std::function<bool()> &isStopped);
	virtual ~AsyncLogic();

	std::pair<Callback::Id, Task> scanFiles(const std::shared_ptr<StrSet> &files);
	std::pair<Callback::Id, Task> scanDir(const std::string &dir);
	std::pair<Callback::Id, Task> scanDirs(const std::shared_ptr<StrSet> &dirs);

	void stop(void);

private:
	template<typename T>
	void copyKvp(CsContext::Key);

	void add(Result *);

	std::shared_ptr<Context> &m_origCtx; // for registering results for auto-release

	// TODO: append it to handle context when destroyed
	std::unique_ptr<Context> m_ctx;

	Callback m_cb;
	void *m_userdata;
	std::function<bool()> m_isStopped;

	std::unique_ptr<Dispatcher> m_dispatcher;
};

template<typename T>
void AsyncLogic::copyKvp(CsContext::Key key)
{
	T value;

	m_origCtx->get(static_cast<int>(key), value);
	m_ctx->set(static_cast<int>(key), value);
}

}
}
