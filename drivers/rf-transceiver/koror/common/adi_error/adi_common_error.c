/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_common_error.c
 * \brief Contains common API error handling functions implementations
 *
 * These functions are public to the customer for getting more details on
 * errors and debugging.
 *
 * ADI common lib Version: 0.0.2.1
 */

#include "adi_common_error.h"
#include "adi_platform_types.h"

#define  ADI_ERROR_FRAME_PRINTF(errPtr, frameIdx)   ADI_LIBRARY_PRINTF("Error Frame: %" PRIu32 "\n"                                             \
                                                                        ADI_COMMON_LOG_ERR_INDENT1 "Recovery Action: %" PRId64 "\n"             \
                                                                        ADI_COMMON_LOG_ERR_INDENT1 "Event Information:\n"                       \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "Error Code: 0x%" PRIX64"\n"                 \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "Error Source: 0x%" PRIX32 "\n"              \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "Error Message: %s\n"                        \
                                                                        ADI_COMMON_LOG_ERR_INDENT1 "File Information:\n"                        \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "Line: %" PRIu32"\n"                         \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "Function: %s\n"                             \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "File: %" PRIu32"\n"                         \
                                                                        ADI_COMMON_LOG_ERR_INDENT1 "Variable Information:\n"                    \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "Variable Name: %s\n"                        \
                                                                        ADI_COMMON_LOG_ERR_INDENT2 "Variable Data: 0x%" PRIX64 "\n\n",          \
                                                                        frameIdx,                                                               \
                                                                        (int64_t)errPtr->stackTrace[frameIdx].action,                           \
                                                                        errPtr->stackTrace[frameIdx].errInfo.errCode,                           \
                                                                        errPtr->stackTrace[frameIdx].errInfo.errSrc,                            \
                                                                        errPtr->stackTrace[frameIdx].errInfo.errMsg,                            \
                                                                        errPtr->stackTrace[frameIdx].fileInfo.line,                             \
                                                                        errPtr->stackTrace[frameIdx].fileInfo.function,                         \
                                                                        errPtr->stackTrace[frameIdx].fileInfo.file,                             \
                                                                        errPtr->stackTrace[frameIdx].varInfo.varName,                           \
                                                                        errPtr->stackTrace[frameIdx].varInfo.varData);

ADI_API adi_common_ErrAction_e adi_common_ErrReport(const adi_common_Device_t* const    commonDev,
                                                    const uint32_t                      errSource,
                                                    const adi_common_ErrAction_e        action,
                                                    const int64_t                       errCode,
                                                    const uint32_t                      lineNum,
                                                    const char*                         funcName,
                                                    const uint32_t                      fileName,
                                                    const char*                         varName,
                                                    const int64_t                       varData,
                                                    const char*                         errMsg)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    ADI_NULL_PTR_RETURN(commonDev->errPtr);

    /* Check if Trace Buffer is Full */
    if (ADI_COMMON_STACK_TRACE_SIZE > commonDev->errPtr->errDataInfo.stackIdx)
    {
        /* Recovery Action */
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].action   = action;
        /* Error Event Info */
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].errInfo.errCode    = errCode;
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].errInfo.errMsg     = errMsg;
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].errInfo.errSrc     = errSource;
        /* File Info */
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].fileInfo.line      = lineNum;
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].fileInfo.function  = funcName;
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].fileInfo.file      = fileName;
        /* Variable Information */
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].varInfo.varName    = varName;
        commonDev->errPtr->stackTrace[commonDev->errPtr->errDataInfo.stackIdx].varInfo.varData    = varData;
        /* Check Error Priority for Highest Recorded Action in Call Stack */
        if (action < commonDev->errPtr->errDebugInfo.highestPriorityAction)
        {
            commonDev->errPtr->errDebugInfo.highestPriorityAction = action;
        }

        if (commonDev->errPtr->errDataInfo.stackIdx == 0U)
        {
            /* Copy Device Info @ 1st Error Frame */
            commonDev->errPtr->errDataInfo.errDeviceInfo.type   = commonDev->deviceInfo.type;
            commonDev->errPtr->errDataInfo.errDeviceInfo.id     = commonDev->deviceInfo.id;
            commonDev->errPtr->errDataInfo.errDeviceInfo.name   = commonDev->deviceInfo.name;
        }

        /* Update StackTrace Index for Next Entry */
        ++commonDev->errPtr->errDataInfo.stackIdx;
        recoveryAction = ADI_COMMON_ERR_ACT_NONE;
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_ErrMsgReport( const adi_common_Device_t* const    commonDev,
                                                        const char*                         errCausePtr,
                                                        const char*                         errActionMsgPtr)
{
    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    ADI_NULL_PTR_RETURN(commonDev->errPtr);

    /* Report First Error Cause & Action Message */
    if ((NULL == commonDev->errPtr->errDebugInfo.errCause) &&
        (NULL == commonDev->errPtr->errDebugInfo.actionMsg))
    {
        commonDev->errPtr->errDebugInfo.errCause    = errCausePtr;
        commonDev->errPtr->errDebugInfo.actionMsg   = errActionMsgPtr;
    }

    return ADI_COMMON_ERR_ACT_NONE;
}

ADI_API void adi_common_ErrStandardOutputWrite(const adi_common_ErrData_t* const errData)
{
    uint32_t idx = 0U;

    if (errData != NULL)
    {
        if (errData->errDataInfo.stackIdx > 0)
        {
            for (idx = 0U; idx < errData->errDataInfo.stackIdx; ++idx)
            {
                ADI_ERROR_FRAME_PRINTF(errData, idx)
            }
        }
    }
}


ADI_API adi_common_ErrData_t* adi_common_ErrCreate(void)
{
    adi_common_ErrData_t* errDataPtr = (adi_common_ErrData_t*)ADI_LIBRARY_CALLOC(1, sizeof(adi_common_ErrData_t));

    if (NULL == errDataPtr)
    {
        return NULL;
    }

    return errDataPtr;
}

ADI_API adi_common_ErrAction_e adi_common_ErrAssign(adi_common_Device_t* const  commonDev,
                                                    adi_common_ErrData_t* const errDataPtr)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    if ((NULL != commonDev) &&
        (NULL != errDataPtr))
    {
        commonDev->errPtr   = errDataPtr;
        recoveryAction      = ADI_COMMON_ERR_ACT_NONE;
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_ErrDestroy(adi_common_ErrData_t* errDataPtr)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    /* Validate Argument(s) */
    ADI_NULL_PTR_RETURN(errDataPtr);

    /* Free Memory */
    ADI_LIBRARY_FREE(errDataPtr);
    errDataPtr = NULL;

    if (NULL == errDataPtr)
    {
        recoveryAction = ADI_COMMON_ERR_ACT_NONE;
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_ErrClear(adi_common_ErrData_t* const errDataPtr)
{
    uint32_t idx = 0U;

    /* Validate Argument(s) */
    ADI_NULL_PTR_RETURN(errDataPtr);

    for (idx = 0U; idx < ADI_COMMON_STACK_TRACE_SIZE; ++idx)
    {
        errDataPtr->stackTrace[idx].action              = ADI_COMMON_ERR_ACT_NONE;
        errDataPtr->stackTrace[idx].errInfo.errCode     = 0;
        errDataPtr->stackTrace[idx].errInfo.errMsg      = NULL;
        errDataPtr->stackTrace[idx].errInfo.errSrc      = 0U;
        errDataPtr->stackTrace[idx].fileInfo.line       = 0U;
        errDataPtr->stackTrace[idx].fileInfo.function   = NULL;
        errDataPtr->stackTrace[idx].fileInfo.file       = 0U;
        errDataPtr->stackTrace[idx].varInfo.varName     = NULL;
        errDataPtr->stackTrace[idx].varInfo.varData     = 0;
    }
    /* Debug Info */
    errDataPtr->errDebugInfo.highestPriorityAction  = ADI_COMMON_ERR_ACT_NONE;
    errDataPtr->errDebugInfo.errCause               = NULL;
    errDataPtr->errDebugInfo.actionMsg              = NULL;

    /* Error Data Info */
    errDataPtr->errDataInfo.errDeviceInfo.type      = 0U;
    errDataPtr->errDataInfo.errDeviceInfo.id        = 0U;
    errDataPtr->errDataInfo.errDeviceInfo.name      = NULL;
    errDataPtr->errDataInfo.stackIdx                = 0U;

    return ADI_COMMON_ERR_ACT_NONE;
}

ADI_API adi_common_ErrAction_e adi_common_ErrFrameGet(  const adi_common_Device_t* const    commonDev,
                                                        const adi_common_ErrFrameId_e       frameId,
                                                        adi_common_ErrFrame_t* const        errFrame)
{
    uint8_t idx = 0U;

    /* Validate Argument(s) */
    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    ADI_NULL_PTR_RETURN(commonDev->errPtr);

    ADI_NULL_PTR_RETURN(errFrame);

    if (ADI_COMMON_ERRFRAME_MAX < frameId)
    {
        /* Frame Index Out of Range; ADI_COMMON_ERRFRAME_MAX / TOP is only non Index Allowed */
        return ADI_COMMON_ERR_ACT_CHECK_PARAM;
    }

    if (ADI_COMMON_ERRFRAME_API == frameId)
    {
        if (ADI_COMMON_ERRFRAME_IDX_0 != commonDev->errPtr->errDataInfo.stackIdx)
        {
            /* Stack Index is configured for the next Error Entry; We want the last entry */
            idx = (commonDev->errPtr->errDataInfo.stackIdx - 1U);
        }
    }
    else
    {
        idx = frameId;
    }

    /* Shallow Copy */
    *errFrame = commonDev->errPtr->stackTrace[idx];

    return ADI_COMMON_ERR_ACT_NONE;
}

ADI_API adi_common_ErrAction_e adi_common_ErrorGet( const int64_t                   errCode,
                                                    const char** const              errMsgPtr,
                                                    adi_common_ErrAction_e* const   action)
{
    static const adi_common_api_errors_t adi_common_api_errors[] =
    {
        { ADI_COMMON_ERRCODE_NONE,                      ADI_STRING("No Error"),                     ADI_COMMON_ERR_ACT_NONE },
        { ADI_COMMON_ERRCODE_INVALID_PARAM,             ADI_STRING("Invalid Parameter Value"),      ADI_COMMON_ERR_ACT_CHECK_PARAM },
        { ADI_COMMON_ERRCODE_NULL_PTR,                  ADI_STRING("Null Pointer"),                 ADI_COMMON_ERR_ACT_CHECK_PARAM },
        { ADI_COMMON_ERRCODE_API,                       ADI_STRING("API Call Error"),               ADI_COMMON_ERR_ACT_CHECK_PARAM },
        { ADI_COMMON_ERRCODE_ARRAY_SIZE,                ADI_STRING("Invalid Array Size"),           ADI_COMMON_ERR_ACT_CHECK_PARAM },
        { ADI_COMMON_ERRCODE_LOCK,                      ADI_STRING("Mutex (Un)Locking Problem"),    ADI_COMMON_ERR_ACT_RESET_DEVICE },
        { ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED,   ADI_STRING("Feature Not Implemented"),      ADI_COMMON_ERR_ACT_CHECK_FEATURE },
        { ADI_COMMON_ERRCODE_TIMEOUT,                   ADI_STRING("Timeout"),                      ADI_COMMON_ERR_ACT_RESET_FEATURE },
        { ADI_COMMON_ERRCODE_BOOT_TIMEOUT,              ADI_STRING("Boot Timeout"),                 ADI_COMMON_ERR_ACT_RESET_DEVICE }
    };
    
    uint32_t                idx;
    adi_common_ErrAction_e  recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    ADI_NULL_PTR_RETURN(errMsgPtr);

    ADI_NULL_PTR_RETURN(action);

    for (idx = 0; (idx < (sizeof(adi_common_api_errors) / sizeof(adi_common_api_errors_t))); ++idx)
    {
        if (errCode == adi_common_api_errors[idx].errCode)
        {
            *errMsgPtr      = adi_common_api_errors[idx].errMsg;
            *action         = adi_common_api_errors[idx].action;
            recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
            break;
        }
    }
    return recoveryAction;
}


ADI_API adi_common_ErrAction_e adi_common_ErrInfoGet(   const adi_common_ErrSources_e       errSrc,
                                                        const int64_t                       errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_common_ErrAction_e* const       errAction)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    ADI_NULL_PTR_RETURN(errMsgPtr);

    ADI_NULL_PTR_RETURN(errAction);

    /* Default Values */
    *errMsgPtr  = NULL;
    *errAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    switch (errSrc)
    {
    case ADI_COMMON_ERRSRC_API:
    case ADI_COMMON_ERRSRC_HAL:
    case ADI_COMMON_ERRSRC_DEVICEHAL:
    case ADI_COMMON_ERRSRC_DEVICEBF:
        /* All Using Same Generic Error Codes */
        recoveryAction = adi_common_ErrorGet(errCode, errMsgPtr, errAction);
        break;

    case ADI_COMMON_ERRSRC_NONE:
        /* Fall Through */

    default:
        /* Invalid Error Source; No Lookup can be performed */
        recoveryAction = ADI_COMMON_ERR_ACT_CHECK_FEATURE;
        break;
    }

    return recoveryAction;
}
