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
 * @file        thread-pool.cpp
 * @author      Jaemin Ryu (jm77.ryu@samsung.com)
 *              Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/thread-pool.h"

#include <utility>
#include <stdexcept>

#include "common/audit/logger.h"
#include "common/exception.h"

#define __BEGIN_CRITICAL__ { std::unique_lock<std::mutex> lock(this->m_mutex);
#define __END_CRITICAL__   }

namespace Csr {

ThreadPool::ThreadPool(size_t threads) : m_stop(false), m_runningWorkersNum(0)
{
	for (size_t i = 0; i < threads; ++i) {
		this->m_workers.emplace_back([this] {
			INFO("Thread[" << std::this_thread::get_id() << "] start in pool");

			while (true) {
				std::function<void()> task;

				__BEGIN_CRITICAL__
				this->m_cv.wait(lock, [this]() {
					return this->m_stop || !this->m_tasks.empty();
				});

				if (this->m_stop && this->m_tasks.empty()) {
					INFO("Thread pool stop requested. "
						  "thread[" << std::this_thread::get_id() << "] returning.");
					break;
				}

				task = std::move(this->m_tasks.front());
				this->m_tasks.pop();
				__END_CRITICAL__

				DEBUG("Start task on thread[" << std::this_thread::get_id() << "]");

				++this->m_runningWorkersNum;

				task();

				--this->m_runningWorkersNum;
			}
		});
	}

	DEBUG("Thread pool initialized with [" << this->m_workers.size() << "] threads");
}

bool ThreadPool::isTaskRunning() const
{
	return this->m_runningWorkersNum != 0;
}

ThreadPool::~ThreadPool()
{
	__BEGIN_CRITICAL__
	this->m_stop = true;
	__END_CRITICAL__

	this->m_cv.notify_all();

	for (auto &worker : this->m_workers) {
		if (worker.joinable())
			worker.join();
	}
}

void ThreadPool::submit(std::function<void()> &&task)
{
	__BEGIN_CRITICAL__

	if (!this->m_stop)
		this->m_tasks.emplace(std::move(task));

	__END_CRITICAL__

	this->m_cv.notify_one();
}

}
