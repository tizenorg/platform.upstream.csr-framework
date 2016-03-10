/*
    Copyright (c) 2013, McAfee, Inc.
    
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    
    Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.
    
    Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.
    
    Neither the name of McAfee, Inc. nor the names of its contributors may be used
    to endorse or promote products derived from this software without specific prior
    written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef TCSERRORCODES_H
#define TCSERRORCODES_H

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * @file TCSErrorCodes.h
 * @brief TCS Error Code Header File
 *  
 * This file provides the TCS error code definition.
 */

/**
 * @addtogroup CAPI_CSRFW_TCS_MODULE
 * @{
 */


/**
 * @brief A generic error code.
 * @since_tizen 2.3
 */
#define TCS_ERROR_MODULE_GENERIC 1

/**
 * @brief Operations cancelled.
 * @since_tizen 2.3
 */
#define TCS_ERROR_CANCELLED 1

/**
 * @brief Unable to access data.
 * @since_tizen 2.3
 */
#define TCS_ERROR_DATA_ACCESS 2

/**
 * @brief Invalid parameter.
 * @since_tizen 2.3
 */
#define TCS_ERROR_INVALID_PARAM 3

/**
 * @brief Insufficient resource.
 * @since_tizen 2.3
 */
#define TCS_ERROR_INSUFFICIENT_RES 4

/**
 * @brief Unexpected internal error.
 * @since_tizen 2.3
 */
#define TCS_ERROR_INTERNAL 5

/**
 * @brief Invalid handle.
 * @since_tizen 2.3
 */
#define TCS_ERROR_INVALID_HANDLE 6

/**
 * @brief Specified functionality is not implemented in the TCS plug-in. (e.g. repair)
 * @since_tizen 2.3
 */
#define TCS_ERROR_NOT_IMPLEMENTED 7

/**
 * @}
 */

#ifdef __cplusplus
}
#endif 

#endif /* TCSERRORCODES_H */
