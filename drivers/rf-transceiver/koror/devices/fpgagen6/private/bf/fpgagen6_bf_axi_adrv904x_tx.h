/** 
 * \file fpgagen6_bf_axi_adrv904x_tx.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_AXI_ADRV904X_TX_H_
#define _FPGAGEN6_BF_AXI_ADRV904X_TX_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_tx_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief No documentation provided
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_DetlatComplete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



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
                                                                           const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_DetlatEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief No documentation provided
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



/**
 * \brief Set to a 1 when the Command Interface is active.  Cleared to 0 when idle.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);



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
                                                                                         const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfNumLanesInUse_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                         uint8_t* const bfValue);



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
                                                                                 const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



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
                                                                                   const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfRunStop_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



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
                                                                                  const uint8_t bfValue);



/**
 * \brief This bit is set high when the transmit FIFO is empty.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



/**
 * \brief This field indicates the number of elements that are currently in the transmit FIFO.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxFifoOccupancy_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue);



/**
 * \brief This bit is set high when the transmit FIFO is full.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfTxFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);



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
                                                                                 const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Jesd204cCmdIfWidth_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



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
                                                                       const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_LaneReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6TxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Mode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                   uint16_t* const bfValue);



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
                                                                              const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_PhysicalLaneSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);



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
                                                                      const uint32_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);



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
                                                                             const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_SyncbSourceSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



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
                                                                           const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_SysrefEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Tx_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6TxChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_ADRV904X_TX_H_

