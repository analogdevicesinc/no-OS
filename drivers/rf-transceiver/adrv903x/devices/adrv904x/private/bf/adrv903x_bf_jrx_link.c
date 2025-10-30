/** 
 * \file adrv903x_bf_jrx_link.c Automatically generated file with generator ver 0.0.13.0.
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

#include "../../private/bf/adrv903x_bf_jrx_link.h"
#include "adi_common_error.h"
#include "adi_adrv903x_error.h"

#include "adrv903x_bf_error_types.h"

#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_BF_JRX_LINK

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreBufDepth_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreBufDepth_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA5U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreChksumCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                         uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreChksumCfg_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxCoreChksumCfg_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD4 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreClearSyncNeCount_BfSet(adi_adrv903x_Device_t* const device,
                                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                                const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JrxLink_JrxCoreClearSyncNeCount_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreClearSyncNeCount_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreConvSel_BfSet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 127U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_JrxLink_JrxCoreConvSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreConvSel_BfSet Address");
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
            (channelId != 0xB) &&
            (channelId != 0xC) &&
            (channelId != 0xD) &&
            (channelId != 0xE) &&
            (channelId != 0xF) &&
            (channelId != 0x10) &&
            (channelId != 0x11) &&
            (channelId != 0x12) &&
            (channelId != 0x13) &&
            (channelId != 0x14) &&
            (channelId != 0x15) &&
            (channelId != 0x16) &&
            (channelId != 0x17) &&
            (channelId != 0x18) &&
            (channelId != 0x19) &&
            (channelId != 0x1A) &&
            (channelId != 0x1B) &&
            (channelId != 0x1C) &&
            (channelId != 0x1D) &&
            (channelId != 0x1E) &&
            (channelId != 0x1F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxCoreConvSel_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x4 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x7FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreConvSel_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                       uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreConvSel_BfGet Address");
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
            (channelId != 0xB) &&
            (channelId != 0xC) &&
            (channelId != 0xD) &&
            (channelId != 0xE) &&
            (channelId != 0xF) &&
            (channelId != 0x10) &&
            (channelId != 0x11) &&
            (channelId != 0x12) &&
            (channelId != 0x13) &&
            (channelId != 0x14) &&
            (channelId != 0x15) &&
            (channelId != 0x16) &&
            (channelId != 0x17) &&
            (channelId != 0x18) &&
            (channelId != 0x19) &&
            (channelId != 0x1A) &&
            (channelId != 0x1B) &&
            (channelId != 0x1C) &&
            (channelId != 0x1D) &&
            (channelId != 0x1E) &&
            (channelId != 0x1F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxCoreConvSel_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreCsCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreCsCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD1U),
                                                 &bfValueTmp,
                                                 0xC0U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 6);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreDidCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                      const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreDidCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xACU),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreDscrCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreDscrCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xADU),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 7);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreECfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreECfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xF5U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreFCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreFCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xAEU),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreHdCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreHdCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD1U),
                                                 &bfValueTmp,
                                                 0x20U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreJesdvCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreJesdvCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD3U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreKCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreKCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xAFU),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreLaneSel_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                       uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreLaneSel_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxCoreLaneSel_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x84 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreLidCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                      const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                      uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreLidCfg_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxCoreLidCfg_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xB0 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreLCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreLCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xADU),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreMCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreMCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD0U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreNpCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreNpCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD2U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreNCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreNCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD1U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCorePclkErrorClear_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JrxLink_JrxCorePclkErrorClear_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCorePclkErrorClear_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0U),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCorePclkErrorClear_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCorePclkErrorClear_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0U),
                                                 &bfValueTmp,
                                                 0x20U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCorePclkFastError_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                             uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCorePclkFastError_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxCorePclkFastError_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x84 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 7);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCorePclkSlowError_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                             uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCorePclkSlowError_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxCorePclkSlowError_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x84 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x40U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 6);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCorePhaseAdjust_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                           const uint16_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 65535U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_JrxLink_JrxCorePhaseAdjust_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCorePhaseAdjust_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCorePhaseAdjust_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                           uint16_t* const bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV903X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCorePhaseAdjust_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp << 8);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCorePhaseDiff_BfGet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                         uint16_t* const bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV903X_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCorePhaseDiff_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA6U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp);

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA7U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp << 8);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSubclassvCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSubclassvCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD2U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSyncNeCount_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSyncNeCount_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSyncNSel_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSyncNSel_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefClrPhaseErr_BfSet(adi_adrv903x_Device_t* const device,
                                                                                 adi_adrv903x_SpiCache_t* const spiCache,
                                                                                 const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JrxLink_JrxCoreSysrefClrPhaseErr_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefClrPhaseErr_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA8U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefClrPhaseErr_BfGet(adi_adrv903x_Device_t* const device,
                                                                                 adi_adrv903x_SpiCache_t* const spiCache,
                                                                                 const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefClrPhaseErr_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA8U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefEnable_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JrxLink_JrxCoreSysrefEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA8U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefEnable_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA8U),
                                                 &bfValueTmp,
                                                 0x4U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefForRelink_BfGet(adi_adrv903x_Device_t* const device,
                                                                               adi_adrv903x_SpiCache_t* const spiCache,
                                                                               const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefForRelink_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0U),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 7);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefForStartup_BfGet(adi_adrv903x_Device_t* const device,
                                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                                const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefForStartup_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0U),
                                                 &bfValueTmp,
                                                 0x40U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 6);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefIgnoreWhenLinked_BfGet(adi_adrv903x_Device_t* const device,
                                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                                      const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefIgnoreWhenLinked_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA8U),
                                                 &bfValueTmp,
                                                 0x2U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefNShotCount_BfGet(adi_adrv903x_Device_t* const device,
                                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                                const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefNShotCount_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA9U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefNShotEnable_BfGet(adi_adrv903x_Device_t* const device,
                                                                                 adi_adrv903x_SpiCache_t* const spiCache,
                                                                                 const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefNShotEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA9U),
                                                 &bfValueTmp,
                                                 0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSysrefRcvd_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSysrefRcvd_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xAAU),
                                                 &bfValueTmp,
                                                 0x4U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreSCfg_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreSCfg_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD3U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxCoreUsrDataRdy_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxCoreUsrDataRdy_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xAAU),
                                                 &bfValueTmp,
                                                 0x2U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxLinkType_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfJrxLinkChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxLinkType_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JrxLink_JrxTplBufDepth_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJrxLinkChanAddr_e baseAddr,
                                                                       uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_) &&
            (baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JrxLink_JrxTplBufDepth_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_JrxLink_JrxTplBufDepth_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x87 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

