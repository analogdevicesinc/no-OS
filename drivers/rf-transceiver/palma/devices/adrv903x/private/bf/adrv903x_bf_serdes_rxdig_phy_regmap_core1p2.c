/** 
 * \file adrv903x_bf_serdes_rxdig_phy_regmap_core1p2.c Automatically generated file with generator ver
 * 0.0.13.0.
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

#include "../../private/bf/adrv903x_bf_serdes_rxdig_phy_regmap_core1p2.h"
#include "adi_common_error.h"
#include "adi_adrv903x_error.h"

#include "adrv903x_bf_error_types.h"

#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_BF_SERDES_RXDIG_PHY_REGMAP_CORE1P2

ADI_API adi_adrv903x_ErrAction_e adrv903x_SerdesRxdigPhyRegmapCore1p2_AfePdVcm_BfSet(adi_adrv903x_Device_t* const device,
                                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                                     const adrv903x_BfSerdesRxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_SerdesRxdigPhyRegmapCore1p2_AfePdVcm_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_1_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_2_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_3_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_4_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_5_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_6_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_7_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_SerdesRxdigPhyRegmapCore1p2_AfePdVcm_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SerdesRxdigPhyRegmapCore1p2_RxdesAmuxSel_BfSet(adi_adrv903x_Device_t* const device,
                                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                                         const adrv903x_BfSerdesRxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
                                                                                         const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_SerdesRxdigPhyRegmapCore1p2_RxdesAmuxSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_1_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_2_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_3_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_4_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_5_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_6_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_7_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_SerdesRxdigPhyRegmapCore1p2_RxdesAmuxSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xEU),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

