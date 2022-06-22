// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_radioctrl.h
 * \brief Contains Talise related function prototypes for talise_radioctrl.c
 *
 *  Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_RADIOCTRL_H_
#define TALISE_RADIOCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_radioctrl_types.h"
#include "talise_tx_types.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Loads binary array into stream processor data memory
 *
 * A 4096 element byte array is passed into this function.  The byte array is
 * obtained by reading the binary stream processor file provided by Analog
 * Devices.  The stream processor uses the information in the stream file to
 * properly power up and down the various signal chains.
 *
 * \pre This function is called after the device has been initialized, and PLL
 *      lock status has been verified
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is structure pointer to the Talise data structure containing settings
 * \param binary is a byte array containing the parsed stream processor binary file
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_loadStreamFromBinary(taliseDevice_t *device, uint8_t *binary);

/**
 * \brief Instructs the Talise ARM which GPIO pins are used for TDD pin control
 *
 * The Talise can control any of these related signals by sending an ARM command, or
 * the Talise ARM signals can be routed to GPIO pins.  Each signal in the armGpio
 * structure has an enable member.  If the enable member is set, the Talise ARM
 * will expect the BBP to drive that signal on the specified GPIO pin. The
 * signals can be intermixed - some on GPIO pins, and some set by ARM command.
 *
 * The BBP should call this function after loading the ARM processor
 * TALISE_loadArmFromBinary() function call.  If the BBP wishes to change the GPIO
 * assignments this function can be called again to change the configuration while
 * the ARM is in the radioOff state.  This function also sets the Talise GPIO pin
 * direction for any GPIO pins that are enabled in this function.
 *
 * \pre This function can be called after loading Talise ARM binary, but before
 *      entering radioOn during initialization.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param armGpio Pointer to structure that describes which GPIO pins/settings to use for each possible ARM GPIO signal
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setArmGpioPins(taliseDevice_t *device,
			       taliseArmGpioConfig_t *armGpio);


/**
 * \brief Sets the Tx, Rx, and ORx radio control bit mask for pin mode control
 *
 *  This function should be used in the radio off state.
 *
 * \pre This function may be called after the device has been fully initialized
 *       any time during run-time operation, but only after the ARM and GPIO
 *        have been configured
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param pinOptionsMask is comprised of or'd taliseRadioCtlCfg1_t enumerated types for
 * radio control configuration register 1:
 * TAL_TXRX_PIN_MODE = 0x01: selects Tx and Rx signaling via pins
 * TAL_ORX_PIN_MODE = 0x02: selects ORx signaling via pins
 * TAL_ORX_USES_RX_PINS = 0x04: selects if Rx pins are used to indicate ORx mode activation/deactivation
 * TAL_ORX_SEL = 0x10: selects ORx2 (1), when not used defaults to ORx1 (0)
 * TAL_ORX_SINGLE_CHANNEL = 0x20: selects SPI to choose ORx channel and single pin to enable ORx mode
 * TAL_ORX_ENAB_SEL_PIN = 0x40: selects the pin from the GPIO pin pair for ORx enable
 *
 * \param orxEnGpioPinSel is a taliseRadioCtlCfg2_t enumerated type
 * which selects one of three possible GPIO pin pair combinations:
 * TAL_ORX1ORX2_PAIR_01_SEL = 0x00: GPIO pin pair (0,1) assigned to ORx1 and ORx2 respectively
 * TAL_ORX1ORX2_PAIR_45_SEL = 0x01: GPIO pin pair (4,5) assigned to ORx1 and ORx2 respectively
 * TAL_ORX1ORX2_PAIR_89_SEL = 0x02: GPIO pin pair (8,9) assigned to ORx1 and ORx2 respectively
 * TAL_ORX1ORX2_PAIR_NONE_SEL = 0x03: no GPIO pins selected
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_setRadioCtlPinMode(taliseDevice_t *device,
				   uint8_t pinOptionsMask, taliseRadioCtlCfg2_t orxEnGpioPinSel);

/**
 * \brief Reads back the Tx, Rx, and ORx radio control bit mask for pin mode
 *        control
 *  This function can be called from the radio off or radio on state.
 *
 * \pre This function may be called after the device has been fully initialized
 *      any time during run-time operation, but only after the ARM and GPIO
 *      have been configured
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param pinOptionsMask Return bitmask comprised of or'd taliseRadioCtlCfg1_t enumerated types for
 * radio control configuration register 1:
 * TAL_TXRX_PIN_MODE = 0x01: selects Tx and Rx signaling via pins
 * TAL_ORX_PIN_MODE = 0x02: selects ORx signaling via pins
 * TAL_ORX_USES_RX_PINS = 0x04: selects if Rx pins are used to indicate ORx mode activation/deactivation
 * TAL_ORX_SEL = 0x10: selects ORx2 (1), when not used defaults to ORx1 (0)
 * TAL_ORX_SINGLE_CHANNEL = 0x20: selects SPI to choose ORx channel and single pin to enable ORx mode
 * TAL_ORX_ENAB_SEL_PIN = 0x40: selects the pin from the GPIO pin pair for ORx enable
 *
 * \param orxEnGpioPinSel is a taliseRadioCtlCfg2_t enumerated type
 * which selects one of three possible GPIO pin pair combinations:
 * TAL_ORX1ORX2_PAIR_01_SEL = 0x00: GPIO pin pair (0,1) assigned to ORx1 and ORx2 respectively
 * TAL_ORX1ORX2_PAIR_45_SEL = 0x01: GPIO pin pair (4,5) assigned to ORx1 and ORx2 respectively
 * TAL_ORX1ORX2_PAIR_89_SEL = 0x02: GPIO pin pair (8,9) assigned to ORx1 and ORx2 respectively
 * TAL_ORX1ORX2_PAIR_NONE_SEL = 0x03: no GPIO pins selected
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getRadioCtlPinMode(taliseDevice_t *device,
				   uint8_t *pinOptionsMask, taliseRadioCtlCfg2_t *orxEnGpioPinSel);


/**
 * \brief Sets the ORx LO (local oscillator) source (RFPLL or AuxPLL) settings.
 *
 *  This function can optionally be called during the application layers init
 *  sequence if it is desired to use the AuxLO for the Observation receiver, or
 *  if it is desired to be able to switch the ORx LO between the RFPLL and
 *  AuxLO.  If this function is not called, the ORx LO will use the RFPLL.
 *
 *  In certain cases, it may be useful for the BBIC to use the AuxLO on the ORx
 *  receiver to offset the ORx receive data from the Tx data, which uses RFPLL.
 *  This can help in separate the Tx LO leakage from the ORx DC offset to improve
 *  calibration performance.
 *
 *  If this feature is enabled, note that the Talise trackings calibrations
 *  will be limited or not usable, as some of the Talise tracking cals require
 *  using the AuxLO.
 *
 *  If this function is enabled to set the ORx LO source to AuxLO, the BBIC can
 *  control the ORx Lo source either using the TALISE_setOrxLoSource() function
 *  or by assigning a GPIO pin (GPIO0-15), where the pin level determines the
 *  LO source.  If real time ORx LO source control is required, the pin mode
 *  is recommended. Assigning the orxLoCfg->gpioSelect = TAL_GPIO_INVALID
 *  disables pin mode.
 *
 * NOTE: When the LO source is changed(either by API or GPIO pin),
 * the setting does not take affect until the ORx rising edge to power up the
 * ORx channel(s). If the LO source is changed while ORx is powered up, the
 * LO source will not take affect until ORx is disabled and re-enabled.  This is
 * valid when using pins to set the LO source and control when ORx is enabled as
 * well.
 *
 * \pre This function should be called after loading the ARM, but before
 *      running init calibrations.  The AuxLO frequency must also be set
 *      after setting the RFPLL frequency, but before init cals.  This function
 *      can be called in the ARM READY(before init cals) or Radio off state.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param orxLoCfg Structure to provide desired settings for this feature
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_setOrxLoCfg(taliseDevice_t *device,
			    const taliseOrxLoCfg_t *orxLoCfg);

/**
 * \brief Gets the ORx LO (local oscillator) source (RFPLL or AuxPLL)
 *
 *  This function can be used to read back the ORx LO config set by the
 *  TALISE_setOrxLoCfg() function.
 *
 * \pre This function can be called anytime after TALISE_setOrxLoCfg() has been
 *      called. This function can be called in the ARM READY(before init cals)
 *      or Radio off state.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param orxLoCfg Structure to return settings readback for this feature
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getOrxLoCfg(taliseDevice_t *device, taliseOrxLoCfg_t *orxLoCfg);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/**
 * \brief Instructs the ARM processor to move the radio state to the Radio ON state
 *
 * When the ARM to the Radio On state, the enabled Rx and Tx signal chains will power up,
 * and the ARM tracking calibrations will begin.  To exit this state back to a low power,
 * off-line state, call the TALISE_radioOff() function.
 *
 * \pre This function is called after a full device initialization has taken place, all PLLs
 * are configured and locked, Multi-Chip Sync (MCS) has taken place, and the JESD204B
 * links are configured and operational.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
 * \retval TALACT_ERR_RERUN_INIT_CALS Recovery action to re-run init calibrations required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_radioOn(taliseDevice_t *device);

/**
 * \brief Instructs the ARM processor to move the radio state to the off state
 *
 * When the ARM moves from the Radio On state to Radio Off (Idle) the ARM tracking calibrations
 * are stopped and the TxEnable/RxEnable, etc GPIO control pins will be ignored.  This will also
 * keep the receive and transmit chains powered down until the TALISE_radioOn() function
 * is called again.
 *
 * \pre This function may be called any time TALISE_radioOn had been called previously
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end

 * \param device Pointer to the device settings structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_radioOff(taliseDevice_t *device);

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
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param radioStatus The current ARM radio state is returned in this parameter
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRadioState(taliseDevice_t *device, uint32_t *radioStatus);

/**
 * \brief This API sets the Rx/ORx/Tx channels to be enabled or disabled in the
 *        transceiver
 *
 * For use cases where pin mode is not required, this function can be used to
 * enable/disable the Rx/ORx/Tx signal paths while in the radioOn state.  This
 * function should be called after initialization and loading the stream
 * processor.  Rx and ORx channels can not be enabled at the same time.  Also
 * ORx1 and ORx2 at the same time is not currently a supported option, but may
 * be supported later when the ORx profile has a lower bandwidth and does not
 * require ADC stitching.
 *
 * \pre This function can be called after entering radioOn. During radioOff, all
 *      transmit and receive chains are forced off.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param rxOrxChannel Desired Rx/ORx signal chain (channel) to power up.
 *                     Either Rx or ORx, not both at the same time.
 * \param txChannel Desired Tx signal chains (channel) to power up
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setRxTxEnable(taliseDevice_t *device,
			      taliseRxORxChannels_t rxOrxChannel, taliseTxChannels_t txChannel);

/**
 * \brief This API reads back the Rx/ORx/Tx channels that are powered up in the
 *        transceiver
 *
 * For use cases where pin mode is not required, this function can be used to
 * read the current state of the Rx/ORx/Tx channel enable (power up) signals.
 * This function should be called after initialization and loading the stream
 * processor.
 *
 * \pre This function can be called after entering radioOn.  During radioOff, all
 *      transmit /receive chains are forced off.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param rxOrxChannel Current Rx/ORx signal chains enabled returned to this pointer address
 * \param txChannel Current Tx signal chains enabled returned to this pointer address
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRxTxEnable(taliseDevice_t *device,
			      taliseRxORxChannels_t *rxOrxChannel, taliseTxChannels_t *txChannel);

/**
 * \brief Sets the Tx to ORx external signal routing for external LOL initialization calibrations and tracking calibrations
 *
 * Two pins define the Tx to ORx mappings. Each of these pins can be individually configured using GPIO.
 * The BBIC can toggle the GPIO pins directly to specify which Tx channel is observable on either
 * ORx channel. However, in the case of the lack of GPIO pins a command mode can be invoked to perform this task.
 * This function uses the command mode to map the Tx to ORx routing.
 *
 * \pre This function may be called after the ARM has been initialized and loaded. But, before initialization
 * calibrations are run because this is used for both the external LOL initialization and tracking calibrations.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param txCalEnable Enables Tx calibrations when set. '1' = run, '0' = off.  This bit is ignored whenever
 * oRx1Map[0] = '0' or oRx2Map[0] = '0'
 *
 * \param oRx1Map enumerated type taliseTxToOrxMapping_t which maps either Tx1 or Tx2 to ORx1.
 * \param oRx2Map enumerated type taliseTxToOrxMapping_t which maps either Tx1 or Tx2 to ORx2.
 *
 * Both oRx1Map and oRx2Map use the taliseTxToOrxMapping_t enumerated type encoded as a bit-field:
 * TAL_MAP_NONE    = 0x00 = No Tx at ORx input
 * (No ENUM)       = 0x01 = No Tx at ORx input
 * TAL_MAP_TX1_ORX = 0x02 = Tx1 mapped to enabled ORx
 * TAL_MAP_TX2_ORX = 0x03 = Tx2 mapped to enabled ORx
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for user ARM reset required
 * \retval TALACT_ERR_RERUN_INIT_CALS Recovery action to re-run init calibrations required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setTxToOrxMapping(taliseDevice_t *device, uint8_t txCalEnable,
				  taliseTxToOrxMapping_t oRx1Map, taliseTxToOrxMapping_t oRx2Map);

/**
 * \brief Sets the RF PLL local oscillator (LO) frequency (RF carrier frequency).
 *
 * \pre This function can be called after the ARM has been initialized, and
 *      device must be in radioOff state.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param pllName Name of the PLL to configure based on the enumerated types in taliseRfPllName_t
 * \param rfPllLoFrequency_Hz Desired RF LO frequency in Hz
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setRfPllFrequency(taliseDevice_t *device,
				  taliseRfPllName_t pllName, uint64_t rfPllLoFrequency_Hz);

/**
 * \brief Gets a PLL Local Oscillator (LO) frequency (RF carrier frequency).
 *
 * This function is used to get a PLL's current frequency. A taliseRfPllName_t enumerated type is passed for
 * the desired PLL frequency to read: RF_PLL, AUX_PLL, or CLK_PLL.
 *
 * \pre This function can be used after the device has been initialized and the
 *      PLLs configured.  For AUX_PLL or RF_PLL, the ARM firmware must also be
 *      loaded and running to read back the PLL frequencies.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param pllName Name of the desired PLL to read the frequency
 * \param rfPllLoFrequency_Hz Current LO frequency for the PLL specified
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRfPllFrequency(taliseDevice_t *device,
				  taliseRfPllName_t pllName, uint64_t *rfPllLoFrequency_Hz);

/**
 * \brief Checks if the PLLs are locked
 *
 * This function returns the status of the Talise PLLs via the pllLockStatus pointer.
 * The 3 LSBs of the uint8_t value at pllLockStatus represent the lock status
 * of the CLK PLL, RF PLL and AUX PLL.
 *
 * pllLockStatus bit 0 = CLK PLL Lock status
 * pllLockStatus bit 1 = RF PLL Lock status
 * pllLockStatus bit 2 = AUX PLL Lock Status
 *
 * A bit value of 1 indicates the corresponding PLL is locked
 * A bit value of 0 indicates the corresponding PLL is unlocked.
 *
 * \pre This may be called any time after the PLLs have been configured and
 * are operational
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param pllLockStatus PLLs lock status byte pointer to return the bit wise representation of
 *                      the Talise PLLs lock status
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getPllsLockStatus(taliseDevice_t *device,
				  uint8_t *pllLockStatus);

/**
 * \brief Sets the configuration for the RF PLL loop filter
 *
 * \deprecated This function is deprecated.  TALISE_setPllLoopFilter should be
 *             used instead, as it allows setting the RF or Aux PLL loop
 *             filter settings.
 *
 * \pre This function can be called after the ARM has been initialized, and
 * device must be in radioOff state. This function must be followed with a
 * TALISE_setRfPllFrequency function for the TAL_RF_PLL to set up the RF PLL
 * with the new configuration.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param loopBandwidth_kHz Desired loop bandwidth in kHz. Valid range is between
 * 50kHz and 750kHz.
 * \param stability Factor impacts noise and stability of the loop filter.
 * Valid range is between 3 and 15.
 * Lower the value, the lower the stability and higher the rejection of noise.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setRfPllLoopFilter(taliseDevice_t *device,
				   uint16_t loopBandwidth_kHz, uint8_t stability);

/**
 * \brief Gets the configuration for the RF PLL loop filter
 *
 * \deprecated This function is deprecated.  TALISE_getPllLoopFilter should be
 *             used instead, as it allows reading the RF or Aux PLL loop
 *             filter settings.
 *
 * This function is used to get the current loop bandwidth and the stability
 * factor for the RF_PLL
 *
 * \pre This function can be used after the device has been initialized and the
 * RF_PLL configured. The ARM firmware must also be loaded and running.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param loopBandwidth_kHz Loop bandwidth of RFPLL in kHz
 * \param stability Factor impacts noise and stability of the loop filter.
 * Lower the value, the lower the stability and higher the rejection of noise.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getRfPllLoopFilter(taliseDevice_t *device,
				   uint16_t *loopBandwidth_kHz, uint8_t *stability);

/**
 * \brief Sets the loop filter bandwidth for the RFPLL or AuxPLL
 *
 * \pre This function can be called after the ARM has been initialized, and
 * device must be in radioOff state. This function must be followed with a
 * TALISE_setRfPllFrequency function for the selected PLL to set up the PLL
 * with the new loop filter settings.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param pllName Desired PLL to adjust loop filter settings for
 * \param loopBandwidth_kHz Desired loop bandwidth in kHz. Valid range is between
 * 50kHz and 750kHz.
 * \param stability Factor impacts noise and stability of the loop filter.
 * Valid range is between 3 and 15.
 * Lower the value, the lower the stability and higher the rejection of noise.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setPllLoopFilter(taliseDevice_t *device,
				 taliseRfPllName_t pllName, uint16_t loopBandwidth_kHz, uint8_t stability);

/**
 * \brief Gets the Loop filter settings for the RFPLL or Aux PLL loop filter
 *
 * This function is used to get the current loop bandwidth and the stability
 * factor for the RF or Aux PLL.
 *
 * \pre This function can be used after the device has been initialized and the
 * desired PLL configured. The ARM firmware must also be loaded and running.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param pllName Desired PLL to read back loop filter settings for
 * \param loopBandwidth_kHz Loop bandwidth of RFPLL in kHz
 * \param stability Factor impacts noise and stability of the loop filter.
 * Lower the value, the lower the stability and higher the rejection of noise.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getPllLoopFilter(taliseDevice_t *device,
				 taliseRfPllName_t pllName, uint16_t *loopBandwidth_kHz, uint8_t *stability);

/**
 * \brief Sets the ORx LO (local Oscillator) source (RF PLL or Aux PLL)
 *
 * NOTE: This function sets the LO source for ORx to RF PLL or Aux PLL, but
 * the setting does not take affect until the ORx rising edge to power up the
 * ORx channel(s). If the LO source is changed while ORx is powered up, the
 * LO source will not take affect until ORx is disabled and re-enabled.  This is
 * valid when using pins to set the LO source and control when ORx is enabled as
 * well.
 *
 * \pre This function can be used after the device has been fully initialized
 *      and the init cals have already run. This should only be called in the
 *      radio Off state.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param orxLoSource Input parameter to choose the desired ORx LO source (RF PLL or Aux PLL)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setOrxLoSource(taliseDevice_t *device,
			       taliseObsRxLoSource_t orxLoSource);

/**
 * \brief Gets the ORx LO (local Oscillator) source (RF PLL or Aux PLL)
 *
 * This function reads back the current ORx LO source (RF PLL or Aux PLL)
 *
 * \pre This function can be used after the device has been fully initialized
 *      and the init cals have already run. This can be called in the
 *      radio On or Off state.
 *
 * \note In radio Off, this function will read back RF PLL
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param orx1LoSource Returns ORx1 LO source (RFLO or AuxLO)
 * \param orx2LoSource Returns ORx2 LO source (RFLO or AuxLO)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getOrxLoSource(taliseDevice_t *device,
			       taliseObsRxLoSource_t *orx1LoSource, taliseObsRxLoSource_t *orx2LoSource);

/**
 * \brief This function configures frequency hop trigger GPIO input to ARM and
 *        also the frequency hopping range.
 *
 * FHM trigger GPIO pin, frequency hopping minimum frequency and frequency hopping
 * maximum frequencies are configured by this function.
 *
 * Please note that only TAL_GPIO_00 - TAL_GPIO_15 are valid for frequency
 * hop trigger GPIO pin. If TAL_GPIO_INVALID is set, then the GPIO is
 * unassigned from FHM trigger functionality.
 *
 * \pre This function can be used after the device has been fully initialized
 *      and the ARM processor is initialized. This function can
 *      be called only in the radio Off state.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param fhmConfig pointer to FHM configuration data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setFhmConfig(taliseDevice_t *device,
			     taliseFhmConfig_t *fhmConfig);

/**
 * \brief This function retrieves the current FHM setting
 *
 * This function reads back FHM trigger GPIO pin, frequency hopping minimum frequency
 * and frequency hopping maximum frequencies.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param fhmConfig FHM configuration data structure pointer memory which will be
 *        updated with the current FHM config
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getFhmConfig(taliseDevice_t *device,
			     taliseFhmConfig_t *fhmConfig);

/**
 * \brief This function sets the mode for frequency hopping
 *
 * This function can be used to enable / disable frequency hopping
 * enable/disable MCS synchronization and sets the exit mode to quick/full
 * and the initial frequecy for hopping
 *
 * For Frequency Hopping, MCS calibration parameters are modified to
 * speed up calibration but are restored on exiting FHM mode
 *
 * PLL loop bandwidth is increased to 600KHz for FHM and on full exit the
 * PLL loop bandwidth will be resored to narrowband, RF and Aux PLLs recalibrated
 * and tracking cals resumed. In quick exit case, RF PLL loop bandwidth is left
 * unchanged.
 *
 * Please note that in order to switch modes, FHM mode has to be disabled first.
 *
 * \pre This function can be used after the device has been fully initialized
 *      and the ARM processor is initialized and TALISE_setFhmConfig() is called.
 *      This function can only be called in Radio On state.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param fhmMode Frequency hop mode settings
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setFhmMode(taliseDevice_t *device, taliseFhmMode_t *fhmMode);

/**
 * \brief This function retrieves the current FHM mode settings for frequency hopping
 *
 * This function can be used to read the current FHM configuration (current
 * status of FHM enable/disable, MCS synchronization and exit mode settings)
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param fhmMode Structure pointer to frequency hop mode settings to update
 *        read back mode settings information.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getFhmMode(taliseDevice_t *device, taliseFhmMode_t *fhmMode);

/**
 * \brief This function sets the next RF PLL LO hop frequency and
 *        in a non-gpio mode sends a command to ARM to hop to next frequency
 *
 * This function sets the next RF PLL LO hop frequency for GPIO and Non-GPIO
 * modes and in case of a Non-GPIO mode, sends a command to the ARM processor
 * to trigger a frequency hop. In a GPIO mode the user is expected to transmit
 * a low to high pulse on the assigned GPIO to trigger a frequency hop after
 * calling this function.
 *
 * \pre This function can be called after the device has been fully initialized
 *      and the ARM processor is initialized and FHM config is set via
 *      TALISE_setFhmConfig() API and the FHM mode is set via
 *      TALISE_setFhmMode() API. This API can be called only in Radio ON state
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param nextRfPllFrequency_Hz Next FHM mode hop frequency for RF PLL
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setFhmHop(taliseDevice_t *device,
			  uint64_t nextRfPllFrequency_Hz);

/**
 * \brief This function allows the user to readback current RF PLL Frequency
 *        in Frequency Hopping mode
 *
 * This function can be used to read the current Rf Pll frequency in fast
 * frequency hopping mode
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param fhmRfPllFrequency_Hz pointer to FHM hop frequency which will be updated
 *        with the current Rf Pll frequency in hop mode
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getFhmRfPllFrequency(taliseDevice_t *device,
				     uint64_t *fhmRfPllFrequency_Hz);

/**
 * \brief This function returns the current FHM status
 *
 * This function can be used to read the current FHM status. Please refer to taliseFhmStatus_t structure definition
 * for detailed description of status fields.
 *
 * Currently active FHM error status recorded is available in fhmStatus->currentFhmHopErrorStatus field
 * The table below gives information about the format of currentFhmHopErrorStatus
 *
 *          fhmStatus->currentFhmHopErrorStatus  |   Error Info
 *          -------------------------------------|---------------------------
 *                                       bit[0]  |   Not Used
 *                                       bit[1]  |   Not Used
 *                                       bit[2]  |   Not Used
 *                                       bit[3]  |   Not Used
 *                                       bit[4]  |   Not Used
 *                                       bit[5]  |   1 = Invalid Scan Range specified, 0 = No invalid scan range encountered
 *                                       bit[6]  |   1 = Requested hop frequency outside scan range, 0 = No invalid hop freq requested
 *                                       bit[7]  |   1 = PLL Error, 0 = No PLL Error
 *                                       bit[8]  |   1 = Loop Filter algorithm failed to find solution, 0 = No loop filter alg error
 *                                       bit[9]  |   1 = Loop Filter invalid input parameter, 0 = No loop filter error
 *                                      bit[10]  |   1 = PLL LO computation error, 0 = No PLL LO computation error
 *                                      bit[11]  |   1 = RF PLL Lock failed, 0 = No RF PLL lock failures
 *                                      bit[12]  |   1 = Aux PLL Lock failed, 0 = No Aux PLL lock failures
 *                                      bit[13]  |   1 = General Error, 0 = No General Error
 *                                      bit[14]  |   Not Used
 *                                      bit[15]  |   Not Used
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param fhmStatus pointer to FHM status data structure where current arm status read back will be updated
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getFhmStatus(taliseDevice_t *device,
			     taliseFhmStatus_t *fhmStatus);

/**
 * \brief This function enables/disables the external LO output and sets the
 *        external LO output divider.
 *
 * This function selects the Ext LO output frequency by dividing from
 * (RFPLL VCO / 2).  The RFPLL VCO is always in the range of 6G to 12G.
 * Therefore, the maximum Ext LO output frequency is 6G.
 *
 * To properly set the extLoOutDivide function parameter, the calling function
 * must know the RFPLL VCO frequency. The easiest way to calculate the RFPLL
 * VCO frequency is to multiply the RFPLL LO frequency that was set with a
 * power of 2 until in the 6G to 12G range.
 *
 * If the internal LO dividers for the RFPLL are changed with
 * TALISE_setRfPllFrequency() and the internal LO divider changes, it is likely this
 * function would need to be called again to scale extLoOutDivide as well to
 * handle boundary conditions.
 *
 * Boundary conditions exist at these frequencies: 3000 Mhz, 1500Mhz, 750Mhz,
 * 375Mhz, and 187.5Mhz.  When the internal LO frequency moves across one of these
 * boundaries, the extLoOutDivide setting will need to be adjusted.
 *
 * If External LO Input mode is enabled, the External LO output can not be used,
 * as it shares the same pin on the device.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param enableExtLoOutput Enable/Disable the LO output buffer.
 * \param extLoOutDivide Sets external LO output frequency = (RFPLL VCO/2/extLoOutDivide)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setExtLoOutCfg(taliseDevice_t *device,
			       uint8_t enableExtLoOutput, taliseExtLoDiv_t extLoOutDivide);

/**
 * \brief This function reads back the enable status of the External LO output
 *        and the external LO output divider setting.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param enableExtLoOutput Readback of Enable/Disable the LO output buffer.
 * \param extLoOutDivide Readback of the external LO output divider
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action to debug why HAL layer timer returned an error
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getExtLoOutCfg(taliseDevice_t *device,
			       uint8_t *enableExtLoOutput, taliseExtLoDiv_t *extLoOutDivide);
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

#endif /* TALISE_RADIOCTRL_H_ */
