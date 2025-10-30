/** 
 * \file adrv903x_bf_tx_datapath.c Automatically generated file with generator ver 0.0.13.0.
 * 
 * \brief Contains BitField functions to support ADRV903X transceiver device.
 * 
 * ADRV903X BITFIELD VERSION: 0.0.0.8
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2025 Analog Devices Inc.
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#include "../../private/bf/adrv903x_bf_tx_datapath.h"
#include "adi_common_error.h"
#include "adi_adrv903x_error.h"

#include "adrv903x_bf_error_types.h"

#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_BF_TX_DATAPATH

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDatapath_PfirCoeffDataI_BfSet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfTxDatapathChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint32_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4294967295U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_TxDatapath_PfirCoeffDataI_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_TX_0__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_1__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_2__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_3__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_4__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_5__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_6__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_7__TX_DATAPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataI_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

#if ADRV903X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3) &&
            (channelId != 0x4) &&
            (channelId != 0x5) &&
            (channelId != 0x6) &&
            (channelId != 0x7) &&
            (channelId != 0x8) &&
            (channelId != 0x9) &&
            (channelId != 0xA) &&
            (channelId != 0xB) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataI_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x18 + channelId * 4),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x19 + channelId * 4),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A + channelId * 4),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B + channelId * 4),
                                                  bfValue >> 24,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDatapath_PfirCoeffDataI_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfTxDatapathChanAddr_e baseAddr,
                                                                          uint8_t channelId,
                                                                          uint32_t* const bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV903X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_TX_0__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_1__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_2__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_3__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_4__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_5__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_6__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_7__TX_DATAPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataI_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

#if ADRV903X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3) &&
            (channelId != 0x4) &&
            (channelId != 0x5) &&
            (channelId != 0x6) &&
            (channelId != 0x7) &&
            (channelId != 0x8) &&
            (channelId != 0x9) &&
            (channelId != 0xA) &&
            (channelId != 0xB) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataI_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x18 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x19 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDatapath_PfirCoeffDataQ_BfSet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfTxDatapathChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          const uint32_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4294967295U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_TxDatapath_PfirCoeffDataQ_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_TX_0__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_1__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_2__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_3__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_4__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_5__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_6__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_7__TX_DATAPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataQ_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

#if ADRV903X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3) &&
            (channelId != 0x4) &&
            (channelId != 0x5) &&
            (channelId != 0x6) &&
            (channelId != 0x7) &&
            (channelId != 0x8) &&
            (channelId != 0x9) &&
            (channelId != 0xA) &&
            (channelId != 0xB) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataQ_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x48 + channelId * 4),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x49 + channelId * 4),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x4A + channelId * 4),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x4B + channelId * 4),
                                                  bfValue >> 24,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDatapath_PfirCoeffDataQ_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfTxDatapathChanAddr_e baseAddr,
                                                                          uint8_t channelId,
                                                                          uint32_t* const bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV903X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_TX_0__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_1__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_2__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_3__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_4__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_5__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_6__TX_DATAPATH) &&
            (baseAddr != ADRV903X_BF_SLICE_TX_7__TX_DATAPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataQ_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

#if ADRV903X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3) &&
            (channelId != 0x4) &&
            (channelId != 0x5) &&
            (channelId != 0x6) &&
            (channelId != 0x7) &&
            (channelId != 0x8) &&
            (channelId != 0x9) &&
            (channelId != 0xA) &&
            (channelId != 0xB) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_TxDatapath_PfirCoeffDataQ_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x48 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x49 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4A + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4B + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

