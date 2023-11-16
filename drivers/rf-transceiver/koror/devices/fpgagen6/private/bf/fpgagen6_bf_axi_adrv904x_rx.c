/** 
 * \file fpgagen6_bf_axi_adrv904x_rx.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/fpgagen6_bf_axi_adrv904x_rx.h"
#include "adi_fpgagen6_hal.h"
#include "adi_common_error.h"
#include "adi_fpgagen6_error.h"

#include "fpgagen6_bf_error_types.h"

#define ADI_FILE ADI_FPGAGEN6_FILE_PRIVATE_BF_AXI_ADRV904X_RX


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_BlockId_BfGet Address");
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
 * \brief Identifies the N' of the actual data.  This should match the N' of the received sample.  Valid values are:
 *        'h0: 12 bits; 
 *        'h1: 16 bits; 
 *        'h2: 24 bits;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatActualNp_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                             const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_DetlatActualNp_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatActualNp_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x61U),
                                                     (uint32_t) bfValue,
                                                     0x3,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Identifies the N' of the actual data.  This should match the N' of the received sample.  Valid values are:
 *        'h0: 12 bits; 
 *        'h1: 16 bits; 
 *        'h2: 24 bits;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatActualNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatActualNp_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x61U),
                                                    &bfValueTmp,
                                                    0x3,
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
 * \brief Identifies the number of octets per frame in the receive link.  Used to pick out C0 of the incoming stream for analysis.  Value written is F-1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatCfgF_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                         const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 31U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_DetlatCfgF_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatCfgF_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x61),
                                                     bfValue << 4,
                                                     0xF0,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x62),
                                                     bfValue >> 4,
                                                     0x1,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Identifies the number of octets per frame in the receive link.  Used to pick out C0 of the incoming stream for analysis.  Value written is F-1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatCfgF_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatCfgF_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x61U),
                                                    &bfValueTmp,
                                                    0xF0,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint8_t) bfValueTmp >> 4);

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x62U),
                                                    &bfValueTmp,
                                                    0x1,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue |= ((uint8_t) bfValueTmp << 4);

    return recoveryAction;
}

/**
 * \brief Identifies the number of samples per converter per frame in the receive link.  Used to pick out C0 of the incoming stream for analysis.  Value written is S-1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatCfgS_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                         const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_DetlatCfgS_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatCfgS_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x62U),
                                                     ((uint32_t) bfValue << 4),
                                                     0x30,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Identifies the number of samples per converter per frame in the receive link.  Used to pick out C0 of the incoming stream for analysis.  Value written is S-1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatCfgS_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatCfgS_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x62U),
                                                    &bfValueTmp,
                                                    0x30,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

/**
 * \brief When high, the test is complete.  Clears when test is disabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatComplete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatComplete_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x60U),
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
 * \brief When written to a 1, the test is enabled.  When the test is complete, this bit must be written to 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_DetlatEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x60U),
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
 * \brief When written to a 1, the test is enabled.  When the test is complete, this bit must be written to 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x60U),
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
 * \brief Identifies the N' of the expected data.  This should match the N' of the transmitted sample.  Valid values are:
 *        'h0: 12 bits; 
 *        'h1: 16 bits; 
 *        'h2: 24 bits;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatExpectedNp_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                               const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 3U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_DetlatExpectedNp_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatExpectedNp_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x60U),
                                                     ((uint32_t) bfValue << 4),
                                                     0x30,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Identifies the N' of the expected data.  This should match the N' of the transmitted sample.  Valid values are:
 *        'h0: 12 bits; 
 *        'h1: 16 bits; 
 *        'h2: 24 bits;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatExpectedNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatExpectedNp_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x60U),
                                                    &bfValueTmp,
                                                    0x30,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 4);

    return recoveryAction;
}

/**
 * \brief Gives the number of core clock cycles from the time that the test pattern was inserted into the transmit data to the time when the pattern was detected in the receive data.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatLatency_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_DetlatLatency_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
                                                    (uint32_t) (baseAddr + 0x65U),
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
                                                    (uint32_t) (baseAddr + 0x66U),
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
                                                    (uint32_t) (baseAddr + 0x67U),
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
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_InstanceId_BfGet Address");
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
 * \brief This bitfield only applies to the JESD204C lane FIFOs.  It should be set to ((E-1)*32)+16.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                              const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x70U),
                                                     (uint32_t) bfValue,
                                                     0xFF,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief This bitfield only applies to the JESD204C lane FIFOs.  It should be set to ((E-1)*32)+16.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x70U),
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
 * \brief This bit will be a 1 if an empty condition is detected in the lane crossbar FIFO. This bit is sticky. Write a 1 to clear.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x12 + channelId * 4U),
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
 * \brief This bit will be a 1 if an empty condition is detected in the lane crossbar FIFO. This bit is sticky. Write a 1 to clear.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                        uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x12 + channelId * 4U),
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
 * \brief This bit will be a 1 if a full condition is detected in the lane crossbar FIFO. This bit is sticky. Write a 1 to clear.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneFull_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_LaneFull_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneFull_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneFull_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x12 + channelId * 4U),
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
 * \brief This bit will be a 1 if a full condition is detected in the lane crossbar FIFO. This bit is sticky. Write a 1 to clear.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                       uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneFull_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneFull_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x12 + channelId * 4U),
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
 * \brief If set to 0x1, the corresponding lane is held in reset. Clear the bit to 0 to allow data to flow. This has no effect on the PHY cores, only the data path from PHY is either disabled or enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneReset_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x13 + channelId * 4U),
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
 * \brief If set to 0x1, the corresponding lane is held in reset. Clear the bit to 0 to allow data to flow. This has no effect on the PHY cores, only the data path from PHY is either disabled or enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                        uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneReset_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LaneReset_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x13 + channelId * 4U),
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
 * \brief These bits control the lane mapping to the link layer logical lanes. This must be set to the source PHY channel for this lane of this link. Software must make sure that across all links, a PHY channel is used only once. There is no hardware checking for programming errors. Deframer 0 has 8 logical lanes and can select from 8 physical lanes (values 0 - 7) and 8 loopback lanes from Framer 0 (values 8 - 15). Deframer 1 has 4 logical lanes and can select from 8 physical lanes (values 0 - 7) and four loopback lanes from Framer 1 (values 8 - 11). Deframer 2 has 2 logical lanes and can select from 8 physical lanes (values 0 - 7).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 15U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x10 + channelId * 4U),
                                                     (uint32_t) bfValue,
                                                     0xF,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief These bits control the lane mapping to the link layer logical lanes. This must be set to the source PHY channel for this lane of this link. Software must make sure that across all links, a PHY channel is used only once. There is no hardware checking for programming errors. Deframer 0 has 8 logical lanes and can select from 8 physical lanes (values 0 - 7) and 8 loopback lanes from Framer 0 (values 8 - 15). Deframer 1 has 4 logical lanes and can select from 8 physical lanes (values 0 - 7) and four loopback lanes from Framer 1 (values 8 - 11). Deframer 2 has 2 logical lanes and can select from 8 physical lanes (values 0 - 7).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                             uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 


#if FPGAGEN6_CHANNELID_CHECK > 0
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
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x10 + channelId * 4U),
                                                    &bfValueTmp,
                                                    0xF,
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
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_Mode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_Mode_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x6U),
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
                                                    (uint32_t) (baseAddr + 0x7U),
                                                    &bfValueTmp,
                                                    0xFF,
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_Scratch_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_Scratch_BfSet Address");
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_Scratch_BfGet Address");
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
 * \brief If set to 0x1, the SYSREF signal is connected to the deframer. If 0x0, the SYSREF is ignored.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_SysrefEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Rx_SysrefEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_SysrefEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x50U),
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
 * \brief If set to 0x1, the SYSREF signal is connected to the deframer. If 0x0, the SYSREF is ignored.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_SysrefEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_SysrefEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x50U),
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
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_2))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Rx_Version_BfGet Address");
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
