/** 
 * \file fpgagen6_bf_axi_adrv904x_rx.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_AXI_ADRV904X_RX_H_
#define _FPGAGEN6_BF_AXI_ADRV904X_RX_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_rx_types.h"

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
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



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
                                                                             const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatActualNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



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
                                                                         const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatCfgF_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



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
                                                                         const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatCfgS_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



/**
 * \brief When high, the test is complete.  Clears when test is disabled.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatComplete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



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
                                                                           const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



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
                                                                               const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatExpectedNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief Gives the number of core clock cycles from the time that the test pattern was inserted into the transmit data to the time when the pattern was detected in the receive data.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_DetlatLatency_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                            uint32_t* const bfValue);



/**
 * \brief No documentation provided
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



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
                                                                              const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneBufferDelay_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneEmpty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



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
                                                                       const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneFull_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LaneReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint8_t* const bfValue);



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
                                                                             const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_LogicalLaneSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6RxChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_Mode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                   uint16_t* const bfValue);



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
                                                                      const uint32_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);



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
                                                                           const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_SysrefEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Rx_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAdrvgen6RxChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_ADRV904X_RX_H_

