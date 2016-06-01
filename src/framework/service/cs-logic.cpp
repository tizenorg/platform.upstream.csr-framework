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
#include "service/exception.h"
#include "ui/askuser.h"
#include <csr-error.h>

namespace Csr {

namespace {

void setCoreUsage(const csr_cs_core_usage_e &cu)
{
	switch (cu) {
	case CSR_CS_CORE_USAGE_HALF:
	case CSR_CS_CORE_USAGE_SINGLE:
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

RawBuffer CsLogic::scanData(const CsContext &context, const RawBuffer &data)
{
	EXCEPTION_GUARD_START

	if (this->m_db.getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(EngineDisabled, "engine is disabled");

	setCoreUsage(context.coreUsage);

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	csre_cs_detected_h result;

	toException(this->m_loader.scanData(c, data, &result));

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	auto d = this->convert(result, std::string());

	return this->handleAskUser(context, d);

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

	auto detected = this->convert(result, pkgPath);
	detected.isApp = true;
	detected.pkgId = pkgId;

	return this->handleAskUser(context, detected);
}

CsDetectedPtr CsLogic::scanAppDelta(const std::string &pkgPath, const std::string &pkgId,
									std::string &riskiestPath)
{
	auto starttime = time(nullptr);

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	auto lastScanTime = this->m_db.getLastScanTime(pkgPath, this->m_dataVersion);

	// traverse files in app and take which is more danger than riskiest
	auto visitor = FsVisitor::create(pkgPath, lastScanTime);

	CsDetectedPtr riskiest;

	while (auto file = visitor->next()) {
		DEBUG("Scan file by engine: " << file->getPath());

		csre_cs_detected_h result;
		toException(this->m_loader.scanFile(c, file->getPath(), &result));

		if (!result) {
			if (lastScanTime != -1)
				this->m_db.deleteDetectedByFilepathOnPath(file->getPath());

			continue;
		}

		INFO("New malware detected on file: " << file->getPath());

		auto candidate = this->convert(result, pkgPath);
		candidate.isApp = true;
		candidate.pkgId = pkgId;

		this->m_db.insertName(pkgPath);
		this->m_db.insertDetected(candidate, file->getPath(), this->m_dataVersion);

		if (!riskiest) {
			riskiest.reset(new CsDetected(std::move(candidate)));
			riskiestPath = file->getPath();
		} else if (*riskiest < candidate) {
			*riskiest = std::move(candidate);
			riskiestPath = file->getPath();
		}
	}

	this->m_db.insertLastScanTime(pkgPath, starttime, this->m_dataVersion);

	return riskiest;
}

RawBuffer CsLogic::scanApp(const CsContext &context, const std::string &path)
{
	FilePtr fileptr;
	try {
		fileptr = File::create(path);
	} catch (const FileDoNotExist &e) {
		WARN("Pinned file[" << path << " in app doesn't exist or perm denied.");
	} catch (const FileSystemError &e) {
		WARN("Pinned file[" << path << " in app type isn't regular file or dir.");
	}

	// try again to create FilePtr by package path
	if (!fileptr) {
		try {
			fileptr = File::create(File::getPkgPath(path));
		} catch (const FileDoNotExist &e) {
			WARN("Package path of file[" << path << "] doesn't exist or perm denied.");
		} catch (const FileSystemError &e) {
			WARN("Package path of file[" << path << "] type isn't regular file or dir.");
		}
	}

	if (!fileptr->isInApp())
		ThrowExc(InternalError, "fileptr should be in app.");

	if (!fileptr->isRemovable())
		ThrowExc(PermDenied, "app[" << fileptr->getAppPkgPath() << "] isn't removable.");

	const auto &pkgPath = fileptr->getAppPkgPath();
	const auto &pkgId = fileptr->getAppPkgId();

	if (context.isScanOnCloud)
		return this->scanAppOnCloud(context, pkgPath, pkgId);

	// old history
	auto history = this->m_db.getWorstByPkgId(pkgId);
	// riskiest detected among newly scanned files
	std::string riskiestPath;
	auto riskiest = this->scanAppDelta(pkgPath, pkgId, riskiestPath);
	// history after delta scan. if worst file is changed, it's rescanned in scanAppDelta
	// and deleted from db if it's cured. if history != nullptr && after == nullptr,
	// it means worst detected item is cured anyway.
	auto after = this->m_db.getWorstByPkgId(pkgId);
	if (history && after && riskiest) {
		if (*history < *riskiest) {
			INFO("worst case is remained but the more worst newly detected. on pkg[" <<
				 pkgPath << "]");
			if (history->isIgnored)
				this->m_db.updateIgnoreFlag(pkgPath, false);

			this->m_db.insertWorst(pkgId, pkgPath, riskiestPath);

			return this->handleAskUser(context, *riskiest);
		} else {
			INFO("worst case is remained and can be re-used on pkg[" << pkgPath << "]");
			if (history->isIgnored)
				return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

			return this->handleAskUser(context, *history);
		}
	} else if (history && after && !riskiest) {
		INFO("worst case is remained and NO new detected. history can be re-used. "
			 "on pkg[" << pkgPath << "]");
		if (history->isIgnored)
			return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

		return this->handleAskUser(context, *history);
	} else if (history && !after && riskiest) {
		INFO("worst case is deleted but new detected. we have to find out "
			 "worse case in db and compare it with riskiest first. on pkg[" << pkgPath <<
			 "]");
		Db::RowShPtr worse;
		for (auto &row : this->m_db.getDetectedByFilepathOnDir(pkgPath))
			if (!worse || *worse < *row)
				worse = std::move(row);

		if (!worse) {
			INFO("No detected malware found in db.... Newly detected malware is removed by "
				 "other client. Handle it as fully clean case.");
			return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
		}

		if (*riskiest < *worse) {
			INFO("worse case in db is worse than riskiest. on pkg[" << pkgPath << "]");
			riskiestPath = worse->fileInAppPath;
			*riskiest = std::move(*worse);
		}

		if (*history < *riskiest) {
			INFO("worst case is deleted but the more worst newly detected. on pkg[" <<
				 pkgPath << "]");
			if (history->isIgnored)
				this->m_db.updateIgnoreFlag(pkgPath, false);

			this->m_db.insertWorst(pkgId, pkgPath, riskiestPath);

			return this->handleAskUser(context, *riskiest);
		} else {
			INFO("worst case is deleted but same or less level newly detected. on pkg[" <<
				 pkgPath << "]");
			this->m_db.insertWorst(pkgId, pkgPath, riskiestPath);

			if (history->isIgnored)
				return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

			return this->handleAskUser(context, *riskiest);
		}
	} else if (history && !after && !riskiest) {
		auto rows = this->m_db.getDetectedByFilepathOnDir(pkgPath);

		if (!rows.empty()) {
			INFO("worst case is deleted cascadingly and NO new detected and "
				 "worse case exist on pkg[" << pkgPath << "]. insert it to worst.");
			Db::RowShPtr worse;
			for (auto &row : rows)
				if (!worse || *worse < *row)
					worse = std::move(row);

			if (worse) {
				this->m_db.insertWorst(pkgId, pkgPath, worse->fileInAppPath);

				if (worse->isIgnored)
					return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

				return this->handleAskUser(context, *worse);
			}
		}

		INFO("worst case is deleted cascadingly and NO new detected and "
			 "NO worse case. the pkg[" << pkgPath << "] is clean.");

		this->m_db.deleteDetectedByNameOnPath(pkgPath);
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	} else if (!history && riskiest) {
		INFO("no history and new detected");
		this->m_db.insertWorst(pkgId, pkgPath, riskiestPath);

		return this->handleAskUser(context, *riskiest);
	} else {
		DEBUG("no history and no new detected");
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	}
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

	auto d = this->convert(result, filepath);

	this->m_db.insertName(d.targetName);
	this->m_db.insertDetected(d, d.targetName, this->m_dataVersion);

	return this->handleAskUser(context, d, std::forward<FilePtr>(fileptr));
}

RawBuffer CsLogic::scanFile(const CsContext &context, const std::string &filepath)
{
	EXCEPTION_GUARD_START

	if (this->m_db.getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(EngineDisabled, "engine is disabled");

	setCoreUsage(context.coreUsage);

	if (File::isInApp(filepath))
		return this->scanApp(context, filepath);

	DEBUG("Scan request on file: " << filepath);

	auto history = this->m_db.getDetectedByNameOnPath(filepath);

	FilePtr fileptr;

	// if history exist, fileptr can be null because of modified since value
	// from history.
	if (history)
		fileptr = File::createIfModified(filepath, static_cast<time_t>(history->ts));
	else
		fileptr = File::create(filepath);

	// non-null fileptr means the file is modified since the last history
	// OR there's no history at all.
	if (fileptr) {
		if (history)
			this->m_db.deleteDetectedByNameOnPath(filepath);

		if (fileptr->isDir())
			ThrowExc(FileSystemError, "file type shouldn't be directory: " << filepath);

		DEBUG("file[" << filepath << "] is modified since the detected time. "
			  "let's remove history and re-scan");
		return this->scanFileWithoutDelta(context, filepath, std::move(fileptr));
	}

	DEBUG("Usable scan history exist on file: " << filepath);

	if (history->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	return this->handleAskUser(context, *history);

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

	if (this->m_db.getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(EngineDisabled, "engine is disabled");

	auto lastScanTime = this->m_db.getLastScanTime(dir, this->m_dataVersion);

	auto visitor = FsVisitor::create(dir, lastScanTime);

	StrSet fileset;

	while (auto file = visitor->next()) {
		if (!file->isRemovable())
			continue;

		if (file->isInApp()) {
			DEBUG("Scannable app: " << file->getAppPkgPath());
			fileset.insert(file->getAppPkgPath());
		} else {
			DEBUG("Scannable file: " << file->getPath());
			fileset.insert(file->getPath());
		}
	}

	if (lastScanTime != -1) {
		// for case: scan history exist and not modified.
		for (auto &row : this->m_db.getDetectedByNameOnDir(File::getPkgPath(dir))) {
			try {
				auto fileptr = File::create(row->targetName);

				fileset.insert(fileptr->isInApp() ?
						fileptr->getAppPkgPath() : fileptr->getPath());
			} catch (const FileDoNotExist &) {
				this->m_db.deleteDetectedByNameOnPath(row->targetName);
			} catch (const FileSystemError &) {
				this->m_db.deleteDetectedByNameOnPath(row->targetName);
			}
		}
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, fileset).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::setDirTimestamp(const std::string &dir, time_t ts)
{
	EXCEPTION_GUARD_START

	this->m_db.insertLastScanTime(dir, ts, this->m_dataVersion);

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::judgeStatus(const std::string &filepath, csr_cs_action_e action)
{
	EXCEPTION_GUARD_START

	// for file existence / status check. exception thrown.
	FilePtr file;
	try {
		file = File::create(filepath);
	} catch (const Exception &e) {
		ERROR("file system related exception occured on file: " << filepath <<
			  " This case might be file not exist or type invalid,"
			  " file has changed anyway... Don't refresh detected history to know that"
			  " it's changed since the time.");

		throw;
	}

	const auto &targetName = (file->isInApp() ? file->getAppPkgPath() : filepath);

	auto history = this->m_db.getDetectedByNameOnPath(targetName);

	if (!history) {
		ERROR("Target to be judged doesn't exist in db. name: " << targetName);
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER).pop();
	}

	// file create based on fileInAppPath(for app target, it is worst detected)
	if (File::createIfModified(history->fileInAppPath, static_cast<time_t>(history->ts)))
		ThrowExc(FileChanged, "File[" << history->fileInAppPath << "] modified since "
				 "db delta inserted. Don't refresh detected history to know that it's "
				 "changed since the time.");

	switch (action) {
	case CSR_CS_ACTION_REMOVE:
		file->remove();

		this->m_db.deleteDetectedByNameOnPath(targetName);
		break;

	case CSR_CS_ACTION_IGNORE:
		this->m_db.updateIgnoreFlag(targetName, true);
		break;

	case CSR_CS_ACTION_UNIGNORE:
		this->m_db.updateIgnoreFlag(targetName, false);
		break;

	default:
		ThrowExc(InternalError, "Invalid acation enum val: " <<
				 static_cast<csr_cs_action_e>(action));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::getDetected(const std::string &filepath)
{
	EXCEPTION_GUARD_START

	auto row = this->m_db.getDetectedByNameOnPath(File::getPkgPath(filepath));

	if (row && !row->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::getDetectedList(const StrSet &dirSet)
{
	EXCEPTION_GUARD_START

	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : this->m_db.getDetectedByNameOnDir(File::getPkgPath(dir)))
			if (!row->isIgnored)
				rows.emplace_back(std::move(row));
	});

	if (rows.empty())
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();

	EXCEPTION_GUARD_END
}

// TODO: is this command needed?
RawBuffer CsLogic::getIgnored(const std::string &filepath)
{
	EXCEPTION_GUARD_START

	auto row = this->m_db.getDetectedByNameOnPath(File::getPkgPath(filepath));

	if (row && row->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::getIgnoredList(const StrSet &dirSet)
{
	EXCEPTION_GUARD_START

	Db::RowShPtrs rows;
	std::for_each(dirSet.begin(), dirSet.end(),
	[this, &rows](const std::string & dir) {
		for (auto &row : this->m_db.getDetectedByNameOnDir(File::getPkgPath(dir)))
			if (row->isIgnored)
				rows.emplace_back(std::move(row));
	});

	if (rows.empty())
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();

	EXCEPTION_GUARD_END
}

RawBuffer CsLogic::handleAskUser(const CsContext &c, CsDetected &d, FilePtr &&fileptr)
{
	if (c.askUser == CSR_CS_ASK_USER_NO) {
		d.response = CSR_CS_USER_RESPONSE_USER_NOT_ASKED;
		return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();
	}

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
	d.response = askUser.cs(cid, c.popupMessage, d);

	if (d.response == CSR_CS_USER_RESPONSE_REMOVE && !d.targetName.empty()) {
		try {
			FilePtr _fileptr;

			if (fileptr)
				_fileptr = std::forward<FilePtr>(fileptr);
			else
				_fileptr = File::create(d.targetName);

			_fileptr->remove();
		} catch (const FileDoNotExist &) {
			WARN("File already removed.: " << d.targetName);
		} catch (const FileSystemError &) {
			WARN("File type is changed, considered as different file: " << d.targetName);
		}

		this->m_db.deleteDetectedByNameOnPath(File::getPkgPath(d.targetName));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();
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
