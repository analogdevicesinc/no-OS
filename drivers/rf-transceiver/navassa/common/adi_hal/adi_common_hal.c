/**
* \file
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*        
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/* Common HAL layer maintained by Analog Devices */
#include "adi_common_hal.h"
#include "adi_common_error_types.h"
#include "adi_platform.h"

int32_t adi_common_hal_CallBack_Verify(adi_common_Device_t *commonDev)
{
    if ((adi_hal_HwOpen == NULL) ||
        (adi_hal_HwClose == NULL) ||
        (adi_hal_HwReset == NULL) ||
        (adi_hal_SpiWrite == NULL) ||
        (adi_hal_SpiRead == NULL) ||
        (adi_hal_LogFileOpen == NULL) ||
        (adi_hal_LogLevelSet == NULL) ||
        (adi_hal_LogLevelGet == NULL) ||
        (adi_hal_LogWrite == NULL) ||
        (adi_hal_Wait_us == NULL) ||
        (adi_hal_Wait_ms == NULL))
    {
        ADI_ERROR_REPORT(commonDev,
            ADI_COMMON_ERRSRC_ADI_HAL,
            ADI_HAL_LIBRARY_NOT_AVAILABLE,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            NULL,
            "HAL library Function pointer is NULL");
        ADI_ERROR_RETURN(commonDev->error.newAction);
    }

    return commonDev->error.newAction;
}

int32_t adi_common_hal_HwOpen(adi_common_Device_t *commonDev)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    if (commonDev == NULL)
    {
        return ADI_COMMON_ACT_ERR_CHECK_PARAM;
    }
    else
    {
        recoveryAction = adi_hal_HwOpen(commonDev->devHalInfo);
    }
    
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        /* Report error in this layer */
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_HAL,
                         recoveryAction,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Hardware can not be opened");
    }

    return commonDev->error.newAction;
}

int32_t adi_common_hal_HwClose(adi_common_Device_t *commonDev)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if (commonDev == NULL)
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
        return ADI_COMMON_ACT_ERR_CHECK_PARAM;
    }
    else
    {
        halError = adi_hal_HwClose(commonDev->devHalInfo);
    }
    
    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        /* Report error in this layer */
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_HAL,
                         halError,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Hardware can not be closed");
    }

    return commonDev->error.newAction;
}

int32_t adi_common_hal_HwReset(adi_common_Device_t *commonDev, uint8_t pinLevel)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if (commonDev == NULL)
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
        return ADI_COMMON_ACT_ERR_CHECK_PARAM;
    }
    else
    {
        halError = adi_hal_HwReset(commonDev->devHalInfo, pinLevel);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        /* Report error in this layer */
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_HAL,
                         halError,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Hardware can not be opened");
    }

    return commonDev->error.newAction;
}

int32_t adi_common_hal_Wait_us(adi_common_Device_t *commonDev, uint32_t time_us)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if (commonDev == NULL)
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
        return ADI_COMMON_ACT_ERR_CHECK_PARAM;
    }
    else
    {
        halError = adi_hal_Wait_us(commonDev->devHalInfo, time_us);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        /* Report error in this layer */
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_HAL,
                         halError,
                         ADI_COMMON_ACT_ERR_CHECK_TIMER,
                         NULL,
                         "Wait function failed");
    }

    return commonDev->error.newAction;
}

int32_t adi_common_hal_Wait_ms(adi_common_Device_t *commonDev, uint32_t time_ms)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if (commonDev == NULL)
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
        return ADI_COMMON_ACT_ERR_CHECK_PARAM;
    }
    else
    {
        halError = adi_hal_Wait_us(commonDev->devHalInfo, time_ms * 1000);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        /* Report error in this layer */
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_HAL,
                         halError,
                         ADI_COMMON_ACT_ERR_CHECK_TIMER,
                         NULL,
                         "Wait function failed");
    }

    return commonDev->error.newAction;
}
