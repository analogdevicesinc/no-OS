// SPDX-License-Identifier: GPL-2.0
/**
 * \file Automatically generated file: adrv9025_bf_analog_tx_mem_map.c
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

#include "./../../private/include/adrv9025_bf_analog_tx_mem_map.h"
#include "./../../private/include/adrv9025_bf_hal.h"
#include "adi_common_error.h"

int32_t adrv9025_AnalogTxMemMapAuxdac0BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4095U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac0BfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac0BfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x6F),
                          (uint8_t)(bfValue >> 8),
                          0xF,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x70),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac0BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac0BfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x6F),
                         &rxBfData[0x0],
                         0xF,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x70),
                        &rxBfData[0x1],
                        0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x2,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint16_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac0ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac0ConfigBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac0ConfigBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x6F),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac1BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4095U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac1BfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac1BfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x71),
                          (uint8_t)(bfValue >> 8),
                          0xF,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x72),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac1BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac1BfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x71),
                         &rxBfData[0x0],
                         0xF,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x72),
                        &rxBfData[0x1],
                        0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x2,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint16_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac1ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac1ConfigBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac1ConfigBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x71),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac2BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4095U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac2BfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac2BfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x73),
                          (uint8_t)(bfValue >> 8),
                          0xF,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x74),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac2BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac2BfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x73),
                         &rxBfData[0x0],
                         0xF,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x74),
                        &rxBfData[0x1],
                        0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x2,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint16_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac2ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac2ConfigBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac2ConfigBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x73),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac3BfSet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t                            bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4095U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac3BfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac3BfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x75),
                          (uint8_t)(bfValue >> 8),
                          0xF,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x76),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac3BfGet(adi_adrv9025_Device_t*              device,
                                            adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                            uint16_t*                           bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac3BfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x75),
                         &rxBfData[0x0],
                         0xF,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x76),
                        &rxBfData[0x1],
                        0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x2,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint16_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdac3ConfigBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
                                                  uint8_t                             bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac3ConfigBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdac3ConfigBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x75),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_AnalogTxMemMapAuxdacLatchEnBfSet(adi_adrv9025_Device_t*              device,
                                                  adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr,
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
                         "Invalid adrv9025_AnalogTxMemMapAuxdacLatchEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ANALOG_TX) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH1) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH2) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH3) &&
        (baseAddr != ADRV9025_BF_ANALOG_TX_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_AnalogTxMemMapAuxdacLatchEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x77),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}
