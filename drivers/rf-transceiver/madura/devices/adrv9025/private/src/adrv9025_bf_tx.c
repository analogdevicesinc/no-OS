// SPDX-License-Identifier: GPL-2.0
/**
 * \file Automatically generated file: adrv9025_bf_tx.c
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

#include "./../../private/include/adrv9025_bf_tx.h"
#include "./../../private/include/adrv9025_bf_hal.h"
#include "adi_common_error.h"

int32_t adrv9025_TxArmOverrideControlBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfTxChanAddr_e baseAddr,
                                           uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxArmOverrideControlBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxArmOverrideControlBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xEA),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxArmOverrideControlBfGet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfTxChanAddr_e baseAddr,
                                           uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxArmOverrideControlBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xEA),
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

int32_t adrv9025_TxCfrBasePulse21bBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxCfrBasePulse21bBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxCfrBasePulse21bBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xF),
                          (uint8_t)bfValue,
                          0x8,
                          0x3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxCfrBasePulse21bBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxCfrBasePulse21bBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xF),
                         &rxBfData[0],
                         0x8,
                         0xC3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxCfrHalfPulseLenBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxCfrHalfPulseLenBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x12),
                         &rxBfData[0x0],
                         0x3,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x11),
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

int32_t adrv9025_TxDpdActClkEnableBfSet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxDpdActClkEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxDpdActClkEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x7),
                          (uint8_t)bfValue,
                          0x40,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxDpdActClkEnableBfGet(adi_adrv9025_Device_t*  device,
                                        adrv9025_BfTxChanAddr_e baseAddr,
                                        uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxDpdActClkEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x7),
                         &rxBfData[0],
                         0x40,
                         0xC6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxJesdDfrmMaskBfSet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfTxChanAddr_e baseAddr,
                                     uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxJesdDfrmMaskBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxJesdDfrmMaskBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x32),
                          (uint8_t)bfValue,
                          0x3,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxJesdDfrmMaskBfGet(adi_adrv9025_Device_t*  device,
                                     adrv9025_BfTxChanAddr_e baseAddr,
                                     uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxJesdDfrmMaskBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x32),
                         &rxBfData[0],
                         0x3,
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

int32_t adrv9025_TxNcoTestEnableBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxNcoTestEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxNcoTestEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xC5),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxNcoTestEnableBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxNcoTestEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xC5),
                         &rxBfData[0],
                         0x2,
                         0xC1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAprEnBfSet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfTxChanAddr_e baseAddr,
                                          uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionAprEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAprEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9A),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAprEnBfGet(adi_adrv9025_Device_t*  device,
                                          adrv9025_BfTxChanAddr_e baseAddr,
                                          uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAprEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x9A),
                         &rxBfData[0],
                         0x2,
                         0xC1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAveragePeakRatioBfGet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfTxChanAddr_e baseAddr,
                                                     uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAveragePeakRatioBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xA6),
                         &rxBfData[0x0],
                         0x7F,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xA5),
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

int32_t adrv9025_TxPaProtectionAveragePowerBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAveragePowerBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xA4),
                        &rxBfData[0x0],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xA3),
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

int32_t adrv9025_TxPaProtectionAvgThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint16_t                bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8191U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxPaProtectionAvgThresholdBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvgThresholdBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9C),
                          (uint8_t)(bfValue >> 8),
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x9B),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAvgThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvgThresholdBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x9C),
                         &rxBfData[0x0],
                         0x1F,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x9B),
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

int32_t adrv9025_TxPaProtectionAvgpowerEnBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionAvgpowerEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvgpowerEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x99),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAvgpowerEnBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvgpowerEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x99),
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

int32_t adrv9025_TxPaProtectionAvgpowerErrorBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvgpowerErrorBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xA2),
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

int32_t adrv9025_TxPaProtectionAvgpowerErrorClearBfSet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfTxChanAddr_e baseAddr,
                                                       uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionAvgpowerErrorClearBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvgpowerErrorClearBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xA2),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAvgpowerErrorClearRequiredBfSet(adi_adrv9025_Device_t*  device,
                                                               adrv9025_BfTxChanAddr_e baseAddr,
                                                               uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionAvgpowerErrorClearRequiredBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvgpowerErrorClearRequiredBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x99),
                          (uint8_t)bfValue,
                          0x4,
                          0x2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAvrgDurBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionAvrgDurBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvrgDurBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x99),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionAvrgDurBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionAvrgDurBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x99),
                         &rxBfData[0],
                         0xF0,
                         0xC4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionErrorPowerBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionErrorPowerBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xA8),
                        &rxBfData[0x0],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xA7),
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

int32_t adrv9025_TxPaProtectionGainRampDownEnBfSet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionGainRampDownEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionGainRampDownEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xA0),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionGainRampDownEnBfGet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionGainRampDownEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xA0),
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

int32_t adrv9025_TxPaProtectionGainRampUpEnBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionGainRampUpEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionGainRampUpEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xA2),
                          (uint8_t)bfValue,
                          0x80,
                          0x7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

/**
 * \brief PA Protection Gain ramp Up Enable
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *  Parameter is of type ::adrv9025_BfTxChanAddr_e.
 * \param bfValue Data to be configured. Parameter is of type uint8_t.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset.
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check.
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required.
 *
 */
int32_t adrv9025_TxPaProtectionGainRampUpEnBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  rxBfData[8]    = {0};
    uint64_t data           = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

#if ADRV9025_BITFIELD_NULL_CHECK > 0
    /* NULL check */
    ADI_NULL_PTR_RETURN(&device->common,
                        bfValue);
#endif /* ADRV9025_BITFIELD_NULL_CHECK */

#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    /* Base Address check */
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionGainRampUpEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif /* ADRV9025_BITFIELD_ADDR_CHECK */

    recoveryAction = adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Field */
    recoveryAction = adrv9025_BfFieldRead(device,
                                          (baseAddr + 0xA2),
                                          &rxBfData[0],
                                          0x80,
                                          0xC7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_BfReadAssembleData(device,
                                                 &rxBfData[0x0],
                                                 0x1,
                                                 &data);
    ADI_ERROR_RETURN(device->common.error.newAction);

    *bfValue = (uint8_t)data;

    return recoveryAction;
}

int32_t adrv9025_TxPaProtectionGainStepDownEnBfSet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionGainStepDownEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionGainStepDownEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9F),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionInputSelBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionInputSelBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionInputSelBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x99),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionInputSelBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionInputSelBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x99),
                         &rxBfData[0],
                         0x2,
                         0xC1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakCountBfSet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxPaProtectionPeakCountBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakCountBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9E),
                          (uint8_t)bfValue,
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakCountBfGet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakCountBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x9E),
                         &rxBfData[0],
                         0x1F,
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

int32_t adrv9025_TxPaProtectionPeakDurBfSet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionPeakDurBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakDurBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9A),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakDurBfGet(adi_adrv9025_Device_t*  device,
                                            adrv9025_BfTxChanAddr_e baseAddr,
                                            uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakDurBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x9A),
                         &rxBfData[0],
                         0xF0,
                         0xC4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakThresholdBfSet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint16_t                bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8191U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxPaProtectionPeakThresholdBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakThresholdBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xAA),
                          (uint8_t)(bfValue >> 8),
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0xA9),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakThresholdBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakThresholdBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xAA),
                         &rxBfData[0x0],
                         0x1F,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xA9),
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

int32_t adrv9025_TxPaProtectionPeakpowerEnBfSet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionPeakpowerEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakpowerEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9A),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakpowerEnBfGet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakpowerEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x9A),
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

int32_t adrv9025_TxPaProtectionPeakpowerErrorBfGet(adi_adrv9025_Device_t*  device,
                                                   adrv9025_BfTxChanAddr_e baseAddr,
                                                   uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakpowerErrorBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xA2),
                         &rxBfData[0],
                         0x4,
                         0xC2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakpowerErrorClearBfSet(adi_adrv9025_Device_t*  device,
                                                        adrv9025_BfTxChanAddr_e baseAddr,
                                                        uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionPeakpowerErrorClearBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakpowerErrorClearBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xA2),
                          (uint8_t)bfValue,
                          0x8,
                          0x3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionPeakpowerErrorClearRequiredBfSet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                                uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionPeakpowerErrorClearRequiredBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionPeakpowerErrorClearRequiredBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9A),
                          (uint8_t)bfValue,
                          0x4,
                          0x2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionRampMaxAttenuationBfSet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfTxChanAddr_e baseAddr,
                                                       uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 127U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxPaProtectionRampMaxAttenuationBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionRampMaxAttenuationBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xA0),
                          (uint8_t)bfValue,
                          0xFE,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionRampMaxAttenuationBfGet(adi_adrv9025_Device_t*  device,
                                                       adrv9025_BfTxChanAddr_e baseAddr,
                                                       uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionRampMaxAttenuationBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xA0),
                         &rxBfData[0],
                         0xFE,
                         0xC1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionRampStepDurationBfSet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfTxChanAddr_e baseAddr,
                                                     uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionRampStepDurationBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionRampStepDurationBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xA1),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionRampStepDurationBfGet(adi_adrv9025_Device_t*  device,
                                                     adrv9025_BfTxChanAddr_e baseAddr,
                                                     uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionRampStepDurationBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xA1),
                         &rxBfData[0],
                         0xF0,
                         0xC4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionRampStepSizeBfSet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPaProtectionRampStepSizeBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionRampStepSizeBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xA1),
                          (uint8_t)bfValue,
                          0xF,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPaProtectionRampStepSizeBfGet(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfTxChanAddr_e baseAddr,
                                                 uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionRampStepSizeBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xA1),
                         &rxBfData[0],
                         0xF,
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

int32_t adrv9025_TxPaProtectionTxAttenStepBfSet(adi_adrv9025_Device_t*  device,
                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 127U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxPaProtectionTxAttenStepBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPaProtectionTxAttenStepBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9F),
                          (uint8_t)bfValue,
                          0xFE,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPllJesdProtClrBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfTxChanAddr_e baseAddr,
                                       uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPllJesdProtClrBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPllJesdProtClrBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x32),
                          (uint8_t)bfValue,
                          0x40,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPllJesdProtClrReqdBfSet(adi_adrv9025_Device_t*  device,
                                           adrv9025_BfTxChanAddr_e baseAddr,
                                           uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxPllJesdProtClrReqdBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPllJesdProtClrReqdBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x32),
                          (uint8_t)bfValue,
                          0x20,
                          0x5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPllJesdProtEventBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPllJesdProtEventBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x32),
                         &rxBfData[0],
                         0x80,
                         0xC7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxPllUnlockMaskBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxPllUnlockMaskBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPllUnlockMaskBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x31),
                          (uint8_t)bfValue,
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxPllUnlockMaskBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxPllUnlockMaskBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x31),
                         &rxBfData[0],
                         0x1F,
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

int32_t adrv9025_TxSpi2TxAttenGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxSpi2TxAttenGpioSelectBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSpi2TxAttenGpioSelectBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xF1),
                          (uint8_t)bfValue,
                          0xF,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxSpi2TxAttenGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                              adrv9025_BfTxChanAddr_e baseAddr,
                                              uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSpi2TxAttenGpioSelectBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xF1),
                         &rxBfData[0],
                         0xF,
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

int32_t adrv9025_TxSrlIrqBfGet(adi_adrv9025_Device_t*  device,
                               adrv9025_BfTxChanAddr_e baseAddr,
                               uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlIrqBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x69),
                         &rxBfData[0],
                         0x10,
                         0xC4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlIrqEnBfSet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxSrlIrqEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlIrqEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x69),
                          (uint8_t)bfValue,
                          0x8,
                          0x3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlIrqEnBfGet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlIrqEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x69),
                         &rxBfData[0],
                         0x8,
                         0xC3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlSlewOffsetBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxSrlSlewOffsetBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlSlewOffsetBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x68),
                          (uint8_t)bfValue,
                          0x1E,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlSlewOffsetBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlSlewOffsetBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x68),
                         &rxBfData[0],
                         0x1E,
                         0xC1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlStatBfGet(adi_adrv9025_Device_t*  device,
                                adrv9025_BfTxChanAddr_e baseAddr,
                                uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlStatBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x6A),
                        &rxBfData[0x0],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x6B),
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

int32_t adrv9025_TxSrlStatEnBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxSrlStatEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlStatEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x69),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlStatEnBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlStatEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x69),
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

int32_t adrv9025_TxSrlStatModeBfSet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxSrlStatModeBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlStatModeBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x69),
                          (uint8_t)bfValue,
                          0x6,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlTableSelBfSet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxSrlTableSelBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlTableSelBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x68),
                          (uint8_t)bfValue,
                          0xC0,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxSrlTableSelBfGet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxSrlTableSelBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x68),
                         &rxBfData[0],
                         0xC0,
                         0xC6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTpcDecrAttenGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTpcDecrAttenGpioSelectBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTpcDecrAttenGpioSelectBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xF0),
                          (uint8_t)bfValue,
                          0xF0,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTpcDecrAttenGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTpcDecrAttenGpioSelectBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xF0),
                         &rxBfData[0],
                         0xF0,
                         0xC4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTpcIncrAttenGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTpcIncrAttenGpioSelectBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTpcIncrAttenGpioSelectBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xF0),
                          (uint8_t)bfValue,
                          0xF,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTpcIncrAttenGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                               adrv9025_BfTxChanAddr_e baseAddr,
                                               uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTpcIncrAttenGpioSelectBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xF0),
                         &rxBfData[0],
                         0xF,
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

int32_t adrv9025_TxTxAttenConfigBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxTxAttenConfigBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenConfigBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x30),
                          (uint8_t)bfValue,
                          0x30,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenConfigBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenConfigBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x30),
                         &rxBfData[0],
                         0x30,
                         0xC4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenModeBfSet(adi_adrv9025_Device_t*  device,
                                    adrv9025_BfTxChanAddr_e baseAddr,
                                    uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxTxAttenModeBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenModeBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x30),
                          (uint8_t)bfValue,
                          0x3,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenUpdGpioEnBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxAttenUpdGpioEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenUpdGpioEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x42),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenUpdGpioEnBfGet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenUpdGpioEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x42),
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

int32_t adrv9025_TxTxAttenUpdGpioSelectBfSet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxAttenUpdGpioSelectBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenUpdGpioSelectBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x42),
                          (uint8_t)bfValue,
                          0x1E,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenUpdGpioSelectBfGet(adi_adrv9025_Device_t*  device,
                                             adrv9025_BfTxChanAddr_e baseAddr,
                                             uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenUpdGpioSelectBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x42),
                         &rxBfData[0],
                         0x1E,
                         0xC1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenuationBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint16_t                bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1023U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxTxAttenuationBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenuationBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x34),
                          (uint8_t)(bfValue >> 8),
                          0x3,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x33),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenuationBfGet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint16_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenuationBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x34),
                         &rxBfData[0x0],
                         0x3,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x33),
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

int32_t adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfSet(adi_adrv9025_Device_t*  device,
                                                    adrv9025_BfTxChanAddr_e baseAddr,
                                                    uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x32),
                          (uint8_t)bfValue,
                          0x4,
                          0x2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfGet(adi_adrv9025_Device_t*  device,
                                                    adrv9025_BfTxChanAddr_e baseAddr,
                                                    uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDataRampDownOnJesdDfrmIrqBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x32),
                         &rxBfData[0],
                         0x4,
                         0xC2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxDataRampDownOnPllUnlockBfSet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxDataRampDownOnPllUnlockBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDataRampDownOnPllUnlockBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x30),
                          (uint8_t)bfValue,
                          0x4,
                          0x2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxDataRampDownOnPllUnlockBfGet(adi_adrv9025_Device_t*  device,
                                                  adrv9025_BfTxChanAddr_e baseAddr,
                                                  uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDataRampDownOnPllUnlockBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x30),
                         &rxBfData[0],
                         0x4,
                         0xC2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxDpNcoEnableBfSet(adi_adrv9025_Device_t*  device,
                                      adrv9025_BfTxChanAddr_e baseAddr,
                                      uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxDpNcoEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDpNcoEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x88),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxDpNcoFtwBfSet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfTxChanAddr_e baseAddr,
                                   uint32_t                bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4294967295U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxTxDpNcoFtwBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDpNcoFtwBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x89),
                         (uint8_t)(bfValue >> 24));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x8A),
                         (uint8_t)(bfValue >> 16));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x8B),
                         (uint8_t)(bfValue >> 8));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x8C),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxDpNcoFtwBfGet(adi_adrv9025_Device_t*  device,
                                   adrv9025_BfTxChanAddr_e baseAddr,
                                   uint32_t*               bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDpNcoFtwBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x89),
                        &rxBfData[0x0],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x8A),
                        &rxBfData[0x1],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x8B),
                        &rxBfData[0x2],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x8C),
                        &rxBfData[0x3],
                        0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x4,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint32_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxDpNcoFtwUpdateBfSet(adi_adrv9025_Device_t*  device,
                                         adrv9025_BfTxChanAddr_e baseAddr,
                                         uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxDpNcoFtwUpdateBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxDpNcoFtwUpdateBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x96),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxEnableBfSet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xE8),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxEnableBfGet(adi_adrv9025_Device_t*  device,
                                 adrv9025_BfTxChanAddr_e baseAddr,
                                 uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xE8),
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

int32_t adrv9025_TxTxIncrDecrWordBfSet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfTxChanAddr_e baseAddr,
                                       uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxTxIncrDecrWordBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxIncrDecrWordBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x38),
                          (uint8_t)bfValue,
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxIncrDecrWordBfGet(adi_adrv9025_Device_t*  device,
                                       adrv9025_BfTxChanAddr_e baseAddr,
                                       uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxIncrDecrWordBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x38),
                         &rxBfData[0],
                         0x1F,
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

int32_t adrv9025_TxTxNcoGainBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t                 bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxTxNcoGainBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxNcoGainBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x88),
                          (uint8_t)bfValue,
                          0xC0,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxNcoGainBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxNcoGainBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x88),
                         &rxBfData[0],
                         0xC0,
                         0xC6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxPinModeBfSet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t                 bfValue)
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
                         "Invalid adrv9025_TxTxPinModeBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxPinModeBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0xE8),
                          (uint8_t)bfValue,
                          0x10,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxPinModeBfGet(adi_adrv9025_Device_t*  device,
                                  adrv9025_BfTxChanAddr_e baseAddr,
                                  uint8_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxPinModeBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0xE8),
                         &rxBfData[0],
                         0x10,
                         0xC4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfSet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                                uint8_t                 bfValue)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    /* Range check */
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_DEVICEBF,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            bfValue,
            "Invalid adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif /* ADRV9025_BITFIELD_VALUE_CHECK */

#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    /* Base Address check */
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif /* ADRV9025_BITFIELD_ADDR_CHECK */

    recoveryAction = adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Write to Single Byte */
    recoveryAction = adrv9025_BfByteWrite(device,
                                          (baseAddr + 0x3E),
                                          ((uint8_t)bfValue));
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfGet(adi_adrv9025_Device_t*  device,
                                                                adrv9025_BfTxChanAddr_e baseAddr,
                                                                uint8_t*                bfValue)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  rxBfData[8]    = {0};
    uint64_t data           = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

#if ADRV9025_BITFIELD_NULL_CHECK > 0
    /* NULL check */
    ADI_NULL_PTR_RETURN(&device->common,
                        bfValue);
#endif /* ADRV9025_BITFIELD_NULL_CHECK */

#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    /* Base Address check */
    if ((baseAddr != ADRV9025_BF_ALL_TX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_TX_CH0) &&
        (baseAddr != ADRV9025_BF_TX_CH1) &&
        (baseAddr != ADRV9025_BF_TX_CH2) &&
        (baseAddr != ADRV9025_BF_TX_CH3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxTxAttenuationEarlyDelayCounterForAnalogBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif /* ADRV9025_BITFIELD_ADDR_CHECK */

    recoveryAction = adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read to Single Byte */
    recoveryAction = adrv9025_BfByteRead(device,
                                         (baseAddr + 0x3E),
                                         &rxBfData[0],
                                         0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    recoveryAction = adrv9025_BfReadAssembleData(device,
                                                 &rxBfData[0x0],
                                                 0x1,
                                                 &data);
    ADI_ERROR_RETURN(device->common.error.newAction);

    *bfValue = (uint8_t)data;

    return recoveryAction;
}
