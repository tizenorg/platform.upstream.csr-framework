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
 * @file        engine-manager.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR Engine manager APIs
 */
#include "csr/engine-manager.h"

#include "client/utils.h"
#include "common/audit/logger.h"

API
int csr_get_current_engine(csr_engine_id_e id, csr_engine_h *engine)
{
	(void) id;
	(void) engine;
	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_engine_get_vendor(csr_engine_h engine, char **vendor)
{
	(void) engine;
	(void) vendor;
	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_engine_get_name(csr_engine_h engine, char **name)
{
	(void) engine;
	(void) name;
	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_engine_get_version(csr_engine_h engine, char **version)
{
	(void) engine;
	(void) version;
	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_engine_get_data_version(csr_engine_h engine, char **version)
{
	(void) engine;
	(void) version;
	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_engine_get_activated(csr_engine_h engine, csr_activated_e *pactivated)
{
	(void) engine;
	(void) pactivated;

	DEBUG("start");
	return CSR_ERROR_NONE;
}

API
int csr_engine_destroy(csr_engine_h engine)
{
	(void) engine;
	DEBUG("start");
	return CSR_ERROR_NONE;
}
