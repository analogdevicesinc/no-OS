/** 
 * \file adrv904x_bf_tx_dfe_dig_regs.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_tx_dfe_dig_regs.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_TX_DFE_DIG_REGS

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PostCfrDigitalGain_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxDfeDigRegs_PostCfrDigitalGain_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PostCfrDigitalGain_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1CU),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PostCfrDigitalGain_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PostCfrDigitalGain_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1CU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PreDpdHcClipCntStat_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PreDpdHcClipCntStat_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x20U),
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
                                                 (uint32_t) (baseAddr + 0x21U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PreDpdHcCntClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxDfeDigRegs_PreDpdHcCntClear_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PreDpdHcCntClear_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PreDpdHcCntUpd_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxDfeDigRegs_PreDpdHcCntUpd_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PreDpdHcCntUpd_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PreDpdHcEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxDfeDigRegs_PreDpdHcEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PreDpdHcEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PreDpdHcEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PreDpdHcEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PreDpdHcThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxDfeDigRegs_PreDpdHcThreshold_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PreDpdHcThreshold_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x18),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x19),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1A),
                                                  bfValue >> 16,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_PreDpdHcThreshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_PreDpdHcThreshold_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x18U),
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
                                                 (uint32_t) (baseAddr + 0x19U),
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
                                                 (uint32_t) (baseAddr + 0x1AU),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint32_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 16));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_TxVswrDataGenAhbAccessEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                                          const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxDfeDigRegs_TxVswrDataGenAhbAccessEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_TxVswrDataGenAhbAccessEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x111U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_TxVswrDataGenEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_TxVswrDataGenEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x110U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x111),
                                                  bfValue << 4,
                                                  0xF0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x112),
                                                  bfValue >> 4,
                                                  0x3F);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxDfeDigRegsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_DFE_TX_DFE_DIG_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_DFE_TX_DFE_DIG_REGS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x111U),
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
                                                 (uint32_t) (baseAddr + 0x112U),
                                                 &bfValueTmp,
                                                 0x3FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 4));

    return recoveryAction;
}

