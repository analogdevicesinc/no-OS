/** 
 * \file adrv904x_bf_cduc_hb_dpath.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_cduc_hb_dpath.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_CDUC_HB_DPATH

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_BandDest_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const adrv904x_Bf_CducHbDpath_BandDest_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_BAND_DEST_ENUM000                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_BAND_DEST_ENUM001                        )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_BandDest_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_BandDest_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_BandDest_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0 + channelId * 32U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain0_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain0_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x160),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x161),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x162),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain0_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain0_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x160U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x161U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x162U),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain1_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain1_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x164),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x165),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x166),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain1_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain1_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x164U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x165U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x166U),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain2_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain2_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x168),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x169),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16A),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain2_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain2_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x168U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x169U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16AU),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain3_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain3_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain3_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16C),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16D),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16E),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain3_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain3_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16DU),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16EU),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain4_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain4_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain4_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x170),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x171),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x172),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain4_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain4_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x170U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x171U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x172U),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain5_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain5_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain5_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x174),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x175),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x176),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain5_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain5_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x174U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x175U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x176U),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain6_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain6_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain6_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x178),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x179),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x17A),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain6_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain6_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x178U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x179U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x17AU),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain7_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8388607U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGain7_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain7_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x17C),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x17D),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x17E),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGain7_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint32_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGain7_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x17CU),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x17DU),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x17EU),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierGainEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierGainEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierGainEnable_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_CarrierGainEnable_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2 + channelId * 32U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect0_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect0_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x158U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect1_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect1_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x158U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect2_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect2_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x159U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect3_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect3_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect3_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x159U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect4_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect4_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect4_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15AU),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect5_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect5_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect5_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15AU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect6_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect6_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect6_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15BU),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrierSelect7_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                           const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrierSelect7_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrierSelect7_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15BU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand0Attn_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                          const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrBand0Attn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrBand0Attn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x180U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand0Attn_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrBand0Attn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x180U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand1Attn_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                          const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CarrBand1Attn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrBand1Attn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x182U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CarrBand1Attn_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CarrBand1Attn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x182U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CducCarrierEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                              const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CducCarrierEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CducCarrierEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x100U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CducCarrierEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CducCarrierEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x100U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_CducClkToCarrierRate_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const adrv904x_Bf_CducHbDpath_CducClkToCarrierRate_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM000                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM001                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM002                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM003                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM004                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM005                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM006                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM007                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM008                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM009                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM010                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM011                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM012                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM013                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM014                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM015                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM016                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM017                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM018                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM019                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM020                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM021                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM022                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM023                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM024                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM025                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM026                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM027                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM028                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM029                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM030                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM031                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM032                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM033                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM034                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM035                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM036                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM037                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM038                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM039                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM040                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM041                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM042                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM043                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM044                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM045                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM046                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM047                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM048                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM049                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM050                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM051                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM052                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM053                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM054                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM055                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM056                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM057                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM058                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM059                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM060                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM061                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM062                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM063                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM064                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM065                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM066                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM067                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM068                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM069                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM070                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM071                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM072                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM073                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM074                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM075                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM076                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM077                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM078                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM079                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM080                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM081                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM082                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM083                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM084                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM085                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM086                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM087                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM088                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM089                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM090                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM091                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM092                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM093                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM094                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM095                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM096                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM097                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM098                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM099                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM100                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM101                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM102                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM103                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM104                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM105                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM106                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM107                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM108                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM109                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM110                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM111                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM112                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM113                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM114                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM115                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM116                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM117                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM118                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM119                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM120                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM121                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM122                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM123                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM124                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM125                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM126                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM127                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM128                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM129                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM130                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM131                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM132                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM133                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM134                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM135                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM136                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM137                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM138                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM139                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM140                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM141                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM142                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM143                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM144                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM145                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM146                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM147                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM148                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM149                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM150                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM151                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM152                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM153                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM154                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM155                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM156                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM157                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM158                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM159                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM160                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM161                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM162                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM163                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM164                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM165                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM166                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM167                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM168                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM169                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM170                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM171                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM172                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM173                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM174                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM175                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM176                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM177                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM178                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM179                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM180                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM181                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM182                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM183                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM184                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM185                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM186                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM187                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM188                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM189                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM190                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM191                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM192                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM193                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM194                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM195                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM196                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM197                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM198                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM199                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM200                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM201                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM202                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM203                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM204                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM205                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM206                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM207                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM208                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM209                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM210                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM211                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM212                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM213                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM214                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM215                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM216                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM217                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM218                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM219                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM220                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM221                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM222                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM223                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM224                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM225                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM226                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM227                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM228                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM229                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM230                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM231                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM232                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM233                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM234                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM235                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM236                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM237                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM238                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM239                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM240                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM241                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM242                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM243                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM244                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM245                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM246                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM247                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM248                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM249                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM250                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM251                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM252                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM253                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM254                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM255                        )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_CducClkToCarrierRate_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_CducClkToCarrierRate_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_CducClkToCarrierRate_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1C + channelId * 32U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DaisyChainEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_DaisyChainEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_DaisyChainEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15EU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DeinterleaverMaxSlot_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 127U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_DeinterleaverMaxSlot_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_DeinterleaverMaxSlot_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11CU),
                                                  (uint32_t) bfValue,
                                                  0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DeinterleaverSlotValidLower_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                        const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4294967295U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_DeinterleaverSlotValidLower_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_DeinterleaverSlotValidLower_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x144),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x145),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x146),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x147),
                                                  bfValue >> 24,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DeinterleaverSlotValidUpper_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                                        const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4294967295U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_DeinterleaverSlotValidUpper_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_DeinterleaverSlotValidUpper_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x140),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x141),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x142),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x143),
                                                  bfValue >> 24,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DelayCompEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                        const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_DelayCompEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_DelayCompEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15CU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DelayMemEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                       const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_DelayMemEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_DelayMemEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15DU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_DownsamplerRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const adrv904x_Bf_CducHbDpath_DownsamplerRatio_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM000                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM001                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM002                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM003                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM004                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM005                        )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_DownsamplerRatio_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_DownsamplerRatio_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_DownsamplerRatio_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0 + channelId * 32),
                                                  bfValue << 6,
                                                  0xC0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1 + channelId * 32),
                                                  bfValue >> 2,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_FreqKhz_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 16777215U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_FreqKhz_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_FreqKhz_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_FreqKhz_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10 + channelId * 32),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11 + channelId * 32),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x12 + channelId * 32),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_IntRate_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const adrv904x_Bf_CducHbDpath_IntRate_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM000                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM001                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM002                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM003                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM004                        ) &&
            (bfValue != ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM005                        )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_IntRate_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_IntRate_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_IntRate_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0 + channelId * 32U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x3CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_MixerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_MixerEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_MixerEn_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_MixerEn_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1 + channelId * 32U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_NcoEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_NcoEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_NcoEnable_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_NcoEnable_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13 + channelId * 32U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_NcoFtwUpdate_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_NcoFtwUpdate_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_NcoFtwUpdate_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_NcoFtwUpdate_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13 + channelId * 32U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducHbDpath_OneMsClocks_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCducHbDpathChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 16777215U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducHbDpath_OneMsClocks_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducHbDpath_OneMsClocks_BfSet Address");
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
                         "Invalid adrv904x_CducHbDpath_OneMsClocks_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14 + channelId * 32),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15 + channelId * 32),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16 + channelId * 32),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

