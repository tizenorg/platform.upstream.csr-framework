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

#ifndef TCSIMPL_H
#define TCSIMPL_H

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * @addtogroup CAPI_CSRFW_TCS_MODULE
 * @{
 */

/**
 * @brief Instructs the scan functions to perform scanning only.
 * @since_tizen 2.3
 * */
#define TCS_SA_SCANONLY 1

/**
 * @brief Instructs the scan functions to carry out both scanning and repair/removal of detected malware.
 * @since_tizen 2.3
 */
#define TCS_SA_SCANREPAIR 2

/**
 * @brief Informs the caller a malicious code has been detected in the scan target. \n
 * The callback data argument pParam is set to point to a TCSDetected structure.
 * @since_tizen 2.3
 */
#define TCS_CB_DETECTED 1

/**
 * @brief Scan for malicious content in an unknown data type. \n
 * This data type should be used when the other types are not appropriate.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_UNKNOWN 0

/**
 * @brief Scan for malicious content in HTML.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_HTML 1

/**
 * @brief Scan for URL with malicious content. url-string format should conform to the \n
 * Uniform Resource Locators (RFC 1738) specification.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_URL 2

/**
 * @brief Scan for email-address with malicious intent. \n
 * email-string format should conform with the Internet E-mail address format \n
 * (RFC 822) specification.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_EMAIL 3

/**
 * @brief Scan for phone number with malicious intent.\n
 * phone-number string consists of the numeric characters '0' through '9', \n
 * and the '#' and '*' characters.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_PHONE 4

/**
 * @brief Scan for malicious Java code.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_JAVA 5

/**
 * @brief Scan for malicious Java code.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_JAVAS 6

/**
 * @brief Scan text data for malicious content.
 * @since_tizen 2.3
 */
#define TCS_DTYPE_TEXT 7

/**
 * @brief  Malware type.
 * @since_tizen 2.3
 */
#define TCS_VTYPE_MALWARE 1

/**
 * @brief Detected malware is harmful to the user.
 * @since_tizen 2.3
 */
#define TCS_SC_USER 1

/**
 * @brief Detected malware is harmful to the terminal.
 * @since_tizen 2.3
 */
#define TCS_SC_TERMINAL 2

/**
 * @brief Process with a warning. This severity level may be assigned to data previously considered malicious.
 * @since_tizen 2.3
 */
#define TCS_BC_LEVEL0 0

/**
 * @brief Prompt the user before processing. Ask the user if they want the application to process the data.
 * @since_tizen 2.3
 */
#define TCS_BC_LEVEL1 1
/**
 * @brief Do not process the data.
 * @since_tizen 2.3
 */
#define TCS_BC_LEVEL2 2

/**
 * @brief SDo not process the data and prompt user for removal.\n
 * If the content is stored on the terminal, prompt the user for permission before removal.
 * @since_tizen 2.3
 */
#define TCS_BC_LEVEL3 3
/**
 * @brief Do not process the data and automatically remove if stored.
 * @since_tizen 2.3
 */
#define TCS_BC_LEVEL4 4


/*==================================================================================================
                                            MACROS
==================================================================================================*/

/**
 * @brief Helper macro to get error module.
 * @since_tizen 2.3
 */
#define TCS_ERRMODULE(e) (((e) >> 24) & 0xff)

/**
 * @brief Helper macro to get error code.
 * @since_tizen 2.3
 */
#define TCS_ERRCODE(e) ((e) & 0x00ffffff)

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * @brief Dummy data structure to avoid unexpected data type casting.
 * @since_tizen 2.3
 */
struct TCSLibHandle_struct {int iDummy;};

/**
 * @brief TCS library handle type.
 * @since_tizen 2.3
 */
typedef struct TCSLibHandle_struct *TCSLIB_HANDLE;

/**
 * @brief Invalid Content Screening library interface handle.
 * @since_tizen 2.3
 */
#define INVALID_TCSLIB_HANDLE ((TCSLIB_HANDLE) 0)

/**
 * @brief error code type.
 * @since_tizen 2.3
 */
typedef unsigned long TCSErrorCode;

/**
 * @brief Support 64 bits data / file locating
 * @since_tizen 2.3
 */
typedef long long TCSOffset;

/**
 * @brief The calling application specifies scan
 * parameters using the TCSScanParam structure. The information
 * contained in the structure provides the scan functions with:
 * - scan action type (iAction)
 * - the scan data type (iDataType)
 * - data pointer to the scan target (pPrivate)
 * - callback function to retrieve the data size in bytes (pfGetSize)
 * - callback function to resize the scan data (pfSetSize)
 * - callback function used by the scan functions to retrieve a
 *     block of scan data (pfRead)
 * - callback function used to write to the scan data (pfWrite)
 * - callback function for status/progress reporting (pfCallBack)
 * @since_tizen 2.3
 */
typedef struct TCSScanParam_struct
{
    int iAction; /**< The scan-action specifies the type of scanning to be performed on supplied scan data. */

    int iDataType; /**< The calling application specifies the data type/format of the data to be scanned using this variable. */

    int iCompressFlag; /**< 0 - decompression disabled, 1 - decompression enabled. */

    void *pPrivate; /**< Pointer (or handle) to an application object being scanned.
                       The scan functions do not perform direct memory I/O using this data
                       pointer/handle. The data pointer/handle is simply passed back to the caller when
                       performing data read/write using caller specified I/O functions. Also the private
                       data is passed back to the caller using the pfCallback function if it is set. */

    TCSOffset (*pfGetSize)(void *pPrivate); /**< Used by the scan functions
                                               to obtain the scan target data size (in bytes) from the caller.
                                                                                                            
                                               This is a synchronous API.
                                               [in] pPrivate Pointer (or handle) to an application object being scanned.

                                               return - Return Type (int)
                                               The size (in bytes) of the data to be scanned.
                                               */

    int (*pfSetSize)(void *pPrivate, TCSOffset uSize); /**< Called by the scan
                                                          functions to resize the scanned data to a given size (in bytes) during
                                                          repair/clean. The resize function pointer needs to be set if the scan-action
                                                          (iAction) is set to TCS_SA_SCANREPAIR.

                                                          This is a synchronous API.

                                                          @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                          @param[in] uSize The size (in bytes) of the repaired data.

                                                          @return Return Type (int)
                                                          The size (in bytes) of the application data.
                                                          Not equal to the value of uSize indicating this call fails.
                                                          */

    unsigned int (*pfRead)(void *pPrivate, TCSOffset uOffset, void *pBuffer,
                           unsigned int uCount); /**< Used for reading a specified
                                                    amount of application data during scanning/analysis.
                                                                                                                    
                                                    This is a synchronous API.

                                                    @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                    @param[in] uOffset Read from the offset in the application data.
                                                    @param[out] pBuffer The buffer used to store the read data.
                                                    @param[in] uCount The size (in bytes) of the data to be read.

                                                    @return Return Type (int) 
                                                    The size (in bytes) of the read data. 
                                                    Not equal to the value of uCount indicating this call fails.
                                                    */

    unsigned int (*pfWrite)(void *pPrivate, TCSOffset uOffset, void const *pBuffer,
                            unsigned int uCount); /**< The scan functions use the
                                                     given function to write a specified amount of data to the scanned object as a part
                                                     of the repair process. The function pointer needs to be set if the scan action
                                                     (iAction) is set to TCS_SA_SCANREPAIR.
                                                                                                                    
                                                     This is a synchronous API.

                                                     @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                     @param[in] uOffset Write data from the offset in the application data.
                                                     @param[in] pBuffer The buffer hold the data to be written.
                                                     @param[in] uCount The size (in bytes) of the data to be written.

                                                     @return Return Type (int)
                                                     The size (in bytes) of the written data.
                                                     Not equal to the value of uCount indicating this call fails.
                                                     */

    int (*pfCallBack)(void *pPrivate, int iReason, void *pParam); /**< This callback
                                                                     function is set by the caller to be notified to each detected malware while
                                                                     scanning is in process. If specified (not NULL), the scan functions call the
                                                                     specified function with the information (e.g. TCS_CB_DETECTED) for each malware
                                                                     detected in the content/data during scanning.
                                                                                                                                                        
                                                                     This is a synchronous API.
                                                                                                                                                    
                                                                     @param[in] pPrivate Pointer (or handle) to an application object being scanned.
                                                                     @param[in] iReason Reason of this callback.
                                                                     @param[in] pParam The data for specified callback reason respectively.
                                                                                                                                                    
                                                                     @return Return Type (int) 
                                                                     The scanning process continues if the callback function returns 0. If a negative
                                                                     value (e.g. -1) is returned, the scanning process is aborted and control is
                                                                     returned to the caller.
                                                                     */
} TCSScanParam;

/**
 * @brief Detected malicious code/content information structure.
 * @since_tizen 2.3
 */
typedef struct TCSDetected_struct
{
    struct TCSDetected_struct *pNext; /**< Pointer to next malware found, NULL if at the end of list. */

    char const *pszName; /**< Detected malware name. */
    char const *pszVariant; /**< Detected malware's variant name. pszName and
                               pszVariant report detected malicious code/content and variant names. The maximum
                               string length for both strings is 64 characters and each is terminated by a null
                               character ('\\0') - the maximum buffer size for both strings is 65 bytes.

                               pszVariant is set to an empty string ("\0") if the detected malware is not a
                               variant. */

    unsigned int uType; /**< Detected malware type. \see TCS_VTYPE_MALWARE */
    unsigned int uAction; /**< Bit-field specifying severity, class and behavior level.

                             Included in the TCSDetected structure is a bit-field variable containing malware
                             severity flags and client application behavior levels.

                             The scan functions set the TCS_SC_USER flag if the scanned object/data contains
                             malware harmful to the user. TCS_SC_TERMINAL flag is set if the malware is
                             harmful to the terminal itself. Both TCS_SC_USER and TCS_SC_TERMINAL flags are
                             set if the malware is harmful to both the user and the terminal.

                             The application behavior level specifies what to do with the data/object
                             containing the detected malware.

                             When multiple behavior level codes are found in a scanned data/object, the
                             calling application would be expected to act with the highest behavior level.
                             For example, if both TCS_BC_LEVEL0 and TCS_BC_LEVEL3 were reported, the application
                             would need to take on TCS_BC_LEVEL3 action. */

    char const *pszFileName; /**< Path of the infected file. The pszFileName field
                                report, if not NULL, the complete file path of the infected content. If the scan
                                functions have the ability to scan/analyze inside archives, then the path
                                reported in pszFileName would be composed of multiple paths separated by the '|'
                                character. The first path of the sequence is the real file system path of the
                                currently scanned file, for TCSScanFile(), or empty for TCSScanData(). No
                                assumption should be made on the path name separator used for the archive
                                components of the path (the ones following the first). Only the first component,
                                if not empty, is the real file path of the currently scanned content. */
} TCSDetected;

/**
 * @brief Detected malware information is returned to the caller in the TCSScanResult
 * structure provided by the caller. The TCSScanResult structure contains a pointer 
 * to a structure that contains scan result information and a pointer to a function 
 * used to remove the scan result resource. The memory used to hold the scan result 
 * is allocated by the scan functions and freed by calling the function pointed by the 
 * pfFreeResult pointer. The detected malware information includes the malware
 * information which had been reported via the callback (pfCallback) function during
 * scanning.
 *
 * @since_tizen 2.3
 * @code
 * int ScanAppData( ... )
 * {
 *     TCSScanResult scanResult;
 *     .
 *     .
 *     if (TCSScanData(hScanner, &scanParam, &scanResult) == 0)
 *     {
 *         .
 *         .
 *         scanResult.pfFreeResult( &scanResult );
 *     }
 *     .
 *     .
 * }
 * @endcode
 */
typedef struct TCSScanResult_struct
{
    int iNumDetected; /**< Number of malware found. */
    TCSDetected *pDList; /**< Detected malware list. */
    void (*pfFreeResult)(struct TCSScanResult_struct *pResult); /**< Function pointer
                                                                   used to free reported scan result.
                                                                                                                                                
                                                                   This is a synchronous API.
                                                                   @param[in] pResult Pointer to data structure in which detected scan result
                                                                   information is stored.

                                                                   @return None
                                                                   */
} TCSScanResult;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * @brief Initializes and returns a Tizen Content Screening library interface handle.
 * @details A Content Screening library interface handle (or TCS library handle) is \n
 * obtained using the TCSLibraryOpen() function. The library handle is required for \n
 * subsequent TCS API calls. The TCSLibraryClose() function releases/closes the library \n
 * handle. Multiple library handles can be obtained using TCSLibraryOpen(). \n
 *  \n
 * This is a synchronous API.
 *
 * @since_tizen 2.3
 *
 * @return TCS library interface handle on success, otherwise #INVALID_TCSLIB_HANDLE.
 * @see TCSLibraryClose()
 */
TCSLIB_HANDLE TCSLibraryOpen(void);

/**
 * @brief Releases system resources associated with an TCS API library
 * @details handle returned by the TCSLibraryOpen() function. \n
 * \n
 * This is a synchronous API. \n
 *
 * @since_tizen 2.3
 *
 * @param[in] hLib TCS library handle returned by TCSLibraryOpen().
 *
 * @return 0 on success, otherwise -1.
 * @see TCSLibraryOpen()
 */
int TCSLibraryClose(TCSLIB_HANDLE hLib);

/**
 * @brief Returns the last error code associated with the given TCS library handle.
 * @details Once the TCS library handle has been successfully obtained from TCSLibraryOpen(), \n
 * TCSGetLastError() can be used to retrieve the last TCS error that occurred. All TCS \n
 * API functions return zero (= 0) or a valid object pointer if successful, and -1 \n
 * or a null object handle (e.g. INVALID_TCSSCAN_HANDLE) in case of an error. The \n
 * TCSGetLastError() function is used to retrieve error information when a TCS \n
 * function fails. \n
 * \n
 * This is a synchronous API. \n
 *
 * @since_tizen 2.3
 *
 * @param[in] hLib TCS library handle returned by TCSLibraryOpen().
 *
 * @return Last error code set by the TCS library. The TCSErrorCode data type is defined as a \n
 * 32-bit unsigned integer which contains both component and an error code (see \n
 * Figure about TCS Error Code Format). Two macros are available to extract the error \n
 * module and the error code. Call TCS_ERRMODULE(error-code) to get the error module, \n
 * and TCS_ERRCODE(error-code) to get the error code (where error-code is the value \n
 * returned by TCSGetLastError()). \n
 * \n
 * TCS library call sequence with a call to the TCSGetLastError() function: \n
 * @see TCSErrorCodes.h
 */
TCSErrorCode TCSGetLastError(TCSLIB_HANDLE hLib);

/**
 * @brief TCSScanData() is used to scan a data buffer for malware.
 * @details The caller specifies a scanner action, scan target data type, set I/O functions to access \n
 * the data, and an optional callback function for information retrieval. The result \n
 * of the data scanning is returned in a caller provided data structure. \n
 * \n
 * This is a synchronous API. \n
 *
 * @since_tizen 2.3
 * @param[in] hLib instance handle obtained from a call to the TCSLibraryOpen() \n
 * function.
 * @param[in] pParam Pointer to a structure containing data scan parameters.
 * @param[out] pResult Pointer to a structure containing data scan results.
 *
 * @return 0 on success, otherwise -1 and error code is set.
 * @see #TCSScanParam
 * @see #TCSScanResult
 * @see #TCSDetected
 * @see TCSScanFile()
 * @see TCSLibraryClose()
 */
int TCSScanData(TCSLIB_HANDLE hLib, TCSScanParam *pParam, TCSScanResult *pResult);

/**
 * @brief TCSScanFile() is used to scan a file for malware.
 * @details The caller specifies a file name, a scanner action, and scan target data type. The scan result is \n
 * returned in a caller provided data structure. \n
 * \n
 * This is a synchronous API. \n
 *
 * @since_tizen 2.3
 * @param[in] hLib instance handle obtained from a call to the  \n
 * TCSLibraryOpen() function.
 * @param[in] pszFileName Name of file to scan. The file name must include the \n
 * absolute path.
 * @param[in] iDataType Type of data contained in the file. This is used to \n
 * perform data type specific scans on files.
 * @param[in] iAction Type of scanning to perform on file.
 * @param[in] iCompressFlag Flag of decompression
 * @param[out] pResult Pointer to a structure containing data scan results.
 *
 * @return 0 on success, otherwise -1 and error code is set.
 * @see #TCSScanResult
 * @see #TCSDetected
 * @see TCSScanData()
 * @see TCSLibraryClose()
 */
int TCSScanFile(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType, int iAction, int iCompressFlag, TCSScanResult *pResult);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif 

#endif  /* TCSIMPL_H */
