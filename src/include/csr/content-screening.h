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
 * @file        content-screening.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef __CSR_CONTENT_SCREENING_API_H_
#define __CSR_CONTENT_SCREENING_API_H_

#include <time.h>
#include <stdbool.h>
#include <stddef.h>

#include "csr/content-screening-types.h"
#include "csr/error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and returns a Malware Screening API handle.
 *
 * @details A Malware Screening API handle (or CSR CS handle) is obtained using the
 *          csr_cs_context_create() function. The handle is required for subsequent
 *          CSR CS API calls. The csr_cs_context_destroy() function releases/closes
 *          the handle. Multiple handles can be obtained using csr_cs_context_create().
 *
 * @param[out] phandle A pointer of CSR CS context handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     phandle is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Permission denied
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_context_create(csr_cs_context_h *phandle);

/**
 * @brief Releases all system resources associated with a Malware Screening API handle.
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
 * @details If #CSR_CS_ASK_USER is set, a popup will be prompted to a user when a malware
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
 * @retval #CSR_ERROR_INVALID_PARAMETER     ask_user is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_ask_user(csr_cs_context_h handle, csr_cs_ask_user_e ask_user);

/**
 * @brief Sets a popup message of a client in case that a malware is detected.
 *
 * @details When a popup is prompted to a user, the message set by this method will be
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
 * @retval #CSR_ERROR_INVALID_PARAMETER     message is invalid. Max size is 64 bytes.
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 *
 * @see csr_cs_context_create()
 */
int csr_cs_set_popup_message(csr_cs_context_h handle, const char *message);

/**
 * @brief Sets a maxium core usage during scanning.
 *
 * @details If a core usage is not set, CSR_CS_USE_CORE_DEFAULT will be used.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] usage     A maxium core usage during scanning.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     usage is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_core_usage(csr_cs_context_h handle, csr_cs_core_usage_e usage);

/**
 * @brief Sets a database which is used in scanning.
 *
 * @details If a database is not set or an engine does not support "scanning on cloud",
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
 * @details  Scan data synchronously.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  data       A scan target data.
 * @param[in]  length     A size of a scan target data.
 * @param[out] pdetected  A pointer of the detected malware handle. It can be null when no malware detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     data or presult is invalid
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
					 csr_cs_detected_h *pdetected);

/**
 * @brief Main function for caller to scan a file specified by file path for malware.
 *
 * @details  Scan file synchronously.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of scan target file.
 * @param[out] pdetected  A pointer of the detected malware handle. It can be null when no malware detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     file_path or presult is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_REMOVE_FAILED         File remove failed when malware exist and
 *                                          user select to remove by popup. @a pdetected
 *                                          will be allocated on this error unlike others.
 * @retval #CSR_ERROR_FILE_NOT_FOUND        File not found
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
					 csr_cs_detected_h *pdetected);

/**
 * @brief Sets a callback function for detection of a malware.
 *
 * @details Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  a callback function for detection of a malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_callback_on_detected(csr_cs_context_h handle,
									csr_cs_on_detected_cb callback);

/**
 * @brief Sets a callback function for scanning completed without an error.
 *
 * @details Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  a callback function for scanning completed without an error.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_callback_on_completed(csr_cs_context_h handle,
									 csr_cs_on_completed_cb callback);

/**
 * @brief Sets a callback function for scanning cancelled.
 *
 * @details Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  a callback function for scanning cancelled.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_callback_on_cancelled(csr_cs_context_h handle,
									 csr_cs_on_cancelled_cb callback);

/**
 * @brief Sets a callback function for scanning stopped with an error.
 *
 * @details Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  a callback function for scanning stopped with an error.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_callback_on_error(csr_cs_context_h handle,
								 csr_cs_on_error_cb callback);


/**
 * @brief Sets a callback function for the case that a file scan is completed.
 *
 * @details Callback for asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  a callback function for the case that a file scan is completed.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     callback is invalid
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_set_callback_on_file_scanned(csr_cs_context_h handle,
										csr_cs_on_file_scanned_cb callback);

/**
 * @brief Main function for caller to scan files specified by an array of file paths
 *        for malware.
 *
 * @details  Asynchronous function. The caller should set callback functions before
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
 * @retval #CSR_ERROR_INVALID_PARAMETER     file_paths or count is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_FILE_NOT_FOUND        File not found
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
 * @brief Main function for caller to scan a directoy specified by
 *        directory path for malware.
 *
 * @details  Asynchronous function. The caller should set callback functions before calls
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
 * @retval #CSR_ERROR_INVALID_PARAMETER     file_path or presult is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_FILE_NOT_FOUND        File not found
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
 * @details  Asynchronous function. The caller should set callback functions before calls
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
 * @retval #CSR_ERROR_INVALID_PARAMETER     dir_paths or count is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_FILE_NOT_FOUND        File not found
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
 * @details It's only for an asynchronous scan function.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  a callback function for the case that a file scan is completed.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     callback is invalid
 * @retval #CSR_ERROR_NO_TASK               No task to cancel
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_scan_cancel(csr_cs_context_h handle);

//==============================================================================
// Result related
//==============================================================================

/**
 * @brief extracts the severity of a detected malware from the detected malware handle.
 *
 * @param[in]  detected    A detected malware handle returned
 *                         by csr_cs_result_get_detected_by_idx() or
 *                         csr_cs_result_get_detected_most_severe().
 * @param[out] pseverity  A pointer of the severity of a detected malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    pseverity is invalid.
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_get_severity(csr_cs_detected_h detected,
								 csr_cs_severity_level_e *pseverity);

/**
 * @brief extracts the name of a detected malware from the detected malware handle.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] malware_name  A pointer of the name of a detected malware. A caller
 *                           should not free this string.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    malware_name is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_get_malware_name(csr_cs_detected_h detected,
									 const char **pmalware_name);

/**
 * @brief extracts an url that contains detailed information on vendor's web site from the detected malware handle.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] detailed_url  A pointer of an url that contains detailed information on vendor's web site.\n
 *                           It can be null if a vendor doesn't provide this information.\n
 *                           A caller should not free this string.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    malware_name is invalid.
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_get_detailed_url(csr_cs_detected_h detected,
									 const char **pdetailed_url);

/**
 * @brief extracts the time stamp when a malware is detected from the detected malware handle.
 *
 * @param[in]  detected   A detected malware handle.
 * @param[out] timestamp  A pointer of the time stamp in milli second when a malware is detected. A caller should not free this.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    timestamp is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_get_timestamp(csr_cs_detected_h detected,
								  time_t *ptimestamp);

/**
 * @brief extracts the file name where a malware is detected from the detected malware handle.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] file_name  A pointer of the file name where a malware is detected. A caller should not free this string. The file name is Null for csr_cs_scan_data.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    file_name is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_get_file_name(csr_cs_detected_h detected,
								  const char **pfile_name);

/**
 * @brief extracts a user reponse of a popup from the detected malware handle.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] presponse     A pointer of the user response.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    presponse is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_get_user_response(csr_cs_detected_h detected,
									  csr_cs_user_response_e *presponse);

/**
 * @brief check if a malware was detected in an application or in a file.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] pis_app       A pointer of a flag indicating the position a malware was detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    pis_app is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_is_app(csr_cs_detected_h detected, bool *pis_app);

/**
 * @brief extracts the package id of an application where a malware is detected from detected malware handle.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] ppkg_id       A pointer of the pakcage id where a malware is detected. A caller should not free this string. This is a null when a malware was not detected in an application.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    ppkg_id is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_detected_get_pkg_id(csr_cs_detected_h detected,
							   const char **ppkg_id);

/**
 * @brief Judges how a detected malware file is handled.
 *
 * @details  A detected malware may be removed or ignored. When action is
 *           #CSR_CS_ACTION_REMOVE, the detected malware file will be removed. If a
 *           detected malware is in an application, the application will be removed.
 *           Otherwise, only the file will be removed. When a client removes a detected
 *           malware with this API, the client must have the privilege to remove it.
 *           When action is #CSR_CS_ACTION_IGNORE, the dectected malware file won't be
 *           removed. And the ignored file will not treated as malicious. When action is
 *           #CSR_CS_ACTION_UNIGNORE, the ignored file will be considered as mailicous
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
 * @retval #CSR_ERROR_INVALID_PARAMETER     detected or action is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_NOT_FOUND        File to take action on not found
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_judge_detected_malware(csr_cs_context_h handle,
								  csr_cs_detected_h detected, csr_cs_action_e action);


/**
 * @brief Gets information on a detected malware file specified by file path.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of a detected malware file.
 * @param[out] pdetected  A pointer of the detected malware handle. It can be null when
 *                        no malware file.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     file_path or action is invalid
 * @retval #CSR_ERROR_FILE_NOT_FOUND        No malware file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_detected_malware(csr_cs_context_h handle, const char *file_path,
								csr_cs_detected_h *pdetected);

/**
 * @brief Gets information on a detected malware files specified by directory path.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_paths  A directory path where detected malware files exists.
 * @param[in]  count      Count of array element of @a dir_paths
 * @param[out] plist      A pointer of the detected malware list handle. It can be null
 *                        when no malware file.
 * @param[out] pcount     Count of detected malware files which existed in the specified
 *                        directory.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     file_path or action is invalid
 * @retval #CSR_ERROR_FILE_NOT_FOUND        No malware file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_detected_malwares(csr_cs_context_h handle,
								 const char *dir_paths[], size_t count,
								 csr_cs_detected_list_h *plist, size_t *pcount);

/**
 * @brief Gets information on a ignored malware file specified by file path.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of a ignored malware file.
 * @param[out] pdetected  A pointer of the detected malware handle. It can be null when
 *                        no ignored file.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     file_path or action is invalid
 * @retval #CSR_ERROR_FILE_NOT_FOUND        No ignored file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_ignored_malware(csr_cs_context_h handle, const char *file_path,
							   csr_cs_detected_h *pdetected);

/**
 * @brief Gets information on a ignored malware files specified by directory path.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_paths  A directory path where ignored malware files exists.
 * @param[in]  count      Count of array element of @a dir_paths
 * @param[out] plist      A pointer of the detected malware list handle. It can be null
 *                        when no ignored file.
 * @param[out] pcount     Count of ignored malware files which existed in the specified
 *                        directory.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     file_path or action is invalid
 * @retval #CSR_ERROR_FILE_NOT_FOUND        No ingored file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 */
int csr_cs_get_ignored_malwares(csr_cs_context_h handle,
								const char *dir_paths[], size_t count,
								csr_cs_detected_list_h *plist, size_t *pcount);

/**
 * @brief Extracts the detected malware handle from the detected malware list handle.
 *
 * @param[in]  list        A detected malware list handle returned by
 *                         csr_cs_get_detected_malwares() or
 *                         csr_cs_get_ignored_malwares().
 * @param[in]  index       An index of a target detected malware handle to get.
 * @param[out] pdetected   A pointer of the detected malware handle. It can be null when
 *                         index is invalid.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid list
 * @retval #CSR_ERROR_INVALID_PARAMETER    index or pdetected is invalid.
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_cs_dlist_get_detected(csr_cs_detected_list_h list, size_t index,
							  csr_cs_detected_h *pdetected);

#ifdef __cplusplus
}
#endif

#endif
