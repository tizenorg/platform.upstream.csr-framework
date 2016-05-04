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
 * @file        content-screening-types.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef __CSR_CONTENT_SCREENING_TYPES_H_
#define __CSR_CONTENT_SCREENING_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Severity level of an detected malware
 */
typedef enum {
	CSR_CS_SEVERITY_LOW    = 0x01,  /**< Low Severity. Process with a warning log. */
	CSR_CS_SEVERITY_MEDIUM = 0x02,  /**< Medium Severity. Prompt the user before processing. Ask the user if the user wants the application to process the data. */
	CSR_CS_SEVERITY_HIGH   = 0x03   /**< High Severity. Do not process the data and prompt user for removal. */
} csr_cs_severity_level_e;

/**
 * @brief the options about prompting a popup to a user.
 */
typedef enum {
	CSR_CS_NOT_ASK_USER = 0x00, /**< Do not ask the user even if malicious contents were found.*/
	CSR_CS_ASK_USER     = 0x01  /**< Ask the user when malicious contents were found */
} csr_cs_ask_user_e;

/**
 * @brief the user response for a popup.
 */
typedef enum {
	CSR_CS_NO_ASK_USER            = 0x00, /**< There was no popup for asking the user. */
	CSR_CS_REMOVE                 = 0x01, /**< A user decided to remove a detected malicious content and it was removed */
	CSR_CS_IGNORE                 = 0x02, /**< A user decided to ignore a detected malicious content. */
	CSR_CS_SKIP                   = 0x03, /**< A user decided to skip a detected malicious content. */
	CSR_CS_PROCESSING_ALLOWED     = 0x04, /**< A user decided to process a detected malicious data. */
	CSR_CS_PROCESSING_DISALLOWED  = 0x05, /**< A user decided not to process a detected malicious data. */
} csr_cs_user_response_e;

/**
 * @brief  the action types for the detected malware files
 */
typedef enum {
	CSR_CS_ACTION_REMOVE   = 0x00,  /* Remove the detected malware file */
	CSR_CS_ACTION_IGNORE   = 0x01,  /* Ignore the detected malware file */
	CSR_CS_ACTION_UNIGNORE = 0x02   /* Unignore the previously ignored file */
} csr_cs_action_e;

/**
 * @brief Maximum core usage during scanning
 */
typedef enum {
	CSR_CS_USE_CORE_DEFAULT = 0x00,  /* Use default setting value */
	CSR_CS_USE_CORE_ALL     = 0x01,  /* Use all cores during scanning. */
	CSR_CS_USE_CORE_HALF    = 0x02,  /* Use half cores during scanning. */
	CSR_CS_USE_CORE_SINGLE  = 0x03   /* Use a single core during scanning */
} csr_cs_core_usage_e;

/**
 * @brief TCS context handle.
 */
typedef struct __csr_cs_context_s *csr_cs_context_h;

/**
 * @brief TCS detected malware info handle.
 */
typedef struct __csr_cs_detected_s *csr_cs_detected_h;

/**
 * @brief TCS detected malware list handle
 */
typedef struct __csr_cs_detected_list_s *csr_cs_detected_list_h;

/**
 * @brief TCS engine info handle.
 */
typedef struct __csr_cs_engine_s *csr_cs_engine_h;

/**
 * @brief The callback function is called  when a malware detected.\
 *        It's only for an asynchronous scan function.
 */
typedef void (*csr_cs_on_detected_cb)(void *user_data,
									  csr_cs_detected_h detected);

/**
 * @brief The callback function is called  when scanning is fininshed without an error.\
 *        It's only for an asynchronous scan function.
 */
typedef void (*csr_cs_on_completed_cb)(void *user_data);

/**
 * @brief The callback function is called  when scanning is cancelled without an error.\
 *        It's only for an asynchronous scan function.
 */
typedef void (*csr_cs_on_cancelled_cb)(void *user_data);

/**
 * @brief The callback function is called when scanning is fininshed with an error.\
 *        It's only for an asynchronous scan function.
 */
typedef void (*csr_cs_on_error_cb)(void *user_data, int error_code);

/**
 * @brief The callback function is called when a file scan is completed.\
 *        It's only for an asynchronous scan function.
 */
typedef void (*csr_cs_on_file_scanned_cb)(void *user_data,
		const char *file_path);

#ifdef __cplusplus
}
#endif

#endif
