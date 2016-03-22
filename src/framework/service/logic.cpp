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
#include "logic.h"

#include <string>
#include <utility>
#include <stdexcept>

#include "audit/logger.h"
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
		std::string filepath;
		// csr-cs handle deserialize
		info.second.Deserialize(filepath);
		return scanFile(filepath);
	}

	/* TODO: should we separate command->logic mapping of CS and WP ? */
	case CommandId::CHECK_URL: {
		std::string url;
		// csr-wp handle deserialize
		info.second.Deserialize(url);
		return checkUrl(url);
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

RawBuffer Logic::scanFile(const std::string &filepath)
{
	(void) filepath;

	DEBUG("Scan file[" << filepath << "] by engine");
	return MessageBuffer::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer Logic::checkUrl(const std::string &url)
{
	(void) url;

	DEBUG("CHeck url[" << url << "] by engine");
	return MessageBuffer::Serialize(CSR_ERROR_NONE).pop();
}

}
