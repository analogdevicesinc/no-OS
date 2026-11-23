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
#include "adi_common_hal_wrapper.h"
#include "adi_common_error.h"
#include "adi_common_hal.h"

int32_t adi_common_hal_wrapper_Wait_us(adi_common_Device_t *commonDev, uint32_t time_us)
{
    int32_t halError = 0;
    
    ADI_NULL_DEVICE_PTR_RETURN(commonDev);

    halError = adi_common_hal_Wait_us(commonDev->devHalInfo, time_us);

    /* Report error in this layer */
    ADI_ERROR_REPORT(commonDev,
                     ADI_COMMON_SRC_HAL,
                     halError,
                     ADI_COMMON_ACT_ERR_CHECK_TIMER,
                     NULL,
                     "Wait function failed");

    ADI_API_RETURN(commonDev);
}
