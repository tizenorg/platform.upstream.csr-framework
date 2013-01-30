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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "TWPImpl.h"

#include "XMHttp.h"
#include "TWPTest.h"
#include "UrlInfo.h"

/* Test cases. */
static void TWPStartup(void);
static void TWPCleanup(void);
static void TWPInitLibrary_0001(void);
static void TWPInitLibrary_0002(void);
static void TWPInitLibrary_0003(void);
static void TWPInitLibrary_0004(void);
static void TWPConfigurationCreate_0001(void);
static void TWPConfigurationCreate_0002(void);
static void TWPConfigurationCreate_0003(void);
static void TWPPolicyCreate_0001(void);
static void TWPPolicyCreate_0002(void);
static void TWPPolicyCreate_0003(void);
static void TWPLookupUrls_0001(void);
static void TWPLookupUrls_0002(void);
static void TWPLookupUrls_0003(void);
static void TWPLookupUrls_0004(void);
static void TWPLookupUrls_0005(void);
static void TWPGetUrlRating_0001(void);
static void TWPGetUrlRating_0002(void);
static void TWPGetUrlRating_0003(void);
static void TWPGetUrlRating_0004(void);
static void TWPGetUrlRating_0005(void);
static void TWPGetUrlRating_0006(void);
static void TWPGetUrlRatingsCount_0001(void);
static void TWPGetUrlRatingsCount_0002(void);
static void TWPGetRedirUrlFor_0001(void);
static void TWPGetRedirUrlFor_0002(void);
static void TWPPolicyValidate_0001(void);
static void TWPPolicyValidate_0002(void);
static void TWPPolicyValidate_0003(void);
static void TWPPolicyGetViolations_0001(void);
static void TWPPolicyGetViolations_0002(void);
static void TWPPolicyGetViolations_0003(void);
static void TWPRatingGetScore_0001(void);
static void TWPRatingGetScore_0002(void);
static void TWPRatingGetUrl_0001(void);
static void TWPRatingGetUrl_0002(void);
static void TWPRatingGetDLAUrl_0001(void);
static void TWPRatingGetDLAUrl_0002(void);
static void TWPRatingHasCategory_0001(void);
static void TWPRatingHasCategory_0002(void);
static void TWPRatingHasCategory_0003(void);
static void TWPRatingGetCategories_0001(void);
static void TWPRatingGetCategories_0002(void);
static void TWPRatingGetCategories_0003(void);

static void TestCases(void);


extern int TestCasesCount;
extern int Success;
extern int Failures;
extern TWPResponseHandle hAResponse;

TWPAPIInit Init;
TWPConfiguration Cfg;
TRequest Request;


int main(int argc, char **argv)
{
    TWPStartup();
    TestCases();
    TWPCleanup();

    return 0;
}


static void TestCases(void)
{
    TWPInitLibrary_0001();
    TWPInitLibrary_0002();
    TWPInitLibrary_0003();
    TWPInitLibrary_0004();
    TWPConfigurationCreate_0001();
    TWPConfigurationCreate_0002();
    TWPConfigurationCreate_0003();
    TWPPolicyCreate_0001();
    TWPPolicyCreate_0002();
    TWPPolicyCreate_0003();
    TWPLookupUrls_0001();
    TWPLookupUrls_0002();
    TWPLookupUrls_0003();
    TWPLookupUrls_0004();
    TWPLookupUrls_0005();
    TWPGetUrlRating_0001();
    TWPGetUrlRating_0002();
    TWPGetUrlRating_0003();
    TWPGetUrlRating_0004();
    TWPGetUrlRating_0005();
    TWPGetUrlRating_0006();
    TWPGetUrlRatingsCount_0001();
    TWPGetUrlRatingsCount_0002();
    TWPGetRedirUrlFor_0001();
    TWPGetRedirUrlFor_0002();
    TWPPolicyValidate_0001();
    TWPPolicyValidate_0002();
    TWPPolicyValidate_0003();
    TWPPolicyGetViolations_0001();
    TWPPolicyGetViolations_0002();
    TWPPolicyGetViolations_0003();
    TWPRatingGetScore_0001();
    TWPRatingGetScore_0002();
    TWPRatingGetUrl_0001();
    TWPRatingGetUrl_0002();
    TWPRatingGetDLAUrl_0001();
    TWPRatingGetDLAUrl_0002();
    TWPRatingHasCategory_0001();
    TWPRatingHasCategory_0002();
    TWPRatingHasCategory_0003();
    TWPRatingGetCategories_0001();
    TWPRatingGetCategories_0002();
    TWPRatingGetCategories_0003();
}


static void TWPInitLibrary_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib = INVALID_TWPLIB_HANDLE;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TESTCASEDTOR(&TestCtx);
    TWPUninitLibrary(hLib);
}


static void TWPInitLibrary_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;

    TESTCASECTOR(&TestCtx, __FUNCTION__);

    RemoveEngine();

    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) == INVALID_TWPLIB_HANDLE);
    TESTCASEDTOR(&TestCtx);

    RestoreEngine();
}


static void TWPInitLibrary_0003(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;

    RemoveEngine();

    TESTCASECTOR(&TestCtx, __FUNCTION__);

    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) == INVALID_TWPLIB_HANDLE);
    RestoreEngine();
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TWPUninitLibrary(hLib);

    TESTCASEDTOR(&TestCtx);
}


static void TWPInitLibrary_0004(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;

    TESTCASECTOR(&TestCtx, __FUNCTION__);

    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TWPUninitLibrary(hLib);
    RemoveEngine();
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) == INVALID_TWPLIB_HANDLE);
    TESTCASEDTOR(&TestCtx);
    RestoreEngine();
}


static void TWPConfigurationCreate_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPConfigurationCreate_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, NULL, &hCfg) != TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPConfigurationCreate_0003(void)
{
    TestCase TestCtx;
    TWPConfigurationHandle hCfg;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT(TWPConfigurationCreate(INVALID_TWPLIB_HANDLE, NULL, &hCfg) != TWP_SUCCESS);
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyCreate_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPPolicyHandle hPolicy;
    TWPConfigurationHandle hCfg;
    TWPCategories Categories[1] =
    {
        TWP_Artcultureheritage, 
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyCreate(hLib, hCfg, Categories, ELEMENT_NUM(Categories), &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(hPolicy != NULL);
    TEST_ASSERT(TWPPolicyDestroy(hLib, &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyCreate_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPPolicyHandle hPolicy;
    TWPConfigurationHandle hCfg;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyCreate(hLib, hCfg, NULL, 0, &hPolicy) != TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyCreate_0003(void)
{
    TestCase TestCtx;
    TWPPolicyHandle hPolicy;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT(TWPPolicyCreate(INVALID_TWPLIB_HANDLE, NULL, NULL, 0, &hPolicy) != TWP_SUCCESS);
    TESTCASEDTOR(&TestCtx);
}


static void TWPLookupUrls_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0, ppUrls,
                              ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPLookupUrls_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                NULL, 0, &hResponse) != TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPLookupUrls_0003(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TRequest ARequest = Request;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    ARequest.Request.receivefunc = NULL;
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &ARequest, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseWrite(hLib, hAResponse, ARequest.ResponseBody, ARequest.ResponseLength) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseWrite(hLib, hAResponse, "", 0) == TWP_SUCCESS);
    TEST_ASSERT(hAResponse != NULL);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hAResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPLookupUrls_0004(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0, ppUrls,
                ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPLookupUrls_0005(void)
{
    TestCase TestCtx;
    TWPResponseHandle hResponse;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    RemoveEngine();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT(TWPLookupUrls(INVALID_TWPLIB_HANDLE, NULL, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRating_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByIndex(hLib, hResponse, 0, &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRating_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, URL_0_0,
                strlen(URL_0_0), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRating_0003(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, NULL, 0, &hRating) != TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRating_0004(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByIndex(hLib, hResponse, 0, &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRating_0005(void)
{
    TestCase TestCtx;
    TWPUrlRatingHandle hRating;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    /* pre-condition is stub library */
    RemoveEngine();
    TEST_ASSERT(TWPResponseGetUrlRatingByIndex(INVALID_TWPLIB_HANDLE,
                NULL, 0, &hRating) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRating_0006(void)
{
    TestCase TestCtx;
    TWPUrlRatingHandle hRating;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    /* pre-condition is stub library */
    RemoveEngine();
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(INVALID_TWPLIB_HANDLE, NULL,
                NULL, 0, &hRating) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRatingsCount_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[] =
    {
        URL_0_0,
        URL_1_0
    };
    unsigned int uCount = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByIndex(hLib, hResponse, 0, &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseGetUrlRatingsCount(hLib, hResponse, &uCount) == TWP_SUCCESS);
    TEST_ASSERT(uCount == ELEMENT_NUM(ppUrls));
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetUrlRatingsCount_0002(void)
{
    unsigned int uCount;
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    /* pre-condition is stub library */
    RemoveEngine();
    TEST_ASSERT(TWPResponseGetUrlRatingsCount(INVALID_TWPLIB_HANDLE,
                NULL, &uCount) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetRedirUrlFor_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_3_0
    };
    char *pUrl = NULL;
    unsigned int uLength = 0;
    TWPPolicyHandle hPolicy;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 1,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByIndex(hLib, hResponse, 0, &hRating) == TWP_SUCCESS);
    TEST_ASSERT(hRating != NULL);
    TEST_ASSERT(TWPPolicyCreate(hLib, hCfg, CATEGORIES_0_0_1, ELEMENT_NUM(CATEGORIES_0_0_1), &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(hPolicy != NULL);
    TEST_ASSERT(TWPResponseGetRedirUrlFor(hLib, hResponse, hRating, hPolicy, &pUrl, &uLength) == TWP_SUCCESS);
    TEST_ASSERT(pUrl != NULL);
    free(pUrl);
    TEST_ASSERT(TWPPolicyDestroy(hLib, &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPGetRedirUrlFor_0002(void)
{
    TestCase TestCtx;
    TWPPolicyHandle hPolicy = NULL;
    char *pUrl = NULL;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    /* pre-condition is stub library */
    RemoveEngine();
    TEST_ASSERT(TWPResponseGetRedirUrlFor(INVALID_TWPLIB_HANDLE,
                NULL, NULL, hPolicy, &pUrl, &uLength) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyValidate_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    TWPPolicyHandle hPolicy;
    int iViolated = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyCreate(hLib, hCfg, CATEGORIES_0_0_1, ELEMENT_NUM(CATEGORIES_0_0_1), &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyValidate(hLib, hPolicy, hRating, &iViolated) == TWP_SUCCESS);
    TEST_ASSERT(iViolated == 1);
    TEST_ASSERT(TWPPolicyDestroy(hLib, &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyValidate_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    TWPPolicyHandle hPolicy;
    int iViolated = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyCreate(hLib, hCfg, CATEGORIES_0_0_0, ELEMENT_NUM(CATEGORIES_0_0_0), &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyValidate(hLib, hPolicy, hRating, &iViolated) == TWP_SUCCESS);
    TEST_ASSERT(iViolated == 0);
    TEST_ASSERT(TWPPolicyDestroy(hLib, &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyValidate_0003(void)
{
    TestCase TestCtx;
    int iViolated = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    RemoveEngine();
    TEST_ASSERT(TWPPolicyValidate(INVALID_TWPLIB_HANDLE,
                NULL, NULL, &iViolated) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyGetViolations_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    TWPPolicyHandle hPolicy;
    TWPCategories *pViolated = NULL;
    unsigned int uLength = 0;
    int i;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyCreate(hLib, hCfg, CATEGORIES_0_0_2, ELEMENT_NUM(CATEGORIES_0_0_2), &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyGetViolations(hLib, hPolicy, hRating, &pViolated, &uLength) == TWP_SUCCESS);
    TEST_ASSERT(uLength == ELEMENT_NUM(VIOLATIONS_0_0_2));
    for (i = 0; i < uLength; i++)
    {
        TEST_ASSERT(pViolated[i] == VIOLATIONS_0_0_2[i]);
    }
    free(pViolated);
    TEST_ASSERT(TWPPolicyDestroy(hLib, &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyGetViolations_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    TWPPolicyHandle hPolicy;
    TWPCategories *pViolated = NULL;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyCreate(hLib, hCfg, CATEGORIES_0_0_0, ELEMENT_NUM(CATEGORIES_0_0_0), &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPPolicyGetViolations(hLib, hPolicy, hRating, &pViolated, &uLength) == TWP_SUCCESS);
    TEST_ASSERT(uLength == 0);
    TEST_ASSERT(pViolated == NULL);
    TEST_ASSERT(TWPPolicyDestroy(hLib, &hPolicy) == TWP_SUCCESS);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPPolicyGetViolations_0003(void)
{
    TestCase TestCtx;
    TWPCategories *pViolated = NULL;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    RemoveEngine();
    TEST_ASSERT(TWPPolicyGetViolations(INVALID_TWPLIB_HANDLE, NULL, NULL,
        &pViolated, &uLength) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetScore_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    int iScore;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPUrlRatingGetScore(hLib, hRating, &iScore) == TWP_SUCCESS);
    TEST_ASSERT(iScore == SCORE_0_0);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetScore_0002(void)
{
    TestCase TestCtx;
    int iScore;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    RemoveEngine();
    TEST_ASSERT(TWPUrlRatingGetScore(INVALID_TWPLIB_HANDLE, NULL, &iScore) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetUrl_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    char *pUrl;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPUrlRatingGetUrl(hLib, hRating, &pUrl, &uLength) == TWP_SUCCESS);
    TEST_ASSERT(strcmp(pUrl, ppUrls[0]) == 0);
    TEST_ASSERT(uLength == strlen(ppUrls[0]));
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetUrl_0002(void)
{
    TestCase TestCtx;
    char *pUrl;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    RemoveEngine();
    TEST_ASSERT(TWPUrlRatingGetUrl(INVALID_TWPLIB_HANDLE, NULL, &pUrl, &uLength) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetDLAUrl_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_2_0
    };
    char *pUrl;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPUrlRatingGetDLAUrl(hLib, hRating, &pUrl, &uLength) == TWP_SUCCESS);
    TEST_ASSERT(pUrl != NULL);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetDLAUrl_0002(void)
{
    TestCase TestCtx;
    char *pUrl;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    RemoveEngine();
    TEST_ASSERT(TWPUrlRatingGetDLAUrl(INVALID_TWPLIB_HANDLE, NULL, &pUrl, &uLength) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingHasCategory_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    int iPresent = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPUrlRatingHasCategory(hLib, hRating, CATEGORY_0_0_1, &iPresent) == TWP_SUCCESS);
    TEST_ASSERT(iPresent == 1);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingHasCategory_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    int iPresent = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPUrlRatingHasCategory(hLib, hRating, CATEGORY_0_0_0, &iPresent) == TWP_SUCCESS);
    TEST_ASSERT(iPresent == 0);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingHasCategory_0003(void)
{
    TestCase TestCtx;
    int iPresent = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    RemoveEngine();
    TEST_ASSERT(TWPUrlRatingHasCategory(INVALID_TWPLIB_HANDLE, NULL, CATEGORY_0_0_0, &iPresent) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetCategories_0001(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_0_0
    };
    TWPCategories *pCategories;
    unsigned int uLength;
    int i;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPUrlRatingGetCategories(hLib, hRating, &pCategories, &uLength) == TWP_SUCCESS);
    TEST_ASSERT(uLength == ELEMENT_NUM(CATEGORIES_0_0_1));
    for (i = 0; i < uLength; i++)
    {
        TEST_ASSERT(pCategories[i] == CATEGORIES_0_0_1[i]);
    }
    free(pCategories);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetCategories_0002(void)
{
    TestCase TestCtx;
    TWPLIB_HANDLE hLib;
    TWPConfigurationHandle hCfg;
    TWPResponseHandle hResponse;
    TWPUrlRatingHandle hRating;
    const char *ppUrls[1] =
    {
        URL_1_0
    };
    TWPCategories *pCategories = NULL;
    unsigned int uLength = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    TEST_ASSERT((hLib = TWPInitLibrary(&Init)) != INVALID_TWPLIB_HANDLE);
    TEST_ASSERT(TWPConfigurationCreate(hLib, &Cfg, &hCfg) == TWP_SUCCESS);
    TEST_ASSERT(hCfg != NULL);
    TEST_ASSERT(TWPLookupUrls(hLib, hCfg, (TWPRequest *) &Request, 0,
                              ppUrls, ELEMENT_NUM(ppUrls), &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(hResponse != NULL);
    TEST_ASSERT(TWPResponseGetUrlRatingByUrl(hLib, hResponse, ppUrls[0], strlen(ppUrls[0]), &hRating) == TWP_SUCCESS);
    TEST_ASSERT(TWPUrlRatingGetCategories(hLib, hRating, &pCategories, &uLength) == TWP_SUCCESS);
    TEST_ASSERT(pCategories != NULL);
    TEST_ASSERT(TWPResponseDestroy(hLib, &hResponse) == TWP_SUCCESS);
    TEST_ASSERT(TWPConfigurationDestroy(hLib, &hCfg) == TWP_SUCCESS);
    TWPUninitLibrary(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TWPRatingGetCategories_0003(void)
{
    TestCase TestCtx;
    TWPCategories *pCategories;
    unsigned int uLength;

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    RemoveEngine();
    TEST_ASSERT(TWPUrlRatingGetCategories(INVALID_TWPLIB_HANDLE, NULL, &pCategories, &uLength) != TWP_SUCCESS);
    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}


static void TWPStartup(void)
{
    extern int TestCasesCount;
    extern int Success;
    extern int Failures;

    TestCasesCount = 0;
    Success = 0;
    Failures = 0;

    Init.api_version = TWPAPI_VERSION;
    Init.memallocfunc = (TWPFnMemAlloc) malloc;
    Init.memfreefunc = free;

    srandom(time(NULL));

    Cfg.config_version = TWPCONFIG_VERSION;
    Cfg.client_id = "SamsungTizen";
	Cfg.client_key = "{3353DFB5-6978-43E7-AFBE-E3B251B4C303}";
    Cfg.host = NULL;
    Cfg.secure_connection = 0;
    Cfg.skip_dla = 0;
    Cfg.obfuscate_request = 1;
    Cfg.randomfunc = GenerateRandomNumber;

    Request.Request.request_version = TWPREQUEST_VERSION;
    Request.Request.seturlfunc = CbSetUrl;
    Request.Request.setmethodfunc = CbSetMethod;
    Request.Request.sendfunc = CbSend;
    Request.Request.receivefunc = CbRecv;

    CreateTestDirs();
}


static void TWPCleanup(void)
{
    LOG_OUT("@@@@@@@@@@@@@@@@@@@@@@@@\n");
    LOG_OUT("Test done: %d executed, %d passed, %d failure\n", TestCasesCount, Success, Failures);
    
    if (Request.hHttp != INVALID_XM_HTTP_HANDLE)
        XmHttpClose(Request.hHttp);

    DestoryTestDirs();
}
