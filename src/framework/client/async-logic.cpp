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
 * @file        async-logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "client/async-logic.h"

#include <utility>

#include "common/cs-detected.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Client {

AsyncLogic::AsyncLogic(std::shared_ptr<Context> &context, const Callback &cb,
					   void *userdata, const std::function<bool()> &isStopped) :
	m_origCtx(context),
	m_ctx(new CsContext),
	m_cb(cb),
	m_userdata(userdata),
	m_isStopped(isStopped),
	m_dispatcher(new Dispatcher("/tmp/." SERVICE_NAME ".socket"))
{
	copyKvp<int>(CsContext::Key::AskUser);
	copyKvp<int>(CsContext::Key::CoreUsage);
	copyKvp<std::string>(CsContext::Key::PopupMessage);
	copyKvp<bool>(CsContext::Key::ScanOnCloud);
}

AsyncLogic::~AsyncLogic()
{
	DEBUG("AsyncLogic dtor. Results num in "
		  "mother ctx[" << m_origCtx->size() << "] "
		  "and here[" << m_ctx->size() << "]");

	for (auto &resultPtr : m_ctx->m_results)
		m_origCtx->add(std::move(resultPtr));

	DEBUG("Integrated mother ctx results num: " << m_origCtx->size());
}

std::pair<Callback::Id, Task> AsyncLogic::scanDirs(const std::shared_ptr<StrSet>
		&dirs)
{
	// TODO: canonicalize dirs. (e.g. Can omit subdirectory it there is
	//       parent directory in set)
	std::pair<Callback::Id, Task> t(Callback::Id::OnCompleted, [this] {
		if (m_cb.onCompleted)
			m_cb.onCompleted(this->m_userdata);
	});

	for (const auto &dir : *dirs) {
		t = scanDir(dir);

		if (t.first != Callback::Id::OnCompleted)
			return t;
	}

	return t;
}

std::pair<Callback::Id, Task> AsyncLogic::scanDir(const std::string &dir)
{
	// For in case of there's already detected malware for dir
	auto retResults =
		m_dispatcher->methodCall<std::pair<int, std::vector<CsDetected *>>>(
			CommandId::DIR_GET_RESULTS, m_ctx, dir);

	if (retResults.first != CSR_ERROR_NONE) {
		ERROR("[Error] ret: " << retResults.first);

		for (auto r : retResults.second)
			delete r;

		auto ec = retResults.first;
		return std::make_pair(Callback::Id::OnError, [this, ec] {
			if (this->m_cb.onError)
				this->m_cb.onError(this->m_userdata, ec);
		});
	}

	// Register already detected malwares to context to be freed with context.
	for (auto r : retResults.second) {
		add(r);

		if (m_cb.onDetected)
			m_cb.onDetected(m_userdata, reinterpret_cast<csr_cs_detected_h>(r));
	}

	// Already scanned files are excluded according to history
	auto retFiles = m_dispatcher->methodCall<std::pair<int, StrSet *>>(
						CommandId::DIR_GET_FILES, m_ctx, dir);

	if (retFiles.first != CSR_ERROR_NONE) {
		ERROR("[Error] ret: " << retFiles.first);
		delete retFiles.second;
		auto ec = retFiles.first;
		return std::make_pair(Callback::Id::OnError, [this, ec] {
			if (this->m_cb.onError)
				this->m_cb.onError(this->m_userdata, ec);
		});
	}

	// Let's start scan files!
	std::shared_ptr<StrSet> strSetPtr(retFiles.second);
	auto task = scanFiles(strSetPtr);
	// TODO: register results(in outs) to db and update dir scanning history...
	return task;
}

std::pair<Callback::Id, Task> AsyncLogic::scanFiles(const
		std::shared_ptr<StrSet> &fileSet)
{
	for (const auto &file : *fileSet) {
		if (m_isStopped()) {
			INFO("async operation cancelled!");
			return std::make_pair(Callback::Id::OnCancelled, [this] {
				if (this->m_cb.onCancelled)
					this->m_cb.onCancelled(this->m_userdata);
			});
		}

		auto ret = m_dispatcher->methodCall<std::pair<int, CsDetected *>>(
					   CommandId::SCAN_FILE, m_ctx, file);

		if (ret.first != CSR_ERROR_NONE) {
			ERROR("[Error] ret: " << ret.first);
			delete ret.second;
			auto ec = ret.first;
			return std::make_pair(Callback::Id::OnError, [this, ec] {
				if (this->m_cb.onError)
					this->m_cb.onError(this->m_userdata, ec);

				return;
			});
		}

		if (!ret.second->hasValue()) {
			DEBUG("[Scanned] file[" << file << "]");
			delete ret.second;

			if (m_cb.onScanned)
				m_cb.onScanned(m_userdata, file.c_str());

			continue;
		}

		// malware detected!
		INFO("[Detected] file[" << file << "]");
		add(ret.second);

		if (m_cb.onDetected)
			m_cb.onDetected(m_userdata, reinterpret_cast<csr_cs_detected_h>(ret.second));
	}

	return std::make_pair(Callback::Id::OnCompleted, [this] {
		DEBUG("[Completed]");

		if (this->m_cb.onCompleted)
			this->m_cb.onCompleted(this->m_userdata);
	});
}

void AsyncLogic::add(Result *r)
{
	m_ctx->add(r);
}

}
}
