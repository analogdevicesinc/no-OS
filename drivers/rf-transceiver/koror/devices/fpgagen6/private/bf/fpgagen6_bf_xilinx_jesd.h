/** 
 * \file fpgagen6_bf_xilinx_jesd.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_XILINX_JESD_H_
#define _FPGAGEN6_BF_XILINX_JESD_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_xilinx_jesd_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief 1= An 8B10B RX misalignment has been detected. Misalignment is determined by
 *        monitoring the Multi-frame framing characters. If eight consecutive framing
 *        characters are detected in misaligned positions, then this bit is asserted.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_AlignmentError8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief The amount of data in the receiver buffer.  The value returned is the number of 64-bit words in the buffer.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_BufferLevel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint16_t* const bfValue);



/**
 * \brief 1 = The receiver buffer has overflowed
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_BufferOverflow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
                                                                     const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgAdjcnt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);



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
                                                                     const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgAdjdir_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);



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
                                                                  const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgBid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



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
                                                                 const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgCf_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



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
                                                                 const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgCs_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



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
                                                                  const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgDid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



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
                                                                  const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgErr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgErrCntrEn_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief Parameter range 1–256;
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
                                                                const uint8_t bfValue);


/**
 * \brief Parameter range 1–256;
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgF_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);



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
                                                                 const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgHd_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



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
                                                                  const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgIla_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief Parameter range 1–32;
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
                                                                const uint8_t bfValue);


/**
 * \brief Parameter range 1–32;
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgK_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);



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
                                                                const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgL_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
                                                                uint8_t* const bfValue);



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
                                                                  const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgLid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint8_t* const bfValue);



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
                                                                const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgM_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);



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
                                                                      const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgMbInEmb_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



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
                                                                const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgN_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);



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
                                                                 const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



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
                                                                    const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgPhadj_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



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
                                                                   const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgRes1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



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
                                                                   const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgRes2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



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
                                                                const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgS_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);



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
                                                                  const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgScr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CfgSubclassv_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief 1 = The link has achieved Code Group Sync. This bit is applicable to an 8B10B RX only.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CgsStarted8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief When 1, the core has been configured for JESD204C (64b66b encoding).  When 0, the core has been configured for JESD204B (8b10b encoding).
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CoreIs64b66b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief When 1, the core is built as a framer (TX).  When 0, the core is built as a deframer (RX).
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CoreIsTx_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_CrcErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint16_t* const bfValue);



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
                                                                              const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_EnableCmdInterface_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



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
                                                                               const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_EnableDataInterface_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_FecCorrectedErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief When 1, FEC is included in the JESD204C device.  When 0, FEC is not included.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_FecIncluded_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_FecUncorrectedErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint16_t* const bfValue);



/**
 * \brief RX: captured configuration data from the ILA sequence (per lane)
 *        TX: Sets the values to be transmitted in the ILA sequence for all lanes.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaAdjcnt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     uint8_t* const bfValue);



/**
 * \brief ADJDIR (Adjust Direction) [Subclass 2 Only]
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaAdjdir_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                     uint8_t channelId, 
                                                                     uint8_t* const bfValue);



/**
 * \brief BID (Bank ID)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaBid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint8_t* const bfValue);



/**
 * \brief CF (Control Words per Frame)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaCf_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t channelId, 
                                                                 uint8_t* const bfValue);



/**
 * \brief CS (Control bits per Sample)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaCs_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t channelId, 
                                                                 uint8_t* const bfValue);



/**
 * \brief DID (Device ID)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaDid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaF_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
                                                                uint8_t* const bfValue);



/**
 * \brief FCHK (Checksum) [RX only, not writeable for TX]
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaFchk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);



/**
 * \brief HD (High Density format)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaHd_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t channelId, 
                                                                 uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaJesdv_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaK_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
                                                                uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaL_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
                                                                uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaLid_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint8_t* const bfValue);



/**
 * \brief M (Converters per Device)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaM_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
                                                                uint8_t* const bfValue);



/**
 * \brief Multiframes in the Transmitted Initial Lane Alignment Sequence.
 *        Parameter Range: 4–256; program the register with required value minus 1.
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
                                                                          const uint8_t bfValue);


/**
 * \brief Multiframes in the Transmitted Initial Lane Alignment Sequence.
 *        Parameter Range: 4–256; program the register with required value minus 1.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaMultiFrames_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief N (Converter Resolution)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaN_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
                                                                uint8_t* const bfValue);



/**
 * \brief N' (Totals bits per Sample)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaNp_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                 uint8_t channelId, 
                                                                 uint8_t* const bfValue);



/**
 * \brief PHADJ (Phase Adjust Request) [Subclass 2 Only]
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaPhadj_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                    uint8_t channelId, 
                                                                    uint8_t* const bfValue);



/**
 * \brief RES2 (Reserved Field 1)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaRes1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);



/**
 * \brief RES2 (Reserved Field 2)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaRes2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                   uint8_t channelId, 
                                                                   uint8_t* const bfValue);



/**
 * \brief S (Samples per Converter per Frame)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaS_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                uint8_t channelId, 
                                                                uint8_t* const bfValue);



/**
 * \brief SCR (Scrambling Enable) [RX only, not writeable for TX]
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaScr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IlaSubclass_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t channelId, 
                                                                       uint8_t* const bfValue);



/**
 * \brief 1 = 8B10B RX AXI-Stream data start interrupt triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Irq8b10bRxDataStart_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief 1 = 8B10B RX Resync request interrupt triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Irq8b10bRxResync_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



/**
 * \brief 1 = 8B10B SYNC assertion interrupt triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Irq8b10bSyncAssert_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on Block Sync error triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqBlockSyncError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on CRC error triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqCrcError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



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
                                                                                 const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bRxDataStart_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



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
                                                                              const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bRxResync_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



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
                                                                                const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEn8b10bSyncAssert_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



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
                                                                               const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnBlockSyncError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



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
                                                                         const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnCrcError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



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
                                                                         const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnFecError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



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
                                                                       const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnGlobal_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



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
                                                                           const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnLossMbLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



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
                                                                                   const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnLossSyncHeaderLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnMbError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



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
                                                                              const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnOverflowError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



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
                                                                            const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnSysrefError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



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
                                                                               const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqEnSysrefReceived_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on FEC Error triggered(TBD).
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqFecError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on Loss of Multi-block lock triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqLossMbLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on loss of 64b66b sync header lock triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqLossSyncHeaderLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on Multi-block error triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqMbError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on overflow error triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqOverflowError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



/**
 * \brief Interrupt pending.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqPending_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on SYSREF error triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqSysrefError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief 1 = Interrupt on SYSREF Received triggered.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_IrqSysrefReceived_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief 1 bit per lane. Set to 1 when alignment is achieved.  0 otherwise.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_Lane64b66bAlign_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



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
                                                                      const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_LaneEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief 1 bit per lane.  Set to 1 when alignment is achieved.  0 otherwise.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_LaneMbAlign_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief Count of total received link errors (per lane) when Link Error Counters is Enabled.
 *        Errors counted are Disparity or Not In Table errors indicated by the lane.
 *        The error counter can be reset by disabling and re-enabling using the control bit in
 *        the Error Reporting register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_LinkErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t channelId, 
                                                                          uint32_t* const bfValue);



/**
 * \brief 1 = Multi-block lock achieved on all lanes.  This bit is a logical OR of the individual lane status bits.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_MbLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



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
                                                                    const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_MetaMode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_MulitblockAlignmentErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                         uint8_t channelId, 
                                                                                         uint8_t* const bfValue);



/**
 * \brief Identifies the number of lanes built into the core.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_NumberOfLanesInCore_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



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
                                                                      const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_ResetFixed_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



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
                                                                         const uint16_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxBufferDelay_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                         uint16_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane0_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane3_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane4_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane5_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane6_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxErrStatLane7_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane0_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane1_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane3_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane4_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane5_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane6_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Bit 3: 1 = Start of Data was Detected
 *        Bit 2: 1 = Start of ILA was Detected
 *        Bit 1: 1 = Lane has Code Group Sync
 *        Bit 0: 1 = Lane is currently receiving K28.5's (BC alignment characters)
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxLinkDebugStatLane7_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



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
                                                                  const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxMbTh_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief 1 = The link has started outputting data on the AXI-Stream port. This bit is applicable
 *        to an 8B10B RX only.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_RxStarted8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief Count of error seen on lane.  The count is cumulative and is cleared on read or reset.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SyncHeaderAlignmentErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                         uint8_t channelId, 
                                                                                         uint8_t* const bfValue);



/**
 * \brief 1 = Sync Header lock achieved on all lanes.  This bit is a logical OR of all individual lane status bits.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SyncHeaderLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief 1 = The receiver has signaled SYNC has been achieved. This bit is applicable to an
 *        8B10B link only.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SyncStatus8b10b_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



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
                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefAlways_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief SYSREF captured.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefCaptured_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
                                                                       const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefDelay_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief A sysref was detected out of phase with the local extended multi-block clock.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



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
                                                                          const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_SysrefOnResync_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



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
                                                                    const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestMode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



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
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestModeErrorCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint32_t* const bfValue);



/**
 * \brief Count of total ILA Sequences received when Test Mode = 010 (Continuous ILA)
 *        This count resets to zero on transition to Test Mode = 010, and retains any count value
 *        on transition out of test mode.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestModeIlaCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint32_t* const bfValue);



/**
 * \brief Count of total ILA Multiframes received when Test Mode = 010 (Continuous ILA)
 *        This count resets to zero on transition to Test Mode = 010 and retains any count value
 *        on transition out of test mode.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxJesdChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TestModeMultiframeCount_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint32_t* const bfValue);



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
                                                                       const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_TxSyncForce_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief Version Major
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_VersionMajor_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief Version Minor
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_VersionMinor_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief Revision
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxJesd_VersionRevision_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfXilinxJesdChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);
#endif // _FPGAGEN6_BF_XILINX_JESD_H_

