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
#include <unistd.h>
#include <dlfcn.h>
#include <malloc.h>

#include "TCSImpl.h"
#include "TCSErrorCodes.h"


#define TCS_CONSTRUCT_ERRCODE(m, e) (((m) << 24) | (e))

#if defined(DEBUG)
#define DEBUG_LOG(_fmt_, _param_...)    { \
                                            printf("[TCS] %s,%d: " _fmt_, __FILE__, __LINE__, ##_param_); \
                                        }
#else
#define DEBUG_LOG(_fmt_, _param_...)
#endif


#define PLUGIN_PATH "/opt/usr/share/sec_plugin/libengine.so"


typedef TCSLIB_HANDLE (*FuncLibraryOpen)(void);
typedef int (*FuncLibraryClose)(TCSLIB_HANDLE hLib);
typedef TCSErrorCode (*FuncGetLastError)(TCSLIB_HANDLE hLib);
typedef int (*FuncScanData)(TCSLIB_HANDLE hLib, TCSScanParam *pParam, TCSScanResult *pResult);
typedef int (*FuncScanFile)(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                            int iAction, int iCompressFlag, TCSScanResult *pResult);


typedef struct PluginContext_struct
{
    TCSLIB_HANDLE hLib;
    void *pPlugin;
    FuncLibraryOpen pfLibraryOpen;
    FuncLibraryClose pfLibraryClose;
    FuncGetLastError pfGetLastError;
    FuncScanData pfScanData;
    FuncScanFile pfScanFile;
} PluginContext;


static PluginContext *LoadPlugin(void);


TCSLIB_HANDLE TCSLibraryOpen(void)
{
    PluginContext *pCtx = NULL;

    DEBUG_LOG("%s", "tcs lib open\n");
    pCtx = LoadPlugin();
    if (pCtx != NULL)
    {
        if (pCtx->pfLibraryOpen == NULL)
        {
            free(pCtx);
            return INVALID_TCSLIB_HANDLE;
        }
        DEBUG_LOG("%s", "call to TCSPLibraryOpen\n");
        pCtx->hLib = (*pCtx->pfLibraryOpen)();
        if (pCtx->hLib == INVALID_TCSLIB_HANDLE)
        {
            DEBUG_LOG("%s", "failed to open engine\n");
            if (pCtx->pPlugin != NULL)
                dlclose(pCtx->pPlugin);
            free(pCtx);
        }
        else
        {
            return (TCSLIB_HANDLE) pCtx;
        }
    }

    return INVALID_TCSLIB_HANDLE;
}

int TCSLibraryClose(TCSLIB_HANDLE hLib)
{
    int iRet = -1;
    PluginContext *pCtx = NULL;

    if (hLib == INVALID_TCSLIB_HANDLE)
        return iRet;

    pCtx = (PluginContext *) hLib;
    if (pCtx->pfLibraryClose == NULL)
        return iRet;

    iRet = (*pCtx->pfLibraryClose)(pCtx->hLib);
    if (pCtx->pPlugin != NULL)
        dlclose(pCtx->pPlugin);
   
    free(pCtx);

    return iRet;
}


TCSErrorCode TCSGetLastError(TCSLIB_HANDLE hLib)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfGetLastError == NULL)
    {
        return TCS_CONSTRUCT_ERRCODE(TCS_ERROR_MODULE_GENERIC,
                                     TCS_ERROR_NOT_IMPLEMENTED);
    }
    return (*pCtx->pfGetLastError)(pCtx->hLib);
}


int TCSScanData(TCSLIB_HANDLE hLib, TCSScanParam *pParam, TCSScanResult *pResult)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfScanData == NULL)
    {
        return -1;
    }
    return (*pCtx->pfScanData)(pCtx->hLib, pParam, pResult);
}


int TCSScanFile(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                int iAction, int iCompressFlag, TCSScanResult *pResult)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfScanFile == NULL)
    {
        return -1;
    }
    return (*pCtx->pfScanFile)(pCtx->hLib, pszFileName, iDataType, iAction, iCompressFlag, pResult);
}


static PluginContext *LoadPlugin(void)
{
    PluginContext *pCtx = NULL;
    void *pTmp = dlopen(PLUGIN_PATH, RTLD_LAZY);
    DEBUG_LOG("%s", "load plugin\n");
    if (pTmp != NULL)
    {
        FuncLibraryOpen TmpLibraryOpen;
        FuncLibraryClose TmpLibraryClose;
        FuncGetLastError TmpGetLastError;
        FuncScanData TmpScanData;
        FuncScanFile TmpScanFile;
        
        do
        {
            TmpLibraryOpen = dlsym(pTmp, "TCSPLibraryOpen");
            DEBUG_LOG("%s", "load api TCSPLibraryOpen\n");
            if (TmpLibraryOpen == NULL)
            {
                DEBUG_LOG("Failed to load TCSPLibraryOpen in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpLibraryClose = dlsym(pTmp, "TCSPLibraryClose");
            if (TmpLibraryClose == NULL)
            {
                DEBUG_LOG("Failed to load TCSPLibraryClose in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpGetLastError = dlsym(pTmp, "TCSPGetLastError");
            if (TmpGetLastError == NULL)
            {
                DEBUG_LOG("Failed to load TCSPGetLastError in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpScanData = dlsym(pTmp, "TCSPScanData");
            if (TmpScanData == NULL)
            {
                DEBUG_LOG("Failed to load TCSPScanData in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpScanFile = dlsym(pTmp, "TCSPScanFile");
            if (TmpScanFile == NULL)
            {
                DEBUG_LOG("Failed to load TCSPScanFile in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            pCtx = (PluginContext *) malloc(sizeof(PluginContext));
            if (pCtx == NULL)
            {
                dlclose(pTmp);
                break;
            }
            pCtx->pPlugin = pTmp;
            pCtx->pfLibraryOpen = TmpLibraryOpen;
            pCtx->pfLibraryClose = TmpLibraryClose;
            pCtx->pfGetLastError = TmpGetLastError;
            pCtx->pfScanData = TmpScanData;
            pCtx->pfScanFile = TmpScanFile;
        } while(0);
    }
    else
    {
        DEBUG_LOG("No plugin found.\n");
    }

    return pCtx;
}


