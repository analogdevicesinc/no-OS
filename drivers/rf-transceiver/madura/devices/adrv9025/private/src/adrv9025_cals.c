// SPDX-License-Identifier: GPL-2.0
/**
 * \file adrv9025_cals.c
 * \brief Contains ADRV9025 calibration related private function implementation
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_cals_types.h"
#include "../../private/include/adrv9025_cals.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_cpu.h"

#ifdef __KERNEL__
#include <linux/kernel.h> /* needed for memset */
#endif

#ifdef _RELEASE_BUILD_
#line __LINE__ "adrv9025_cals.c"
#endif

const char* adrv9025_CalsErrMsgGet(adi_adrv9025_Device_t* device,
                                   uint32_t               errCode)
{
#if ADI_ADRV9025_ARM_VERBOSE == 0
    UNUSED_PARA(device);
    UNUSED_PARA(errCode);
    return "";
#else
    const char *errorString = NULL;

    if (errCode > 0)
    {
        errorString = adrv9025_CpuErrCodeToString(device, errCode);

        if (errorString != NULL)
        {
            return (errorString);
        }
    }

    return "Unknown Init Calibration Error\n";
#endif
}

int32_t adrv9025_InitCalStructureCheck(adi_adrv9025_Device_t*   device,
                                       adi_adrv9025_InitCals_t* initCals)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        initCals);

    /* Check if the mask is out of range*/
    if (initCals->calMask > ADI_ADRV9025_MAX_INIT_CAL_MASK)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         initCals->calMask,
                         "Invalid Initcal Cal Mask Selected - Mask contains one more more invalid bits");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((initCals->channelMask < ADI_ADRV9025_CAL_CH_1) || (initCals->channelMask > ADI_ADRV9025_CAL_CH_ALL))
    {
        /* Error condition channelMask is outside range of allowed values */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         initCals->channelMask,
                         "Invalid Initcal Channel Mask - Channel Mask out of range (0x01 - 0x0F)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    /* No errors found return ADI_COMMON_ACT_NO_ACTION */
    return recoveryAction;
}
