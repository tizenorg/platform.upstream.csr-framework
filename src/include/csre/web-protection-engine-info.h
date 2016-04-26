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
 * @file        web-protection-engine-info.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Interface for engine management on engine
 */
#ifndef __CSRE_WEB_PROTECTION_ENGINE_INFO_H_
#define __CSRE_WEB_PROTECTION_ENGINE_INFO_H_

#include <time.h>
#include "csre/error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct csre_wp_engine_s *csre_wp_engine_h;

/**
 * @brief State of engine activation
 */
typedef enum {
	CSRE_WP_NOT_ACTIVATED = 0x01,  /**< Engine is not activated */
	CSRE_WP_ACTIVATED     = 0x02   /**< Engine is activated */
} csre_wp_activated_e;

/**
 * @brief Gets the handle of a selected engine information.
 *
 * @param[in]  id       Engine identifier to get handle.
 * @param[out] pengine  A pointer of the engine information handle.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    pengine is invalid
 * @retval #CSRE_ERROR_ENGINE_NOT_SELECTED  No engine selected
 * @retval #CSRE_ERROR_ENGINE_NOT_ACTIVATED Engine is not activated
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_wp_engine_get_info(csre_wp_engine_h *engine);

/**
 * @brief Releases all system resources associated with a engine information handle.
 *
 * @param[in]  engine      The engine information handle.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                  Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSRE_ERROR_SOCKET                Socket error between client and server
 * @retval #CSRE_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSRE_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN               Error with unknown reason
 */
int csre_wp_engine_destroy(csre_wp_engine_h engine);

/**
 * @brief returns the engine API version.
 *
 * @param[in]  engine   The engine information handle.
 * @param[out] version  A pointer of the API version string. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    api_ver is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 *
 * @see csre_wp_engine_get_info()
 */
int csre_wp_engine_get_api_version(csre_wp_engine_h engine,
								   const char **version);

/**
 * @brief Extracts an vendor name from the engine information handle.
 *
 * @param[in]  engine  The engine information handle.
 * @param[out] vendor  A pointer of the engine's vendor name. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    engine_vendor is invalid
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csre_wp_engine_get_info()
 */
int csre_wp_engine_get_vendor(csre_wp_engine_h engine, const char **vendor);

/**
 * @brief Extracts an engine name from the engine information handle.
 *
 * @param[in]  engine  The engine information handle.
 * @param[out] name    A pointer of the engine's name. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    engine_name is invalid
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csre_wp_engine_get_info()
 */
int csre_wp_engine_get_name(csre_wp_engine_h engine, const char **name);

/**
 * @brief Extracts an engine version from the engine information handle.
 *
 * @param[in]  engine   An engine information handle.
 * @param[out] version  A pointer of the engine's version. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    engine_version is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csre_wp_engine_get_info()
 */
int csre_wp_engine_get_version(csre_wp_engine_h engine, const char **version);

/**
 * @brief Extracts an engine's data version from the engine information handle.
 *
 * @param[in]  engine   The engine information handle.
 * @param[out] version  A pointer of the data version. It can be null. A caller should not free it.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    engine_version is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csre_wp_engine_get_info()
 */
int csre_wp_engine_get_data_version(csre_wp_engine_h engine,
									const char **version);

/**
 * @brief Extracts the latest update time of an engine from the engine information handle.
 *
 * @param[in]  engine   The engine information handle.
 * @param[out] time     A pointer of lasted update time.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    time is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csre_wp_get_engine_info()
 */
int csre_wp_engine_get_latest_update_time(csre_wp_engine_h engine,
		time_t *time);

/**
 * @brief Extracts the state of engine activation from the engine information handle.
 *
 * @param[in]  engine      The engine information handle.
 * @param[out] pactivated  A pointer of the engine state.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    pactivated is invalid
 * @retval #CSRE_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csre_wp_engine_get_info()
 */
int csre_wp_engine_get_activated(csre_wp_engine_h engine,
								 csre_wp_activated_e *pactivated);

/**
 * @brief Extracts an vendor logo image from the engine information handle.
 *
 * @details If log image is provided, the vendor logo is shown in a popup. The format
 *          of the logo image should be jpeg, gif, bmp or png.
 *
 * @param[in]  engine             The engine information handle.
 * @param[out] vendor_logo_image  A pointer of the vendor logo image. A caller should not
 *                                free it.
 * @param[out] image_size         Size of log image.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    vendor_logo_imgage or image_size is invalid
 * @retval #CSRE_ERROR_NO_DATA              No vendor logo image
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 *
 * @see csre_wp_engine_get_info()
 */
int csre_wp_engine_get_vendor_logo(csre_wp_engine_h engine,
								   unsigned char **vendor_logo_image, unsigned int *image_size);

#ifdef __cplusplus
}
#endif

#endif
