/** 
 * \file fpgagen6_bf_axi_rxdma.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/fpgagen6_bf_axi_rxdma.h"
#include "adi_fpgagen6_hal.h"
#include "adi_common_error.h"
#include "adi_fpgagen6_error.h"

#include "fpgagen6_bf_error_types.h"

#define ADI_FILE ADI_FPGAGEN6_FILE_PRIVATE_BF_AXI_RXDMA


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_AddrdecError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_AddrdecError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_AddrdecError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x80U),
                                                     ((uint32_t) bfValue << 6),
                                                     0x40,
                                                     0x0U);
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_AddrdecError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_AddrdecError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
                                                    &bfValueTmp,
                                                    0x40,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 6);

    return recoveryAction;
}

/**
 * \brief Allows DMA to align the captured data to the first valid data after the Start of Frame is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_AlignToSof_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_AlignToSof_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_AlignToSof_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x24U),
                                                     ((uint32_t) bfValue << 3),
                                                     0x8,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Allows DMA to align the captured data to the first valid data after the Start of Frame is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_AlignToSof_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_AlignToSof_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x24U),
                                                    &bfValueTmp,
                                                    0x8,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 3);

    return recoveryAction;
}

/**
 * \brief This allows arbitration of transactions among multiple DMA cores. 0x0: Default, always granted access to AXI bus. 0x1: Data-Threshold, posts transaction when a predefined number of samples are collected (fifo-not-empty). 0x2: Token-Ring, posts transaction when a predefined token is received, after that token is released. The master core always initiates the token. 0x3: Token-Ring + Data-Threshold, posts transaction when a predefined token is received and a predefined number of samples are collected (fifo-not-empty), after that token is released. The master core always initiates the token.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_ArbitrationSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_ArbitrationSelect_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_ArbitrationSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x25U),
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
 * \brief This allows arbitration of transactions among multiple DMA cores. 0x0: Default, always granted access to AXI bus. 0x1: Data-Threshold, posts transaction when a predefined number of samples are collected (fifo-not-empty). 0x2: Token-Ring, posts transaction when a predefined token is received, after that token is released. The master core always initiates the token. 0x3: Token-Ring + Data-Threshold, posts transaction when a predefined token is received and a predefined number of samples are collected (fifo-not-empty), after that token is released. The master core always initiates the token.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_ArbitrationSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_ArbitrationSelect_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x25U),
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_BlockId_BfGet Address");
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Busy_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Busy_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Complete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Complete_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
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
 * \brief Allows DMA to be cycled through until terminated with the same settings.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Cyclic_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Cyclic_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Cyclic_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x24U),
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
 * \brief Allows DMA to be cycled through until terminated with the same settings.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Cyclic_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Cyclic_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x24U),
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
 * \brief When written to a 1, the data paths will activate as long as the DMAs are active. Important Note, the DMAs must be configured and activated prior to activating the data paths. The bit will automatically clear when all of the DMAs are no longer active. This bit can be written to 0 to force the data paths to deactivate.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_DatapathEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_DatapathEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_DatapathEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x26U),
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
 * \brief When written to a 1, the data paths will activate as long as the DMAs are active. Important Note, the DMAs must be configured and activated prior to activating the data paths. The bit will automatically clear when all of the DMAs are no longer active. This bit can be written to 0 to force the data paths to deactivate.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_DatapathEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_DatapathEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x26U),
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
 * \brief The incoming data stream can be masked based on one of 32 signals such as TDD enables.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_DataMaskSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_DataMaskSelect_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_DataMaskSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3C),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3D),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3E),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3F),
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
 * \brief The incoming data stream can be masked based on one of 32 signals such as TDD enables.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_DataMaskSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_DataMaskSelect_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3CU),
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
                                                    (uint32_t) (baseAddr + 0x3DU),
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
                                                    (uint32_t) (baseAddr + 0x3EU),
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
                                                    (uint32_t) (baseAddr + 0x3FU),
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
 * \brief Software may use this information to compute the granularity of an AXI transaction. This may be needed to compute burst size and length settings.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_DataWidth_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_DataWidth_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x10U),
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
                                                    (uint32_t) (baseAddr + 0x11U),
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
                                                    (uint32_t) (baseAddr + 0x12U),
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
                                                    (uint32_t) (baseAddr + 0x13U),
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
 * \brief When this bit is set to 1, it indicates that one or more DMAs are active.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_DmaStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_DmaStatus_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x81U),
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_HaltComplete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_HaltComplete_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
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
 * \brief If 1 DMA includes the capture control module and if 0 Capture control is bypassed
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_IncludeCaptureCtrl_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_IncludeCaptureCtrl_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x24U),
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_InstanceId_BfGet Address");
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
 * \brief Length in bytes (granularity of DATA_WIDTH)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Length_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Length_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Length_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x30),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x31),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x32),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x33),
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
 * \brief Length in bytes (granularity of DATA_WIDTH)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Length_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Length_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_LengthError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_LengthError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_LengthError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x80U),
                                                     ((uint32_t) bfValue << 4),
                                                     0x10,
                                                     0x0U);
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_LengthError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_LengthError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
                                                    &bfValueTmp,
                                                    0x10,
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Overflow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Overflow_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Overflow_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x80U),
                                                     ((uint32_t) bfValue << 3),
                                                     0x8,
                                                     0x0U);
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Overflow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Overflow_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
                                                    &bfValueTmp,
                                                    0x8,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 3);

    return recoveryAction;
}

/**
 * \brief When written to a 1, the data path capture control passes the token to the next DMA in the arbitration token ring. For Transmit control, setting this bit to a 1 before the data path is activated, allows the DMAs to begin fetching data from memory and fill the FIFOs in front of the data path tollgates.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_PassToken_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_PassToken_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_PassToken_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x26U),
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
 * \brief When written to a 1, the data path capture control passes the token to the next DMA in the arbitration token ring. For Transmit control, setting this bit to a 1 before the data path is activated, allows the DMAs to begin fetching data from memory and fill the FIFOs in front of the data path tollgates.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_PassToken_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_PassToken_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x26U),
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
 * \brief This bit may be set to the desired outstanding transactions in the data mover queue. This allows multiple transactions posted in the AXI bus. The number of outstanding transactions is value + 1 for up to 8.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Qthreshold_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
                                                                    const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 7U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Qthreshold_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Qthreshold_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x24U),
                                                     ((uint32_t) bfValue << 4),
                                                     0x70,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief This bit may be set to the desired outstanding transactions in the data mover queue. This allows multiple transactions posted in the AXI bus. The number of outstanding transactions is value + 1 for up to 8.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Qthreshold_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Qthreshold_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x24U),
                                                    &bfValueTmp,
                                                    0x70,
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Scratch_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Scratch_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Scratch_BfGet Address");
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_SlaveError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_SlaveError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_SlaveError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x80U),
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_SlaveError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_SlaveError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
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
 * \brief Software may set this bit to initiate a DMA request. This bit is cleared when the DMA is completed or terminated (see STOP bit below).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Start_BfSet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Start_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Start_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x24U),
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
 * \brief Software may set this bit to initiate a DMA request. This bit is cleared when the DMA is completed or terminated (see STOP bit below).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Start_BfGet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Start_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x24U),
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
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_StartAddrHigh_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_StartAddrHigh_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_StartAddrHigh_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x2C),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x2D),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x2E),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x2F),
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_StartAddrHigh_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_StartAddrHigh_BfGet Address");
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_StartAddrLow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_StartAddrLow_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_StartAddrLow_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x28),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x29),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x2A),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x2B),
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_StartAddrLow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_StartAddrLow_BfGet Address");
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
 * \brief Software may set this bit to initiate a premature DMA termination. This bit is cleared when the request is completed. The stop bit is effective only if the START bit is set.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Stop_BfSet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Stop_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Stop_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x24U),
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
 * \brief Software may set this bit to initiate a premature DMA termination. This bit is cleared when the request is completed. The stop bit is effective only if the START bit is set.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Stop_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Stop_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x24U),
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
 * \brief This is self-clearing and resets the core. There is no need to reset the core between multiple DMA requests. All DMA requests is gracefully completed and all internal elements are cleared (no stale data).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Swreset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_Swreset_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Swreset_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x20U),
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
 * \brief This is self-clearing and resets the core. There is no need to reset the core between multiple DMA requests. All DMA requests is gracefully completed and all internal elements are cleared (no stale data).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Swreset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Swreset_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x20U),
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
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_TlastError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_TlastError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_TlastError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x80U),
                                                     ((uint32_t) bfValue << 5),
                                                     0x20,
                                                     0x0U);
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
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_TlastError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_TlastError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
                                                    &bfValueTmp,
                                                    0x20,
                                                    0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldRead Issue");
        return recoveryAction;
    }

    *bfValue = (uint8_t) (bfValueTmp >> 5);

    return recoveryAction;
}

/**
 * \brief When '1', the tollgate is open.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_TollgateOpen_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_TollgateOpen_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x81U),
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
 * \brief Limits the transaction size of the DMA to this value and split up the transaction to satisfy the LENGTH above. This register must be set to a value less than or equal to the LENGTH.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_TransSize_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_TransSize_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_TransSize_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x34),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x35),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x36),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x37),
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
 * \brief Limits the transaction size of the DMA to this value and split up the transaction to satisfy the LENGTH above. This register must be set to a value less than or equal to the LENGTH.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_TransSize_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_TransSize_BfGet Address");
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
 * \brief Allows DMA to begin based on a hardware event.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_TriggerSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_TriggerSelect_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_TriggerSelect_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x38),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x39),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3A),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3B),
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
 * \brief Allows DMA to begin based on a hardware event.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_TriggerSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_TriggerSelect_BfGet Address");
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
 * \brief This field indicates what the width of valid data is on the stream interface. This field is used to pack/unpack the data from the memory. The valid values are: 0: 32 bits; 1: 64 bits; 3: 128 bits; 7: 256 bits; F: 512 bits
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_ValidDataWidthStream_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiRxdma_ValidDataWidthStream_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_ValidDataWidthStream_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x25U),
                                                     ((uint32_t) bfValue << 4),
                                                     0xF0,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief This field indicates what the width of valid data is on the stream interface. This field is used to pack/unpack the data from the memory. The valid values are: 0: 32 bits; 1: 64 bits; 3: 128 bits; 7: 256 bits; F: 512 bits
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_ValidDataWidthStream_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_ValidDataWidthStream_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x25U),
                                                    &bfValueTmp,
                                                    0xF0,
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
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiRxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiRxdma_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiRxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_RXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiRxdma_Version_BfGet Address");
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
