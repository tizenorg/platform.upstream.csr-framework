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
 * @file        csr-content-screening-types.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Content screening CAPI enums, handles and callbacks
 */
#ifndef __CSR_CONTENT_SCREENING_TYPES_H_
#define __CSR_CONTENT_SCREENING_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_CSR_FRAMEWORK_TYPES_MODULE
 * @{
 */

/**
 * @brief Severity level of a detected malware
 * @since_tizen 3.0
 */
typedef enum {
	CSR_CS_SEVERITY_LOW    = 0x01,  /**< Low Severity. User can choose how to handle between skip, ignore and remove. */
	CSR_CS_SEVERITY_MEDIUM = 0x02,  /**< Medium Severity. User can choose how to handle between skip, ignore and remove. */
	CSR_CS_SEVERITY_HIGH   = 0x03   /**< High Severity. User can choose how to handle between skip and remove. */
} csr_cs_severity_level_e;

/**
 * @brief The option of asking user about handling a detected malware
 * @since_tizen 3.0
 */
typedef enum {
	CSR_CS_NOT_ASK_USER = 0x00, /**< Do not ask the user even if malicious contents were found.*/
	CSR_CS_ASK_USER     = 0x01  /**< Ask the user when malicious contents were found. */
} csr_cs_ask_user_e;

/**
 * @brief The user response from popup
 * @since_tizen 3.0
 */
typedef enum {
	CSR_CS_NO_ASK_USER            = 0x00, /**< No response from user. */
	CSR_CS_REMOVE                 = 0x01, /**< A user decided to remove a detected malicious content and it was removed. */
	CSR_CS_PROCESSING_ALLOWED     = 0x02, /**< A user decided to process a detected malware. */
	CSR_CS_PROCESSING_DISALLOWED  = 0x03, /**< A user decided not to process a detected malware. */
} csr_cs_user_response_e;

/**
 * @brief The action types for the detected malware files
 * @since_tizen 3.0
 */
typedef enum {
	CSR_CS_ACTION_REMOVE   = 0x00,  /* Remove the detected malware file. */
	CSR_CS_ACTION_IGNORE   = 0x01,  /* Ignore the detected malware file. */
	CSR_CS_ACTION_UNIGNORE = 0x02   /* Unignore the previously ignored file. */
} csr_cs_action_e;

/**
 * @brief Maximum core usage during scanning
 * @since_tizen 3.0
 */
typedef enum {
	CSR_CS_USE_CORE_DEFAULT = 0x00,  /* Use default setting value. */
	CSR_CS_USE_CORE_ALL     = 0x01,  /* Use all cores during scanning. */
	CSR_CS_USE_CORE_HALF    = 0x02,  /* Use half cores during scanning. */
	CSR_CS_USE_CORE_SINGLE  = 0x03   /* Use a single core during scanning. */
} csr_cs_core_usage_e;

/**
 * @brief Context handle of content screening APIs.
 * @since_tizen 3.0
 */
typedef struct __csr_cs_context_s *csr_cs_context_h;

/**
 * @brief Detected malware handle.
 * @since_tizen 3.0
 */
typedef struct __csr_cs_malware_s *csr_cs_malware_h;

/**
 * @brief Detected malware list handle.
 * @since_tizen 3.0
 */
typedef struct __csr_cs_malware_list_s *csr_cs_malware_list_h;

/**
 * @brief Engine info handle.
 * @since_tizen 3.0
 */
typedef struct __csr_cs_engine_s *csr_cs_engine_h;

/**
 * @brief Called when each file scanning is done without malware.
 *
 * @since_tizen 3.0
 *
 * @remarks Only for asynchronous scan functions.
 * @remarks Called for each file or application which is not detected malware.
 *
 * @param[in] user_data  A pointer of a user data. It's provided by client
 *                       when calling asyncronous scanning method.
 * @param[in] file_path  A path of the file scanned. It would be package path if it's
 *                       in application.
 *
 * @see csr_cs_set_file_scanned_cb()
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
typedef void (*csr_cs_file_scanned_cb)(void *user_data, const char *file_path);

/**
 * @brief Called when each file scanning is done with malware.
 *
 * @since_tizen 3.0
 *
 * @remarks Only for asynchronous scan functions.
 * @remarks Called for each file or application which is detected malware.
 *
 * @param[in] user_data  A pointer of a user data. It's provided by client
 *                       when calling asyncronous scanning method.
 * @param[in] malware    The detected malware handle.
 *
 * @see csr_cs_set_detected_cb()
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
typedef void (*csr_cs_detected_cb)(void *user_data, csr_cs_malware_h malware);

/**
 * @brief Called when scanning is finished successfully.
 *
 * @since_tizen 3.0
 *
 * @remarks Only for asynchronous scan functions.
 * @remarks Called only once at the end of scanning when success.
 *
 * @param[in] user_data  A pointer of a user data. It's provided by client
 *                       when calling asyncronous scanning method.
 *
 * @see csr_cs_set_completed_cb()
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
typedef void (*csr_cs_completed_cb)(void *user_data);

/**
 * @brief Called when scanning is cancelled by csr_cs_cancel_scanning().
 *
 * @since_tizen 3.0
 *
 * @remarks Only for asynchronous scan functions.
 * @remarks Called only once at the end of scanning by being cancelled.
 *
 * @param[in] user_data  A pointer of a user data. It's provided by client
 *                       when calling asyncronous scanning method.
 *
 * @see csr_cs_set_cancelled_cb()
 * @see csr_cs_cancel_scanning()
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
typedef void (*csr_cs_cancelled_cb)(void *user_data);

/**
 * @brief Called when scanning is stopped with an error.
 *
 * @since_tizen 3.0
 *
 * @remarks Only for asynchronous scan functions.
 * @remarks Called only once at the end of scanning when failed with error.
 *
 * @param[in] user_data   A pointer of a user data. It's provided by client
 *                        when calling asynchronous scanning method.
 * @param[in] error_code  Error code of #csr_error_e defined in csr-error.h
 *
 * @see csr_cs_set_error_cb()
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
typedef void (*csr_cs_error_cb)(void *user_data, int error_code);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
