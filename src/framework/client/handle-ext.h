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
 * @file        handle-ext.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       handle with async request extension
 */
#pragma once

#include <mutex>
#include <thread>
#include <memory>

#include "client/handle.h"
#include "client/callback.h"

namespace Csr {
namespace Client {

class HandleExt : public Handle {
public:
	explicit HandleExt(SockId id, ContextShPtr &&);
	virtual ~HandleExt();

	void dispatchAsync(const std::shared_ptr<Task> &task);
	void setStopFunc(std::function<void()> &&func);
	void stop(void);
	bool isStopped(void) const;
	bool isRunning(void) const;

	Callback m_cb; // TODO: to refine..
	std::mutex m_dispatchMutex;

	virtual void add(ResultPtr &&) override;
	virtual void add(ResultListPtr &&) override;

private:
	bool m_stop;
	bool m_isRunning;
	std::thread m_worker;
	std::function<void()> m_stopFunc;
	mutable std::mutex m_resultsMutex;
	mutable std::mutex m_flagMutex;
};

} // namespace Client
} // namespace Csr
