/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_common_hal.c
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*
* ADI common lib Version: 0.0.2.1
*/

#include "adi_common_hal.h"
#include "adi_common_error.h"
#include "adi_platform.h"

#define ADI_FILE    ADI_COMMON_FILE_HAL

ADI_API adi_common_ErrAction_e adi_common_hal_ErrCodeConvert(const adi_hal_Err_e halCode)
{
    adi_common_ErrAction_e action = ADI_COMMON_ERR_ACT_RESET_DEVICE;

    switch (halCode)
    {
        case ADI_HAL_ERR_OK:
            action = ADI_COMMON_ERR_ACT_NONE;
            break;

        case ADI_HAL_ERR_I2C:
        case ADI_HAL_ERR_I2C_WRITE:
        case ADI_HAL_ERR_I2C_READ:
        case ADI_HAL_ERR_SPI:
        case ADI_HAL_ERR_SPI_WRITE:
        case ADI_HAL_ERR_SPI_READ:
        case ADI_HAL_ERR_GPIO:
            /* Fall Through */
            action = ADI_COMMON_ERR_ACT_CHECK_INTERFACE;
            break;

        case ADI_HAL_ERR_SPI_CS:
            action = ADI_COMMON_ERR_ACT_RESET_INTERFACE;
            break;

        case ADI_HAL_ERR_BBICCTRL:
        case ADI_HAL_ERR_BBICCTRL_CORE:
        case ADI_HAL_ERR_BBICCTRL_RAM:
        case ADI_HAL_ERR_BBICCTRL_SPI:
            action = ADI_COMMON_ERR_ACT_RESET_FEATURE;
            break;

        case ADI_HAL_ERR_PARAM:
        case ADI_HAL_ERR_NULL_PTR:
            /* Fall Through */
            action = ADI_COMMON_ERR_ACT_CHECK_PARAM;
            break;

        case ADI_HAL_ERR_TIMER:
        case ADI_HAL_ERR_NOT_IMPLEMENTED:
        case ADI_HAL_ERR_LOG:
        case ADI_HAL_ERR_MEMORY:
        case ADI_HAL_ERR_LIBRARY:
        case ADI_HAL_ERR_EEPROM_DATA:
            /* Fall Through */

        default:
            /* Operating System Error Code */
            action = ADI_COMMON_ERR_ACT_CHECK_FEATURE;
            break;
    }

    return action;
}

ADI_API adi_common_ErrAction_e adi_common_hal_HwOpen(adi_common_Device_t* const commonDev)
{
    adi_common_ErrAction_e  recoveryAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;

    ADI_NULL_PTR_RETURN(commonDev);

    halError = adi_hal_MutexInit(&commonDev->mutex);
    if (ADI_HAL_ERR_OK != halError)
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            halError,
                            "Device Mutex Cannot Be Initialized");
        return recoveryAction;
    }

    halError = adi_hal_HwOpen(commonDev->devHalInfo);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            halError,
                            "HAL Hardware Open Failed");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_HwClose(adi_common_Device_t* const commonDev)
{
    adi_common_ErrAction_e  recoveryAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    halError = adi_hal_HwClose(commonDev->devHalInfo);
    if (ADI_HAL_ERR_OK != halError)
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            halError,
                            "HAL Hardware Close Failed");
        return recoveryAction;
    }

    halError = adi_hal_MutexDestroy(&commonDev->mutex);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_PARAM_ERROR_REPORT(commonDev, recoveryAction, &commonDev->publicCnt, "Public Count");
        ADI_PARAM_ERROR_REPORT(commonDev, recoveryAction, &commonDev->lockCnt, "Lock Count");
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            halError,
                            "Device Mutex Cannot Be Destroyed");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_HwReset(const adi_common_Device_t* const commonDev, const uint8_t pinLevel)
{    
    adi_common_ErrAction_e  recoveryAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    halError = adi_hal_HwReset(commonDev->devHalInfo, pinLevel);

    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            pinLevel,
                            "HAL HwReset Level Set Issue");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_Wait_us(const adi_common_Device_t* const commonDev, const uint32_t time_us)
{
    adi_common_ErrAction_e  recoveryAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    halError = adi_hal_Wait_us(commonDev->devHalInfo, time_us);

    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            halError,
                            "HAL Wait(us) Failed");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_Wait_ms(const adi_common_Device_t* const commonDev, const uint32_t time_ms)
{
    adi_common_ErrAction_e  recoveryAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    halError = adi_hal_Wait_us(commonDev->devHalInfo, (time_ms * 1000));

    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            halError,
                            "HAL Wait(ms) Failed");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_Lock(adi_common_Device_t* const commonDev)
{
    adi_common_ErrAction_e  recoveryAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;

    /* Use NULL_DEVICE as NULL_COMMON_PTR also checks common->errPtr which is not set yet */
    ADI_NULL_DEVICE_PTR_RETURN(commonDev);

    halError = adi_hal_MutexLock(&commonDev->mutex);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_LOCK,
                            recoveryAction,
                            halError,
                            "HAL Mutex Lock Failed");
    }

    ++commonDev->lockCnt;
    if (commonDev->lockCnt == 0U)
    {
        recoveryAction = ADI_COMMON_ERR_ACT_RESET_DEVICE;
        /* Range of lockCnt type exceeded. Do not check unlock return */
        (void) adi_hal_MutexUnlock(&commonDev->mutex);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_COMMON_ERRCODE_LOCK,
                            recoveryAction,
                            ADI_NO_VARIABLE,
                            "Lock Count Overflow");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_Unlock(adi_common_Device_t* const commonDev)
{
    adi_common_ErrAction_e  recoveryAction  = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    if (commonDev->lockCnt == 0U)
    {
        recoveryAction = ADI_COMMON_ERR_ACT_RESET_DEVICE;
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERRCODE_LOCK,
                            recoveryAction,
                            commonDev->lockCnt,
                            "Lock Never Used");
        return recoveryAction;
    }
    --commonDev->lockCnt;

    halError = adi_hal_MutexUnlock(&commonDev->mutex);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        ++commonDev->lockCnt;
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERRCODE_LOCK,
                            recoveryAction,
                            halError,
                            "Unlock Failed");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_ApiEnter_vLogCtl(adi_common_Device_t* const  commonDev,
                                                               const char* const           fnName,
                                                               const uint32_t              doLocking,
                                                               const adi_common_LogCtl_e   logCtl)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    /* Device Not Locked & Error Pointer is Not Set in Common Device Structure */

    ADI_NULL_DEVICE_PTR_RETURN(commonDev);

    ADI_NULL_PTR_REPORT_RETURN(commonDev, fnName);

    switch (doLocking)
    {
        case ADI_TRUE:
            /* Initialized Device Mutex Required for Locking */
            if (ADI_FALSE == ADI_COMMON_DEVICE_STATE_IS_OPEN(*commonDev))
            {
                recoveryAction = ADI_COMMON_ERR_ACT_OPEN_DEVICE;

                if ( NULL != adi_hal_TlsGet(HAL_TLS_ERR))
                {
                    /* Bypass Common Device Structure for Error Reporting if TLS has Error Pointer */
                    adi_common_Device_t dummyDev;

                    ADI_LIBRARY_MEMSET(&dummyDev, 0, sizeof(adi_common_Device_t));

                    dummyDev.errPtr = (adi_common_ErrData_t*) adi_hal_TlsGet(HAL_TLS_ERR);

                    ADI_ERROR_REPORT(   &dummyDev,
                                        ADI_COMMON_ERRSRC_API,
                                        ADI_COMMON_ERRCODE_INVALID_PARAM,
                                        recoveryAction,
                                        ADI_COMMON_DEVICE_STATE_IS_OPEN(*commonDev),
                                        "Device Requires HwOpen");
                }

                return recoveryAction;
            }

            /* Lock Device */
            recoveryAction = adi_common_hal_Lock(commonDev);
            break;

        case ADI_FALSE:
            /* Fall Through */

        default:
            /* No Locking Required */
            recoveryAction = ADI_COMMON_ERR_ACT_NONE;
            break;
    }

    if (ADI_COMMON_ERR_ACT_NONE != recoveryAction)
    {
        /* Device Locking Failed */
        if (NULL != adi_hal_TlsGet(HAL_TLS_ERR))
        {
            /* Bypass Common Device Structure for Error Reporting if TLS has Error Pointer */
            adi_common_Device_t dummyDev;

            ADI_LIBRARY_MEMSET(&dummyDev, 0, sizeof(adi_common_Device_t));

            dummyDev.errPtr = (adi_common_ErrData_t*) adi_hal_TlsGet(HAL_TLS_ERR);

            ADI_ERROR_REPORT(   &dummyDev,
                                ADI_COMMON_ERRSRC_API,
                                ADI_COMMON_ERRCODE_LOCK,
                                recoveryAction,
                                commonDev->lockCnt,         /* Snapshot of Locking History */
                                "Device Locking Issue");
        }

        return recoveryAction;
    }

    /* Device is now locked or else locking was not requested. */
    ++commonDev->publicCnt;

    /* Log API entry */
    if (commonDev->publicCnt > 1U && logCtl == ADI_COMMON_DEVICE_LOGCTL_QUIET)
    {
        /* Recursive public function entry detected; Change from Public to Private Logging Level */
        if (ADI_FALSE == ADI_COMMON_DEVICE_STATE_IS_TC(*commonDev))
        {
            /* Time Critical Mode Off - All adi_common_LogWrite(), bar error logging, are gated by this check to avoid unnecessary adi_hal_LogWrite() calls */
            adi_common_LogWrite(commonDev, ADI_HAL_LOG_API_PRIV, "-> %s()", fnName);
        }
    }
    else
    {
        /* Normal case - just log the entry at Public API level */
        if (ADI_FALSE == ADI_COMMON_DEVICE_STATE_IS_TC(*commonDev))
        {
            /* Time Critical Mode Off - All adi_common_LogWrite(), bar error logging, are gated by this check to avoid unnecessary adi_hal_LogWrite() calls */
            adi_common_LogWrite(commonDev, ADI_HAL_LOG_API, "-> %s()", fnName);
        }
    }

    if (NULL != adi_hal_TlsGet(HAL_TLS_ERR))
    {
        /* Assume using TLS for Error Reporting */
        commonDev->errPtr = (adi_common_ErrData_t*) adi_hal_TlsGet(HAL_TLS_ERR);
    }
    else
    {
        /*  Do Nothing; Assuming Application is Managing Device Error Structure(s) Directly.
         *  If Multi Threaded, Application is required to provide its own Mutual Exclusion Mechanism for the Error Structure.
         */
    }

    /* Assume Error Structure Pointer Assigned to Current Device at this Point (i.e. using TLS or Externally Managed) */

    if (commonDev->publicCnt == 1U)
    {
        /* Clear Error Memory on First Entry into Public API Only
         *  - This is to allow further error reporting from subsequent Public API calls during an Error State
         */
        (void) ADI_CLEAR_ERROR_MEMORY(commonDev->errPtr);
    }

    /* Assume Error Memory has been cleared at this point */

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_common_hal_ApiExit_vLogCtl(adi_common_Device_t* const  commonDev,
                                                              const char* const           fnName,
                                                              const uint32_t              doLocking,
                                                              const adi_common_LogCtl_e   logCtl)

{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    ADI_NULL_PTR_REPORT_RETURN(commonDev, fnName);

    /* Log API exit */
    if (commonDev->publicCnt > 1U && logCtl == ADI_COMMON_DEVICE_LOGCTL_QUIET)
    {
        /* Recursive public function exit detected; Change from Public to Private Logging Level */
        if (ADI_FALSE == ADI_COMMON_DEVICE_STATE_IS_TC(*commonDev))
        {
            /* Time Critical Mode Off - All adi_common_LogWrite(), bar error logging, are gated by this check to avoid unnecessary adi_hal_LogWrite() calls */
            adi_common_LogWrite(commonDev, ADI_HAL_LOG_API_PRIV, "<- %s()", fnName);
        }
    }
    else
    {
        /* Normal case - just log the exit at Public API level */
        if (ADI_FALSE == ADI_COMMON_DEVICE_STATE_IS_TC(*commonDev))
        {
            /* Time Critical Mode Off - All adi_common_LogWrite(),  bar error logging, are gated by this check to avoid unnecessary adi_hal_LogWrite() calls */
            adi_common_LogWrite(commonDev, ADI_HAL_LOG_API, "<- %s()", fnName);
        }
    }

    if (commonDev->publicCnt == 0U)
    {
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERRCODE_LOCK,
                            recoveryAction,
                            commonDev->publicCnt,
                            "Check API Device Locking Mechanism");
        return recoveryAction;
    }

    --commonDev->publicCnt;

    switch (doLocking)
    {
        case ADI_TRUE:
            /* Unlock Device */
            recoveryAction = adi_common_hal_Unlock(commonDev);
            if (ADI_COMMON_ERR_ACT_NONE != recoveryAction)
            {
                /* Device Locked Up */
                recoveryAction = ADI_COMMON_ERR_ACT_RESET_DEVICE;
            }
            break;

        case ADI_FALSE:
            /* Fall Through */

        default:
            /* No Unlocking Required */
            recoveryAction = ADI_COMMON_ERR_ACT_NONE;
            break;
    }

    if (ADI_COMMON_ERR_ACT_NONE != recoveryAction)
    {
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERRCODE_LOCK,
                            recoveryAction,
                            ADI_NO_VARIABLE,
                            "Device Unlocking Issue");
        return recoveryAction;
    }

    /* Assume Device Unlocked at this Point */

    return recoveryAction;
}


ADI_API adi_common_ErrAction_e adi_common_hal_ApiEnter( adi_common_Device_t* const  commonDev,
                                                        const char* const           fnName,
                                                        const uint32_t              doLocking)
{
    return adi_common_hal_ApiEnter_vLogCtl(commonDev, fnName, doLocking, ADI_COMMON_DEVICE_LOGCTL_NORMAL);
}

ADI_API adi_common_ErrAction_e adi_common_hal_ApiExit(  adi_common_Device_t* const  commonDev,
                                                        const char* const           fnName,
                                                        const uint32_t              doLocking)
{
    return adi_common_hal_ApiExit_vLogCtl(commonDev, fnName, doLocking, ADI_COMMON_DEVICE_LOGCTL_NORMAL);
}

ADI_API adi_common_ErrAction_e adi_common_hal_PlatformFunctionCheck(adi_common_Device_t* const commonDev)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_NONE;

    ADI_NULL_COMMON_PTR_RETURN(commonDev);

    if ((adi_hal_HwOpen         == NULL)    ||
        (adi_hal_HwClose        == NULL)    ||
        (adi_hal_HwReset        == NULL)    ||
        (adi_hal_SpiWrite       == NULL)    ||
        (adi_hal_SpiRead        == NULL)    ||
        (adi_hal_LogFileOpen    == NULL)    ||
        (adi_hal_LogLevelSet    == NULL)    ||
        (adi_hal_LogLevelGet    == NULL)    ||
        (adi_hal_LogWrite       == NULL)    ||
        (adi_hal_Wait_us        == NULL)    ||
        (adi_hal_Wait_ms        == NULL)    ||
        (adi_hal_MutexInit      == NULL)    ||
        (adi_hal_MutexLock      == NULL)    ||
        (adi_hal_MutexUnlock    == NULL)    ||
        (adi_hal_MutexDestroy   == NULL)    ||
        (adi_hal_TlsSet         == NULL)    ||
        (adi_hal_TlsGet         == NULL))
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(ADI_HAL_ERR_NOT_IMPLEMENTED);
        ADI_ERROR_REPORT(   commonDev,
                            ADI_COMMON_ERRSRC_HAL,
                            ADI_HAL_ERR_NOT_IMPLEMENTED,
                            recoveryAction,
                            ADI_NO_VARIABLE,
                            "All HAL Function Pointers Not Configured");
        return recoveryAction;
    }

    return recoveryAction;
}
