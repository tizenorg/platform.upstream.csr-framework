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
 * @file        cs-logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "service/cs-logic.h"

#include <utility>
#include <algorithm>
#include <ctime>
#include <climits>

#include "common/audit/logger.h"
#include "service/type-converter.h"
#include "service/engine-error-converter.h"
#include "service/core-usage.h"
#include "ui/askuser.h"
#include "csr/error.h"

namespace Csr {

namespace {

void setCoreUsage(const csr_cs_core_usage_e &cu)
{
	switch (cu) {
	case CSR_CS_USE_CORE_HALF:
	case CSR_CS_USE_CORE_SINGLE:
		CpuUsageManager::set(cu);
		break;

	default:
		break;
	}
}

} // namespace anonymous

CsLogic::CsLogic(CsLoader &loader, Db::Manager &db) : m_loader(loader), m_db(db)
{
	CsEngineInfo csEngineInfo(this->m_loader);
	toException(this->m_loader.getEngineDataVersion(csEngineInfo.get(),
				this->m_dataVersion));
}

CsLogic::~CsLogic()
{
}

RawBuffer CsLogic::scanData(const CsContext &context, const RawBuffer &data)
{
	EXCEPTION_GUARD_START

	if (this->m_db.getEngineState(CSR_ENGINE_CS) != CSR_ENABLE)
		ThrowExc(EngineDisabled, "engine is disabled");

	setCoreUsage(context.coreUsage);

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	csre_cs_detected_h result;

	toException(this->m_loader.scanData(c, data, &result));

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	auto d = convert(result, std::string());

	d.response = getUserResponse(context, d);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::scanAppOnCloud(const CsContext &context,
								  const std::string &pkgPath,
								  const std::string &pkgId)
{
	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	csre_cs_detected_h result;
	toException(this->m_loader.scanAppOnCloud(c, pkgPath, &result));

	if (!result)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	auto detected = convert(result, pkgPath);
	detected.isApp = true;
	detected.pkgId = pkgId;
	detected.response = getUserResponse(context, detected);

	return handleUserResponse(detected);
}

CsDetectedPtr CsLogic::scanAppDelta(const std::string &pkgPath, const std::string &pkgId)
{
	auto starttime = time(nullptr);

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	// traverse files in app and take which is more danger than riskiest.
	auto visitor = FsVisitor::create(
					   pkgPath,
					   this->m_db.getLastScanTime(pkgPath, this->m_dataVersion));

	CsDetectedPtr riskiest;

	while (auto file = visitor->next()) {
		csre_cs_detected_h result;
		toException(this->m_loader.scanFile(c, file->getPath(), &result));

		if (!result)
			continue;

		INFO("New malware detected on file: " << file->getPath());

		auto candidate = convert(result, file->getPath());
		candidate.isApp = true;
		candidate.pkgId = pkgId;
		this->m_db.insertDetectedMalware(candidate, this->m_dataVersion);

		if (!riskiest)
			riskiest.reset(new CsDetected(std::move(candidate)));
		else if (*riskiest < candidate)
			*riskiest = std::move(candidate);
	}

	this->m_db.insertLastScanTime(pkgPath, starttime, this->m_dataVersion);

	if (riskiest) {
		INFO("Riskiest malware selected in pkg: " << pkgPath);
		riskiest->targetName = pkgPath;
	}

	return riskiest;
}

RawBuffer CsLogic::scanApp(const CsContext &context, const std::string &path)
{
	auto fileptr = File::create(path);

	if (!fileptr)
		ThrowExc(InternalError, "fileptr shouldn't be null because no modified since.");

	if (!fileptr->isInApp())
		ThrowExc(InternalError, "fileptr should be in app.");

	const auto &pkgPath = fileptr->getAppPkgPath();
	const auto &pkgId = fileptr->getAppPkgId();

	if (context.isScanOnCloud)
		return this->scanAppOnCloud(context, pkgPath, pkgId);

	auto riskiest = this->scanAppDelta(pkgPath, pkgId);
	auto history = this->m_db.getDetectedMalware(pkgPath);

	if (riskiest && history) {
		if (*riskiest > *history) {
			INFO("new malware found and more risky! history should be updated.");
			this->m_db.insertDetectedMalware(*riskiest, this->m_dataVersion);
		} else {
			INFO("new malware is found but not riskier than history. history reusable.");
			history->response = history->isIgnored
								? CSR_CS_IGNORE : this->getUserResponse(context, *history);
			return this->handleUserResponse(*history);
		}
	} else if (riskiest && !history) {
		INFO("new malware found and no history exist! history should be inserted.");
		this->m_db.insertDetectedMalware(*riskiest, this->m_dataVersion);
	} else if (!riskiest && history) {
		INFO("no malware found and history exist! history reusable.");
		history->response = history->isIgnored
							? CSR_CS_IGNORE : this->getUserResponse(context, *history);
		return this->handleUserResponse(*history);
	} else {
		INFO("no malware found and no history exist! it's clean!");
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	}

	// new and more risky malware found and db updated case only left.
	riskiest->response = this->getUserResponse(context, *riskiest);
	return this->handleUserResponse(*riskiest);
}

RawBuffer CsLogic::scanFileWithoutDelta(const CsContext &context,
										const std::string &filepath, FilePtr &&fileptr)
{
	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	csre_cs_detected_h result;
	toException(this->m_loader.scanFile(c, filepath, &result));

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	auto d = convert(result, filepath);

	this->m_db.insertDetectedMalware(d, this->m_dataVersion);

	d.response = getUserResponse(context, d);
	return handleUserResponse(d, std::forward<FilePtr>(fileptr));
}

RawBuffer CsLogic::scanFile(const CsContext &context, const std::string &filepath)
{
	EXCEPTION_GUARD_START

	if (this->m_db.getEngineState(CSR_ENGINE_CS) != CSR_ENABLE)
		ThrowExc(EngineDisabled, "engine is disabled");

	setCoreUsage(context.coreUsage);

	if (File::isInApp(filepath))
		return scanApp(context, filepath);

	DEBUG("Scan request on file: " << filepath);

	auto history = this->m_db.getDetectedMalware(filepath);

	FilePtr fileptr;

	// if history exist, fileptr can be null because of modified since value
	// from history.
	if (history)
		fileptr = File::create(filepath, static_cast<time_t>(history->ts));
	else
		fileptr = File::create(filepath);

	// non-null fileptr means the file is modified since the last history
	// OR there's no history at all.
	if (fileptr) {
		if (history)
			this->m_db.deleteDetectedMalware(filepath);

		if (fileptr->isDir())
			ThrowExc(FileSystemError, "file type shouldn't be directory: " << filepath);

		DEBUG("file[" << filepath << "] is modified since the detected time. "
			  "let's remove history and re-scan");
		return scanFileWithoutDelta(context, filepath, std::move(fileptr));
	}

	DEBUG("Usable scan history exist on file: " << filepath);

	history->response = history->isIgnored
						? CSR_CS_IGNORE : getUserResponse(context, *history);
	return handleUserResponse(*history);

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

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
RawBuffer CsLogic::getScannableFiles(const std::string &dir)
{
	EXCEPTION_GUARD_START

	if (this->m_db.getEngineState(CSR_ENGINE_CS) != CSR_ENABLE)
		ThrowExc(EngineDisabled, "engine is disabled");

	auto lastScanTime = this->m_db.getLastScanTime(dir, this->m_dataVersion);

	auto visitor = FsVisitor::create(dir, lastScanTime);

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
		for (auto &row : this->m_db.getDetectedMalwares(dir)) {
			try {
				auto fileptr = File::create(row->targetName);

				if (fileptr->isInApp())
					fileset.insert(fileptr->getAppPkgPath());
				else
					fileset.insert(fileptr->getPath());
			} catch (const FileDoNotExist &) {
				this->m_db.deleteDetectedMalware(row->targetName);
			} catch (const FileSystemError &) {
				this->m_db.deleteDetectedMalware(row->targetName);
			}
		}
	}

	// update last scan time before start.
	// to set scan time early is safe because file which is modified between
	// scan start time and end time will be traversed by FsVisitor and re-scanned
	// being compared to start time as modified since.
	this->m_db.insertLastScanTime(dir, time(nullptr), this->m_dataVersion);

	return BinaryQueue::Serialize(CSR_ERROR_NONE, fileset).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::judgeStatus(const std::string &filepath, csr_cs_action_e action)
{
	EXCEPTION_GUARD_START

	auto history = this->m_db.getDetectedMalware(filepath);

	if (!history) {
		ERROR("Target to be judged doesn't exist in db. name: " << filepath);
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER).pop();
	}

	auto fileptr = File::create(filepath, static_cast<time_t>(history->ts));

	if (fileptr) {
		ERROR("Target modified since db delta inserted. name: " << filepath);
		this->m_db.deleteDetectedMalware(filepath);

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

		this->m_db.deleteDetectedMalware(filepath);
		break;

	case CSR_CS_ACTION_IGNORE:
		this->m_db.setDetectedMalwareIgnored(filepath, true);
		break;

	case CSR_CS_ACTION_UNIGNORE:
		this->m_db.setDetectedMalwareIgnored(filepath, false);
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

RawBuffer CsLogic::getDetected(const std::string &filepath)
{
	EXCEPTION_GUARD_START

	auto row = this->m_db.getDetectedMalware(filepath);

	if (row)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::getDetectedList(const StrSet &dirSet)
{
	EXCEPTION_GUARD_START

	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : this->m_db.getDetectedMalwares(dir))
			rows.emplace_back(std::move(row));
	});

	if (rows.empty())
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

	EXCEPTION_GUARD_END
}

// TODO: is this command needed?
RawBuffer CsLogic::getIgnored(const std::string &filepath)
{
	EXCEPTION_GUARD_START

	auto row = this->m_db.getDetectedMalware(filepath);

	if (row && row->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret, CsDetected()).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::getIgnoredList(const StrSet &dirSet)
{
	EXCEPTION_GUARD_START

	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : this->m_db.getDetectedMalwares(dir))
			if (row->isIgnored)
				rows.emplace_back(std::move(row));
	});

	if (rows.empty())
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();

	EXCEPTION_GUARD_CLOSER(ret)

	return BinaryQueue::Serialize(ret).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::handleUserResponse(const CsDetected &d, FilePtr &&fileptr)
{
	switch (d.response) {
	case CSR_CS_IGNORE:
		this->m_db.setDetectedMalwareIgnored(d.targetName, true);
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

		this->m_db.deleteDetectedMalware(d.targetName);
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

csr_cs_user_response_e CsLogic::getUserResponse(const CsContext &c, const CsDetected &d)
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

CsDetected CsLogic::convert(csre_cs_detected_h &result, const std::string &targetName)
{
	DEBUG("convert engine result handle to CsDetected start");

	CsDetected d;

	d.targetName = targetName;

	csre_cs_severity_level_e eseverity = CSRE_CS_SEVERITY_LOW;

	toException(this->m_loader.getSeverity(result, &eseverity));
	toException(this->m_loader.getMalwareName(result, d.malwareName));
	toException(this->m_loader.getDetailedUrl(result, d.detailedUrl));
	toException(this->m_loader.getTimestamp(result, &d.ts));

	d.severity = Csr::convert(eseverity);

	return d;
}

}
