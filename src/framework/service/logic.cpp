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
#include <iostream>

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
	case CommandId::FILE_SCAN: {
		std::string filepath;
		info.second.Deserialize(filepath);
		return fileScan(filepath);
	}

	case CommandId::FILE_JUDGE: {
		std::string filepath;
		int judge;
		info.second.Deserialize(filepath, judge);
		return fileJudge(filepath, judge);
	}

	default:
		/* TODO: throw request info broken exception */
		return RawBuffer();
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

RawBuffer Logic::fileScan(const std::string &filepath)
{
	DEBUG("Do filescan through engine with filepath: " << filepath);

	return MessageBuffer::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer Logic::fileJudge(const std::string &filepath, int judge)
{
	DEBUG("Do filejudge through engine with filepath: " << filepath
		<< " and judge: " << judge);

	return MessageBuffer::Serialize(CSR_ERROR_NONE).pop();
}

}
