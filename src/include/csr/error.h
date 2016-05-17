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
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef __CSR_ERROR_H_
#define __CSR_ERROR_H_

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/* define it temporary until this code goes into capi-base-common package */
#ifndef TIZEN_ERROR_CSR
#define TIZEN_ERROR_CSR 0x6000 // 0110 0000 0000 0000
#endif


/**
 * @brief Enumeration for CSR Errors.
 * @since_tizen 3.0
 */
typedef enum {
	CSR_ERROR_NONE                  = TIZEN_ERROR_NONE,              /**< Successful */
	CSR_ERROR_INVALID_PARAMETER     = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid function parameter */
	CSR_ERROR_OUT_OF_MEMORY         = TIZEN_ERROR_OUT_OF_MEMORY,     /**< Out of memory */
	CSR_ERROR_PERMISSION_DENIED     = TIZEN_ERROR_PERMISSION_DENIED, /**< Permission denied */
	CSR_ERROR_SOCKET                = TIZEN_ERROR_CSR | 0x01,        /**< Socket error between client and server */
	CSR_ERROR_INVALID_HANDLE        = TIZEN_ERROR_CSR | 0x02,        /**< The given handle is invalid */
	CSR_ERROR_SERVER                = TIZEN_ERROR_CSR | 0x03,        /**< Server has been failed for some reason */
	CSR_ERROR_NO_TASK               = TIZEN_ERROR_CSR | 0x04,        /**< No Task exists*/
	CSR_ERROR_DB                    = TIZEN_ERROR_CSR | 0x05,        /**< DB transaction error */
	CSR_ERROR_REMOVE_FAILED         = TIZEN_ERROR_CSR | 0x06,        /**< Removing file or application is failed */
	CSR_ERROR_FILE_DO_NOT_EXIST     = TIZEN_ERROR_CSR | 0x07,        /**< File not exist */
	CSR_ERROR_FILE_CHANGED          = TIZEN_ERROR_CSR | 0x08,        /**< File changed after detection */
	CSR_ERROR_FILE_SYSTEM           = TIZEN_ERROR_CSR | 0x09,        /**< File type is invalid */
	CSR_ERROR_ENGINE_PERMISSION     = TIZEN_ERROR_CSR | 0x11,        /**< Insufficient permission of engine */
	CSR_ERROR_ENGINE_NOT_EXIST      = TIZEN_ERROR_CSR | 0x12,        /**< No engine exists*/
	CSR_ERROR_ENGINE_DIASABLED      = TIZEN_ERROR_CSR | 0x13,        /**< Engine is in disabled state*/
	CSR_ERROR_ENGINE_NOT_ACTIVATED  = TIZEN_ERROR_CSR | 0x14,        /**< Engine is not activated*/
	CSR_ERROR_ENGINE_INTERNAL       = TIZEN_ERROR_CSR | 0x19,        /**< Engine Internal error*/
	CSR_ERROR_UNKNOWN               = TIZEN_ERROR_CSR | 0xFF,        /**< The error with unknown reason */
} csr_error_e;

/**
 * @brief get the error string for a given error code.
 *
 * @since_tizen 3.0
 *
 * @param[in]  error_code  An error code.
 * @param[out] string      A pointer of the error string. A caller should not free it.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_PARAMETER    error_code or error_string is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_get_error_string(int error_code, char **string);


#ifdef __cplusplus
}
#endif

#endif
