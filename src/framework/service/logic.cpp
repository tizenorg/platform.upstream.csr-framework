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
 * @file        logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/logic.h"

#include <string>
#include <utility>
#include <stdexcept>

#include "common/audit/logger.h"
#include "csr/error.h"

namespace Csr {

Logic::Logic()
{
}

Logic::~Logic()
{
}

RawBuffer Logic::dispatch(const RawBuffer &in)
{
	auto info = getRequestInfo(in);

	INFO("Request dispatch! CommandId: " << static_cast<int>(info.first));

	switch (info.first) {
	case CommandId::SCAN_FILE: {
		Context context;
		std::string filepath;
		info.second.Deserialize(context, filepath);
		return scanFile(context, filepath);
	}

	/* TODO: should we separate command->logic mapping of CS and WP ? */
	case CommandId::CHECK_URL: {
		Context context;
		std::string url;
		info.second.Deserialize(context, url);
		return checkUrl(context, url);
	}

	case CommandId::DIR_GET_RESULTS: {
		Context context;
		std::string dir;
		info.second.Deserialize(context, dir);
		return dirGetResults(context, dir);
	}

	case CommandId::DIR_GET_FILES: {
		Context context;
		std::string dir;
		info.second.Deserialize(context, dir);
		return dirGetFiles(context, dir);
	}

	default:
		throw std::range_error(FORMAT("Command id[" << static_cast<int>(info.first)
			<< "] isn't in range."));
	}
}

std::pair<CommandId, BinaryQueue> Logic::getRequestInfo(const RawBuffer &data)
{
	CommandId id;

	BinaryQueue q;
	q.push(data);
	q.Deserialize(id);

	return std::make_pair(id, std::move(q));
}

RawBuffer Logic::scanFile(const Context &context, const std::string &filepath)
{
	INFO("Scan file[" << filepath << "] by engine");
	(void) context;

	return BinaryQueue::Serialize(CSR_ERROR_NONE, Result()).pop();
}

RawBuffer Logic::checkUrl(const Context &context, const std::string &url)
{
	INFO("Check url[" << url << "] by engine");
	(void) context;

	return BinaryQueue::Serialize(CSR_ERROR_NONE, Result()).pop();
}

RawBuffer Logic::dirGetResults(const Context &context, const std::string &dir)
{
	INFO("Dir[" << dir << "] get results");
	(void) context;

	return BinaryQueue::Serialize(CSR_ERROR_NONE, StrSet()).pop();
}

RawBuffer Logic::dirGetFiles(const Context &context, const std::string &dir)
{
	INFO("Dir[" << dir << "] get files");
	(void) context;

	return BinaryQueue::Serialize(CSR_ERROR_NONE, std::vector<Result>()).pop();
}

}
