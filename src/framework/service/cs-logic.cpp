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
#include "common/async-protocol.h"
#include "service/type-converter.h"
#include "service/core-usage.h"
#include "service/dir-blacklist.h"
#include "service/fs-utils.h"
#include "ui/askuser.h"

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

	auto fileptr = File::create(resolved, nullptr);

	if (!isReadable(fileptr->getName()))
		ThrowExcWarn(CSR_ERROR_FILE_DO_NOT_EXIST, "File is not readable: " << fileptr->getName());

	return fileptr;
}

void eraseSubdirectories(StrSet &dirset)
{
	if (dirset.size() < 2)
		return;

	for (auto it = dirset.begin(); it != dirset.end(); ++it) {
		auto itsub = it;
		++itsub;
		while (true) {
			if (itsub == dirset.end())
				break;

			auto itl = it->length();
			auto itsubl = itsub->length();

			if (itl + 1 >= itsubl || // to short to be sub-directory
				itsub->compare(0, itl, *it) != 0 || // prefix isn't matched
				(*it != "/" && itsub->at(itl) != '/')) { // has '/' at the end of prefix
				++itsub;
				continue;
			}

			itsub = dirset.erase(itsub);
		}
	}
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

	auto malware = this->convert(result, std::string(), timestamp);

	return BinaryQueue::Serialize(this->handleAskUser(context, *malware), malware).pop();
}

int CsLogic::scanAppOnCloud(const CsContext &context, const FilePtr &pkgPtr,
							CsDetectedPtr &malware)
{
	const auto &pkgPath = pkgPtr->getName();
	const auto &pkgId = pkgPtr->getAppPkgId();

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	auto timestamp = ::time(nullptr);

	csre_cs_detected_h result = nullptr;
	this->m_loader->scanAppOnCloud(c, pkgPath, &result);

	if (!result)
		return CSR_ERROR_NONE;

	malware = this->convert(result, pkgPath, timestamp);
	malware->isApp = true;
	malware->pkgId = pkgId;

	this->m_db->insertDetectedAppByCloud(pkgPath, pkgId, *malware, this->m_dataVersion);

	return this->handleAskUser(context, *malware, pkgPtr);
}

CsDetectedPtr CsLogic::scanAppDelta(const FilePtr &pkgPtr, std::string &riskiestPath, const std::function<void()> &isCancelled)
{
	const auto &pkgPath = pkgPtr->getName();
	const auto &pkgId = pkgPtr->getAppPkgId();

	auto starttime = ::time(nullptr);

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	auto t = this->m_loader->getEngineLatestUpdateTime(c);
	auto lastScanTime = this->m_db->getLastScanTime(pkgPath, t);

	CsDetectedPtr riskiest;
	// traverse files in app and take which is more danger than riskiest
	auto visitor = FsVisitor::create([&](const FilePtr &file) {
		if (isCancelled != nullptr)
			isCancelled();

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
		candidate->isApp = true;
		candidate->pkgId = pkgId;

		this->m_db->insertDetectedFileInApp(file->getPath(), *candidate, this->m_dataVersion);

		if (riskiest == nullptr || *riskiest < *candidate) {
			riskiest = std::move(candidate);
			riskiestPath = file->getPath();
		}
	}, pkgPath, false, lastScanTime);

	visitor->run();

	this->m_db->insertLastScanTime(pkgPath, this->m_dataVersion, starttime);

	return riskiest;
}

int CsLogic::scanApp(const CsContext &context, const FilePtr &pkgPtr,
					 CsDetectedPtr &malware, const std::function<void()> &isCancelled)
{
	const auto &pkgPath = pkgPtr->getName();
	const auto &pkgId = pkgPtr->getAppPkgId();

	if (context.isScanOnCloud && this->m_loader->scanAppOnCloudSupported())
		return this->scanAppOnCloud(context, pkgPtr, malware);

	CsEngineContext engineContext(this->m_loader);
	auto since = this->m_loader->getEngineLatestUpdateTime(engineContext.get());

	// old history
	auto history = this->m_db->getWorstByPkgPath(pkgPath, since);
	// riskiest detected among newly scanned files
	std::string riskiestPath;
	auto riskiest = this->scanAppDelta(pkgPtr, riskiestPath, isCancelled);
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

			malware.reset(new CsDetected());
			*malware = std::move(*riskiest);
			return this->handleAskUser(context, *malware);
		} else {
			INFO("worst case is remained and can be re-used on pkg[" << pkgPath << "]");
			if (history->isIgnored)
				return CSR_ERROR_NONE;

			malware.reset(new CsDetected());
			*malware = std::move(*history);
			return this->handleAskUser(context, *malware);
		}
	} else if (history && after && !riskiest) {
		INFO("worst case is remained and NO new detected. history can be re-used. "
			 "on pkg[" << pkgPath << "]");
		if (history->isIgnored)
			return CSR_ERROR_NONE;

		malware.reset(new CsDetected());
		*malware = std::move(*history);
		return this->handleAskUser(context, *malware);
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
			return CSR_ERROR_NONE;
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

			malware.reset(new CsDetected());
			*malware = std::move(*riskiest);
			return this->handleAskUser(context, *malware);
		} else {
			INFO("worst case is deleted but same or less level newly detected. on pkg[" <<
				 pkgPath << "]");
			this->m_db->insertWorst(pkgId, pkgPath, riskiestPath);

			if (history->isIgnored)
				return CSR_ERROR_NONE;

			malware.reset(new CsDetected());
			*malware = std::move(*riskiest);
			return this->handleAskUser(context, *malware);
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
					return CSR_ERROR_NONE;

				malware.reset(new CsDetected());
				*malware = std::move(*worse);
				return this->handleAskUser(context, *malware);
			}
		}

		INFO("worst case is deleted cascadingly and NO new detected and "
			 "NO worse case. the pkg[" << pkgPath << "] is clean.");

		this->m_db->deleteDetectedByNameOnPath(pkgPath);
		return CSR_ERROR_NONE;
	} else if (!history && riskiest) {
		INFO("no history and new detected");
		this->m_db->insertWorst(pkgId, pkgPath, riskiestPath);

		malware.reset(new CsDetected());
		*malware = std::move(*riskiest);
		return this->handleAskUser(context, *malware);
	} else {
		DEBUG("no history and no new detected");
		return CSR_ERROR_NONE;
	}
}

int CsLogic::scanFileInternal(const CsContext &context, const FilePtr &target,
							  CsDetectedPtr &malware, const std::function<void()> &isCancelled)
{
	if (target->isInApp())
		return this->scanApp(context, target, malware, isCancelled);

	const auto &name = target->getName();

	if (target->isDir())
		ThrowExc(CSR_ERROR_FILE_SYSTEM, "file type shouldn't be directory: " << name);

	DEBUG("Scan request on file: " << name);

	if (isInBlackList(name))
		return CSR_ERROR_NONE;

	CsEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	auto timestamp = ::time(nullptr);

	csre_cs_detected_h result = nullptr;
	this->m_loader->scanFile(c, name, &result);

	// detected handle is null if it's safe
	if (result == nullptr)
		return CSR_ERROR_NONE;

	INFO("Malware detected on file: " << name);

	malware = this->convert(result, name, timestamp);

	// check malware detected history for inherit ignored flag
	auto since = this->m_loader->getEngineLatestUpdateTime(c);
	auto history = this->m_db->getDetectedAllByNameOnPath(name, since);

	this->m_db->insertDetectedFile(*malware, this->m_dataVersion);

	if (history != nullptr && history->isIgnored && !(*malware > *history)) {
		INFO("Ignore malware on file: " << name);
		malware.reset();
		return CSR_ERROR_NONE;
	} else {
		return this->handleAskUser(context, *malware, target);
	}
}

RawBuffer CsLogic::scanFile(const CsContext &context, const std::string &filepath)
{
	if (this->m_db->getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	setCoreUsage(context.coreUsage);

	CsDetectedPtr malware;
	auto ret = this->scanFileInternal(context, canonicalizePathWithFile(filepath), malware);
	if (malware != nullptr)
		return BinaryQueue::Serialize(ret, malware).pop();
	else
		return BinaryQueue::Serialize(ret).pop();
}

RawBuffer CsLogic::scanFilesAsync(const ConnShPtr &conn, const CsContext &context,
								  StrSet &paths, const std::function<void()> &isCancelled)
{
	if (this->m_db->getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	conn->send(BinaryQueue::Serialize(ASYNC_EVENT_START).pop());

	StrSet canonicalized;

	for (const auto &path : paths) {
		isCancelled();

		FilePtr target;
		try {
			target = canonicalizePathWithFile(path);

			if (canonicalized.find(target->getName()) != canonicalized.end())
				continue;

			INFO("Insert to canonicalized list: " << target->getName());
			canonicalized.insert(target->getName());
		} catch (const Exception &e) {
			if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST ||
				e.error() == CSR_ERROR_FILE_SYSTEM ||
				e.error() == CSR_ERROR_PERMISSION_DENIED) {
				WARN("File-system & permission related exception occured while getting "
					 "canonicalize path of path: " << path << " " << e.what() <<
					 ". Ignore this exception.");
				continue;
			} else {
				ERROR("Non-file-system related exception occured while getting "
					 "canonicalize path of path: " << path << " " << e.what());
				throw;
			}
		}

		CsDetectedPtr malware;
		auto retcode = this->scanFileInternal(context, target, malware, isCancelled);

		switch (retcode) {
		case CSR_ERROR_NONE:
		case CSR_ERROR_FILE_DO_NOT_EXIST:
		case CSR_ERROR_FILE_CHANGED:
		case CSR_ERROR_FILE_SYSTEM:
			if (malware != nullptr) {
				conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_DETECTED).pop());
				conn->send(BinaryQueue::Serialize(malware).pop());
			} else if (context.isScannedCbRegistered) {
				conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_NONE).pop());
				conn->send(BinaryQueue::Serialize(target->getName()).pop());
			}

			break;

		default:
			ThrowExc(retcode, "Error on async scanning: " << retcode);
		}
	}

	return BinaryQueue::Serialize(ASYNC_EVENT_COMPLETE).pop();
}

RawBuffer CsLogic::scanDirsAsync(const ConnShPtr &conn, const CsContext &context,
								 StrSet &paths, const std::function<void()> &isCancelled)
{
	if (this->m_db->getEngineState(CSR_ENGINE_CS) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	StrSet dirs;

	for (const auto &path : paths) {
		try {
			auto target = canonicalizePath(path, true);

			if (dirs.find(target) == dirs.end()) {
				INFO("Insert to canonicalized list: " << target);
				dirs.emplace(std::move(target));
			}
		} catch (const Exception &e) {
			if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST ||
				e.error() == CSR_ERROR_FILE_SYSTEM ||
				e.error() == CSR_ERROR_PERMISSION_DENIED) {
				WARN("File-system & permission related exception occured while getting "
					 "canonicalize path of path: " << path << " " << e.what() <<
					 ". Ignore this exception.");
				continue;
			} else {
				ERROR("Non-file-system related exception occured while getting "
					 "canonicalize path of path: " << path << " " << e.what());
				throw;
			}
		}
	}

	eraseSubdirectories(dirs);

	DEBUG("send error none to client before starting scanning");

	conn->send(BinaryQueue::Serialize(ASYNC_EVENT_START).pop());

	CsEngineContext engineContext(this->m_loader);
	auto t = this->m_loader->getEngineLatestUpdateTime(engineContext.get());

	DEBUG("Start async scanning!!!!!");

	StrSet malwareList;
	for (const auto &dir : dirs) {
		isCancelled();

		DEBUG("Start async scanning for dir: " << dir);

		for (auto &row : this->m_db->getDetectedAllByNameOnDir(dir, t)) {
			isCancelled();

			try {
				auto fileptr = File::create(row->targetName, nullptr);

				CsDetectedPtr malware;
				auto retcode = this->scanFileInternal(context, fileptr, malware);

				switch (retcode) {
				case CSR_ERROR_NONE:
				case CSR_ERROR_FILE_DO_NOT_EXIST:
				case CSR_ERROR_FILE_CHANGED:
				case CSR_ERROR_FILE_SYSTEM:
					if (malware != nullptr) {
						conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_DETECTED).pop());
						conn->send(BinaryQueue::Serialize(malware).pop());
						malwareList.insert(row->targetName);
					} else if (context.isScannedCbRegistered) {
						conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_NONE).pop());
						conn->send(BinaryQueue::Serialize(row->targetName).pop());
						this->m_db->deleteDetectedByNameOnPath(row->targetName);
					}

					break;

				default:
					ERROR("Error on rescanning detected malwares in db: " << retcode <<
						  " file: " << fileptr->getName());
					this->m_db->deleteDetectedByNameOnPath(row->targetName);
					return BinaryQueue::Serialize(retcode).pop();
				}
			} catch (const Exception &e) {
				if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST ||
					e.error() == CSR_ERROR_FILE_SYSTEM)
					this->m_db->deleteDetectedByNameOnPath(row->targetName);
				else
					throw;
			}
		}

		auto startTime = ::time(nullptr);
		auto lastScanTime = this->m_db->getLastScanTime(dir, t);
		auto visitor = FsVisitor::create([&](const FilePtr &file) {
			isCancelled();

			CsDetectedPtr malware;
			auto retcode = this->scanFileInternal(context, file, malware);

			DEBUG("scanFileInternal done. file: " << file->getName() <<
				  " retcode: " << retcode);

			switch (retcode) {
			case CSR_ERROR_NONE:
			case CSR_ERROR_FILE_DO_NOT_EXIST:
			case CSR_ERROR_FILE_CHANGED:
			case CSR_ERROR_FILE_SYSTEM:
				if (malware != nullptr) {
					auto it = malwareList.find(file->getName());
					if (it != malwareList.end()) {
						malwareList.erase(it);
						break;
					}

					DEBUG("Malware detected!!!");
					conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_DETECTED).pop());
					conn->send(BinaryQueue::Serialize(malware).pop());
				} else if (context.isScannedCbRegistered) {
					DEBUG("File scanned!!!");
					conn->send(BinaryQueue::Serialize(ASYNC_EVENT_MALWARE_NONE).pop());
					conn->send(BinaryQueue::Serialize(file->getName()).pop());
				}

				break;

			default:
				ThrowExc(retcode, "Error on async scanning: " << retcode);
			}
		}, dir, true, lastScanTime);

		visitor->run();

		this->m_db->insertLastScanTime(dir, this->m_dataVersion, startTime);

		if (lastScanTime == -1)
			continue;
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

int CsLogic::handleAskUser(const CsContext &c, CsDetected &d, const FilePtr &fileptr)
{
	if (c.askUser == CSR_CS_ASK_USER_NO) {
		d.response = CSR_CS_USER_RESPONSE_USER_NOT_ASKED;
		return CSR_ERROR_NONE;
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
		return CSR_ERROR_USER_RESPONSE_FAILED;
	}

	d.response = r;

	if (d.response != CSR_CS_USER_RESPONSE_REMOVE || d.targetName.empty())
		return CSR_ERROR_NONE;

	try {
		if (fileptr != nullptr)
			fileptr->remove();
		else
			File::create(d.targetName, nullptr)->remove();
	} catch (const Exception &e) {
		if (e.error() == CSR_ERROR_FILE_DO_NOT_EXIST)
			WARN("File already removed.: " << d.targetName);
		else if (e.error() == CSR_ERROR_FILE_SYSTEM)
			WARN("File type is changed, considered as different file: " << d.targetName);
		else if (e.error() == CSR_ERROR_REMOVE_FAILED)
			return CSR_ERROR_REMOVE_FAILED;
		else
			throw;
	}

	this->m_db->deleteDetectedByNameOnPath(d.targetName);
	return CSR_ERROR_NONE;
}

CsDetectedPtr CsLogic::convert(csre_cs_detected_h &result, const std::string &targetName,
							time_t timestamp)
{
	DEBUG("convert engine result handle to CsDetected start");

	CsDetectedPtr malware(new CsDetected());

	malware->targetName = targetName;

	csre_cs_severity_level_e eseverity = CSRE_CS_SEVERITY_LOW;

	this->m_loader->getSeverity(result, &eseverity);
	this->m_loader->getMalwareName(result, malware->malwareName);
	this->m_loader->getDetailedUrl(result, malware->detailedUrl);

	malware->ts = timestamp;
	malware->severity = Csr::convert(eseverity);

	return malware;
}

}
