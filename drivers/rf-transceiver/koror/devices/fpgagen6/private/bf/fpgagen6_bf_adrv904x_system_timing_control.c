/** 
 * \file fpgagen6_bf_adrv904x_system_timing_control.c Automatically generated file with generator ver
 * 0.0.16.0.
 * 
 * \brief Contains BitField functions to support FPGAGEN6 transceiver device.
 * 
 * FPGAGEN6 BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2023 Analog Devices Inc.
 * 
 * Released under the FPGAGEN6 API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#include "../../private/bf/fpgagen6_bf_adrv904x_system_timing_control.h"
#include "adi_fpgagen6_hal.h"
#include "adi_common_error.h"
#include "adi_fpgagen6_error.h"

#include "fpgagen6_bf_error_types.h"

#define ADI_FILE ADI_FPGAGEN6_FILE_PRIVATE_BF_ADRV904X_SYSTEM_TIMING_CONTROL


/**
 * \brief Write a 1 to place the AD9528 (connected to the FMC connector) in reset.  Write a 0 to allow normal operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Ad9528Reset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_Ad9528Reset_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_Ad9528Reset_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x10U),
                                                     (uint32_t) bfValue,
                                                     0x1,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Write a 1 to place the AD9528 (connected to the FMC connector) in reset.  Write a 0 to allow normal operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Ad9528Reset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_Ad9528Reset_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x10U),
                                                    &bfValueTmp,
                                                    0x1,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_BlockId_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x4U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief When written to a 1, the polarity of the external SYSREF signal is inverted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                              const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                              const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x16U),
                                                     ((uint32_t) bfValue << 1),
                                                     0x2,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief When written to a 1, the polarity of the external SYSREF signal is inverted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                              const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                              uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x16U),
                                                    &bfValueTmp,
                                                    0x2,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

/**
 * \brief When written to a 1, a single device clock wide pulse is generated on the external SYSREF request signal.  This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefRequest_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefRequest_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefRequest_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x16U),
                                                     ((uint32_t) bfValue << 2),
                                                     0x4,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief When written to a 1, a single device clock wide pulse is generated on the external SYSREF request signal.  This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefRequest_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefRequest_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x16U),
                                                    &bfValueTmp,
                                                    0x4,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

/**
 * \brief The external SYSREF signal is sampled on the falling edge when this bit is 0, and sampled on the rising edge when this bit is 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                                const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                                const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x16U),
                                                     (uint32_t) bfValue,
                                                     0x1,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief The external SYSREF signal is sampled on the falling edge when this bit is 0, and sampled on the rising edge when this bit is 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                                const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                                uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x16U),
                                                    &bfValueTmp,
                                                    0x1,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief This field contains the frequency of the clock to Receive Link 0 in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink0Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink0Clk_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x20U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x21U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x22U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x23U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief This field contains the frequency of the clock to Receive Link 1 in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink1Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink1Clk_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x24U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x25U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x26U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x27U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief This field contains the frequency of the clock to Receive Link 2 in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink2Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink2Clk_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x28U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x29U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2AU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2BU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief This field contains the frequency of the clock to SPI Master Alternate Base Clock in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqSpiAltBaseClk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                              const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                              uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_FreqSpiAltBaseClk_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x38U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x39U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3AU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3BU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief This field contains the frequency of the clock to TDD State Machine in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqTddClk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                       uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_FreqTddClk_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x34U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x35U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x36U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x37U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief This field contains the frequency of the clock to Transmit Link 0 in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqTxLink0Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_FreqTxLink0Clk_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2CU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2DU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2EU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2FU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief This field contains the frequency of the clock to Transmit Link 1 in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqTxLink1Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_FreqTxLink1Clk_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x30U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x31U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x32U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x33U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_InstanceId_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief When written to 1, the internal SYSREF generator is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                            const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                            const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x14U),
                                                     (uint32_t) bfValue,
                                                     0x1,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief When written to 1, the internal SYSREF generator is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                            const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                            uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x14U),
                                                    &bfValueTmp,
                                                    0x1,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief When written to a 1, the internally generated SYSREF signal will be a free running pulse train based on the gen period bit field.  The internal SYSREF must be enabled and requested before the pulse train begins.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefFreerun_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_IntSysrefFreerun_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefFreerun_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x14U),
                                                     ((uint32_t) bfValue << 2),
                                                     0x4,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief When written to a 1, the internally generated SYSREF signal will be a free running pulse train based on the gen period bit field.  The internal SYSREF must be enabled and requested before the pulse train begins.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefFreerun_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefFreerun_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x14U),
                                                    &bfValueTmp,
                                                    0x4,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 2);

    return recoveryAction;
}

/**
 * \brief This bit field indicates the number of device clock cycles needed to generate an internal SYSREF pulse.  The actual number written should be number of device clock cycles minus 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                               const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                               const uint16_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4095U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x1C),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x1D),
                                                     bfValue >> 8,
                                                     0xF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief This bit field indicates the number of device clock cycles needed to generate an internal SYSREF pulse.  The actual number written should be number of device clock cycles minus 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                               const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                               uint16_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint16_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x1CU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x1DU),
                                                    &bfValueTmp,
                                                    0xF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint16_t) bfValueTmp << 8);

    return recoveryAction;
}

/**
 * \brief When written to 1, this bit initiates an internal SYSREF pulse that is one device clock cycle wide synchronized to the end of the internal SYSREF generator period.  This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefRequest_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_IntSysrefRequest_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefRequest_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x14U),
                                                     ((uint32_t) bfValue << 1),
                                                     0x2,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief When written to 1, this bit initiates an internal SYSREF pulse that is one device clock cycle wide synchronized to the end of the internal SYSREF generator period.  This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefRequest_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_IntSysrefRequest_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x14U),
                                                    &bfValueTmp,
                                                    0x2,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 1);

    return recoveryAction;
}

/**
 * \brief This bit field selects the clock provide to the Link Layer MMCM as a reference. When set to '1' the MMCM is driven by the MGT reference clock. When cleared to '0', the MMCM is driven by a dedicated clock input. The MGT reference clock is limited to 820MHz. However, the path for the using the MGT reference clock to the MMCM is limited to 512MHz. The path for the dedicated clock input is limited to 850MHz. So, the dedicated clock path is the preference path fro greater input frequency range. The MGT reference clock path is included for backward compatibility to older boards without the dedicated clock path routed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_RefclkSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                         const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_RefclkSelect_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_RefclkSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x17U),
                                                     ((uint32_t) bfValue << 7),
                                                     0x80,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief This bit field selects the clock provide to the Link Layer MMCM as a reference. When set to '1' the MMCM is driven by the MGT reference clock. When cleared to '0', the MMCM is driven by a dedicated clock input. The MGT reference clock is limited to 820MHz. However, the path for the using the MGT reference clock to the MMCM is limited to 512MHz. The path for the dedicated clock input is limited to 850MHz. So, the dedicated clock path is the preference path fro greater input frequency range. The MGT reference clock path is included for backward compatibility to older boards without the dedicated clock path routed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_RefclkSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                         uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_RefclkSelect_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x17U),
                                                    &bfValueTmp,
                                                    0x80,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 7);

    return recoveryAction;
}

/**
 * \brief General purpose Read/Write register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                    const uint32_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 4294967295U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_Adrvgen6SystemTimingControl_Scratch_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_Scratch_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x8),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x9),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0xA),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0xB),
                                                     bfValue >> 24,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief General purpose Read/Write register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_Scratch_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x8U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x9U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0xAU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0xBU),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}

/**
 * \brief When this bit is 1, a valid external SYSREF was received.  This bit is self clearing on read.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_SysrefReceived_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint8_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_SysrefReceived_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x18U),
                                                    &bfValueTmp,
                                                    0x1,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief The Minor Version indicates that a change has been made to the design to modify or add a feature.  Minor Version changes will be backward compatible with previous software releases.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_Adrvgen6SystemTimingControlChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t bfValueTmp = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if FPGAGEN6_BITFIELD_NULL_CHECK > 0
    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, bfValue);
#endif /* FPGAGEN6_BITFIELD_NULL_CHECK */

    *bfValue = (uint32_t) 0;


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_KOROR_SYSTEM_TIMING_KOROR_SYSTEM_TIMING_CONTROL))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_Adrvgen6SystemTimingControl_Version_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x0U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x1U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 8);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x2U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 16);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3U),
                                                    &bfValueTmp,
                                                    0xFF,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint32_t) bfValueTmp << 24);

    return recoveryAction;
}
