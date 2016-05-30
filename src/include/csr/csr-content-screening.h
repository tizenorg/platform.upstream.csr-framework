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
 * @file        csr-content-screening.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef __CSR_CONTENT_SCREENING_API_H_
#define __CSR_CONTENT_SCREENING_API_H_

#include <time.h>
#include <stdbool.h>
#include <stddef.h>

#include <csr-content-screening-types.h>
#include <csr-error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_CSR_FRAMEWORK_CS_MODULE
 * @{
 */


/**
 * @brief Initializes and returns a Content Screening API handle.
 *
 * @since_tizen 3.0
 *
 * @remarks A Content Screening API handle (or CSR CS handle) is obtained using the
 *          csr_cs_context_create() function. The handle is required for subsequent
 *          CSR CS API calls. The csr_cs_context_destroy() function releases/closes
 *          the handle. Multiple handles can be obtained using csr_cs_context_create().
 *
 * @param[out] handle A pointer of CSR CS context handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a handle is invalid
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_context_create(csr_cs_context_h *handle);

/**
 * @brief Releases all system resources associated with a Content Screening API handle.
 *
 * @since_tizen 3.0
 *
 * @param[in] handle CSR CS context handle returned by csr_cs_context_create().
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_context_destroy(csr_cs_context_h handle);

/**
 * @brief Sets a popup option in case that a malware is detected.
 *
 * @since_tizen 3.0
 *
 * @remarks If #CSR_CS_ASK_USER is set, a popup will be prompted to a user when a malware
 *          is detected(#CSR_CS_SEVERITY_MEDIUM or #CSR_CS_SEVERITY_HIGH). If
 *          #CSR_CS_NOT_ASK_USER is set, no popup will be prompted even when a malware is
 *          detected. The default value of this option is #CSR_CS_ASK_USER. When a user
 *          selects to remove a detected malicious content, the content will be removed
 *          only if the client has the permission to remove the content.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] ask_user  A popup option in case for a risky URL.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a ask_user is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_ask_user(csr_cs_context_h handle, csr_cs_ask_user_e ask_user);

/**
 * @brief Sets a popup message of a client in case that a malware is detected.
 *
 * @since_tizen 3.0
 *
 * @remarks When a popup is prompted to a user, the message set by this method will be
 *          shown. When a client doesn't set his own popup message, the default message
 *          will be shown in the popup.
 *
 * @param[in] handle  CSR CS context handle returned by csr_cs_context_create().
 * @param[in] message a message in a popup.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a message is too long or empty. Max size
 *                                          is 64 bytes.
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 *
 * @see csr_cs_context_create()
 */
int csr_cs_set_popup_message(csr_cs_context_h handle, const char *message);

/**
 * @brief Sets a maximum core usage during scanning.
 *
 * @since_tizen 3.0
 *
 * @remarks If a core usage is not set, CSR_CS_USE_CORE_DEFAULT will be used.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] usage     A maximum core usage during scanning.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a usage is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_core_usage(csr_cs_context_h handle, csr_cs_core_usage_e usage);

/**
 * @brief Sets a database which is used in scanning.
 *
 * @since_tizen 3.0
 *
 * @remarks If a database is not set or an engine does not support "scanning on cloud",
 *          the scanning will be done in a local device.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_scan_on_cloud(csr_cs_context_h handle);

/**
 * @brief Main function for caller to scan a data buffer for malware.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  Scan data synchronously.
 * @remarks  The @a detected will be released when @a handle is destroyed.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  data       A scan target data.
 * @param[in]  length     A size of a scan target data.
 * @param[out] detected   A pointer of the detected malware handle. It can be null when no malware detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a data or @a detected is invalid
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_scan_data(csr_cs_context_h handle,
					 const unsigned char *data,
					 size_t length,
					 csr_cs_malware_h *detected);

/**
 * @brief Main function for caller to scan a file specified by file path for malware.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  Scan file synchronously.
 * @remarks  The @a detected will be released when @a handle is destroyed.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of scan target file.
 * @param[out] detected   A pointer of the detected malware handle. It can be null when no malware detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_path or @a detected is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_REMOVE_FAILED         File remove failed when malware exist and
 *                                          user select to remove by popup. @a detected
 *                                          will be allocated on this error unlike others.
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_scan_file(csr_cs_context_h handle,
					 const char *file_path,
					 csr_cs_malware_h *detected);

/**
 * @brief Sets a callback function for detection of a malware.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for detection of a malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_detected_cb(csr_cs_context_h handle, csr_cs_detected_cb callback);

/**
 * @brief Sets a callback function for scanning completed without an error.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for scanning completed without an error.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_completed_cb(csr_cs_context_h handle, csr_cs_completed_cb callback);

/**
 * @brief Sets a callback function for scanning cancelled.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for scanning cancelled.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_cancelled_cb(csr_cs_context_h handle, csr_cs_cancelled_cb callback);

/**
 * @brief Sets a callback function for scanning stopped with an error.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for scanning stopped with an error.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_error_cb(csr_cs_context_h handle, csr_cs_error_cb callback);


/**
 * @brief Sets a callback function for the case that a file scan is completed.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for the case that a file scan is completed.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_file_scanned_cb(csr_cs_context_h handle, csr_cs_file_scanned_cb callback);

/**
 * @brief Main function for caller to scan files specified by an array of file paths
 *        for malware.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  Asynchronous function. The caller should set callback functions before
 *           calls this.
 *
 * @param[in]  handle       CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_paths   An array of scan target files.
 * @param[in]  count        A number of scan target files.
 * @param[in]  user_data    The pointer of a user data. It can be null.\n
 *                          It is delivered back to the client when a callback function is called.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_paths is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_scan_files_async(csr_cs_context_h handle,
							const char *file_paths[],
							size_t count,
							void *user_data);

/**
 * @brief Main function for caller to scan a directory specified by
 *        directory path for malware.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  Asynchronous function. The caller should set callback functions before calls
 *           this. All files under target directory which can be accessed by a client are
 *           scanned.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_path   A path of scan target directory.
 * @param[in]  user_data  The pointer of a user data. It can be null.\n
 *                        It is delivered back to the client when a callback function is called.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a dir_path is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_scan_dir_async(csr_cs_context_h handle,
						  const char *dir_path,
						  void *user_data);

/**
 * @brief Main function for caller to scan directories specified by
 *        an array of directory paths for malware.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  Asynchronous function. The caller should set callback functions before calls
 *           this. All files under target directories which can be accessed by a client
 *           are scanned.
 *
 * @param[in]  handle       CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_paths    An array of scan target directories.
 * @param[in]  count        A number of scan target directories.
 * @param[in]  user_data    The pointer of a user data. It can be null.
 *                          It is delivered back to the client when a callback
 *                          function is called.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a dir_paths is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_scan_dirs_async(csr_cs_context_h handle,
						   const char *dir_paths[],
						   size_t count,
						   void *user_data);


/**
 * @brief Cancels a running scanning task.
 *
 * @since_tizen 3.0
 *
 * @remarks It's only for an asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_NO_TASK               No task to cancel
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_cancel_scanning(csr_cs_context_h handle);

//==============================================================================
// Result related
//==============================================================================

/**
 * @brief Extracts the severity of a detected malware from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  detected    A detected malware handle returned
 *                         by csr_cs_result_get_detected_by_idx() or
 *                         csr_cs_result_get_detected_most_severe().
 * @param[out] severity    A pointer of the severity of a detected malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a severity is invalid.
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_get_severity(csr_cs_malware_h detected, csr_cs_severity_level_e *severity);

/**
 * @brief Extracts the name of a detected malware from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a malware_name must be released using free().
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] malware_name  A pointer of the name of a detected malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a malware_name is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_get_malware_name(csr_cs_malware_h detected, char **malware_name);

/**
 * @brief Extracts an url that contains detailed information on vendor's web site from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a detailed_url must be released using free().
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] detailed_url  A pointer of an url that contains detailed information on vendor's web site.\n
 *                           It can be null if a vendor doesn't provide this information.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a detailed_url is invalid.
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_get_detailed_url(csr_cs_malware_h detected, char **detailed_url);

/**
 * @brief Extracts the time stamp when a malware is detected from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  detected   A detected malware handle.
 * @param[out] timestamp  A pointer of the time stamp in milli second when a malware is detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a timestamp is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_get_timestamp(csr_cs_malware_h detected, time_t *timestamp);

/**
 * @brief Extracts the file name where a malware is detected from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a file_name must be released using free().
 *
 * @param[in]  detected   A detected malware handle.
 * @param[out] file_name  A pointer of the file name where a malware is detected. The file name is Null for csr_cs_scan_data.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a file_name is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_get_file_name(csr_cs_malware_h detected, char **file_name);

/**
 * @brief Extracts a user response of a popup from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] response      A pointer of the user response.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a response is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_get_user_response(csr_cs_malware_h detected, csr_cs_user_response_e *response);

/**
 * @brief Checks if a malware was detected in an application or in a file.
 *
 * @since_tizen 3.0
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] is_app        A pointer of a flag indicating the position a malware was detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a is_app is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_is_app(csr_cs_malware_h detected, bool *is_app);

/**
 * @brief Extracts the package id of an application where a malware is detected from detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a pkg_id must be released using free().
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] pkg_id        A pointer of the pakcage id where a malware is detected. This is a null when a malware was not detected in an application.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pkg_id is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_get_pkg_id(csr_cs_malware_h detected, char **pkg_id);

/**
 * @brief Judges how a detected malware file is handled.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @remarks  A detected malware may be removed or ignored. When action is
 *           #CSR_CS_ACTION_REMOVE, the detected malware file will be removed. If a
 *           detected malware is in an application, the application will be removed.
 *           Otherwise, only the file will be removed. When a client removes a detected
 *           malware with this API, the client must have the privilege to remove it.
 *           When action is #CSR_CS_ACTION_IGNORE, the detected malware file won't be
 *           removed. And the ignored file will not treated as malicious. When action is
 *           #CSR_CS_ACTION_UNIGNORE, the ignored file will be considered as malicious
 *           again.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  detected   A handle of a detected malware.
 * @param[in]  action     An action to be taken.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a detected or @a action is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File to take action on not found
 * @retval #CSR_ERROR_FILE_CHANGED          File to take action on changed after detection
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_judge_detected_malware(csr_cs_context_h handle,
								csr_cs_malware_h detected,
								csr_cs_action_e action);


/**
 * @brief Gets information on a detected malware file specified by file path.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  The @a detected will be released when @a handle is destroyed.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of a detected malware file.
 * @param[out] detected  A pointer of the detected malware handle. It can be null when
 *                        no malware file.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_path or @a detected is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     No malware file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_detected_malware(csr_cs_context_h handle,
								const char *file_path,
								csr_cs_malware_h *detected);

/**
 * @brief Gets information on a detected malware files specified by directory path.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  The @a list will be released when @a handle is destroyed.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_paths  A directory path where detected malware files exists.
 * @param[in]  count      Count of array element of @a dir_paths
 * @param[out] list       A pointer of the detected malware list handle. It can be null
 *                        when no malware file.
 * @param[out] list_count Count of detected malware files which existed in the specified
 *                        directory.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a dir_paths, @a list, or @a count is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     No malware file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_detected_malwares(csr_cs_context_h handle,
								 const char *dir_paths[], size_t count,
								 csr_cs_malware_list_h *list, size_t *list_count);

/**
 * @brief Gets information on a ignored malware file specified by file path.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  The @a detected will be released when @a handle is destroyed.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of a ignored malware file.
 * @param[out] detected  A pointer of the detected malware handle. It can be null when
 *                        no ignored file.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_path or @a detected is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     No ignored file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_ignored_malware(csr_cs_context_h handle, const char *file_path,
							   csr_cs_malware_h *detected);

/**
 * @brief Gets information on a ignored malware files specified by directory path.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  The @a list will be released when @a handle is destroyed.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_paths  A directory path where ignored malware files exists.
 * @param[in]  count      Count of array element of @a dir_paths
 * @param[out] list       A pointer of the detected malware list handle. It can be null
 *                        when no ignored file.
 * @param[out] list_count Count of ignored malware files which existed in the specified
 *                        directory.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a dir_paths, @a list, or @a count is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     No ignored file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_ignored_malwares(csr_cs_context_h handle,
								const char *dir_paths[], size_t count,
								csr_cs_malware_list_h *list, size_t *list_count);

/**
 * @brief Extracts the detected malware handle from the detected malware list handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  list        A detected malware list handle returned by
 *                         csr_cs_get_detected_malwares() or
 *                         csr_cs_get_ignored_malwares().
 * @param[in]  index       An index of a target detected malware handle to get.
 * @param[out] detected    A pointer of the detected malware handle. It can be null when
 *                         index is invalid.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid list
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a index or @a detected is invalid.
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_malware_list_get_detected(csr_cs_malware_list_h list, size_t index,
							  csr_cs_malware_h *detected);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
