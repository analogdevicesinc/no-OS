/** 
 * \file adrv904x_bf_serdes_txdig_phy_regmap_core1p2.c Automatically generated file with generator ver
 * 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_serdes_txdig_phy_regmap_core1p2.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_SERDES_TXDIG_PHY_REGMAP_CORE1P2

ADI_API adi_adrv904x_ErrAction_e adrv904x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet(adi_adrv904x_Device_t* const device,
                                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                                           const adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x7U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x20U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_SerdesTxdigPhyRegmapCore1p2_PdSer_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_SerdesTxdigPhyRegmapCore1p2_SerEnRc_BfGet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_1_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_2_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_3_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_4_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_5_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_6_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_7_) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_ALL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_SerdesTxdigPhyRegmapCore1p2_SerEnRc_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1U),
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

