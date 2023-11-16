/** 
 * \file fpgagen6_bf_axi_adrv904x_tx.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/fpgagen6_bf_axi_adrv904x_tx.h"
#include "adi_fpgagen6_hal.h"
#include "adi_common_error.h"
#include "adi_fpgagen6_error.h"

#include "fpgagen6_bf_error_types.h"

#define ADI_FILE ADI_FPGAGEN6_FILE_PRIVATE_BF_AXI_ADRV904X_TX


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_BlockId_BfGet Address");
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
 * \brief When high, the test is complete.  Clears when test is disabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_DetlatComplete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_DetlatComplete_BfGet Address");
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
 * \brief When high, the test is complete.  Clears when test is disabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_DetlatEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_DetlatEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_DetlatEnable_BfSet Address");
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
 * \brief When high, the test is complete.  Clears when test is disabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_DetlatEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_DetlatEnable_BfGet Address");
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_InstanceId_BfGet Address");
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
 * \brief Set to a 1 when the Command Interface is active.  Cleared to 0 when idle.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfActive_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x58U),
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
 * \brief Specify the number of lanes to distribute the Command Words across.  Valid values are:
 *        
 *        4'b0000: 1 lane;
 *        others: not supported
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfNumLanesInUse_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfNumLanesInUse_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfNumLanesInUse_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x54U),
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
 * \brief Specify the number of lanes to distribute the Command Words across.  Valid values are:
 *        
 *        4'b0000: 1 lane;
 *        others: not supported
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfNumLanesInUse_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfNumLanesInUse_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x54U),
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
 * \brief Soft reset for the core.  This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfReset_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfReset_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x56U),
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
 * \brief Soft reset for the core.  This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfReset_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x56U),
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
 * \brief When written to a 1 the JESD204C Command Interface begins sending the data in the transmit FIFO.  Any non-idle receive data is stored in the receive FIFO.  Write to a 0 to stop the interface.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfRunStop_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfRunStop_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfRunStop_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x54U),
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
 * \brief When written to a 1 the JESD204C Command Interface begins sending the data in the transmit FIFO.  Any non-idle receive data is stored in the receive FIFO.  Write to a 0 to stop the interface.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfRunStop_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfRunStop_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x54U),
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
 * \brief Data to be pushed into the TX Data FIFO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxData_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxData_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxData_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x5CU),
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
 * \brief This bit is set high when the transmit FIFO is empty.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxEmpty_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x58U),
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
 * \brief This field indicates the number of elements that are currently in the transmit FIFO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxFifoOccupancy_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxFifoOccupancy_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x59U),
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
 * \brief This bit is set high when the transmit FIFO is full.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxFull_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x58U),
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
 * \brief Set to 0 for 7 bit wide (when CRC is used in the Meta Word) or set to 1 for 19 bit wide (when Command uses the entire Meta Word).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfWidth_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfWidth_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfWidth_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x54U),
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
 * \brief Set to 0 for 7 bit wide (when CRC is used in the Meta Word) or set to 1 for 19 bit wide (when Command uses the entire Meta Word).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfWidth_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfWidth_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x54U),
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
 * \brief This bit will be a 1 if an empty condition is detected in the lane crossbar FIFO. This bit is sticky. Write a 1 to clear.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfSet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfSet Channel ID");
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
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfGet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfGet Channel ID");
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
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneFull_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_LaneFull_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneFull_BfSet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneFull_BfSet Channel ID");
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
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneFull_BfGet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneFull_BfGet Channel ID");
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
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneReset_BfSet Channel ID");
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
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneReset_BfGet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_LaneReset_BfGet Channel ID");
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
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Mode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Mode_BfGet Address");
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
 * \brief These bits control the lane mapping to the physical layer lanes. This must be set to the source Link Layer channel for this physical lane of this link. Software must make sure that across all links, a PHY channel is used only once. There is no hardware checking for programming errors. 
 *        Link 0 physical lanes consist of 8 lanes to the PHY and 8 lanes for a near-end loopback to Deframer 0.  Each of these physical lanes can be connected to 8 logical lanes from Framer 0 (values 0 - 7) and 8 far-end loopback lanes from Deframer 0 (values 8 - 15). 
 *        Link 1 physical lanes consist of 8 lanes to the PHY and 4 lanes for a near-end loopback to Deframer 1.  Each of these physical lanes can be connected to 4 logical lanes from Framer 1 (values 0 - 3).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfSet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfSet Channel ID");
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
 * \brief These bits control the lane mapping to the physical layer lanes. This must be set to the source Link Layer channel for this physical lane of this link. Software must make sure that across all links, a PHY channel is used only once. There is no hardware checking for programming errors. 
 *        Link 0 physical lanes consist of 8 lanes to the PHY and 8 lanes for a near-end loopback to Deframer 0.  Each of these physical lanes can be connected to 8 logical lanes from Framer 0 (values 0 - 7) and 8 far-end loopback lanes from Deframer 0 (values 8 - 15). 
 *        Link 1 physical lanes consist of 8 lanes to the PHY and 4 lanes for a near-end loopback to Deframer 1.  Each of these physical lanes can be connected to 4 logical lanes from Framer 1 (values 0 - 3).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfGet Address");
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
                         "Invalid fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfGet Channel ID");
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
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_Scratch_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Scratch_BfSet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Scratch_BfGet Address");
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
 * \brief This bit field controls the selection of where the SYNCB signal to the framer came from.  0: SYNCINB 0 pin;  1: SYNCINB 1 pin;  2: Deframer 0;  3: Deframer 1;  4: Deframer 2;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
                                                     (uint32_t) bfValue,
                                                     0x7,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief This bit field controls the selection of where the SYNCB signal to the framer came from.  0: SYNCINB 0 pin;  1: SYNCINB 1 pin;  2: Deframer 0;  3: Deframer 1;  4: Deframer 2;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
                                                    &bfValueTmp,
                                                    0x7,
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
 * \brief If set to 0x1, the SYSREF signal is connected to the framer. If 0x0, the SYSREF is ignored.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_SysrefEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_AxiAdrvgen6Tx_SysrefEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_SysrefEnable_BfSet Address");
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
 * \brief If set to 0x1, the SYSREF signal is connected to the framer. If 0x0, the SYSREF is ignored.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_SysrefEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_SysrefEnable_BfGet Address");
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
 *          Parameter is of type fpgagen6_Bf_AxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_AxiAdrvgen6Tx_Version_BfGet Address");
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
