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
 * @file        wp-logic.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Web protection logic
 */
#pragma once

#include <string>
#include <memory>

#include "common/types.h"
#include "common/wp-context.h"
#include "common/wp-result.h"
#include "service/wp-loader.h"
#include "service/logic.h"

namespace Csr {

class WpLogic : public Logic {
public:
	WpLogic();
	virtual ~WpLogic();

	RawBuffer checkUrl(const WpContext &context, const std::string &url);

private:
	WpResult convert(csre_wp_check_result_h &result);

	static csr_wp_user_response_e getUserResponse(const WpContext &context,
												  const std::string &url,
												  const WpResult &result);

	std::unique_ptr<WpLoader> m_loader;

	std::string m_dataVersion;
};

}
