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
#ifndef __CSR_WEB_PROTECTION_API_H_
#define __CSR_WEB_PROTECTION_API_H_

#include "csr/web-protection-types.h"
#include "csr/error.h"

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Main function related
//==============================================================================
/**
 * @brief Initializes and returns a CSR Web Protection API handle.
 *
 * @details A Web Protection API handle (or CSR WP handle) is obtained using the
 *          csr_wp_context_create(). The handle is required for subsequent CSR WP API
 *          calls. The csr_wp_context_destroy() releases/closes the handle. Multiple
 *          handles can be obtained using csr_wp_context_create().
 *
 * @param[out] phandle A pointer of CSR WP context handle.
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
 *
 * @see csr_wp_context_destroy()
 */
int csr_wp_context_create(csr_wp_context_h *phandle);

/**
 * @brief Releases all system resources associated with a CSR Web Protection API handle.
 *
 * @param[in] handle CSR WP context handle returned by csr_wp_context_create().
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 *
 * @see csr_wp_context_create()
 */
int csr_wp_context_destroy(csr_wp_context_h handle);

/**
 * @brief Sets a popup option in case for a risky URL.
 *
 * @details If #CSR_WP_ASK_USER is set, a popup will be prompted to a user when a URL
 *          turns out risky(#CSR_WP_RISK_MEDIUM or #CSR_WP_RISK_HIGH). If
 *          #CSR_WP_NOT_ASK_USER is set, no popup will be prompted even when a URL turns
 *          out risky. The default value of this option is #CSR_WP_ASK_USER.
 *
 * @param[in] handle    TWS context handle returned by csr_wp_context_create().
 * @param[in] ask_user  A popup option in case for a risky URL.
 *
 * @return #TWS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #TWS_ERROR_NONE                  Successful
 * @retval #TWS_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #TWS_ERROR_INVALID_PARAMETER     ask_user is invalid
 * @retval #TWS_ERROR_UNKNOWN               Error with unknown reason
 *
 * @see csr_wp_context_create()
 */
int csr_wp_set_ask_user(csr_wp_context_h handle, csr_wp_ask_user_e ask_user);

/**
 * @brief Sets a popup message of a client in case for a risky URL.
 *
 * @details When a popup is prompted to a user, the message set by this method will be
 *          shown. When a client doesn't set his own popup message, the default message
 *          will be shown in the popup.
 *
 * @param[in] handle   CSR WP context handle returned by csr_wp_context_create().
 * @param[in] message  A message in a popup.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a message is too long or empty. Max size
 *                                          is 64 bytes.
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 *
 * @see csr_wp_context_create()
 */
int csr_wp_set_popup_message(csr_wp_context_h handle, const char *message);

/**
 * @brief Main function for caller to check URL reputation against the engine vendor's
 *        database.
 *
 * @details Checks whether accessing the URL is risky or not and returns a result handle
 *          with the Risk level for the URL. The system resources associated with the
 *          result handle will be released when csr_wp_context_destroy() is called.
 *
 * @param[in]  handle   CSR WP context handle returned by csr_wp_context_create().
 * @param[in]  url      URL to check.
 * @param[out] presult  A pointer of the result handle with the Risk level for the URL.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_PERMISSION_DENIED     Permission denied
 * @retval #CSR_ERROR_INVALID_PARAMETER     URL or presult is invalid
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN               Error with unknown reason
 *
 * @see csr_wp_context_create()
 * @see csr_wp_set_ask_user()
 * @see csr_wp_set_popup_message()
 */
int csr_wp_check_url(csr_wp_context_h handle, const char *url,
					 csr_wp_check_result_h *presult);

//==============================================================================
// Result related
//==============================================================================
/**
 * @brief Extracts a risk level of the url from the result handle.
 *
 * @param[in]  result  A result handle returned by csr_wp_check_url().
 * @param[out] plevel  A pointer of the risk level for the given URL.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    plevel is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_wp_check_url()
 */
int csr_wp_result_get_risk_level(csr_wp_check_result_h result,
								 csr_wp_risk_level_e *plevel);

/**
 * @brief Extracts an url of vendor's web site that contains detailed information about the risk
 *        from the result handle.
 *
 * @param[in]  result  A result handle returned by csr_wp_check_url().
 * @param[out] detailed_url  A pointer of an url that contains detailed information about the risk.
 *                           If the risk level is CSR_WP_RISK_MEDIUM or CSR_WP_RISK_HIGH,
 *                           this url should be provided by the engine.
 *                           A caller should not free this string.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    detailed_url is invalid
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_wp_check_url()
 */
int csr_wp_result_get_detailed_url(csr_wp_check_result_h result,
								   const char **detailed_url);

/**
 * @brief Extracts a user reponse of a popup from the result handle.
 *
 * @param[in]  result     A result handle returned by csr_wp_check_url().
 * @param[out] presponse  A pointer of the user response.
 *
 * @return #TWS_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #TWS_ERROR_NONE                 Successful
 * @retval #TWS_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #TWS_ERROR_INVALID_PARAMETER    presponse is invalid
 * @retval #TWS_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_wp_check_url()
 * @see #csr_wp_user_response_e
 */
int csr_wp_result_get_user_response(csr_wp_check_result_h result,
									csr_wp_user_response_e *presponse);


#ifdef __cplusplus
}
#endif

#endif
