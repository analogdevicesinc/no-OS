// SPDX-License-Identifier: GPL-2.0
/**
 * \file Automatically generated file: adrv9025_bf_jesd_common.c
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

#include "./../../private/include/adrv9025_bf_jesd_common.h"
#include "./../../private/include/adrv9025_bf_hal.h"
#include "adi_common_error.h"

int32_t adrv9025_JesdCommonJrxTestLaneClearErrorsBfSet(adi_adrv9025_Device_t*          device,
                                                       adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                       uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJrxTestLaneClearErrorsBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestLaneClearErrorsBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x2C),
                          (uint8_t)bfValue,
                          0x1,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJrxTestLaneErrorCountBfGet(adi_adrv9025_Device_t*          device,
                                                      adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                      uint8_t                         channelId,
                                                      uint32_t*                       bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestLaneErrorCountBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_JesdCommonJrxTestLaneErrorCountBfGet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x5F + channelId * 1),
                        &rxBfData[0x0],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x4F + channelId * 1),
                        &rxBfData[0x1],
                        0x00);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x3F + channelId * 1),
                        &rxBfData[0x2],
                        0xC0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x3,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (uint32_t)data;
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJrxTestLaneErrorFlagBfGet(adi_adrv9025_Device_t*          device,
                                                     adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                     uint8_t                         channelId,
                                                     uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestLaneErrorFlagBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_JesdCommonJrxTestLaneErrorFlagBfGet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x2F + channelId * 1),
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

int32_t adrv9025_JesdCommonJrxTestLaneInvBfGet(adi_adrv9025_Device_t*          device,
                                               adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                               uint8_t                         channelId,
                                               uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestLaneInvBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_JesdCommonJrxTestLaneInvBfGet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x2F + channelId * 1),
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

int32_t adrv9025_JesdCommonJrxTestLaneInvalidDataFlagBfGet(adi_adrv9025_Device_t*          device,
                                                           adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                           uint8_t                         channelId,
                                                           uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestLaneInvalidDataFlagBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
#if ADRV9025_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3)
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelId,
                         "Invalid adrv9025_JesdCommonJrxTestLaneInvalidDataFlagBfGet channelId");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x2F + channelId * 1),
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

int32_t adrv9025_JesdCommonJrxTestLaneUpdateErrorCountBfSet(adi_adrv9025_Device_t*          device,
                                                            adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                            uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJrxTestLaneUpdateErrorCountBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestLaneUpdateErrorCountBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x2C),
                          (uint8_t)bfValue,
                          0x2,
                          0x1);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJrxTestModeBfSet(adi_adrv9025_Device_t*          device,
                                            adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                            uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJrxTestModeBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestModeBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x2E),
                          (uint8_t)bfValue,
                          0x7,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJrxTestModeBfGet(adi_adrv9025_Device_t*          device,
                                            adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                            uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestModeBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x2E),
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

int32_t adrv9025_JesdCommonJrxTestSampleClearErrorsBfSet(adi_adrv9025_Device_t*          device,
                                                         adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                         uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJrxTestSampleClearErrorsBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestSampleClearErrorsBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x2C),
                          bfValue,
                          0x20,
                          0x5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJrxTestSampleClearErrorsBfGet(adi_adrv9025_Device_t*          device,
                                                         adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                         uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestSampleClearErrorsBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x2C),
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

int32_t adrv9025_JesdCommonJrxTestSampleErrorCountBfGet(adi_adrv9025_Device_t*          device,
                                                        adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                        uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestSampleErrorCountBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfByteRead(device,
                        (baseAddr + 0x2D),
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

int32_t adrv9025_JesdCommonJrxTestSampleErrorFlagBfGet(adi_adrv9025_Device_t*          device,
                                                       adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                       uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestSampleErrorFlagBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x2C),
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

int32_t adrv9025_JesdCommonJrxTestSampleUpdateErrorCountBfSet(adi_adrv9025_Device_t*          device,
                                                              adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                              uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJrxTestSampleUpdateErrorCountBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestSampleUpdateErrorCountBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x2C),
                          (uint8_t)bfValue,
                          0x10,
                          0x4);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJrxTestSourceBfSet(adi_adrv9025_Device_t*          device,
                                              adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                              uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJrxTestSourceBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestSourceBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x2E),
                          (uint8_t)bfValue,
                          0x80,
                          0x7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJrxTestSourceBfGet(adi_adrv9025_Device_t*          device,
                                              adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                              uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxTestSourceBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x2E),
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

int32_t adrv9025_JesdCommonJtxLinkEnBfGet(adi_adrv9025_Device_t*          device,
                                          adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                          uint8_t*                        bfValue)
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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJtxLinkEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x89),
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

int32_t adrv9025_JesdCommonJtxOverrideSrstEnBfSet(adi_adrv9025_Device_t*          device,
                                                  adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                  uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJtxOverrideSrstEnBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJtxOverrideSrstEnBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x8A),
                          (uint8_t)bfValue,
                          0x40,
                          0x6);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJtxOverrideSrstEnBfGet(adi_adrv9025_Device_t*          device,
                                                  adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                  uint8_t*                        bfValue)
{
    uint8_t  rxBfData[8] = {0};
    uint64_t data        = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

#if ADRV9025_BITFIELD_NULL_CHECK > 0
    ADI_NULL_PTR_RETURN(&device->common,
                        bfValue);
#endif /* ADRV9025_BITFIELD_NULL_CHECK */

#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    /* Base Address check */
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJtxOverrideSrstEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif /* ADRV9025_BITFIELD_ADDR_CHECK */

    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Field */
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x8A),
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

int32_t adrv9025_JesdCommonJtxTriggerSrstNBfSet(adi_adrv9025_Device_t*          device,
                                                adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                uint8_t                         bfValue)
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
                         "Invalid adrv9025_JesdCommonJtxTriggerSrstNBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJtxTriggerSrstNBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x8A),
                          (uint8_t)bfValue,
                          0x80,
                          0x7);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_JesdCommonJtxTriggerSrstNBfGet(adi_adrv9025_Device_t*          device,
                                                adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                                uint8_t*                        bfValue)
{
    uint8_t  rxBfData[8] = {0};
    uint64_t data        = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

#if ADRV9025_BITFIELD_NULL_CHECK > 0
    ADI_NULL_PTR_RETURN(&device->common,
                        bfValue);
#endif /* ADRV9025_BITFIELD_NULL_CHECK */

#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJtxTriggerSrstNBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif /* ADRV9025_BITFIELD_ADDR_CHECK */

    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Field */
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x8A),
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

int32_t adrv9025_JesdCommonJrxLinkEnBfGet(adi_adrv9025_Device_t*          device,
                                          adrv9025_BfJesdCommonChanAddr_e baseAddr,
                                          uint8_t*                        bfValue)
{
    uint8_t  rxBfData[8] = {0};
    uint64_t data        = 0;

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
    if ((baseAddr != ADRV9025_BF_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_JesdCommonJrxLinkEnBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif /* ADRV9025_BITFIELD_ADDR_CHECK */

    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Field */
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x7C),
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
