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

#ifndef TWPIMPL_H
#define TWPIMPL_H


#ifdef __cplusplus 
extern "C" {
#endif

/**
 * @addtogroup CAPI_CSRFW_TWP_MODULE
 * @{
 */


/**
 * @brief Size unit
 * @since_tizen 2.3
 */
typedef long unsigned int TWPMallocSizeT;

/**
 * @brief Dummy data structure to avoid unexpected data type casting.
 * @since_tizen 2.3
 */
struct TWPLibHandle_struct {int iDummy;};

/**
 * @brief TWP library handle type.
 * @since_tizen 2.3
 */
typedef struct TWPLibHandle_struct *TWPLIB_HANDLE;

/**
 * @brief SDK version.
 * @since_tizen 2.3
 */
#define TWPAPI_VERSION 1

/**
 * @brief Configure version.
 * @since_tizen 2.3
 */
#define TWPCONFIG_VERSION 1

/**
 * @brief Request version.
 * @since_tizen 2.3
 */
#define TWPREQUEST_VERSION 1

/**
 * @brief Invalid web protection library interface handle.
 * @since_tizen 2.3
 */
#define INVALID_TWPLIB_HANDLE ((TWPLIB_HANDLE) 0)
	
/**
 * @brief Result code used by TWP_RESULT
 * @since_tizen 2.3
 */
typedef enum
{
	TWP_SUCCESS = 0, /**< Success */
	TWP_ERROR = 1, /**< Generic error */
	TWP_NOMEM = 2,	/**< Function failed to allocate required memory */
	TWP_INVALID_HANDLE = 3, /**< The given handle is invalid */
	TWP_INVALID_PARAMETER = 4, /**< The given parameter other than handle is invalid */
	TWP_INVALID_VERSION = 5, /**< The version of the structure is incorrect */
	TWP_INVALID_RESPONSE = 6, /**< The server response is invalid */
	TWP_NO_DATA = 7, /**< Requested data is not available.  This error code is
                         usually returned when necessity data is not passed */
	TWP_NOT_IMPLEMENTED = 500 /**< Tizen Web Protection APIs are not implemented on system
                               Or the APIs are called while CSR framework fails to load */
} TWP_RESULT;

/**
 * @brief Web site category definitions. \n
 * See "Categories definitions" section in "Tizen Web Protection API Specification" \n
 * docoument for more detail.
 * @since_tizen 2.3
 */
typedef enum 
{
    TWP_Artcultureheritage, /**< Artculture and heritage */
    TWP_Alcohol, /**< Alcohol */
    TWP_Anonymizers, /**< Anonymizers */
    TWP_Anonymizingutilities, /**< Anonymizingutilities */
    TWP_Business, /**< Business */
    TWP_Chat, /**< Chat */
    TWP_Publicinformation, /**< Publicinformation */
    TWP_Potentialcriminalactivities, /**< Potentialcriminalactivities */
    TWP_Drugs, /**< Drugs */
    TWP_Educationreference, /**< Educationreference */
    TWP_Entertainment, /**< Entertainment */
    TWP_Extreme, /**< Extreme */
    TWP_Financebanking, /**< Finance banking */
    TWP_Gambling, /**< Gambling */
    TWP_Games, /**< Games */
    TWP_Governmentmilitary, /**< Governmentmilitary */
    TWP_Potentialhackingcomputercrime, /**< Potential hacking and computer crime */
    TWP_Health, /**< Health */
    TWP_Humorcomics, /**< Humorcomics */
    TWP_Discrimination, /**< Discrimination */
    TWP_Instantmessaging, /**< Instantmessaging */
    TWP_Stocktrading, /**< Stocktrading */
    TWP_Internetradiotv, /**< Internet radio and tv */
    TWP_Jobsearch, /**< Jobsearch */
    TWP_Informationsecurity, /**< Information security */
    TWP_E_RESERVED_1, /**< Reserved */
    TWP_Mobilephone, /**< Mobilephone */
    TWP_Mediadownloads, /**< Media downloads */
    TWP_Malicioussites, /**< Maliciouss ites */
    TWP_E_RESERVED_2, /**< REserved */
    TWP_Nudity, /**< Nudity */
    TWP_Nonprofitadvocacyngo, /**< Non profit advocacy ngo */
    TWP_Generalnews, /**< General news */
    TWP_Onlineshopping, /**< Online shopping */
    TWP_Provocativeattire, /**< Provocative attire */
    TWP_P2pfilesharing, /**< P2p file sharing */
    TWP_Politicsopinion, /**< Politics opinion */
    TWP_Personalpages, /**< Personal pages */
    TWP_Portalsites, /**< Portal sites */
    TWP_Remoteaccess, /**< Remote access */
    TWP_Religionideology, /**< Religionideology */
    TWP_Resourcesharing, /**< Resource sharing */
    TWP_Searchengines, /**< Search engines */
    TWP_Sports, /**< Sports */
    TWP_Streamingmedia, /**< Streaming media */
    TWP_Sharewarefreeware, /**< Shareware and freeware */
    TWP_Pornography, /**< Pornography */
    TWP_Spywareadwarekeyloggers, /**< Spyware and adware and keyloggers */
    TWP_Tobacco, /**< Tobacco */
    TWP_Travel, /**< Travel */
    TWP_Violence, /**< Violence */
    TWP_Webads, /**< Webads */
    TWP_Weapons, /**< Weapons */
    TWP_Webmail, /**< Webmail */
    TWP_Webphone, /**< Webphone */
    TWP_Auctionsclassifieds, /**< Auctions and classifieds */
    TWP_Forumbulletinboards, /**< Forum and bulletinboards */
    TWP_Profanity, /**< Profanity */
    TWP_Schoolcheatinginformation, /**< Schoolcheating information */
    TWP_Sexualmaterials, /**< Sexual materials */
    TWP_Gruesomecontent, /**< Gruesomecontent */
    TWP_Visualsearchengine, /**< Visual search engine */
    TWP_Technicalbusinessforums, /**< Technical business forums */
    TWP_Gamblingrelated, /**< Gambling related */
    TWP_Messaging, /**< Messaging */
    TWP_Gamecartoonviolence, /**< Game and cartoon violence */
    TWP_Phishing, /**< Phishing */
    TWP_Personalnetworkstorage, /**< Personal network storage */
    TWP_Spamurls, /**< Spamurls */
    TWP_Interactivewebapplications, /**< Interactive webapplications */
    TWP_Fashionbeauty, /**< Fashionbeauty */
    TWP_Softwarehardware, /**< Softwarehardware */
    TWP_Potentialillegalsoftware, /**< Potentialillegalsoftware */
    TWP_Contentserver, /**< Contentserver */
    TWP_Internetservices, /**< Internetservices */
    TWP_Mediasharing, /**< Mediasharing */
    TWP_Incidentalnudity, /**< Incidentalnudity */
    TWP_Marketingmerchandising, /**< Marketing merchandising */
    TWP_Parkeddomain, /**< Parkeddomain */
    TWP_Pharmacy, /**< Pharmacy */
    TWP_Restaurants, /**< Restaurants */
    TWP_Realestate, /**< Realestate */
    TWP_Recreationhobbies, /**< Recreation hobbies */
    TWP_Blogswiki, /**< Blogs wiki */
    TWP_Digitalpostcards, /**< Digital postcards */
    TWP_Historicalrevisionism, /**< Historical revisionism */
    TWP_Technicalinformation, /**< Technical information */
    TWP_Datingpersonals, /**< Dating personals */
    TWP_Motorvehicles, /**< Motor vehicles */
    TWP_Professionalnetworking, /**< Professional networking */
    TWP_Socialnetworking, /**< Socialnetworking */
    TWP_Texttranslators, /**< Text translators */
    TWP_Webmeetings, /**< Web meetings */
    TWP_Forkids, /**< For kids */
    TWP_E_RESERVED_3, /**< Reserved */
    TWP_Moderated, /**< Moderated */
    TWP_Textspokenonly, /**< Text spoken only */
    TWP_Controversialopinions, /**< Controversial opinions */
    TWP_Residentialipaddresses, /**< Residential ip addresses */
    TWP_Browserexploits, /**< Browser exploits */
    TWP_Consumerprotection, /**< Consumer protection */
    TWP_Illegaluk, /**< Illegal site in uk */
    TWP_Majorglobalreligions, /**< Major global religions */
    TWP_Maliciousdownloads, /**< Malicious downloads */
    TWP_Potentiallyunwantedprograms, /**< Potentially unwanted programs */

    TWP_LastCategoryPlaceholder = 128, /**< LastCategory Placeholder */
    TWP_OverallPhishing = 129, /**< Overall Phishing site*/
    TWP_OverallRiskHigh = 130, /**< Overall RiskHigh */
    TWP_OverallRiskMedium = 131, /**< Overall RiskMedium */
    TWP_OverallRiskMinimal = 132, /**< Overall RiskMinimal */
    TWP_OverallRiskUnverified = 137, /**< Overall risk unverified */
    TWP_LastAttributePlaceholder = 160, /**< LastAttribute Placeholder */
} TWPCategories;

/**
 * @brief Risk level
 * @since_tizen 2.3
 */
typedef enum
{
	TWP_Minimal, /**< Risk minimal */
	TWP_Unverified, /**< Risk unverified  */
	TWP_Medium, /**< Risk medium */
	TWP_High, /**< Risk High */
} TWPRiskLevel;

/**
 * @brief Score range
 * @since_tizen 2.3
 */
typedef enum 
{
	TWP_MinimalLow = 0, /**< Lowest score coresponding to the minimal level */
	TWP_MinimalHigh = 14, /**< Highest score coresponding to the minimal level */
	TWP_UnverifiedLow = 15, /**< Lowest score coresponding to the unverified level */
	TWP_UnverifiedHigh = 29, /**< Highest score coresponding to the unverified level */
	TWP_MediumLow = 30, /**< Lowest score coresponding to the medium level */
	TWP_MediumHigh = 49, /**< Highest score coresponding to the medium level */
	TWP_HighLow= 50, /**< Lowest score coresponding to the high level */
	TWP_HighHigh = 127 /**< Highest score coresponding to the high level */
} TWPScoreRange;

/**
 * @brief HTTP submit method
 * @since_tizen 2.3
 */
typedef enum
{
	TWPPOST, /**< HTTP Post method */
} TWPSubmitMethod;
	
/* forward declaration */
struct TWPRequest;	

/**
 * @brief typedef declaration of the pointer to TWPConfiguration structure
 * @since_tizen 2.3
 */
typedef struct TWPConfiguration *TWPConfigurationHandle;

/**
 * @brief typedef declaration of the pointer to TWPResponse structure
 * @since_tizen 2.3
 */
typedef struct TWPResponse*	TWPResponseHandle;

/**
 * @brief typedef declaration of the pointer to TWPUrlRating structure
 * @since_tizen 2.3
 */
typedef struct TWPUrlRating *TWPUrlRatingHandle;

/**
 * @brief typedef declaration of the pointer to TWPPolicy structure
 * @since_tizen 2.3
 */
typedef struct TWPPolicy *TWPPolicyHandle;

/**
 * @brief typedef declaration of the pointer to memory allocation function
 *
 * @since_tizen 2.3
 * @param[in] size allocation size
 * @return pointer to the allocated memory. NULL if failed to allocate
 * @see #TWPAPIInit
 */
typedef void *(*TWPFnMemAlloc)(TWPMallocSizeT size);

/**
 * @brief typedef declaration of the pointer to memory release function
 *
 * @since_tizen 2.3
 * @param[in] address an address of memroy region which is returned by void *(*TWPFnMemAlloc)(TWPMallocSizeT size);
 * @see #TWPAPIInit
 */
typedef void (*TWPFnMemFree)(void *address);

/**
 * @brief typedef declaration of the pointer to random number generation function
 *
 * @since_tizen 2.3
 * @return random number in the range of 'long' type size
 * @see #TWPConfiguration
 */
typedef long (*TWPFnRandom)(void);

/**
 * @brief typedef declaration of the pointer to the function which sets request URL
 *
 * @since_tizen 2.3
 * @param[in] request Pointer to struct TWPRequest which was passed to TWPLookupUrl()
 * @param[in] url Url to be set
 * @param[in] length The string length of the Url
 * @return #TWP_RESULT
 * @retval #TWP_SUCCESS Url is set successfuly
 * @see #TWPRequest
 * @see TWPLookupUrl()
 */
typedef TWP_RESULT (*TWPFnRequestSetUrl)(struct TWPRequest *request, const char *url,
                                         unsigned int length);

/**
 * @brief typedef declaration of the function of request method
 *
 * @since_tizen 2.3
 * @param[in] request Pointer to struct TWPRequest which was passed to TWPLookupUrl()
 * @param[in] only WP_POST is passed
 * @return #TWP_RESULT
 * @retval #TWP_SUCCESS Method was successfuly set
 * @see #TWPRequest 
 * @see TWPLookupUrl()
 */
typedef TWP_RESULT (*TWPFnRequestSetMethod)(struct TWPRequest *request, TWPSubmitMethod method);

/**
 * @brief typedef declaration of the function to send request
 *
 * @since_tizen 2.3
 * @param[in] request Pointer to struct TWPRequest which was passed to TWPLookupUrl()
 * @param[in] response TWPResponseHandle. Set NULL for synchronous request.\n
 *            uninitialized response object for asynchronous request            
 * @param[in] data request body to send to the server
 * @param[in] length the lengh of the reqest body
 * @return #TWP_RESULT 
 * @retval #TWP_SUCCESS if request is successfully sent
 * @see #TWPRequest
 * @see TWPLookupUrl()
 */
typedef TWP_RESULT (*TWPFnRequestSend)(struct TWPRequest *request, TWPResponseHandle response,
                                       const void *data, unsigned int length);

/**
 * @brief typedef declaration of the function to receive request
 *
 * @since_tizen 2.3
 * @param[in] request Pointer to struct TWPRequest which was passed to TWPLookupUrl()
 * @param[in] buffer Buffer to store response 
 * @param[in] buffer_length The length of the buffer in byte 
 * @param[in] length number of bytes that is acutualy stored in the buffer
 * @return #TWP_RESULT
 * @retval #TWP_SUCCESS if response is successfully stored in the buffer
 * @see #TWPRequest
 * @see TWPLookupUrl()
 */
typedef TWP_RESULT (*TWPFnRequestReceive)(struct TWPRequest *request, void *buffer,
                                          unsigned int buffer_length, unsigned int *length);	

/**
 * @brief Initialize data requested by SDK initialization
 * @since_tizen 2.3
 */
typedef struct TWPAPIInit
{
    int api_version; /**< Version of this structure */
    TWPFnMemAlloc memallocfunc; /**< Pointer to malloc function */
    TWPFnMemFree memfreefunc; /**< Pointer to free function */
} TWPAPIInit;

/**
 * @brief Configuration which enable caller to customize the SDK
 * @since_tizen 2.3
 */
typedef struct TWPConfiguration
{
	int config_version; /**< Configuration version */
	const char *client_id; /**< Client id for cloud to qualify */
	const char *client_key;	/**< Corresponding key for specific client for validation from cloud */
	const char *host; /**< Host name for cloud where SDK send request to, set to NULL for SDK to use default settings in plug-in */
	int	secure_connection; /**< 1 - use secured connection (HTTPS), 0 - not secured connection. */
	int	skip_dla; /**< 1 - disable DLA lookup, 0 - enable DLA lookup */
	int	obfuscate_request; /**< 1 - obfuscate request data, 0 - do not obfuscate request data */
	TWPFnRandom randomfunc; /**< Caller customized random function */
} TWPConfiguration;

/**
 * @brief Request for SDK to check URL against cloud database
 * @since_tizen 2.3
 */
typedef struct TWPRequest
{
	int request_version; /**< Request version */
	TWPFnRequestSetUrl seturlfunc; /**< Callback for SDK to set URL from SDK */
	TWPFnRequestSetMethod setmethodfunc; /**< Callback for SDK to set HTTP request method */
	TWPFnRequestSend sendfunc; /**< Callback for SDK to send HTTP request */
	TWPFnRequestReceive receivefunc; /**< Callback for SDK to receive HTTP response, if caller set it to be NULL,
                                        SDK will assume the HTTP request will be handled in a-synchronized manner */
} TWPRequest;
	
/**
 * @brief Initialize SDK.
 * @details This is a synchronized API
 *
 * @since_tizen 2.3
 * @param[in] pApiInit API initialization data structure.
 *
 * @return TWP library interface handle on success, otherwise #INVALID_TWPLIB_HANDLE.
 * @see TWPLibraryClose()
 */
TWPLIB_HANDLE TWPInitLibrary(TWPAPIInit *pApiInit);

/**
 * @brief Uninitialize SDK.
 * @details This is a synchronized API
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @see TWPLibraryOpen()
 */
void TWPUninitLibrary(TWPLIB_HANDLE hLib);

/**
 * @brief Create TWP configuration to customize SDK.
 * @details This is a synchronized API.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen(). 
 * @param[in] pConfigure caller configurations
 * @param[out] phConfigure created configuration for SDK
 *
 * @return #TWP_RESULT
 * @retval #TWP_SUCCESS - on success.
 * @retval #TWP_INVALID_PARAMETER - if any of the following errors occur:
 *           - pConfigure is NULL \n
 *           - phConfigure is NULL \n
 *           - TWPConfigration.randomfunc is NULL and TWPConfiguration.obfuscate_request is not NULL
 * @retval #TWP_INVALID_VERSION - TWPConfiguration.config_version is not TWP_CONFIG_VERSION
 * @retval #TWP_NOMEM - The function failed to allocate memory required to complete initialization of the configuration object
 * @see #TWPConfiguration
 * @see TWPConfigurationDestroy()
 */
TWP_RESULT TWPConfigurationCreate(TWPLIB_HANDLE hLib, TWPConfiguration *pConfigure, TWPConfigurationHandle *phConfigure);

/**
 * @brief Release the configuration resources allocated by TWPConfigurationCreate().
 * @details This is a synchronized API.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen(). 
 * @param[in] phConfigure configuration to be destroyed
 * @return #TWP_RESULT
 * @retval #TWP_SUCCESS - on success. 
 * @retval #TWP_INVALID_HANDLE - the value that is pointed to by phConfigure is invalid
 * @see TWPConfigurationCreate()
 */
TWP_RESULT TWPConfigurationDestroy(TWPLIB_HANDLE hLib, TWPConfigurationHandle *phConfigure);

/**
 * @brief Main function for caller to check URL reputation against the cloud database.
 * @details This can be a synchronized API or a-synchronized API depends on the configuration from caller. \n
 * \n
 * Synchronous mode \n
 *      In this synchronous operation mode, the function invokes TWPRequest::sendfunc and \n
 *      TWPRequest::receivefunc, one right after the other, expecting the entire HTTP \n
 *      transaction to be completed between the calls. Upon successful completion, the \n
 *      phResponse will point to a valid response handle that can be used to analyze results. \n
 * \n
 * Asynchronous mode \n
 *      In the asynchronous mode, the function invokes TWPRequest::sendfunc and returns \n
 *      immediately with TWP_SUCCESS. Upon completion, phResponse is NULL. The application \n
 *      is supposed to complete the HTTP transaction while calling TWPResponseWrite as \n
 *      response data becomes available. When all data was read, TWPResponseWrite must \n
 *      be called again with zero data length to signal the end transaction. \n
 * \n
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen(). 
 * @param[in] hConfigure Configuration of caller
 * @param[in] pRequest Request data structure for SDK to check with cloud
 * @param[in] iRedirUrl 1 indicating instruct the cloud server to provide a landing page \n
 *            URL to which blocked URLs can be redirected. 
 * @param[in] ppUrls An array of 7 bit ASCII character strings representing URLs to obtain \n
 *            the rating for. \n
 * \n
 *            Note: All URLs have to be normalized before submission (see RFC 3986) and \n
 *            pynicoded if required. 
 * @param[in] uCount Length of the ppUrls array.
 * @param[out] phResponse For synchronous requests, a pointer to the location where the \n
 *             response object handle will be stored upon completion. It can be NULL for \n
 *             asynchronous requests.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE - hConfigure is not a valid configuraiton handle
 * @retval #TWP_INVALID_PARAMETER - if any of the following errors occur:
 *           - ppUrls is NULL \n
 *           - any string in the ppUrls array is NULL or has zero length \n
 *           - uCount is 0 \n
 *           - pRequest is NULL \n
 *           - TWPRequest.receivefunc is not NULL and TWPRequest.phResponse is NULL (if TWPRequest.receivefunc is NULL, hResponse can be NULL)
 * @retval #TWP_INVALID_VERSION - TWPConfiguration.config_version is not TWP_CONFIG_VERSION
 * @retval #TWP_NOMEM - The function failed to allocate memory required to complete initialization of the configuration object
 * @retval #TWP_SUCCESS - on success. 
 * @retval other - Any integer value other than TWP_SUCCESS is returned by TWPRequest::setmethodfunc, TWPRequest::seturlfunc and TWPRequest::sendfunc functions
 * @see #TWPRequest
 * @see TWPLookupUrls()
 * @see TWPResponseDestory()
 */
TWP_RESULT TWPLookupUrls(TWPLIB_HANDLE hLib, TWPConfigurationHandle hConfigure, TWPRequest *pRequest,
                         int iRedirUrl, const char **ppUrls, unsigned int uCount, TWPResponseHandle *phResponse);

/**
 * @brief In asynchronous mode, caller will call this API to write received HTTP response data to SDK.
 * @details Writing with zero data length will be taken as end of HTTP transaction for SDK. \n
 * This is a synchronized API.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen(). 
 * @param[in] hResponse Response handle for SDK to keep track on HTTP transaction.
 * @param[in] pData Received HTTP response data chunk.
 * @param[in] uLength Length of the HTTP response data.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE - hResponse is not a valid handle
 * @retval #TWP_ERROR - Internal error occured
 * @retval #TWP_INVALID_RESPONSE - Server response is invalid. This can usually happen when the entire response \n
 *                                was not passed to the SDK
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPLookupUrls()
 * @see TWPResponseDestory()
 */
TWP_RESULT TWPResponseWrite(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, const void *pData, unsigned uLength);

/**
 * @brief Get web site rating by its index in URL list in the response which comply to the URL list order passed by caller in TWPLookupUrls().
 * @details This is a synchronized API.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen(). 
 * @param[in] hResponse Response handle created based on cloud response.
 * @param[in] uIndex Index of the web site in request list.
 * @param[out] phRating Rating of the specified web site.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE - hResponse is not a valid handle
 * @retval #TWP_INVALID_PARAMETER - phRating is NULL 
 * @retval #TWP_NO_DATA - Index is out of bound
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPLookupUrls()
 * @see TWPResponseDestory()
 */
TWP_RESULT TWPResponseGetUrlRatingByIndex(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, unsigned int uIndex,
                                          TWPUrlRatingHandle *phRating);

/**
 * @brief Get web site rating by its URL string.
 * @details This is a synchronized API.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen(). 
 * @param[in] hResponse Response handle created based on cloud response.
 * @param[in] pUrl URL string
 * @param[in] uUrlLength URL string length
 * @param[out] phRating Rating of the specified web site
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE - hResponse is not a valid handle
 * @retval #TWP_INVALID_PARAMETER - phRating is NULL 
 * @retval #TWP_NO_DATA - pUrl does not corespond to an existing URL rating object
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPLookupUrls()
 * @see TWPResponseDestory()
 */
TWP_RESULT TWPResponseGetUrlRatingByUrl(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, const char *pUrl,
                                        unsigned int uUrlLength, TWPUrlRatingHandle *phRating);

/**
 * @brief Get the redirection URL for blocked URL to display to user.
 * @details Blocking pages can be used by application that want to block users \n
 * from navigating to a URL that violates one of the defined policies. \n
 * The returned string must be deallocated by the application using \n
 * TWPAPIInit::TWPFnMemFree function. \n
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hResponse Response handle created based on cloud response.
 * @param[in] hRating Rating handle resolved from cloud response.
 * @param[in] hPolicy Policy handle created by caller.
 * @param[out] ppUrl Redirection URL.
 * @param[out] puLength Length of redirection URL
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE - One of hResponse, hRating and hPolicy is not a valid handle
 * @retval #TWP_INVALID_PARAMETER - ppUrl is NULL 
 * @retval #TWP_NO_DATA - The URL rating does not violate the policy (if TWPPolicyValidate() called with HPolicy and hRating would indicate no violations)
 * @retval #TWP_ERROR - An internal error occurs
 * @retval #TWP_NOMEM - Memory for the URL can not be allocated
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPLookupUrls()
 * @see TWPResponseDestory()
 */
TWP_RESULT TWPResponseGetRedirUrlFor(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, TWPUrlRatingHandle hRating,
                                     TWPPolicyHandle hPolicy, char **ppUrl, unsigned int *puLength);

/**
 * @brief Get the rating count of specified response.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hResponse Response handle created based on cloud response.
 * @param[out] puCount Rating count.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE - hResponse is not valid
 * @retval #TWP_INVALID_PARAMETER - puCount is NULL 
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPLookupUrls()
 * @see TWPResponseDestory()
 */
TWP_RESULT TWPResponseGetUrlRatingsCount(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, unsigned int *puCount);	

/**
 * @brief Release resource for response handle.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] phResponse Response handle created based on cloud response.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE - phResponse is not valid
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPLookupUrls()
 */
TWP_RESULT TWPResponseDestroy(TWPLIB_HANDLE hLib, TWPResponseHandle *phResponse);

/**
 * @brief Create the policy (set of web site categories) caller want to check.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hCfg configuration handle
 * @param[in] pCategories Web site category list
 * @param[in] uCount Category list length.
 * @param[out] phPolicy Policy handle.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_PARAMETER - if one of the following errors occur:
 *                                  - uCount is 0
 *                                  - phPolicy is NULL 
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPPolicyDestroy()
 */
TWP_RESULT TWPPolicyCreate(TWPLIB_HANDLE hLib, TWPConfigurationHandle hCfg, TWPCategories *pCategories, unsigned int uCount, TWPPolicyHandle *phPolicy);

/**
 * @brief Compare the categories assigned by security vendor to the URL represented 
 * by hRating with the categories assigned to the policy handle.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hPolicy Polcy handle
 * @param[in] hRating Rating for specific URL
 * @param[out] piViolated non-zero if intersection found between the policy and URL rating categories.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if hPolicy or hRating does not correspond to a valid handle
 * @retval #TWP_INVALID_PARAMETER - piViolated is NULL
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPResponseGetUrlRatingByUrl()
 * @see TWPResponseGetUrlRatingByIndex()
 */
TWP_RESULT TWPPolicyValidate(TWPLIB_HANDLE hLib, TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating, int *piViolated);

/**
 * @brief Retrieves all categories common between the policy and URL rating.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hPolicy Policy handle.
 * @param[in] hRating URL rating handle.
 * @param[out] ppViolated An array of all common categories. This array is allocated by using \n
 *             TWPAPIInit::memallocfunc and has to be deallocated by the caller.
 * @param[out] puLength Length of violation array.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if hPolicy or hRating does not correspond to a valid handle
 * @retval #TWP_INVALID_PARAMETER - piViolated or piLength is NULL
 * @retval #TWP_NOMEM - ppViolated can not be allocated
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPResponseGetUrlRatingByUrl()
 * @see TWPResponseGetUrlRatingByIndex()
 */
TWP_RESULT TWPPolicyGetViolations(TWPLIB_HANDLE hLib, TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating,
                                  TWPCategories **ppViolated, unsigned *puLength);

/**
 * @brief Release resource for policy handle.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] phPolicy Pointer to policy handle.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if phPolicy does not correspond to a valid handle
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPPolicyCreate()
 */
TWP_RESULT TWPPolicyDestroy(TWPLIB_HANDLE hLib, TWPPolicyHandle *phPolicy);

/**
 * @brief Get score from URL rating data structure which is assigned by security vendor.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hRating Rating handle.
 * @param[out] piScore URL score.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if hRating does not correspond to a valid handle
 * @retval #TWP_INVALID_PARAMETER - piScore is NULL
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPResponseGetUrlRatingByUrl()
 * @see TWPResponseGetUrlRatingByIndex()
 */
TWP_RESULT TWPUrlRatingGetScore(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, int *piScore);

/**
 * @brief Get corresponding URL from rating handle.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hRating Rating handle and the handle should have been obtained using TWPResponseGetUrlRatingByUrl() \n 
 * TWPResponseGetUrlRatingByIndex()
 * @param[out] ppUrl A pointer to a NULL terminated string representing \n
 *             the URL. The string is valid as long as the URL rating \n
 *             handle is valid. \n
 * @param[out] puLength An optional pointer to the length of URL string.
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if hRating does not correspond to a valid handle
 * @retval #TWP_INVALID_PARAMETER - ppUrl is NULL
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPResponseGetUrlRatingByUrl()
 * @see TWPResponseGetUrlRatingByIndex()
 */
TWP_RESULT TWPUrlRatingGetUrl(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, char **ppUrl,
                              unsigned int *puLength);

/**
 * @brief Get DLA (Deep Link Analysis) URL
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hRating Rating handle and the handle should have been obtained using TWPResponseGetUrlRatingByUrl() \n 
 * TWPResponseGetUrlRatingByIndex()
 * @param[out] ppDlaUrl A ponit to a NULL terminated string representing \n
 *             the DLA URL. This string is valid as long as the URL rating \n
 *             handle is valid. \n
 * @param[out] puLength Length of DLA URL string.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if hRating does not correspond to a valid handle
 * @retval #TWP_INVALID_PARAMETER - ppDlaUrl is NULL
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPResponseGetUrlRatingByUrl()
 * @see TWPResponseGetUrlRatingByIndex()
 */
TWP_RESULT TWPUrlRatingGetDLAUrl(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, char **ppDlaUrl,
                                 unsigned int *puLength);

/**
 * @brief Determine whether the URL rating object has the specified category.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hRating Rating handle and the handle should have been obtained using TWPResponseGetUrlRatingByUrl() \n 
 * TWPResponseGetUrlRatingByIndex()
 * @param[in] Category Category enum value.
 * @param[out] piPresent Non-zero value indicating exists.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if hRating does not correspond to a valid handle
 * @retval #TWP_INVALID_PARAMETER - piPresent is NULL
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPResponseGetUrlRatingByUrl()
 * @see TWPResponseGetUrlRatingByIndex()
 */
TWP_RESULT TWPUrlRatingHasCategory(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, TWPCategories Category,
                                   int *piPresent);

/**
 * @brief Retrives categories assigned by security vendor for the rated URL.
 *
 * @since_tizen 2.3
 * @param[in] hLib TWP library handle returned by TWPLibraryOpen().
 * @param[in] hRating Rating handle and the handle should have been obtained using TWPResponseGetUrlRatingByUrl() \n 
 * TWPResponseGetUrlRatingByIndex()
 * @param[out] ppCategories The pointer to a variable that contains the address \n
 *             of the category list. \n
 * @param[out] puLength Length of category list.
 *
 * @return #TWP_RESULT
 * @retval #TWP_INVALID_HANDLE if hPolicy or hRating does not correspond to a valid handle
 * @retval #TWP_INVALID_PARAMETER - piViolated or piLength is NULL
 * @retval #TWP_NOMEM - ppViolated can not be allocated
 * @retval #TWP_SUCCESS - on success. 
 * @see TWPResponseGetUrlRatingByUrl()
 * @see TWPResponseGetUrlRatingByIndex()
 */
TWP_RESULT TWPUrlRatingGetCategories(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, TWPCategories **ppCategories,
                                     unsigned int *puLength);
	
#ifdef __cplusplus
}
#endif
#endif

/**
 @}
 */
