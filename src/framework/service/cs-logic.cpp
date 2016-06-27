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
#include <cstdlib>
#include <climits>
#include <cerrno>
#include <unistd.h>

#include <csr-error.h>

#include "common/audit/logger.h"
#include "common/exception.h"
#include "service/type-converter.h"
#include "service/core-usage.h"
#include "service/dir-blacklist.h"
#include "service/fs-utils.h"
#include "ui/askuser.h"

namespace Csr {

namespace {

int readEc(const RawBuffer &buf)
{
	if (buf.size() < sizeof(int))
		ThrowExc(CSR_ERROR_SERVER, "Failed to read error code from buf");

	int ec = -1;

	::memcpy(&ec, buf.data(), sizeof(int));

	return ec;
}

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

// resolve all of "/." and "/.." in absolute path (which starts with '/')
std::string resolvePath(const std::string &_path)
{
	auto path = _path;

	while (path.back() == '/')
		path.pop_back();

	size_t from = 0;
	size_t to = 0;

	while (true) {
		if (path.empty()) {
			path = "/";
			break;
		}

		auto len = path.length();

		if (from >= len)
			break;

		while (len > from + 1 && path[from + 1] == '/') {
			path.erase(from, 1);
			--len;
		}

		to = path.find_first_of('/', from + 1);

		if (to == std::string::npos)
			to = len;

		auto substr = path.substr(from, to - from);

		if (substr == "/.") {
			path.erase(from, 2);
		} else if (substr == "/..") {
			path.erase(from, 3);

			if (from == 0)
				continue;

			auto parent = path.find_last_of('/', from - 1);
			path.erase(parent, from - parent);

			from = parent;
		} else {
			from = to;
		}
	}

	return path;
}

std::string canonicalizePath(const std::string &path, bool checkAccess)
{
	auto resolved = resolvePath(path);

	auto target = File::getPkgPath(resolved);

	if (checkAccess && !isReadable(path)) {
		const int err = errno;
		if (err == ENOENT)
			ThrowExcWarn(CSR_ERROR_FILE_DO_NOT_EXIST, "File do not exist: " << target);
		else if (err == EACCES)
			ThrowExc(CSR_ERROR_PERMISSION_DENIED,
					 "Perm denied to get real path: " << target);
		else
			ThrowExc(CSR_ERROR_FILE_SYSTEM, "Failed to get real path: " << target <<
					 " with errno: " << err);
	}

	return target;
}

FilePtr canonicalizePathWithFile(const std::string &path)
{
	auto resolved = resolvePath(path);

	auto fileptr = File::create(path, nullptr);

	if (!isReadable(fileptr->getName()))
		ThrowExcWarn(CSR_ERROR_FILE_DO_NOT_EXIST, "File is not readable: " << fileptr->getName());

	return fileptr;
}

} // namespace anonymous

CsLogic::CsLogic(const std::shared_ptr<CsLoader> &loader,
				 const std::shared_ptr<Db::Manager> &db) :
	m_loader(loader), m_db(db)
{
	if (!this->m_db)
		ThrowExc(CSR_ERROR_DB, "Failed to init db");

	if (this->m_loader) {
		CsEngineContext csEngineContext(this->m_loader);
		this->m_loader->getEngineDataVersion(csEngineContext.get(), this->m_dataVersion);
		this->m_db->deleteDetectedDeprecated(this->m_loader->getEngineLatestUpdateTime(csEngineContext.get()));
	}
}

RawBuffer CsLogic::scanData(const CsContext &context, const RawBuffer &data)
{
	if (this->m_db->getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	setCoreUsage(context.coreUsage);

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	csre_cs_detected_h result = nullptr;

	auto timestamp = ::time(nullptr);

	this->m_loader->scanData(c, data, &result);

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	auto d = this->convert(result, std::string(), timestamp);

	return this->handleAskUser(context, d);
}

RawBuffer CsLogic::scanAppOnCloud(const CsContext &context,
								  const std::string &pkgPath,
								  const std::string &pkgId)
{
	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	auto timestamp = ::time(nullptr);

	csre_cs_detected_h result = nullptr;
	this->m_loader->scanAppOnCloud(c, pkgPath, &result);

	if (!result)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	auto detected = this->convert(result, pkgPath, timestamp);
	detected.isApp = true;
	detected.pkgId = pkgId;

	this->m_db->insertDetectedAppByCloud(pkgPath, pkgId, detected, this->m_dataVersion);

	return this->handleAskUser(context, detected);
}

CsDetectedPtr CsLogic::scanAppDelta(const std::string &pkgPath, const std::string &pkgId,
									std::string &riskiestPath)
{
	auto starttime = ::time(nullptr);

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	auto t = this->m_loader->getEngineLatestUpdateTime(c);
	auto lastScanTime = this->m_db->getLastScanTime(pkgPath, t);

	CsDetectedPtr riskiest;
	// traverse files in app and take which is more danger than riskiest
	auto visitor = FsVisitor::create([&](const FilePtr &file) {
		DEBUG("Scan file by engine: " << file->getPath());

		auto timestamp = ::time(nullptr);

		csre_cs_detected_h result = nullptr;
		this->m_loader->scanFile(c, file->getPath(), &result);

		if (!result) {
			if (lastScanTime != -1)
				this->m_db->deleteDetectedByFilepathOnPath(file->getPath());

			return;
		}

		INFO("New malware detected on file: " << file->getPath());

		auto candidate = this->convert(result, pkgPath, timestamp);
		candidate.isApp = true;
		candidate.pkgId = pkgId;

		this->m_db->insertDetectedFileInApp(pkgPath, file->getPath(), candidate,
											this->m_dataVersion);

		if (!riskiest) {
			riskiest.reset(new CsDetected(std::move(candidate)));
			riskiestPath = file->getPath();
		} else if (*riskiest < candidate) {
			*riskiest = std::move(candidate);
			riskiestPath = file->getPath();
		}
	}, pkgPath, false, lastScanTime);

	visitor->run();

	this->m_db->insertLastScanTime(pkgPath, this->m_dataVersion, starttime);

	return riskiest;
}

RawBuffer CsLogic::scanApp(const CsContext &context, const FilePtr &pkgPtr)
{
	const auto &pkgPath = pkgPtr->getName();
	const auto &pkgId = pkgPtr->getAppPkgId();

	if (context.isScanOnCloud && this->m_loader->scanAppOnCloudSupported())
		return this->scanAppOnCloud(context, pkgPath, pkgId);

	CsEngineContext engineContext(this->m_loader);
	auto since = this->m_loader->getEngineLatestUpdateTime(engineContext.get());

	// old history
	auto history = this->m_db->getWorstByPkgPath(pkgPath, since);
	// riskiest detected among newly scanned files
	std::string riskiestPath;
	auto riskiest = this->scanAppDelta(pkgPath, pkgId, riskiestPath);
	// history after delta scan. if worst file is changed, it's rescanned in scanAppDelta
	// and deleted from db if it's cured. if history != nullptr && after == nullptr,
	// it means worst detected item is cured anyway.
	auto after = this->m_db->getWorstByPkgPath(pkgPath, since);
	if (history && after && riskiest) {
		if (*history < *riskiest) {
			INFO("worst case is remained but the more worst newly detected. on pkg[" <<
				 pkgPath << "]");
			if (history->isIgnored)
				this->m_db->updateIgnoreFlag(pkgPath, false);

			this->m_db->insertWorst(pkgId, pkgPath, riskiestPath);

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
		since = this->m_loader->getEngineLatestUpdateTime(engineContext.get());
		for (auto &row : this->m_db->getDetectedByFilepathOnDir(pkgPath, since))
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
				this->m_db->updateIgnoreFlag(pkgPath, false);

			this->m_db->insertWorst(pkgId, pkgPath, riskiestPath);

			return this->handleAskUser(context, *riskiest);
		} else {
			INFO("worst case is deleted but same or less level newly detected. on pkg[" <<
				 pkgPath << "]");
			this->m_db->insertWorst(pkgId, pkgPath, riskiestPath);

			if (history->isIgnored)
				return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

			return this->handleAskUser(context, *riskiest);
		}
	} else if (history && !after && !riskiest) {
		since = this->m_loader->getEngineLatestUpdateTime(engineContext.get());
		auto rows = this->m_db->getDetectedByFilepathOnDir(pkgPath, since);

		if (!rows.empty()) {
			INFO("worst case is deleted cascadingly and NO new detected and "
				 "worse case exist on pkg[" << pkgPath << "]. insert it to worst.");
			Db::RowShPtr worse;
			for (auto &row : rows)
				if (!worse || *worse < *row)
					worse = std::move(row);

			if (worse) {
				this->m_db->insertWorst(pkgId, pkgPath, worse->fileInAppPath);

				if (worse->isIgnored)
					return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

				return this->handleAskUser(context, *worse);
			}
		}

		INFO("worst case is deleted cascadingly and NO new detected and "
			 "NO worse case. the pkg[" << pkgPath << "] is clean.");

		this->m_db->deleteDetectedByNameOnPath(pkgPath);
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	} else if (!history && riskiest) {
		INFO("no history and new detected");
		this->m_db->insertWorst(pkgId, pkgPath, riskiestPath);

		return this->handleAskUser(context, *riskiest);
	} else {
		DEBUG("no history and no new detected");
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	}
}

RawBuffer CsLogic::scanFileWithoutDelta(const CsContext &context,
										const std::string &filepath, FilePtr &&fileptr)
{
	if (isInBlackList(filepath))
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	auto timestamp = ::time(nullptr);

	csre_cs_detected_h result = nullptr;
	this->m_loader->scanFile(c, filepath, &result);

	// detected handle is null if it's safe
	if (result == nullptr)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	INFO("New malware detected on file: " << filepath);

	auto d = this->convert(result, filepath, timestamp);

	this->m_db->insertDetectedFile(d.targetName, d, this->m_dataVersion);

	return this->handleAskUser(context, d, std::move(fileptr));
}

RawBuffer CsLogic::scanFileInternal(const CsContext &context, const std::string &filepath)
{
	auto target = canonicalizePathWithFile(filepath, true);

	if (target->isInApp())
		return this->scanApp(context, target);

	const auto &name = target->getName();

	if (target->isDir())
		ThrowExc(CSR_ERROR_FILE_SYSTEM, "file type shouldn't be directory: " << name);

	DEBUG("Scan request on file: " << name);

	CsEngineContext engineContext(this->m_loader);
	auto since = this->m_loader->getEngineLatestUpdateTime(engineContext.get());
	auto history = this->m_db->getDetectedAllByNameOnPath(name, since);

	if (history == nullptr) {
		DEBUG("No history exist on target. Newly scan needed: " << name);
		return this->scanFileWithoutDelta(context, name, std::move(target));
	} else if (target->isModifiedSince(history->ts)) {
		DEBUG("file[" << name << "] is modified since the detected time. "
			  "let's remove history and re-scan");
		this->m_db->deleteDetectedByNameOnPath(name);
		return this->scanFileWithoutDelta(context, name, std::move(target));
	}

	DEBUG("Usable scan history exist on file: " << name);

	if (history->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();

	return this->handleAskUser(context, *history);
}

RawBuffer CsLogic::scanFile(const CsContext &context, const std::string &filepath)
{
	if (this->m_db->getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	setCoreUsage(context.coreUsage);

	return this->scanFileInternal(context, filepath);
}

RawBuffer CsLogic::scanFilesAsync(const ConnShPtr &conn, const CsContext &context, StrSet &paths)
{
	if (this->m_db->getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	StrSet canonicalized;

	for (const auto &path : paths) {
		auto target = canonicalizePath(path, true);

		if (canonicalized.find(target) == canonicalized.end()) {
			INFO("Insert to canonicalized list: " << target);
			canonicalized.emplace(std::move(target));
		}
	}

	conn->send(BinaryQueue::Serialize(CSR_ERROR_NONE).pop());

	for (const auto &path : canonicalized) {
		auto out = this->scanFileInternal(context, path);
		int retcode = readEc(out);
		bool isMalwareDetected = out.data() > sizeof(int);

		switch (retcode) {
		case CSR_ERROR_NONE:
		case CSR_ERROR_FILE_DO_NOT_EXIST:
		case CSR_ERROR_FILE_CHANGED:
		case CSR_ERROR_FILE_SYSTEM:
			if (isMalwareDetected) {
				conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_DETECTED).pop());
				conn->send(out);
			} else if (context.isScannedCbRegistered) {
				conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_NONE).pop());
				conn->send(BinaryQueue::Serialize(path).pop());
			}

			break;

		default:
			return out;
		}
	}

	return BinaryQueue::Serialize(ASYNC_EVENT_COMPLETE).pop();
}

RawBuffer CsLogic::scanDirsAsync(const ConnShPtr &conn, const CsContext &context, StrSet &paths)
{
	if (this->m_db->getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	StrSet canonicalized;

	for (const auto &path : paths) {
		auto target = canonicalizePath(File::getPkgPath(path), true);

		if (canonicalized.find(target) == canonicalized.end()) {
			INFO("Insert to canonicalized list: " << target);
			canonicalized.emplace(std::move(target));
		}
	}

	auto dirs = eraseSubdirectories(canonicalized);

	conn->send(BinaryQueue::Serialize(CSR_ERROR_NONE).pop());

	CsEngineContext engineContext(this->m_loader);
	auto t = this->m_loader->getEngineLatestUpdateTime(engineContext.get());

	for (const auto &dir : dirs) {
		auto starTime = ::time(nullptr);
		auto lastScanTime = this->m_db->getLastScanTime(dir, t);
		auto visitor = FsVisitor::createTargets(dir, lastScanTime);

		while (auto file = visitor->next()) {
			auto out = this->scanFileInternal(context, file->getPath());
			int retcode = readEc(out);
			bool isMalwareDetected = out.data() > sizeof(int);

			switch (retcode) {
			case CSR_ERROR_NONE:
			case CSR_ERROR_FILE_DO_NOT_EXIST:
			case CSR_ERROR_FILE_CHANGED:
			case CSR_ERROR_FILE_SYSTEM:
				if (isMalwareDetected) {
					conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_DETECTED).pop());
					conn->send(out);
				} else if (context.isScannedCbRegistered) {
					conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_NONE).pop());
					conn->send(BinaryQueue::Serialize(file->getPath()).pop());
				}

				break;

			default:
				ERROR("Error on async scanning: " << retcode);
				return out;
			}
		}

		this->m_db->insertLastScanTime(dir, startTime);

		if (lastScanTime != -1) {
			for (auto &row : this->m_db->getDetectedAllByNameOnDir(dir)) {
				try {
					auto fileptr = File::create(row->targetName);

					conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_DETECTED).pop());
					conn->send(BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop());
				} catch (const Exception &e) {
					if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST ||
						e.error() == CSR_ERROR_FILE_SYSTEM)
						this->m_db->deleteDetectedByNameOnPath(row->targetName);
					else
						throw;
				}
			}
		}
	}

	return BinaryQueue::Serialize(ASYNC_EVENT_COMPLETE).pop();
}

RawBuffer CsLogic::judgeStatus(const std::string &filepath, csr_cs_action_e action)
{
	// for file existence / status check. exception thrown.
	FilePtr file;
	try {
		file = File::create(filepath, nullptr);
	} catch (const Exception &e) {
		ERROR("file system related exception occured on file: " << filepath <<
			  " This case might be file not exist or type invalid,"
			  " file has changed anyway... Don't refresh detected history to know that"
			  " it's changed since the time.");

		throw;
	}

	const auto &targetName = file->getName();

	CsEngineContext csEngineContext(this->m_loader);
	auto since = this->m_loader->getEngineLatestUpdateTime(csEngineContext.get());

	bool isCloudHistory = false;
	auto history = this->m_db->getDetectedAllByNameOnPath(targetName, since, &isCloudHistory);

	if (!history) {
		ERROR("Target to be judged doesn't exist in db. name: " << targetName);
		return BinaryQueue::Serialize(CSR_ERROR_INVALID_PARAMETER).pop();
	}

	// file create based on fileInAppPath(for app target, it is worst detected)
	if (!isCloudHistory && file->isModifiedSince(history->ts))
		ThrowExc(CSR_ERROR_FILE_CHANGED,
				 "File[" << history->fileInAppPath << "] modified since db delta inserted."
				 " Don't refresh detected history to know that it's changed since the"
				 " time.");

	switch (action) {
	case CSR_CS_ACTION_REMOVE:
		file->remove();

		this->m_db->deleteDetectedByNameOnPath(targetName);
		break;

	case CSR_CS_ACTION_IGNORE:
		this->m_db->updateIgnoreFlag(targetName, true);
		break;

	case CSR_CS_ACTION_UNIGNORE:
		this->m_db->updateIgnoreFlag(targetName, false);
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid acation enum val: " <<
				 static_cast<csr_cs_action_e>(action));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer CsLogic::getDetected(const std::string &filepath)
{
	std::string target;
	try {
		target = canonicalizePath(filepath, true);
	} catch (const Exception &e) {
		WARN("Ignore exceptions on file canonicalize/existence check for getting"
			 " history e.g., detected/ignored. filepath: " << filepath);
		this->m_db->deleteDetectedByNameOnPath(canonicalizePath(filepath, false));
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	}

	CsEngineContext csEngineContext(this->m_loader);
	auto row = this->m_db->getDetectedAllByNameOnPath(target,
			this->m_loader->getEngineLatestUpdateTime(csEngineContext.get()));

	if (row && !row->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer CsLogic::getDetectedList(const StrSet &_dirSet)
{
	StrSet dirSet;
	for (const auto &dir : _dirSet)
		dirSet.emplace(canonicalizePath(dir, false));

	CsEngineContext csEngineContext(this->m_loader);
	auto since = this->m_loader->getEngineLatestUpdateTime(csEngineContext.get());
	Db::RowShPtrs rows;
	for (const auto &dir : dirSet) {
		for (auto &row : this->m_db->getDetectedAllByNameOnDir(dir, since)) {
			if (!row->fileInAppPath.empty() && !isReadable(row->targetName)) {
				WARN("Exclude not-accessable malware detected file from the list: " <<
					 row->targetName);
				this->m_db->deleteDetectedByNameOnPath(row->targetName);
			} else if (!row->isIgnored) {
				rows.emplace_back(std::move(row));
			}
		}
	}

	if (rows.empty())
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();
}

RawBuffer CsLogic::getIgnored(const std::string &filepath)
{
	std::string target;
	try {
		target = canonicalizePath(filepath, true);
	} catch (const Exception &e) {
		WARN("Ignore exceptions on file canonicalize/existence check for getting"
			 " history e.g., detected/ignored. filepath: " << filepath);
		this->m_db->deleteDetectedByNameOnPath(canonicalizePath(filepath, false));
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	}

	CsEngineContext csEngineContext(this->m_loader);
	auto row = this->m_db->getDetectedAllByNameOnPath(target,
			this->m_loader->getEngineLatestUpdateTime(csEngineContext.get()));

	if (row && row->isIgnored)
		return BinaryQueue::Serialize(CSR_ERROR_NONE, row).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
}

RawBuffer CsLogic::getIgnoredList(const StrSet &_dirSet)
{
	StrSet dirSet;
	for (const auto &dir : _dirSet)
		dirSet.emplace(canonicalizePath(dir, false));

	CsEngineContext csEngineContext(this->m_loader);
	auto since = this->m_loader->getEngineLatestUpdateTime(csEngineContext.get());
	Db::RowShPtrs rows;
	for (const auto &dir : dirSet) {
		for (auto &row : this->m_db->getDetectedAllByNameOnDir(dir, since)) {
			if (!row->fileInAppPath.empty() && !isReadable(row->targetName)) {
				WARN("Exclude not-accessable malware detected file from the list: " <<
					 row->targetName);
				this->m_db->deleteDetectedByNameOnPath(row->targetName);
			} else if (row->isIgnored) {
				rows.emplace_back(std::move(row));
			}
		}
	}

	if (rows.empty())
		return BinaryQueue::Serialize(CSR_ERROR_NONE).pop();
	else
		return BinaryQueue::Serialize(CSR_ERROR_NONE, rows).pop();
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
		ThrowExc(CSR_ERROR_SERVER, "Invalid severity: " << static_cast<int>(d.severity));
	}

	Ui::AskUser askUser;
	auto r = askUser.cs(cid, c.popupMessage, d);
	if (r == -1) {
		ERROR("Failed to get user response by popup service for target: " << d.targetName);
		return BinaryQueue::Serialize(CSR_ERROR_USER_RESPONSE_FAILED, d).pop();
	}

	d.response = r;

	if (d.response == CSR_CS_USER_RESPONSE_REMOVE && !d.targetName.empty()) {
		try {
			FilePtr _fileptr;
			if (fileptr)
				_fileptr = std::move(fileptr);
			else
				_fileptr = File::create(d.targetName, nullptr);

			_fileptr->remove();
		} catch (const Exception &e) {
			if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST)
				WARN("File already removed.: " << d.targetName);
			else if (e.error() == CSR_ERROR_FILE_SYSTEM)
				WARN("File type is changed, considered as different file: " <<
					 d.targetName);
			else if (e.error() == CSR_ERROR_REMOVE_FAILED)
				return BinaryQueue::Serialize(CSR_ERROR_REMOVE_FAILED, d).pop();
			else
				throw;
		}

		this->m_db->deleteDetectedByNameOnPath(d.targetName);
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, d).pop();
}

CsDetected CsLogic::convert(csre_cs_detected_h &result, const std::string &targetName,
							time_t timestamp)
{
	DEBUG("convert engine result handle to CsDetected start");

	CsDetected d;

	d.targetName = targetName;

	csre_cs_severity_level_e eseverity = CSRE_CS_SEVERITY_LOW;

	this->m_loader->getSeverity(result, &eseverity);
	this->m_loader->getMalwareName(result, d.malwareName);
	this->m_loader->getDetailedUrl(result, d.detailedUrl);

	d.ts = timestamp;
	d.severity = Csr::convert(eseverity);

	return d;
}

}
