/** 
 * \file fpgagen6_bf_adrv904x_system_timing_control.h Automatically generated file with generator ver
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

#ifndef _FPGAGEN6_BF_ADRV904X_SYSTEM_TIMING_CONTROL_H_
#define _FPGAGEN6_BF_ADRV904X_SYSTEM_TIMING_CONTROL_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_adrv904x_system_timing_control_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



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
                                                                                        const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Ad9528Reset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);



/**
 * \brief No documentation provided
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                    uint8_t* const bfValue);



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
                                                                                              const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                              const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                              uint8_t* const bfValue);



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
                                                                                             const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefRequest_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             uint8_t* const bfValue);



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
                                                                                                const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_ExtSysrefSampleEdge_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                                const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                                uint8_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock to Receive Link 0 in Hz.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink0Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock to Receive Link 1 in Hz.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink1Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock to Receive Link 2 in Hz.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqRxLink2Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock to SPI Master Alternate Base Clock in Hz.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqSpiAltBaseClk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                              const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                              uint32_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock to TDD State Machine in Hz.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqTddClk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                       uint32_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock to Transmit Link 0 in Hz.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqTxLink0Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue);



/**
 * \brief This field contains the frequency of the clock to Transmit Link 1 in Hz.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_FreqTxLink1Clk_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint32_t* const bfValue);



/**
 * \brief No documentation provided
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);



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
                                                                                            const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                            const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                            uint8_t* const bfValue);



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
                                                                                             const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefFreerun_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             uint8_t* const bfValue);



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
                                                                                               const uint16_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefGenPeriod_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                               const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                               uint16_t* const bfValue);



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
                                                                                             const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_IntSysrefRequest_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                             const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                             uint8_t* const bfValue);



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
                                                                                         const uint8_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_RefclkSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                         const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                         uint8_t* const bfValue);



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
                                                                                    const uint32_t bfValue);


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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue);



/**
 * \brief When this bit is 1, a valid external SYSREF was received.  This bit is self clearing on read.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_SysrefReceived_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue);



/**
 * \brief The Minor Version indicates that a change has been made to the design to modify or add a feature.  Minor Version changes will be backward compatible with previous software releases.
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
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6SystemTimingControl_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6SystemTimingControlChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_ADRV904X_SYSTEM_TIMING_CONTROL_H_

