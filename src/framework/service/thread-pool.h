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
 * @file        thread-pool.h
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 *              Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <queue>

namespace Csr {

class ThreadPool {
public:
	// worker thread dynamically created / joined from min to max
	explicit ThreadPool(size_t min, size_t max);
	virtual ~ThreadPool();

	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;
	ThreadPool(ThreadPool &&) = delete;
	ThreadPool &operator=(ThreadPool &&) = delete;

	// submit task to thread pool
	void submit(std::function<void()> &&task);

	// get workers size in thread pool
	size_t size(void);

private:
	void add(void);

	const size_t m_min;
	const size_t m_max;
	std::atomic<bool> m_stop;
	std::map<std::thread::id, std::thread> m_workers;
	std::queue<std::function<void()>> m_tasks;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};

}
