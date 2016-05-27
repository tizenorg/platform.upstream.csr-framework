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
 * @file        csr-engine-manager.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Engine management CAPI header
 */
#ifndef __CSR_ENGINE_MANAGER_H_
#define __CSR_ENGINE_MANAGER_H_

#include <time.h>
#include <csr-error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_CSR_FRAMEWORK_ADMIN_MODULE
 * @{
 */

/**
 * @brief Engine handle
 * @since_tizen 3.0
 */
typedef struct __csr_engine_h *csr_engine_h;

/**
 * @brief Engine identifier
 * @since_tizen 3.0
 */
typedef enum {
	CSR_ENGINE_CS = 0x01,  /**< Content screening engine id */
	CSR_ENGINE_WP = 0x02   /**< Web protection engine id */
} csr_engine_id_e;

/**
 * @brief State of engine activation
 * @since_tizen 3.0
 */
typedef enum {
	CSR_NOT_ACTIVATED = 0x01,  /**< Engine is not activated */
	CSR_ACTIVATED     = 0x02   /**< Engine is activated */
} csr_activated_e;

/**
 * @brief State of engine
 * @since_tizen 3.0
 */
typedef enum {
	CSR_ENABLE        = 0x01,  /**< Enable engine */
	CSR_DISABLE       = 0x02   /**< Disable engine */
} csr_state_e;

/**
 * @brief Gets the handle of a current engine information.
 *
 * @since_tizen 3.0
 *
 * @param[in]  id       Engine identifier to get handle.
 * @param[out] pengine  A pointer of the engine information handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pengine is invalid
 * @retval #CSR_ERROR_ENGINE_NOT_EXIST     No engine exists
 * @retval #CSR_ERROR_ENGINE_NOT_ACTIVATED Engine is not activated
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 */
int csr_get_current_engine(csr_engine_id_e id, csr_engine_h *pengine);

/**
 * @brief Extracts an vendor name from the engine information handle.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @remarks  The @a pvendor must be released using free().
 *
 * @param[in]  engine   The engine information handle.
 * @param[out] pvendor  A pointer of the engine's vendor name.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pvendor is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_get_vendor(csr_engine_h engine, char **pvendor);

/**
 * @brief Extracts an engine name from the engine information handle.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @remarks  The @a pname must be released using free().
 *
 * @param[in]  engine  The engine information handle.
 * @param[out] pname   A pointer of the engine's name.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pname is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_get_name(csr_engine_h engine, char **pname);

/**
 * @brief Extracts an engine version from the engine information handle.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @remarks  The @a pname must be released using free().
 *
 * @param[in]  engine    An engine information handle.
 * @param[out] pversion  A pointer of the engine's version.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pversion is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_get_version(csr_engine_h engine, char **pversion);

/**
 * @brief Extracts an engine's data version from the engine information handle.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @remarks  The @a pversion must be released using free().
 *
 * @param[in]  engine    The engine information handle.
 * @param[out] pversion  A pointer of the data version. It can be null.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pversion is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_get_data_version(csr_engine_h engine, char **pversion);

/**
 * @brief Extracts the latest update time of an engine from the engine information handle.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @param[in]  engine   The engine information handle.
 * @param[out] ptime    A pointer of lasted update time.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a ptime is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_get_latest_update_time(csr_engine_h engine, time_t *ptime);

/**
 * @brief Extracts the state of engine activation from the engine information handle.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @param[in]  engine      The engine information handle.
 * @param[out] pactivated  A pointer of the engine activation state.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pactivated is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_get_activated(csr_engine_h engine, csr_activated_e *pactivated);

/**
 * @brief Enable or disable an engine.
 *        If an engine is disabled, all major operations fail with CSR_ERROR_ENGINE_DISABLED error.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @param[in]  engine      The engine information handle.
 * @param[in]  state       The engine state.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a state is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_set_state(csr_engine_h engine, csr_state_e state);

/**
 * @brief Gets the engine state.
 *
 * @since_tizen 3.0
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/antivirus.admin
 *
 * @param[in]  engine      The engine information handle.
 * @param[out] pstate      A pointer of the engine state.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                 Successful
 * @retval #CSR_ERROR_INVALID_HANDLE       Invalid engine information handle
 * @retval #CSR_ERROR_INVALID_PARAMETER    @a pstate is invalid
 * @retval #CSR_ERROR_PERMISSION_DENIED    Permission denied
 * @retval #CSR_ERROR_ENGINE_INTERNAL      Engine Internal error
 * @retval #CSR_ERROR_UNKNOWN              Error with unknown reason
 *
 * @see csr_get_current_engine()
 */
int csr_engine_get_state(csr_engine_h engine, csr_state_e *pstate);

/**
 * @brief Releases all system resources associated with a engine information handle.
 *
 * @since_tizen 3.0
 *
 * @param[in]  engine      The engine information handle.
 *
 * @return #CSR_ERROR_NONE on success, otherwise a negative error value
 *
 * @retval #CSR_ERROR_NONE                  Successful
 * @retval #CSR_ERROR_INVALID_HANDLE        Invalid engine information handle
 */
int csr_engine_destroy(csr_engine_h engine);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif