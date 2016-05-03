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
 * @file        logic.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>
#include <utility>
#include <memory>

#include "common/types.h"
#include "common/cs-context.h"
#include "common/wp-context.h"
#include "common/cs-detected.h"
#include "common/wp-result.h"
#include "db/manager.h"
#include "service/cs-loader.h"
#include "service/wp-loader.h"

#include "csr/content-screening-types.h"

namespace Csr {

class Logic {
public:
	Logic();
	virtual ~Logic();

	RawBuffer scanData(const CsContext &context, const RawBuffer &data);
	RawBuffer scanFile(const CsContext &context, const std::string &filepath);
	RawBuffer getScannableFiles(const std::string &dir);
	RawBuffer judgeStatus(const std::string &filepath, csr_cs_action_e action);
	RawBuffer getDetected(const std::string &filepath);
	RawBuffer getDetectedList(const StrSet &dirSet);
	RawBuffer getIgnored(const std::string &filepath);
	RawBuffer getIgnoredList(const StrSet &dirSet);

	RawBuffer checkUrl(const WpContext &context, const std::string &url);

private:
	RawBuffer scanFileHelper(const CsContext &context, const std::string &filepath);
	CsDetected convert(csre_cs_detected_h &);
	WpResult convert(csre_wp_check_result_h &);

	static csr_cs_user_response_e getUserResponse(const CsContext &,
			const CsDetected &);
	static csr_wp_user_response_e getUserResponse(const WpContext &,
			const std::string &url, const WpResult &);

	std::shared_ptr<CsLoader> m_cs;
	std::shared_ptr<WpLoader> m_wp;
	std::unique_ptr<Db::Manager> m_db;

	std::string m_csDataVersion;
	std::string m_wpDataVersion;
};

}
