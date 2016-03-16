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
 * @file        error.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       csr error codes
 */
#ifndef __TIZEN_CORE_CSR_ERROR_H_
#define __TIZEN_CORE_CSR_ERROR_H_

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIZEN_ERROR_CSR 0x60 // 0110 0000

typedef enum {
	CSR_ERROR_NONE              = TIZEN_ERROR_NONE,              /**< Success */
	CSR_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid function parameter */
	CSR_ERROR_OUT_OF_MEMORY     = TIZEN_ERROR_OUT_OF_MEMORY,     /**< Out of memory */
	CSR_ERROR_PERMISSION_DEINED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permission denied */
	CSR_ERROR_SOCKET            = TIZEN_ERROR_CSR | 0x01,        /**< Socket error between client and csr-server */
	CSR_ERROR_BAD_REQUEST       = TIZEN_ERROR_CSR | 0x02,        /**< Invalid request from client */
	CSR_ERROR_BAD_RESPONSE      = TIZEN_ERROR_CSR | 0x03,        /**< Invalid response from csr-server */
	CSR_ERROR_SEND_FAILED       = TIZEN_ERROR_CSR | 0x04,        /**< Transmitting request failed */
	CSR_ERROR_RECV_FAILED       = TIZEN_ERROR_CSR | 0x05,        /**< Receiving response failed */
	CSR_ERROR_SERVER            = TIZEN_ERROR_CSR | 0x06,        /**< csr-server has been failed for some reason */
	CSR_ERROR_UNKNOWN           = TIZEN_ERROR_CSR | 0xFF         /**< Unknown reason */
} csr_error_e;

#ifdef __cplusplus
}
#endif

#endif
