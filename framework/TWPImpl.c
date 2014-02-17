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


#include <unistd.h>
#include <dlfcn.h>
#include <malloc.h>

/* For multi-user support */
#include <tzplatform_config.h>

#include "TWPImpl.h"


#define SITE_PLUGIN_PATH tzplatform_mkpath(TZ_USER_SHARE, "sec_plugin/libwpengine.so")

#if defined(DEBUG)
#define DEBUG_LOG(_fmt_, _param_...)    { \
                                            printf("[TCS] %s,%d: " _fmt_, __FILE__, __LINE__, ##_param_); \
                                        }
#else
#define DEBUG_LOG(_fmt_, _param_...)
#endif


typedef TWP_RESULT (*FuncInitLibrary)(TWPAPIInit *pApiInit);
typedef void (*FuncUninitLibrary)(void);
typedef TWP_RESULT (*FuncConfigurationCreate)(TWPConfiguration *pConfigure, TWPConfigurationHandle *phConfigure);
typedef TWP_RESULT (*FuncConfigurationDestroy)(TWPConfigurationHandle *hConfigure);
typedef TWP_RESULT (*FuncLookupUrls)(TWPConfigurationHandle hConfigure, TWPRequest *pRequest, int iRedirUrl,
                                     const char **ppUrls, unsigned int uCount, TWPResponseHandle *phResponse);
typedef TWP_RESULT (*FuncResponseWrite)(TWPResponseHandle hResponse, const void *pData, unsigned uLength);
typedef TWP_RESULT (*FuncResponseGetUrlRatingByIndex)(TWPResponseHandle hResponse, unsigned int iIndex,
                                                      TWPUrlRatingHandle *hRating);
typedef TWP_RESULT (*FuncResponseGetUrlRatingByUrl)(TWPResponseHandle hResponse, const char *pUrl,
                                                    unsigned int iUrlLength, TWPUrlRatingHandle *hRating);
typedef TWP_RESULT (*FuncResponseGetRedirUrlFor)(TWPResponseHandle hResponse, TWPUrlRatingHandle hRating,
                                                 TWPPolicyHandle hPolicy, char **ppUrl, unsigned int *puLength);
typedef TWP_RESULT (*FuncResponseGetUrlRatingsCount)(TWPResponseHandle hResponse, unsigned int *puCount);
typedef TWP_RESULT (*FuncResponseDestroy)(TWPResponseHandle *handle_response);
typedef TWP_RESULT (*FuncPolicyCreate)(TWPConfigurationHandle hCfg, TWPCategories *pCategories, unsigned int uCount,
                                       TWPPolicyHandle *phPolicy);
typedef TWP_RESULT (*FuncPolicyValidate)(TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating, int *piViolated);
typedef TWP_RESULT (*FuncPolicyGetViolations)(TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating,
                                              TWPCategories **ppViolated, unsigned *puLength);
typedef TWP_RESULT (*FuncPolicyDestroy)(TWPPolicyHandle *hPolicy);
typedef TWP_RESULT (*FuncUrlRatingGetScore)(TWPUrlRatingHandle hRating, int *piScore);
typedef TWP_RESULT (*FuncUrlRatingGetUrl)(TWPUrlRatingHandle hRating, const char **ppUrl,
                                          unsigned int *puLength);
typedef TWP_RESULT (*FuncUrlRatingGetDLAUrl)(TWPUrlRatingHandle hRating, const char **ppDlaUrl,
                                             unsigned int *puLength);
typedef TWP_RESULT (*FuncUrlRatingHasCategory)(TWPUrlRatingHandle hRating, TWPCategories Category,
                                               int *piPresent);
typedef TWP_RESULT (*FuncUrlRatingGetCategories)(TWPUrlRatingHandle hRating, TWPCategories **ppCategories,
                                                 unsigned int *puLength);


typedef struct SitePluginContext_struct
{
    void *pPlugin;
    FuncUninitLibrary pfUninitLibrary;
    FuncInitLibrary pfInitLibrary;
    FuncConfigurationCreate pfConfigurationCreate;
    FuncConfigurationDestroy pfConfigurationDestroy;
    FuncLookupUrls pfLookupUrls;
    FuncResponseWrite pfResponseWrite;
    FuncResponseGetUrlRatingByIndex pfResponseGetUrlRatingByIndex;
    FuncResponseGetUrlRatingByUrl pfResponseGetUrlRatingByUrl;
    FuncResponseGetRedirUrlFor pfResponseGetRedirUrlFor;
    FuncResponseGetUrlRatingsCount pfResponseGetUrlRatingsCount;
    FuncResponseDestroy pfResponseDestroy;
    FuncPolicyCreate pfPolicyCreate;
    FuncPolicyValidate pfPolicyValidate;
    FuncPolicyGetViolations pfPolicyGetViolations;
    FuncPolicyDestroy pfPolicyDestroy;
    FuncUrlRatingGetScore pfUrlRatingGetScore;
    FuncUrlRatingGetUrl pfUrlRatingGetUrl;
    FuncUrlRatingGetDLAUrl pfUrlRatingGetDLAUrl;
    FuncUrlRatingHasCategory pfUrlRatingHasCategory;
    FuncUrlRatingGetCategories pfUrlRatingGetCategories;
} SitePluginContext;


static SitePluginContext *LoadPlugin(void);


TWPLIB_HANDLE TWPInitLibrary(TWPAPIInit *pApiInit)
{
    SitePluginContext *pCtx = NULL;

    pCtx = LoadPlugin();
    if (pCtx != NULL)
    {
        if (pCtx->pfInitLibrary != NULL &&
            (*pCtx->pfInitLibrary)(pApiInit) == TWP_SUCCESS)
            return (TWPLIB_HANDLE) pCtx;

        TWPUninitLibrary((TWPLIB_HANDLE) pCtx);
        return INVALID_TWPLIB_HANDLE;
    }

    return INVALID_TWPLIB_HANDLE;
}

void TWPUninitLibrary(TWPLIB_HANDLE hLib)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx != NULL)
    {
        if (pCtx->pfUninitLibrary != NULL)
            (*pCtx->pfUninitLibrary)();
        if (pCtx->pPlugin != NULL)
            dlclose(pCtx->pPlugin);
        free(pCtx);
    }
}

TWP_RESULT TWPConfigurationCreate(TWPLIB_HANDLE hLib, TWPConfiguration *pConfigure,
                                  TWPConfigurationHandle *phConfigure)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfConfigurationCreate == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfConfigurationCreate)(pConfigure, phConfigure);
}

TWP_RESULT TWPConfigurationDestroy(TWPLIB_HANDLE hLib, TWPConfigurationHandle *hConfigure)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfConfigurationDestroy == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfConfigurationDestroy)(hConfigure);
}

TWP_RESULT TWPLookupUrls(TWPLIB_HANDLE hLib, TWPConfigurationHandle hConfigure, TWPRequest *pRequest,
                         int iRedirUrl, const char **ppUrls, unsigned int uCount, TWPResponseHandle *phResponse)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfLookupUrls == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfLookupUrls)(hConfigure, pRequest, iRedirUrl, ppUrls, uCount, phResponse);
}

TWP_RESULT TWPResponseWrite(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, const void *pData, unsigned uLength)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfResponseWrite == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfResponseWrite)(hResponse, pData, uLength);
}

TWP_RESULT TWPResponseGetUrlRatingByIndex(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, unsigned int uIndex,
                                          TWPUrlRatingHandle *hRating)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfResponseGetUrlRatingByIndex == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfResponseGetUrlRatingByIndex)(hResponse, uIndex, hRating);
}

TWP_RESULT TWPResponseGetUrlRatingByUrl(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, const char *pUrl,
                                        unsigned int uUrlLength, TWPUrlRatingHandle *hRating)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfResponseGetUrlRatingByUrl == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfResponseGetUrlRatingByUrl)(hResponse, pUrl, uUrlLength, hRating);
}

TWP_RESULT TWPResponseGetRedirUrlFor(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, TWPUrlRatingHandle hRating,
                                     TWPPolicyHandle hPolicy, char **ppUrl, unsigned int *puLength)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfResponseGetRedirUrlFor == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfResponseGetRedirUrlFor)(hResponse, hRating, hPolicy, ppUrl, puLength);
}

TWP_RESULT TWPResponseGetUrlRatingsCount(TWPLIB_HANDLE hLib, TWPResponseHandle hResponse, unsigned int *puCount)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfResponseGetUrlRatingsCount == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfResponseGetUrlRatingsCount)(hResponse, puCount);
}

TWP_RESULT TWPResponseDestroy(TWPLIB_HANDLE hLib, TWPResponseHandle *hResponse)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfResponseDestroy == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfResponseDestroy)(hResponse);
}

TWP_RESULT TWPPolicyCreate(TWPLIB_HANDLE hLib, TWPConfigurationHandle hCfg, TWPCategories *pCategories,
                           unsigned int uCount, TWPPolicyHandle *phPolicy)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfPolicyCreate == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfPolicyCreate)(hCfg, pCategories, uCount, phPolicy);
}

TWP_RESULT TWPPolicyValidate(TWPLIB_HANDLE hLib, TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating, int *piViolated)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfPolicyValidate == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfPolicyValidate)(hPolicy, hRating, piViolated);
}

TWP_RESULT TWPPolicyGetViolations(TWPLIB_HANDLE hLib, TWPPolicyHandle hPolicy, TWPUrlRatingHandle hRating,
                                  TWPCategories **ppViolated, unsigned *puLength)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfPolicyGetViolations == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfPolicyGetViolations)(hPolicy, hRating, ppViolated, puLength);
}

TWP_RESULT TWPPolicyDestroy(TWPLIB_HANDLE hLib, TWPPolicyHandle *hPolicy)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfPolicyDestroy == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfPolicyDestroy)(hPolicy);
}

TWP_RESULT TWPUrlRatingGetScore(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, int *piScore)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfUrlRatingGetScore == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfUrlRatingGetScore)(hRating, piScore);
}

TWP_RESULT TWPUrlRatingGetUrl(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, char **ppUrl,
                              unsigned int *puLength)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfUrlRatingGetUrl == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfUrlRatingGetUrl)(hRating, (const char **) ppUrl, puLength);
}

TWP_RESULT TWPUrlRatingGetDLAUrl(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, char **ppDlaUrl,
                                 unsigned int *puLength)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfUrlRatingGetDLAUrl == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfUrlRatingGetDLAUrl)(hRating, (const char **) ppDlaUrl, puLength);
}

TWP_RESULT TWPUrlRatingHasCategory(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, TWPCategories Category,
                                   int *piPresent)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfUrlRatingHasCategory == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfUrlRatingHasCategory)(hRating, Category, piPresent);
}

TWP_RESULT TWPUrlRatingGetCategories(TWPLIB_HANDLE hLib, TWPUrlRatingHandle hRating, TWPCategories **ppCategories,
                                     unsigned int *puLength)
{
    SitePluginContext *pCtx = (SitePluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfUrlRatingGetCategories == NULL)
        return TWP_NOT_IMPLEMENTED;

    return (*pCtx->pfUrlRatingGetCategories)(hRating, ppCategories, puLength);
}

static SitePluginContext *LoadPlugin(void)
{
    SitePluginContext *pCtx = NULL;
    void *pTmp = dlopen(SITE_PLUGIN_PATH, RTLD_LAZY);
    DEBUG_LOG("%s", "load site plugin\n");
    if (pTmp != NULL)
    {
        FuncUninitLibrary TmpUninitLibrary;
        FuncInitLibrary TmpInitLibrary;
        FuncConfigurationCreate TmpConfigurationCreate;
        FuncConfigurationDestroy TmpConfigurationDestroy;
        FuncLookupUrls TmpLookupUrls;
        FuncResponseWrite TmpResponseWrite;
        FuncResponseGetUrlRatingByIndex TmpResponseGetUrlRatingByIndex;
        FuncResponseGetUrlRatingByUrl TmpResponseGetUrlRatingByUrl;
        FuncResponseGetRedirUrlFor TmpResponseGetRedirUrlFor;
        FuncResponseGetUrlRatingsCount TmpResponseGetUrlRatingsCount;
        FuncResponseDestroy TmpResponseDestroy;
        FuncPolicyCreate TmpPolicyCreate;
        FuncPolicyValidate TmpPolicyValidate;
        FuncPolicyGetViolations TmpPolicyGetViolations;
        FuncPolicyDestroy TmpPolicyDestroy;
        FuncUrlRatingGetScore TmpUrlRatingGetScore;
        FuncUrlRatingGetUrl TmpUrlRatingGetUrl;
        FuncUrlRatingGetDLAUrl TmpUrlRatingGetDLAUrl;
        FuncUrlRatingHasCategory TmpUrlRatingHasCategory;
        FuncUrlRatingGetCategories TmpUrlRatingGetCategories;
        
        do
        {
            TmpInitLibrary = dlsym(pTmp, "TWPPInitLibrary");
            DEBUG_LOG("%s", "load api TWPPInitLibrary\n");
            if (TmpInitLibrary == NULL)
            {
                DEBUG_LOG("Failed to load TWPPInitLibrary in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpUninitLibrary = dlsym(pTmp, "TWPPUninitLibrary");
            DEBUG_LOG("%s", "load api TWPPUninitLibrary\n");
            if (TmpUninitLibrary == NULL)
            {
                DEBUG_LOG("Failed to load TWPPUninitLibrary in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpConfigurationCreate = dlsym(pTmp, "TWPPConfigurationCreate");
            DEBUG_LOG("%s", "load api TWPPConfigurationCreate\n");
            if (TmpConfigurationCreate == NULL)
            {
                DEBUG_LOG("Failed to load TWPPConfigurationCreate in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpConfigurationDestroy = dlsym(pTmp, "TWPPConfigurationDestroy");
            DEBUG_LOG("%s", "load api TWPPConfigurationDestroy\n");
            if (TmpConfigurationDestroy == NULL)
            {
                DEBUG_LOG("Failed to load TWPPConfigurationDestroy in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpLookupUrls = dlsym(pTmp, "TWPPLookupUrls");
            DEBUG_LOG("%s", "load api TWPPLookupUrls\n");
            if (TmpLookupUrls == NULL)
            {
                DEBUG_LOG("Failed to load TWPPLookupUrls in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpResponseWrite = dlsym(pTmp, "TWPPResponseWrite");
            DEBUG_LOG("%s", "load api TWPPResponseWrite\n");
            if (TmpResponseWrite == NULL)
            {
                DEBUG_LOG("Failed to load TWPPResponseWrite in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpResponseGetUrlRatingByIndex = dlsym(pTmp, "TWPPResponseGetUrlRatingByIndex");
            DEBUG_LOG("%s", "load api TWPPResponseGetUrlRatingByIndex\n");
            if (TmpResponseGetUrlRatingByIndex == NULL)
            {
                DEBUG_LOG("Failed to load TWPPResponseGetUrlRatingByIndex in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpResponseGetUrlRatingByUrl = dlsym(pTmp, "TWPPResponseGetUrlRatingByUrl");
            DEBUG_LOG("%s", "load api TWPPResponseGetUrlRatingByUrl\n");
            if (TmpResponseGetUrlRatingByUrl == NULL)
            {
                DEBUG_LOG("Failed to load TWPPResponseGetUrlRatingByUrl in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpResponseGetRedirUrlFor = dlsym(pTmp, "TWPPResponseGetRedirUrlFor");
            DEBUG_LOG("%s", "load api TWPPResponseGetRedirUrlFor\n");
            if (TmpResponseGetRedirUrlFor == NULL)
            {
                DEBUG_LOG("Failed to load TWPPResponseGetRedirUrlFor in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpResponseGetUrlRatingsCount = dlsym(pTmp, "TWPPResponseGetUrlRatingsCount");
            DEBUG_LOG("%s", "load api TWPPResponseGetUrlRatingsCount\n");
            if (TmpResponseGetUrlRatingsCount == NULL)
            {
                DEBUG_LOG("Failed to load TWPPResponseGetUrlRatingsCount in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpResponseDestroy = dlsym(pTmp, "TWPPResponseDestroy");
            DEBUG_LOG("%s", "load api TWPPResponseDestroy\n");
            if (TmpResponseDestroy == NULL)
            {
                DEBUG_LOG("Failed to load TWPPResponseDestroy in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpPolicyCreate = dlsym(pTmp, "TWPPPolicyCreate");
            DEBUG_LOG("%s", "load api TWPPPolicyCreate\n");
            if (TmpPolicyCreate == NULL)
            {
                DEBUG_LOG("Failed to load TWPPPolicyCreate in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpPolicyValidate = dlsym(pTmp, "TWPPPolicyValidate");
            DEBUG_LOG("%s", "load api TWPPPolicyValidate\n");
            if (TmpPolicyValidate == NULL)
            {
                DEBUG_LOG("Failed to load TWPPPolicyValidate in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpPolicyGetViolations = dlsym(pTmp, "TWPPPolicyGetViolations");
            DEBUG_LOG("%s", "load api TWPPPolicyGetViolations\n");
            if (TmpPolicyGetViolations == NULL)
            {
                DEBUG_LOG("Failed to load TWPPPolicyGetViolations in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpPolicyDestroy = dlsym(pTmp, "TWPPPolicyDestroy");
            DEBUG_LOG("%s", "load api TWPPPolicyDestroy\n");
            if (TmpPolicyDestroy == NULL)
            {
                DEBUG_LOG("Failed to load TWPPPolicyDestroy in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpUrlRatingGetScore = dlsym(pTmp, "TWPPUrlRatingGetScore");
            DEBUG_LOG("%s", "load api TWPPUrlRatingGetScore\n");
            if (TmpUrlRatingGetScore == NULL)
            {
                DEBUG_LOG("Failed to load TWPPUrlRatingGetScore in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpUrlRatingGetUrl = dlsym(pTmp, "TWPPUrlRatingGetUrl");
            DEBUG_LOG("%s", "load api TWPPUrlRatingGetUrl\n");
            if (TmpUrlRatingGetUrl == NULL)
            {
                DEBUG_LOG("Failed to load TWPPUrlRatingGetUrl in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpUrlRatingGetDLAUrl = dlsym(pTmp, "TWPPUrlRatingGetDLAUrl");
            DEBUG_LOG("%s", "load api TWPPUrlRatingGetDLAUrl\n");
            if (TmpUrlRatingGetDLAUrl == NULL)
            {
                DEBUG_LOG("Failed to load TWPPUrlRatingGetDLAUrl in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpUrlRatingHasCategory = dlsym(pTmp, "TWPPUrlRatingHasCategory");
            DEBUG_LOG("%s", "load api TWPPUrlRatingHasCategory\n");
            if (TmpUrlRatingHasCategory == NULL)
            {
                DEBUG_LOG("Failed to load TWPPUrlRatingHasCategory in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpUrlRatingGetCategories = dlsym(pTmp, "TWPPUrlRatingGetCategories");
            DEBUG_LOG("%s", "load api TWPPUrlRatingGetCategories\n");
            if (TmpUrlRatingGetCategories == NULL)
            {
                DEBUG_LOG("Failed to load TWPPUrlRatingGetCategories in %s\n", SITE_PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            pCtx = (SitePluginContext *) malloc(sizeof(SitePluginContext));
            if (pCtx == NULL)
            {
                dlclose(pTmp);
                break;
            }
            
            pCtx->pPlugin = pTmp;
            pCtx->pfUninitLibrary = TmpUninitLibrary;
            pCtx->pfInitLibrary = TmpInitLibrary;
            pCtx->pfConfigurationCreate = TmpConfigurationCreate;
            pCtx->pfConfigurationDestroy = TmpConfigurationDestroy;
            pCtx->pfLookupUrls = TmpLookupUrls;
            pCtx->pfResponseWrite = TmpResponseWrite;
            pCtx->pfResponseGetUrlRatingByIndex = TmpResponseGetUrlRatingByIndex;
            pCtx->pfResponseGetUrlRatingByUrl = TmpResponseGetUrlRatingByUrl;
            pCtx->pfResponseGetRedirUrlFor = TmpResponseGetRedirUrlFor;
            pCtx->pfResponseGetUrlRatingsCount = TmpResponseGetUrlRatingsCount;
            pCtx->pfResponseDestroy = TmpResponseDestroy;
            pCtx->pfPolicyCreate = TmpPolicyCreate;
            pCtx->pfPolicyValidate = TmpPolicyValidate;
            pCtx->pfPolicyGetViolations = TmpPolicyGetViolations;
            pCtx->pfPolicyDestroy = TmpPolicyDestroy;
            pCtx->pfUrlRatingGetScore = TmpUrlRatingGetScore;
            pCtx->pfUrlRatingGetUrl = TmpUrlRatingGetUrl;
            pCtx->pfUrlRatingGetDLAUrl = TmpUrlRatingGetDLAUrl;
            pCtx->pfUrlRatingHasCategory = TmpUrlRatingHasCategory;
            pCtx->pfUrlRatingGetCategories = TmpUrlRatingGetCategories;
        } while(0);
    }
    else
    {
        DEBUG_LOG("No plugin found.\n");
    }

    return pCtx;
}
