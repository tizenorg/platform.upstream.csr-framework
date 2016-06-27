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
 * @file        async-protocol.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Protocol for asynchronous scanning operations
 */
#pragma once

namespace Csr {

// should be positive values not to be conflicted with error code in csr-error.h
typedef enum {
	ASYNC_EVENT_START            = 0x10, // operation started
	ASYNC_EVENT_COMPLETE         = 0x20, // operation completed
	ASYNC_EVENT_CANCEL           = 0x30, // operation cancelled
	ASYNC_EVENT_MALWARE_NONE     = 0x40, // target scanned and no malware detected
	ASYNC_EVENT_MALWARE_DETECTED = 0x50, // target scanned and malware detected
} async_op_protocol_e;

}
