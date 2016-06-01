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
 * @brief       Content screening to detect malware in files
 */
#ifndef __CSR_CONTENT_SCREENING_API_H_
#define __CSR_CONTENT_SCREENING_API_H_

#include <time.h>
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
 * @details A Content Screening API handle (or CSR CS handle) is obtained by this method.
 *          The handle is required for subsequent CSR CS API calls.
 *
 * @since_tizen 3.0
 *
 * @remarks The @a handle should be released using csr_cs_context_destroy().
 * @remarks Multiple handles can be obtained.
 *
 * @param[out] handle A pointer of CSR CS context handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        @a handle may be null
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_destroy()
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
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_create()
 */
int csr_cs_context_destroy(csr_cs_context_h handle);

/**
 * @brief Sets a popup option for malware detected.
 *
 * @details If #CSR_CS_ASK_USER_YES is set, a popup will be prompted to a user when a malware
 *          is detected. If #CSR_CS_ASK_USER_NO is set which is default value, no popup
 *          will be prompted even if a malware is detected. User can allow, disallow and
 *          remove detected malware by popup. Selection can be different between malware's
 *          severity.
 *
 * @since_tizen 3.0
 *
 * @remarks This option is disabled(#CSR_CS_ASK_USER_NO) as a default.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] ask_user  Popup option to set or unset.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a ask_user is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_create()
 * @see csr_cs_context_destroy()
 */
int csr_cs_set_ask_user(csr_cs_context_h handle, csr_cs_ask_user_e ask_user);

/**
 * @brief Sets a popup message of a client in case that a malware is detected.
 *
 * @details Default message is "Malware which may harm your device is detected.".
 *
 * @since_tizen 3.0
 *
 * @remarks Meaningful only when ask user option is set by csr_cs_set_ask_user().
 * @remarks The message will be printed on popup for user.
 * @remarks Default popup message will be used if it isn't set.
 *
 * @param[in] handle   CSR CS context handle returned by csr_cs_context_create().
 * @param[in] message  A message to print on a popup.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a message is too long or empty. Max size
 *                                          is 64 bytes.
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_create()
 * @see csr_cs_context_destroy()
 */
int csr_cs_set_popup_message(csr_cs_context_h handle, const char *message);

/**
 * @brief Sets a maximum core usage during scanning.
 *
 * @since_tizen 3.0
 *
 * @remarks If a core usage is not set, #CSR_CS_CORE_USAGE_DEFAULT will be used.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] usage     A maximum core usage during scanning.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a usage is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_create()
 * @see csr_cs_context_destroy()
 */
int csr_cs_set_core_usage(csr_cs_context_h handle, csr_cs_core_usage_e usage);

/**
 * @brief Sets a scan on cloud option.
 *
 * @since_tizen 3.0
 *
 * @remarks Scan on cloud option is turned off as a default.
 * @remarks If an engine does not support "scanning on cloud", this option is silently
 *          ignored.
 *
 * @param[in]  handle       CSR CS context handle returned by csr_cs_context_create().
 * @param[int] is_on_cloud  Flag to turn on(#true) or off(#false) of scan on cloud option.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_create()
 * @see csr_cs_context_destroy()
 */
int csr_cs_set_scan_on_cloud(csr_cs_context_h handle, bool is_on_cloud);

/**
 * @brief Scans a data buffer for malware.
 *
 * @details @a malware result of this method is not available for being judged by
 *          csr_cs_judge_detected_malware() because it's data, not file, so cannot being
 *          removed or ignored.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks Scan data synchronously.
 * @remarks The @a malware will be released when @a handle is released using
 *          csr_cs_context_destroy().
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  data       A scan target data.
 * @param[in]  length     A size of a scan target data.
 * @param[out] malware    A pointer of the detected malware handle. It can be null when
 *                        no malware detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a data or @a malware is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No privilege to call
 * @retval #CSR_ERROR_NOT_SUPPORTED         Device needed to run API is not supported
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_create()
 * @see csr_cs_context_destroy()
 */
int csr_cs_scan_data(csr_cs_context_h handle, const unsigned char *data, size_t length,
					 csr_cs_malware_h *malware);

/**
 * @brief Scans a file specified by file path for malware.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  Scan file synchronously.
 * @remarks  The @a malware will be released when @a handle is released using
 *           csr_cs_context_destroy().
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of scan target file.
 * @param[out] malware    A pointer of the detected malware handle. It can be null when
 *                        no malware detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_path or @a malware is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_NOT_SUPPORTED         Device needed to run API is not supported
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_REMOVE_FAILED         File remove failed when malware exist and
 *                                          user select to remove by popup. @a malware
 *                                          will be allocated on this error unlike others.
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_PERMISSION     Insufficient permission of engine
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_context_create()
 * @see csr_cs_context_destroy()
 */
int csr_cs_scan_file(csr_cs_context_h handle, const char *file_path,
					 csr_cs_malware_h *malware);

/**
 * @brief Sets a callback function for the case that a file scan is completed.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan functions.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for each file or application scanning
 *                      done without any malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
int csr_cs_set_file_scanned_cb(csr_cs_context_h handle, csr_cs_file_scanned_cb callback);

/**
 * @brief Sets a callback function for detection of a malware.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan functions.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for each file or application scanning done
 *                      with malware detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
int csr_cs_set_detected_cb(csr_cs_context_h handle, csr_cs_detected_cb callback);

/**
 * @brief Sets a callback function for scanning completed without an error.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan functions.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for scanning completed successfully.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
int csr_cs_set_completed_cb(csr_cs_context_h handle, csr_cs_completed_cb callback);

/**
 * @brief Sets a callback function for scanning cancelled.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan functions.
 * @remarks Client can cancel asynchronous scanning by csr_cs_cancel_scanning().
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for scanning cancelled.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 * @see csr_cs_cancel_scanning()
 */
int csr_cs_set_cancelled_cb(csr_cs_context_h handle, csr_cs_cancelled_cb callback);

/**
 * @brief Sets a callback function for scanning stopped with an error.
 *
 * @since_tizen 3.0
 *
 * @remarks Callback for asynchronous scan functions.
 *
 * @param[in] handle    CSR CS context handle returned by csr_cs_context_create().
 * @param[in] callback  A callback function for scanning stopped with an error.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a callback is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
int csr_cs_set_error_cb(csr_cs_context_h handle, csr_cs_error_cb callback);

/**
 * @brief Scan files specified by an array of file paths for malware.
 *
 * @details If scanning of the single file is done without detected malware,
 *          csr_cs_file_scanned_cb() called and else if malware detected
 *          csr_cs_detected_cb() called. If scanning is cancelled by
 *          csr_cs_cancel_scanning(), csr_cs_cancelled_cb() called. If scanning is failed
 *          with error, csr_cs_error_cb() is called. If scanning is completed without
 *          error, csr_cs_completed_cb(). Every callbacks are registered by callback
 *          setter methods to @a handle and if callback is not registered, it will just
 *          skipped to be called.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks Asynchronous function.
 * @remarks The caller should set callback functions before call this method.
 * @remarks Detected malware which is provided to the callback will be released when
 *          @a handle is released using csr_cs_context_destroy().
 *
 * @param[in]  handle       CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_paths   An array of scan target files.
 * @param[in]  count        A number of scan target files.
 * @param[in]  user_data    The pointer of a user data. It can be null.
 *                          It is delivered back to the client when a callback function
 *                          is called.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_paths is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_NOT_SUPPORTED         Device needed to run API is not supported
 * @retval #CSR_ERROR_BUSY                  Busy for processing another request
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_ENGINE_PERMISSION     Insufficient permission of engine
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @pre It is required to set callbacks, csr_cs_completed_cb, csr_cs_error_cb(),
 *      csr_cs_cancel_scanning(), csr_cs_detected_cb(), and/or csr_cs_file_scanned_cb().
 *
 * @see csr_cs_set_file_scanned_cb()
 * @see csr_cs_set_detected_cb()
 * @see csr_cs_set_completed_cb()
 * @see csr_cs_set_cancelled_cb()
 * @see csr_cs_set_error_cb()
 * @see csr_cs_cancel_scanning()
 */
int csr_cs_scan_files_async(csr_cs_context_h handle, const char *file_paths[],
							size_t count, void *user_data);

/**
 * @brief Scans a directory specified by directory path for malware.
 *
 * @details If scanning of the single file is done without detected malware,
 *          csr_cs_file_scanned_cb() called and else if malware detected
 *          csr_cs_detected_cb() called. If scanning is cancelled by
 *          csr_cs_cancel_scanning(), csr_cs_cancelled_cb() called. If scanning is failed
 *          with error, csr_cs_error_cb() is called. If scanning is completed without
 *          error, csr_cs_completed_cb(). Every callbacks are registered by callback
 *          setter methods to @a handle and if callback is not registered, it will just
 *          skipped to be called.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks Asynchronous function.
 * @remarks The caller should set callback functions before calls this method.
 * @remarks Detected malware which is provided to the callback will be released when
 *          @a handle is released using csr_cs_context_destroy().
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_path   A path of scan target directory.
 * @param[in]  user_data  The pointer of a user data. It can be null. It is used on
 *                        the callback functions which are registered to @a handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a dir_path is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_NOT_SUPPORTED         Device needed to run API is not supported
 * @retval #CSR_ERROR_BUSY                  Busy for processing another request
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_FILE_SYSTEM           File type is invalid. It should be directory
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_ENGINE_PERMISSION     Insufficient permission of engine
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @pre It is required to set callbacks, csr_cs_completed_cb, csr_cs_error_cb(),
 *      csr_cs_cancel_scanning(), csr_cs_detected_cb(), and/or csr_cs_file_scanned_cb().
 *
 * @see csr_cs_set_file_scanned_cb()
 * @see csr_cs_set_detected_cb()
 * @see csr_cs_set_completed_cb()
 * @see csr_cs_set_cancelled_cb()
 * @see csr_cs_set_error_cb()
 * @see csr_cs_cancel_scanning()
 */
int csr_cs_scan_dir_async(csr_cs_context_h handle, const char *dir_path, void *user_data);

/**
 * @brief Scan directories specified by an array of directory paths for malware.
 *
 * @details If scanning of the single file is done without detected malware,
 *          csr_cs_file_scanned_cb() called and else if malware detected
 *          csr_cs_detected_cb() called. If scanning is cancelled by
 *          csr_cs_cancel_scanning(), csr_cs_cancelled_cb() called. If scanning is failed
 *          with error, csr_cs_error_cb() is called. If scanning is completed without
 *          error, csr_cs_completed_cb(). Every callbacks are registered by callback
 *          setter methods to @a handle and if callback is not registered, it will just
 *          skipped to be called.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks Asynchronous function.
 * @remarks The caller should set callback functions before calls this method.
 * @remarks Detected malware which is provided to the callback will be released when
 *          @a handle is released using csr_cs_context_destroy().
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  dir_paths  An array of scan target directories.
 * @param[in]  count      A number of scan target directories.
 * @param[in]  user_data  The pointer of a user data. It can be null. It is used on
 *                        the callback functions which are registered to @a handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a dir_paths is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     Access denied
 * @retval #CSR_ERROR_NOT_SUPPORTED         Device needed to run API is not supported
 * @retval #CSR_ERROR_BUSY                  Busy for processing another request
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File not found
 * @retval #CSR_ERROR_FILE_SYSTEM           File type is invalid. It should be directory
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_ENGINE_PERMISSION     Insufficient permission of engine
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @pre It is required to set callbacks, csr_cs_completed_cb, csr_cs_error_cb(),
 *      csr_cs_cancel_scanning(), csr_cs_detected_cb(), and/or csr_cs_file_scanned_cb().
 *
 * @see csr_cs_set_file_scanned_cb()
 * @see csr_cs_set_detected_cb()
 * @see csr_cs_set_completed_cb()
 * @see csr_cs_set_cancelled_cb()
 * @see csr_cs_set_error_cb()
 * @see csr_cs_cancel_scanning()
 */
int csr_cs_scan_dirs_async(csr_cs_context_h handle, const char *dir_paths[], size_t count,
						   void *user_data);


/**
 * @brief Cancels a running scanning task, asynchronously.
 *
 * @since_tizen 3.0
 *
 * @remarks Only for asynchronous scan functions.
 *
 * @param[in] handle  CSR CS context handle returned by csr_cs_context_create().
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_NO_TASK               No task to cancel
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_scan_files_async()
 * @see csr_cs_scan_dir_async()
 * @see csr_cs_scan_dirs_async()
 */
int csr_cs_cancel_scanning(csr_cs_context_h handle);


/**
 * @brief Extracts the severity of a detected malware from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  malware   A detected malware handle returned by csr_cs_scan_data(),
 *                       csr_cs_scan_file() or csr_cs_malware_list_get_malware().
 * @param[out] severity  A pointer of the severity of a detected malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a severity is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_get_severity(csr_cs_malware_h malware,
								csr_cs_severity_level_e *severity);

/**
 * @brief Extracts the name of a detected malware from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a name must be released using free().
 *
 * @param[in]  malware  A detected malware handle.
 * @param[out] name     A pointer of the name of a detected malware.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a name is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_get_name(csr_cs_malware_h malware, char **name);

/**
 * @brief Extracts an url that contains detailed information on vendor's web site from
 *        the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a detailed_url must be released using free().
 *
 * @param[in]  malware       A detected malware handle.
 * @param[out] detailed_url  A pointer of an url that contains detailed information on
 *                           vendor's web site. It can be null if a vendor doesn't
 *                           provide this information.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a detailed_url is invalid.
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_get_detailed_url(csr_cs_malware_h malware, char **detailed_url);

/**
 * @brief Extracts the time stamp when a malware is detected from the detected malware
 *        handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  malware    A detected malware handle.
 * @param[out] timestamp  A pointer of the time stamp in milli second when a malware is
 *                        detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a timestamp is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_get_timestamp(csr_cs_malware_h malware, time_t *timestamp);

/**
 * @brief Extracts the file name where a malware is detected from the detected malware
 *        handle.
 *
 * @since_tizen 3.0
 *
 * @remarks The @a file_name must be released using free().
 *
 * @param[in]  malware    A detected malware handle.
 * @param[out] file_name  A pointer of the file name where a malware is detected. The
 *                        file name is null for csr_cs_scan_data().
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a file_name is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_get_file_name(csr_cs_malware_h malware, char **file_name);

/**
 * @brief Extracts a user response of a popup from the detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  malware   A detected malware handle.
 * @param[out] response  A pointer of the user response.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a response is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_get_user_response(csr_cs_malware_h malware,
									 csr_cs_user_response_e *response);

/**
 * @brief Checks if a malware was detected in an application or in a file.
 *
 * @since_tizen 3.0
 *
 * @param[in]  malware  A detected malware handle.
 * @param[out] is_app   A pointer of a flag indicating the position a malware was detected.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a is_app is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_is_app(csr_cs_malware_h malware, bool *is_app);

/**
 * @brief Extracts the package id of an application where a malware is detected from
 *        detected malware handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a pkg_id must be released using free().
 *
 * @param[in]  malware  A detected malware handle.
 * @param[out] pkg_id   A pointer of the package id where a malware is detected.
 *                      It is null when a malware was not detected in an application.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pkg_id is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_get_pkg_id(csr_cs_malware_h malware, char **pkg_id);

/**
 * @brief Judges how a detected malware file is handled.
 *
 * @details  Detected malware will removed by #CSR_CS_ACTION_REMOVE action. File or
 *           application which contains malware will be removed.
 *           Detected malware will ignored by #CSR_CS_ACTION_IGNORE action. File or
 *           application which contains malware will be ignored and will not be treated
 *           as malware until this API called with #CSR_CS_ACTION_UNIGNORE action.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @remarks Detected malware can be removed or ignored.
 *
 * @param[in]  handle   CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  malware  A handle of a detected malware.
 * @param[in]  action   An action to be taken.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a malware or @a action is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     File to take action on not found
 * @retval #CSR_ERROR_FILE_CHANGED          File to take action on changed after detection
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_SYSTEM                System error
 */
int csr_cs_judge_detected_malware(csr_cs_context_h handle, csr_cs_malware_h malware,
								  csr_cs_action_e action);


/**
 * @brief Gets information on a detected malware file specified by file path.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks The @a malware will be released when @a handle is destroyed.
 * @remarks @a file_path will be null if it's result of csr_cs_scan_data().
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of a detected malware file.
 * @param[out] malware    A pointer of the detected malware handle. It can be null when
 *                        no malware file.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_path or @a malware is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     No malware file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_cs_scan_data()
 * @see csr_cs_scan_file()
 * @see csr_cs_detected_cb
 */
int csr_cs_get_detected_malware(csr_cs_context_h handle, const char *file_path,
								csr_cs_malware_h *malware);

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
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_SYSTEM                System error
 */
int csr_cs_get_detected_malwares(csr_cs_context_h handle, const char *dir_paths[],
								 size_t count, csr_cs_malware_list_h *list,
								 size_t *list_count);

/**
 * @brief Gets information on a ignored malware file specified by file path.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.scan
 *
 * @remarks  The @a malware will be released when @a handle is destroyed.
 *
 * @param[in]  handle     CSR CS context handle returned by csr_cs_context_create().
 * @param[in]  file_path  A path of a ignored malware file.
 * @param[out] malware    A pointer of the detected malware handle. It can be null when
 *                        no ignored file.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a file_path or @a malware is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED     No permission to remove
 * @retval #CSR_ERROR_FILE_DO_NOT_EXIST     No ignored file
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_SYSTEM                System error
 */
int csr_cs_get_ignored_malware(csr_cs_context_h handle, const char *file_path,
							   csr_cs_malware_h *malware);

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
 * @retval #CSR_ERROR_DB                    DB transaction error
 * @retval #CSR_ERROR_SYSTEM                System error
 */
int csr_cs_get_ignored_malwares(csr_cs_context_h handle, const char *dir_paths[],
								size_t count, csr_cs_malware_list_h *list,
								size_t *list_count);

/**
 * @brief Extracts the detected malware handle from the detected malware list handle.
 *
 * @since_tizen 3.0
 *
 * @remarks The @a malware will be released when a context is released using
 *          csr_cs_context_destroy().
 *
 * @param[in]  list     A detected malware list handle returned by
 *                      csr_cs_get_detected_malwares() or
 *                      csr_cs_get_ignored_malwares().
 * @param[in]  index    An index of a target detected malware handle to get.
 * @param[out] malware  A pointer of the detected malware handle. It can be null when
 *                      index is invalid.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid list
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a index or @a malware is invalid.
 * @retval #CSR_ERROR_SYSTEM               System error
 */
int csr_cs_malware_list_get_malware(csr_cs_malware_list_h list, size_t index,
									csr_cs_malware_h *malware);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
