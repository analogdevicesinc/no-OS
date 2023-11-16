/** 
 * \file adrv904x_bf_sequencer.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_sequencer.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_SEQUENCER

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ApplyNow_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_ApplyNow_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ApplyNow_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ApplyOnMultiframeBoundary_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_ApplyOnMultiframeBoundary_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ApplyOnMultiframeBoundary_BfSet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_DbgState_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfSequencerChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_DbgState_BfGet Address");
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
            (channelId != 0xF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_DbgState_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xB2 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_ErrorClear_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ErrorClear_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x7U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExtendedSymbolSpacing_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                                const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4095U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_Sequencer_ExtendedSymbolSpacing_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ExtendedSymbolSpacing_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15),
                                                  bfValue >> 8,
                                                  0xF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExtendedSymbolWidth_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_ExtendedSymbolWidth_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ExtendedSymbolWidth_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x11),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExternalJumpEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_ExternalJumpEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ExternalJumpEnable_BfSet Address");
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
            (channelId != 0xF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_ExternalJumpEnable_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xB3 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExternalJumpPointer_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint16_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4095U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_Sequencer_ExternalJumpPointer_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ExternalJumpPointer_BfSet Address");
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
            (channelId != 0xF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_ExternalJumpPointer_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xB0 + channelId * 4),
                                                  bfValue << 4,
                                                  0xF0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xB1 + channelId * 4),
                                                  bfValue >> 4,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ExternalJumpPointer_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ExternalJumpPointer_BfGet Address");
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
            (channelId != 0xF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_ExternalJumpPointer_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xB0 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) >> 4));

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0xB1 + channelId * 4U),
                                                 &bfValueTmp,
                                                 0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 4));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_GeneralPurposeReg_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_GeneralPurposeReg_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_GeneralPurposeReg_BfSet Address");
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
            (channelId != 0xBF) &&
            (channelId != 0xC0) &&
            (channelId != 0xC1) &&
            (channelId != 0xC2) &&
            (channelId != 0xC3) &&
            (channelId != 0xC4) &&
            (channelId != 0xC5) &&
            (channelId != 0xC6) &&
            (channelId != 0xC7) &&
            (channelId != 0xC8) &&
            (channelId != 0xC9) &&
            (channelId != 0xCA) &&
            (channelId != 0xCB) &&
            (channelId != 0xCC) &&
            (channelId != 0xCD) &&
            (channelId != 0xCE) &&
            (channelId != 0xCF) &&
            (channelId != 0xD0) &&
            (channelId != 0xD1) &&
            (channelId != 0xD2) &&
            (channelId != 0xD3) &&
            (channelId != 0xD4) &&
            (channelId != 0xD5) &&
            (channelId != 0xD6) &&
            (channelId != 0xD7) &&
            (channelId != 0xD8) &&
            (channelId != 0xD9) &&
            (channelId != 0xDA) &&
            (channelId != 0xDB) &&
            (channelId != 0xDC) &&
            (channelId != 0xDD) &&
            (channelId != 0xDE) &&
            (channelId != 0xDF) &&
            (channelId != 0xE0) &&
            (channelId != 0xE1) &&
            (channelId != 0xE2) &&
            (channelId != 0xE3) &&
            (channelId != 0xE4) &&
            (channelId != 0xE5) &&
            (channelId != 0xE6) &&
            (channelId != 0xE7) &&
            (channelId != 0xE8) &&
            (channelId != 0xE9) &&
            (channelId != 0xEA) &&
            (channelId != 0xEB) &&
            (channelId != 0xEC) &&
            (channelId != 0xED) &&
            (channelId != 0xEE) &&
            (channelId != 0xEF) &&
            (channelId != 0xF0) &&
            (channelId != 0xF1) &&
            (channelId != 0xF2) &&
            (channelId != 0xF3) &&
            (channelId != 0xF4) &&
            (channelId != 0xF5) &&
            (channelId != 0xF6) &&
            (channelId != 0xF7) &&
            (channelId != 0xF8) &&
            (channelId != 0xF9) &&
            (channelId != 0xFA) &&
            (channelId != 0xFB) &&
            (channelId != 0xFC) &&
            (channelId != 0xFD) &&
            (channelId != 0xFE) &&
            (channelId != 0xFF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_GeneralPurposeReg_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x100 + channelId * 4),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x101 + channelId * 4),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x102 + channelId * 4),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_GlobalReset_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_GlobalReset_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_GlobalReset_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xFU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_MultiframePhaseAdjust_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                                const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 33554431U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_Sequencer_MultiframePhaseAdjust_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_MultiframePhaseAdjust_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x4),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x6),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x7),
                                                  bfValue >> 24,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_PointerInit_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 268435455U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_Sequencer_PointerInit_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_PointerInit_BfSet Address");
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
            (channelId != 0xF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_PointerInit_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x70 + channelId * 4),
                                                  bfValue << 4,
                                                  0xF0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x71 + channelId * 4),
                                                  bfValue >> 4,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x72 + channelId * 4),
                                                  bfValue >> 12,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x73 + channelId * 4),
                                                  bfValue >> 20,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_ShortSymbolWidth_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_ShortSymbolWidth_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_ShortSymbolWidth_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x12),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x13),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncGpioSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_SsbSyncGpioSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_SsbSyncGpioSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncPeriod_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfSequencerChanAddr_e baseAddr,
                                                                        const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 33554431U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_Sequencer_SsbSyncPeriod_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_SsbSyncPeriod_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xC),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xD),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xE),
                                                  bfValue >> 16,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xF),
                                                  bfValue >> 24,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncPhaseError_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfSequencerChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_SsbSyncPhaseError_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_SsbSyncPhaseErrorClear_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncTrackingMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_SsbSyncTrackingMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_SsbSyncTrackingMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_SsbSyncTrackingMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfSequencerChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_SsbSyncTrackingMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_XbarCtrlSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_XbarCtrlSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_XbarCtrlSel_BfSet Address");
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
            (channelId != 0xBF) &&
            (channelId != 0xC0) &&
            (channelId != 0xC1) &&
            (channelId != 0xC2) &&
            (channelId != 0xC3) &&
            (channelId != 0xC4) &&
            (channelId != 0xC5) &&
            (channelId != 0xC6) &&
            (channelId != 0xC7) &&
            (channelId != 0xC8) &&
            (channelId != 0xC9) &&
            (channelId != 0xCA) &&
            (channelId != 0xCB) &&
            (channelId != 0xCC) &&
            (channelId != 0xCD) &&
            (channelId != 0xCE) &&
            (channelId != 0xCF) &&
            (channelId != 0xD0) &&
            (channelId != 0xD1) &&
            (channelId != 0xD2) &&
            (channelId != 0xD3) &&
            (channelId != 0xD4) &&
            (channelId != 0xD5) &&
            (channelId != 0xD6) &&
            (channelId != 0xD7) &&
            (channelId != 0xD8) &&
            (channelId != 0xD9) &&
            (channelId != 0xDA) &&
            (channelId != 0xDB) &&
            (channelId != 0xDC) &&
            (channelId != 0xDD) &&
            (channelId != 0xDE) &&
            (channelId != 0xDF) &&
            (channelId != 0xE0) &&
            (channelId != 0xE1) &&
            (channelId != 0xE2) &&
            (channelId != 0xE3) &&
            (channelId != 0xE4) &&
            (channelId != 0xE5) &&
            (channelId != 0xE6) &&
            (channelId != 0xE7) &&
            (channelId != 0xE8) &&
            (channelId != 0xE9) &&
            (channelId != 0xEA) &&
            (channelId != 0xEB) &&
            (channelId != 0xEC) &&
            (channelId != 0xED) &&
            (channelId != 0xEE) &&
            (channelId != 0xEF) &&
            (channelId != 0xF0) &&
            (channelId != 0xF1) &&
            (channelId != 0xF2) &&
            (channelId != 0xF3) &&
            (channelId != 0xF4) &&
            (channelId != 0xF5) &&
            (channelId != 0xF6) &&
            (channelId != 0xF7) &&
            (channelId != 0xF8) &&
            (channelId != 0xF9) &&
            (channelId != 0xFA) &&
            (channelId != 0xFB) &&
            (channelId != 0xFC) &&
            (channelId != 0xFD) &&
            (channelId != 0xFE) &&
            (channelId != 0xFF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_XbarCtrlSel_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x500 + channelId * 4U),
                                                  ((uint32_t) bfValue << 4),
                                                  0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Sequencer_XbarSequencerSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfSequencerChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Sequencer_XbarSequencerSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Sequencer_XbarSequencerSel_BfSet Address");
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
            (channelId != 0xBF) &&
            (channelId != 0xC0) &&
            (channelId != 0xC1) &&
            (channelId != 0xC2) &&
            (channelId != 0xC3) &&
            (channelId != 0xC4) &&
            (channelId != 0xC5) &&
            (channelId != 0xC6) &&
            (channelId != 0xC7) &&
            (channelId != 0xC8) &&
            (channelId != 0xC9) &&
            (channelId != 0xCA) &&
            (channelId != 0xCB) &&
            (channelId != 0xCC) &&
            (channelId != 0xCD) &&
            (channelId != 0xCE) &&
            (channelId != 0xCF) &&
            (channelId != 0xD0) &&
            (channelId != 0xD1) &&
            (channelId != 0xD2) &&
            (channelId != 0xD3) &&
            (channelId != 0xD4) &&
            (channelId != 0xD5) &&
            (channelId != 0xD6) &&
            (channelId != 0xD7) &&
            (channelId != 0xD8) &&
            (channelId != 0xD9) &&
            (channelId != 0xDA) &&
            (channelId != 0xDB) &&
            (channelId != 0xDC) &&
            (channelId != 0xDD) &&
            (channelId != 0xDE) &&
            (channelId != 0xDF) &&
            (channelId != 0xE0) &&
            (channelId != 0xE1) &&
            (channelId != 0xE2) &&
            (channelId != 0xE3) &&
            (channelId != 0xE4) &&
            (channelId != 0xE5) &&
            (channelId != 0xE6) &&
            (channelId != 0xE7) &&
            (channelId != 0xE8) &&
            (channelId != 0xE9) &&
            (channelId != 0xEA) &&
            (channelId != 0xEB) &&
            (channelId != 0xEC) &&
            (channelId != 0xED) &&
            (channelId != 0xEE) &&
            (channelId != 0xEF) &&
            (channelId != 0xF0) &&
            (channelId != 0xF1) &&
            (channelId != 0xF2) &&
            (channelId != 0xF3) &&
            (channelId != 0xF4) &&
            (channelId != 0xF5) &&
            (channelId != 0xF6) &&
            (channelId != 0xF7) &&
            (channelId != 0xF8) &&
            (channelId != 0xF9) &&
            (channelId != 0xFA) &&
            (channelId != 0xFB) &&
            (channelId != 0xFC) &&
            (channelId != 0xFD) &&
            (channelId != 0xFE) &&
            (channelId != 0xFF) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_Sequencer_XbarSequencerSel_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x500 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

