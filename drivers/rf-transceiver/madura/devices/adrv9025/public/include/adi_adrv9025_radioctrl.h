/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_radioctrl.h
 * \brief Contains ADRV9025 related function prototypes for adi_adrv9025_radioctrl.c
 *
 *  ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_RADIOCTRL_H_
#define _ADI_ADRV9025_RADIOCTRL_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_radioctrl_types.h"
#include "adi_adrv9025_tx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Loads binary array into stream processor data memory
 *
 * A 20K element byte array is passed into this function.  The byte array is
 * obtained by reading the binary stream processor file provided by Analog
 * Devices.  The stream processor uses the information in the stream file to
 * properly power up and down the various signal chains.
 *
 * \pre This function is called after after adi_adrv9025_Initialize, and before ARM is loaded.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device data structure containing settings
 * \param byteOffset Offset (starting from 0) of where to place the binary
 *                   array (if loaded in multiple function calls)
 * \param binary Byte array containing all valid ARM file data bytes
 * \param byteCount The number of bytes in the binary arrayile
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_StreamImageWrite(adi_adrv9025_Device_t* device,
                                      uint32_t               byteOffset,
                                      uint8_t                binary[],
                                      uint32_t               byteCount);

/**
 * \brief Instructs the ADRV9025 ARM which GPIO pins are used for TDD pin control
 *
 * \brief Not currently implemented
 *
 * The ADRV9025 can control any of these related signals by sending an ARM command, or
 * the ADRV9025 ARM signals can be routed to GPIO pins.  Each signal in the armGpio
 * structure has an enable member.  If the enable member is set, the ADRV9025 ARM
 * will expect the BBP to drive that signal on the specified GPIO pin. The
 * signals can be intermixed - some on GPIO pins, and some set by ARM command.
 *
 * The BBP should call this function after loading the ARM processor via
 * adi_adrv9025_ArmImageWrite() function call.  If the BBP wishes to change the GPIO
 * assignments this function can be called again to change the configuration while
 * the ARM is in the radioOff state.  This function also sets the ADRV9025 GPIO pin
 * direction for any GPIO pins that are enabled in this function.
 *
 * \pre This function can be called after loading ADRV9025 ARM binary.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param armGpio Pointer to structure that describes which GPIO pins/settings to use for each possible ARM GPIO signal
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmGpioPinsSet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_ArmGpioCfg_t* armGpio);

/**
* \brief Commands the ADRV9025 ARM to retrieve the GPIO pins used for TDD control
*
* \brief Not currently implemented
*
* The GPIO pins not assigned to radio control are returned as INVALID
*
* The BBP should call this function after loading the ARM processor through
* adi_adrv9025_ArmImageWrite() function call.
*
* \pre This function can be called after loading ADRV9025 ARM binary.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param armGpio Pointer to structure that describes which GPIO pins/settings to use for each possible ARM GPIO signal
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ArmGpioPinsGet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_ArmGpioCfg_t* armGpio);

/**
 * \brief This function sets up the mode (Pin Mode/Non-Pin Mode) through which the signal path is controlled
 *
 *  The ADRV9025 device defaults to Non-pin mode on power up. This function is used to reconfigure
 *  the signal path control mode of Rx, ORx, Tx signal chains. Non-Pin mode signal chain control
 *  can be accomplished through adi_adrv9025_RxTxEnableSet() API. The pin mode signal chain control
 *  is accomplished through a combination of SPI registers dedicated input pins to ADRV9025 device.
 *
 *  In Pin mode, the Tx and Rx signal chains are controlled using dedicated pins RX_ENABLE<3:0> and
 *  TX_ENABLE<3:0>. ORx enable in Pin mode is accomplished through a combination of dedicated ORX_ENABLE<3:0>
 *  pins alongwith SPI registers. The table below outlines the various ORx enable modes and corresponding ORx
 *  select mechanisms.
 *
 *                  ORx PIN Mode     |    ORx Select Mechanism
 *      -----------------------------|------------------------------------------------------------------------------------
 *         Single Channel 3 Pin Mode | ORX_ENABLE[0] is enable/disable ctrl. ORx select is accomplished by ORX_ENABLE[2:1]
 *         Single Channel 2 Pin Mode | ORX_ENABLE[0] is enable/disable ctrl. ORx select is accomplished by SPI registers.
 *                                   | ORx select is multiplexed between 2 possible ORx channels depending on the level of ORx_ENABLE[1] pin
 *         Single Channel 1 Pin Mode | ORX_ENABLE[0] is enable/disable ctrl. ORx select is accomplished by SPI registers.
 *         Dual Channel 4 Pin Mode   | ORX_ENABLE[1:0] pins are enable/disable ctrl. ORx select is accomplished by ORX_ENABLE[3:2]
 *         Dual Channel 2 Pin Mode   | ORX_ENABLE[1:0] pins are enable/disable ctrl. ORx select is accomplished by SPI registers
 *
 * \pre This function may be called after the device has been fully initialized
 *       any time during run-time operation, but only after the ARM and GPIO
 *       have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param radioCtrlCfg is comprised of radio control mode configurations for Rx,ORx and Tx channels
 *
 *  In Order to avoid configuration for a specific signal chain, set the respective enable mode to invalid. Eg: To avoid
 *  Tx configuration, set radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode to ADI_ADRV9025_TX_EN_INVALID_MODE. Similarly
 *  to avoid Rx configuration set radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode to ADI_ADRV9025_RX_EN_INVALID_MODE and
 *  to avoid ORx configuration set radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode to ADI_ADRV9025_ORX_EN_INVALID_MODE.
 *
 *  Parameter radioCtrlCfg->singleChannel1PinModeOrxSel is ignored if radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode
 *  is not set to ADI_ADRV9025_ORX_EN_SINGLE_CH_1PIN_MODE
 *
 *  Parameters radioCtrlCfg->singleChannel2PinModeLowOrxSel and radioCtrlCfg->singleChannel2PinModeHighOrxSel
 *  are ignored if radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode is not set to ADI_ADRV9025_ORX_EN_SINGLE_CH_2PIN_MODE
 *
 *  Parameter radioCtrlCfg->dualChannel2PinModeOrxSel is ignored if radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode is
 *  not set to ADI_ADRV9025_ORX_EN_DUAL_CH_2PIN_MODE
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_RadioCtrlCfgSet(adi_adrv9025_Device_t*           device,
                                     adi_adrv9025_RadioCtrlModeCfg_t* radioCtrlCfg);

/**
* \brief This function performs Range check on parameters passed to adi_adrv9025_RadioCtrlCfgSet() API
*
* This function is called automatically as part of adi_adrv9025_RadioCtrlCfgSet() if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK
* is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param radioCtrlCfg is comprised of radio control mode configurations for Rx,ORx and Tx channels
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RadioCtrlCfgSetRangeCheck(adi_adrv9025_Device_t*           device,
                                               adi_adrv9025_RadioCtrlModeCfg_t* radioCtrlCfg);

/**
 * \brief Reads back the Tx, Rx, and ORx radio control mode configuration
 *
 * This function retrieves Pin mode / Non pin mode configuration of
 * radio ctrl modes for Rx/Tx/ORx signal chains. For Rx and Tx signal chains,
 * a SPI or Pin mode is returned. For the ORx signal chain, in addition to SPI vs
 * Pin mode control, settling delay(in terms of arm clk cycles) alongwith Single
 * channel 2 Pin and 1 Pin mode ORx channel selects, dual channel 2 pin mode ORx channel
 * selects are returned.
 *
 * \pre This function may be called after the device has been fully initialized
 *      any time during run-time operation, but only after the ARM and GPIO
 *      have been configured
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 data structure
 * \param rxChannel Receiver channel for which mode config readback is requested (valid channels Rx1-Rx4)
 * \param txChannel Transmitter channel for which mode config readback is requested (valid channels Tx-Tx4)
 * \param radioCtrlCfg Pointer to radio ctrl config data structure which will be updated with read back
 *        values for Rx,Tx and ORx signal chain control configs.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_RadioCtrlCfgGet(adi_adrv9025_Device_t*           device,
                                     adi_adrv9025_RxChannels_e        rxChannel,
                                     adi_adrv9025_TxChannels_e        txChannel,
                                     adi_adrv9025_RadioCtrlModeCfg_t* radioCtrlCfg);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/**
 * \brief Reads the current ARM radio state
 *
 * Currently, *radioStatus only returns data in the lower 8 bits, but
 * is defined as a 32-bit status word to permit future bitfield expansion.
 *
 * Bits  |  Bitfield
 * -------------------------
 *  1:0  | State[1:0], 0 = POWERUP, 1 = READY, 2 = IDLE, 3 = RADIO ON
 *    2  | Indicates invalid Rx/Tx/ORx profile information was loaded into ARM memory during init/ARM powerup
 *  7:3  | unused
 *
 * \pre This function may be called any time after full device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param radioStatus The current ARM radio state is returned in this parameter
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_RadioStateGet(adi_adrv9025_Device_t* device,
                                   uint32_t*              radioStatus);

/**
* \brief This API sets the Rx/ORx/Tx channels to be enabled or disabled in the
*        transceiver if the signal chain control is configured to be in SPI mode
*
* For use cases where pin mode is not required, this function can be used to
* enable/disable the Rx/ORx/Tx signal paths.  This
* function should be called after initialization and loading the stream
* processor.
*
* \pre This function should be called after initialization and loading the stream
* processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannelMask Desired Rx/ORx signal chain (channel) to power up
* \param txChannelMask Desired Tx signal chains (channel) to power up
*
* The following table lists the rxChannelMask bits and the corresponding channels
* that are enabled/disabled in SPI mode
*
*                rxChannelMask | Channel Enable/Disable
*             -----------------|------------------------------------
*                       bit[0] | 1 = Rx1 Enable, 0 = Rx1 Disable
*                       bit[1] | 1 = Rx2 Enable, 0 = Rx2 Disable
*                       bit[2] | 1 = Rx3 Enable, 0 = Rx3 Disable
*                       bit[3] | 1 = Rx4 Enable, 0 = Rx4 Disable
*                       bit[4] | 1 = ORx1 Enable, 0 = ORx1 Disable
*                       bit[5] | 1 = ORx2 Enable, 0 = ORx2 Disable
*                       bit[6] | 1 = ORx3 Enable, 0 = ORx3 Disable
*                       bit[7] | 1 = ORx4 Enable, 0 = ORx4 Disable
*
* The following table lists the txChannelMask bits and the corresponding channels
* that are enabled/disabled in SPI mode
*
*                txChannelMask | Channel Enable/Disable
*             -----------------|------------------------------------
*                       bit[0] | 1 = Tx1 Enable, 0 = Tx1 Disable
*                       bit[1] | 1 = Tx2 Enable, 0 = Tx2 Disable
*                       bit[2] | 1 = Tx3 Enable, 0 = Tx3 Disable
*                       bit[3] | 1 = Tx4 Enable, 0 = Tx4 Disable
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxTxEnableSet(adi_adrv9025_Device_t* device,
                                   uint32_t               rxChannelMask,
                                   uint32_t               txChannelMask);

/**
* \brief This function performs range check for adi_adrv9025_RxTxEnableSet API
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannelMask Desired Rx/ORx signal chain (channel) to power up
* \param txChannelMask Desired Tx signal chains (channel) to power up
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxTxEnableSetRangeCheck(adi_adrv9025_Device_t* device,
                                             uint32_t               rxChannelMask,
                                             uint32_t               txChannelMask);

/**
* \brief This API reads back the Rx/ORx/Tx channels that are powered up in the
*        transceiver
*
* For use cases where pin mode is not required, this function can be used to
* read the current state of the Rx/ORx/Tx channel enable (power up) signals.
* This function should be called after initialization and loading the stream
* processor.
*
* \pre This function can be called after initialization.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param rxChannelMask Current Rx/ORx signal chains enabled returned to this pointer address
* \param txChannelMask Current Tx signal chains enabled returned to this pointer address
*
* The following table lists the rxChannelMask bits and the corresponding channels
* that will be enabled/disabled in SPI mode
*
*                rxChannelMask | Channel Enable/Disable
*             -----------------|------------------------------------
*                       bit[0] | 1 = Rx1 Enabled, 0 = Rx1 Disabled
*                       bit[1] | 1 = Rx2 Enabled, 0 = Rx2 Disabled
*                       bit[2] | 1 = Rx3 Enabled, 0 = Rx3 Disabled
*                       bit[3] | 1 = Rx4 Enabled, 0 = Rx4 Disabled
*                       bit[4] | 1 = ORx1 Enabled, 0 = ORx1 Disabled
*                       bit[5] | 1 = ORx2 Enabled, 0 = ORx2 Disabled
*                       bit[6] | 1 = ORx3 Enabled, 0 = ORx3 Disabled
*                       bit[7] | 1 = ORx4 Enabled, 0 = ORx4 Disabled
*
* The following table lists the txChannelMask bits and the corresponding channels
* that will be enabled/disabled in SPI mode
*
*                txChannelMask | Channel Enable/Disable
*             -----------------|------------------------------------
*                       bit[0] | 1 = Tx1 Enabled, 0 = Tx1 Disabled
*                       bit[1] | 1 = Tx2 Enabled, 0 = Tx2 Disabled
*                       bit[2] | 1 = Tx3 Enabled, 0 = Tx3 Disabled
*                       bit[3] | 1 = Tx4 Enabled, 0 = Tx4 Disabled
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_RxTxEnableGet(adi_adrv9025_Device_t* device,
                                   uint32_t*              rxChannelMask,
                                   uint32_t*              txChannelMask);

/**
* \brief This function associates a GPIO pin with stream processor GP inputs and enables stream trigger
*        functionality if a valid GPIO(GPIO0-GPIO15) is assigned to the streamGpInput pins.
*
* There are 16 GPIO inputs available to trigger streams. These GPIO pins can
* be mapped to one of GPIOs[0:15].
*
* To unmap a GPIO association with a stream GP input, please set the GPIO input
* to ADI_ADRV9025_GPIO_INVALID.
*
* For B0 HW the GPIO pins are now hard coded to stream triggers as defined in the table below
*
*              Stream | StreamNo
*       --------------|---------
*       GPIO_0_FALL   |  0x23
*       GPIO_1_FALL   |  0x24
*       GPIO_2_FALL   |  0x25
*       GPIO_3_FALL   |  0x26
*       GPIO_4_FALL   |  0x34
*       GPIO_5_FALL   |  0x35
*       GPIO_6_FALL   |  0x36
*       GPIO_7_FALL   |  0x37
*       GPIO_8_FALL   |  0x38
*       GPIO_9_FALL   |  0x39
*       GPIO_10_FALL  |  0x3A
*       GPIO_11_FALL  |  0x3B
*       GPIO_12_FALL  |  0x3C
*       GPIO_13_FALL  |  0x3D
*       GPIO_14_FALL  |  0x3E
*       GPIO_15_FALL  |  0x3F
*       GPIO_0_HIGH   |  0x27
*       GPIO_1_HIGH   |  0x28
*       GPIO_2_HIGH   |  0x29
*       GPIO_3_HIGH   |  0x2A
*       GPIO_4_HIGH   |  0x40
*       GPIO_5_HIGH   |  0x41
*       GPIO_6_HIGH   |  0x42
*       GPIO_7_HIGH   |  0x43
*       GPIO_8_HIGH   |  0x44
*       GPIO_9_HIGH   |  0x45
*       GPIO_10_HIGH  |  0x46
*       GPIO_11_HIGH  |  0x47
*       GPIO_12_HIGH  |  0x48
*       GPIO_13_HIGH  |  0x49
*       GPIO_14_HIGH  |  0x4A
*       GPIO_15_HIGH  |  0x4B
*
* \pre This function requires ARM and Stream binaries to be loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param streamGpioPinCfg Pointer to the structure holding GPIO pin to stream GP input mappings (Input)
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_StreamGpioConfigSet(adi_adrv9025_Device_t*           device,
                                         adi_adrv9025_StreamGpioPinCfg_t* streamGpioPinCfg);

/**
* \brief This function retrieves the current Tx-ORx mapping control GPIO pins
*
* This function retrieves the GPIOs assigned for Tx-ORx mapping stream triggering
*
* \pre This function can be called anytime after ADRV9025 device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param streamGpioPinCfg Pointer to the structure which will be updated with the
*                         active GPIO pin to stream GP input mappings (Output)
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_StreamGpioConfigGet(adi_adrv9025_Device_t*           device,
                                         adi_adrv9025_StreamGpioPinCfg_t* streamGpioPinCfg);

/**
* \brief Sets the Tx to ORx external signal routing for external LOL initialization calibrations and tracking calibrations
*
* This function invokes the streams required to setup a Tx to ORx mapping configuration.
*
* Shown below are the Tx channels that can be mapped to ORx Front End inputs.
*
*                   ORx Front End | Possible Tx channels that can be Mapped to ORx input
*  -------------------------------|-----------------------------------------------------
*               ADI_ADRV9025_ORX1 | ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TXOFF(disables Tx-ORx1 mapping)
*               ADI_ADRV9025_ORX2 | ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TXOFF(disables Tx-ORx2 mapping)
*               ADI_ADRV9025_ORX3 | ADI_ADRV9025_TX3, ADI_ADRV9025_TX4, ADI_ADRV9025_TXOFF(disables Tx-ORx3 mapping)
*               ADI_ADRV9025_ORX4 | ADI_ADRV9025_TX3, ADI_ADRV9025_TX4, ADI_ADRV9025_TXOFF(disables Tx-ORx4 mapping)
*
* Please note that if ADI_ADRV9025_TXOFF is chosen as the tx channel to be mapped to ORx, then the Tx-ORx mapping
* for that ORx front end will be disabled.
*
* \pre This function may be called after the ARM has been initialized and loaded. But, before initialization
* calibrations are run because this is used for both the external LOL initialization and tracking calibrations.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param orxChannel Target ORx Front end for Tx-ORx mapping
* \param txChannel Target Tx channel that is routed back into the requested ORx channel
*
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for user ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxToOrxMappingSet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e orxChannel,
                                       adi_adrv9025_TxChannels_e txChannel);

/**
* \brief Gets the Tx to ORx external signal routing for external LOL initialization calibrations and tracking calibrations
*
* This function commands the ARM to return the Tx channel mapped to the requested ORx channel
*
* \pre This function may be called after the ARM and Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param orxChannel The ORx channel for which Tx channel mapping is requested
* \param txChannel The Tx channel map to the ORx channel
*
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for user ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_TxToOrxMappingGet(adi_adrv9025_Device_t*     device,
                                       adi_adrv9025_RxChannels_e  orxChannel,
                                       adi_adrv9025_TxChannels_e* txChannel);

/**
* \brief Sets the RF PLL local oscillator (LO) frequency (RF carrier frequency).
*
* \pre This function can be called after the ARM has been initialized, and
*      device must be in radioOff state.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param pllName Name of the PLL to configure based on the enumerated types in adi_adrv9025_RfPllName_e
* \param pllLoFrequency_Hz Desired RF LO frequency in Hz
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PllFrequencySet(adi_adrv9025_Device_t* device,
                                     adi_adrv9025_PllName_e pllName,
                                     uint64_t               pllLoFrequency_Hz);

/**
* \brief Sets the RF PLL settings for the desired LO
*
* \pre This function can be called after the ARM has been initialized, and
*      device must be in radioOff state.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param pllConfig Pointer to the PLL config settings
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PllFrequencySet_v2(adi_adrv9025_Device_t*    device,
                                        adi_adrv9025_PllConfig_t* pllConfig);

/**
* \brief Performs range check on parameters for adi_adrv9025_PllFrequencySet function
*
* This function is used to set a PLL's current frequency. An adi_adrv9025_RfPllName_e enumerated type is passed for
* the desired PLL frequency to set: LO1_PLL, LO2_PLL or AUX_PLL.
*
* \pre This function is automatically called as part of adi_adrv9025_PllFrequencySet if
*      ADI_ADRV9025_RADIOCTRL_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param pllName Name of the PLL to configure based on the enumerated types in adi_adrv9025_RfPllName_e
* \param pllLoFrequency_Hz Desired RF LO frequency in Hz
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PllFrequencySetRangeCheck(adi_adrv9025_Device_t* device,
                                               adi_adrv9025_PllName_e pllName,
                                               uint64_t               pllLoFrequency_Hz);

/**
* \brief Performs range check on parameters for adi_adrv9025_PllFrequencySet_v2 function
*
* \pre This function is automatically called as part of adi_adrv9025_PllFrequencySet_v2 if
*      ADI_ADRV9025_RADIOCTRL_RANGE_CHECK is set to 1.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param pllConfig Pointer to the PLL config settings
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PllFrequencySetRangeCheck_v2(adi_adrv9025_Device_t*    device,
                                                  adi_adrv9025_PllConfig_t* pllConfig);

/**
* \brief Gets a PLL Local Oscillator (LO) frequency (RF carrier frequency).
*
* This function is used to get a PLL's current frequency. A adi_adrv9025_RfPllName_e enumerated type is passed for
* the desired PLL frequency to read: LO1_PLL, LO2_PLL or AUX_PLL.
*
* \pre This function can be used after the device has been initialized, ARM firmware loaded and the
*      PLLs configured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param pllName Name of the desired PLL to read the frequency
* \param pllLoFrequency_Hz Current LO frequency for the PLL specified
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PllFrequencyGet(adi_adrv9025_Device_t* device,
                                     adi_adrv9025_PllName_e pllName,
                                     uint64_t*              pllLoFrequency_Hz);

/**
 * \brief Checks if the PLLs are locked
 *
 * This function returns the status of the ADRV9025 PLLs via the pllLockStatus pointer.
 * The 4 LSBs of the uint32_t value at pllLockStatus represent the lock status
 * of the CLK PLL, RF PLL and AUX PLL.
 *
 * pllLockStatus bit 0 = CLK PLL Lock status
 * pllLockStatus bit 1 = LO1 PLL Lock status
 * pllLockStatus bit 2 = LO2 PLL Lock status
 * pllLockStatus bit 3 = AUX(LO3) PLL Lock Status
 *
 * A bit value of 1 indicates the corresponding PLL is locked
 * A bit value of 0 indicates the corresponding PLL is unlocked.
 *
 * \pre This may be called any time after the PLLs have been configured and
 * are operational. This also requires the lockdet_mode<1:0> to have a value
 * of either 1(Run Lock Detect once) or 2(Run Lock Detect Continuously)
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device data structure containing settings
 * \param pllLockStatus PLLs lock status byte pointer to return the bit wise representation of
 *                      the ADRV9025 PLLs lock status
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_PllStatusGet(adi_adrv9025_Device_t* device,
                                  uint32_t*              pllLockStatus);

/**
 * \brief Performs Range Check for adi_adrv9025_PllLoopFilterSet function
 *
 * \pre This function is called automatically as part of adi_adrv9025_PllLoopFilterSet
 *      API if ADI_ADRV9025_RADIOCTRL_RANGE_CHECK is set to 1
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device data structure containing settings
 * \param pllName selects the target PLL for loop filter configuration
 * \param pllLoopFilterConfig pointer to the PLL loop filter configuration structure.
 *        Configuration includes the following 1) Phase margin (40 - 85 degrees),
 *        2) Loop BW(50KHz - ) 3) Power Scale(0 - 10, default is 10)
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_PllLoopFilterSetRangeCheck(adi_adrv9025_Device_t*           device,
                                                adi_adrv9025_PllName_e           pllName,
                                                adi_adrv9025_PllLoopFilterCfg_t* pllLoopFilterConfig);

/**
* \brief Sets the configuration for the PLL loop filter
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure containing settings
* \param pllName selects the target PLL for loop filter configuration
* \param pllLoopFilterConfig pointer to the PLL loop filter configuration structure.
*        Configuration includes the following 1) Phase margin (40 - 85 degrees),
*        2) Loop BW(50KHz - ) 3) Power Scale(0 - 10, default is 10)
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_PllLoopFilterSet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_PllName_e           pllName,
                                      adi_adrv9025_PllLoopFilterCfg_t* pllLoopFilterConfig);

/**
 * \brief Gets the configuration for the RF PLL loop filter
 *
 * This function is used to get the current loop bandwidth, phase margin and the power scale
 * factor for the PLL
 *
 * \pre This function can be used after the device has been initialized and the
 * PLL configured. The ARM firmware must also be loaded and running.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device data structure containing settings
 * \param pllName selects the target PLL for loop filter configuration
 * \param pllLoopFilterConfig pointer to the PLL loop filter configuration structure.
 *
 * \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_PllLoopFilterGet(adi_adrv9025_Device_t*           device,
                                      adi_adrv9025_PllName_e           pllName,
                                      adi_adrv9025_PllLoopFilterCfg_t* pllLoopFilterConfig);

/**
* \brief Retrieves Current Dual Band status
*
* This function can be used to read back the current status of the dual band feature
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param status Pointer to variable which the dual band status value will be written
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for CPU reset required
* \retval ADI_ADRV9025_ACT_ERR_RERUN_INIT_CALS Recovery action rerun init cals required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DualBandStatusGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_DualbandStatus_e* status);

/**
* \brief Reads back the version of the Stream binary loaded into the ADRV9025 ARM memory
*
* This function reads the ARM memory to read back the major.minor.maint.build releaseCandidate
* version for the Stream binary loaded into ARM memory.
*
* <B>Dependencies</B>
* - device->spiSettings->chipSelectIndex
*
* \param device is structure pointer to the ADRV9025 data structure containing settings
* \param streamVersion  will be populated here, it is a pointer to struct type adi_adrv9025_StreamVersion_t.
*
* \retval ADI_ADRV9025_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_StreamVersionGet(adi_adrv9025_Device_t*        device,
                                      adi_adrv9025_StreamVersion_t* streamVersion);

/****************************************************************************
* Helper functions
****************************************************************************
*/

/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_RADIOCTRL_H_ */
