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
#ifndef __CSR_WEB_PROTECTION_TYPES_H_
#define __CSR_WEB_PROTECTION_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum message size.
 */
#define MAX_MESSAGE_SIZE 64

/**
 * CSR WP context handle.
 */
typedef struct __csr_wp_context_s* csr_wp_context_h;

/**
 * CSR WP scan result handle.
 */
typedef struct __csr_wp_check_result_h* csr_wp_check_result_h;

/**
 * @brief the options about prompting a popup to a user.
 */
typedef enum {
	CSR_WP_NOT_ASK_USER = 0x00, /**< Do not ask the user even if a URL turns out risky.*/
	CSR_WP_ASK_USER     = 0x01  /**< Ask the user when a URL turns out risky */
} csr_wp_ask_user_e;

/**
 * @brief the user response for a popup.
 */
typedef enum {
	CSR_WP_NO_ASK_USER           = 0x00, /**< There was no popup for asking the user. */
	CSR_WP_PROCESSING_ALLOWED    = 0x01, /**< A user allowed to process the url. */
	CSR_WP_PROCESSING_DISALLOWED = 0x02  /**< A user disallowed to process the url. */
} csr_wp_user_response_e;

/**
 * @brief Risk level of a url
 */
typedef enum {
	CSR_WP_RISK_LOW        = 0x01,  /**< Risk Low. */
	CSR_WP_RISK_UNVERIFIED = 0x02,  /**< Risk Unverified. There is no information about the url.*/
	CSR_WP_RISK_MEDIUM     = 0x03,  /**< Risk Medium. Prompt the user before processing. Ask the user if they want the application to process the url. */
	CSR_WP_RISK_HIGH       = 0x04   /**< High Risk.Do not process the url and just notify the user */
} csr_wp_risk_level_e;

#ifdef __cplusplus
}
#endif

#endif
