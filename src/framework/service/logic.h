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

#include "common/types.h"
#include "common/cs-context.h"
#include "common/wp-context.h"
#include "common/wp-result.h"
#include "common/command-id.h"
#include "common/raw-buffer.h"
#include "common/binary-queue.h"
#include "service/cs-loader.h"
#include "service/wp-loader.h"

namespace Csr {

class Logic {
public:
	Logic();
	virtual ~Logic();

	RawBuffer dispatch(const RawBuffer &);

private:
	std::pair<CommandId, BinaryQueue> getRequestInfo(const RawBuffer &);

	RawBuffer scanFile(const CsContext &context, const std::string &filepath);
	RawBuffer dirGetResults(const CsContext &context, const std::string &dir);
	RawBuffer dirGetFiles(const CsContext &context, const std::string &dir);

	RawBuffer checkUrl(const WpContext &context, const std::string &url);

	WpResult convert(csre_wp_check_result_h &);
	void getUserResponse(const WpContext &, const std::string &url, csr_wp_risk_level_e, WpResult &);

	std::shared_ptr<CsLoader> m_cs;
	std::shared_ptr<WpLoader> m_wp;
};

}
