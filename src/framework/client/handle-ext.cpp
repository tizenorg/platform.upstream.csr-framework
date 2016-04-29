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
 * @file        handle-ext.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       handle with async request extension
 */
#include "client/handle-ext.h"

#include <algorithm>

#include "client/utils.h"
#include "common/dispatcher.h"
#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {
namespace Client {

HandleExt::HandleExt(ContextShPtr &&context) :
	Handle(std::move(context)),
	m_stop(false)
{
}

HandleExt::~HandleExt()
{
	DEBUG("Destroying extended handle... join all workers...");
	eraseJoinableIf();
}

void HandleExt::stop()
{
	DEBUG("Stop & join all workers...");
	m_stop = true;
	eraseJoinableIf();
}

bool HandleExt::isStopped() const
{
	return m_stop.load();
}

void HandleExt::eraseJoinableIf(std::function<bool(const WorkerMapPair &)> pred)
{
	std::unique_lock<std::mutex> l(m_mutex);
	DEBUG("clean joinable workers! current worker map size: " <<
		  m_workerMap.size());
	auto it = m_workerMap.begin();

	while (it != m_workerMap.end()) {
		DEBUG("Worker map traversing to erase! current iter tid: " << it->first);

		if (!it->second.t.joinable())
			ThrowExc(InternalError, "All workers should be joinable but it isn't. "
					 "tid: " << it->first);

		if (!pred(*it)) {
			++it;
			continue;
		}

		DEBUG("Joining worker! tid:" << it->first);
		l.unlock();
		it->second.t.join(); // release lock for worker who calls done()
		l.lock();
		DEBUG("Joined worker! tid:" << it->first);
		it = m_workerMap.erase(it);
	}
}

void HandleExt::done()
{
	std::lock_guard<std::mutex> l(m_mutex);
	auto it = m_workerMap.find(std::this_thread::get_id());

	if (it == m_workerMap.end())
		ThrowExc(InternalError, "worker done but it's not registered in map. "
				 "tid: " << std::this_thread::get_id());

	it->second.isDone = true;
}

void HandleExt::dispatchAsync(const Task &f)
{
	eraseJoinableIf([](const WorkerMapPair & pair) {
		return pair.second.isDone.load();
	});
	// TODO: how to handle exceptions in workers
	std::thread t([this, f] {
		DEBUG("client async thread dispatched! tid: " << std::this_thread::get_id());

		f();
		done();

		DEBUG("client async thread done! tid: " << std::this_thread::get_id());
	});

	{
		std::lock_guard<std::mutex> l(m_mutex);
		m_workerMap.emplace(t.get_id(), std::move(t));
	}
}

HandleExt::Worker::Worker() : isDone(false)
{
	DEBUG("Worker default constructor called");
}

HandleExt::Worker::Worker(std::thread &&_t) :
	isDone(false),
	t(std::forward<std::thread>(_t))
{
}

HandleExt::Worker::Worker(HandleExt::Worker &&other) :
	isDone(other.isDone.load()),
	t(std::move(other.t))
{
}

HandleExt::Worker &HandleExt::Worker::operator=(HandleExt::Worker &&other)
{
	isDone = other.isDone.load();
	t = std::move(other.t);
	return *this;
}

void HandleExt::add(ResultPtr &&ptr)
{
	std::lock_guard<std::mutex> l(m_resultsMutex);
	m_results.emplace_back(std::forward<ResultPtr>(ptr));
}

void HandleExt::add(ResultListPtr &&ptr)
{
	std::lock_guard<std::mutex> l(m_resultsMutex);
	m_resultLists.emplace_back(std::forward<ResultListPtr>(ptr));
}

} // namespace Client
} // namespace Csr
