/** 
 * \file adrv904x_bf_tx_cfr_reg.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_tx_cfr_reg.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_TX_CFR_REG

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlByp_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr0CtrlByp_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlByp_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlByp_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x53 + channelId * 16U),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlByp_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlByp_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlByp_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x53 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr0CtrlEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlEn_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x53 + channelId * 16U),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlEn_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x53 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x52 + channelId * 16),
                                                  bfValue << 6,
                                                  0xC0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x53 + channelId * 16),
                                                  bfValue >> 2,
                                                  0x3F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0CtrlPkDur_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x52 + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x53 + channelId * 16U),
                                                 &bfValueTmp,
                                                 0x3FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 2));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0PulHpl_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr0PulHpl_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0PulHpl_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x48),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x49),
                                                  bfValue >> 8,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0PulHpl_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0PulHpl_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x48U),
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
                                                 (uint32_t) (baseAddr + 0x49U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0PulInterp_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0PulInterp_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4AU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0ThrCor_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 524287U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr0ThrCor_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0ThrCor_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0ThrCor_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5C + channelId * 16),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5D + channelId * 16),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5E + channelId * 16),
                                                  bfValue >> 16,
                                                  0x7);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0ThrSq_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 262143U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr0ThrSq_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0ThrSq_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0ThrSq_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x54 + channelId * 16),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x55 + channelId * 16),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x56 + channelId * 16),
                                                  bfValue >> 16,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr0ThrSqrt_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 262143U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr0ThrSqrt_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr0ThrSqrt_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr0ThrSqrt_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x58 + channelId * 16),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x59 + channelId * 16),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5A + channelId * 16),
                                                  bfValue >> 16,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlByp_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1CtrlByp_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlByp_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlByp_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8F + channelId * 16U),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlByp_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlByp_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlByp_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x8F + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1CtrlEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlEn_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8F + channelId * 16U),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlEn_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x8F + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8E + channelId * 16),
                                                  bfValue << 6,
                                                  0xC0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8F + channelId * 16),
                                                  bfValue >> 2,
                                                  0x3F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1CtrlPkDur_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x8E + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x8F + channelId * 16U),
                                                 &bfValueTmp,
                                                 0x3FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 2));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1PulHpl_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1PulHpl_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1PulHpl_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x84),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x85),
                                                  bfValue >> 8,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1PulHpl_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1PulHpl_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x84U),
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
                                                 (uint32_t) (baseAddr + 0x85U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1PulInterp_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1PulInterp_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x86U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1Spare_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1Spare_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1Spare_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xBC),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xBD),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1ThrCor_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 524287U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1ThrCor_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrCor_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrCor_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x98 + channelId * 16),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x99 + channelId * 16),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x9A + channelId * 16),
                                                  bfValue >> 16,
                                                  0x7);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1ThrCor_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrCor_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrCor_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x98 + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x99 + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x9A + channelId * 16U),
                                                 &bfValueTmp,
                                                 0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1ThrSq_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 262143U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1ThrSq_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrSq_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrSq_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x90 + channelId * 16),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x91 + channelId * 16),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x92 + channelId * 16),
                                                  bfValue >> 16,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_Cfr1ThrSqrt_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     const uint32_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 262143U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_TxCfrReg_Cfr1ThrSqrt_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrSqrt_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_Cfr1ThrSqrt_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x94 + channelId * 16),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x95 + channelId * 16),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x96 + channelId * 16),
                                                  bfValue >> 16,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlActiveSet_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrCtrlActiveSet_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlChSet_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_CfrCtrlChSet_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrCtrlChSet_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3U),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlPulSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_CfrCtrlPulSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrCtrlPulSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrCtrlPulSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrCtrlPulSel_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkClrStat_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_CfrDbgPkClrStat_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkClrStat_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkClrStat_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B + channelId * 16U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkFoundCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkFoundCurWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkFoundCurWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1C + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x1D + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkFoundPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkFoundPrvWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkFoundPrvWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1E + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x1F + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostCpcCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostCpcCurWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostCpcCurWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x24 + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x25 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostCpcPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostCpcPrvWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostCpcPrvWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x26 + channelId * 16U),
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
                                                 (uint32_t) (baseAddr + 0x27 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostPfCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostPfCurWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostPfCurWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x20 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostPfPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostPfPrvWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostPfPrvWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x22 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostWfCurWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostWfCurWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostWfCurWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x21 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkLostWfPrvWin_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostWfPrvWin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkLostWfPrvWin_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x23 + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkReadStat_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_CfrDbgPkReadStat_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkReadStat_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkReadStat_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B + channelId * 16U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B + channelId * 16U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowEn_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B + channelId * 16U),
                                                  ((uint32_t) bfValue << 4),
                                                  0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

#if ADRV904X_CHANNELID_CHECK > 0
    if (
            (channelId != 0x0) &&
            (channelId != 0x1) &&
            (channelId != 0x2) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv904x_TxCfrReg_CfrDbgPkStatWindowSz_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1B + channelId * 16U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCfrReg_CfrRamShadowRamSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxCfrRegChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxCfrReg_CfrRamShadowRamSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_CFR_TX_CFR_REG) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_CFR_TX_CFR_REG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxCfrReg_CfrRamShadowRamSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xAU),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

