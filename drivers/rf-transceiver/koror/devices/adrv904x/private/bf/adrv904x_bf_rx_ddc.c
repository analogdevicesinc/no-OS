/** 
 * \file adrv904x_bf_rx_ddc.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_rx_ddc.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_RX_DDC

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_DecPowerEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x81U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x81U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x81U),
                                                  ((uint32_t) bfValue << 2),
                                                  0x4U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x81U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerMeasurementDuration_BfSet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_DecPowerMeasurementDuration_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerMeasurementDuration_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x82U),
                                                  (uint32_t) bfValue,
                                                  0x1FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerMeasurementDuration_BfGet(adi_adrv904x_Device_t* const device,
                                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerMeasurementDuration_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x82U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerStartDelayCounter_BfSet(adi_adrv904x_Device_t* const device,
                                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                                const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_DecPowerStartDelayCounter_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerStartDelayCounter_BfSet Address");
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
                                                  0xFF);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerTddModeEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_DecPowerTddModeEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerTddModeEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x86U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerValue_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerValue_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x80U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DecPowerValueReadback_BfSet(adi_adrv904x_Device_t* const device,
                                                                            adi_adrv904x_SpiCache_t* const spiCache,
                                                                            const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_DecPowerValueReadback_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DecPowerValueReadback_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x81U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_DigitalGainEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_DigitalGainEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_DigitalGainEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x20U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_ExternalSlicerPinControlStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_ExternalSlicerPinControlStep_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x33U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpEn_BfSet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_FpEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x35U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpEn_BfGet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x35U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpExponentBits_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     const adrv904x_Bf_RxDdc_FpExponentBits_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_2                      ) &&
            (bfValue != ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_3                      ) &&
            (bfValue != ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_4                      ) &&
            (bfValue != ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_5                      )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_RxDdc_FpExponentBits_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpExponentBits_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x39U),
                                                  (uint32_t) bfValue,
                                                  0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpExponentBits_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                     adrv904x_Bf_RxDdc_FpExponentBits_e* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (adrv904x_Bf_RxDdc_FpExponentBits_e) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpExponentBits_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x39U),
                                                 &bfValueTmp,
                                                 0x3U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (adrv904x_Bf_RxDdc_FpExponentBits_e) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpFloatDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_FpFloatDataFormat_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpFloatDataFormat_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x31U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpFloatDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpFloatDataFormat_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x31U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpHideLeadingOnes_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_FpHideLeadingOnes_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpHideLeadingOnes_BfSet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpHideLeadingOnes_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpHideLeadingOnes_BfGet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpIntDataAtten_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_FpIntDataAtten_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpIntDataAtten_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x36U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpIntDataAtten_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpIntDataAtten_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x36U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpNanEncEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_FpNanEncEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpNanEncEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x3AU),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpNanEncEn_BfGet(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_SpiCache_t* const spiCache,
                                                                 const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpNanEncEn_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x3AU),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpRoundMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                  const adrv904x_Bf_RxDdc_FpRoundMode_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTIESTOEVEN                ) &&
            (bfValue != ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTOWARDSPOSITIVE           ) &&
            (bfValue != ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTOWARDSNEGATIVE           ) &&
            (bfValue != ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTOWARDSZERO               ) &&
            (bfValue != ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTIESTOAWAY                )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_RxDdc_FpRoundMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpRoundMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x33U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_FpRoundMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_SpiCache_t* const spiCache,
                                                                  const adrv904x_BfRxDdcChanAddr_e baseAddr,
                                                                  adrv904x_Bf_RxDdc_FpRoundMode_e* const bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_NULL_CHECK > 0
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* ADRV904X_BITFIELD_NULL_CHECK */

    *bfValue = (adrv904x_Bf_RxDdc_FpRoundMode_e) 0;

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_FpRoundMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x33U),
                                                 &bfValueTmp,
                                                 0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (adrv904x_Bf_RxDdc_FpRoundMode_e) (bfValueTmp >> 4);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompEnable_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_GainCompEnable_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_GainCompEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x21U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompEnable_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_GainCompEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x21U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForExtGain_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_GainCompForExtGain_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_GainCompForExtGain_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x22U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForExtGain_BfGet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_GainCompForExtGain_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x22U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForTempGain_BfSet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_GainCompForTempGain_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_GainCompForTempGain_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x27U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_GainCompForTempGain_BfGet(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_SpiCache_t* const spiCache,
                                                                          const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_GainCompForTempGain_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x27U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataFormat_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntDataFormat_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntDataFormat_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x40U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataFormat_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntDataFormat_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x40U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataResolution_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntDataResolution_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntDataResolution_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x40U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntDataResolution_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntDataResolution_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x40U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicer_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntEmbedSlicer_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEmbedSlicer_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x39U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicer_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEmbedSlicer_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x39U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerNumber_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntEmbedSlicerNumber_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEmbedSlicerNumber_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x32U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerNumber_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEmbedSlicerNumber_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x32U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerPos_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntEmbedSlicerPos_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEmbedSlicerPos_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x32U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEmbedSlicerPos_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEmbedSlicerPos_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x32U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEvenParity_BfSet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntEvenParity_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEvenParity_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x37U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x10U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntEvenParity_BfGet(adi_adrv904x_Device_t* const device,
                                                                    adi_adrv904x_SpiCache_t* const spiCache,
                                                                    const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntEvenParity_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x37U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntParitySupport_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntParitySupport_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntParitySupport_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x41U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntParitySupport_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntParitySupport_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x41U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntSlicerLsbOnQ_BfSet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_IntSlicerLsbOnQ_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntSlicerLsbOnQ_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x35U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_IntSlicerLsbOnQ_BfGet(adi_adrv904x_Device_t* const device,
                                                                      adi_adrv904x_SpiCache_t* const spiCache,
                                                                      const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_IntSlicerLsbOnQ_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x35U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_MaxSlicer_BfSet(adi_adrv904x_Device_t* const device,
                                                                adi_adrv904x_SpiCache_t* const spiCache,
                                                                const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_MaxSlicer_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_MaxSlicer_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x2AU),
                                                  ((uint32_t) bfValue << 4),
                                                  0xF0U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_MaxSlicerOverride_BfSet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_MaxSlicerOverride_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_MaxSlicerOverride_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x27U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatI_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_RxMonFormatI_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_RxMonFormatI_BfSet Address");
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
                         "Invalid adrv904x_RxDdc_RxMonFormatI_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x44 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatI_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_RxMonFormatI_BfGet Address");
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
                         "Invalid adrv904x_RxDdc_RxMonFormatI_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x44 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatQ_BfSet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_RxMonFormatQ_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_RxMonFormatQ_BfSet Address");
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
                         "Invalid adrv904x_RxDdc_RxMonFormatQ_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x46 + channelId * 4U),
                                                  (uint32_t) bfValue,
                                                  0xFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxMonFormatQ_BfGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_SpiCache_t* const spiCache,
                                                                   const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_RxMonFormatQ_BfGet Address");
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
                         "Invalid adrv904x_RxDdc_RxMonFormatQ_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* ADRV904X_CHANNEL_ID_CHECK */

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x46 + channelId * 4U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxTempGainComp_BfSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_RxTempGainComp_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_RxTempGainComp_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x24U),
                                                  (uint32_t) bfValue,
                                                  0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_RxTempGainComp_BfGet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_SpiCache_t* const spiCache,
                                                                     const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_RxTempGainComp_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x24U),
                                                 &bfValueTmp,
                                                 0x7FU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Read Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_SlicerPinControlMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_SlicerPinControlMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x42U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlMode_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_SlicerPinControlMode_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x42U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlStep_BfSet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_SlicerPinControlStep_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_SlicerPinControlStep_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x38U),
                                                  (uint32_t) bfValue,
                                                  0x7U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_SlicerPinControlStep_BfGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_SpiCache_t* const spiCache,
                                                                           const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_SlicerPinControlStep_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x38U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdc_Static3bitSlicerModeEn_BfSet(adi_adrv904x_Device_t* const device,
                                                                             adi_adrv904x_SpiCache_t* const spiCache,
                                                                             const adrv904x_BfRxDdcChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_RxDdc_Static3bitSlicerModeEn_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__RX_DDC_1_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_0_) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__RX_DDC_1_))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_RxDdc_Static3bitSlicerModeEn_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x31U),
                                                  (uint32_t) bfValue,
                                                  0x1U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

