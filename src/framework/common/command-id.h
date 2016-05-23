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
 * @file        command-id.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

namespace Csr {

enum class CommandId : int {
	// content scanning
	// scanning
	SCAN_DATA           = 0x1001,
	SCAN_FILE           = 0x1002,
	// delta, history
	GET_DETECTED        = 0x1101,
	GET_DETECTED_LIST   = 0x1102,
	GET_IGNORED         = 0x1103,
	GET_IGNORED_LIST    = 0x1104,
	GET_SCANNABLE_FILES = 0x1105,
	SET_DIR_TIMESTAMP   = 0x1106,
	// handle result
	JUDGE_STATUS        = 0x1201,

	// web protection
	CHECK_URL           = 0x2001,

	// engine management
	EM_GET_VENDOR       = 0x3001,
	EM_GET_NAME         = 0x3002,
	EM_GET_VERSION      = 0x3003,
	EM_GET_DATA_VERSION = 0x3004,
	EM_GET_UPDATED_TIME = 0x3005,
	EM_GET_ACTIVATED    = 0x3006,
	EM_GET_STATE        = 0x3007,
	EM_SET_STATE        = 0x3008
};

}
