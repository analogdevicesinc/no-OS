// SPDX-License-Identifier: GPL-2.0
/**
 * \file Automatically generated file: adrv9025_bf_orx.c
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

#include "./../../private/include/adrv9025_bf_orx.h"
#include "./../../private/include/adrv9025_bf_hal.h"
#include "adi_common_error.h"

int32_t adrv9025_OrxAgcEnableFastRecoveryLoopBfSet(adi_adrv9025_Device_t*   device,
                                                   adrv9025_BfOrxChanAddr_e baseAddr,
                                                   uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxAgcEnableFastRecoveryLoopBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAgcEnableFastRecoveryLoopBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x9A),
                          (uint8_t)bfValue,
                          0x40,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxAuxAdcDecimationCtlBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxAuxAdcDecimationCtlBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcDecimationCtlBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1BC),
                          (uint8_t)bfValue,
                          0x30,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxAuxAdcPdBfSet(adi_adrv9025_Device_t*   device,
                                  adrv9025_BfOrxChanAddr_e baseAddr,
                                  uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxAuxAdcPdBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcPdBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1BC),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxAuxAdcPdBfGet(adi_adrv9025_Device_t*   device,
                                  adrv9025_BfOrxChanAddr_e baseAddr,
                                  uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcPdBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x1BC),
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

int32_t adrv9025_OrxAuxAdcReadBfGet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint16_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcReadBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x1BE),
                         &rxBfData[0x0],
                         0xF,
                         0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x1BD),
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

int32_t adrv9025_OrxAuxAdcReadbackLockBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxAuxAdcReadbackLockBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcReadbackLockBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1BC),
                          (uint8_t)bfValue,
                          0x80,
                          0x7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxAuxAdcResetBfSet(adi_adrv9025_Device_t*   device,
                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                     uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxAuxAdcResetBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcResetBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1BC),
                          (uint8_t)bfValue,
                          0x8,
                          0x3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxAuxAdcSelBfSet(adi_adrv9025_Device_t*   device,
                                   adrv9025_BfOrxChanAddr_e baseAddr,
                                   uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxAuxAdcSelBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcSelBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1BC),
                          (uint8_t)bfValue,
                          0x6,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxAuxAdcSelBfGet(adi_adrv9025_Device_t*   device,
                                   adrv9025_BfOrxChanAddr_e baseAddr,
                                   uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxAuxAdcSelBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x1BC),
                         &rxBfData[0],
                         0x6,
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

int32_t adrv9025_OrxBbdcLoopback1TrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxBbdcLoopback1TrackingEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcLoopback1TrackingEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x158),
                          (uint8_t)bfValue,
                          0x20,
                          0x5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcLoopback1TrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcLoopback1TrackingEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x158),
                         &rxBfData[0],
                         0x20,
                         0xC5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcLoopback2TrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxBbdcLoopback2TrackingEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcLoopback2TrackingEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x158),
                          (uint8_t)bfValue,
                          0x40,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcLoopback2TrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                                     adrv9025_BfOrxChanAddr_e baseAddr,
                                                     uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcLoopback2TrackingEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x158),
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

int32_t adrv9025_OrxBbdcLoopbackMShiftBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxBbdcLoopbackMShiftBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcLoopbackMShiftBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x159),
                          (uint8_t)bfValue,
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcLoopbackMShiftBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcLoopbackMShiftBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x159),
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

int32_t adrv9025_OrxBbdcMShiftBfSet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxBbdcMShiftBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcMShiftBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x14E),
                          (uint8_t)bfValue,
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcMShiftBfGet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcMShiftBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x14E),
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

int32_t adrv9025_OrxBbdcOrx2MShiftBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxBbdcOrx2MShiftBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcOrx2MShiftBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x15B),
                          (uint8_t)bfValue,
                          0x7C,
                          0x2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcOrx2MShiftBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcOrx2MShiftBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x15B),
                         &rxBfData[0],
                         0x7C,
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

int32_t adrv9025_OrxBbdcOrx2TrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxBbdcOrx2TrackingEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcOrx2TrackingEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x15B),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcOrx2TrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcOrx2TrackingEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x15B),
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

int32_t adrv9025_OrxBbdcTrackingEnableBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxBbdcTrackingEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcTrackingEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x14F),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxBbdcTrackingEnableBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxBbdcTrackingEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x14F),
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

int32_t adrv9025_OrxDecPowerBfGet(adi_adrv9025_Device_t*   device,
                                  adrv9025_BfOrxChanAddr_e baseAddr,
                                  uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDecPowerBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x34),
                        &rxBfData[0],
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

int32_t adrv9025_OrxDynamicSlicerAgcSyncEnableBfSet(adi_adrv9025_Device_t*   device,
                                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                                    uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxDynamicSlicerAgcSyncEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDynamicSlicerAgcSyncEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1D4),
                          (uint8_t)bfValue,
                          0x8,
                          0x3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxDynamicSlicerAgcSyncEnableBfGet(adi_adrv9025_Device_t*   device,
                                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                                    uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDynamicSlicerAgcSyncEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x1D4),
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

int32_t adrv9025_OrxDynamicSlicerModeEnBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxDynamicSlicerModeEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDynamicSlicerModeEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1D4),
                          (uint8_t)bfValue,
                          0x4,
                          0x2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxDynamicSlicerModeEnBfGet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDynamicSlicerModeEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x1D4),
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

int32_t adrv9025_OrxDynamicSlicerResendBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxDynamicSlicerResendBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDynamicSlicerResendBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1D4),
                          (uint8_t)bfValue,
                          0x10,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxDynamicSlicerSyncHeadBfSet(adi_adrv9025_Device_t*   device,
                                               adrv9025_BfOrxChanAddr_e baseAddr,
                                               uint32_t                 bfValue)
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
                         "Invalid adrv9025_OrxDynamicSlicerSyncHeadBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDynamicSlicerSyncHeadBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x1D3),
                         (uint8_t)(bfValue >> 24));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x1D2),
                         (uint8_t)(bfValue >> 16));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x1D1),
                         (uint8_t)(bfValue >> 8));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x1D0),
                         (uint8_t)(bfValue >> 0));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxDynamicSlicerSyncHeadBfGet(adi_adrv9025_Device_t*   device,
                                               adrv9025_BfOrxChanAddr_e baseAddr,
                                               uint32_t*                bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxDynamicSlicerSyncHeadBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x1D3),
                        &rxBfData[0x0],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x1D2),
                        &rxBfData[0x1],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x1D1),
                        &rxBfData[0x2],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x1D0),
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

int32_t adrv9025_OrxFpEnBfSet(adi_adrv9025_Device_t*   device,
                              adrv9025_BfOrxChanAddr_e baseAddr,
                              uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxFpEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x170),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpEnBfGet(adi_adrv9025_Device_t*   device,
                              adrv9025_BfOrxChanAddr_e baseAddr,
                              uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x170),
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

int32_t adrv9025_OrxFpExponentBitsBfSet(adi_adrv9025_Device_t*         device,
                                        adrv9025_BfOrxChanAddr_e       baseAddr,
                                        adrv9025_BfOrxFpExponentBits_e bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV9025_BF_RX1_EXP_2           ) &&
            (bfValue != ADRV9025_BF_RX1_EXP_3           ) &&
            (bfValue != ADRV9025_BF_RX1_EXP_4           ) &&
            (bfValue != ADRV9025_BF_RX1_EXP_5           )
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_OrxFpExponentBitsBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpExponentBitsBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x16E),
                          (uint8_t)bfValue,
                          0x6,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpExponentBitsBfGet(adi_adrv9025_Device_t*          device,
                                        adrv9025_BfOrxChanAddr_e        baseAddr,
                                        adrv9025_BfOrxFpExponentBits_e* bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpExponentBitsBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x16E),
                         &rxBfData[0],
                         0x6,
                         0xC1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (adrv9025_BfOrxFpExponentBits_e)data;
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpFloatDataFormatBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxFpFloatDataFormatBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpFloatDataFormatBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x16E),
                          (uint8_t)bfValue,
                          0x10,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpFloatDataFormatBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpFloatDataFormatBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x16E),
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

int32_t adrv9025_OrxFpHideLeadingOnesBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxFpHideLeadingOnesBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpHideLeadingOnesBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x16E),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpHideLeadingOnesBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpHideLeadingOnesBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x16E),
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

int32_t adrv9025_OrxFpIntDataAttenBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxFpIntDataAttenBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpIntDataAttenBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x16F),
                          (uint8_t)bfValue,
                          0x7,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpIntDataAttenBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpIntDataAttenBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x16F),
                         &rxBfData[0],
                         0x7,
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

int32_t adrv9025_OrxFpNanEncEnBfSet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxFpNanEncEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpNanEncEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x16E),
                          (uint8_t)bfValue,
                          0x8,
                          0x3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpNanEncEnBfGet(adi_adrv9025_Device_t*   device,
                                    adrv9025_BfOrxChanAddr_e baseAddr,
                                    uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpNanEncEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x16E),
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

int32_t adrv9025_OrxFpRoundModeBfSet(adi_adrv9025_Device_t*      device,
                                     adrv9025_BfOrxChanAddr_e    baseAddr,
                                     adrv9025_BfOrxFpRoundMode_e bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV9025_BF_ROUNDTIESTOEVEN     ) &&
            (bfValue != ADRV9025_BF_ROUNDTOWARDSPOSITIVE) &&
            (bfValue != ADRV9025_BF_ROUNDTOWARDSNEGATIVE) &&
            (bfValue != ADRV9025_BF_ROUNDTOWARDSZERO    ) &&
            (bfValue != ADRV9025_BF_ROUNDTIESTOAWAY     )
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_OrxFpRoundModeBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpRoundModeBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x16E),
                          (uint8_t)bfValue,
                          0xE0,
                          0x5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxFpRoundModeBfGet(adi_adrv9025_Device_t*       device,
                                     adrv9025_BfOrxChanAddr_e     baseAddr,
                                     adrv9025_BfOrxFpRoundMode_e* bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxFpRoundModeBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x16E),
                         &rxBfData[0],
                         0xE0,
                         0xC5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (adrv9025_BfOrxFpRoundMode_e)data;
    return device->common.error.newAction;
}

int32_t adrv9025_OrxGainCompEnableBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxGainCompEnableBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxGainCompEnableBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x3A),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxGainCompEnableBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxGainCompEnableBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x3A),
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

int32_t adrv9025_OrxGainCompForExtGainBfSet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxGainCompForExtGainBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxGainCompForExtGainBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x3D),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxGainCompForExtGainBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxGainCompForExtGainBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x3D),
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

int32_t adrv9025_OrxGainCompForTempGainBfSet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxGainCompForTempGainBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxGainCompForTempGainBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x3D),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxGainCompForTempGainBfGet(adi_adrv9025_Device_t*   device,
                                             adrv9025_BfOrxChanAddr_e baseAddr,
                                             uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxGainCompForTempGainBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x3D),
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

int32_t adrv9025_OrxIntDataFormatBfSet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntDataFormatBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntDataFormatBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x171),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntDataFormatBfGet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntDataFormatBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x171),
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

int32_t adrv9025_OrxIntDataResolutionBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntDataResolutionBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntDataResolutionBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x171),
                          (uint8_t)bfValue,
                          0x6,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntDataResolutionBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntDataResolutionBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x171),
                         &rxBfData[0],
                         0x6,
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

int32_t adrv9025_OrxIntEmbedSlicerBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntEmbedSlicerBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEmbedSlicerBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x171),
                          (uint8_t)bfValue,
                          0x8,
                          0x3);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntEmbedSlicerBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEmbedSlicerBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x171),
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

int32_t adrv9025_OrxIntEmbedSlicerNumberBfSet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntEmbedSlicerNumberBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEmbedSlicerNumberBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x171),
                          (uint8_t)bfValue,
                          0x20,
                          0x5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntEmbedSlicerNumberBfGet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEmbedSlicerNumberBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x171),
                         &rxBfData[0],
                         0x20,
                         0xC5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint8_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntEmbedSlicerPosBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntEmbedSlicerPosBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEmbedSlicerPosBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x171),
                          (uint8_t)bfValue,
                          0x10,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntEmbedSlicerPosBfGet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEmbedSlicerPosBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x171),
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

int32_t adrv9025_OrxIntEvenParityBfSet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntEvenParityBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEvenParityBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x171),
                          (uint8_t)bfValue,
                          0x80,
                          0x7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntEvenParityBfGet(adi_adrv9025_Device_t*   device,
                                       adrv9025_BfOrxChanAddr_e baseAddr,
                                       uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntEvenParityBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x171),
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

int32_t adrv9025_OrxIntParitySupportBfSet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntParitySupportBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntParitySupportBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x171),
                          (uint8_t)bfValue,
                          0x40,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntParitySupportBfGet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntParitySupportBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x171),
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

int32_t adrv9025_OrxIntSlicerLsbOnQBfSet(adi_adrv9025_Device_t*   device,
                                         adrv9025_BfOrxChanAddr_e baseAddr,
                                         uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxIntSlicerLsbOnQBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntSlicerLsbOnQBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x170),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxIntSlicerLsbOnQBfGet(adi_adrv9025_Device_t*   device,
                                         adrv9025_BfOrxChanAddr_e baseAddr,
                                         uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxIntSlicerLsbOnQBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x170),
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

int32_t adrv9025_OrxMaxSlicerOverrideBfSet(adi_adrv9025_Device_t*   device,
                                           adrv9025_BfOrxChanAddr_e baseAddr,
                                           uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxMaxSlicerOverrideBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxMaxSlicerOverrideBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1D5),
                          (uint8_t)bfValue,
                          0x80,
                          0x7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxOrx1AgcGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx1AgcGainIndexBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xBB),
                        &rxBfData[0],
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

int32_t adrv9025_OrxOrx1AgcManualGainIndexBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_OrxOrx1AgcManualGainIndexBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx1AgcManualGainIndexBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x75),
                         ((uint8_t)bfValue));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxOrx1AgcManualGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx1AgcManualGainIndexBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x75),
                        &rxBfData[0],
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

int32_t adrv9025_OrxOrx1AgcSetupBfSet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxOrx1AgcSetupBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx1AgcSetupBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x73),
                          (uint8_t)bfValue,
                          0x3,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxOrx1AgcSetupBfGet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx1AgcSetupBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x73),
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

int32_t adrv9025_OrxOrx2AgcGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                          adrv9025_BfOrxChanAddr_e baseAddr,
                                          uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx2AgcGainIndexBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0xBC),
                        &rxBfData[0],
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

int32_t adrv9025_OrxOrx2AgcManualGainIndexBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_OrxOrx2AgcManualGainIndexBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx2AgcManualGainIndexBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteWrite(device,
                         (baseAddr + 0x76),
                         ((uint8_t)bfValue));
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxOrx2AgcManualGainIndexBfGet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx2AgcManualGainIndexBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x76),
                        &rxBfData[0],
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

int32_t adrv9025_OrxOrx2AgcSetupBfSet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxOrx2AgcSetupBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx2AgcSetupBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x73),
                          (uint8_t)bfValue,
                          0xC,
                          0x2);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxOrx2AgcSetupBfGet(adi_adrv9025_Device_t*   device,
                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                      uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxOrx2AgcSetupBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x73),
                         &rxBfData[0],
                         0xC,
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

int32_t adrv9025_OrxRxTempGainCompBfSet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxRxTempGainCompBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxRxTempGainCompBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x20),
                          (uint8_t)bfValue,
                          0x7F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxRxTempGainCompBfGet(adi_adrv9025_Device_t*   device,
                                        adrv9025_BfOrxChanAddr_e baseAddr,
                                        uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxRxTempGainCompBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x20),
                         &rxBfData[0],
                         0x7F,
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

int32_t adrv9025_OrxRxdpSlicerPinCntrlGpioSelectBfGet(adi_adrv9025_Device_t*   device,
                                                      adrv9025_BfOrxChanAddr_e baseAddr,
                                                      uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxRxdpSlicerPinCntrlGpioSelectBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x197),
                         &rxBfData[0],
                         0x70,
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

int32_t adrv9025_OrxRxdpSlicerPositionBfGet(adi_adrv9025_Device_t*   device,
                                            adrv9025_BfOrxChanAddr_e baseAddr,
                                            uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxRxdpSlicerPositionBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x3C),
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

int32_t adrv9025_OrxSlicerPinControlStepBfSet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxSlicerPinControlStepBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxSlicerPinControlStepBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x3B),
                          (uint8_t)bfValue,
                          0x7,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_OrxSlicerPinControlStepBfGet(adi_adrv9025_Device_t*   device,
                                              adrv9025_BfOrxChanAddr_e baseAddr,
                                              uint8_t*                 bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxSlicerPinControlStepBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x3B),
                         &rxBfData[0],
                         0x7,
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

int32_t adrv9025_OrxStatic3bitSlicerModeEnBfSet(adi_adrv9025_Device_t*   device,
                                                adrv9025_BfOrxChanAddr_e baseAddr,
                                                uint8_t                  bfValue)
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
                         "Invalid adrv9025_OrxStatic3bitSlicerModeEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_ORX_CHANNELS) &&
        (baseAddr != ADRV9025_BF_ORX_CH0) &&
        (baseAddr != ADRV9025_BF_ORX_CH1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_OrxStatic3bitSlicerModeEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x1D4),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}
