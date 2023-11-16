/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_common_error.h
 * \brief Contains Common API error handling function prototypes and macros
 * that will be used by the API and will call functions in adi_common_error.c
 *
 * These functions are public to the customer for getting more details on
 * errors and debugging.
 *
* ADI common lib Version: 0.0.2.1
 */

#ifndef _ADI_COMMON_ERROR_H_
#define _ADI_COMMON_ERROR_H_

#include "adi_common_types.h"
#include "adi_common_macros.h"
#include "adi_common_user.h"
#include "adi_common_error_types.h"
#include "adi_common_log.h"


/*
* *******************************
* ADI Common error macros
* *******************************
*/

/*
* \brief    Macro to check for Null Device & Error Pointer
* 
* \param    devicePtr Device Pointer
*/
#define ADI_NULL_DEVICE_PTR_RETURN(devicePtr)   \
if (devicePtr == NULL)                          \
{                                               \
    return ADI_COMMON_ERR_ACT_CHECK_PARAM;      \
}

/*
* \brief    Macro to check for Null Common Device & Error Pointer
* 
* \param    commonDev Common Device Pointer, which also contains the Error Pointer
*/
#define ADI_NULL_COMMON_PTR_RETURN(commonDev)   \
if ((commonDev          == NULL) ||             \
    (commonDev->errPtr  == NULL))               \
{                                               \
    return ADI_COMMON_ERR_ACT_CHECK_PARAM;      \
}

/*
* \brief    Macro to check for Null Pointer
* 
* \param    ptr Pointer to be checked
*/
#define ADI_NULL_PTR_RETURN(ptr)            \
if (ptr == NULL)                            \
{                                           \
    return ADI_COMMON_ERR_ACT_CHECK_PARAM;  \
}
/*
* \brief    Macro to perform the following:
*                       1) Null Pointer Check
*                       2) Report Null Pointer to Error Memory
*                       3) Return ADI_COMMON_ERR_ACT_CHECK_PARAM from API

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param ptr        Pointer to be checked
* 
*/
#define ADI_NULL_PTR_REPORT_RETURN(commonDev, ptr)                  \
{                                                                   \
    if (ptr == NULL)                                                \
    {                                                               \
        ADI_ERROR_REPORT(   commonDev,                              \
                            ADI_COMMON_ERRSRC_API,                  \
                            ADI_COMMON_ERRCODE_NULL_PTR,            \
                            ADI_COMMON_ERR_ACT_CHECK_PARAM,         \
                            ptr,                                    \
                            "Null Pointer");                        \
                                                                    \
        return ADI_COMMON_ERR_ACT_CHECK_PARAM;                      \
    }                                                               \
}

/*
* \brief    Macro to perform the following:
*                       1) Null Pointer Check
*                       2) Report Null Pointer to Error Memory
*                       3) GOTO label specified

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param ptr        Pointer to be checked
* \param label      Label for GOTO
* 
*/
#define ADI_NULL_PTR_REPORT_GOTO(commonDev, ptr, label)             \
{                                                                   \
    if (ptr == NULL)                                                \
    {                                                               \
        ADI_ERROR_REPORT(   commonDev,                              \
                            ADI_COMMON_ERRSRC_API,                  \
                            ADI_COMMON_ERRCODE_NULL_PTR,            \
                            ADI_COMMON_ERR_ACT_CHECK_PARAM,         \
                            ptr,                                    \
                            "Null Pointer");                        \
                                                                    \
        goto label;                                                 \
    }                                                               \
}

#if ADI_COMMON_VERBOSE > 0
    #if ADI_COMMON_VARIABLE_USAGE > 0
        /*
        * Macro to print variable name as a string, used in Error reporting facility
        */
        #ifndef GET_VARIABLE_NAME
            #define GET_VARIABLE_NAME(Variable) (#Variable)
        #endif /* GET_VARIABLE_NAME(Variable) (#Variable) */
    #else
        #ifndef GET_VARIABLE_NAME
            #define GET_VARIABLE_NAME(Variable) (ADI_NO_ERROR_MESSAGE)
        #endif /* GET_VARIABLE_NAME(Variable) (ADI_NO_ERROR_MESSAGE)*/
    #endif /* ADI_COMMON_VARIABLE_USAGE */

    /**
    * \brief    Macro to be used for strings in code (Can be removed via ADI_COMMON_VERBOSE)
    *
    * \param    string Error String
    * 
    */
    #define ADI_STRING(string)  string

#else   /* ADI_COMMON_VERBOSE OFF */
    #define ADI_STRING(string)  ADI_VERBOSE_OFF_MESSAGE;
#endif /* ADI_COMMON_VERBOSE */

/*
* \brief Macro to perform Error Reporting to Memory & Error Logging to File

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param errSource  Source of Error
* \param errAction  Recovery Action to be taken from the current API
* \param errCode    Error Code to be Logged
* \param variable   Variable Name & Data that failed the parameter check, if applicable
*                       NOTE1: Explicit Type Conversion to int64_t for storage in Error Structure
*                       NOTE2: Passing of Pointers/String Literals will store Memory Address
*                       NOTE3: Capturing Entire Data Structures Not Supported
*                       NOTE4: Set to ADI_NO_VARIABLE if there is no variable related to the error
* \param errMsg     Error Message
*/
#define ADI_ERROR_REPORT(           commonDev,                                  \
                                    errSource,                                  \
                                    errCode,                                    \
                                    recoveryAction,                             \
                                    variable,                                   \
                                    errMsg)                                     \
                                                                                \
     (void)adi_common_ErrReport (   commonDev,                                  \
                                    (uint32_t) errSource,                           /* Explicit Type Conversion; errSource stored as uint32_t */    \
                                    (adi_common_ErrAction_e) recoveryAction,        /* Explicit Type Conversion;recoveryAction stored as adi_common_ErrAction_e */  \
                                    (int64_t) errCode,                              /* Explicit Type Conversion; errCode stored as int64_t */   \
                                    __LINE__,                                   \
                                    __func__,                               \
                                    (uint32_t) ADI_FILE,                            /* Explicit Type Conversion; ADI_FILE stored as uint32_t */ \
                                    ADI_STRING(GET_VARIABLE_NAME(variable)),    \
                                    ADI_COMMON_ERROR_VARIABLE_TYPE variable,        /* Explicit Type Conversion; variable stored as int64_t via ADI_COMMON_ERROR_VARIABLE_TYPE */ \
                                    ADI_STRING(errMsg));                        \
                                                                                \
        (void)adi_common_ErrLog (   commonDev,                                  \
                                    (uint32_t) errSource,                           /* Explicit Type Conversion; errSource stored as uint32_t */    \
                                    (adi_common_ErrAction_e) recoveryAction,        /* Explicit Type Conversion;recoveryAction stored as adi_common_ErrAction_e */  \
                                    (int64_t) errCode,                              /* Explicit Type Conversion; errCode stored as int64_t */   \
                                    __LINE__,                                   \
                                    __func__,                               \
                                    (uint32_t) ADI_FILE,                            /* Explicit Type Conversion; ADI_FILE stored as uint32_t */ \
                                    ADI_STRING(GET_VARIABLE_NAME(variable)),    \
                                    ADI_COMMON_ERROR_VARIABLE_TYPE variable,        /* Explicit Type Conversion; variable stored as int64_t via ADI_COMMON_ERROR_VARIABLE_TYPE */ \
                                    ADI_STRING(errMsg))
/*
* \brief Macro to perform Invalid Parameter Error Reporting to Memory & Error Logging to File

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param errAction  Recovery Action to be taken from the current API
* \param variable   Variable Name & Data that failed the parameter check, if applicable
*                       NOTE1: Passing of Pointers will store Memory Address
*                       NOTE2: Capturing Entire Data Structures Not Supported
* \param errMsg     Error Message
*/
#define ADI_PARAM_ERROR_REPORT(     commonDev,                                  \
                                    recoveryAction,                             \
                                    variable,                                   \
                                    errMsg)                                     \
                                                                                \
                ADI_ERROR_REPORT(   commonDev,                                  \
                                    ADI_COMMON_ERRSRC_API,                      \
                                    ADI_COMMON_ERRCODE_INVALID_PARAM,           \
                                    recoveryAction,                             \
                                    variable,                                   \
                                    errMsg);

/*
* \brief Macro to perform API Error Reporting to Memory & Error Logging to File

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param errAction  Recovery Action to be taken from the current API
* \param errMsg     Error Message
*/
#define ADI_API_ERROR_REPORT(       commonDev,                                  \
                                    recoveryAction,                             \
                                    errMsg)                                     \
                                                                                \
                ADI_ERROR_REPORT(   commonDev,                                  \
                                    ADI_COMMON_ERRSRC_API,                      \
                                    ADI_COMMON_ERRCODE_API,                     \
                                    recoveryAction,                             \
                                    ADI_NO_VARIABLE,                            \
                                    errMsg)


/*
* \brief    Macro to perform the following:
*                       1) Write Message to Standard Output
*/
#define ADI_APP_MESSAGE(...)       ADI_LIBRARY_PRINTF(__VA_ARGS__);


/*
* \brief    Macro to perform the following:
*                       1) Write Error Data to Standard Output
*/
#define ADI_APP_ERROR_OUTPUT(errPtr)    adi_common_ErrStandardOutputWrite(errPtr);

/*
* \brief Macro to perform Application Level Error Reporting to Memory & Error Logging to File

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param errCode    Error Code (Mapped to ADI_COMMON_ERRSRC_APP)
* \param errAction  Recovery Action to be taken from the current API
* \param variable   Variable Name & Data that failed the parameter check, if applicable
*                       NOTE1: Passing of Pointers will store Memory Address
*                       NOTE2: Capturing Entire Data Structures Not Supported
* \param errMsg     Error Message
*/
#define ADI_APP_ERROR_REPORT(       errCode,                                    \
                                    recoveryAction,                             \
                                    variable,                                   \
                                    errMsg)                                     \
                                                                                \
{                                                                               \
    adi_common_Device_t commonDev;                                              \
    ADI_LIBRARY_MEMSET(&commonDev, 0, sizeof(adi_common_Device_t));             \
                                                                                \
    commonDev.errPtr = (adi_common_ErrData_t*) adi_hal_TlsGet(HAL_TLS_ERR);     \
                                                                                \
                ADI_ERROR_REPORT(   &commonDev,                                 \
                                    ADI_COMMON_ERRSRC_APP,                      \
                                    errCode,                                    \
                                    recoveryAction,                             \
                                    variable,                                   \
                                    errMsg);                                    \
}

/*
* \brief Macro to perform API Error Reporting to Memory & Error Logging to File

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param errAction  Recovery Action to be taken from the current API
* \param errMsg     Error Message
*/
#define ADI_EXT_ERROR_REPORT(       commonDev,                                  \
                                    errCause,                                   \
                                    errActionMsg)                               \
                                                                                \
                                (void) adi_common_ErrMsgReport( commonDev,      \
                                                                errCause,       \
                                                                errActionMsg)

/*
* \brief    Macro to perform the following:
*                       1) Report Feature Not Implemented
*                       2) Return Common RecoveryAction (i.e. ADI_COMMON_ERR_ACT_CHECK_FEATURE)
*/
#define ADI_API_NOT_IMPLEMENTED_REPORT_RETURN(  commonDev)                              \
                                                                                        \
                ADI_ERROR_REPORT(   commonDev,                                          \
                                    ADI_COMMON_ERRSRC_API,                              \
                                    ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED,         \
                                    ADI_COMMON_ERR_ACT_CHECK_FEATURE,                   \
                                    ADI_NO_VARIABLE,                                    \
                                    "Feature not implemented");                         \
                return ADI_COMMON_ERR_ACT_CHECK_FEATURE;

/*
* \brief    Macro to perform the following:
*                       1) Report Feature Not Implemented
*                       2) GOTO label specified
*/
#define ADI_API_NOT_IMPLEMENTED_REPORT_GOTO(  commonDev, label)                         \
                                                                                        \
                ADI_ERROR_REPORT(   commonDev,                                          \
                                    ADI_COMMON_ERRSRC_API,                              \
                                    ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED,         \
                                    ADI_COMMON_ERR_ACT_CHECK_FEATURE,                   \
                                    ADI_NO_VARIABLE,                                    \
                                    "Feature not implemented");                         \
                goto label;

/*
* \brief Macro to Create an Error Structure
*
* \param errDataPtr Pointer to Error Structure
*/
#define ADI_CREATE_ERROR_MEMORY()                               adi_common_ErrCreate()


/*
* \brief Macro to Assign Error Pointer to Device Structure
*
* \param commonDev  Pointer to Common Device
* \param errDataPtr Pointer to Error Structure
*/
#define ADI_ASSIGN_ERROR_MEMORY(commonDev, errDataPtr)          adi_common_ErrAssign(commonDev, errDataPtr)

/*
* \brief Macro to Destroy an Error Structure
*
* \param errDataPtr Pointer to Error Structure
*/
#define ADI_DESTROY_ERROR_MEMORY(errDataPtr)                    adi_common_ErrDestroy(errDataPtr)

/*
* \brief Macro to clear an Error Structure
*
* \param errDataPtr Pointer to Error Structure
*/
#define ADI_CLEAR_ERROR_MEMORY(errDataPtr)                      adi_common_ErrClear(errDataPtr)

/*
* \brief Macro to Get Error Frame from Stack Trace
*
* \param commonDev  Pointer to Common Device
* \param frameId    Frame ID
* \param errDataPtr Pointer to Error Frame
*/
#define ADI_ERROR_FRAME_GET(commonDev, frameId, framePtr)       adi_common_ErrFrameGet(commonDev, frameId, framePtr)

/*
* *******************************
* ADI Common error functions
* *******************************
*/

/*
 * \brief   Service to Report an Error
 *
 * \param[in] commonDev         pointer to adi_common_Device_t
 * \param[in] actionRequired    RecoveryAction to be taken
 * \param[in] lineNum           line number of where the error was detected
 * \param[in] funcName          function name of the function where the error was detected
 * \param[in] fileName          file name of the file where the error was detected
 * \param[in] detErr            Detected Error Code
 * \param[in] errCodeMsg        Error Message
 * \param[in] varName           Variable name to be captured
 * \param[in] varData           Variable Data will be captured (Limitations Apply)
 * \param[in] errMsg            User Readable Error Message
 */
ADI_API adi_common_ErrAction_e adi_common_ErrReport(const adi_common_Device_t* const    commonDev,
                                                    const uint32_t                      errSource,
                                                    const adi_common_ErrAction_e        action,
                                                    const int64_t                       errCode,
                                                    const uint32_t                      lineNum,
                                                    const char*                         funcName,
                                                    const uint32_t                      fileName,
                                                    const char*                         varName,
                                                    const int64_t                       varData,
                                                    const char*                         errMsg);

/*
 * \brief   Service to Report Additional Error Messages
 *
 * \param[in] commonDev         pointer to adi_common_Device_t
 * \param[in] errCausePtr       Error Cause Message
 * \param[in] errActionMsgPtr   Recovery Action Message
 */
ADI_API adi_common_ErrAction_e adi_common_ErrMsgReport( const adi_common_Device_t* const    commonDev,
                                                        const char*                         errCausePtr,
                                                        const char*                         errActionMsgPtr);

/*
* \brief   Service to Write Error to Standard Output
*
* \param[in] errData   pointer to adi_common_Device_t Error Data
*
*/
ADI_API void adi_common_ErrStandardOutputWrite(const adi_common_ErrData_t* const errData);

/*
 * \brief   Create Error Structure in Memory
 * 
 * \retval adi_common_ErrData_t* Non NULL Pointer to Error Structure is created
 */
ADI_API adi_common_ErrData_t* adi_common_ErrCreate(void);

/*
 * \brief   Assign Error Structure in Memory to Common Device Structure
 *
 * \param[in] commonDev     pointer to adi_common_Device_t
 * \param[in] errDataPtr    pointer to adi_common_ErrData_t
 * 
 * \retval  adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if successfully assigned
 */
ADI_API adi_common_ErrAction_e adi_common_ErrAssign(adi_common_Device_t* const  commonDev,
                                                    adi_common_ErrData_t* const errDataPtr);

/*
 * \brief   Service to Parse an Stack Trace Frame from the Error Structure
 * 
 * \param[in]   commonDev   pointer to adi_common_Device_t
 * \param[in]   frameId     Frame ID to Parse
 * \param[out]  errFrame    pointer to adi_common_ErrFrame_t that will be populated
 *
 * \retval  adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if successfully parsed
 */
ADI_API adi_common_ErrAction_e adi_common_ErrFrameGet(  const adi_common_Device_t* const    commonDev,
                                                        const adi_common_ErrFrameId_e       frameId,
                                                        adi_common_ErrFrame_t* const        errFrame);

/*
 * \brief   Service to Parse a Common Error from a Lookup Table
 * 
 * \param[in]   errCode     Error Code to be Looked Up
 * \param[out]  errMsgPtr   Related Error Message
 * \param[out]  action      Related Recovery Action
 *
 * \retval  adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if successfully parsed
 */
ADI_API adi_common_ErrAction_e adi_common_ErrorGet( const int64_t                   errCode,
                                                    const char** const              errMsgPtr,
                                                    adi_common_ErrAction_e* const   action);

/*
 * \brief   Service to Get Error Information for a given Common Error Source
 * 
 * \param[in]   commonDev   pointer to adi_common_Device_t
 * \param[in]   errSrc      Error Source to be Looked Up
 * \param[in]   errCode     Error Code to be Looked Up
 * \param[out]  errMsgPtr   Related Error Message
 * \param[out]  action      Related Recovery Action
 *
 * \retval  adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if successfully parsed
 */
ADI_API adi_common_ErrAction_e adi_common_ErrInfoGet(   const adi_common_ErrSources_e       errSrc,
                                                        const int64_t                       errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_common_ErrAction_e* const       errAction);

/**
 * \brief Destroy an Error Structure in Memory
 * 
 * \param[in] errDataPtr Pointer to adi_common_ErrData_t
 * 
 * \retval ADI_COMMON_ERR_ACT_NONE if Memory is successfully deallocated
 */
ADI_API adi_common_ErrAction_e adi_common_ErrDestroy(adi_common_ErrData_t* errDataPtr);

/**
 * \brief Clear Error Structure
 * 
 * \param[in] errDataPtr Pointer to adi_common_ErrData_t
 * 
 * \retval ADI_COMMON_ERR_ACT_NONE if Error Data is marked clear
 */
ADI_API adi_common_ErrAction_e adi_common_ErrClear(adi_common_ErrData_t* const errDataPtr);

#endif /* _ADI_COMMON_ERROR_H_ */
