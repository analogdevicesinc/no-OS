/** 
 * \file adrv904x_bf_jesd_common.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_jesd_common.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_JESD_COMMON

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CddcBypassMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_CddcBypassMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_CddcBypassMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x612U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CddcBypassMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_CddcBypassMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x612U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CducBypassMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_CducBypassMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_CducBypassMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x138U),
                                                  ((uint32_t) bfValue << 2),
                                                  0xCU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_CducBypassMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_CducBypassMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x138U),
                                                 &bfValueTmp,
                                                 0xCU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxChannelSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1601 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1601 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxChannelSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B81 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxChannelSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxChannelSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B81 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1602 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1602 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B82 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B82 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxCrcErrCntThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxCrcErrCntThreshold_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxCrcErrCntThreshold_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13AU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bBde_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bBde_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bBde_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bBdCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bBdCnt_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bBdCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C9 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bCgs_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bCgs_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bCgs_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bCks_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bCks_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bCks_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntEna_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxDl204bEcntEna_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntEna_BfSet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntEna_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x348 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntEna_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntEna_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntEna_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x348 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x348 + channelId * 4U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntTch_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxDl204bEcntTch_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntTch_BfSet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntTch_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x349 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEcntTch_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntTch_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bEcntTch_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x349 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEofEvent_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEofEvent_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bEofEvent_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x44B + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEomfEvent_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEomfEvent_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bEomfEvent_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x44B + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bEth_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxDl204bEth_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bEth_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x144U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bFs_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bFs_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bFs_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bFsLost_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bFsLost_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bFsLost_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x44B + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIld_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bIld_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bIld_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIls_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bIls_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bIls_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                           const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 511U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x142),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x143),
                                                  bfValue >> 8,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIrqClr_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bIrqClr_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x142U),
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
                                                 (uint32_t) (baseAddr + 0x143U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bIrqVec_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bIrqVec_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bIrqVec_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4C8 + channelId * 4U),
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
                                                 (uint32_t) (baseAddr + 0x4C9 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x148 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x149 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x24A + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2C9 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x14A + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x14B + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1C9 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1CA + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1CB + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x248 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x249 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bNit_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bNit_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bNit_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bNitCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bNitCnt_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bNitCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3CB + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bUek_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bUek_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bUek_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3C8 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bUekCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bUekCnt_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bUekCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3CA + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204bUserData_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204bUserData_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204bUserData_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x44B + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cClrErrCnt_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxDl204cClrErrCnt_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204cClrErrCnt_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x548U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cCrcErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204cCrcErrCnt_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204cCrcErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5D1 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cEmbErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204cEmbErrCnt_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204cEmbErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5D2 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cMbErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204cMbErrCnt_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204cMbErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x5D2 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cShErrCnt_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204cShErrCnt_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204cShErrCnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x552 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxDl204cState_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxDl204cState_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxDl204cState_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x551 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxLaneLoopback_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxLaneLoopback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxLaneLoopback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x33U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxLinkEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxLinkEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxLinkEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x138U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxLinkEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxLinkEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x138U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxSampleLoopback_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxSampleLoopback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxSampleLoopback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x33U),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxSampleLoopback_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxSampleLoopback_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x33U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneClearErrors_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxTestLaneClearErrors_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestLaneClearErrors_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x30U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneErrorCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestLaneErrorCount_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxTestLaneErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x35 + channelId * 4U),
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
                                                 (uint32_t) (baseAddr + 0x36 + channelId * 4U),
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
                                                 (uint32_t) (baseAddr + 0x37 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneErrorFlag_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestLaneErrorFlag_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxTestLaneErrorFlag_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneInv_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestLaneInv_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxTestLaneInv_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet Address");
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
                         "Invalid adrv904x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x30U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxTestMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x32U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x32U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleClearErrors_BfGet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestSampleClearErrors_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x30U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleErrorCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestSampleErrorCount_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x31U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleErrorFlag_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestSampleErrorFlag_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x30U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                                         const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x30U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSource_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JrxTestSource_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestSource_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x32U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JrxTestSource_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JrxTestSource_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x32U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JtxChannelSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA01 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA01 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JtxChannelSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xF81 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxChannelSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxChannelSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xF81 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA02 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA02 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xF82 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xF82 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfSet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1182 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfGet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1182 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxLinkEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JtxLinkEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxLinkEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x610U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxLinkEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxLinkEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x610U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxSampleLoopback_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_JtxSampleLoopback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxSampleLoopback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x610U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_JtxSampleLoopback_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_JtxSampleLoopback_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x610U),
                                                 &bfValueTmp,
                                                 0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_OrxSampleSelLink2_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_OrxSampleSelLink2_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_OrxSampleSelLink2_BfSet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_OrxSampleSelLink2_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1180 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_OrxSampleSelLink2_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_OrxSampleSelLink2_BfGet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_OrxSampleSelLink2_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1180 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x700 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x700 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE00 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE00 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x700 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x701 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x700 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x701 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE00 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE01 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE00 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE01 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x701 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x701 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE01 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE01 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x701 + channelId * 4),
                                                  bfValue << 7,
                                                  0x80);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x702 + channelId * 4),
                                                  bfValue >> 1,
                                                  0xF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x701 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 7));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x702 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 1));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE01 + channelId * 4),
                                                  bfValue << 7,
                                                  0x80);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE02 + channelId * 4),
                                                  bfValue >> 1,
                                                  0xF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE01 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 7));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE02 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 1));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x702 + channelId * 4),
                                                  bfValue << 4,
                                                  0xF0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x703 + channelId * 4),
                                                  bfValue >> 4,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x702 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 4));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x703 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 4));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE02 + channelId * 4),
                                                  bfValue << 4,
                                                  0xF0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE03 + channelId * 4),
                                                  bfValue >> 4,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE02 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 4));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE03 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 4));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x703 + channelId * 4U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x3EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x703 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE03 + channelId * 4U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x3EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xE03 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA00 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA00 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xF80 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xF80 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA00 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA01 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA00 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xA01 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xF80 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xF81 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xF80 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xF81 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 63U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_JesdCommon_RxSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xD00 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x3FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xD00 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 63U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_JesdCommon_RxSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink1_BfSet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1100 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x3FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_RxSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink1_BfGet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_RxSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1100 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1300 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1300 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A00 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A00 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1300 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1301 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1300 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1301 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A00 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A01 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A00 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A01 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1301 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1301 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A01 + channelId * 4U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x7CU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A01 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1301 + channelId * 4),
                                                  bfValue << 7,
                                                  0x80);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1302 + channelId * 4),
                                                  bfValue >> 1,
                                                  0xF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1301 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 7));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1302 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 1));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A01 + channelId * 4),
                                                  bfValue << 7,
                                                  0x80);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A02 + channelId * 4),
                                                  bfValue >> 1,
                                                  0xF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A01 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 7));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A02 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 1));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1302 + channelId * 4),
                                                  bfValue << 4,
                                                  0xF0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1303 + channelId * 4),
                                                  bfValue >> 4,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1302 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 4));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1303 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 4));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A02 + channelId * 4),
                                                  bfValue << 4,
                                                  0xF0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A03 + channelId * 4),
                                                  bfValue >> 4,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A02 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 4));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A03 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 4));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1303 + channelId * 4U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x3EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1303 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A03 + channelId * 4U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x3EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1A03 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1600 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1600 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B80 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B80 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1600 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1601 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) &&
            (channelId != 0x60) &&
            (channelId != 0x61) &&
            (channelId != 0x62) &&
            (channelId != 0x63) &&
            (channelId != 0x64) &&
            (channelId != 0x65) &&
            (channelId != 0x66) &&
            (channelId != 0x67) &&
            (channelId != 0x68) &&
            (channelId != 0x69) &&
            (channelId != 0x6A) &&
            (channelId != 0x6B) &&
            (channelId != 0x6C) &&
            (channelId != 0x6D) &&
            (channelId != 0x6E) &&
            (channelId != 0x6F) &&
            (channelId != 0x70) &&
            (channelId != 0x71) &&
            (channelId != 0x72) &&
            (channelId != 0x73) &&
            (channelId != 0x74) &&
            (channelId != 0x75) &&
            (channelId != 0x76) &&
            (channelId != 0x77) &&
            (channelId != 0x78) &&
            (channelId != 0x79) &&
            (channelId != 0x7A) &&
            (channelId != 0x7B) &&
            (channelId != 0x7C) &&
            (channelId != 0x7D) &&
            (channelId != 0x7E) &&
            (channelId != 0x7F) &&
            (channelId != 0x80) &&
            (channelId != 0x81) &&
            (channelId != 0x82) &&
            (channelId != 0x83) &&
            (channelId != 0x84) &&
            (channelId != 0x85) &&
            (channelId != 0x86) &&
            (channelId != 0x87) &&
            (channelId != 0x88) &&
            (channelId != 0x89) &&
            (channelId != 0x8A) &&
            (channelId != 0x8B) &&
            (channelId != 0x8C) &&
            (channelId != 0x8D) &&
            (channelId != 0x8E) &&
            (channelId != 0x8F) &&
            (channelId != 0x90) &&
            (channelId != 0x91) &&
            (channelId != 0x92) &&
            (channelId != 0x93) &&
            (channelId != 0x94) &&
            (channelId != 0x95) &&
            (channelId != 0x96) &&
            (channelId != 0x97) &&
            (channelId != 0x98) &&
            (channelId != 0x99) &&
            (channelId != 0x9A) &&
            (channelId != 0x9B) &&
            (channelId != 0x9C) &&
            (channelId != 0x9D) &&
            (channelId != 0x9E) &&
            (channelId != 0x9F) &&
            (channelId != 0xA0) &&
            (channelId != 0xA1) &&
            (channelId != 0xA2) &&
            (channelId != 0xA3) &&
            (channelId != 0xA4) &&
            (channelId != 0xA5) &&
            (channelId != 0xA6) &&
            (channelId != 0xA7) &&
            (channelId != 0xA8) &&
            (channelId != 0xA9) &&
            (channelId != 0xAA) &&
            (channelId != 0xAB) &&
            (channelId != 0xAC) &&
            (channelId != 0xAD) &&
            (channelId != 0xAE) &&
            (channelId != 0xAF) &&
            (channelId != 0xB0) &&
            (channelId != 0xB1) &&
            (channelId != 0xB2) &&
            (channelId != 0xB3) &&
            (channelId != 0xB4) &&
            (channelId != 0xB5) &&
            (channelId != 0xB6) &&
            (channelId != 0xB7) &&
            (channelId != 0xB8) &&
            (channelId != 0xB9) &&
            (channelId != 0xBA) &&
            (channelId != 0xBB) &&
            (channelId != 0xBC) &&
            (channelId != 0xBD) &&
            (channelId != 0xBE) &&
            (channelId != 0xBF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1600 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1601 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfSet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B80 + channelId * 4),
                                                  bfValue << 5,
                                                  0xE0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B81 + channelId * 4),
                                                  bfValue >> 3,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfGet Address");
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
            (channelId != 0x1F) &&
            (channelId != 0x20) &&
            (channelId != 0x21) &&
            (channelId != 0x22) &&
            (channelId != 0x23) &&
            (channelId != 0x24) &&
            (channelId != 0x25) &&
            (channelId != 0x26) &&
            (channelId != 0x27) &&
            (channelId != 0x28) &&
            (channelId != 0x29) &&
            (channelId != 0x2A) &&
            (channelId != 0x2B) &&
            (channelId != 0x2C) &&
            (channelId != 0x2D) &&
            (channelId != 0x2E) &&
            (channelId != 0x2F) &&
            (channelId != 0x30) &&
            (channelId != 0x31) &&
            (channelId != 0x32) &&
            (channelId != 0x33) &&
            (channelId != 0x34) &&
            (channelId != 0x35) &&
            (channelId != 0x36) &&
            (channelId != 0x37) &&
            (channelId != 0x38) &&
            (channelId != 0x39) &&
            (channelId != 0x3A) &&
            (channelId != 0x3B) &&
            (channelId != 0x3C) &&
            (channelId != 0x3D) &&
            (channelId != 0x3E) &&
            (channelId != 0x3F) &&
            (channelId != 0x40) &&
            (channelId != 0x41) &&
            (channelId != 0x42) &&
            (channelId != 0x43) &&
            (channelId != 0x44) &&
            (channelId != 0x45) &&
            (channelId != 0x46) &&
            (channelId != 0x47) &&
            (channelId != 0x48) &&
            (channelId != 0x49) &&
            (channelId != 0x4A) &&
            (channelId != 0x4B) &&
            (channelId != 0x4C) &&
            (channelId != 0x4D) &&
            (channelId != 0x4E) &&
            (channelId != 0x4F) &&
            (channelId != 0x50) &&
            (channelId != 0x51) &&
            (channelId != 0x52) &&
            (channelId != 0x53) &&
            (channelId != 0x54) &&
            (channelId != 0x55) &&
            (channelId != 0x56) &&
            (channelId != 0x57) &&
            (channelId != 0x58) &&
            (channelId != 0x59) &&
            (channelId != 0x5A) &&
            (channelId != 0x5B) &&
            (channelId != 0x5C) &&
            (channelId != 0x5D) &&
            (channelId != 0x5E) &&
            (channelId != 0x5F) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B80 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xE0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 5));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B81 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 3));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink0_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_TxSampleSelLink0_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink0_BfSet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink0_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1900 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink0_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink0_BfGet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink0_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1900 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink1_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_JesdCommon_TxSampleSelLink1_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink1_BfSet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink1_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1D00 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_JesdCommon_TxSampleSelLink1_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfJesdCommonChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink1_BfGet Address");
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
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_JesdCommon_TxSampleSelLink1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1D00 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}
