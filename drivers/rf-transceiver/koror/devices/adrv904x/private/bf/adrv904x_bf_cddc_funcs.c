/** 
 * \file adrv904x_bf_cddc_funcs.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_cddc_funcs.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_CDDC_FUNCS

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcJesdClkDiv_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         const adrv904x_Bf_CddcFuncs_CddcJesdClkDiv_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_1                       ) &&
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_2                       ) &&
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_4                       ) &&
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_8                       ) &&
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_16                      ) &&
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_32                      ) &&
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_64                      ) &&
            (bfValue != ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_128                     )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcJesdClkDiv_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcJesdClkDiv_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1F0U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPeakToPowerMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPeakToPowerMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPeakToPowerMode_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPeakToPowerMode_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x184 + channelId * 12U),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPeakToPowerMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPeakToPowerMode_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPeakToPowerMode_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x184 + channelId * 12U),
                                                 &bfValueTmp,
                                                 0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 6);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPower_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPower_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x188 + channelId * 12U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerInputSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPowerInputSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerInputSelect_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerInputSelect_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x184 + channelId * 12U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x1EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerInputSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerInputSelect_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerInputSelect_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x184 + channelId * 12U),
                                                 &bfValueTmp,
                                                 0x1EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerLargestPeak_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerLargestPeak_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerLargestPeak_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x18C + channelId * 12U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                                   uint8_t channelId, 
                                                                                                   const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 65535U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x185 + channelId * 12),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x186 + channelId * 12),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfGet(adi_adrv904x_Device_t* const device,
                                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                                   uint8_t channelId, 
                                                                                                   uint16_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x185 + channelId * 12U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x186 + channelId * 12U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x187 + channelId * 12U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x3EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfGet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x187 + channelId * 12U),
                                                 &bfValueTmp,
                                                 0x3EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x184 + channelId * 12U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x184 + channelId * 12U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementPause_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPowerMeasurementPause_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementPause_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementPause_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x184 + channelId * 12U),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPowerMeasurementReadback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementReadback_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementReadback_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x189 + channelId * 12U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x180U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x180U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterCarrierSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterCarrierSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterCarrierSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x162U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterCarrierSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterCarrierSelect_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x162U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x168U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x168U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterFpEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x160U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x160U),
                                                 &bfValueTmp,
                                                 0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpExponentBits_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterFpExponentBits_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpExponentBits_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x161U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpExponentBits_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpExponentBits_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x161U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x161U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x161U),
                                                 &bfValueTmp,
                                                 0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x161U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x161U),
                                                 &bfValueTmp,
                                                 0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 3);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpNanEncEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterFpNanEncEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpNanEncEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x161U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpNanEncEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpNanEncEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x161U),
                                                 &bfValueTmp,
                                                 0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpRoundMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterFpRoundMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpRoundMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x161U),
                                                  ((uint32_t) bfValue << 5),
                                                  0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterFpRoundMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterFpRoundMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x161U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntDataFormat_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntDataFormat_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntDataFormat_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataResolution_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntDataResolution_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntDataResolution_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntDataResolution_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntDataResolution_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 7);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfGet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 3);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEvenParity_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntEvenParity_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEvenParity_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntEvenParity_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntEvenParity_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntParitySupport_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntParitySupport_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntParitySupport_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntParitySupport_BfGet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntParitySupport_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x16CU),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x16CU),
                                                 &bfValueTmp,
                                                 0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 6);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x170U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x3CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x170U),
                                                 &bfValueTmp,
                                                 0x3CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x170),
                                                  bfValue << 6,
                                                  0xC0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x171),
                                                  bfValue >> 2,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x170U),
                                                 &bfValueTmp,
                                                 0xC0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 6));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x171U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 2));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x171U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x3CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x171U),
                                                 &bfValueTmp,
                                                 0x3CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x171),
                                                  bfValue << 6,
                                                  0xC0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x172),
                                                  bfValue >> 2,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x171U),
                                                 &bfValueTmp,
                                                 0xC0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 6));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x172U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 2));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x170U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x170U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_FormatterStaticSlicerAgcSyncEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                                             const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_FormatterStaticSlicerAgcSyncEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_FormatterStaticSlicerAgcSyncEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x170U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 16383U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE + channelId * 44),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xF + channelId * 44),
                                                  bfValue >> 8,
                                                  0x3F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xF + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x3FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiDefaultMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiDefaultMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiDefaultMode_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiDefaultMode_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1 + channelId * 44U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiDefaultMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiDefaultMode_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiDefaultMode_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1 + channelId * 44U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiEnable_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiEnable_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x28 + channelId * 44U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiLatchLinearPowerReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiLatchLinearPowerReadback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiLatchLinearPowerReadback_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiLatchLinearPowerReadback_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x17 + channelId * 44U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiLinearPowerReadback_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint64_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint64_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiLinearPowerReadback_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiLinearPowerReadback_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x18 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint64_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x19 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint64_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint64_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint64_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 24));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1C + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint64_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 32));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiManualMeas_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiManualMeas_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiManualMeas_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiManualMeas_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1 + channelId * 44U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiManualPause_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiManualPause_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiManualPause_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiManualPause_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2 + channelId * 44U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration0_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMeasDuration0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration0_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5 + channelId * 44U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration0_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration0_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration1_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMeasDuration1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration1_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x7 + channelId * 44U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration1_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration1_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x7 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration2_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMeasDuration2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration2_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration2_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x9 + channelId * 44U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration2_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration2_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration2_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x9 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration3_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMeasDuration3_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration3_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration3_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xB + channelId * 44U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMeasDuration3_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration3_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMeasDuration3_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xB + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMode_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMode_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0 + channelId * 44U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x30U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMode_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMode_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x30U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor0_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1023U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMulFactor0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor0_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x4 + channelId * 44),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5 + channelId * 44),
                                                  bfValue >> 8,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor0_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor0_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor1_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1023U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMulFactor1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor1_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x6 + channelId * 44),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x7 + channelId * 44),
                                                  bfValue >> 8,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor1_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor1_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x6 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x7 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor2_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1023U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMulFactor2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor2_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor2_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8 + channelId * 44),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x9 + channelId * 44),
                                                  bfValue >> 8,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor2_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor2_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor2_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x8 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x9 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor3_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1023U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiMulFactor3_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor3_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor3_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA + channelId * 44),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xB + channelId * 44),
                                                  bfValue >> 8,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiMulFactor3_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor3_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiMulFactor3_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xB + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiPinMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiPinMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiPinMode_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiPinMode_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3 + channelId * 44U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiPinMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiPinMode_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiPinMode_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiResetOnGainChange_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiResetOnGainChange_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiResetOnGainChange_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiResetOnGainChange_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15 + channelId * 44U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiResetOnGainChange_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiResetOnGainChange_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiResetOnGainChange_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x15 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiRsTriggerSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiRsTriggerSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiRsTriggerSelect_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiRsTriggerSelect_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3 + channelId * 44U),
                                                  ((uint32_t) bfValue << 5),
                                                  0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiRsTriggerSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiRsTriggerSelect_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiRsTriggerSelect_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiStartDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 8191U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiStartDelay_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiStartDelay_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiStartDelay_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xC + channelId * 44),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xD + channelId * 44),
                                                  bfValue >> 8,
                                                  0x1F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiStartDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiStartDelay_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiStartDelay_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xC + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp)));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3 + channelId * 44U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiWaitDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_CddcFuncs_RssiWaitDelay_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiWaitDelay_BfSet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiWaitDelay_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10 + channelId * 44),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11 + channelId * 44),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x12 + channelId * 44),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncs_RssiWaitDelay_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfCddcFuncsChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CddcFuncs_RssiWaitDelay_BfGet Address");
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
                         "Invalid adrv904x_CddcFuncs_RssiWaitDelay_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10 + channelId * 44U),
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
                                                 (uint32_t) (baseAddr + 0x11 + channelId * 44U),
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
                                                 (uint32_t) (baseAddr + 0x12 + channelId * 44U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

