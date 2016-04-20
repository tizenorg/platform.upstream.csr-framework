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

#include "common/cs-detected.h"
#include "common/wp-result.h"
#include "common/audit/logger.h"
#include "csr/error.h"

namespace Csr {

namespace {

// temporal function for debugging until modules integrated to logic.
void printCsContext(const CsContext &context)
{
	std::string popupMessage;
	int askUser;
	int coreUsage;
	bool scanOnCloud;

	context.get(static_cast<int>(CsContext::Key::PopupMessage), popupMessage);
	context.get(static_cast<int>(CsContext::Key::AskUser), askUser);
	context.get(static_cast<int>(CsContext::Key::CoreUsage), coreUsage);
	context.get(static_cast<int>(CsContext::Key::ScanOnCloud), scanOnCloud);

	INFO("Context info:"
		" PopupMessage: " << popupMessage <<
		" AskUser: " << askUser <<
		" CoreUsage: " << coreUsage <<
		" ScanOnCloud: " << scanOnCloud);
}

}

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
	case CommandId::SCAN_DATA: {
		CsContext context;
		RawBuffer data;
		info.second.Deserialize(context, data);
		return scanData(context, data);
	}

	case CommandId::SCAN_FILE: {
		CsContext context;
		std::string filepath;
		info.second.Deserialize(context, filepath);
		return scanFile(context, filepath);
	}

	/* TODO: should we separate command->logic mapping of CS and WP ? */
	case CommandId::CHECK_URL: {
		WpContext context;
		std::string url;
		info.second.Deserialize(context, url);
		return checkUrl(context, url);
	}

	case CommandId::DIR_GET_RESULTS: {
		CsContext context;
		std::string dir;
		info.second.Deserialize(context, dir);
		return dirGetResults(context, dir);
	}

	case CommandId::DIR_GET_FILES: {
		CsContext context;
		std::string dir;
		info.second.Deserialize(context, dir);
		return dirGetFiles(context, dir);
	}

	case CommandId::JUDGE_STATUS: {
		CsContext context;
		std::string filepath;
		info.second.Deserialize(context, filepath);
		return judgeStatus(context, filepath);
	}

	case CommandId::GET_DETECTED: {
		CsContext context;
		std::string filepath;
		info.second.Deserialize(context, filepath);
		return getDetected(context, filepath);
	}

	case CommandId::GET_DETECTED_LIST: {
		CsContext context;
		std::string dir;
		info.second.Deserialize(context, dir);
		//return getDetectedList(context, dir);
		RawBuffer buff = getDetectedList(context, dir);
		INFO(" CommandId::GET_DETECTED_LIST : RawBuffer Size[" << buff.size() << "]");
		return buff;
	}

	case CommandId::GET_IGNORED: {
		CsContext context;
		std::string filepath;
		info.second.Deserialize(context, filepath);
		return getIgnored(context, filepath);
	}

	case CommandId::GET_IGNORED_LIST: {
		CsContext context;
		std::string dir;
		info.second.Deserialize(context, dir);
		return getIgnoredList(context, dir);
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

RawBuffer Logic::scanData(const CsContext &context, const RawBuffer &data)
{
	INFO("Scan Data[size=" << data.size() << "] by engine");

	printCsContext(context);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();
}

RawBuffer Logic::scanFile(const CsContext &context, const std::string &filepath)
{
	INFO("Scan file[" << filepath << "] by engine");

	printCsContext(context);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();
}

RawBuffer Logic::dirGetResults(const CsContext &context, const std::string &dir)
{
	INFO("Dir[" << dir << "] get results");

	printCsContext(context);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, std::vector<CsDetected>()).pop();
}

RawBuffer Logic::dirGetFiles(const CsContext &context, const std::string &dir)
{
	INFO("Dir[" << dir << "] get files");

	printCsContext(context);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, StrSet()).pop();
}

RawBuffer Logic::judgeStatus(const CsContext &context, const std::string &filepath)
{
	INFO("Judge Status[" << filepath << "] by engine");

	printCsContext(context);

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer Logic::getDetected(const CsContext &context, const std::string &filepath)
{
	INFO("Get Detected[" << filepath << "] by engine");

	printCsContext(context);

	CsDetected detected;
	detected.set(static_cast<int>(CsDetected::Key::TargetName), "test_file");

	return BinaryQueue::Serialize(CSR_ERROR_NONE, detected).pop();
}

RawBuffer Logic::getDetectedList(const CsContext &context, const std::string &dir)
{
	INFO("Get Detected List[" << dir << "] by engine");

	printCsContext(context);

	std::unique_ptr<CsDetected> detected(new CsDetected());
	detected->set(static_cast<int>(CsDetected::Key::TargetName), "test_file");

	CsDetectedList list;
	list.add(std::move(detected));

	return BinaryQueue::Serialize(CSR_ERROR_NONE, list).pop();
}

RawBuffer Logic::getIgnored(const CsContext &context, const std::string &filepath)
{
	INFO("Get Ignored[" << filepath << "] by engine");

	printCsContext(context);

	CsDetected detected;
	detected.set(static_cast<int>(CsDetected::Key::TargetName), "test_file");

	return BinaryQueue::Serialize(CSR_ERROR_NONE, detected).pop();
}

RawBuffer Logic::getIgnoredList(const CsContext &context, const std::string &dir)
{
	INFO("Get Ignored List[" << dir << "] by engine");

	printCsContext(context);

	std::unique_ptr<CsDetected> detected(new CsDetected());
	detected->set(static_cast<int>(CsDetected::Key::TargetName), "test_file");

	CsDetectedList list;
	list.add(std::move(detected));

	return BinaryQueue::Serialize(CSR_ERROR_NONE, list).pop();
}

RawBuffer Logic::checkUrl(const WpContext &context, const std::string &url)
{
	INFO("Check url[" << url << "] by engine");
	(void) context;

	return BinaryQueue::Serialize(CSR_ERROR_NONE, WpResult()).pop();
}

}
