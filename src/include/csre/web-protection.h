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
 * @file        web-protection.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#ifndef __CSRE_WEB_PROTECTION_API_H_
#define __CSRE_WEB_PROTECTION_API_H_

#include "csre/web-protection-types.h"
#include "csre/error.h"

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Main function related
//==============================================================================
/**
 * @brief Initializes and returns a Tizen Web Screening engine API handle.
 *
 * @details A Web Screening engine interface handle (or CSR WP engine handle) is obtained
 *          using the csre_wp_context_create() function. The engine handle is required
 *          for subsequent CSR WP engine API calls. The csre_wp_context_destroy() function
 *          releases/closes the engine handle. Multiple handles can be obtained using
 *          csre_wp_context_create().
 *
 * @param[in]  engine_root_dir  A root directory where an engine exists. It is a absolute
 *                              path and it doesn't end with '/'.
 * @param[out] phandle          A pointer of CSR WP Engine context handle.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE               Successful
 * @retval #CSRE_ERROR_INVALID_PARAMETER  phandle is invalid
 * @retval #CSRE_ERROR_OUT_OF_MEMORY      Not enough memory
 * @retval #CSRE_ERROR_UNKNOWN            Error with unknown reason
 * @retval -0x0100~-0xFF00                Engine defined error
 *
 * @see csre_wp_context_destroy()
 */
int csre_wp_context_create(const char *engine_root_dir, csre_wp_context_h* phandle);

/**
 * @brief Releases all system resources associated with a CSR WP engine API handle.
 *
 * @details The handle is one returned by the csre_wp_context_create() function.
 *
 * @param[in] handle CSR WP Engine context handle returned by csre_wp_context_create().
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE             Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE   Invalid handle
 * @retval #CSRE_ERROR_UNKNOWN          Error with unknown reason
 * @retval -0x0100~-0xFF00              Engine defined error
 *
 * @see csre_wp_context_create()
 */
int csre_wp_context_destroy(csre_wp_context_h handle);

/**
 * @brief Main function for caller to check URL reputation against the engine vendor's
 *        database.
 *
 * @details  Checks whether accessing the URL is risky or not and returns a result handle
 *           with the Risk level for the URL. The system resources associated with the
 *           result handle will be released when csre_wp_context_destroy is called.
 *
 * @param[in]  handle   CSR WP Engine context handle returned by csre_wp_context_create().
 * @param[in]  url      URL to check.
 * @param[out] presult  A pointer of the result handle with the Risk level for the URL.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid handle
 * @retval #CSRE_ERROR_OUT_OF_MEMORY        Not enough memory
 * @retval #CSRE_ERROR_INVALID_PARAMETER    URL is invalid
 * @retval #CSRE_ERROR_ENGINE_NOT_ACTIVATED Engine is not activated
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 *
 * @see csre_wp_context_create()
 */
int csre_wp_check_url(csre_wp_context_h handle, const char *url, csre_wp_check_result_h *presult);

/**
 * @brief Extracts a risk level of the url from the result handle.
 *
 * @param[in]  result  A result handle returned by csre_wp_check_url().
 * @param[out] plevel  A pointer of the risk level for the given URL.
 *
 * @return #CSRE_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    plevel is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 *
 * @see csre_wp_check_url()
 */
int csre_wp_result_get_risk_level(csre_wp_check_result_h result, csre_wp_risk_level_e* plevel);

/**
 * @brief Extracts an url of vendor's web site that contains detailed information about the risk
 *        from the result handle.
 *
 * @param[in]  result  A result handle returned by csre_wp_check_url().
 * @param[out] detailed_url  A pointer of an url that contains detailed information about the risk.
 *                           If the risk level is CSRE_WP_RISK_MEDIUM or CSRE_WP_RISK_HIGH,
 *                           this url should be provided by the engine.
 *                           A caller should not free this string.
 *
 * @return #CSRE_CS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSRE_CS_ERROR_NONE                 Successful
 * @retval #CSRE_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSRE_ERROR_INVALID_PARAMETER    detailed_url is invalid
 * @retval #CSRE_ERROR_UNKNOWN              Error with unknown reason
 * @retval -0x0100~-0xFF00                  Engine defined error
 */
int csre_wp_result_get_detailed_url(csre_wp_check_result_h result, const char** detailed_url);

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
int csre_wp_get_error_string(int error_code, const char** string);

#ifdef __cplusplus
}
#endif

#endif
