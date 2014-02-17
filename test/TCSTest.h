#ifndef TCSTEST_H
#define TCSTEST_H


#include <setjmp.h>


/* For multi-user support */
#include <tzplatform_config.h>

#ifdef __cplusplus 
extern "C" {
#endif

#define TEST_SUITE_VERSION "0.0.1"

/* Immediate value definitions. */
#define MAX_TEST_NUM 128

/* Test content polarities */
#define BENIGN_DATA 0
#define INFECTED_DATA 1

/* Maximum CS API name length. */
#define MAX_TCS_API_NAME_LEN 128

/* Content backup directory before repairing test. */
#define TCS_BACKUP_CONTENT_DIR "contents_bak"

/* Content directory for testing. */
#define TCS_TEST_CONTENT_DIR "contents_test"

/* Default maximum number of threads for concurrency test. */
#define MAX_TEST_THREADS 10

/* Default maximum concurrency test timeout (in seconds). */
#define DEFAULT_CONCURRENCY_TEST_TIMEOUT 30

/* Sleep interval for thread context switch. */
#define SLEEP_INTERVAL 500


/* Output methods. */
#define LOG_OUT(fmt, x...) printf("Log:"fmt, ##x)

#define TRY_TEST { \
    int _ret_ = setjmp(JmpBuf); \
    if (_ret_ == 1) { \
        Failures++; \
    } else { \

#define FAIL_TEST longjmp(JmpBuf, 1);

#define TESTCASECTOR(_ctx_, _api_, _testtype_, _polarity_, _action_, _callback_) \
        TRY_TEST \
        TestCaseCtor(_ctx_, _api_, _testtype_, _polarity_, _action_, _callback_);

#define TESTCASECTOREX(_ctx_, _api_, _testtype_, _polarity_, _action_, _flag_, _callback_) \
        TRY_TEST \
        TestCaseCtorEx(_ctx_, _api_, _testtype_, _polarity_, _action_, _flag_, _callback_);

#define TESTCASEDTOR(_ctx_) \
        TestCaseDtor(_ctx_); \
    } \
} \

/* Test assert method. */
#define TEST_ASSERT(cond) if (!(cond)) {LOG_OUT("Test failed!! at : %s, %d\n", __FILE__, __LINE__); FAIL_TEST}

#define ELEMENT_NUM(ary) (sizeof(ary) / sizeof((ary)[0]))

#define TEST_CONSTRUCT_ERRCODE(m, e) (((m) << 24) | (e))

#define PLUGIN_PATH tzplatform_mkpath(TZ_USER_SHARE, "sec_plugin/libengine.so")
#define PLUGIN_DIR tzplatform_mkpath(TZ_USER_SHARE, "sec_plugin/")

/* Test content file types */
enum ENUM_MALWARE_TEST_TYPES
{
    MALWARE_TTYPE_BUFFER = 0,
    MALWARE_TTYPE_HTML,
    MALWARE_TTYPE_URL,
    MALWARE_TTYPE_EMAIL,
    MALWARE_TTYPE_PHONE,
    MALWARE_TTYPE_TEXT,
    MALWARE_TTYPE_JAVA,
    MALWARE_TTYPE_MULTIPLE,
    MALWARE_TTYPE_JAVAS,
    MALWARE_TTYPE_COMPRESS,
    MALWARE_TTYPE_MAX
};

typedef int (*PFScan)(void *pPrivate, int nReason, void *pParam);

/**
 * Test case information data
 */
typedef struct TestCase_struct
{
    char szAPIName[MAX_TCS_API_NAME_LEN]; /* TCS API names */
    int iInfected; /* Index of infected malware passed in pfCallback. */
    int iTestType; /* Sample type. */
    int iPolarity; /* INFECTED_DATA - Infected, BENIGN_DATA - Benign
                      otherwise undefined. */
    int iAction;
    int iCompressFlag;
    int iNotImplTestFlag; /* 1 - repair not implemented test flag, otherwise not. */
    int *pFlags;
    PFScan pfCallback;

    pthread_mutex_t MutexCon; /* Concurrency test communication stuff. */
    pthread_cond_t CondCon; /* Concurrency test communication stuff. */
} TestCase;

/**
 * Concurrency test data
 */
typedef struct ConTestContext_struct
{
    TestCase *pTestCtx;
    int iCid; /* Concurrency test id. */

    /* Report concurrency test status. 1 - success, -1 - failure, 0 - running. */
    int iConTestRet;
} ConTestContext;


/*
 * Very simple/thin porting layer
 *
 * Test malware informations. Malware names to be tested should be
 * prepared in compiling time. Please see the porting guide for test
 * suite.
 */
extern const char *SampleGetMalName(int iType, int iIndex);
extern const char *SampleGetVarName(int iType, int iIndex);
extern const char *SampleGetBenignFileName(int iType);
extern const char *SampleGetInfectedFileName(int iType);
extern unsigned int SampleGetSeverity(int iType, int iIndex);
extern unsigned int SampleGetBehavior(int iType, int iIndex);
extern unsigned int SampleGetMalType(int iType, int iIndex);
extern int SampleGetCount(int iType);

/* Test framework */
extern int CbScanCallback(void *pPrivate, int nReason, void *pParam);
extern int CbAbortCallback(void *pPrivate, int nReason, void *pParam);
extern void TestCaseCtor(TestCase *pCtx, const char *pszAPI, int iTestType,
                         int iPolarity, int iAction, PFScan pfCallback);
extern void TestCaseDtor(TestCase *pCtx);
extern void TestScanFile(const char *pszFunc, int iTType, int iPolarity,
                         int iAction);
extern void TestScanData(const char *pszFunc, int iTType, int iPolarity,
                         int iAction, PFScan pfCallback);
extern void TestScanFileEx(const char *pszFunc, int iTType, int iPolarity,
                           int iAction, int iCompressFlag);
extern void TestScanDataEx(const char *pszFunc, int iTType, int iPolarity,
                           int iAction, int iCompressFlag, PFScan pfCallback);
extern void ConScanFile(TestCase *pCtx, int iAction);
extern void ConScanData(TestCase *pCtx, int iAction);
extern int DetectRepairFunc(void);
extern int CreateTestDirs(void);
extern void DestoryTestDirs(void);
extern int DetectStubLibrary(void);
extern int IsStubTest();
extern int IsTestRepair();
extern void RestoreEngine();
extern void BackupEngine();

extern jmp_buf JmpBuf;

#ifdef __cplusplus 
}
#endif

#endif /* TCSTEST_H */
