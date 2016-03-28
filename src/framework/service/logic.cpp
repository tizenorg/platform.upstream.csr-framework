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
		Cs::Context context;
		std::string filepath;
		info.second.Deserialize(context, filepath);
		return scanFile(context, filepath);
	}

	/* TODO: should we separate command->logic mapping of CS and WP ? */
	case CommandId::CHECK_URL: {
		Wp::Context context;
		std::string url;
		info.second.Deserialize(context, url);
		return checkUrl(context, url);
	}

	default:
		throw std::range_error(FORMAT("Command id[" << static_cast<int>(info.first)
			<< "] isn't in range."));
	}
}

std::pair<CommandId, MessageBuffer> Logic::getRequestInfo(const RawBuffer &data)
{
	CommandId id;

	MessageBuffer msgbuffer;
	msgbuffer.push(data);
	msgbuffer.Deserialize(id);

	return std::make_pair(id, std::move(msgbuffer));
}

RawBuffer Logic::scanFile(const Cs::Context &context, const std::string &filepath)
{
	INFO("Scan file[" << filepath << "] by engine");
	(void) context;

	return MessageBuffer::Serialize(CSR_ERROR_NONE, Cs::Result()).pop();
}

RawBuffer Logic::checkUrl(const Wp::Context &context, const std::string &url)
{
	INFO("Check url[" << url << "] by engine");
	(void) context;

	return MessageBuffer::Serialize(CSR_ERROR_NONE, Wp::Result()).pop();
}

}
