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
 * @file        em-logic.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Engine management logic
 */
#pragma once

#include <memory>

#include "common/types.h"
#include "common/em-context.h"
#include "common/em-result.h"
#include "service/cs-loader.h"
#include "service/wp-loader.h"
#include "service/logic.h"
#include "db/manager.h"
#include "csr/engine-manager.h"

namespace Csr {

class EmLogic : public Logic {
public:
	EmLogic();
	virtual ~EmLogic();

	RawBuffer getEngineName(const EmContext &context);
	RawBuffer getEngineVendor(const EmContext &context);
	RawBuffer getEngineVersion(const EmContext &context);
	RawBuffer getEngineDataVersion(const EmContext &context);
	RawBuffer getEngineUpdatedTime(const EmContext &context);
	RawBuffer getEngineActivated(const EmContext &context);
	RawBuffer getEngineState(const EmContext &context);
	RawBuffer setEngineState(const EmContext &context, csr_state_e state);

private:
	std::unique_ptr<CsLoader> m_cs;
	std::unique_ptr<WpLoader> m_wp;
	std::unique_ptr<Db::Manager> m_db;
};

}
