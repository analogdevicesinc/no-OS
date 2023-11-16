/** 
 * \file fpgagen6_bf_axi_adrv904x_phy.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_AXI_ADRV904X_PHY_H_
#define _FPGAGEN6_BF_AXI_ADRV904X_PHY_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_phy_types.h"

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
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief Reflects the lock of each Channel PLL's lock signal.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelCpllLock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);



/**
 * \brief Controls the power down for a Channel PLL. When a bit is set to 1, the corresponding Channel PLL is powered down. When the bit is cleared to 0, the corresponding Channel PLL powers up. This is the recommended way to reset the Channel PLL.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelCpllPd_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);


/**
 * \brief Controls the power down for a Channel PLL. When a bit is set to 1, the corresponding Channel PLL is powered down. When the bit is cleared to 0, the corresponding Channel PLL powers up. This is the recommended way to reset the Channel PLL.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelCpllPd_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief Selects the input reference clock to the Channel PLL. Set this input to 3'b001 when only one clock source is connected to the Channel PLL reference clock selection multiplexer. Reset must be applied to the Channel PLL after changing the reference clock input. 000: Reserved; 001: GTREFCLK0 selected; 010: GTREFCLK1 selected; 011: GTNORTHREFCLK0 selected; 100: GTNORTHREFCLK1 selected; 101: GTSOUTHREFCLK0 selected; 110: GTSOUTHREFCLK1 selected; 111: GTGREFCLK selected
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelCpllRefClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint8_t bfValue);


/**
 * \brief Selects the input reference clock to the Channel PLL. Set this input to 3'b001 when only one clock source is connected to the Channel PLL reference clock selection multiplexer. Reset must be applied to the Channel PLL after changing the reference clock input. 000: Reserved; 001: GTREFCLK0 selected; 010: GTREFCLK1 selected; 011: GTNORTHREFCLK0 selected; 100: GTNORTHREFCLK1 selected; 101: GTSOUTHREFCLK0 selected; 110: GTSOUTHREFCLK1 selected; 111: GTGREFCLK selected
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelCpllRefClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint8_t* const bfValue);



/**
 * \brief This field is the address of the DRP register being accessed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpAddress_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint16_t bfValue);


/**
 * \brief This field is the address of the DRP register being accessed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpAddress_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint16_t* const bfValue);



/**
 * \brief When set to 1, the DRP access, read or write, is initiated. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                const uint8_t bfValue);


/**
 * \brief When set to 1, the DRP access, read or write, is initiated. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);



/**
 * \brief If a timeout occurs on the DRP access, this bit will be set to 1. In the case of an error, the DRP must be reset using the drprst bit. This bit is clear on read.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);



/**
 * \brief When the DRP access is complete, this bit will be set to 1. The bit will clear on read.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpReady_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);



/**
 * \brief When a read access is performed the data read from the DRP register is placed in this field.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpReadData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint16_t* const bfValue);



/**
 * \brief Reading read-only registers while the XCLK is not toggling (e.g., during reset or change of reference clocks) causes the DRP to not return a DRPRDY signal and prevent further DRP transactions. In such an event, drprst must be pulsed to reset the DRP interface before initiating further DRP transactions. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);


/**
 * \brief Reading read-only registers while the XCLK is not toggling (e.g., during reset or change of reference clocks) causes the DRP to not return a DRPRDY signal and prevent further DRP transactions. In such an event, drprst must be pulsed to reset the DRP interface before initiating further DRP transactions. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);



/**
 * \brief The number of DRP clock cycle to wait for DRP Ready to go high before determining if there is an error condition.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpTimeout_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint32_t bfValue);


/**
 * \brief The number of DRP clock cycle to wait for DRP Ready to go high before determining if there is an error condition.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpTimeout_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint32_t* const bfValue);



/**
 * \brief This field is the data that will be written to the DRP register in a write access.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteData_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint16_t bfValue);


/**
 * \brief This field is the data that will be written to the DRP register in a write access.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint16_t* const bfValue);



/**
 * \brief This field is used to mask which bits of the write data that will be written to the DRP register. Each data bit that corresponds to a mask bit that is set to 1 will be written to the DRP register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteDataMask_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint16_t bfValue);


/**
 * \brief This field is used to mask which bits of the write data that will be written to the DRP register. Each data bit that corresponds to a mask bit that is set to 1 will be written to the DRP register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteDataMask_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint16_t* const bfValue);



/**
 * \brief When set to 1, the DRP access will be a write access. This bit clears automatically after the access is complete.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint8_t bfValue);


/**
 * \brief When set to 1, the DRP access will be a write access. This bit clears automatically after the access is complete.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint8_t* const bfValue);



/**
 * \brief Gigabit Transceiver Power Good.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelGtpowergood_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief 000: Normal operation; 001: Near-end PCS Loopback; 010: Near-end PMA Loopback; 011: Reserved; 100: Far-end PMA Loopback; 101: Reserved; 110: Far-end PCS Loopback
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelLoopback_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               const uint8_t bfValue);


/**
 * \brief 000: Normal operation; 001: Near-end PCS Loopback; 010: Near-end PMA Loopback; 011: Reserved; 100: Far-end PMA Loopback; 101: Reserved; 110: Far-end PCS Loopback
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelLoopback_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint8_t* const bfValue);



/**
 * \brief Set this bit to a one to enable the 8b10b decoder in the Channel receiver - this is needed for JESD204B operation. Clear this bit to bypass the 8b10b decoder - this is needed for JESD204C operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRx8b10bEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint8_t bfValue);


/**
 * \brief Set this bit to a one to enable the 8b10b decoder in the Channel receiver - this is needed for JESD204B operation. Clear this bit to bypass the 8b10b decoder - this is needed for JESD204C operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRx8b10bEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint8_t* const bfValue);



/**
 * \brief This port is driven High and then deasserted to start the RX elastic buffer reset process. In either single mode or sequential mode, activating RXBUFRESET resets the RX elastic buffer only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxBufReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief This port is driven High and then deasserted to start the RX elastic buffer reset process. In either single mode or sequential mode, activating RXBUFRESET resets the RX elastic buffer only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxBufReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief RX buffer status. The RX elastic buffer underflow or overflow error status are not sticky bits and return to normal condition (000b) if the error clears. When an error condition is detected, an RX elastic buffer reset is recommended. When clock correction is disabled, RXBUFSTATUS codes 001b and 010b can be used by setting RXBUF_ADDR_MODE to FULL. 000: Nominal condition; 001: Number of bytes in the buffer are less than CLK_COR_MIN_LAT; 010: Number of bytes in the buffer are greater than CLK_COR_MAX_LAT; 101: RX elastic buffer underflow; 110: RX elastic buffer overflow
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxBufStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief Gigabit Transceiver Receive CDR locked.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxCdrlock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);



/**
 * \brief When set to 1, this bit field holds the clock data recovery settings in the Channel Receiver front end. When cleared to 0, the clock data recovery is allowed to adapt.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxCdrHold_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                const uint8_t bfValue);


/**
 * \brief When set to 1, this bit field holds the clock data recovery settings in the Channel Receiver front end. When cleared to 0, the clock data recovery is allowed to adapt.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxCdrHold_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock for the receiver parallel interface in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxClkFreqDetect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint32_t* const bfValue);



/**
 * \brief When this bit is high, a 8b10b comma has been detected on the receiver.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxCommaDet_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief Controls a receive channel's rx_dfelpm_en signal. When set to 1, the LPM equalizer is enabled. When cleared to 0, the DFE equalizer is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmEn_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief Controls a receive channel's rx_dfelpm_en signal. When set to 1, the LPM equalizer is enabled. When cleared to 0, the DFE equalizer is enabled.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmEn_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief When set to 1, this bit field holds the equalization settings in the Channel Receiver front end. When cleared to 0, the equalizer is allowed to adapt.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmHold_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);


/**
 * \brief When set to 1, this bit field holds the equalization settings in the Channel Receiver front end. When cleared to 0, the equalizer is allowed to adapt.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmHold_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);



/**
 * \brief When set to 1, this bit field overrides the equalization settings in the Channel Receiver front end with the default values in the DRP registers. When cleared to 0, the equalizer is allowed to adapt.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmOverride_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       const uint8_t bfValue);


/**
 * \brief When set to 1, this bit field overrides the equalization settings in the Channel Receiver front end with the default values in the DRP registers. When cleared to 0, the equalizer is allowed to adapt.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmOverride_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                       uint8_t channelId, 
                                                                                       uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is driven High and then deasserted to start the DFE reset process. In single mode, activating RXDFELPMRESET resets only the RX DFE circuits. In sequential mode, activating RXDFELPMRESET starts the RX reset process as shown in Xilinx UG576 Figure 2-22 from RXDFELPMRESET and followed by EYESCANRESET, RXPCSRESET, and RXBUFRESET. Detailed coverage in sequential mode is listed in Xilinx UG576 Table 2-27.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint8_t bfValue);


/**
 * \brief This bit drives the port that is driven High and then deasserted to start the DFE reset process. In single mode, activating RXDFELPMRESET resets only the RX DFE circuits. In sequential mode, activating RXDFELPMRESET starts the RX reset process as shown in Xilinx UG576 Figure 2-22 from RXDFELPMRESET and followed by EYESCANRESET, RXPCSRESET, and RXBUFRESET. Detailed coverage in sequential mode is listed in Xilinx UG576 Table 2-27.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxDfeLpmReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint8_t* const bfValue);



/**
 * \brief When set to 1, this bit field causes the Channel gearbox to slip one position. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxGearboxSlip_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint8_t bfValue);


/**
 * \brief When set to 1, this bit field causes the Channel gearbox to slip one position. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxGearboxSlip_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint8_t* const bfValue);



/**
 * \brief Selects the source of the output clock for the receive path. 000: reserved; 001: RXOUTCLKPCS; 010: RXOUTCLKPMA; 011: RXPLLREFCLK_DIV1; 100: RXPLLREFCLK_DIV2; 101: RXPROGDIVCLK;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxOutClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Selects the source of the output clock for the receive path. 000: reserved; 001: RXOUTCLKPCS; 010: RXOUTCLKPMA; 011: RXPLLREFCLK_DIV1; 100: RXPLLREFCLK_DIV2; 101: RXPROGDIVCLK;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxOutClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is driven High and then deasserted to start the PCS reset process. In single mode, activating RXPCSRESET resets only the RX PCS circuits. In sequential mode, activating RXPCSRESET starts the RX reset process as shown in Xilinx UG576 Figure 2-22 from RXPCSRESET and followed by RXBUFRESET. Detailed coverage in sequential mode is listed in Xilinx UG576 Table 2-27. In both modes, RXPCSRESET does not start the reset process until RXUSERRDY is High.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPcsReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief This bit drives the port that is driven High and then deasserted to start the PCS reset process. In single mode, activating RXPCSRESET resets only the RX PCS circuits. In sequential mode, activating RXPCSRESET starts the RX reset process as shown in Xilinx UG576 Figure 2-22 from RXPCSRESET and followed by RXBUFRESET. Detailed coverage in sequential mode is listed in Xilinx UG576 Table 2-27. In both modes, RXPCSRESET does not start the reset process until RXUSERRDY is High.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPcsReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief Selects the PLL to drive the receiver data path. 00: CPLL; 01: reserved; 10: QPLL1; 11: QPLL0
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPllClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Selects the PLL to drive the receiver data path. 00: CPLL; 01: reserved; 10: QPLL1; 11: QPLL0
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPllClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is driven High and then deasserted to start RX PMA reset process. In single mode, activating RXPMARESET resets only the RX PMA blocks not including CDR and DFE. In sequential mode, activating RXPMARESET starts the RX reset process as shown in Xilinx UG576 Figure 2-22 from RXPMARESET and followed by RXCDRPHASERESET, RXCDRFREQRESET, RXDFELPMRESET, EYESCANRESET, RXPCSRESET, and RXBUFRESET. Detailed coverage on sequential mode is listed in Xilinx UG576 Table 2-27.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPmaReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief This bit drives the port that is driven High and then deasserted to start RX PMA reset process. In single mode, activating RXPMARESET resets only the RX PMA blocks not including CDR and DFE. In sequential mode, activating RXPMARESET starts the RX reset process as shown in Xilinx UG576 Figure 2-22 from RXPMARESET and followed by RXCDRPHASERESET, RXCDRFREQRESET, RXDFELPMRESET, EYESCANRESET, RXPCSRESET, and RXBUFRESET. Detailed coverage on sequential mode is listed in Xilinx UG576 Table 2-27.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPmaReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief This active-High signal indicates RX PMA reset is complete. This port is driven Low when GTRXRESET or RXPMARESET is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPmaResetDone_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint8_t* const bfValue);



/**
 * \brief Controls a receive channel's polarity. When set to 1, the corresponding receive channel is inverted. When cleared to 0, the channel is not inverted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief Controls a receive channel's polarity. When set to 1, the corresponding receive channel is inverted. When cleared to 0, the channel is not inverted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief Powers down the RX channel according to the PCI Express PIPE protocol encoding. 00: P0 (normal operation); 01: P0s (low recovery time power down); 10: P1 (longer recovery time); 11: P2 (lowest power state)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPowerdown_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Powers down the RX channel according to the PCI Express PIPE protocol encoding. 00: P0 (normal operation); 01: P0s (low recovery time power down); 10: P1 (longer recovery time); 11: P2 (lowest power state)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPowerdown_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This field contains the number of PRBS errors found in the sequence. Any error report from the PRBS checker increments the counter. The counter saturates at 'hFFFFFFFF.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsErrCnt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint32_t* const bfValue);



/**
 * \brief Resets the PRBS error counter. A reset should be performed after enabling the PRBS checker and before checking the error count. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsErrCntRst_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      const uint8_t bfValue);


/**
 * \brief Resets the PRBS error counter. A reset should be performed after enabling the PRBS checker and before checking the error count. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsErrCntRst_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint8_t* const bfValue);



/**
 * \brief When 1, PRBS detector is locked to a valid PRBS pattern.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsLocked_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);



/**
 * \brief 0000: Standard operation mode. (PRBS check is off); 0001: PRBS-7; 0010: PRBS-9; 0011: PRBS-15; 0100: PRBS-23; 0101: PRBS-31
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                const uint8_t bfValue);


/**
 * \brief 0000: Standard operation mode. (PRBS check is off); 0001: PRBS-7; 0010: PRBS-9; 0011: PRBS-15; 0100: PRBS-23; 0101: PRBS-31
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxPrbsSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is used to reset the RX Programmable Divider. It is driven High and then low again whenever the input clock to the programmable divider is interrupted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxProgdivReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint8_t bfValue);


/**
 * \brief This bit drives the port that is used to reset the RX Programmable Divider. It is driven High and then low again whenever the input clock to the programmable divider is interrupted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxProgdivReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint8_t* const bfValue);



/**
 * \brief This active-High signal indicates the GTH transceiver RX Programmable Divider has finished reset and the output clock is stable. This port is driven Low when RXPROGDIVRESET goes High.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxProgdivResetDone_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                         uint8_t channelId, 
                                                                                         uint8_t* const bfValue);



/**
 * \brief This bit field sets the final divider for the Channel receiver. The settings are: 000: Use RXOUT_DIV attributes; 001: Divide by 1; 010: Divide by 2; 011: Divide by 4; 100: Divide by 8; 101: Divide by 16; 110: Divide by 1; 111: Divide by 1
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxRate_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);


/**
 * \brief This bit field sets the final divider for the Channel receiver. The settings are: 000: Use RXOUT_DIV attributes; 001: Divide by 1; 010: Divide by 2; 011: Divide by 4; 100: Divide by 8; 101: Divide by 16; 110: Divide by 1; 111: Divide by 1
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxRate_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is driven High and then deasserted to start the full channel RX reset sequence.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);


/**
 * \brief This bit drives the port that is driven High and then deasserted to start the full channel RX reset sequence.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);



/**
 * \brief When asserted, this active-High signal indicates the GTH transceiver RX has finished reset and is ready for use. In sequential mode, this port is driven Low when GTRXRESET is driven High. This signal is not driven High until RXUSERRDY goes High. In single mode, this port is driven Low when any of the RX resets are asserted. This signal is not asserted until all RX resets are deasserted and RXUSERRDY is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelRxResetDone_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief Set this bit to a one to enable the 8b10b encoder in the Channel transmiter - this is needed for JESD204B operation. Clear this bit to bypass the 8b10b encoder - this is needed for JESD204C operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTx8b10bEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint8_t bfValue);


/**
 * \brief Set this bit to a one to enable the 8b10b encoder in the Channel transmiter - this is needed for JESD204B operation. Clear this bit to bypass the 8b10b encoder - this is needed for JESD204C operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTx8b10bEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint8_t* const bfValue);



/**
 * \brief TX buffer status. TXBUFSTATUS[1]: TX buffer overflow or underflow status. When TXBUFSTATUS[1] is set High, it remains High until the TX buffer is reset. 1: TX FIFO has overflow or underflow. 0: No TX FIFO overflow or underflow error. TXBUFSTATUS[0]: TX buffer fullness. 1: TX FIFO is at least half full; 0: TX FIFO is less than half full.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxBufStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock for the transmitter parallel interface in Hz.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxClkFreqDetect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint32_t* const bfValue);



/**
 * \brief Driver Swing Control. The default is user specified. All listed values are in mVPPD.
 *        [4:0]: mVPPD;   
 *        'h00: 390;   
 *        'h01: 430;   
 *        'h02: 450;   
 *        'h03: 470;   
 *        'h04: 490;   
 *        'h05: 530;   
 *        'h06: 550;   
 *        'h07: 570;   
 *        'h08: 590;   
 *        'h09: 630;   
 *        'h0A: 650;   
 *        'h0B: 670;   
 *        'h0C: 690;   
 *        'h0D: 730;   
 *        'h0E: 740;   
 *        'h0F: 760;   
 *        'h10: 780;   
 *        'h11: 820;   
 *        'h12: 840;   
 *        'h13: 850;   
 *        'h14: 870;   
 *        'h15: 900;   
 *        'h16: 920;   
 *        'h17: 930;   
 *        'h18: 950;   
 *        'h19: 970;   
 *        'h1A: 980;   
 *        'h1B: 990;   
 *        'h1C: 1000;   
 *        'h1D: 1020;   
 *        'h1E: 1030;   
 *        'h1F: 1040;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxDiffControl_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint8_t bfValue);


/**
 * \brief Driver Swing Control. The default is user specified. All listed values are in mVPPD.
 *        [4:0]: mVPPD;   
 *        'h00: 390;   
 *        'h01: 430;   
 *        'h02: 450;   
 *        'h03: 470;   
 *        'h04: 490;   
 *        'h05: 530;   
 *        'h06: 550;   
 *        'h07: 570;   
 *        'h08: 590;   
 *        'h09: 630;   
 *        'h0A: 650;   
 *        'h0B: 670;   
 *        'h0C: 690;   
 *        'h0D: 730;   
 *        'h0E: 740;   
 *        'h0F: 760;   
 *        'h10: 780;   
 *        'h11: 820;   
 *        'h12: 840;   
 *        'h13: 850;   
 *        'h14: 870;   
 *        'h15: 900;   
 *        'h16: 920;   
 *        'h17: 930;   
 *        'h18: 950;   
 *        'h19: 970;   
 *        'h1A: 980;   
 *        'h1B: 990;   
 *        'h1C: 1000;   
 *        'h1D: 1020;   
 *        'h1E: 1030;   
 *        'h1F: 1040;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxDiffControl_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint8_t* const bfValue);



/**
 * \brief Selects the source of the output clock for the transmit path. 000: reserved; 001: TXOUTCLKPCS; 010: TXOUTCLKPMA; 011: TXPLLREFCLK_DIV1; 100: TXPLLREFCLK_DIV2; 101: TXPROGDIVCLK;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxOutClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Selects the source of the output clock for the transmit path. 000: reserved; 001: TXOUTCLKPCS; 010: TXOUTCLKPMA; 011: TXPLLREFCLK_DIV1; 100: TXPLLREFCLK_DIV2; 101: TXPROGDIVCLK;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxOutClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is used to reset the TX PCS. It is driven High and then deasserted to start the PCS reset process. In sequential mode, activating this port only resets the TX PCS.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPcsReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief This bit drives the port that is used to reset the TX PCS. It is driven High and then deasserted to start the PCS reset process. In sequential mode, activating this port only resets the TX PCS.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPcsReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief Selects the PLL to drive the transmitter data path. 00: CPLL; 01: reserved; 10: QPLL1; 11: QPLL0
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPllClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Selects the PLL to drive the transmitter data path. 00: CPLL; 01: reserved; 10: QPLL1; 11: QPLL0
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPllClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is used to reset the TX PMA. It is driven High and then deasserted to start the TX PMA reset process. In sequential mode, activating this port resets both the TX PMA and the TX PCS.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPmaReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief This bit drives the port that is used to reset the TX PMA. It is driven High and then deasserted to start the TX PMA reset process. In sequential mode, activating this port resets both the TX PMA and the TX PCS.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPmaReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief This active-high signal indicates TX PMA reset is complete. This port is driven Low when GTTXRESET or TXPMARESET is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPmaResetDone_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint8_t* const bfValue);



/**
 * \brief Controls a transmit channel's polarity. When set to 1, the corresponding transmit channel is inverted. When cleared to 0, the channel is not inverted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief Controls a transmit channel's polarity. When set to 1, the corresponding transmit channel is inverted. When cleared to 0, the channel is not inverted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief Transmitter post-cursor TX pre-emphasis control. The default is user specified. All listed values (dB) are typical. 
 *        [4:0] 	Emphasis (dB) 	|Coefficient Units| 
 *        'h00: 0.00  |0|;   
 *        'h01: 0.20  |1|;   
 *        'h02: 0.32  |2|;   
 *        'h03: 0.74  |3|;   
 *        'h04: 0.82  |4|;   
 *        'h05: 1.19  |5|;   
 *        'h06: 1.32  |6|;   
 *        'h07: 1.77  |7|;   
 *        'h08: 1.91  |8|;   
 *        'h09: 2.28  |9|;   
 *        'h0A: 2.32  |10|;   
 *        'h0B: 2.98  |11|;   
 *        'h0C: 3.14  |12|;   
 *        'h0D: 3.62  |13|;   
 *        'h0E: 3.73  |14|;   
 *        'h0F: 3.99  |15|;   
 *        'h10: 4.16  |16|;   
 *        'h11: 4.76  |17|;   
 *        'h12: 5.05  |18|;   
 *        'h13: 5.57  |19|;   
 *        'h14: 5.81  |20|;   
 *        'h15: 6.69  |21|;   
 *        'h16: 7.07  |22|;   
 *        'h17: 7.43  |23|;   
 *        'h18: 7.84  |24|;   
 *        'h19: 8.77  |25|;   
 *        'h1A: 9.14  |26|;   
 *        'h1B: 10.08  |27|;   
 *        'h1C: 10.51  |28|;   
 *        'h1D: 11.63  |29|;   
 *        'h1E: 12.29  |30|;   
 *        'h1F: 13.67  |31|;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPostcursor_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);


/**
 * \brief Transmitter post-cursor TX pre-emphasis control. The default is user specified. All listed values (dB) are typical. 
 *        [4:0] 	Emphasis (dB) 	|Coefficient Units| 
 *        'h00: 0.00  |0|;   
 *        'h01: 0.20  |1|;   
 *        'h02: 0.32  |2|;   
 *        'h03: 0.74  |3|;   
 *        'h04: 0.82  |4|;   
 *        'h05: 1.19  |5|;   
 *        'h06: 1.32  |6|;   
 *        'h07: 1.77  |7|;   
 *        'h08: 1.91  |8|;   
 *        'h09: 2.28  |9|;   
 *        'h0A: 2.32  |10|;   
 *        'h0B: 2.98  |11|;   
 *        'h0C: 3.14  |12|;   
 *        'h0D: 3.62  |13|;   
 *        'h0E: 3.73  |14|;   
 *        'h0F: 3.99  |15|;   
 *        'h10: 4.16  |16|;   
 *        'h11: 4.76  |17|;   
 *        'h12: 5.05  |18|;   
 *        'h13: 5.57  |19|;   
 *        'h14: 5.81  |20|;   
 *        'h15: 6.69  |21|;   
 *        'h16: 7.07  |22|;   
 *        'h17: 7.43  |23|;   
 *        'h18: 7.84  |24|;   
 *        'h19: 8.77  |25|;   
 *        'h1A: 9.14  |26|;   
 *        'h1B: 10.08  |27|;   
 *        'h1C: 10.51  |28|;   
 *        'h1D: 11.63  |29|;   
 *        'h1E: 12.29  |30|;   
 *        'h1F: 13.67  |31|;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPostcursor_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);



/**
 * \brief Powers down the TX channel according to the PCI Express PIPE protocol encoding. 00: P0 (normal operation); 01: P0s (low recovery time power down); 10: P1 (longer recovery time); 11: P2 (lowest power state)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPowerdown_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Powers down the TX channel according to the PCI Express PIPE protocol encoding. 00: P0 (normal operation); 01: P0s (low recovery time power down); 10: P1 (longer recovery time); 11: P2 (lowest power state)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPowerdown_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief When set to 1, this bit causes errors in the PRBS sequence. The bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPrbsInjErr_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);


/**
 * \brief When set to 1, this bit causes errors in the PRBS sequence. The bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPrbsInjErr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);



/**
 * \brief 0000: Standard operation mode (test pattern generation is off); 0001: PRBS-7; 0010: PRBS-9; 0011: PRBS-15; 0100: PRBS-23; 0101: PRBS-31; 1000: PCI Express compliance pattern. Only works with internal data width 20 bit and 40 bit modes; 1001: Square wave with 2 UI (alternating 0s/1s); 1010: Square wave with 16 UI, 20 UI, 32 UI, or 40 UI period (based on internal data width)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPrbsSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                const uint8_t bfValue);


/**
 * \brief 0000: Standard operation mode (test pattern generation is off); 0001: PRBS-7; 0010: PRBS-9; 0011: PRBS-15; 0100: PRBS-23; 0101: PRBS-31; 1000: PCI Express compliance pattern. Only works with internal data width 20 bit and 40 bit modes; 1001: Square wave with 2 UI (alternating 0s/1s); 1010: Square wave with 16 UI, 20 UI, 32 UI, or 40 UI period (based on internal data width)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPrbsSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint8_t* const bfValue);



/**
 * \brief Transmitter pre-cursor TX pre-emphasis control. The default is user specified. All listed values (dB) are typical. 
 *        [4:0]: Emphasis (dB)  |Coefficient Units|;
 *        'h00:  0.01  |0|;   
 *        'h01:  0.20  |1|;   
 *        'h02:  0.32  |2|;   
 *        'h03:  0.73  |3|;   
 *        'h04:  0.81  |4|;   
 *        'h05:  1.17  |5|;   
 *        'h06:  1.30  |6|;   
 *        'h07:  1.74  |7|;   
 *        'h08:  1.87  |8|;   
 *        'h09:  2.24  |9|;   
 *        'h0A:  2.28  |10|;   
 *        'h0B:  2.93  |11|;   
 *        'h0C:  3.08  |12|;   
 *        'h0D:  3.55  |13|;   
 *        'h0E:  3.66  |14|;   
 *        'h0F:  3.90  |15|;   
 *        'h10:  4.07  |16|;   
 *        'h11:  4.66  |17|;   
 *        'h12:  4.94  |18|;   
 *        'h13:  5.44  |19|;   
 *        'h14:  5.67  |20|;   
 *        'h15:  5.67  |20|;   
 *        'h16:  5.67  |20|;   
 *        'h17:  5.67  |20|;   
 *        'h18:  5.67  |20|;   
 *        'h19:  5.67  |20|;   
 *        'h1A:  5.67  |20|;   
 *        'h1B:  5.67  |20|;   
 *        'h1C:  5.67  |20|;   
 *        'h1D:  5.67  |20|;   
 *        'h1E:  5.67  |20|;   
 *        'h1F:  5.67  |20|;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPrecursor_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Transmitter pre-cursor TX pre-emphasis control. The default is user specified. All listed values (dB) are typical. 
 *        [4:0]: Emphasis (dB)  |Coefficient Units|;
 *        'h00:  0.01  |0|;   
 *        'h01:  0.20  |1|;   
 *        'h02:  0.32  |2|;   
 *        'h03:  0.73  |3|;   
 *        'h04:  0.81  |4|;   
 *        'h05:  1.17  |5|;   
 *        'h06:  1.30  |6|;   
 *        'h07:  1.74  |7|;   
 *        'h08:  1.87  |8|;   
 *        'h09:  2.24  |9|;   
 *        'h0A:  2.28  |10|;   
 *        'h0B:  2.93  |11|;   
 *        'h0C:  3.08  |12|;   
 *        'h0D:  3.55  |13|;   
 *        'h0E:  3.66  |14|;   
 *        'h0F:  3.90  |15|;   
 *        'h10:  4.07  |16|;   
 *        'h11:  4.66  |17|;   
 *        'h12:  4.94  |18|;   
 *        'h13:  5.44  |19|;   
 *        'h14:  5.67  |20|;   
 *        'h15:  5.67  |20|;   
 *        'h16:  5.67  |20|;   
 *        'h17:  5.67  |20|;   
 *        'h18:  5.67  |20|;   
 *        'h19:  5.67  |20|;   
 *        'h1A:  5.67  |20|;   
 *        'h1B:  5.67  |20|;   
 *        'h1C:  5.67  |20|;   
 *        'h1D:  5.67  |20|;   
 *        'h1E:  5.67  |20|;   
 *        'h1F:  5.67  |20|;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxPrecursor_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is used to reset the TX Programmable Divider. It is driven High and then low again whenever the input clock to the programmable divider is interrupted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxProgdivReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint8_t bfValue);


/**
 * \brief This bit drives the port that is used to reset the TX Programmable Divider. It is driven High and then low again whenever the input clock to the programmable divider is interrupted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxProgdivReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint8_t* const bfValue);



/**
 * \brief This active-High signal indicates the GTH transceiver TX Programmable Divider has finished reset and the output clock is stable. This port is driven Low when TXPROGDIVRESET goes High.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxProgdivResetDone_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                         uint8_t channelId, 
                                                                                         uint8_t* const bfValue);



/**
 * \brief This bit drives the port that is driven High and then deasserted to start the full TX reset sequence. The time required for the reset sequence is to be determined.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);


/**
 * \brief This bit drives the port that is driven High and then deasserted to start the full TX reset sequence. The time required for the reset sequence is to be determined.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);



/**
 * \brief This active-High signal indicates the GTH transceiver TX has finished reset and is ready for use. This port is driven Low when GTTXRESET goes High and is not driven High until the GTH transceiver TX detects TXUSERRDY High.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_ChannelTxResetDone_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief Identifies the lane on which the scan is to be performed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanChannel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);


/**
 * \brief Identifies the lane on which the scan is to be performed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanChannel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



/**
 * \brief When set to a 1, the Eye Scan State Machine performs scan on the receive channel identified in eye_scan_channel and stores the results in a RAM accessible through the result* bitfields.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief When set to a 1, the Eye Scan State Machine performs scan on the receive channel identified in eye_scan_channel and stores the results in a RAM accessible through the result* bitfields.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief Set to a 0 when the lane rate is less than or equal to 10Gbps; otherwise set to 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanLaneRateAbove10g_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);


/**
 * \brief Set to a 0 when the lane rate is less than or equal to 10Gbps; otherwise set to 1.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanLaneRateAbove10g_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);



/**
 * \brief The prescaler of the number of bits sampled. Total number of bits sample is up to 2 * 65535 * 2**(prescale+1) * data width.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanPrescale_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);


/**
 * \brief The prescaler of the number of bits sampled. Total number of bits sample is up to 2 * 65535 * 2**(prescale+1) * data width.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanPrescale_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief Write a value sets the address of the RAM that stores the results of the eye scan. The address will automatically increment as data is read. So, this field only needs to be written at the beginning of exfilling the data.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanResultAddr_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 const uint16_t bfValue);


/**
 * \brief Write a value sets the address of the RAM that stores the results of the eye scan. The address will automatically increment as data is read. So, this field only needs to be written at the beginning of exfilling the data.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanResultAddr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint16_t* const bfValue);



/**
 * \brief The test concludes when either the error counter or sample counter reaches 65535. So, only the value that is not 65535 is stored in the results RAM. The results are organized as: [15:0] sample/error counter; [16] counter is sample (1) or error (0); [21:17] prescale
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanResultData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint32_t* const bfValue);



/**
 * \brief When 1, indicates that the scan is running.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusActive_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



/**
 * \brief When 1, indicates that the scan is complete. Clear by writing enable to 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusDone_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief When 1, indicates that an error occured. Clear by writing enable to 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t* const bfValue);



/**
 * \brief Contains the current horizontal offset value.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusHOffset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue);



/**
 * \brief Current PC value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusPc_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief Contains the current prescale value.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusPrescale_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);



/**
 * \brief Current return_PC value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusReturnPc_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                     uint8_t* const bfValue);



/**
 * \brief Contains the current vertical offset value.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_EyeScanStatusVOffset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint16_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief This field is the address of the DRP register being accessed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpAddress_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint16_t bfValue);


/**
 * \brief This field is the address of the DRP register being accessed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpAddress_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint16_t* const bfValue);



/**
 * \brief When set to 1, the DRP access, read or write, is initiated. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             const uint8_t bfValue);


/**
 * \brief When set to 1, the DRP access, read or write, is initiated. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief If a timeout occurs on the DRP access, this bit will be set to 1. In the case of an error, the DRP must be reset using the drprst bit. This bit is clear on read.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);



/**
 * \brief When the DRP access is complete, this bit will be set to 1. The bit will clear on read.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpReady_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                            uint8_t channelId, 
                                                                            uint8_t* const bfValue);



/**
 * \brief When a read access is performed the data read from the DRP register is placed in this field.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpReadData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t channelId, 
                                                                               uint16_t* const bfValue);



/**
 * \brief The number of DRP clock cycle to wait for DRP Ready to go high before determining if there is an error condition.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpTimeout_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint32_t bfValue);


/**
 * \brief The number of DRP clock cycle to wait for DRP Ready to go high before determining if there is an error condition.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpTimeout_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint32_t* const bfValue);



/**
 * \brief This field is the data that will be written to the DRP register in a write access.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteData_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                const uint16_t bfValue);


/**
 * \brief This field is the data that will be written to the DRP register in a write access.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                uint8_t channelId, 
                                                                                uint16_t* const bfValue);



/**
 * \brief This field is used to mask which bits of the write data that will be written to the DRP register. Each data bit that corresponds to a mask bit that is set to 1 will be written to the DRP register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteDataMask_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint16_t bfValue);


/**
 * \brief This field is used to mask which bits of the write data that will be written to the DRP register. Each data bit that corresponds to a mask bit that is set to 1 will be written to the DRP register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteDataMask_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint16_t* const bfValue);



/**
 * \brief When set to 1, the DRP access will be a write access. This bit clears automatically after the access is complete.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief When set to 1, the DRP access will be a write access. This bit clears automatically after the access is complete.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This bit indicates that the Quad PLL 0 is locked.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll0Lock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief When set to 1, the Quad PLL 0 is powered down. When cleared to 0, the Quad PLL 0 powers up.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll0Pd_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           const uint8_t bfValue);


/**
 * \brief When set to 1, the Quad PLL 0 is powered down. When cleared to 0, the Quad PLL 0 powers up.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll0Pd_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);



/**
 * \brief Selects the input reference clock to the Quad PLL 0. Set this input to 3'b001 when only one clock source is connected to the Quad PLL reference clock selection multiplexer. Reset must be applied to the Quad PLL after changing the reference clock input. 000: Reserved; 001: GTREFCLK00 selected; 010: GTREFCLK10 selected; 011: GTNORTHREFCLK00 selected; 100: GTNORTHREFCLK10 selected; 101: GTSOUTHREFCLK00 selected; 110: GTSOUTHREFCLK10 selected; 111: GTGREFCLK0 selected
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll0RefClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Selects the input reference clock to the Quad PLL 0. Set this input to 3'b001 when only one clock source is connected to the Quad PLL reference clock selection multiplexer. Reset must be applied to the Quad PLL after changing the reference clock input. 000: Reserved; 001: GTREFCLK00 selected; 010: GTREFCLK10 selected; 011: GTNORTHREFCLK00 selected; 100: GTNORTHREFCLK10 selected; 101: GTSOUTHREFCLK00 selected; 110: GTSOUTHREFCLK10 selected; 111: GTGREFCLK0 selected
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll0RefClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief Writing this bit to a 1 resets the Quad PLL. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll0Reset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);


/**
 * \brief Writing this bit to a 1 resets the Quad PLL. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll0Reset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);



/**
 * \brief This bit indicates that the Quad PLL 1 is locked.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll1Lock_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                             uint8_t channelId, 
                                                                             uint8_t* const bfValue);



/**
 * \brief When set to 1, the Quad PLL 1 is powered down. When cleared to 0, the Quad PLL 1 powers up.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll1Pd_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           const uint8_t bfValue);


/**
 * \brief When set to 1, the Quad PLL 1 is powered down. When cleared to 0, the Quad PLL 1 powers up.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll1Pd_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                           uint8_t channelId, 
                                                                           uint8_t* const bfValue);



/**
 * \brief Selects the input reference clock to the Quad PLL 1. Set this input to 3'b001 when only one clock source is connected to the Quad PLL reference clock selection multiplexer. Reset must be applied to the Quad PLL after changing the reference clock input. 000: Reserved; 001: GTREFCLK01 selected; 010: GTREFCLK11 selected; 011: GTNORTHREFCLK01 selected; 100: GTNORTHREFCLK11 selected; 101: GTSOUTHREFCLK01 selected; 110: GTSOUTHREFCLK11 selected; 111: GTGREFCLK1 selected
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll1RefClkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief Selects the input reference clock to the Quad PLL 1. Set this input to 3'b001 when only one clock source is connected to the Quad PLL reference clock selection multiplexer. Reset must be applied to the Quad PLL after changing the reference clock input. 000: Reserved; 001: GTREFCLK01 selected; 010: GTREFCLK11 selected; 011: GTNORTHREFCLK01 selected; 100: GTNORTHREFCLK11 selected; 101: GTSOUTHREFCLK01 selected; 110: GTSOUTHREFCLK11 selected; 111: GTGREFCLK1 selected
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll1RefClkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief Writing this bit to a 1 resets the Quad 1 PLL. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll1Reset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              const uint8_t bfValue);


/**
 * \brief Writing this bit to a 1 resets the Quad 1 PLL. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_QuadQpll1Reset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                              uint8_t channelId, 
                                                                              uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                       const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);



/**
 * \brief This bitfield sets the polarity of the SYNCB 0 (FPGA to Koror) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb0F2pPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);


/**
 * \brief This bitfield sets the polarity of the SYNCB 0 (FPGA to Koror) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb0F2pPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief This bit field selects the source of the SYNCB 0 signal (FPGA to Koror): RX Deframer 0 SYNCOUTB; 1: RX Deframer 1 SYNCOUTB; 2: RX Deframer 2 SYNCOUTB; 3: SYNCB 0 (Koror to FPGA) Pin; 4: SYNCB 1 (Koror to FPGA) Pin
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb0F2pSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);


/**
 * \brief This bit field selects the source of the SYNCB 0 signal (FPGA to Koror): RX Deframer 0 SYNCOUTB; 1: RX Deframer 1 SYNCOUTB; 2: RX Deframer 2 SYNCOUTB; 3: SYNCB 0 (Koror to FPGA) Pin; 4: SYNCB 1 (Koror to FPGA) Pin
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb0F2pSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief This bitfield sets the polarity of the SYNCB 0 (Koror to FPGA) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb0P2fPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);


/**
 * \brief This bitfield sets the polarity of the SYNCB 0 (Koror to FPGA) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb0P2fPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief This bitfield sets the polarity of the SYNCB 1 (FPGA to Koror) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb1F2pPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);


/**
 * \brief This bitfield sets the polarity of the SYNCB 1 (FPGA to Koror) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb1F2pPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief This bit field selects the source of the signal that is transmitted on the SYNCB 1 (FPGA to Koror) pin. 0: RX Deframer 0 SYNCOUTB; 1: RX Deframer 1 SYNCOUTB; 2: RX Deframer 2 SYNCOUTB; 3: SYNCB 0 (Koror to FPGA) Pin; 4: SYNCB 1 (Koror to FPGA) Pin
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb1F2pSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);


/**
 * \brief This bit field selects the source of the signal that is transmitted on the SYNCB 1 (FPGA to Koror) pin. 0: RX Deframer 0 SYNCOUTB; 1: RX Deframer 1 SYNCOUTB; 2: RX Deframer 2 SYNCOUTB; 3: SYNCB 0 (Koror to FPGA) Pin; 4: SYNCB 1 (Koror to FPGA) Pin
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb1F2pSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief This bitfield sets the polarity of the SYNCB 1 (Koror to FPGA) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb1P2fPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);


/**
 * \brief This bitfield sets the polarity of the SYNCB 1 (Koror to FPGA) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb1P2fPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief This bitfield sets the polarity of the SYNCB 2 (FPGA to Koror) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb2F2pPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);


/**
 * \brief This bitfield sets the polarity of the SYNCB 2 (FPGA to Koror) pins.  Write a 1 to invert, otherwise write 0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb2F2pPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief This bit field selects the source of the signal that is transmitted on the SYNCB 2 (FPGA to Koror) pin. 0: RX Deframer 0 SYNCOUTB; 1: RX Deframer 1 SYNCOUTB; 2: RX Deframer 2 SYNCOUTB; 3: SYNCB 0 (Koror to FPGA) Pin; 4: SYNCB 1 (Koror to FPGA) Pin
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb2F2pSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);


/**
 * \brief This bit field selects the source of the signal that is transmitted on the SYNCB 2 (FPGA to Koror) pin. 0: RX Deframer 0 SYNCOUTB; 1: RX Deframer 1 SYNCOUTB; 2: RX Deframer 2 SYNCOUTB; 3: SYNCB 0 (Koror to FPGA) Pin; 4: SYNCB 1 (Koror to FPGA) Pin
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Syncb2F2pSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6PhyChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Phy_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_ADRV904X_PHY_H_

