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
#include "adi_common_error.h"
#include "adi_platform.h"

int32_t adi_common_hal_CallBack_Verify(adi_common_Device_t *commonDev)
{
    if ((adi_hal_LogWrite == NULL) ||
        (adi_hal_Wait_us == NULL))
    {
        ADI_ERROR_REPORT(commonDev,
            ADI_COMMON_ERRSRC_ADI_HAL,
            ADI_COMMON_ERR_NULL_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            NULL,
            "HAL library Function pointer is NULL");
        ADI_ERROR_RETURN(commonDev->error.newAction);
    }

    return commonDev->error.newAction;
}

int32_t adi_common_hal_Wait_us(adi_common_Device_t *commonDev, uint32_t time_us)
{
    int32_t halError = 0;

    if (commonDev == NULL)
    {
        return ADI_COMMON_ACT_ERR_CHECK_PARAM;
    }
    else
    {
        halError = adi_hal_Wait_us(commonDev->devHalInfo, time_us);
    }

    if (halError != 0)
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
