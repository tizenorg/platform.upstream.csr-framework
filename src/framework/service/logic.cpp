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
#include <climits>

#include "common/audit/logger.h"
#include "common/exception.h"
#include "service/type-converter.h"
#include "service/file-system.h"
#include "ui/askuser.h"
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
	m_wp(new WpLoader(WP_ENGINE_PATH)),
	m_db(new Db::Manager(RW_DBSPACE "/.csr.db", RO_DBSPACE))
{
	// TODO: Provide engine-specific res/working dirs
	int ret = m_cs->globalInit(SAMPLE_ENGINE_RO_RES_DIR,
							   SAMPLE_ENGINE_RW_WORKING_DIR);

	if (ret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "global init cs engine. ret: " << ret);

	CsEngineInfo csEngineInfo(m_cs);
	ret = m_cs->getEngineDataVersion(csEngineInfo.get(), m_csDataVersion);

	if (ret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "get cs engine data version. ret: " << ret);

	ret = m_wp->globalInit(SAMPLE_ENGINE_RO_RES_DIR, SAMPLE_ENGINE_RW_WORKING_DIR);

	if (ret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "global init wp engine. ret: " << ret);

	WpEngineInfo wpEngineInfo(m_wp);
	ret = m_wp->getEngineDataVersion(wpEngineInfo.get(), m_wpDataVersion);

	if (ret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "get wp engine data version. ret: " << ret);

	DEBUG("Service logic ctor done");
}

Logic::~Logic()
{
	int ret = m_cs->globalDeinit();

	if (ret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "global deinit cs engine. ret: " << ret);

	ret = m_wp->globalDeinit();

	if (ret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "global deinit wp engine. ret: " << ret);
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

	case CommandId::GET_SCANNABLE_FILES: {
		std::string dir;
		info.second.Deserialize(dir);
		return dirGetFiles(dir);
	}

	case CommandId::JUDGE_STATUS: {
		std::string filepath;
		int intAction;
		info.second.Deserialize(filepath, intAction);
		return judgeStatus(filepath, static_cast<csr_cs_action_e>(intAction));
	}

	case CommandId::GET_DETECTED: {
		std::string filepath;
		info.second.Deserialize(filepath);
		return getDetected(filepath);
	}

	case CommandId::GET_DETECTED_LIST: {
		StrSet dirSet;
		info.second.Deserialize(dirSet);
		return getDetectedList(dirSet);
	}

	case CommandId::GET_IGNORED: {
		std::string filepath;
		info.second.Deserialize(filepath);
		return getIgnored(filepath);
	}

	case CommandId::GET_IGNORED_LIST: {
		StrSet dirSet;
		info.second.Deserialize(dirSet);
		return getIgnoredList(dirSet);
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
		ThrowExc(InternalError, "Command id[" << static_cast<int>(info.first) <<
				 "] isn't in range.");
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
		ThrowExc(InternalError, "Invalid severity: " << static_cast<int>(d.severity));
	}

	return BinaryQueue::Serialize(ret, d).pop();
}

RawBuffer Logic::scanFileHelper(const CsContext &context,
								const std::string &filepath)
{
	CsEngineContext engineContext(m_cs);
	auto &c = engineContext.get();

	csre_cs_detected_h result;
	int ret = CSR_ERROR_NONE;
	int eret = m_cs->scanFile(c, filepath, &result);

	if (eret != CSRE_ERROR_NONE) {
		ERROR("Engine error. engine api ret: " << eret);
		return BinaryQueue::Serialize(CSR_ERROR_ENGINE_INTERNAL, CsDetected()).pop();
	}

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(ret, CsDetected()).pop();

	auto d = convert(result);

	d.targetName = filepath;

	switch (d.severity) {
	case CSR_CS_SEVERITY_LOW:
		INFO("severity low for file scanned!");
		break;

	case CSR_CS_SEVERITY_MEDIUM:
		INFO("severity medium for file scanned!");
		d.response = getUserResponse(context, d);
		break;

	case CSR_CS_SEVERITY_HIGH:
		INFO("severity high for file scanned!");
		d.response = getUserResponse(context, d);
		break;

	default:
		ThrowExc(InternalError, "Invalid severity: " << static_cast<int>(d.severity));
	}

	m_db->insertDetectedMalware(d, m_csDataVersion, d.response == CSR_CS_IGNORE);

	return BinaryQueue::Serialize(ret, d).pop();
}

RawBuffer Logic::scanFile(const CsContext &context, const std::string &filepath)
{
	auto history = m_db->getDetectedMalware(filepath);

	if (!history)
		return scanFileHelper(context, filepath);

	// history exist of malware detected for the file.
	// let's check file modified since the detected time.
	auto file = createVisitor(filepath, static_cast<time_t>(history->ts))->next();

	// file is modified since the detected time. let's remove history!
	if (!file) {
		m_db->deleteDetectedMalware(filepath);
		return scanFileHelper(context, filepath);
	}

	// file isn't modified since the detected time. history can be used.
	if (!context.askUser)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, history).pop();

	if (history->isIgnored) {
		history->response = CSR_CS_IGNORE;
	} else {
		switch (history->response = getUserResponse(context, *history)) {
		case CSR_CS_IGNORE:
			m_db->setDetectedMalwareIgnored(filepath, true);
			break;

		case CSR_CS_REMOVE:
			if (!file->remove()) {
				ERROR("Failed to remove filepath: " << filepath);
				return BinaryQueue::Serialize(CSR_ERROR_REMOVE_FAILED, CsDetected()).pop();
			}

			m_db->deleteDetectedMalware(filepath);
			break;

		case CSR_CS_SKIP:
			break;

		default:
			ThrowExc(InternalError, "Invalid response from popup: " <<
					 static_cast<int>(history->response));
		}
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, history).pop();
}

RawBuffer Logic::getScannableFiles(const std::string &dir)
{
	auto lastScanTime = m_db->getLastScanTime(dir, m_csDataVersion);

	// scan history doesn't exist
	if (lastScanTime == -1) {
		auto visitor = createVisitor(dir, LONG_MAX);
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, StrSet()).pop();
}

RawBuffer Logic::judgeStatus(const std::string &filepath,
							 csr_cs_action_e action)
{
	auto history = m_db->getDetectedMalware(filepath);

	if (!history) {
		ERROR("Target to be judged doesn't exist in db. name: " << filepath);
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER).pop();
	}

	auto file = createVisitor(filepath, static_cast<time_t>(history->ts))->next();

	if (!file) {
		ERROR("Target doesn't exist on target path on filesystem or "
			  "modified since db delta inserted. name: " << filepath);
		m_db->deleteDetectedMalware(filepath);
		// TODO: is it okay to just refresh db and return success?
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	}

	switch (action) {
	case CSR_CS_ACTION_REMOVE:
		if (!file->remove()) {
			ERROR("Failed to remove filepath: " << filepath);
			return BinaryQueue::Serialize(CSR_ERROR_REMOVE_FAILED).pop();
		}

		m_db->deleteDetectedMalware(filepath);
		break;

	case CSR_CS_ACTION_IGNORE:
		m_db->setDetectedMalwareIgnored(filepath, true);
		break;

	case CSR_CS_ACTION_UNIGNORE:
		m_db->setDetectedMalwareIgnored(filepath, false);
		break;

	default:
		ThrowExc(InternalError, "Invalid acation enum val: " <<
				 static_cast<csr_cs_action_e>(action));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer Logic::getDetected(const std::string &filepath)
{
	auto row = m_db->getDetectedMalware(filepath);

	if (row)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();
}

RawBuffer Logic::getDetectedList(const StrSet &dirSet)
{
	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : m_db->getDetectedMalwares(dir))
			rows.emplace_back(std::move(row));
	});

	return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();
}

// TODO: is this command needed?
RawBuffer Logic::getIgnored(const std::string &filepath)
{
	auto row = m_db->getDetectedMalware(filepath);

	if (row && row->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();
}

RawBuffer Logic::getIgnoredList(const StrSet &dirSet)
{
	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : m_db->getDetectedMalwares(dir))
			if (row->isIgnored)
				rows.emplace_back(std::move(row));
	});

	return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();
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
		ThrowExc(InternalError, "Invalid level: " << static_cast<int>(wr.riskLevel));
	}

	return BinaryQueue::Serialize(ret, wr).pop();
}

csr_cs_user_response_e Logic::getUserResponse(const CsContext &c,
		const CsDetected &d)
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

csr_wp_user_response_e Logic::getUserResponse(const WpContext &c,
		const std::string &url, const WpResult &wr)
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

	// getting severity level
	csre_cs_severity_level_e eseverity = CSRE_CS_SEVERITY_LOW;
	int eret = m_cs->getSeverity(result, &eseverity);

	if (eret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "getting severity of cs detected. ret: " << eret);

	d.severity = Csr::convert(eseverity);

	// getting threat type
	csre_cs_threat_type_e ethreat = CSRE_CS_THREAT_GENERIC;
	eret = m_cs->getThreatType(result, &ethreat);

	if (eret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "getting threat of cs detected. ret: " << eret);

	d.threat = Csr::convert(ethreat);

	// getting malware name
	eret = m_cs->getMalwareName(result, d.malwareName);

	if (eret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "getting malware name of cs detected. ret: " << eret);

	// getting detailed url
	eret = m_cs->getDetailedUrl(result, d.detailedUrl);

	if (eret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "getting detailed url of cs detected. ret: " << eret);

	// getting time stamp
	eret = m_cs->getTimestamp(result, &d.ts);

	if (eret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "getting time stamp of cs detected. ret: " << eret);

	return d;
}

WpResult Logic::convert(csre_wp_check_result_h &r)
{
	DEBUG("convert engine result handle to WpResult start");

	WpResult wr;

	// getting risk level
	csre_wp_risk_level_e elevel;
	int eret = m_wp->getRiskLevel(r, &elevel);

	if (eret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "getting risk level of wp result. ret: " << eret);

	wr.riskLevel = Csr::convert(elevel);

	// getting detailed url
	eret = m_wp->getDetailedUrl(r, wr.detailedUrl);

	if (eret != CSRE_ERROR_NONE)
		ThrowExc(EngineError, "getting detailed url of wp result. ret: " << eret);

	return wr;
}

}
