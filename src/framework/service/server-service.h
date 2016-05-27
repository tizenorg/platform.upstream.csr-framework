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
 * @file        server-service.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <memory>

#include "common/service.h"
#include "common/types.h"
#include "service/thread-pool.h"
#include "service/cs-logic.h"
#include "service/wp-logic.h"
#include "service/em-logic.h"
#include "service/cs-loader.h"
#include "service/wp-loader.h"
#include "db/manager.h"

namespace Csr {

class ServerService : public Service {
public:
	ServerService();
	virtual ~ServerService() = default;

private:
	virtual void onMessageProcess(const ConnShPtr &) override;

	RawBuffer processCs(const ConnShPtr &, RawBuffer &);
	RawBuffer processWp(const ConnShPtr &, RawBuffer &);
	RawBuffer processAdmin(const ConnShPtr &, RawBuffer &);

	ThreadPool m_workqueue;

	std::unique_ptr<CsLoader> m_cs;
	std::unique_ptr<WpLoader> m_wp;
	std::unique_ptr<Db::Manager> m_db;

	CsLogic m_cslogic;
	WpLogic m_wplogic;
	EmLogic m_emlogic;

};

}
