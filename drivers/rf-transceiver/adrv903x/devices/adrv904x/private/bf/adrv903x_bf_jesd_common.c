/** 
 * \file adrv903x_bf_jesd_common.c Automatically generated file with generator ver 0.0.13.0.
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

#include "../../private/bf/adrv903x_bf_jesd_common.h"
#include "adi_common_error.h"
#include "adi_adrv903x_error.h"

#include "adrv903x_bf_error_types.h"

#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_BF_JESD_COMMON

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxCrcErrCntThreshold_BfSet(adi_adrv903x_Device_t* const device,
                                                                                 adi_adrv903x_SpiCache_t* const spiCache,
                                                                                 const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxCrcErrCntThreshold_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxCrcErrCntThreshold_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x116U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bBde_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bBde_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bBde_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bBdCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bBdCnt_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bBdCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A1 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bCgs_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bCgs_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bCgs_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bCks_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bCks_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bCks_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntEna_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxDl204bEcntEna_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntEna_BfSet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntEna_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x320 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntEna_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntEna_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntEna_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x320 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntRst_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxDl204bEcntRst_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntRst_BfSet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntRst_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x320 + channelId * 4U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntTch_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxDl204bEcntTch_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntTch_BfSet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntTch_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x321 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEcntTch_BfGet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntTch_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bEcntTch_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x321 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEofEvent_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEofEvent_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bEofEvent_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x423 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEomfEvent_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEomfEvent_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bEomfEvent_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x423 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bEth_BfSet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                        const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxDl204bEth_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bEth_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11CU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bFs_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bFs_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bFs_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x8U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 3);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bFsLost_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bFsLost_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bFsLost_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x423 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIld_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bIld_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bIld_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIls_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bIls_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bIls_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIrqClr_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           const uint16_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 511U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxDl204bIrqClr_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bIrqClr_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11A),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11B),
                                                  bfValue >> 8,
                                                  0x1);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIrqClr_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bIrqClr_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x11AU),
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
                                                 (uint32_t) (baseAddr + 0x11BU),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp << 8);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bIrqVec_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                           uint8_t channelId,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bIrqVec_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bIrqVec_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4A0 + channelId * 4U),
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
                                                 (uint32_t) (baseAddr + 0x4A1 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp << 8);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x120 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x121 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x222 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2A1 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x122 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x123 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A1 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A2 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A3 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x220 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x221 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bNit_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bNit_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bNit_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bNitCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bNitCnt_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bNitCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A3 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bSyncN_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bSyncN_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bSyncN_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x423 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x8U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 3);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bUek_BfGet(adi_adrv903x_Device_t* const device,
                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                        const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bUek_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bUek_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A0 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bUekCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bUekCnt_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bUekCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3A2 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204bUserData_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204bUserData_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204bUserData_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x423 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cClrErrCnt_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxDl204cClrErrCnt_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204cClrErrCnt_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x520U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cCrcErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204cCrcErrCnt_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204cCrcErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5A9 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cEmbErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204cEmbErrCnt_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204cEmbErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5AA + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xF0U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cHoldErrCnt_BfSet(adi_adrv903x_Device_t* const device,
                                                                               adi_adrv903x_SpiCache_t* const spiCache,
                                                                               const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxDl204cHoldErrCnt_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204cHoldErrCnt_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x520U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cMbErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204cMbErrCnt_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204cMbErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5AA + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cShErrCnt_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204cShErrCnt_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204cShErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x52A + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxDl204cState_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxDl204cState_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxDl204cState_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x529 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxLaneLoopback_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxLaneLoopback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxLaneLoopback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x114U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxLinkEn_BfSet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
                                                                     const uint8_t bfValue)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV903X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxLinkEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxLinkEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x114U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxLinkEn_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxLinkEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x114U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxSampleLoopback_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxSampleLoopback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxSampleLoopback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x114U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxSampleLoopback_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxSampleLoopback_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x114U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneClearErrors_BfSet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxTestLaneClearErrors_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestLaneClearErrors_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xCU),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneErrorCount_BfGet(adi_adrv903x_Device_t* const device,
                                                                                 adi_adrv903x_SpiCache_t* const spiCache,
                                                                                 const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestLaneErrorCount_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxTestLaneErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x11 + channelId * 4U),
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
                                                 (uint32_t) (baseAddr + 0x12 + channelId * 4U),
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
                                                 (uint32_t) (baseAddr + 0x13 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneErrorFlag_BfGet(adi_adrv903x_Device_t* const device,
                                                                                adi_adrv903x_SpiCache_t* const spiCache,
                                                                                const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestLaneErrorFlag_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxTestLaneErrorFlag_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneInv_BfGet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestLaneInv_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxTestLaneInv_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet(adi_adrv903x_Device_t* const device,
                                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                                      const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet Address");
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
                         "Invalid adrv903x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10 + channelId * 4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet(adi_adrv903x_Device_t* const device,
                                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xCU),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestMode_BfSet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxTestMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xEU),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestMode_BfGet(adi_adrv903x_Device_t* const device,
                                                                       adi_adrv903x_SpiCache_t* const spiCache,
                                                                       const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xEU),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleClearErrors_BfGet(adi_adrv903x_Device_t* const device,
                                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                                    const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestSampleClearErrors_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xCU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleErrorCount_BfGet(adi_adrv903x_Device_t* const device,
                                                                                   adi_adrv903x_SpiCache_t* const spiCache,
                                                                                   const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestSampleErrorCount_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xDU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleErrorFlag_BfGet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestSampleErrorFlag_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xCU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet(adi_adrv903x_Device_t* const device,
                                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                                         const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xCU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSource_BfSet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JrxTestSource_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestSource_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xEU),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JrxTestSource_BfGet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JrxTestSource_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xEU),
                                                 &bfValueTmp,
                                                 0x8U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 3);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxLinkEn_BfSet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JtxLinkEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JtxLinkEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5F0U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxLinkEn_BfGet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_SpiCache_t* const spiCache,
                                                                     const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JtxLinkEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5F0U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxSampleLoopback_BfSet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_JesdCommon_JtxSampleLoopback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JtxSampleLoopback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5F0U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_JesdCommon_JtxSampleLoopback_BfGet(adi_adrv903x_Device_t* const device,
                                                                             adi_adrv903x_SpiCache_t* const spiCache,
                                                                             const adrv903x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_JesdCommon_JtxSampleLoopback_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5F0U),
                                                 &bfValueTmp,
                                                 0x70U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

