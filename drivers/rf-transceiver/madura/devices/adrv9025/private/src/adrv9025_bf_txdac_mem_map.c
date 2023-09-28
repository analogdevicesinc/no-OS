// SPDX-License-Identifier: GPL-2.0
/**
 * \file Automatically generated file: adrv9025_bf_txdac_mem_map.c
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

#include "./../../private/include/adrv9025_bf_txdac_mem_map.h"
#include "./../../private/include/adrv9025_bf_hal.h"
#include "adi_common_error.h"

int32_t adrv9025_TxdacMemMapTxdacFscBoostIBfSet(adi_adrv9025_Device_t*                 device,
                                                adrv9025_BfTxdacMemMapChanAddr_e       baseAddr,
                                                adrv9025_BfTxdacMemMapTxdacFscBoostI_e bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV9025_BF_TXDAC_IDAC_FSC4MA            ) &&
            (bfValue != ADRV9025_BF_TXDAC_IDAC_FSC5P6MA          )
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxdacMemMapTxdacFscBoostIBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TXDAC) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH1) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH2) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH3) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxdacMemMapTxdacFscBoostIBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x20),
                          (uint8_t)bfValue,
                          0x20,
                          0x5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxdacMemMapTxdacFscBoostIBfGet(adi_adrv9025_Device_t*                  device,
                                                adrv9025_BfTxdacMemMapChanAddr_e        baseAddr,
                                                adrv9025_BfTxdacMemMapTxdacFscBoostI_e* bfValue)
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
    if ((baseAddr != ADRV9025_BF_ALL_TXDAC) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH1) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH2) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH3) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxdacMemMapTxdacFscBoostIBfGet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfReadCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldRead(device,
                         (baseAddr + 0x20),
                         &rxBfData[0],
                         0x20,
                         0xC5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfReadAssembleData(device,
                                &rxBfData[0x0],
                                0x1,
                                &data);
    ADI_ERROR_RETURN(device->common.error.newAction);
    *bfValue = (adrv9025_BfTxdacMemMapTxdacFscBoostI_e)data;
    return device->common.error.newAction;
}

int32_t adrv9025_TxdacMemMapTxdacFscBoostQBfSet(adi_adrv9025_Device_t*                 device,
                                                adrv9025_BfTxdacMemMapChanAddr_e       baseAddr,
                                                adrv9025_BfTxdacMemMapTxdacFscBoostQ_e bfValue)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
#if ADRV9025_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV9025_BF_TXDAC_QDAC_FSC4MA            ) &&
            (bfValue != ADRV9025_BF_TXDAC_QDAC_FSC5P6MA          )
           )
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         bfValue,
                         "Invalid adrv9025_TxdacMemMapTxdacFscBoostQBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TXDAC) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH1) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH2) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH3) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxdacMemMapTxdacFscBoostQBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x21),
                          (uint8_t)bfValue,
                          0x20,
                          0x5);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxdacMemMapTxdacFscTuneIBfSet(adi_adrv9025_Device_t*           device,
                                               adrv9025_BfTxdacMemMapChanAddr_e baseAddr,
                                               uint8_t                          bfValue)
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
                         "Invalid adrv9025_TxdacMemMapTxdacFscTuneIBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TXDAC) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH1) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH2) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH3) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxdacMemMapTxdacFscTuneIBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x20),
                          (uint8_t)bfValue,
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}

int32_t adrv9025_TxdacMemMapTxdacFscTuneQBfSet(adi_adrv9025_Device_t*           device,
                                               adrv9025_BfTxdacMemMapChanAddr_e baseAddr,
                                               uint8_t                          bfValue)
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
                         "Invalid adrv9025_TxdacMemMapTxdacFscTuneQBfSet parameter");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
#if ADRV9025_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV9025_BF_ALL_TXDAC) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH1) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH2) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH3) &&
        (baseAddr != ADRV9025_BF_TXDAC_CH0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid adrv9025_TxdacMemMapTxdacFscTuneQBfSet Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif
    adrv9025_BfWriteCacheInit(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfFieldWrite(device,
                          (baseAddr + 0x21),
                          (uint8_t)bfValue,
                          0x1F,
                          0x0);
    ADI_ERROR_RETURN(device->common.error.newAction);
    adrv9025_BfWriteCacheFlush(device);
    ADI_ERROR_RETURN(device->common.error.newAction);
    return device->common.error.newAction;
}
