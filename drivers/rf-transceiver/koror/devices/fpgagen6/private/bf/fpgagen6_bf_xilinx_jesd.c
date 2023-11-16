/** 
 * \file fpgagen6_bf_xilinx_jesd.c Automatically generated file with generator ver 0.0.16.0.
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

#include "../../private/bf/fpgagen6_bf_xilinx_jesd.h"
#include "adi_fpgagen6_hal.h"
#include "adi_common_error.h"
#include "adi_fpgagen6_error.h"

#include "fpgagen6_bf_error_types.h"

#define ADI_FILE ADI_FPGAGEN6_FILE_PRIVATE_BF_XILINX_JESD


/**
 * \brief 1= An 8B10B RX misalignment has been detected. Misalignment is determined by
 *        monitoring the Multi-frame framing characters. If eight consecutive framing
 *        characters are detected in misaligned positions, then this bit is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_AlignmentError8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_AlignmentError8b10b_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x61U),
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
 * \brief The amount of data in the receiver buffer.  The value returned is the number of 64-bit words in the buffer.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_BufferLevel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_BufferLevel_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_BufferLevel_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x400 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x401 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x3,
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
 * \brief 1 = The receiver buffer has overflowed
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_BufferOverflow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_BufferOverflow_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x61U),
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
 * \brief RX: captured configuration data from the ILA sequence (per lane)
 *        TX: Sets the values to be transmitted in the ILA sequence for all lanes.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgAdjcnt_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgAdjcnt_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgAdjcnt_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x7CU),
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
 * \brief RX: captured configuration data from the ILA sequence (per lane)
 *        TX: Sets the values to be transmitted in the ILA sequence for all lanes.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgAdjcnt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgAdjcnt_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x7CU),
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
 * \brief ADJDIR (Adjust Direction) [Subclass 2 Only]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgAdjdir_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgAdjdir_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgAdjdir_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x7EU),
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
 * \brief ADJDIR (Adjust Direction) [Subclass 2 Only]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgAdjdir_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgAdjdir_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x7EU),
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
 * \brief BID (Bank ID)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgBid_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgBid_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgBid_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x71U),
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
 * \brief BID (Bank ID)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgBid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgBid_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x71U),
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
 * \brief CF (Control Words per Frame)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgCf_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgCf_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgCf_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x7BU),
                                                     (uint32_t) bfValue,
                                                     0x1F,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief CF (Control Words per Frame)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgCf_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgCf_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x7BU),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief CS (Control bits per Sample)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgCs_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgCs_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgCs_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x77U),
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
 * \brief CS (Control bits per Sample)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgCs_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgCs_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x77U),
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
 * \brief DID (Device ID)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgDid_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgDid_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgDid_BfSet Address");
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
 * \brief DID (Device ID)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgDid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgDid_BfGet Address");
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
 * \brief 1 = Error reporting using SYNC interface Enabled
 *        0 = Error reporting using SYNC interface Disabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgErr_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgErr_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgErr_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3EU),
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
 * \brief 1 = Error reporting using SYNC interface Enabled
 *        0 = Error reporting using SYNC interface Disabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgErr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgErr_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3EU),
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
 * \brief 1 = Enable Link Error counters (Link errors are counted and reported using Link Error
 *        Count registers per lane)
 *        0 = Disable Link Error counters
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgErrCntrEn_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgErrCntrEn_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgErrCntrEn_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3EU),
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
 * \brief 1 = Enable Link Error counters (Link errors are counted and reported using Link Error
 *        Count registers per lane)
 *        0 = Disable Link Error counters
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgErrCntrEn_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgErrCntrEn_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3EU),
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
 * \brief Parameter range 1???256;
 *        Program register with required value minus 1
 *        (for example, for F = 4, 0x03 should be programmed)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgF_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgF_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgF_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3CU),
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
 * \brief Parameter range 1???256;
 *        Program register with required value minus 1
 *        (for example, for F = 4, 0x03 should be programmed)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgF_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgF_BfGet Address");
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

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief HD (High Density format)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgHd_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgHd_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgHd_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x7AU),
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
 * \brief HD (High Density format)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgHd_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgHd_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x7AU),
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
 * \brief 1 = Enable ILA Support
 *        0 = Disable ILA Support
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgIla_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgIla_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgIla_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3EU),
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
 * \brief 1 = Enable ILA Support
 *        0 = Disable ILA Support
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgIla_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgIla_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3EU),
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
 * \brief Parameter range 1???32;
 *        Program register with required value minus 1
 *        (for example, for K = 16, 0x0F should be programmed)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgK_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgK_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgK_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3DU),
                                                     (uint32_t) bfValue,
                                                     0x1F,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Parameter range 1???32;
 *        Program register with required value minus 1
 *        (for example, for K = 16, 0x0F should be programmed)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgK_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgK_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3DU),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief Number of active lanes in the link.  This information is included in the JESD204B ILAS.  Actual value should be L-1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgL_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgL_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgL_BfSet Address");
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
                         "Invalid fpgagen6_XilinxJesd_CfgL_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x406 + channelId * 128U),
                                                     (uint32_t) bfValue,
                                                     0x1F,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Number of active lanes in the link.  This information is included in the JESD204B ILAS.  Actual value should be L-1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgL_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgL_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_CfgL_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x406 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief ID of lane N. Value can be anywhere between 0 and 31. The default value N is set to
 *        the lane number. For interfaces using more than 8 lanes and hence multiple JESD204
 *        cores, this register should be programmed to ensure each lane has the correct
 *        identifier.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgLid_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgLid_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgLid_BfSet Address");
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
                         "Invalid fpgagen6_XilinxJesd_CfgLid_BfSet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x404 + channelId * 128U),
                                                     (uint32_t) bfValue,
                                                     0x1F,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief ID of lane N. Value can be anywhere between 0 and 31. The default value N is set to
 *        the lane number. For interfaces using more than 8 lanes and hence multiple JESD204
 *        cores, this register should be programmed to ensure each lane has the correct
 *        identifier.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgLid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgLid_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_CfgLid_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x404 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief M (Converters per Device)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgM_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgM_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgM_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x74U),
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
 * \brief M (Converters per Device)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgM_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgM_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x74U),
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
 * \brief Number of multi-blocks in an extended multi-block.  Program this register with the actual value.  NOTE: 0 is NOT valid.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgMbInEmb_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgMbInEmb_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgMbInEmb_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x30U),
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
 * \brief Number of multi-blocks in an extended multi-block.  Program this register with the actual value.  NOTE: 0 is NOT valid.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgMbInEmb_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgMbInEmb_BfGet Address");
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

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief N (Converter Resolution)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgN_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgN_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgN_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x75U),
                                                     (uint32_t) bfValue,
                                                     0x1F,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief N (Converter Resolution)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgN_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgN_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x75U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief N' (Totals bits per Sample)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgNp_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgNp_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgNp_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x76U),
                                                     (uint32_t) bfValue,
                                                     0x1F,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief N' (Totals bits per Sample)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgNp_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x76U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief PHADJ (Phase Adjust Request) [Subclass 2 Only]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgPhadj_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgPhadj_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgPhadj_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x7DU),
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
 * \brief PHADJ (Phase Adjust Request) [Subclass 2 Only]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgPhadj_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgPhadj_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x7DU),
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
 * \brief RES2 (Reserved Field 1)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgRes1_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgRes1_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgRes1_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x80U),
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
 * \brief RES2 (Reserved Field 1)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgRes1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgRes1_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x80U),
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
 * \brief RES2 (Reserved Field 2)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgRes2_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgRes2_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgRes2_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x81U),
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
 * \brief RES2 (Reserved Field 2)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgRes2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgRes2_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x81U),
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
 * \brief S (Samples per Converter per Frame)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgS_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgS_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgS_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x79U),
                                                     (uint32_t) bfValue,
                                                     0x1F,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief S (Samples per Converter per Frame)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgS_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgS_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x79U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief 1 = Enable Scrambling
 *        0 = Disable Scrambling
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgScr_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgScr_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgScr_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3EU),
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
 * \brief 1 = Enable Scrambling
 *        0 = Disable Scrambling
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgScr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgScr_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x3EU),
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
 * \brief 0 = Subclass 0
 *        1 = Subclass 1
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgSubclassv_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_CfgSubclassv_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgSubclassv_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x34U),
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
 * \brief 0 = Subclass 0
 *        1 = Subclass 1
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgSubclassv_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CfgSubclassv_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x34U),
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
 * \brief 1 = The link has achieved Code Group Sync. This bit is applicable to an 8B10B RX only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CgsStarted8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CgsStarted8b10b_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x61U),
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
 * \brief When 1, the core has been configured for JESD204C (64b66b encoding).  When 0, the core has been configured for JESD204B (8b10b encoding).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CoreIs64b66b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CoreIs64b66b_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x6U),
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
 * \brief When 1, the core is built as a framer (TX).  When 0, the core is built as a deframer (RX).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CoreIsTx_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CoreIsTx_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x6U),
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
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CrcErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_CrcErrorCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_CrcErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x412 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x413 + channelId * 128U),
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
 * \brief 1 = Enables the AXI4-Stream Cmd interface an the associated processing of the sync header meta data.
 *        0 = Cmd words will be zeroed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_EnableCmdInterface_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_EnableCmdInterface_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_EnableCmdInterface_BfSet Address");
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
 * \brief 1 = Enables the AXI4-Stream Cmd interface an the associated processing of the sync header meta data.
 *        0 = Cmd words will be zeroed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_EnableCmdInterface_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_EnableCmdInterface_BfGet Address");
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
 * \brief 1 = Enables the AXI4-Stream Data interface and transmits/receives data on the link.
 *        0 = The link will be transmitting/receiving scrambled 0s.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_EnableDataInterface_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_EnableDataInterface_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_EnableDataInterface_BfSet Address");
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
 * \brief 1 = Enables the AXI4-Stream Data interface and transmits/receives data on the link.
 *        0 = The link will be transmitting/receiving scrambled 0s.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_EnableDataInterface_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_EnableDataInterface_BfGet Address");
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
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_FecCorrectedErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_FecCorrectedErrorCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_FecCorrectedErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x414 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x415 + channelId * 128U),
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
 * \brief When 1, FEC is included in the JESD204C device.  When 0, FEC is not included.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_FecIncluded_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_FecIncluded_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x6U),
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
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_FecUncorrectedErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_FecUncorrectedErrorCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_FecUncorrectedErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x416 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x417 + channelId * 128U),
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
 * \brief RX: captured configuration data from the ILA sequence (per lane)
 *        TX: Sets the values to be transmitted in the ILA sequence for all lanes.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaAdjcnt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaAdjcnt_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaAdjcnt_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x448 + channelId * 128U),
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
 * \brief ADJDIR (Adjust Direction) [Subclass 2 Only]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaAdjdir_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaAdjdir_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaAdjdir_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x44A + channelId * 128U),
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
 * \brief BID (Bank ID)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaBid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaBid_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaBid_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x43D + channelId * 128U),
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
 * \brief CF (Control Words per Frame)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaCf_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaCf_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaCf_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x447 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief CS (Control bits per Sample)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaCs_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaCs_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaCs_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x443 + channelId * 128U),
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
 * \brief DID (Device ID)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaDid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaDid_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaDid_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x43C + channelId * 128U),
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
 * \brief F (Octets per Frame).
 *        Notes:
 *        1. RX only: captured configuration data from the ILA sequence (per lane);
 *        TX: Not applicable, the values transmitted in the ILA sequence are generated automatically by the core based on
 *        the configuration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaF_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaF_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaF_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x434 + channelId * 128U),
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
 * \brief FCHK (Checksum) [RX only, not writeable for TX]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaFchk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaFchk_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaFchk_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x44E + channelId * 128U),
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
 * \brief HD (High Density format)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaHd_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaHd_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaHd_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x446 + channelId * 128U),
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
 * \brief JESDV (JESD204 version)
 *        Notes:
 *        1. RX only: captured configuration data from the ILA sequence (per lane);
 *        TX: Not applicable, the values transmitted in the ILA sequence are generated automatically by the core based on
 *        the configuration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaJesdv_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaJesdv_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaJesdv_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x431 + channelId * 128U),
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
 * \brief K (Frames per Multiframe).
 *        Notes:
 *        1. RX only: captured Configuration data from the ILA sequence (per lane);
 *        TX: Not Applicable, the values transmitted in the ILA sequence are generated automatically by the core based on
 *        the configuration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaK_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaK_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaK_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x438 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief L (Lanes per Link) [RX only, not writeable for TX].
 *        Notes:
 *        1. RX: captured configuration data from the ILA sequence (per lane);
 *        TX: Sets the values to be transmitted in the ILA sequence for all lanes. LID and L values cannot be programmed,
 *        they are generated automatically by the core based on the configuration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaL_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaL_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaL_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x43F + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief LID (Lane ID) [RX only, not writeable for TX].
 *        Notes:
 *        1. RX: captured configuration data from the ILA sequence (per lane);
 *        TX: Sets the values to be transmitted in the ILA sequence for all lanes. LID and L values cannot be programmed,
 *        they are generated automatically by the core based on the configuration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaLid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaLid_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaLid_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x43E + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief M (Converters per Device)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaM_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaM_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaM_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x440 + channelId * 128U),
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
 * \brief Multiframes in the Transmitted Initial Lane Alignment Sequence.
 *        Parameter Range: 4???256; program the register with required value minus 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaMultiFrames_BfSet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          const uint8_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 3) ||
        (bfValue > 255U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IlaMultiFrames_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaMultiFrames_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x3FU),
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
 * \brief Multiframes in the Transmitted Initial Lane Alignment Sequence.
 *        Parameter Range: 4???256; program the register with required value minus 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaMultiFrames_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaMultiFrames_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

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

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief N (Converter Resolution)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaN_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaN_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaN_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x441 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief N' (Totals bits per Sample)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaNp_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaNp_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x442 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief PHADJ (Phase Adjust Request) [Subclass 2 Only]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaPhadj_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaPhadj_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaPhadj_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x449 + channelId * 128U),
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
 * \brief RES2 (Reserved Field 1)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaRes1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaRes1_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaRes1_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x44C + channelId * 128U),
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
 * \brief RES2 (Reserved Field 2)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaRes2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaRes2_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaRes2_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x44D + channelId * 128U),
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
 * \brief S (Samples per Converter per Frame)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaS_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaS_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaS_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x445 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x1F,
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
 * \brief SCR (Scrambling Enable) [RX only, not writeable for TX]
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaScr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaScr_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaScr_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x444 + channelId * 128U),
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
 * \brief SUBCLASS.
 *        Notes:
 *        1. RX only: captured configuration data from the ILA sequence (per lane);
 *        TX: Not applicable, the values transmitted in the ILA sequence are generated automatically by the core based on
 *        the configuration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaSubclass_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IlaSubclass_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_IlaSubclass_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x430 + channelId * 128U),
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
 * \brief 1 = 8B10B RX AXI-Stream data start interrupt triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Irq8b10bRxDataStart_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_Irq8b10bRxDataStart_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x69U),
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
 * \brief 1 = 8B10B RX Resync request interrupt triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Irq8b10bRxResync_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_Irq8b10bRxResync_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x69U),
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
 * \brief 1 = 8B10B SYNC assertion interrupt triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Irq8b10bSyncAssert_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_Irq8b10bSyncAssert_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x69U),
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
 * \brief 1 = Interrupt on Block Sync error triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqBlockSyncError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqBlockSyncError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x68U),
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
 * \brief 1 = Interrupt on CRC error triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqCrcError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqCrcError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x69U),
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
 * \brief 1 = Enable interrupt on 8B10B RX AXI-Stream data start.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bRxDataStart_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEn8b10bRxDataStart_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEn8b10bRxDataStart_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x65U),
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
 * \brief 1 = Enable interrupt on 8B10B RX AXI-Stream data start.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bRxDataStart_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEn8b10bRxDataStart_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x65U),
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
 * \brief 1 = Enable interrupt on 8B10B RX Resync request.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bRxResync_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEn8b10bRxResync_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEn8b10bRxResync_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x65U),
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
 * \brief 1 = Enable interrupt on 8B10B RX Resync request.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bRxResync_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEn8b10bRxResync_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x65U),
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
 * \brief 1 = Enable interrupt on 8B10B SYNC assertion.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bSyncAssert_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEn8b10bSyncAssert_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEn8b10bSyncAssert_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x65U),
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
 * \brief 1 = Enable interrupt on 8B10B SYNC assertion.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bSyncAssert_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEn8b10bSyncAssert_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x65U),
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
 * \brief 1 = Enable Interrupt on Block Sync error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnBlockSyncError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnBlockSyncError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnBlockSyncError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on Block Sync error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnBlockSyncError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnBlockSyncError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on CRC error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnCrcError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnCrcError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnCrcError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x65U),
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
 * \brief 1 = Enable Interrupt on CRC error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnCrcError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnCrcError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x65U),
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
 * \brief 1 = Enable Interrupt on FEC Error (TBD).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnFecError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnFecError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnFecError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x65U),
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
 * \brief 1 = Enable Interrupt on FEC Error (TBD).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnFecError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnFecError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x65U),
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
 * \brief Must be set for any interrupt to function.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnGlobal_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnGlobal_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnGlobal_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
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
 * \brief Must be set for any interrupt to function.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnGlobal_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnGlobal_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on Loss of Multi-block lock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnLossMbLock_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnLossMbLock_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnLossMbLock_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on Loss of Multi-block lock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnLossMbLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnLossMbLock_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on loss of 64b66b sync header lock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnLossSyncHeaderLock_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnLossSyncHeaderLock_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnLossSyncHeaderLock_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on loss of 64b66b sync header lock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnLossSyncHeaderLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnLossSyncHeaderLock_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on Multi-block error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnMbError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnMbError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnMbError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on Multi-block error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnMbError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnMbError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on overflow error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnOverflowError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnOverflowError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnOverflowError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x65U),
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
 * \brief 1 = Enable Interrupt on overflow error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnOverflowError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnOverflowError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x65U),
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
 * \brief 1 = Enable Interrupt on SYSREF error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnSysrefError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnSysrefError_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnSysrefError_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on SYSREF error.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnSysrefError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnSysrefError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on SYSREF Received.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnSysrefReceived_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_IrqEnSysrefReceived_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnSysrefReceived_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x64U),
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
 * \brief 1 = Enable Interrupt on SYSREF Received.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnSysrefReceived_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqEnSysrefReceived_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x64U),
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
 * \brief 1 = Interrupt on FEC Error triggered(TBD).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqFecError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqFecError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x69U),
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
 * \brief 1 = Interrupt on Loss of Multi-block lock triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqLossMbLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqLossMbLock_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x68U),
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
 * \brief 1 = Interrupt on loss of 64b66b sync header lock triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqLossSyncHeaderLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqLossSyncHeaderLock_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x68U),
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
 * \brief 1 = Interrupt on Multi-block error triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqMbError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqMbError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x68U),
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
 * \brief 1 = Interrupt on overflow error triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqOverflowError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqOverflowError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x69U),
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
 * \brief Interrupt pending.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqPending_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqPending_BfGet Address");
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
 * \brief 1 = Interrupt on SYSREF error triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqSysrefError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqSysrefError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x68U),
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
 * \brief 1 = Interrupt on SYSREF Received triggered.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqSysrefReceived_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_IrqSysrefReceived_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x68U),
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
 * \brief 1 bit per lane. Set to 1 when alignment is achieved.  0 otherwise.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Lane64b66bAlign_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_Lane64b66bAlign_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x54U),
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
 * \brief Default is all lanes enabled.  Set 1 bit per lane (bit 0 = lane 0, bit 1 = lane 1, etc.)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_LaneEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_LaneEnable_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_LaneEnable_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x40U),
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
 * \brief Default is all lanes enabled.  Set 1 bit per lane (bit 0 = lane 0, bit 1 = lane 1, etc.)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_LaneEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_LaneEnable_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x40U),
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
 * \brief 1 bit per lane.  Set to 1 when alignment is achieved.  0 otherwise.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_LaneMbAlign_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_LaneMbAlign_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x56U),
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
 * \brief Count of total received link errors (per lane) when Link Error Counters is Enabled.
 *        Errors counted are Disparity or Not In Table errors indicated by the lane.
 *        The error counter can be reset by disabling and re-enabling using the control bit in
 *        the Error Reporting register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_LinkErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_LinkErrorCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_LinkErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x420 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x421 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x422 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x423 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x7F,
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
 * \brief 1 = Multi-block lock achieved on all lanes.  This bit is a logical OR of the individual lane status bits.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_MbLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_MbLock_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x60U),
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
 * \brief 0 = CRC12; 1 = CRC3(TBD); 2 = CMD; 3 = FEC
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_MetaMode_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_MetaMode_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_MetaMode_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x38U),
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
 * \brief 0 = CRC12; 1 = CRC3(TBD); 2 = CMD; 3 = FEC
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_MetaMode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_MetaMode_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x38U),
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
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_MulitblockAlignmentErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_MulitblockAlignmentErrorCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_MulitblockAlignmentErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x411 + channelId * 128U),
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
 * \brief Identifies the number of lanes built into the core.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_NumberOfLanesInCore_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_NumberOfLanesInCore_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x4U),
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
 * \brief Write 1 to hold the core in the reset state. Write 0 to release the core from reset. After setting this bit to 1 and clearing back to 0, this bit will read back 1 until the reset process has completed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_ResetFixed_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_ResetFixed_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_ResetFixed_BfSet Address");
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
 * \brief Write 1 to hold the core in the reset state. Write 0 to release the core from reset. After setting this bit to 1 and clearing back to 0, this bit will read back 1 until the reset process has completed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_ResetFixed_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_ResetFixed_BfGet Address");
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
 * \brief Advance the release of the receiver buffer by N 64-bit words.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxBufferDelay_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                         const uint16_t bfValue)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);


#if FPGAGEN6_BITFIELD_VALUE_CHECK > 0
    if ((bfValue < 0) ||
        (bfValue > 1023U))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         bfValue,
                         "Invalid BitField Value in fpgagen6_XilinxJesd_RxBufferDelay_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxBufferDelay_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x44),
                                                     bfValue,
                                                     0xFF,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x45),
                                                     bfValue >> 8,
                                                     0x3,
                                                     0x0);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegisterFieldWrite Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
 * \brief Advance the release of the receiver buffer by N 64-bit words.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxBufferDelay_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxBufferDelay_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x44U),
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
                                                    (uint32_t) (baseAddr + 0x45U),
                                                    &bfValueTmp,
                                                    0x3,
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane0_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane0_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x58U),
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane1_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x58U),
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane2_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x59U),
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane3_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane3_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x59U),
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane4_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane4_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5AU),
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane5_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane5_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5AU),
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane6_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane6_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5BU),
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
 * \brief Bit 3: unused
 *        Bit 2: Unexpected K-character(s) received
 *        Bit 1: Disparity Error(s) received
 *        Bit 0: Not in table Error(s) received
 *        Each bit indicates that 1 or more errors of that type have been received in Lane 0 since
 *        the register was last read.
 *        All status bits are cleared to 0 on read of this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane7_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxErrStatLane7_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5BU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane0_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane0_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5CU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane1_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5CU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane2_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5DU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane3_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane3_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5DU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane4_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane4_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5EU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane5_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane5_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5EU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane6_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane6_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5FU),
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
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane7_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxLinkDebugStatLane7_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x5FU),
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
 * \brief How many correct/incorrect multi-block alignment markers are required to achieve/lose multi-block lock.  The actual value used is 1 plus the number in this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxMbTh_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_RxMbTh_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxMbTh_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x4CU),
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
 * \brief How many correct/incorrect multi-block alignment markers are required to achieve/lose multi-block lock.  The actual value used is 1 plus the number in this register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxMbTh_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxMbTh_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x4CU),
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
 * \brief 1 = The link has started outputting data on the AXI-Stream port. This bit is applicable
 *        to an 8B10B RX only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxStarted8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_RxStarted8b10b_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x61U),
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
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SyncHeaderAlignmentErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SyncHeaderAlignmentErrorCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_SyncHeaderAlignmentErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x410 + channelId * 128U),
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
 * \brief 1 = Sync Header lock achieved on all lanes.  This bit is a logical OR of all individual lane status bits.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SyncHeaderLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SyncHeaderLock_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x60U),
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
 * \brief 1 = The receiver has signaled SYNC has been achieved. This bit is applicable to an
 *        8B10B link only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SyncStatus8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SyncStatus8b10b_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x61U),
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
 * \brief 1 = The core will align the LMFC/LEMC counter on all SYSREF events.
 *        0 = The core will only align the LMFC/LEMC counter on the first SYSREF event
 *        following a reset, all subsequent SYSREF events will be ignored.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefAlways_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_SysrefAlways_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefAlways_BfSet Address");
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
 * \brief 1 = The core will align the LMFC/LEMC counter on all SYSREF events.
 *        0 = The core will only align the LMFC/LEMC counter on the first SYSREF event
 *        following a reset, all subsequent SYSREF events will be ignored.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefAlways_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefAlways_BfGet Address");
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
 * \brief SYSREF captured.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefCaptured_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefCaptured_BfGet Address");
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
 * \brief SYSREF delay: add additional delay to SYSREF re-alignment of LMFC counter
 *        1111 = 15 core_clk cycles delay
 *        ....
 *        0000 = 0 core_clk cycles delay
 *        This register is used to retard the phase of the local extended multi-block clock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefDelay_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_SysrefDelay_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefDelay_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x52U),
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
 * \brief SYSREF delay: add additional delay to SYSREF re-alignment of LMFC counter
 *        1111 = 15 core_clk cycles delay
 *        ....
 *        0000 = 0 core_clk cycles delay
 *        This register is used to retard the phase of the local extended multi-block clock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefDelay_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefDelay_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x52U),
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
 * \brief A sysref was detected out of phase with the local extended multi-block clock.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefError_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x60U),
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
 * \brief 1 = Following a Link Re-Sync event, a SYSREF event is required to re-align the local
 *        LMFC/LEMC before the link will operate.
 *        0 = No SYSREF is required to restart a link after a Re-sync event.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefOnResync_BfSet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_SysrefOnResync_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefOnResync_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x50U),
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
 * \brief 1 = Following a Link Re-Sync event, a SYSREF event is required to re-align the local
 *        LMFC/LEMC before the link will operate.
 *        0 = No SYSREF is required to restart a link after a Re-sync event.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefOnResync_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_SysrefOnResync_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x50U),
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
 * \brief Test mode select (8B10B mode):
 *        000 = Normal operation
 *        001 = Transmit receive /K28.5/ indefinitely
 *        010 = Synchronize as normal then transmit/receive repeated ILA sequences.
 *        011 = Transmit D21.5 indefinitely (Tx only)
 *        101 = Transmit Modified Random Pattern RPAT (Tx Only)
 *        111 = Transmit Scrambled Jitter Pattern JSPAT (Tx Only)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestMode_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_TestMode_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_TestMode_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x48U),
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
 * \brief Test mode select (8B10B mode):
 *        000 = Normal operation
 *        001 = Transmit receive /K28.5/ indefinitely
 *        010 = Synchronize as normal then transmit/receive repeated ILA sequences.
 *        011 = Transmit D21.5 indefinitely (Tx only)
 *        101 = Transmit Modified Random Pattern RPAT (Tx Only)
 *        111 = Transmit Scrambled Jitter Pattern JSPAT (Tx Only)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestMode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_TestMode_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x48U),
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
 * \brief Count of Errors received in Data link Layer test modes.
 *        Test Mode = 001 (Continuous K28.5): counts any non K28.5 characters received
 *        Test Mode = 010 (Continuous ILA): counts any unexpected characters received
 *        This count resets to zero on transition to an active test mode and retains any count
 *        value on transition out of an active test mode.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestModeErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_TestModeErrorCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_TestModeErrorCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x424 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x425 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x426 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x427 + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x7F,
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
 * \brief Count of total ILA Sequences received when Test Mode = 010 (Continuous ILA)
 *        This count resets to zero on transition to Test Mode = 010, and retains any count value
 *        on transition out of test mode.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestModeIlaCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_TestModeIlaCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_TestModeIlaCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x428 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x429 + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x42A + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x42B + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x7F,
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
 * \brief Count of total ILA Multiframes received when Test Mode = 010 (Continuous ILA)
 *        This count resets to zero on transition to Test Mode = 010 and retains any count value
 *        on transition out of test mode.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestModeMultiframeCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_TestModeMultiframeCount_BfGet Address");
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
                         "Invalid fpgagen6_XilinxJesd_TestModeMultiframeCount_BfGet Channel ID");
        return recoveryAction;
    }
#endif /* FPGAGEN6_CHANNEL_ID_CHECK */

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x42C + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x42D + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x42E + channelId * 128U),
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
                                                    (uint32_t) (baseAddr + 0x42F + channelId * 128U),
                                                    &bfValueTmp,
                                                    0x7F,
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
 * \brief Force on 8b10b transmitter.  When set to 1, this register overrides the value on the tx_sync pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TxSyncForce_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
                         "Invalid BitField Value in fpgagen6_XilinxJesd_TxSyncForce_BfSet");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_VALUE_CHECK */


#if FPGAGEN6_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_TxSyncForce_BfSet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0x28U),
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
 * \brief Force on 8b10b transmitter.  When set to 1, this register overrides the value on the tx_sync pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TxSyncForce_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_TxSyncForce_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_fpgagen6_RegisterFieldRead(device,
                                                    (uint32_t) (baseAddr + 0x28U),
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
 * \brief Version Major
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_VersionMajor_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_VersionMajor_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

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

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief Version Minor
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_VersionMinor_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_VersionMinor_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

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

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}

/**
 * \brief Revision
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_Bf_XilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_VersionRevision_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
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
    if ((baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204B_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_RX_204C_DEFRAMER_2) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204B_FRAMER_1) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_0) &&
            (baseAddr != FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_TX_204C_FRAMER_1))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_FPGAGEN6_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM,
                         baseAddr, 
                         "Invalid fpgagen6_XilinxJesd_VersionRevision_BfGet Address");
        return recoveryAction;
    }
#endif /* FPGAGEN6_BITFIELD_ADDR_CHECK */ 

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

    *bfValue = (uint8_t) bfValueTmp;

    return recoveryAction;
}
