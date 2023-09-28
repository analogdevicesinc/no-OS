// SPDX-License-Identifier: GPL-2.0
/**
 * \file Automatically generated file: adrv9025_bf_analog_orx_mem_map.c
 *
 * \brief Contains BitField functions to support ADRV9025 transceiver device.
 *
 * ADRV9025 BITFIELD VERSION: 0.0.0.1
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#include "./../../private/include/adrv9025_bf_analog_orx_mem_map.h"
#include "./../../private/include/adrv9025_bf_hal.h"
#include "adi_common_error.h"

int32_t adrv9025_AnalogOrxMemMapLomuxSelBfGet(adi_adrv9025_Device_t*               device,
                                              adrv9025_BfAnalogOrxMemMapChanAddr_e baseAddr,
                                              uint8_t*                             bfValue)
{
    uint8_t  rxBfData[8] = {0};
    uint64_t data        = 0;
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_NULL_CHECK > 0
    ADI_NULL_PTR_RETURN(&device->common,
                        bfValue);
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ANALOG_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ANALOG_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogOrxMemMapLomuxSelBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xAA),
                         &rxBfData[0],
                         0x1,
                         0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}
