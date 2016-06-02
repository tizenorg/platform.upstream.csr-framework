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
 * @file        csre-content-screening-engine-info.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Interface for engine management on engine
 */
#ifndef __CSRE_CS_ENGINE_INFO_H_
#define __CSRE_CS_ENGINE_INFO_H_

#include <time.h>
#include <csre-error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief State of engine activation
 */
typedef enum {
	CSRE_CS_NOT_ACTIVATED = 0x01,  /**< Engine is not activated */
	CSRE_CS_ACTIVATED     = 0x02   /**< Engine is activated */
} csre_cs_activated_e;

/**
 * @brief returns the engine API version.
 *
 * @param[in]  context  CSR CS context handle.
 * @param[out] version  A pointer of the API version string. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    api_ver is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 */
int csre_cs_engine_get_api_version(csre_cs_context_h context, const char **version);

/**
 * @brief Extracts an vendor name from CSR CS context handle.
 *
 * @param[in]  context CSR CS context handle.
 * @param[out] vendor  A pointer of the engine's vendor name. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    @a vendor is invalid
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_cs_engine_get_vendor(csre_cs_context_h context, const char **vendor);

/**
 * @brief Extracts an engine name from CSR CS context handle.
 *
 * @param[in]  context CSR CS context handle.
 * @param[out] name    A pointer of the engine's name. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    @a name is invalid
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_cs_engine_get_name(csre_cs_context_h context, const char **name);

/**
 * @brief Extracts an engine version from CSR CS context handle.
 *
 * @param[in]  context  CSR CS context handle.
 * @param[out] version  A pointer of the engine's version. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    @a version is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_cs_engine_get_version(csre_cs_context_h context, const char **version);

/**
 * @brief Extracts an engine's data version from CSR CS context handle.
 *
 * @param[in]  context  CSR CS context handle.
 * @param[out] version  A pointer of the data version. It can be null. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    @a version is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_cs_engine_get_data_version(csre_cs_context_h context, const char **version);

/**
 * @brief Extracts the latest update time of an engine from CSR CS context handle.
 *
 * @param[in]  context  CSR CS context handle.
 * @param[out] time     A pointer of lasted update time.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    time is invalid
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_cs_engine_get_latest_update_time(csre_cs_context_h context, time_t *time);

/**
 * @brief Extracts the state of engine activation from CSR CS context handle.
 *
 * @param[in]  context     CSR CS context handle.
 * @param[out] pactivated  A pointer of the engine state.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    @a pactivated is invalid
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_cs_engine_get_activated(csre_cs_context_h context,
								 csre_cs_activated_e *pactivated);

/**
 * @brief Extracts an vendor logo image from CSR CS context handle.
 *
 * @details If log image is provided, the vendor logo is shown in a popup. The format
 *          of the logo image should be jpeg, gif, bmp or png.
 *
 * @param[in]  context            CSR CS context handle.
 * @param[out] vendor_logo_image  A pointer of the vendor logo image. A caller should not
 *                                free it.
 * @param[out] image_size         Size of log image.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid context handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    @a vendor_logo_imgage or @a image_size is invalid
 * @retval #CSRE_ERROR_NO_DATA              No vendor logo image
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 */
int csre_cs_engine_get_vendor_logo(csre_cs_context_h context,
								   unsigned char **vendor_logo_image,
								   unsigned int *image_size);

#ifdef __cplusplus
}
#endif

#endif