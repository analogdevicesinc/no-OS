/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADI API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_common_log.c
* \brief Contains ADI Transceiver Logging functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*        
* ADI common lib Version: 0.0.2.1
*/

/* Intermediate platform HAL layer maintained by Analog Devices */
#include "adi_common_log.h"
#include "adi_common_hal.h"
#include "adi_platform.h"
#include "adi_common_error_types.h"
#include "adi_common_types.h"

#define ADI_FILE    ADI_COMMON_FILE_LOGGING

ADI_API void adi_common_LogWrite(   const adi_common_Device_t* const    commonDev,
                                    const adi_hal_LogLevel_e            logLevel,
                                    const char*                         comment,
                                    ...)
{
    va_list argp;
    uint8_t indent = 0U;

    if ((NULL != commonDev) &&
        (NULL != commonDev->devHalInfo))
    {
        if (commonDev->publicCnt > 1U)
        {
            indent = ADI_LOG_INDENT;
        }

        ADI_LIBRARY_VA_START(argp, comment);
        /* ADI Logging Feature Reports Errors via own HAL Data Structure */
        (void) adi_hal_LogWrite(commonDev->devHalInfo, logLevel, indent, comment, argp);
        ADI_LIBRARY_VA_END(argp);
    }
}

ADI_API void adi_common_LogLevelSet(const adi_common_Device_t* const    commonDev,
                                    const uint32_t                      logMask)
{
    if ((NULL != commonDev) &&
        (NULL != commonDev->devHalInfo))
    {
        /* ADI Logging Feature Reports Errors via own HAL Data Structure */
        (void) adi_hal_LogLevelSet(commonDev->devHalInfo, logMask);

        if (logMask == 0U)
        {
            ADI_COMMON_DEVICE_STATE_TC_SET(*(adi_common_Device_t*)commonDev);
        }
        else
        {
            ADI_COMMON_DEVICE_STATE_TC_CLR(*(adi_common_Device_t*)commonDev);
        }
    }
}

ADI_API void adi_common_LogLevelGet(const adi_common_Device_t* const    commonDev,
                                    uint32_t* const                     logMask)
{
    if ((NULL != commonDev)                 &&
        (NULL != commonDev->devHalInfo)     &&
        (NULL != logMask))
    {
        /* ADI Logging Feature Reports Errors via own HAL Data Structure */
        (void) adi_hal_LogLevelGet(commonDev->devHalInfo, logMask);
    }
}

ADI_API adi_common_ErrAction_e adi_common_LogStatusGet( const adi_common_Device_t* const    commonDev,
                                                        adi_hal_LogStatusGet_t* const       logStatus)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    ADI_NULL_PTR_RETURN(commonDev->devHalInfo);

    ADI_NULL_PTR_RETURN(logStatus);

    (void) ADI_LIBRARY_MEMSET(logStatus, 0, sizeof(adi_hal_LogStatusGet_t));

    if (ADI_HAL_ERR_OK == adi_hal_LogStatusGet(commonDev->devHalInfo, logStatus))
    {
        recoveryAction = ADI_COMMON_ERR_ACT_NONE;
    }

    return recoveryAction;
}

ADI_API void adi_common_LogConsoleSet(const adi_common_Device_t* const commonDev, adi_hal_LogConsole_e logConsoleFlag)
{
    if ((NULL != commonDev)                 &&
        (NULL != commonDev->devHalInfo))
    {
        /* ADI Logging Feature Reports Errors via own HAL Data Structure */
        (void) adi_hal_LogConsoleSet(commonDev->devHalInfo, logConsoleFlag);
    }
}

ADI_API void adi_common_LogFileOpen(const adi_common_Device_t* const    commonDev,
                                    const char* const                   fileName)
{
    if ((NULL != commonDev)                 &&
        (NULL != commonDev->devHalInfo)     &&
        (NULL != fileName))
    {
        /* ADI Logging Feature Reports Errors via own HAL Data Structure */
        (void) adi_hal_LogFileOpen(commonDev->devHalInfo, fileName);
    }
}

ADI_API void adi_common_LogFileClose(const adi_common_Device_t* const commonDev)
{
    if ((NULL != commonDev) &&
        (NULL != commonDev->devHalInfo))
    {
        /* ADI Logging Feature Reports Errors via own HAL Data Structure */
        (void) adi_hal_LogFileClose(commonDev->devHalInfo);
    }
}

ADI_API adi_common_ErrAction_e adi_common_ErrLog(   const adi_common_Device_t* const    commonDev,
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

    adi_common_ErrData_t localSave;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    ADI_LIBRARY_MEMSET(&localSave, 0, sizeof(adi_common_ErrData_t));

    /* All Log Writing Errors will be passed to the Log Data Structure */

    /* Parse Data for Logging */
    localSave.stackTrace->action = action;

    /* Device Information */
    localSave.errDataInfo.errDeviceInfo.type       = commonDev->deviceInfo.type;
    localSave.errDataInfo.errDeviceInfo.id         = commonDev->deviceInfo.id;
    localSave.errDataInfo.errDeviceInfo.name       = commonDev->deviceInfo.name;

    /* Error Event */
    localSave.stackTrace->errInfo.errCode           = errCode;
    localSave.stackTrace->errInfo.errMsg            = errMsg;
    localSave.stackTrace->errInfo.errSrc            = errSource;

    /* File Info */
    localSave.stackTrace->fileInfo.line             = lineNum;
    localSave.stackTrace->fileInfo.function         = funcName;
    localSave.stackTrace->fileInfo.file             = fileName;

    /* Variable Information */
    localSave.stackTrace->varInfo.varName           = varName;
    localSave.stackTrace->varInfo.varData           = varData;

    /* Debug Information */
    localSave.errDebugInfo.highestPriorityAction    = commonDev->errPtr->errDebugInfo.highestPriorityAction;

    ADI_ERROR_LOG(commonDev, localSave);

    recoveryAction = ADI_COMMON_ERR_ACT_NONE;

    return recoveryAction;
}

