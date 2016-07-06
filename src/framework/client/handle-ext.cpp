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

#include <utility>

#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {
namespace Client {

HandleExt::HandleExt(SockId id, ContextShPtr &&context) :
	Handle(id, std::move(context)), m_stop(false), m_isRunning(false)
{
}

HandleExt::~HandleExt()
{
	DEBUG("Destroying extended handle... join worker...");
	if (this->m_worker.joinable())
		this->m_worker.join();
}

void HandleExt::setStopFunc(std::function<void()> &&func)
{
	std::lock_guard<std::mutex> l(this->m_flagMutex);
	this->m_stopFunc = std::move(func);
}

void HandleExt::stop()
{
	DEBUG("Stop & join worker...");

	{
		std::lock_guard<std::mutex> l(this->m_flagMutex);
		this->m_stop = true;

		if (this->m_stopFunc != nullptr)
			this->m_stopFunc();
	}

	if (this->m_worker.joinable())
		this->m_worker.join();
}

bool HandleExt::isStopped() const
{
	std::lock_guard<std::mutex> l(this->m_flagMutex);
	return this->m_stop;
}

bool HandleExt::isRunning() const
{
	std::lock_guard<std::mutex> l(this->m_flagMutex);
	return this->m_isRunning;
}

void HandleExt::dispatchAsync(const std::shared_ptr<Task> &f)
{
	std::lock_guard<std::mutex> l(this->m_flagMutex);

	if (this->m_isRunning)
		ThrowExc(CSR_ERROR_BUSY, "Worker is already running. Async is busy!");

	if (this->m_worker.joinable())
		this->m_worker.join();

	this->m_isRunning = true;
	this->m_stop = false;

	this->m_worker = std::thread([this, f] {
		DEBUG("client async thread dispatched! tid: " << std::this_thread::get_id());

		{
			// Wait for client lib API func returned & mutex freed by scoped-lock dtor
			// This is for invoking registered callbacks follows returning API func
			std::lock_guard<std::mutex> _l(this->m_dispatchMutex);
		}
			(*f)();

		{
			std::lock_guard<std::mutex> _l(this->m_flagMutex);
			this->m_isRunning = false;
		}

		DEBUG("client async thread done! tid: " << std::this_thread::get_id());
	});
}

void HandleExt::add(ResultPtr &&ptr)
{
	std::lock_guard<std::mutex> l(this->m_resultsMutex);

	this->m_results.emplace_back(std::move(ptr));
}

void HandleExt::add(ResultListPtr &&ptr)
{
	std::lock_guard<std::mutex> l(this->m_resultsMutex);

	this->m_resultLists.emplace_back(std::move(ptr));
}

} // namespace Client
} // namespace Csr
