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
 * @file        cs-logic.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <memory>
#include <string>
#include <functional>
#include <ctime>

#include "common/types.h"
#include "common/cs-context.h"
#include "common/cs-detected.h"
#include "db/manager.h"
#include "db/cache.h"
#include "service/cs-loader.h"
#include "service/file-system.h"
#include "service/logic.h"

#include <csr-content-screening-types.h>

namespace Csr {

class CsLogic : public Logic {
public:
	CsLogic(const std::shared_ptr<CsLoader> &loader,
			const std::shared_ptr<Db::Manager> &db);
	virtual ~CsLogic() = default;

	RawBuffer scanData(const CsContext &context, const RawBuffer &data);
	RawBuffer scanFile(const CsContext &context, const std::string &filepath);
	RawBuffer scanFilesAsync(const ConnShPtr &conn, const CsContext &context,
							 StrSet &paths, const std::function<void()> &isCancelled);
	RawBuffer scanDirsAsync(const ConnShPtr &conn, const CsContext &context,
							StrSet &paths, const std::function<void()> &isCancelled);
	RawBuffer judgeStatus(const std::string &filepath, csr_cs_action_e action);
	RawBuffer getDetected(const std::string &filepath);
	RawBuffer getDetectedList(const StrSet &dirSet);
	RawBuffer getIgnored(const std::string &filepath);
	RawBuffer getIgnoredList(const StrSet &dirSet);

private:
	int scanFileInternal(const CsContext &context, const FilePtr &target, CsDetectedPtr &malware);
	int scanApp(const CsContext &context, const FilePtr &pkgPtr, CsDetectedPtr &malware);
	int scanAppOnCloud(const CsContext &context, const FilePtr &pkgPtr, CsDetectedPtr &malware);
	Db::Cache scanAppDelta(const FilePtr &pkgPtr);

	CsDetectedPtr convert(csre_cs_detected_h &result, const std::string &targetName,
					   time_t timestamp);
	int handleAskUser(const CsContext &c, CsDetected &d, const FilePtr &fileptr = nullptr);

	Db::RowShPtr getWorseByPkgPath(const std::string &pkgPath, time_t since);

	enum class ScanStage : int {
		NEW_RISKIEST           = 0x1001,
		NEW_RISKIEST_KEEP_FLAG = 0x1002,
		HISTORY_RISKIEST       = 0x1003,
		WORSE_RISKIEST         = 0x1004,
		NO_DETECTED            = 0x1005
	};

	CsLogic::ScanStage judgeScanStage(
			const Db::RowShPtr &history,
			const Db::RowShPtr &after,
			const CsDetectedPtr &riskiest,
			Db::RowShPtr &jWorse,
			Db::RowShPtr &jHistory,
			time_t since);

	std::shared_ptr<CsLoader> m_loader;
	std::shared_ptr<Db::Manager> m_db;

	std::string m_dataVersion;

};

}
