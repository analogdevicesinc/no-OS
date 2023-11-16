/** 
 * \file adrv904x_bf_channel_filter.c Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#include "../../private/bf/adrv904x_bf_channel_filter.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_CHANNEL_FILTER

ADI_API adi_adrv904x_ErrAction_e adrv904x_ChannelFilter_CarrierBypassFilter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfChannelFilterChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CHANNEL_FILTER))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_ChannelFilter_CarrierBypassFilter_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3) &&
            (channelId != 0x4) &&
            (channelId != 0x5) &&
            (channelId != 0x6) &&
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_ChannelFilter_CarrierBypassFilter_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_ChannelFilter_CarrierEndBank_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfChannelFilterChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CHANNEL_FILTER) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CHANNEL_FILTER))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_ChannelFilter_CarrierEndBank_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) &&
            (channelId != 0x3) &&
            (channelId != 0x4) &&
            (channelId != 0x5) &&
            (channelId != 0x6) &&
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_ChannelFilter_CarrierEndBank_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

