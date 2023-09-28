// SPDX-License-Identifier: GPL-2.0
/**
 * \file Automatically generated file: adrv9025_bf_analog_rx_mem_map.c
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

#include "./../../private/include/adrv9025_bf_analog_rx_mem_map.h"
#include "./../../private/include/adrv9025_bf_hal.h"
#include "adi_common_error.h"

int32_t adrv9025_AnalogRxMemMapRxBlockDetDecayBfSet(adi_adrv9025_Device_t*              device,
                                                    adrv9025_BfAnalogRxMemMapChanAddr_e baseAddr,
                                                    uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetDecayBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_RX) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetDecayBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x62),
                          (uint8_t)bfValue,
                          0x7,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogRxMemMapRxBlockDetLlbthBfSet(adi_adrv9025_Device_t*              device,
                                                    adrv9025_BfAnalogRxMemMapChanAddr_e baseAddr,
                                                    uint8_t                             channelId,
                                                    uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 63U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetLlbthBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_RX) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetLlbthBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetLlbthBfSet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x63 + channelId * 1),
                          (uint8_t)bfValue,
                          0x3F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogRxMemMapRxBlockDetLlbthBfGet(adi_adrv9025_Device_t*              device,
                                                    adrv9025_BfAnalogRxMemMapChanAddr_e baseAddr,
                                                    uint8_t                             channelId,
                                                    uint8_t*                            bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_RX) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetLlbthBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetLlbthBfGet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x63 + channelId * 1),
                         &rxBfData[0],
                         0x3F,
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

int32_t adrv9025_AnalogRxMemMapRxBlockDetUlbthBfSet(adi_adrv9025_Device_t*              device,
                                                    adrv9025_BfAnalogRxMemMapChanAddr_e baseAddr,
                                                    uint8_t                             channelId,
                                                    uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 63U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetUlbthBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_RX) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetUlbthBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetUlbthBfSet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x65 + channelId * 1),
                          (uint8_t)bfValue,
                          0x3F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogRxMemMapRxBlockDetUlbthBfGet(adi_adrv9025_Device_t*              device,
                                                    adrv9025_BfAnalogRxMemMapChanAddr_e baseAddr,
                                                    uint8_t                             channelId,
                                                    uint8_t*                            bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_RX) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetUlbthBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_AnalogRxMemMapRxBlockDetUlbthBfGet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x65 + channelId * 1),
                         &rxBfData[0],
                         0x3F,
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

int32_t adrv9025_AnalogRxMemMapRxTiaForceUpdateBfSet(adi_adrv9025_Device_t*              device,
                                                     adrv9025_BfAnalogRxMemMapChanAddr_e baseAddr,
                                                     uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogRxMemMapRxTiaForceUpdateBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_RX) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_RX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogRxMemMapRxTiaForceUpdateBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x3E),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}
