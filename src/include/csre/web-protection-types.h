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
 * @file        web-protection-types.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef __CSRE_WEB_PROTECTION_TYPES_H_
#define __CSRE_WEB_PROTECTION_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TWP(Tizen Web Screening) Engine API version.
 */
#define CSRE_WP_API_VERSION "1.0"

/**
 * TWP Engine context handle.
 */
typedef struct csre_wp_context_s* csre_wp_context_h;

/**
 * TWP Engine's check result handle.
 */
typedef struct csre_wp_check_result_s* csre_wp_check_result_h;

/**
 * @brief Risk level of a url
 */
typedef enum {
	CSRE_WP_RISK_LOW        = 0x01,  /**< Risk Low. */
	CSRE_WP_RISK_UNVERIFIED = 0x02,  /**< Risk Unverified. There is no information about the url.*/
	CSRE_WP_RISK_MEDIUM     = 0x03,  /**< Risk Medium. Prompt the user before processing. Ask the user if they want the application to process the url. */
	CSRE_WP_RISK_HIGH       = 0x04   /**< High Risk.Do not process the url and just notify the user */
} csre_wp_risk_level_e;

#ifdef __cplusplus
}
#endif

#endif
