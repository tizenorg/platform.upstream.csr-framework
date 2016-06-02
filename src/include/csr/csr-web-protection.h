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
 * @file        csr-web-protection.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Web Protection CAPI Header
 */
#ifndef __CSR_WEB_PROTECTION_API_H_
#define __CSR_WEB_PROTECTION_API_H_

#include <csr-web-protection-types.h>
#include <csr-error.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup CAPI_CSR_FRAMEWORK_WP_MODULE
 * @{
 */

/**
 * @partner
 * @brief Initializes and returns a CSR Web Protection API handle.
 *
 * @details A Web Protection API handle (or CSR WP handle) is obtained by this method.
 *          The handle is required for subsequent CSR WP API calls.
 *
 * @since_tizen 3.0
 *
 * @remarks The @a handle should be released using csr_wp_context_destroy().
 * @remarks Multiple handles can be obtained.
 *
 * @param[out] handle A pointer of CSR WP context handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a handle is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_wp_context_destroy()
 */
int csr_wp_context_create(csr_wp_context_h *handle);

/**
 * @partner
 * @brief Releases all system resources associated with a Web Protection API handle.
 *
 * @since_tizen 3.0
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
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_wp_context_create()
 */
int csr_wp_context_destroy(csr_wp_context_h handle);

/**
 * @partner
 * @brief Sets a popup option for risky URL checked.
 *
 * @details If #CSR_WP_ASK_USER_YES is set, a popup will be prompted to a user when a URL
 *          turns out risky. If #CSR_WP_ASK_USER_NO is set, no popup will be prompted
 *          even when a URL turns out risky.
 *
 * @since_tizen 3.0
 *
 * @remarks This option is disabled(#CSR_WP_ASK_USER_NO) as a default.
 *
 * @param[in] handle    CSR WP context handle returned by csr_wp_context_create().
 * @param[in] ask_user  A popup option in case for a risky URL.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a ask_user is invalid
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_wp_context_create()
 */
int csr_wp_set_ask_user(csr_wp_context_h handle, csr_wp_ask_user_e ask_user);

/**
 * @partner
 * @brief Sets a popup message of a client in case for a risky URL.
 *
 * @details Default message is "Risky URL which may harm your device is detected".
 *
 * @since_tizen 3.0
 *
 * @remarks Meaningful only when ask user option is set by csr_wp_set_ask_user().
 * @remarks The message will be printed on popup for user.
 * @remarks Default popup message will be used if it isn't set.
 *
 * @param[in] handle   CSR WP context handle returned by csr_wp_context_create().
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
 * @see csr_wp_context_create()
 */
int csr_wp_set_popup_message(csr_wp_context_h handle, const char *message);

/**
 * @partner
 * @brief Checks URL reputation against the engine vendor's database.
 *
 * @details Checks whether accessing the URL is risky or not and returns a result handle
 *          with the risk level for the URL.
 *
 * @since_tizen 3.0
 * @privlevel partner
 * @privilege %http://tizen.org/privilege/antivirus.webprotect
 *
 * @remarks The @a result will be released when @a handle is released using
 *          csr_wp_context_destroy().
 *
 * @param[in]  handle   CSR WP context handle returned by csr_wp_context_create().
 * @param[in]  url      URL to check.
 * @param[out] result   A pointer of the result handle with the Risk level for the URL.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid handle
 * @retval #CSR_ERROR_OUT_OF_MEMORY         Not enough memory
 * @retval #CSR_ERROR_PERMISSION_DENIED     Permission denied
 * @retval #CSR_ERROR_NOT_SUPPORTED         Device needed to run API is not supported
 * @retval #CSR_ERROR_INVALID_PARAMETER     @a url or @a result is invalid
 * @retval #CSR_ERROR_SOCKET                Socket error between client and server
 * @retval #CSR_ERROR_SERVER                Server has been failed for some reason
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST      No engine exists
 * @retval #CSR_ERROR_ENGINE_DISABLED       Engine is in disabled state
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED  Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL       Engine Internal error
 * @retval #CSR_ERROR_SYSTEM                System error
 *
 * @see csr_wp_context_create()
 * @see csr_wp_set_ask_user()
 * @see csr_wp_set_popup_message()
 */
int csr_wp_check_url(csr_wp_context_h handle, const char *url,
					 csr_wp_check_result_h *result);


/**
 * @partner
 * @brief Extracts a risk level of the url from the result handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  result  A result handle returned by csr_wp_check_url().
 * @param[out] level   A pointer of the risk level for the given URL.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a level is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 *
 * @see csr_wp_check_url()
 */
int csr_wp_result_get_risk_level(csr_wp_check_result_h result, csr_wp_risk_level_e *level);

/**
 * @partner
 * @brief Extracts an url of vendor's web site that contains detailed information about
 *        the risk from the result handle.
 *
 * @since_tizen 3.0
 *
 * @remarks  The @a detailed_url must be released using free().
 *
 * @param[in]  result        A result handle returned by csr_wp_check_url().
 * @param[out] detailed_url  A pointer of an url that contains detailed information about
 *                           the risk.
 *                           If the risk level is #CSR_WP_RISK_MEDIUM or #CSR_WP_RISK_HIGH,
 *                           this url should be provided by the engine.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a detailed_url is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 *
 * @see csr_wp_check_url()
 */
int csr_wp_result_get_detailed_url(csr_wp_check_result_h result, char **detailed_url);

/**
 * @partner
 * @brief Extracts a user response of a popup from the result handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  result     A result handle returned by csr_wp_check_url().
 * @param[out] response   A pointer of the user response.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid result handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a response is invalid
 * @retval #CSR_ERROR_SYSTEM               System error
 *
 * @see csr_wp_check_url()
 * @see #csr_wp_user_response_e
 */
int csr_wp_result_get_user_response(csr_wp_check_result_h result,
									csr_wp_user_response_e *response);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
