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
#include <atomic>

#include <string>
#include <map>
#include <set>
#include <utility>

#include "client/handle.h"
#include "client/callback.h"
#include "common/icontext.h"

namespace Csr {
namespace Client {

class HandleExt : public Handle {
public:
	explicit HandleExt(ContextShPtr &&);
	virtual ~HandleExt();

	void dispatchAsync(const Task &task);
	void stop(void);
	bool isStopped(void) const;

	Callback m_cb; // TODO: to refine..

private:
	struct Worker {
		std::atomic<bool> isDone;
		std::thread t;

		Worker();
		Worker(const std::thread &_t) = delete; // to prevent thread instance copied
		Worker(std::thread &&_t);
		Worker(Worker &&other);
		Worker &operator=(Worker &&other);
	};

	using WorkerMapPair = std::pair<const std::thread::id, Worker>;

	void eraseJoinableIf(std::function<bool(const WorkerMapPair &)>
						 = [](const WorkerMapPair &)
	{
		return true;
	});
	void done(void);

	std::atomic<bool> m_stop;
	std::mutex m_mutex;
	std::map<std::thread::id, Worker> m_workerMap;
};

} // namespace Client
} // namespace Csr
