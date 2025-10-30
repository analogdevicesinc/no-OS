/** 
 * \file adrv903x_bf_actrl_orx_west_regmap.c Automatically generated file with generator ver 0.0.13.0.
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

#include "../../private/bf/adrv903x_bf_actrl_orx_west_regmap.h"
#include "adi_common_error.h"
#include "adi_adrv903x_error.h"

#include "adrv903x_bf_error_types.h"

#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_BF_ACTRL_ORX_WEST_REGMAP

ADI_API adi_adrv903x_ErrAction_e adrv903x_ActrlOrxWestRegmap_TrmAtten_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfActrlOrxWestRegmapChanAddr_e baseAddr,
                                                                            const adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_0DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_1DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_2DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_3DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_4DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_5DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_6DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_7DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_8DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_9DB                      ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_10DB                     ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_11DB                     ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_12DB                     ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_13DB                     ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_14DB                     ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_15DB                     ) &&
            (bfValue != ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_16DB                     )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_ActrlOrxWestRegmap_TrmAtten_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP) &&
            (baseAddr != ADRV903X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_ActrlOrxWestRegmap_TrmAtten_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3F0U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_ActrlOrxWestRegmap_TrmAtten_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfActrlOrxWestRegmapChanAddr_e baseAddr,
                                                                            adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e* const bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV903X_BITFIELD_NULL_CHECK */

    *bfValue = (adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e) 0;

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP) &&
            (baseAddr != ADRV903X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_ActrlOrxWestRegmap_TrmAtten_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3F0U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_ActrlOrxWestRegmap_TrmAttenCmCapEn_BfSet(adi_adrv903x_Device_t* const device,
                                                                                   adi_adrv903x_SpiCache_t* const spiCache,
                                                                                   const adrv903x_BfActrlOrxWestRegmapChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_ActrlOrxWestRegmap_TrmAttenCmCapEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP) &&
            (baseAddr != ADRV903X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_ActrlOrxWestRegmap_TrmAttenCmCapEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3F1U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_ActrlOrxWestRegmap_TrmAttenPd_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfActrlOrxWestRegmapChanAddr_e baseAddr,
                                                                              const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_ActrlOrxWestRegmap_TrmAttenPd_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP) &&
            (baseAddr != ADRV903X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_ActrlOrxWestRegmap_TrmAttenPd_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3F1U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_ActrlOrxWestRegmap_TrmAttenSpare2_BfSet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfActrlOrxWestRegmapChanAddr_e baseAddr,
                                                                                  const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 63U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_ActrlOrxWestRegmap_TrmAttenSpare2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP) &&
            (baseAddr != ADRV903X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_ActrlOrxWestRegmap_TrmAttenSpare2_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3F1U),
                                                  ((uint32_t) bfValue << 2),
                                                  0xFCU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

