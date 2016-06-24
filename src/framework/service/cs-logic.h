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
	RawBuffer getScannableFiles(const std::string &dir, const std::function<void()> &isCancelled);
	RawBuffer canonicalizePaths(const StrSet &paths);
	RawBuffer setDirTimestamp(const std::string &dir, time_t ts);
	RawBuffer judgeStatus(const std::string &filepath, csr_cs_action_e action);
	RawBuffer getDetected(const std::string &filepath);
	RawBuffer getDetectedList(const StrSet &dirSet);
	RawBuffer getIgnored(const std::string &filepath);
	RawBuffer getIgnoredList(const StrSet &dirSet);

private:
	RawBuffer scanApp(const CsContext &context, const std::string &pkgPath);
	RawBuffer scanAppOnCloud(const CsContext &context, const std::string &pkgPath,
							 const std::string &pkgId);
	CsDetectedPtr scanAppDelta(const std::string &pkgPath, const std::string &pkgId,
							   std::string &riskiestPath);

	RawBuffer scanFileWithoutDelta(const CsContext &context, const std::string &filepath,
								   FilePtr &&fileptr);

	CsDetected convert(csre_cs_detected_h &result, const std::string &targetName,
					   time_t timestamp);
	RawBuffer handleAskUser(const CsContext &c, CsDetected &d,
							FilePtr &&fileptr = nullptr);

	std::shared_ptr<CsLoader> m_loader;
	std::shared_ptr<Db::Manager> m_db;

	std::string m_dataVersion;
};

}
