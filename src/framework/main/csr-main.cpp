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
 * @file        csr-main.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Main of csr daemon
 */
#include "service/server-service.h"

#include "common/audit/logger.h"

int main(void)
{
	try {
		INFO("Start csr-server main!");

		Csr::ServerService service;

		INFO("Let's start csr-server service!");

		service.start(SERVICE_IDLE_TIMEOUT_TIME);

		return 0;
	} catch (const std::exception &e) {
		ERROR("std exception occured in csr-server main! what: " << e.what());
		return -1;
	} catch (...) {
		ERROR("Unhandled exception occured in csr-server main!");
		return -1;
	}
}
