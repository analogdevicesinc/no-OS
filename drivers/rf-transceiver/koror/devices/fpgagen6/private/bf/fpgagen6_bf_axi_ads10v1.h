/** 
 * \file fpgagen6_bf_axi_ads10v1.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_AXI_ADS10V1_H_
#define _FPGAGEN6_BF_AXI_ADS10V1_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_ads10v1_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief Write a 1 to put the ADS10v1 AD9528 in reset. Write a 0 to clear the reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10Ad9528Reset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);


/**
 * \brief Write a 1 to put the ADS10v1 AD9528 in reset. Write a 0 to clear the reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10Ad9528Reset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



/**
 * \brief Read for the current state of the DIP switches on the ADS10v1 board
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10DipSwitch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief Write a 1 to source from FMC; Write a 0 to source from AD9528; [0]: Selects clock source for GTY228 (DP0 - DP3); [1]: Selects clock source for GTY229 (DP4 - DP71); [2]: Selects clock source for GTY230 (DP8 - DP11); [3]: Selects clock source for GTY231 (DP12 - DP15); [4]: Selects clock source for GTY226 (DP16 - DP19); [5]: Selects clock source for GTY227 (DP20 - DP23);
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10MgtRefclkSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief Write a 1 to source from FMC; Write a 0 to source from AD9528; [0]: Selects clock source for GTY228 (DP0 - DP3); [1]: Selects clock source for GTY229 (DP4 - DP71); [2]: Selects clock source for GTY230 (DP8 - DP11); [3]: Selects clock source for GTY231 (DP12 - DP15); [4]: Selects clock source for GTY226 (DP16 - DP19); [5]: Selects clock source for GTY227 (DP20 - DP23);
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10MgtRefclkSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief Read for the current state of the Pushbutton switches on the ADS10v1 board
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10PbSwitch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



/**
 * \brief Write a 1 to put the ADS10v1 PCA9545 in reset. Write a 0 to clear the reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10Pca9545Reset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief Write a 1 to put the ADS10v1 PCA9545 in reset. Write a 0 to clear the reset.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Ads10Pca9545Reset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief Indicates which SMA input signal is connected to a Desination. Refer to the User Guide for the identity of each Destination.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_DestSmaSel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint8_t bfValue);


/**
 * \brief Indicates which SMA input signal is connected to a Desination. Refer to the User Guide for the identity of each Destination.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_DestSmaSel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      uint8_t* const bfValue);



/**
 * \brief Write a 1 to issue a global reset to the entire FPGA register space. This will return all of the registers to their default value. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_GlobalReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);


/**
 * \brief Write a 1 to issue a global reset to the entire FPGA register space. This will return all of the registers to their default value. This bit is self clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_GlobalReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief All IP cores allow (if applicable) multiple instantiations in master-slave configuration. The master instance identifier is 0x0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);



/**
 * \brief Duration in milliseconds; on time = off time = (N+1) x 1ms
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedBlinkDuration_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                            const uint16_t bfValue);


/**
 * \brief Duration in milliseconds; on time = off time = (N+1) x 1ms
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedBlinkDuration_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                            uint16_t* const bfValue);



/**
 * \brief The current value of the LEDs.  The timing is not constrained on this path.  So, if the signal is static, the read value should be accurate.  If the signal is dynamic, it is hit or miss.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedCurrentValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief Any non-zero value will delay the change to the mode defined in the led_mode bit field. The delay is in milliseconds. A 16 bit number provides over 1 minute worth of delay.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedDelayToModeChange_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                                const uint16_t bfValue);


/**
 * \brief Any non-zero value will delay the change to the mode defined in the led_mode bit field. The delay is in milliseconds. A 16 bit number provides over 1 minute worth of delay.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedDelayToModeChange_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                                uint16_t* const bfValue);



/**
 * \brief Each bit masks the corresponding LED. Write a 1 to enable the LED output. Write a 0 to disable the LED output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedMask_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);


/**
 * \brief Each bit masks the corresponding LED. Write a 1 to enable the LED output. Write a 0 to disable the LED output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedMask_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief 0: Normal operation - LEDs are controlled by module inputs; 1: Running pattern (KITT); 2: Blink; 3: SW defined
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedMode_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);


/**
 * \brief 0: Normal operation - LEDs are controlled by module inputs; 1: Running pattern (KITT); 2: Blink; 3: SW defined
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedMode_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief 0: Blinks continuously; 1: Blinks once; ... 15: Blinks 15 times;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedNumBlinks_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);


/**
 * \brief 0: Blinks continuously; 1: Blinks once; ... 15: Blinks 15 times;
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedNumBlinks_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief When the led_mode is set to SW defined, the value written here is driven on the LEDs.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedSwDefinedValue_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief When the led_mode is set to SW defined, the value written here is driven on the LEDs.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_LedSwDefinedValue_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief This read only field indicates the number of destinations in the FPGA that any of the signals from the SMAs may be directed.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_NumExtTrigDests_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                           uint32_t* const bfValue);



/**
 * \brief This read only field indicates the number of source signals that the module has been configured to allow to each SMA pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_NumExtTrigSrcs_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                          uint32_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                   const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                   uint32_t* const bfValue);



/**
 * \brief Sets the direction of the SMA. 1: Input; 0: Output
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_SmaDir_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  const uint8_t bfValue);


/**
 * \brief Sets the direction of the SMA. 1: Input; 0: Output
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_SmaDir_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint8_t* const bfValue);



/**
 * \brief Sets the source of the signal to be driven on the SMA when it is configured as an output. Refer to the User Guide for the valid values.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_SmaSrc_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  const uint8_t bfValue);


/**
 * \brief Sets the source of the signal to be driven on the SMA when it is configured as an output. Refer to the User Guide for the valid values.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_SmaSrc_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Timer_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                 const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Timer_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                 uint32_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAds10v1ChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAds10v1_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiAds10v1ChanAddr_e baseAddr,
                                                                   uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_ADS10V1_H_

