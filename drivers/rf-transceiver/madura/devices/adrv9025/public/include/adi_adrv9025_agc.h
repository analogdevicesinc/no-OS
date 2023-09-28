/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_agc.h
 * \brief Contains ADRV9025 API AGC function prototypes for adi_adrv9025_agc.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_AGC_H_
#define _ADI_ADRV9025_AGC_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_agc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Sets up the device Rx Automatic Gain Control (AGC) registers
*
* The instantiated AGC setting structure (adi_adrv9025_AgcCfg_t) must be initialized with
* valid settings before this function may be used due to the dependencies
*
* This function can be called by the user to load a AGC Config or
* to reconfigure the AGC Config. The AGC config for a receiver type is set with the
* parameters passed by adi_adrv9025_AgcCfg_t agcConfig array.
* The arraySize conveyed the number of agcConfig passed in.
*
* The AGC configs can be broadcast / multicast based on the channel mask
* parameter in adi_adrv9025_AgcCfg_t structure.
*
*                        rxChannelMask | Rx Channels Programmed
*     ---------------------------------|--------------------------------------
*                         bit[0] = 1   |  Enables Rx1 gain table programming
*     ---------------------------------|--------------------------------------
*                         bit[1] = 1   |  Enables Rx2 gain table programming
*     ---------------------------------|--------------------------------------
*                         bit[2] = 1   |  Enables Rx3 gain table programming
*     ---------------------------------|--------------------------------------
*                         bit[3] = 1   |  Enables Rx4 gain table programming
*
*     Eg: To program the same gain table to channels and Rx1, the rxChannelMask
*         should be set to 0x00000001
*
*     Eg: To program a single gain table to all channels, the rxChannelMask
*         should be set to 0x0000000F
*
* \pre This function is called only if AGC will be used, and only after the
* initialization process (ADI_ADRV9025_Initialize).
* The API function that sets the Rx gain control mode adi_adrv9025_RxGainCtrlModeSet
* needs to be called after setup to switch between MGC and AGCSLOW.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{agcConfig}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure containing settings
* \param agcConfig Array contain AgcCfg data structure containing settings
* \param arraySize Size of agcConfig Array.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcCfgSet(adi_adrv9025_Device_t* device,
                               adi_adrv9025_AgcCfg_t  agcConfig[],
                               uint8_t                arraySize);

/**
* \brief Checks the Range for the device Rx Automatic Gain Control (AGC) registers
*
* The instantiated AGC setting structure (adi_adrv9025_AgcCfg_t) must be initialized with
* valid settings before this function may be used due to the dependencies	 *
*
* This function called automatically as part of adi_adrv9025_AgcCfgSet() API
* if ADI_ADRV9025_AGC_RANGE_CHECK is set to 1. This function can be called independently
* for standalone range checking of Rx AGC Config Set functionality.
*
*
* \pre This function is called only if AGC will be used, and only after the
* initialization process (ADI_ADRV9025_Initialize)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{agcConfig}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure containing settings
* \param agcConfig Array contain AgcCfg data structure containing settings
* \param arraySize Size of agcConfig Array.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcCfgSetRangeCheck(adi_adrv9025_Device_t* device,
                                         adi_adrv9025_AgcCfg_t  agcConfig[],
                                         uint8_t                arraySize);

/**
* \brief Gets the value of the AGC related registers from the ADRV9025 device.
*
* This function is used to get value of the AGC registers.
*
* \pre This function may be used only if AGC is active
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{agcConfig (most members)}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select Rx Channel.
* \param agcConfig Pointer to the agcCfg structure. Function will update values in the structure to return the current setup.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcCfgGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               adi_adrv9025_AgcCfg_t*    agcConfig);

/**
* \brief This function will setup the AGC for the dualband mode.
* \brief Not currently implemented
*
* This function compliments the adi_adrv9025_AgcCfgSet() function to set up the
* AGC for the dualband mode. The function also sets up the GPIO's,
* to enable the device to control external LNA's.
*
* \pre This function may be used only if AGC is active. This function can be
* used in radio off state.  Device must be initialized to dual band mode as
* a part of adi_adrv9025_Initialize()
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxAgcCtrlDualBand Pointer to the ADRV9025 AGC dualband settings data
* structure
* \param arraySize Size of Array.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcDualBandCfgSet(adi_adrv9025_Device_t*        device,
                                       adi_adrv9025_AgcDualBandCfg_t rxAgcCtrlDualBand[],
                                       uint8_t                       arraySize);

/**
* \brief This function will get the AGC for the dualband mode.
* \brief Not currently implemented
*
* This function compliments the adi_adrv9025_AgcCfgGet() function to set up the
* AGC for the dualband mode. The function also sets up the GPIO's,
* to enable the device to control external LNA's.
*
* \pre This function may be used only if AGC is active. This function can be
* used in radio off state.  Device must be initialized to dual band mode as
* a part of adi_adrv9025_Initialize()
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select either Rx1, Rx2,
* Rx1 + Rx2 for programming the dualband AGC.
* \param rxAgcCtrlDualBand Pointer to the ADRV9025 AGC dualband settings data
* structure
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcDualBandCfgGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_RxChannels_e      rxChannel,
                                       adi_adrv9025_AgcDualBandCfg_t* rxAgcCtrlDualBand);

/**
* \brief This function returns the current values of the LNA controls for the
* dualband mode.
* \brief Not currently implemented
*
* This function can be used if the user is not controlling the external LNA's
* through the 3.3V GPIO pins of the device. The LNA controls are programmed by
* the user in the dualband LNA gain table. The AGC then indexes to one of the
* entries in the table based on its algorithm.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select either Rx1 or Rx2
* \param rxDualBandLnaControls Pointer to the AGC dualband LNA settings data
* structure
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcDualBandLnaGpioOutGet(adi_adrv9025_Device_t*         device,
                                              adi_adrv9025_RxChannels_e      rxChannel,
                                              adi_adrv9025_DualBandLnaCfg_t* rxDualBandLnaControls);

/**
* \brief This function sets the min/max gain indexes for AGC for the specified channel.
*
* This function can be used to set the min/max gains of RX channel in runtime.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param agcGainRange Pointer to the ADRV9025 AGC dualband settings data
* structure
* \param arraySize Size of Array.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcGainIndexRangeSet(adi_adrv9025_Device_t*      device,
                                          adi_adrv9025_AgcGainRange_t agcGainRange[],
                                          uint8_t                     arraySize);

/**
* \brief This function sets the min/max gain indexes for AGC for the specified channel.
*
* This function can be used to set the min/max gains of RX channel in runtime.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannel adi_adrv9025_RxChannels_e enum type to select Rx for programming the dualband AGC.
* \param agcGainRange Pointer to the ADRV9025 AGC dualband settings data
* structure
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcGainIndexRangeGet(adi_adrv9025_Device_t*       device,
                                          adi_adrv9025_RxChannels_e    rxChannel,
                                          adi_adrv9025_AgcGainRange_t* agcGainRange);

/**
* \brief This function resets all the state machines in the Gain Control Block.
*
* resets all state machines within the gain control block to state 0
* and maximum gain (for slow, fast attack, and hybrid AGC loops)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxChannelMask Maskbits from adi_adrv9025_RxChannels_e
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcReset(adi_adrv9025_Device_t* device,
                              uint32_t               rxChannelMask);

/**
* \brief This function will enable or disable the AGC Overload GPIO output
*
* Prerequisite: The AGC clock needs to be set to Slow Loop AGC Mode before adi_adrv9025_AgcOverloadIndicatorGpioSet is invoked
*
* The function will Iterate through GPIO pins 6 - 17 and acquire the shared resource
* Sets the direction of the pins to output
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param overloadEnable > 0 will enable the overload or to Disable Overload overloadEnable = 0
*
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for GPIO output, shared resource release or AGC mode failure
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_AgcOverloadIndicatorGpioSet(adi_adrv9025_Device_t* device,
                                                 uint8_t                overloadEnable);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_AGC_H_ */
