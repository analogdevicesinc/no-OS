/** 
 * \file adrv903x_bf_serdes_txdig_phy_regmap_core1p2.c Automatically generated file with generator ver
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

#include "../../private/bf/adrv903x_bf_serdes_txdig_phy_regmap_core1p2.h"
#include "adi_common_error.h"
#include "adi_adrv903x_error.h"

#include "adrv903x_bf_error_types.h"

#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_BF_SERDES_TXDIG_PHY_REGMAP_CORE1P2

ADI_API adi_adrv903x_ErrAction_e adrv903x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet(adi_adrv903x_Device_t* const device,
                                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                                           const adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
                                                                                           const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x7U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet(adi_adrv903x_Device_t* const device,
                                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                                          const adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
                                                                                          const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x20U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfGet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV903X_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_SerdesTxdigPhyRegmapCore1p2_SerEnRc_BfGet(adi_adrv903x_Device_t* const device,
                                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                                    const adrv903x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV903X_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_SerdesTxdigPhyRegmapCore1p2_SerEnRc_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

