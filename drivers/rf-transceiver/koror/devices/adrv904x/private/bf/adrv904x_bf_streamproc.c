/** 
 * \file adrv904x_bf_streamproc.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/adrv904x_bf_streamproc.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_STREAMPROC

ADI_API adi_adrv904x_ErrAction_e adrv904x_Streamproc_DbgRdbkMode_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfStreamprocChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Streamproc_DbgRdbkMode_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_MAIN_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Streamproc_DbgRdbkMode_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x5U),
                                                  ((uint32_t) bfValue << 1),
                                                  0x2U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Streamproc_ErroredStreamNumber_BfSet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfStreamprocChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Streamproc_ErroredStreamNumber_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_MAIN_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Streamproc_ErroredStreamNumber_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x14U),
                                                  (uint32_t) bfValue,
                                                  0xFFU);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Streamproc_ErroredStreamNumber_BfGet(adi_adrv904x_Device_t* const device,
                                                                               adi_adrv904x_SpiCache_t* const spiCache,
                                                                               const adrv904x_BfStreamprocChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_MAIN_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Streamproc_ErroredStreamNumber_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x14U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_Streamproc_RdbkErrorVal_BfGet(adi_adrv904x_Device_t* const device,
                                                                        adi_adrv904x_SpiCache_t* const spiCache,
                                                                        const adrv904x_BfStreamprocChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_MAIN_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Streamproc_RdbkErrorVal_BfGet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 spiCache,
                                                 (uint32_t) (baseAddr + 0x24U),
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
                                                 (uint32_t) (baseAddr + 0x25U),
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_Streamproc_StreamError_BfSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfStreamprocChanAddr_e baseAddr,
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
                         "Invalid BitField Value in adrv904x_Streamproc_StreamError_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_MAIN_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Streamproc_StreamError_BfSet Address");
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

ADI_API adi_adrv904x_ErrAction_e adrv904x_Streamproc_StreamError_BfGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_SpiCache_t* const spiCache,
                                                                       const adrv904x_BfStreamprocChanAddr_e baseAddr,
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
    if ((baseAddr != ADRV904X_BF_DIGITAL_CORE_MAIN_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC) &&
            (baseAddr != ADRV904X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_Streamproc_StreamError_BfGet Address");
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

