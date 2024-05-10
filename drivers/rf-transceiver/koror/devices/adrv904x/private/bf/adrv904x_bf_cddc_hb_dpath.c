/** 
 * \file adrv904x_bf_cddc_hb_dpath.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_cddc_hb_dpath.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_CDDC_HB_DPATH

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_BandSource_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       const adrv904x_Bf_CddcHbDpath_BandSource_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_BAND_SOURCE_ENUM000                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_BAND_SOURCE_ENUM001                        )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcHbDpath_BandSource_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_BandSource_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_BandSource_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0 + channelId * 28U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE0U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE0U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain0_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain0_BfSet Address");
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
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain0_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain0_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x140U),
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
                                                 (uint32_t) (baseAddr + 0x141U),
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
                                                 (uint32_t) (baseAddr + 0x142U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain1_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain1_BfSet Address");
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
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain1_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain1_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x144U),
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
                                                 (uint32_t) (baseAddr + 0x145U),
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
                                                 (uint32_t) (baseAddr + 0x146U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain2_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain2_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x148),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x149),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14A),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain2_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain2_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x148U),
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
                                                 (uint32_t) (baseAddr + 0x149U),
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
                                                 (uint32_t) (baseAddr + 0x14AU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain3_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain3_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain3_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14C),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14D),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14E),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain3_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain3_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x14CU),
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
                                                 (uint32_t) (baseAddr + 0x14DU),
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
                                                 (uint32_t) (baseAddr + 0x14EU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain4_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain4_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain4_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x150),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x151),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x152),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain4_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain4_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x150U),
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
                                                 (uint32_t) (baseAddr + 0x151U),
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
                                                 (uint32_t) (baseAddr + 0x152U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain5_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain5_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain5_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x154),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x155),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x156),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain5_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain5_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x154U),
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
                                                 (uint32_t) (baseAddr + 0x155U),
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
                                                 (uint32_t) (baseAddr + 0x156U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain6_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain6_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain6_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x158),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x159),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15A),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain6_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain6_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x158U),
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
                                                 (uint32_t) (baseAddr + 0x159U),
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
                                                 (uint32_t) (baseAddr + 0x15AU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain7_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGain7_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain7_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15C),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15D),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15E),
                                                  bfValue >> 16,
                                                  0x7F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGain7_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGain7_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x15CU),
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
                                                 (uint32_t) (baseAddr + 0x15DU),
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
                                                 (uint32_t) (baseAddr + 0x15EU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierGainEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierGainEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierGainEnable_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_CarrierGainEnable_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2 + channelId * 28U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect0_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect0_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x138U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect1_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect1_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x138U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect2_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect2_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x139U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect3_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect3_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect3_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x139U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect4_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect4_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect4_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13AU),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect5_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect5_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect5_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13AU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect6_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect6_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect6_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13BU),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_CarrierSelect7_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_CarrierSelect7_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_CarrierSelect7_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13BU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DaisyChainEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_DaisyChainEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_DaisyChainEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13EU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DecRate_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const adrv904x_Bf_CddcHbDpath_DecRate_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM000                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM001                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM002                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM003                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM004                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM005                        )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcHbDpath_DecRate_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_DecRate_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_DecRate_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0 + channelId * 28U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x3CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DelayCmpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_DelayCmpEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_DelayCmpEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13CU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_DelayMemEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_DelayMemEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_DelayMemEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13DU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_FreqKhz_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_FreqKhz_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_FreqKhz_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_FreqKhz_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10 + channelId * 28),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11 + channelId * 28),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x12 + channelId * 28),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_InterleaverMaxSlot_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_InterleaverMaxSlot_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_InterleaverMaxSlot_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xFCU),
                                                  (uint32_t) bfValue,
                                                  0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_InterleaverSlotValidLower_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_InterleaverSlotValidLower_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_InterleaverSlotValidLower_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x120),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x121),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x122),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x123),
                                                  bfValue >> 24,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_InterleaverSlotValidUpper_BfSet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_InterleaverSlotValidUpper_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_InterleaverSlotValidUpper_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x124),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x125),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x126),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x127),
                                                  bfValue >> 24,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_MixerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_MixerEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_MixerEn_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_MixerEn_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1 + channelId * 28U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_NcoEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_NcoEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_NcoEnable_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_NcoEnable_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13 + channelId * 28U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_NcoFtwUpdate_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_NcoFtwUpdate_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_NcoFtwUpdate_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_NcoFtwUpdate_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13 + channelId * 28U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_OneMsClocks_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcHbDpath_OneMsClocks_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_OneMsClocks_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_OneMsClocks_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14 + channelId * 28),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15 + channelId * 28),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16 + channelId * 28),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpath_UpsamplerRatio_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfCddcHbDpathChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           const adrv904x_Bf_CddcHbDpath_UpsamplerRatio_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM000                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM001                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM002                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM003                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM004                        ) &&
            (bfValue != ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM005                        )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcHbDpath_UpsamplerRatio_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcHbDpath_UpsamplerRatio_BfSet Address");
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
                         "Invalid adrv904x_CddcHbDpath_UpsamplerRatio_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0 + channelId * 28),
                                                  bfValue << 6,
                                                  0xC0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1 + channelId * 28),
                                                  bfValue >> 2,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

