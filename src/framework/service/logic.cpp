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
#include <algorithm>
#include <stdexcept>

#include "common/audit/logger.h"
#include "ui/askuser.h"
#include "service/type-converter.h"
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

Logic::Logic() :
	m_cs(new CsLoader(CS_ENGINE_PATH)),
	m_wp(new WpLoader(WP_ENGINE_PATH))
{
	// TODO: Provide engine-specific res/working dirs
	int ret = m_cs->globalInit(SAMPLE_ENGINE_RO_RES_DIR,
							   SAMPLE_ENGINE_RW_WORKING_DIR);

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("global init cs engine. ret: " << ret));

	ret = m_wp->globalInit(SAMPLE_ENGINE_RO_RES_DIR, SAMPLE_ENGINE_RW_WORKING_DIR);

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("global init wp engine. ret: " << ret));

	DEBUG("Service logic ctor done");
}

Logic::~Logic()
{
	int ret = m_cs->globalDeinit();

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("global deinit cs engine. ret: " << ret));

	ret = m_wp->globalDeinit();

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("global deinit wp engine. ret: " << ret));
}

RawBuffer Logic::dispatch(const RawBuffer &in)
{
	auto info = getRequestInfo(in);

	INFO("Request dispatch! CommandId: " << static_cast<int>(info.first));

	switch (info.first) {
	// Content scanning
	case CommandId::SCAN_DATA: {
		CsContextShPtr cptr;
		RawBuffer data;
		info.second.Deserialize(cptr, data);

		return scanData(*cptr, data);
	}

	case CommandId::SCAN_FILE: {
		CsContextShPtr cptr;
		std::string filepath;
		info.second.Deserialize(cptr, filepath);
		return scanFile(*cptr, filepath);
	}

	case CommandId::DIR_GET_RESULTS: {
		CsContextShPtr cptr;
		std::string dir;
		info.second.Deserialize(cptr, dir);
		return dirGetResults(*cptr, dir);
	}

	case CommandId::DIR_GET_FILES: {
		CsContextShPtr cptr;
		std::string dir;
		info.second.Deserialize(cptr, dir);
		return dirGetFiles(*cptr, dir);
	}

	case CommandId::JUDGE_STATUS: {
		CsContextShPtr cptr;
		std::string filepath;
		info.second.Deserialize(cptr, filepath);
		return judgeStatus(*cptr, filepath);
	}

	case CommandId::GET_DETECTED: {
		CsContextShPtr cptr;
		std::string filepath;
		info.second.Deserialize(cptr, filepath);
		return getDetected(*cptr, filepath);
	}

	case CommandId::GET_DETECTED_LIST: {
		CsContextShPtr cptr;
		StrSet dirSet;
		info.second.Deserialize(cptr, dirSet);
		return getDetectedList(*cptr, dirSet);
	}

	case CommandId::GET_IGNORED: {
		CsContextShPtr cptr;
		std::string filepath;
		info.second.Deserialize(cptr, filepath);
		return getIgnored(*cptr, filepath);
	}

	case CommandId::GET_IGNORED_LIST: {
		CsContextShPtr cptr;
		StrSet dirSet;
		info.second.Deserialize(cptr, dirSet);
		return getIgnoredList(*cptr, dirSet);
	}

	// Web protection
	/* TODO: should we separate command->logic mapping of CS and WP ? */
	case CommandId::CHECK_URL: {
		WpContextShPtr cptr;
		std::string url;
		info.second.Deserialize(cptr, url);
		return checkUrl(*cptr, url);
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
	CsEngineContext engineContext(m_cs);
	auto &c = engineContext.get();

	csre_cs_detected_h result;
	int ret = CSR_ERROR_NONE;
	int eret = m_cs->scanData(c, data, &result);
	if (eret != CSRE_ERROR_NONE) {
		ERROR("Engine error. engine api ret: " << eret);
		return BinaryQueue::Serialize(CSR_ERROR_ENGINE_INTERNAL, CsDetected()).pop();
	}

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(ret, CsDetected()).pop();

	auto d = convert(result);

	d.targetName.clear();

	switch (d.severity) {
	case CSR_CS_SEVERITY_LOW:
		INFO("severity low for data scanned!");
		break;

	case CSR_CS_SEVERITY_MEDIUM:
		INFO("severity medium for data scanned!");
		d.response = getUserResponse(context, d);
		break;

	case CSR_CS_SEVERITY_HIGH:
		INFO("severity high for data scanned!");
		d.response = getUserResponse(context, d);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid severity: " <<
									  static_cast<int>(d.severity)));
	}

	return BinaryQueue::Serialize(ret, d).pop();
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

RawBuffer Logic::judgeStatus(const CsContext &context,
							 const std::string &filepath)
{
	INFO("Judge Status[" << filepath << "] by engine");

	printCsContext(context);

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer Logic::getDetected(const CsContext &context,
							 const std::string &filepath)
{
	INFO("Get Detected[" << filepath << "] by engine");

	printCsContext(context);

	CsDetected detected;
	detected.targetName = "test_file";

	return BinaryQueue::Serialize(CSR_ERROR_NONE, detected).pop();
}

RawBuffer Logic::getDetectedList(const CsContext &context, const StrSet &dirSet)
{
	INFO("Get detected list logic");
	std::for_each(dirSet.begin(), dirSet.end(), [](const std::string & dir) {
		INFO("dir[" << dir << "] in directory set of get detected list logic");
	});

	printCsContext(context);

	CsDetectedPtr detected(new CsDetected());
	detected->targetName = "test_file";

	CsDetectedList list;
	list.emplace_back(std::move(detected));

	return BinaryQueue::Serialize(CSR_ERROR_NONE, list).pop();
}

RawBuffer Logic::getIgnored(const CsContext &context,
							const std::string &filepath)
{
	INFO("Get Ignored[" << filepath << "] by engine");

	printCsContext(context);

	CsDetected detected;
	detected.targetName = "test_file";

	return BinaryQueue::Serialize(CSR_ERROR_NONE, detected).pop();
}

RawBuffer Logic::getIgnoredList(const CsContext &context, const StrSet &dirSet)
{
	INFO("Get ignored list logic");
	std::for_each(dirSet.begin(), dirSet.end(), [](const std::string & dir) {
		INFO("dir[" << dir << "] in directory set of get ignored list logic");
	});

	printCsContext(context);

	CsDetectedPtr detected(new CsDetected());
	detected->targetName = "test_file";

	CsDetectedList list;
	list.emplace_back(std::move(detected));

	return BinaryQueue::Serialize(CSR_ERROR_NONE, list).pop();
}

RawBuffer Logic::checkUrl(const WpContext &context, const std::string &url)
{
	DEBUG("Logic::checkUrl start");

	WpEngineContext engineContext(m_wp);
	auto &c = engineContext.get();

	csre_wp_check_result_h result;
	int ret = CSR_ERROR_NONE;
	int eret = m_wp->checkUrl(c, url.c_str(), &result);

	if (eret != CSRE_ERROR_NONE) {
		ERROR("Engine error. engine api ret: " << eret);
		return BinaryQueue::Serialize(CSR_ERROR_ENGINE_INTERNAL, WpResult()).pop();
	}

	auto wr = convert(result);

	DEBUG("checking level.. prepare for asking user");

	switch (wr.riskLevel) {
	case CSR_WP_RISK_UNVERIFIED:
		DEBUG("url[" << url << "] risk level is unverified");
		break;

	case CSR_WP_RISK_LOW:
		DEBUG("url[" << url << "] risk level is low");
		break;

	case CSR_WP_RISK_MEDIUM:
		DEBUG("url[" << url << "] risk level is medium. let's ask user to process.");
		wr.response = getUserResponse(context, url, wr);
		break;

	case CSR_WP_RISK_HIGH:
		DEBUG("url[" << url << "] risk level is high. let's notify user to deny url.");
		wr.response = getUserResponse(context, url, wr);
		break;

	default:
		throw std::logic_error(FORMAT("Invalid level: " <<
									  static_cast<int>(wr.riskLevel)));
	}

	return BinaryQueue::Serialize(ret, wr).pop();
}

csr_cs_user_response_e Logic::getUserResponse(const CsContext &c, const CsDetected &d)
{
	if (c.askUser == CSR_CS_NOT_ASK_USER)
		return CSR_CS_NO_ASK_USER;

	Ui::CommandId cid;
	if (d.severity == CSR_CS_SEVERITY_MEDIUM) {
		if (d.targetName.empty())
			cid = Ui::CommandId::CS_PROMPT_DATA;
		else if (d.isApp)
			cid = Ui::CommandId::CS_PROMPT_APP;
		else
			cid = Ui::CommandId::CS_PROMPT_FILE;
	} else {
		if (d.targetName.empty())
			cid = Ui::CommandId::CS_NOTIFY_DATA;
		else if (d.isApp)
			cid = Ui::CommandId::CS_NOTIFY_APP;
		else
			cid = Ui::CommandId::CS_NOTIFY_FILE;
	}

	Ui::AskUser askUser;
	return askUser.cs(cid, c.popupMessage, d);
}

csr_wp_user_response_e Logic::getUserResponse(const WpContext &c, const std::string &url, const WpResult &wr)
{
	if (c.askUser == CSR_WP_NOT_ASK_USER)
		return CSR_WP_NO_ASK_USER;

	Ui::CommandId cid;
	if (wr.riskLevel == CSR_WP_RISK_MEDIUM)
		cid = Ui::CommandId::WP_PROMPT;
	else
		cid = Ui::CommandId::WP_NOTIFY;

	Ui::UrlItem item;
	item.url = url;
	item.risk = wr.riskLevel;

	Ui::AskUser askUser;
	return askUser.wp(cid, c.popupMessage, item);
}

CsDetected Logic::convert(csre_cs_detected_h &result)
{
	DEBUG("convert engine result handle to CsDetected start");

	CsDetected d;

	csre_cs_severity_level_e eseverity = CSRE_CS_SEVERITY_LOW;
	int eret = m_cs->getSeverity(result, &eseverity);
	if (eret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("Converting cs detected[seveirty]. ret: " << eret));
	d.severity = Csr::convert(eseverity);

	csre_cs_threat_type_e ethreat = CSRE_CS_THREAT_GENERIC;
	eret = m_cs->getThreatType(result, &ethreat);
	if (eret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("Converting cs detected[threat]. ret: " << eret));
	d.threat = Csr::convert(ethreat);

	eret = m_cs->getMalwareName(result, d.malwareName);
	if (eret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("Converting cs detected[name]. ret: " << eret));

	eret = m_cs->getDetailedUrl(result, d.detailedUrl);
	if (eret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("Converting cs detected[detailed url]. ret: " << eret));

	eret = m_cs->getTimestamp(result, &d.ts);
	if (eret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("Converting cs detected[timestamp]. ret: " << eret));

	return d;
}

WpResult Logic::convert(csre_wp_check_result_h &r)
{
	DEBUG("convert engine result handle to WpResult start");

	WpResult wr;

	csre_wp_risk_level_e elevel;
	int eret = m_wp->getRiskLevel(r, &elevel);
	if (eret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("Converting wp result. ret: " << eret));
	wr.riskLevel = Csr::convert(elevel);

	eret = m_wp->getDetailedUrl(r, wr.detailedUrl);
	if (eret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("Converting wp result. ret: " << eret));

	return wr;
}

}
