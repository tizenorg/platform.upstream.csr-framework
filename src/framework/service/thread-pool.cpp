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
#include "service/exception.h"

#define __BEGIN_CRITICAL__ { std::lock_guard<std::mutex> lock(this->m_mutex);
#define __END_CRITICAL__   }

namespace Csr {

ThreadPool::ThreadPool(size_t min, size_t max) :
	m_min(min),
	m_max(max),
	m_stop(false),
	m_runningWorkersNum(0)
{
	if (this->m_min > this->m_max)
		throw std::invalid_argument("thread pool MIN shouldn't be bigger than MAX");

	for (size_t i = 0; i < this->m_min; i++)
		this->add();

	DEBUG("Thread pool initialized with [" << this->m_workers.size() << "] threads");
}

void ThreadPool::add(void)
{
	std::thread t([this]() {
		DEBUG("Thread[" << std::this_thread::get_id() << "] start in pool");

		while (true) {
			std::unique_lock<std::mutex> lock(this->m_mutex);
			this->m_cv.wait(lock, [this]() {
				return this->m_workers.size() > this->m_min ||
					   this->m_stop ||
					   !this->m_tasks.empty();
			});

			if (this->m_stop && this->m_tasks.empty()) {
				DEBUG("Thread pool stop requested. "
					  "thread[" << std::this_thread::get_id() << "] returning.");
				break;
			}

			if (this->m_workers.size() > this->m_min && this->m_tasks.empty()) {
				DEBUG("Terminate idle thread[" << std::this_thread::get_id() << "]");

				// move thread itself to me and erase dummy in this->m_workers
				auto currentId = std::this_thread::get_id();
				this->m_workers[currentId].detach();
				this->m_workers.erase(currentId);
				break;
			}

			auto task = std::move(this->m_tasks.front());
			this->m_tasks.pop();

			lock.unlock();

			INFO("Start task on thread[" << std::this_thread::get_id() << "]");

			++this->m_runningWorkersNum;

			task();

			--this->m_runningWorkersNum;
		}
	});

	this->m_workers[t.get_id()] = std::move(t);
}

size_t ThreadPool::size() const
{
	return this->m_workers.size();
}

bool ThreadPool::isTaskRunning() const
{
	return this->m_runningWorkersNum != 0;
}

ThreadPool::~ThreadPool()
{
	this->m_stop = true;

	this->m_cv.notify_all();

	for (auto &worker : this->m_workers) {
		if (worker.second.joinable())
			worker.second.join();
	}
}

void ThreadPool::submit(std::function<void()> &&task)
{
	if (!this->m_tasks.empty() && this->m_workers.size() < this->m_max) {
		DEBUG("more workers needed. let's add.");
		this->add();
	}

	__BEGIN_CRITICAL__

	if (!this->m_stop)
		this->m_tasks.emplace(std::move(task));

	__END_CRITICAL__

	this->m_cv.notify_one();
}

}
