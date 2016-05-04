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
#include <ctime>
#include <climits>

#include "common/audit/logger.h"
#include "common/exception.h"
#include "service/type-converter.h"
#include "service/file-system.h"
#include "service/access-control.h"
#include "ui/askuser.h"
#include "csr/error.h"

#define EXCEPTION_GUARD_START          return Csr::exceptionGuard([&]() {
#define EXCEPTION_GUARD_CLOSER(retArg) }, [](int retArg) {
#define EXCEPTION_GUARD_END            });

namespace Csr {

namespace {

RawBuffer exceptionGuard(const std::function<RawBuffer()> &func,
						 const std::function<RawBuffer(int)> &closer)
{
	try {
		return func();
	} catch (const Exception &e) {
		ERROR("Exception caught. code: " << e.error() << " message: " << e.what());
		return closer(e.error());
	} catch (const std::bad_alloc &) {
		ERROR("memory alloc failed.");
		return closer(CSR_ERROR_OUT_OF_MEMORY);
	} catch (const std::exception &e) {
		ERROR("std exception: " << e.what());
		return closer(CSR_ERROR_UNKNOWN);
	} catch (...) {
		ERROR("Unknown exception occured in logic");
		return closer(CSR_ERROR_UNKNOWN);
	}
}

} // namespace anonymous

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

RawBuffer Logic::scanData(const CsContext &context, const RawBuffer &data)
{
	EXCEPTION_GUARD_START

	CsEngineContext engineContext(m_cs);
	auto &c = engineContext.get();

	csre_cs_detected_h result;
	int eret = m_cs->scanData(c, data, &result);

	if (eret != CSRE_ERROR_NONE) {
		ERROR("Engine error. engine api ret: " << eret);
		return BinaryQueue::Serialize(CSR_ERROR_ENGINE_INTERNAL, CsDetected()).pop();
	}

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();

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

	return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, CsDetected()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::scanFileHelper(const CsContext &context, const std::string &filepath)
{
	CsEngineContext engineContext(m_cs);
	auto &c = engineContext.get();

	csre_cs_detected_h result;
	int eret = m_cs->scanFile(c, filepath, &result);

	if (eret != CSRE_ERROR_NONE) {
		ERROR("Engine error. engine api ret: " << eret);
		return BinaryQueue::Serialize(CSR_ERROR_ENGINE_INTERNAL, CsDetected()).pop();
	}

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();

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

	return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();
}

RawBuffer Logic::scanFile(const CsContext &context, const std::string &filepath)
{
	EXCEPTION_GUARD_START

	DEBUG("Scan request on file: " << filepath);
	auto history = m_db->getDetectedMalware(filepath);

	if (!history)
		return scanFileHelper(context, filepath);

	DEBUG("Scan history exist on file: " << filepath);

	try {
		if (File::create(filepath, static_cast<time_t>(history->ts))) {
			DEBUG("file[" << filepath << "] is modified since the detected time. "
				  "let's remove history and re-scan");
			m_db->deleteDetectedMalware(filepath);
			return scanFileHelper(context, filepath);
		}
	} catch (const FileSystemError &e) {
		ERROR("file doesn't exist or type isn't regular: " << filepath <<
			  " " << e.what());
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER, CsDetected()).pop();
	}

	DEBUG("file[" << filepath << "] isn't modified since the detected time. "
		  "history can be used.");

	if (!context.askUser) {
		return BinaryQueue::Serialize(CSR_ERROR_NONE, history).pop();
	} else if (history->isIgnored) {
		history->response = CSR_CS_IGNORE;
		return BinaryQueue::Serialize(CSR_ERROR_NONE, history).pop();
	}

	DEBUG("user response is needed.");

	switch (history->response = getUserResponse(context, *history)) {
	case CSR_CS_IGNORE:
		m_db->setDetectedMalwareIgnored(filepath, true);
		break;

	case CSR_CS_REMOVE:
		try {
			auto file = File::create(filepath);

			if (!file && !file->remove()) {
				ERROR("Failed to remove filepath: " << filepath);
				return BinaryQueue::Serialize(CSR_ERROR_REMOVE_FAILED, history).pop();
			}
		} catch (const FileDoNotExist &) {
			WARN("File already removed... : " << filepath);
		} catch (const FileSystemError &) {
			WARN("File type is changed... it's considered as different file "
				 "in same path: " << filepath);
		}

		m_db->deleteDetectedMalware(filepath);
		break;

	case CSR_CS_SKIP:
		break;

	default:
		ThrowExc(InternalError, "Invalid response from popup: " <<
				 static_cast<int>(history->response));
	}

	DEBUG("file[" << filepath << "] user response: " << static_cast<int>(history->response));

	return BinaryQueue::Serialize(CSR_ERROR_NONE, history).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, CsDetected()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::getScannableFiles(const std::string &dir)
{
	EXCEPTION_GUARD_START

	auto lastScanTime = m_db->getLastScanTime(dir, m_csDataVersion);

	FsVisitorPtr visitor;

	try {
		visitor = FsVisitor::create(dir, lastScanTime);
	} catch (const FileDoNotExist &) {
		WARN("Directory isn't exist: " << dir << " return success with empty file set "
			 "to skip it softly.");
		return BinaryQueue::Serialize(CSR_ERROR_NONE, StrSet()).pop();
	} catch (const FileSystemError &) {
		WARN("Directory isn't directory... file type changed: " << dir << " return "
			 "empty file set to skip it softly.");
		return BinaryQueue::Serialize(CSR_ERROR_NONE, StrSet()).pop();
	}

	StrSet fileset;

	while (auto file = visitor->next()) {
		if (hasPermToRemove(file->getPath())) {
			DEBUG("Scannable file[" << file->getPath() << "]");
			fileset.insert(file->getPath());
		}
	}

	if (lastScanTime != -1) {
		// for case: scan history exist and not modified.
		for (auto &row : m_db->getDetectedMalwares(dir))
			fileset.insert(row->targetName);
	}

	// update last scan time before start.
	// to set scan time early is safe because file which is modified between
	// scan start time and end time will be traversed by FsVisitor and re-scanned
	// being compared to start time as modified since.
	m_db->insertLastScanTime(dir, time(nullptr), m_csDataVersion);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, fileset).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, StrSet()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::judgeStatus(const std::string &filepath, csr_cs_action_e action)
{
	EXCEPTION_GUARD_START

	auto history = m_db->getDetectedMalware(filepath);

	if (!history) {
		ERROR("Target to be judged doesn't exist in db. name: " << filepath);
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER).pop();
	}

	try {
		if (File::create(filepath, static_cast<time_t>(history->ts))) {
			ERROR("Target modified since db delta inserted. name: " << filepath);
			m_db->deleteDetectedMalware(filepath);
			// TODO: is it okay to just refresh db and return success?
			return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
		}
	} catch (const FileSystemError &e) {
		ERROR("file doesn't exist or type isn't regular: " << filepath <<
			  " " << e.what());
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER).pop();
	}

	switch (action) {
	case CSR_CS_ACTION_REMOVE:
		try {
			auto file = File::create(filepath);

			if (!file && !file->remove()) {
				ERROR("Failed to remove filepath: " << filepath);
				return BinaryQueue::Serialize(CSR_ERROR_REMOVE_FAILED).pop();
			}
		} catch (const FileDoNotExist &) {
			WARN("File already removed... : " << filepath);
		} catch (const FileSystemError &) {
			WARN("File type is changed... it's considered as different file "
				 "in same path: " << filepath);
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

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::getDetected(const std::string &filepath)
{
	EXCEPTION_GUARD_START

	auto row = m_db->getDetectedMalware(filepath);

	if (row)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, CsDetected()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::getDetectedList(const StrSet &dirSet)
{
	EXCEPTION_GUARD_START

	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : m_db->getDetectedMalwares(dir))
			rows.emplace_back(std::move(row));
	});

	return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, Db::RowShPtrs()).pop();

	EXCEPTION_GUARD_END
}

// TODO: is this command needed?
RawBuffer Logic::getIgnored(const std::string &filepath)
{
	EXCEPTION_GUARD_START

	auto row = m_db->getDetectedMalware(filepath);

	if (row && row->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, CsDetected()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::getIgnoredList(const StrSet &dirSet)
{
	EXCEPTION_GUARD_START

	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : m_db->getDetectedMalwares(dir))
			if (row->isIgnored)
				rows.emplace_back(std::move(row));
	});

	return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, Db::RowShPtrs()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::checkUrl(const WpContext &context, const std::string &url)
{
	EXCEPTION_GUARD_START

	DEBUG("Logic::checkUrl start");

	WpEngineContext engineContext(m_wp);
	auto &c = engineContext.get();

	csre_wp_check_result_h result;
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

	return BinaryQueue::Serialize(CSR_ERROR_NONE, wr).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, WpResult()).pop();

	EXCEPTION_GUARD_END
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

csr_wp_user_response_e Logic::getUserResponse(const WpContext &c, const std::string &url,
		const WpResult &wr)
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
