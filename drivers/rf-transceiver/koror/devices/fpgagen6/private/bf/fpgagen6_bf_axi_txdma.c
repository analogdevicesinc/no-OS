/** 
 * \file fpgagen6_bf_axi_txdma.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/fpgagen6_bf_axi_txdma.h"
#include "adi_fpgagen6_hal.h"
#include "adi_common_error.h"
#include "adi_fpgagen6_error.h"

#include "fpgagen6_bf_error_types.h"

#define ADI_FILE ADI_FPGAGEN6_FILE_PRIVATE_BF_AXI_TXDMA


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AddrdecError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_AddrdecError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_AddrdecError_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AddrdecError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_AddrdecError_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AlignToSof_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_AlignToSof_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_AlignToSof_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AlignToSof_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_AlignToSof_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ArbitrationSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_ArbitrationSelect_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_ArbitrationSelect_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ArbitrationSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_ArbitrationSelect_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_BlockId_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Busy_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Busy_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Complete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Complete_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Cyclic_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Cyclic_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Cyclic_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Cyclic_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Cyclic_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DatapathEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_DatapathEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_DatapathEnable_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DatapathEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_DatapathEnable_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DataMaskSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_DataMaskSelect_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_DataMaskSelect_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DataMaskSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_DataMaskSelect_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DataWidth_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_DataWidth_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DmaStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_DmaStatus_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_HaltComplete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_HaltComplete_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_IncludeCaptureCtrl_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_IncludeCaptureCtrl_BfGet Address");
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
 * \brief If this read only bit field is a 1, the Waveform Sequencer module is included in the design.  If it is 0, the WS has been omitted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_IncludeWs_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_IncludeWs_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x26U),
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_InstanceId_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Length_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Length_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Length_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Length_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Length_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_LengthError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_LengthError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_LengthError_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_LengthError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_LengthError_BfGet Address");
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
 * \brief When written to a 1, the data path capture control passes the token to the next DMA in the arbitration token ring. For Transmit control, setting this bit to a 1 before the data path is activated, allows the DMAs to begin fetching data from memory and fill the FIFOs in front of the data path tollgates.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_PassToken_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_PassToken_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_PassToken_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_PassToken_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_PassToken_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Qthreshold_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Qthreshold_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Qthreshold_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Qthreshold_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Qthreshold_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Scratch_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Scratch_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Scratch_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_SlaveError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_SlaveError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_SlaveError_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_SlaveError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_SlaveError_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Start_BfSet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Start_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Start_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Start_BfGet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Start_BfGet Address");
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
 * \brief Combined with START_ADDR_LOW to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrHigh_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_StartAddrHigh_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_StartAddrHigh_BfSet Address");
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
 * \brief Combined with START_ADDR_LOW to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrHigh_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_StartAddrHigh_BfGet Address");
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
 * \brief Combined with START_ADDR_HIGH to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrLow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_StartAddrLow_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_StartAddrLow_BfSet Address");
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
 * \brief Combined with START_ADDR_HIGH to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrLow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_StartAddrLow_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Stop_BfSet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Stop_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Stop_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Stop_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Stop_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Swreset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Swreset_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Swreset_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Swreset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Swreset_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TlastError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_TlastError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_TlastError_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TlastError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_TlastError_BfGet Address");
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
 * \brief Limits the transaction size of the DMA to this value and split up the transaction to satisfy the LENGTH above. This register must be set to a value less than or equal to the LENGTH.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TransSize_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_TransSize_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_TransSize_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TransSize_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_TransSize_BfGet Address");
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
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TriggerSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_TriggerSelect_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_TriggerSelect_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TriggerSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_TriggerSelect_BfGet Address");
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Underflow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_Underflow_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Underflow_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Underflow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Underflow_BfGet Address");
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
 * \brief This field indicates what the width of valid data is on the stream interface. This field is used to pack/unpack the data from the memory. The valid values are: 0: 32 bits; 1: 64 bits; 3: 128 bits; 7: 256 bits; F: 512 bits
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ValidDataWidthStream_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_ValidDataWidthStream_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_ValidDataWidthStream_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ValidDataWidthStream_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_ValidDataWidthStream_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_Version_BfGet Address");
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

/**
 * \brief Writing a 1 to the Waveform Sequencer Enable will use the start addresses, lengths, and iterations that are defined for the Waveform Sequencer in place of the default start address and length.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_WsEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x26U),
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
 * \brief Writing a 1 to the Waveform Sequencer Enable will use the start addresses, lengths, and iterations that are defined for the Waveform Sequencer in place of the default start address and length.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x26U),
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
 * \brief Number of iterations to transmit the waveform before moving to the next entry within the Waveform Sequencer.  If a 0 is written, this entry will be skipped.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsIterations_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint16_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 65535U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_AxiTxdma_WsIterations_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsIterations_BfSet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsIterations_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x10C + channelId * 16),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x10D + channelId * 16),
                                                     bfValue >> 8,
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
 * \brief Number of iterations to transmit the waveform before moving to the next entry within the Waveform Sequencer.  If a 0 is written, this entry will be skipped.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsIterations_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsIterations_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsIterations_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x10C + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x10D + channelId * 16U),
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
 * \brief Length in bytes (granularity of DATA_WIDTH) of waveform in Waveform Sequencer Entry
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsLength_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_WsLength_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsLength_BfSet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsLength_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x108 + channelId * 16),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x109 + channelId * 16),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x10A + channelId * 16),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x10B + channelId * 16),
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
 * \brief Length in bytes (granularity of DATA_WIDTH) of waveform in Waveform Sequencer Entry
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsLength_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                  uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsLength_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsLength_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x108 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x109 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x10A + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x10B + channelId * 16U),
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
 * \brief Writing a 1 advances the Waveform Sequencer to the next waveform with a non-zero number of iterations.  This bit if self-clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmAdvance_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_WsMmAdvance_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsMmAdvance_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x26U),
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
 * \brief Writing a 1 advances the Waveform Sequencer to the next waveform with a non-zero number of iterations.  This bit if self-clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmAdvance_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsMmAdvance_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x26U),
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
 * \brief Writing a 1 enables the Manual Mode.  In Manual Mode, the Waveform Sequencer dwells on any waveform definition with a non-zero number of iterations until the Manual Mode Advance bit field is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_WsMmEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsMmEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x26U),
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
 * \brief Writing a 1 enables the Manual Mode.  In Manual Mode, the Waveform Sequencer dwells on any waveform definition with a non-zero number of iterations until the Manual Mode Advance bit field is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsMmEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x26U),
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
 * \brief Combined with WS_START_ADDR_LOW to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrHigh_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_WsStartAddrHigh_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrHigh_BfSet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrHigh_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x104 + channelId * 16),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x105 + channelId * 16),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x106 + channelId * 16),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x107 + channelId * 16),
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
 * \brief Combined with WS_START_ADDR_LOW to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrHigh_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                         uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrHigh_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrHigh_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x104 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x105 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x106 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x107 + channelId * 16U),
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
 * \brief Combined with WS_START_ADDR_HIGH to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrLow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_AxiTxdma_WsStartAddrLow_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrLow_BfSet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrLow_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x100 + channelId * 16),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x101 + channelId * 16),
                                                     bfValue >> 8,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x102 + channelId * 16),
                                                     bfValue >> 16,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x103 + channelId * 16),
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
 * \brief Combined with WS_START_ADDR_HIGH to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrLow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        uint8_t channelId,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_3) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_4) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_5) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_6) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_7) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_8) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_9) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_10) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_11) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_12) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_13) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_14) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_TXDMA_15))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrLow_BfGet Address");
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
            (channelId != 0x7) 
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         channelId, 
                         "Invalid fpgagen6_AxiTxdma_WsStartAddrLow_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x100 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x101 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x102 + channelId * 16U),
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
                                                    (uint32_t) (baseAddr + 0x103 + channelId * 16U),
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
