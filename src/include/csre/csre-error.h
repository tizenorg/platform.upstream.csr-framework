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
 * @file        csre-error.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef __CSRE_ERROR_H_
#define __CSRE_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration for CSR CS Engine Errors.
 */
typedef enum {
	CSRE_ERROR_NONE                  =  0x00,  /**< Successful */
	CSRE_ERROR_INVALID_PARAMETER     = -0x01,  /**< Invalid parameter */
	CSRE_ERROR_OUT_OF_MEMORY         = -0x02,  /**< Out of memory */
	CSRE_ERROR_INVALID_HANDLE        = -0x03,  /**< The given handle is invalid */
	CSRE_ERROR_ENGINE_NOT_ACTIVATED  = -0x04,  /**< Engine is not activated*/
	CSRE_ERROR_PERMISSION_DENIED     = -0x05,  /**< Access denied */
	CSRE_ERROR_FILE_NOT_FOUND        = -0x06,  /**< File not found */
	CSRE_ERROR_NO_DATA               = -0x07,  /**< Data not found */
	CSRE_ERROR_UNKNOWN               = -0xFF,  /**< The error with unknown reason */
	// -0x0100~-0xFF00 are reserved for engine defined errors.
} csre_error_e;

#ifdef __cplusplus
}
#endif

#endif
