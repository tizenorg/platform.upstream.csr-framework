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
 * @brief       Interface for content-screening engine
 */
#ifndef __CSRE_CONTENT_SCREENING_API_H_
#define __CSRE_CONTENT_SCREENING_API_H_

#include "csre/content-screening-types.h"
#include "csre/error.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Main function related
//==============================================================================
/**
 * @brief Initializes and returns a Tizen Malware Screening engine API handle.
 *
 * @details A Malware Screening engine interface handle (or CSR CS engine handle) is
 *          obtained using the csre_cs_context_create() function. The engine handle is
 *          required for subsequent CSR CS engine API calls. The csre_cs_context_destroy()
 *          function releases/closes the engine handle. Multiple handles can be obtained
 *          using csre_cs_context_create().
 *
 * @param[in]  engine_root_dir  A root directory where an engine exists. It is a absolute
 *                              path and it doesn't end with '/'.
 * @param[out] phandle          A pointer of CSR CS Engine context handle.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE               Successful
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER  phandle is invalid
 * @retval #CSRE_CS_ERROR_OUT_OF_MEMORY      Not enough memory
 * @retval #CSRE_CS_ERROR_UNKNOWN            Error with unknown reason
 * @retval -0x0100~-0xFF00                   Engine defined error
 *
 * @see csre_cs_context_destroy()
 */
int csre_cs_context_create(const char *engine_root_dir, csre_cs_context_h* phandle);

/**
 * @brief Releases all system resources associated with a Tizen Malware Screening engine
 *        API handle.
 *
 * @details The handle is one returned by the csre_cs_context_create() function.
 *
 * @param[in] handle CSR CS Engine context handle returned by csre_cs_context_create().
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE             Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE   Invalid handle
 * @retval #CSRE_CS_ERROR_UNKNOWN          Error with unknown reason
 * @retval -0x0100~-0xFF00                 Engine defined error
 *
 * @see csre_cs_context_create()
 */
int csre_cs_context_destroy(csre_cs_context_h handle);

/**
 * @brief Sets a database which is used in scanning.
 *
 * @details If a database is not set or an engine does not support "scanning on cloud",
 *          the scanning will be done in a local device.
 *
 * @param[in] handle    CSR CS context handle returned by csre_cs_context_create().
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                  Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSRE_CS_ERROR_UNKNOWN               Error with unknown reason
 */
int csre_cs_set_scan_on_cloud(csre_cs_context_h handle);

/**
 * @brief Main function for caller to scan a data buffer for malware.
 *
 * @param[in]  handle     CSR CS Engine context handle returned by
 *                        csre_cs_context_create().
 * @param[in]  data       A scan target data.
 * @param[in]  length     A size of a scan target data.
 * @param[out] pdetected  A pointer of the detected malware handle. It can be null when
 *                        no malware detected.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid handle
 * @retval #CSRE_CS_ERROR_OUT_OF_MEMORY        Not enough memory
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    data or presult is invalid
 * @retval #CSRE_CS_ERROR_ENGINE_NOT_ACTIVATED Engine is not activated
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                     Engine defined error
 *
 * @see csre_cs_context_create()
 * @see csre_cs_scan_file()
 * @see csre_cs_scan_file_by_fd()
 */
int csre_cs_scan_data(csre_cs_context_h handle,
                    const unsigned char *data,
                    size_t length,
                    csre_cs_detected_h *pdetected);

/**
 * @brief Main function for caller to scan a file specified by file path for malware.
 *
 * @param[in]  handle     CSR CS Engine context handle returned by
 *                        csre_cs_context_create().
 * @param[in]  file_path  A path of scan target file.
 * @param[out] pdetected  A pointer of the detected malware handle. It can be null when
 *                        no malware detected.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid handle
 * @retval #CSRE_CS_ERROR_OUT_OF_MEMORY        Not enough memory
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    file_path or presult is invalid
 * @retval #CSRE_CS_ERROR_ENGINE_NOT_ACTIVATED Engine is not activated
 * @retval #CSRE_CS_ERROR_PERMISSION_DENIED    Access denied
 * @retval #CSRE_CS_ERROR_FILE_NOT_FOUND       File not found
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                     Engine defined error
 *
 * @see csre_cs_context_create()
 * @see csre_cs_scan_data()
 * @see csre_cs_scan_file_by_fd()
 */
int csre_cs_scan_file(csre_cs_context_h handle,
                  const char *file_path,
                  csre_cs_detected_h *pdetected);

/**
 * @brief Main function for caller to scan a file specified by file descriptor for malware.
 *
 * @details  The file is opened in readonly by another processe and its file descriptor
 *           is delivered to the engine. This is useful in case of an insufficient
 *           permission of a server process with scanning function. The client with
 *           permission to a file opens the file and deliver its descriptor to a server
 *           with an insufficient permission.
 *
 * @param[in]  handle           CSR CS Engine context handle returned by
 *                              csre_cs_context_create().
 * @param[in]  file_descriptor  A file descriptor of scan target file.
 * @param[out] pdetected        A pointer of the detected malware handle. It can be null
 *                              when no malware detected.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid handle
 * @retval #CSRE_CS_ERROR_OUT_OF_MEMORY        Not enough memory
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    presult is invalid
 * @retval #CSRE_CS_ERROR_ENGINE_NOT_ACTIVATED Engine is not activated
 * @retval #CSRE_CS_ERROR_PERMISSION_DENIED    Access denied
 * @retval #CSRE_CS_ERROR_FILE_NOT_FOUND       File not found
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                     Engine defined error
 *
 * @see csre_cs_context_create()
 * @see csre_cs_scan_data()
 * @see csre_cs_scan_file()
 */
int csre_cs_scan_file_by_fd(csre_cs_context_h handle,
                  int file_descriptor,
                  csre_cs_detected_h *pdetected);

//==============================================================================
// Result related
//==============================================================================

/**
 * @brief Extracts the severity of a detected malware from the detected malware handle.
 *
 * @param[in]  detected    A detected malware handle.
 * @param[out] pseverity  A pointer of the severity of a detected malware.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    pseverity is invalid.
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                     Engine defined error
 *
 * @see csre_cs_result_get_detected_by_idx()
 * @see csre_cs_result_get_detected_most_severe()
 */
int csre_cs_detected_get_severity(csre_cs_detected_h detected, csre_cs_severity_level_e* pseverity);


/**
 * @brief Extracts the threat type of a detected malware from the detected malware handle.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] pthreat_type  A pointer of the threat type of a detected malware.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    pharmful_type is invalid.
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 */
int csre_cs_detected_get_threat_type(csre_cs_detected_h detected, csre_cs_threat_type_e* pthreat_type);

/**
 * @brief Extracts the name of a detected malware from the detected malware handle.
 *
 * @param[in]  detected  A detected malware handle.
 * @param[out] name      A pointer of the name of a detected malware. A caller should not
 *                       free it.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    malware_name is invalid.
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                     Engine defined error
 *
 * @see csre_cs_result_get_detected_by_idx()
 * @see csre_cs_result_get_detected_most_severe()
 */
int csre_cs_detected_get_malware_name(csre_cs_detected_h detected, const char** name);

/**
 * @brief Extracts an url that contains detailed information on vendor's web site from the
 *        detected malware handle.
 *
 * @param[in]  detected      A detected malware handle.
 * @param[out] detailed_url  A pointer of an url that contains detailed information on
 *                           vendor's web site. It can be null if a vendor doesn't provide
 *                           this information. A caller should not free this string.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    malware_name is invalid.
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                     Engine defined error
 */
int csre_cs_detected_get_detailed_url(csre_cs_detected_h detected, const char** detailed_url);

/**
 * @brief Extracts the time stamp when a malware is detected from the detected malware
 *        handle.
 *
 * @param[in]  detected   A detected malware handle.
 * @param[out] timestamp  A pointer of the time stamp in millisecond when a malware is
 *                        detected. A caller should not free it.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_CS_ERROR_INVALID_HANDLE       Invalid detected malware handle
 * @retval #CSRE_CS_ERROR_INVALID_PARAMETER    timestamp is invalid
 * @retval #CSRE_CS_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                     Engine defined error
 */
int csre_cs_detected_get_timestamp(csre_cs_detected_h detected, long* timestamp);

/**
 * @brief Get the error string for a given engine-defined error code.
 *
 * @details The error strings are managed by the engine, therefore a caller should not
 *          free it.
 *
 * @param[in]  error_code  an error code including an engine-defined error.
 * @param[out] string      A pointer of the error string.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_PARAMETER    error_code or error_string is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 */
int csre_cs_get_error_string(int error_code, const char** string);

#ifdef __cplusplus
}
#endif

#endif
