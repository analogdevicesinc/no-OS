/** 
 * \file adrv904x_bf_tx_funcs.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_tx_funcs.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_TX_FUNCS

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_AnaRampHoldSampleEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_AnaRampHoldSampleEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_AnaRampHoldSampleEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_AnaRampHoldSampleEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_AnaRampHoldSampleEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_JesdDfrmMask_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_JesdDfrmMask_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_JesdDfrmMask_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_JesdDfrmMask_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_JesdDfrmMask_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAprEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAprEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAprEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x34U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAprEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAprEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAverageDur_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAverageDur_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x31U),
                                                  (uint32_t) bfValue,
                                                  0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageErrorPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAverageErrorPower_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4CU),
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
                                                 (uint32_t) (baseAddr + 0x4DU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAveragePeakRatio_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAveragePeakRatio_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x44U),
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
                                                 (uint32_t) (baseAddr + 0x45U),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint16_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 8));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAveragePower_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAveragePower_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x40U),
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
                                                 (uint32_t) (baseAddr + 0x41U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAverageThreshold_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAverageThreshold_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x32),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x33),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAverageThreshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAverageThreshold_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x32U),
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
                                                 (uint32_t) (baseAddr + 0x33U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfSet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x30U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerError_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerError_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x48U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAvgpowerErrorClear_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerErrorClear_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x48U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfSet(adi_adrv904x_Device_t* const device,
                                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x30U),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfGet(adi_adrv904x_Device_t* const device,
                                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                                               const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfGet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfSet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x30U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x30U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x30U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionGainRampUpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionGainRampUpEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionGainRampUpEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x48U),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionInputSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionInputSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionInputSel_BfSet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionInputSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionInputSel_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x30U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionOverloadCountTh_BfSet(adi_adrv904x_Device_t* const device,
                                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionOverloadCountTh_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionOverloadCountTh_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3BU),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionOverloadThPre_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionOverloadThPre_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionOverloadThPre_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x38),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x39),
                                                  bfValue >> 8,
                                                  0x1);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionOverloadWindowSize_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionOverloadWindowSize_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionOverloadWindowSize_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3AU),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x34U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerError_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerError_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x48U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                                        const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakpowerErrorClear_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerErrorClear_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x48U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfSet(adi_adrv904x_Device_t* const device,
                                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x34U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfGet(adi_adrv904x_Device_t* const device,
                                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x34U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x34U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakCount_BfSet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakCount_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakCount_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x34),
                                                  bfValue << 6,
                                                  0xC0);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x35),
                                                  bfValue >> 2,
                                                  0xF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakCount_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakCount_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x34U),
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
                                                 (uint32_t) (baseAddr + 0x35U),
                                                 &bfValueTmp,
                                                 0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) ((uint64_t)(*bfValue) | (((uint64_t)bfValueTmp) << 2));

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakDur_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakDur_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakDur_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x35U),
                                                  ((uint32_t) bfValue << 4),
                                                  0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakDur_BfGet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakDur_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x35U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakErrorPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakErrorPower_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4EU),
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
                                                 (uint32_t) (baseAddr + 0x4FU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakPower_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x42U),
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
                                                 (uint32_t) (baseAddr + 0x43U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakThreshold_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionPeakThreshold_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakThreshold_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x36),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x37),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionPeakThreshold_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionPeakThreshold_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x36U),
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
                                                 (uint32_t) (baseAddr + 0x37U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionRampMaxAttenuation_BfSet(adi_adrv904x_Device_t* const device,
                                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 127U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionRampMaxAttenuation_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionRampMaxAttenuation_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3CU),
                                                  (uint32_t) bfValue,
                                                  0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionRampStepDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                                     const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionRampStepDuration_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionRampStepDuration_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3DU),
                                                  ((uint32_t) bfValue << 4),
                                                  0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionRampStepSize_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionRampStepSize_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionRampStepSize_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3DU),
                                                  (uint32_t) bfValue,
                                                  0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PaProtectionReadbackUpdate_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PaProtectionReadbackUpdate_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PaProtectionReadbackUpdate_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x45U),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtClr_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PllJesdProtClr_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PllJesdProtClr_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2U),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtClrReqd_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PllJesdProtClrReqd_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PllJesdProtClrReqd_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2U),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtClrReqd_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PllJesdProtClrReqd_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllJesdProtEvent_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PllJesdProtEvent_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x2U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllUnlockMask_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_PllUnlockMask_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PllUnlockMask_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1U),
                                                  (uint32_t) bfValue,
                                                  0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_PllUnlockMask_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_PllUnlockMask_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdArvEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdArvEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x64U),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdArvEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x64U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvTxonQual_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdArvTxonQual_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdArvTxonQual_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x65U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvTxonQual_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdArvTxonQual_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x65U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvWait_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdArvWait_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdArvWait_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x65U),
                                                  ((uint32_t) bfValue << 4),
                                                  0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdArvWait_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdArvWait_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x65U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdDinSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdDinSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdDinSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x64U),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdDinSel_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdDinSel_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x64U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdDisAnalogDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdDisAnalogDelay_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdDisAnalogDelay_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdEn_BfSet(adi_adrv904x_Device_t* const device,
                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x64U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdEn_BfGet(adi_adrv904x_Device_t* const device,
                                                              adi_adrv904x_SpiCache_t* const spiCache,
                                                              const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x64U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdError_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdError_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x65U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdErrorClear_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdErrorClear_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdErrorClear_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x65U),
                                                  ((uint32_t) bfValue << 3),
                                                  0x8U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdIrqEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdIrqEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdIrqEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x64U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdIrqEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdIrqEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x64U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdRdnEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdRdnEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdRdnEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x64U),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdRdnEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdRdnEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x64U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdSlewOffset_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdSlewOffset_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdSlewOffset_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x66),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x67),
                                                  bfValue >> 8,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdSlewOffset_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdSlewOffset_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x66U),
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
                                                 (uint32_t) (baseAddr + 0x67U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStat_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdStat_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x6CU),
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
                                                 (uint32_t) (baseAddr + 0x6DU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdStatEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdStatEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x64U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdStatEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x64U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_SrdStatMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdStatMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x64U),
                                                  ((uint32_t) bfValue << 2),
                                                  0xCU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_SrdStatMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_SrdStatMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x64U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenuation_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxAttenuation_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenuation_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8),
                                                  bfValue,
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x9),
                                                  bfValue >> 8,
                                                  0x3);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenuation_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenuation_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x8U),
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
                                                 (uint32_t) (baseAddr + 0x9U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenConfig_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxAttenConfig_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenConfig_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x30U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenConfig_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenConfig_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x0U),
                                                 &bfValueTmp,
                                                 0x30U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxAttenMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x0U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenPapDisAnalogDelay_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxAttenPapDisAnalogDelay_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenPapDisAnalogDelay_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1U),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x6U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenUpdGpioEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenUpdGpioEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x6U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3U),
                                                  ((uint32_t) bfValue << 5),
                                                  0x20U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPeakToPowerMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxPeakToPowerMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPeakToPowerMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x70U),
                                                  ((uint32_t) bfValue << 6),
                                                  0x40U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPeakToPowerMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPeakToPowerMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x70U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPower_BfGet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPower_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x74U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerInputSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxPowerInputSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerInputSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x70U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x1EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerInputSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerInputSelect_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x70U),
                                                 &bfValueTmp,
                                                 0x1EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerLargestPeak_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerLargestPeak_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x78U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxPowerMeasurementDuration_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerMeasurementDuration_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x73U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x3EU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementDuration_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerMeasurementDuration_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x73U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxPowerMeasurementEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerMeasurementEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x70U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerMeasurementEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x70U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_TxPowerMeasurementReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_TxPowerMeasurementReadback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_TxPowerMeasurementReadback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x75U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfGet(adi_adrv904x_Device_t* const device,
                                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                                 const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_TxFuncs_UseSliceAttenValue_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfTxFuncsChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_TxFuncs_UseSliceAttenValue_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_TxFuncs_UseSliceAttenValue_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3U),
                                                  ((uint32_t) bfValue << 7),
                                                  0x80U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

