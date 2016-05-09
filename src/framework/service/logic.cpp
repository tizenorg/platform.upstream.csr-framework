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

	auto d = convert(result, std::string());

	d.response = getUserResponse(context, d);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, CsDetected()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer Logic::scanAppWithoutDelta(const CsContext &context, const FilePtr &appDirPtr)
{
	const auto &pkgPath = appDirPtr->getAppPkgPath();

	auto starttime = time(nullptr);

	CsEngineContext engineContext(m_cs);
	auto &c = engineContext.get();

	CsDetected detected;

	if (context.isScanOnCloud) {
		csre_cs_detected_h result;
		int eret = m_cs->scanAppOnCloud(c, pkgPath, &result);

		if (eret != CSRE_ERROR_NONE) {
			ERROR("engine error. engine api ret: " << eret);
			return BinaryQueue::Serialize(CSR_ERROR_ENGINE_INTERNAL, CsDetected()).pop();
		}

		if (result)
			detected = convert(result, pkgPath);
	} else {
		// traverse files in app. take which is more danger than detected.
		FsVisitorPtr visitor;

		try {
			visitor = FsVisitor::create(pkgPath,
										m_db->getLastScanTime(pkgPath, m_csDataVersion));
		} catch (const FileDoNotExist &) {
			ERROR("app directory doesn't exist: " << pkgPath);
			return BinaryQueue::Serialize(CSR_ERROR_FILE_DO_NOT_EXIST, CsDetected()).pop();
		} catch (const FileSystemError &) {
			ERROR("app directory type isn't dir: " << pkgPath);
			return BinaryQueue::Serialize(CSR_ERROR_FILE_SYSTEM, CsDetected()).pop();
		}

		while (auto file = visitor->next()) {
			csre_cs_detected_h result;
			int eret = m_cs->scanFile(c, file->getPath(), &result);

			if (eret != CSRE_ERROR_NONE) {
				ERROR("engine error. engine api ret: " << eret);
				return BinaryQueue::Serialize(CSR_ERROR_ENGINE_INTERNAL, CsDetected()).pop();
			}

			if (result) {
				auto d = convert(result, pkgPath);

				if (!detected.hasValue() || detected.severity < d.severity)
					detected = std::move(d);
			}
		}
	}

	m_db->insertLastScanTime(pkgPath, starttime, m_csDataVersion);

	if (detected.hasValue()) {
		detected.isApp = true;
		detected.pkgId = appDirPtr->getAppPkgId();

		// cloud scan detected history inserted by targetname = app base directory path
		m_db->insertDetectedMalware(detected, m_csDataVersion, false);
		detected.response = getUserResponse(context, detected);
		return handleUserResponse(detected);
	} else {
		return BinaryQueue::Serialize(CSR_ERROR_NONE, detected).pop();
	}
}

RawBuffer Logic::scanApp(const CsContext &context, const std::string &path)
{
	FilePtr fileptr;

	try {
		fileptr = File::create(path);
	} catch (const FileDoNotExist &) {
		return BinaryQueue::Serialize(CSR_ERROR_FILE_DO_NOT_EXIST, CsDetected()).pop();
	} catch (const FileSystemError &) {
		return BinaryQueue::Serialize(CSR_ERROR_FILE_SYSTEM, CsDetected()).pop();
	}

	if (!fileptr)
		ThrowExc(InternalError, "fileptr shouldn't be null because no modified since.");

	if (!fileptr->isInApp())
		ThrowExc(InternalError, "fileptr should be in app.");

	const auto &pkgPath = fileptr->getAppPkgPath();

	auto lastScanTime = m_db->getLastScanTime(pkgPath, m_csDataVersion);

	try {
		auto visitor = FsVisitor::create(pkgPath, lastScanTime);

		// visitor with the last scan time has at least a file to traverse
		// which means there's file which is modified since the last scan time.
		// if there's no scan history so lastScanTime is -1, all existing files in path
		// are traversable. visitor class isn't reusable because it already wasted a
		// file to check.
		if (visitor->next())
			return scanAppWithoutDelta(context, fileptr);
	} catch (const FileDoNotExist &) {
		ERROR("app directory doesn't exist: " << pkgPath);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_DO_NOT_EXIST, CsDetected()).pop();
	} catch (const FileSystemError &) {
		ERROR("app directory type isn't dir: " << pkgPath);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_SYSTEM, CsDetected()).pop();
	}

	auto history = m_db->getDetectedMalware(pkgPath);
	if (!history)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, CsDetected()).pop();

	history->response = history->isIgnored
			? CSR_CS_IGNORE : getUserResponse(context, *history);
	return handleUserResponse(*history);
}

RawBuffer Logic::scanFileWithoutDelta(const CsContext &context,
									  const std::string &filepath, FilePtr &&fileptr)
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

	auto d = convert(result, filepath);

	m_db->insertDetectedMalware(d, m_csDataVersion, false);

	d.response = getUserResponse(context, d);
	return handleUserResponse(d, std::forward<FilePtr>(fileptr));
}

RawBuffer Logic::scanFile(const CsContext &context, const std::string &filepath)
{
	EXCEPTION_GUARD_START

	if (File::isInApp(filepath))
		return scanApp(context, filepath);

	DEBUG("Scan request on file: " << filepath);

	auto history = m_db->getDetectedMalware(filepath);

	FilePtr fileptr;

	try {
		// if history exist, fileptr can be null because of modified since value
		// from history.
		if (history)
			fileptr = File::create(filepath, static_cast<time_t>(history->ts));
		else
			fileptr = File::create(filepath);
	} catch (const FileDoNotExist &) {
		ERROR("file doesn't exist: " << filepath);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_DO_NOT_EXIST, CsDetected()).pop();
	} catch (const FileSystemError &) {
		ERROR("file type isn't regular or dir: " << filepath);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_SYSTEM, CsDetected()).pop();
	}

	// non-null fileptr means the file is modified since the last history
	// OR there's no history at all.
	if (fileptr) {
		if (history)
			m_db->deleteDetectedMalware(filepath);

		if (fileptr->isDir()) {
			ERROR("file type invalid here. it shouldn't be directory: " << filepath);
			return BinaryQueue::Serialize(CSR_ERROR_FILE_SYSTEM, CsDetected()).pop();
		} else {
			DEBUG("file[" << filepath << "] is modified since the detected time. "
				  "let's remove history and re-scan");
			return scanFileWithoutDelta(context, filepath, std::move(fileptr));
		}
	}

	DEBUG("Usable scan history exist on file: " << filepath);

	history->response = history->isIgnored
			? CSR_CS_IGNORE : getUserResponse(context, *history);
	return handleUserResponse(*history);

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, CsDetected()).pop();

	EXCEPTION_GUARD_END
}

// Application in input param directory will be treated as one item.
// Application base directory path is inserted to file set.
// e.g., input param dir : "/opt/usr" (applications in "/opt/usr/apps")
//       ls /opt/usr/ :
//           /opt/usr/file-not-in-app1
//           /opt/usr/file-not-in-app2
//           /opt/usr/apps/org.tizen.tutorial
//           /opt/usr/apps/org.tizen.tutorial/file-in-app1
//           /opt/usr/apps/org.tizen.tutorial/file-in-app2
//           /opt/usr/apps/org.tizen.message/file-in-app1
//           /opt/usr/apps/org.tizen.message/file-in-app2
//           /opt/usr/apps/org.tizen.flash/file-in-app1
//           /opt/usr/apps/org.tizen.flash/file-in-app2
//
//           and detected history exist on...
//           /opt/usr/apps/org.tizen.message/file-in-app2
//           /opt/usr/apps/org.tizen.flash (If target name is app base directory path,
//                                          it's detected by scan on cloud)
//
//       output scannable file set will be:
//           1) /opt/usr/file-not-in-app1
//           2) /opt/usr/file-not-in-app2
//           3) /opt/usr/apps/org.tizen.tutorial (app base directory path)
//           4) /opt/usr/apps/org.tizen.message  (app base directory path)
//           5) /opt/usr/apps/org.tizen.flash    (app base directory path)
//           % items which has detected history is included in list as well.
RawBuffer Logic::getScannableFiles(const std::string &dir)
{
	EXCEPTION_GUARD_START

	auto lastScanTime = m_db->getLastScanTime(dir, m_csDataVersion);

	FsVisitorPtr visitor;

	try {
		visitor = FsVisitor::create(dir, lastScanTime);
	} catch (const FileDoNotExist &) {
		ERROR("Directory isn't exist: " << dir);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_DO_NOT_EXIST, StrSet()).pop();
	} catch (const FileSystemError &) {
		ERROR("Directory isn't directory... file type changed: " << dir);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_SYSTEM, StrSet()).pop();
	}

	StrSet fileset;

	while (auto file = visitor->next()) {
		// app is removed by pkgmgr API so we need not permission to remove it directly
		if (file->isInApp()) {
			DEBUG("Scannable app: " << file->getAppPkgPath());
			fileset.insert(file->getAppPkgPath());
		} else if (hasPermToRemove(file->getPath())) {
			DEBUG("Scannable file: " << file->getPath());
			fileset.insert(file->getPath());
		}
	}

	if (lastScanTime != -1) {
		// for case: scan history exist and not modified.
		for (auto &row : m_db->getDetectedMalwares(dir)) {
			try {
				auto fileptr = File::create(row->targetName);
				if (fileptr->isInApp())
					fileset.insert(fileptr->getAppPkgPath());
				else
					fileset.insert(fileptr->getPath());
			} catch (const FileDoNotExist &) {
				m_db->deleteDetectedMalware(row->targetName);
			} catch (const FileSystemError &) {
				m_db->deleteDetectedMalware(row->targetName);
			}
		}
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

	FilePtr fileptr;

	try {
		fileptr = File::create(filepath, static_cast<time_t>(history->ts));
	} catch (const FileDoNotExist &) {
		ERROR("file doesn't exist: " << filepath);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_DO_NOT_EXIST).pop();
	} catch (const FileSystemError &e) {
		ERROR("file type isn't regular: " << filepath);
		return BinaryQueue::Serialize(CSR_ERROR_FILE_SYSTEM).pop();
	}

	if (fileptr) {
		ERROR("Target modified since db delta inserted. name: " << filepath);
		m_db->deleteDetectedMalware(filepath);
		// TODO: is it okay to just refresh db and return success?
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
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

RawBuffer Logic::handleUserResponse(const CsDetected &d, FilePtr &&fileptr)
{
	switch (d.response) {
	case CSR_CS_IGNORE:
		m_db->setDetectedMalwareIgnored(d.targetName, true);
		break;

	case CSR_CS_REMOVE:
		try {
			FilePtr _fileptr;

			if (fileptr)
				_fileptr = std::forward<FilePtr>(fileptr);
			else
				_fileptr = File::create(d.targetName);

			if (!_fileptr->remove()) {
				ERROR("Failed to remove file: " << d.targetName);
				return BinaryQueue::Serialize(CSR_ERROR_REMOVE_FAILED, d).pop();
			}
		} catch (const FileDoNotExist &) {
			WARN("File already removed.: " << d.targetName);
		} catch (const FileSystemError &) {
			WARN("File type is changed. it's considered as different file: " << d.targetName);
		}

		m_db->deleteDetectedMalware(d.targetName);
		break;

	case CSR_CS_SKIP:
	case CSR_CS_NO_ASK_USER:
		break;

	default:
		ThrowExc(InternalError, "Invalid response from popup: " <<
				 static_cast<int>(d.response));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();
}

csr_cs_user_response_e Logic::getUserResponse(const CsContext &c, const CsDetected &d)
{
	if (c.askUser == CSR_CS_NOT_ASK_USER)
		return CSR_CS_NO_ASK_USER;

	Ui::CommandId cid;

	switch (d.severity) {
	case CSR_CS_SEVERITY_LOW:
	case CSR_CS_SEVERITY_MEDIUM:
		if (d.targetName.empty())
			cid = Ui::CommandId::CS_PROMPT_DATA;
		else if (d.isApp)
			cid = Ui::CommandId::CS_PROMPT_APP;
		else
			cid = Ui::CommandId::CS_PROMPT_FILE;

		break;

	case CSR_CS_SEVERITY_HIGH:
		if (d.targetName.empty())
			cid = Ui::CommandId::CS_NOTIFY_DATA;
		else if (d.isApp)
			cid = Ui::CommandId::CS_NOTIFY_APP;
		else
			cid = Ui::CommandId::CS_NOTIFY_FILE;

		break;

	default:
		ThrowExc(InternalError, "Invalid severity: " << static_cast<int>(d.severity));
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

CsDetected Logic::convert(csre_cs_detected_h &result, const std::string &targetName)
{
	DEBUG("convert engine result handle to CsDetected start");

	CsDetected d;

	d.targetName = targetName;

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
