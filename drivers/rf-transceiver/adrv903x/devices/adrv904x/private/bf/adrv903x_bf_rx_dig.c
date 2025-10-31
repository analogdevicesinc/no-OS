/** 
 * \file adrv903x_bf_rx_dig.c Automatically generated file with generator ver 0.0.13.0.
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

#include "../../private/bf/adrv903x_bf_rx_dig.h"
#include "adi_common_error.h"
#include "adi_adrv903x_error.h"

#include "adrv903x_bf_error_types.h"

#define ADI_FILE ADI_ADRV903X_FILE_PRIVATE_BF_RX_DIG

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestClkSel_BfSet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AdcTestClkSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestClkSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x109U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestClkSel_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestClkSel_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x109U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenEnSel_BfSet(adi_adrv903x_Device_t* const device,
                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                      const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AdcTestGenEnSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenEnSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10CU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenEnSel_BfGet(adi_adrv903x_Device_t* const device,
                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                      const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenEnSel_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10CU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenEnSpi_BfSet(adi_adrv903x_Device_t* const device,
                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                      const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AdcTestGenEnSpi_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenEnSpi_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10DU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenEnSpi_BfGet(adi_adrv903x_Device_t* const device,
                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                      const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenEnSpi_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10DU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenSel_BfSet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AdcTestGenSel_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenSel_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10AU),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenSel_BfGet(adi_adrv903x_Device_t* const device,
                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                    const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenSel_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10AU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenSineFreq_BfSet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AdcTestGenSineFreq_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenSineFreq_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10DU),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AdcTestGenSineFreq_BfGet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AdcTestGenSineFreq_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x10DU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AgcDecGainGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AgcDecGainGpioSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AgcDecGainGpioSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x105U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AgcGainChangeGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                              adi_adrv903x_SpiCache_t* const spiCache,
                                                                              const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AgcGainChangeGpioSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AgcGainChangeGpioSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x104U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AgcIncGainGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AgcIncGainGpioSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AgcIncGainGpioSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x106U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AgcManualGainLockGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                                  adi_adrv903x_SpiCache_t* const spiCache,
                                                                                  const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AgcManualGainLockGpioSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AgcManualGainLockGpioSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x100U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_AgcSlowloopFreezeEnableGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                                        adi_adrv903x_SpiCache_t* const spiCache,
                                                                                        const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_AgcSlowloopFreezeEnableGpioSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_AgcSlowloopFreezeEnableGpioSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x101U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_Ddc1Hb1OutClkDivideRatio_BfSet(adi_adrv903x_Device_t* const device,
                                                                               adi_adrv903x_SpiCache_t* const spiCache,
                                                                               const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_Ddc1Hb1OutClkDivideRatio_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_Ddc1Hb1OutClkDivideRatio_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x12U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_DecpwrClkDivideRatio_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_DecpwrClkDivideRatio_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_DecpwrClkDivideRatio_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xFU),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_DecpwrClkEnable_BfSet(adi_adrv903x_Device_t* const device,
                                                                      adi_adrv903x_SpiCache_t* const spiCache,
                                                                      const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_DecpwrClkEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_DecpwrClkEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x15U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_Hb2OutClkDivideRatio_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_Hb2OutClkDivideRatio_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x14U),
                                                 &bfValueTmp,
                                                 0x30U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_PeakDetectClkDivideRatio_BfSet(adi_adrv903x_Device_t* const device,
                                                                               adi_adrv903x_SpiCache_t* const spiCache,
                                                                               const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_PeakDetectClkDivideRatio_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_PeakDetectClkDivideRatio_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1CU),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_PeakDetectClkEnable_BfSet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_PeakDetectClkEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_PeakDetectClkEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x8U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_ReferenceClockCycles_BfGet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_ReferenceClockCycles_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x1EU),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_RoutClkDivideRatio_BfGet(adi_adrv903x_Device_t* const device,
                                                                         adi_adrv903x_SpiCache_t* const spiCache,
                                                                         const adrv903x_BfRxDigChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_RoutClkDivideRatio_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x4U),
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

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_RssiEnableGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                           adi_adrv903x_SpiCache_t* const spiCache,
                                                                           const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_RssiEnableGpioSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_RssiEnableGpioSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x102U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_RxGpioSourceSelection_BfSet(adi_adrv903x_Device_t* const device,
                                                                            adi_adrv903x_SpiCache_t* const spiCache,
                                                                            const adrv903x_BfRxDigChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
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
                         "Invalid BitField Value in adrv903x_RxDig_RxGpioSourceSelection_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_RxGpioSourceSelection_BfSet Address");
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
            (channelId != 0x17) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         channelId, 
                         "Invalid adrv903x_RxDig_RxGpioSourceSelection_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV903X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0xA0 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_StreamprocDdc1Hb1OutClkEnable_BfSet(adi_adrv903x_Device_t* const device,
                                                                                    adi_adrv903x_SpiCache_t* const spiCache,
                                                                                    const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_StreamprocDdc1Hb1OutClkEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_StreamprocDdc1Hb1OutClkEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x94U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_RxDig_TestGenEnGpioSelect_BfSet(adi_adrv903x_Device_t* const device,
                                                                          adi_adrv903x_SpiCache_t* const spiCache,
                                                                          const adrv903x_BfRxDigChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv903x_RxDig_TestGenEnGpioSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_VALUE_CHECK */

#if ADRV903X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV903X_BF_SLICE_RX_0__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_1__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_2__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_3__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_4__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_5__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_6__RX_DIG) &&
            (baseAddr != ADRV903X_BF_SLICE_RX_7__RX_DIG))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV903X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv903x_RxDig_TestGenEnGpioSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV903X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv903x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x10BU),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

