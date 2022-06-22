// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_tx.h
 * \brief Contains Talise transmit related function prototypes for
 *        talise_tx.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_TX_H_
#define TALISE_TX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_tx_types.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/**
 * \brief This API function has to be used in order to set the Tx RF output attenuation for each Tx output channel
 *
 * The attenuation that will be applied to the passed \c txChannel as follows:
 * txChannel  |  Description
 * -----------|------------------------------------
 *   TX1      | sets attenuation txAttenuation_mdB for TX1
 *   TX2      | sets attenuation txAttenuation_mdB for TX2
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device's data structure
 * \param txChannel should be one of the enum taliseTxAttChannels_t, note that TXOFF wont have any effect. Only can set one channel at a time.
 * \param txAttenuation_mdB The desired TxAttenuation in milli-dB (Range: 0 to 41950 mdB)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setTxAttenuation(taliseDevice_t *device,
				 taliseTxChannels_t txChannel, uint16_t txAttenuation_mdB);

/**
 * \brief Reads back the Tx1 or Tx2 RF output attenuation
 *
 *  This function reads back the Tx attenuation setting for either the Tx1 or Tx2 RF channel currently applied to the transmit chain.
 *  This function can work with SPI mode or pin controlled Tx attenuation mode using the increment/decrement GPIO
 *  pins.
 *
 * \pre The Tx data path must be powered up for the current attenuation value to be valid.  If the Tx data path
 *  is powered down or the radio is off, the last Tx attenuation setting when the Tx output was previously active will be
 *  read back.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device's data structure
 * \param txChannel Is one of the enum taliseTxAttChannels_t channel types, where only TAL_TX1 or TAL_TX2 are valid choices.
 *  Note that TAL_TX_OFF will not have any effect.
 * \param txAttenuation_mdB Pointer to the readback value of the desired Tx channel attenuation in milli-dB (Range: 0 to 41950 mdB)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getTxAttenuation(taliseDevice_t *device,
				 taliseTxChannels_t txChannel, uint16_t *txAttenuation_mdB);


/**
 * \brief Sets the DAC full scale current boost
 *
 * This function can be used to adjust the DAC fullscale.
 * This will change the Tx output power. The DAC fullscale of zero DB is the
 * default setting.
 *
 * \pre This function must be called before loading the Talise ARM processor
 *
 * \param device Pointer to the Talise device's data structure
 * \param dacFullScale Is of the enum taliseDacFullScale_t that are the options
 *  for the DAC boost levels.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setDacFullScale(taliseDevice_t *device,
				taliseDacFullScale_t dacFullScale);

/**
 * \brief Enables/Disables the Tx NCO test tone
 *
 *  This function enables/disables a digital numerically controlled oscillator
 *  in the Talise Digital to create a test CW tone on Tx1 and Tx2 RF outputs.
 *
 *  The TxAttenuation is forced in this function to max analog output power,
 *  but the digital attenuation is backed off 6dB to make sure the digital
 *  filter does not clip and cause spurs in the tx spectrum. Ensure no other
 *  API's are called that change the Tx attenuation mode when using this
 *  function
 *
 *  When the Tx NCO test tone is disabled, the function sets the Tx attenuation
 *  to the SPI mode. User may need to use other functions to change the mode.
 *
 * \param device Pointer to the Talise device's data structure
 * \param txNcoTestToneCfg Pointer to taliseTxNcoTestToneCfg_t structure that
 * contains the configuration for the Tx NCO test tone
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableTxNco(taliseDevice_t *device,
			    taliseTxNcoTestToneCfg_t *txNcoTestToneCfg);

/**
 * \brief Enables/Disables the Tx NCO
 *
 *  This function enables/disables a digital numerically controlled oscillator
 *  in the Talise Digital to create a test CW tone on Tx1 and Tx2 RF outputs.
 *
 *  The TxAttenuation is forced in this function to max analog output power,
 *  but the digital attenuation is backed off 6dB to make sure the digital
 *  filter does not clip and cause spurs in the tx spectrum. Ensure no other
 *  API's are called that change the Tx attenuation mode when using this
 *  function
 *
 * This api is similar in functionality to TALISE_enableTxNco but it has a few
 * additional uses, in addition to configuring the NCO it can also configure
 * Each nco independantly and it can also modify the signal of each by altering
 * the phase of each.
 *
 * \param device Pointer to the Talise device's data structure
 * \param txNcoTestToneCfg Pointer to taliseTxNcoShifterCfg_t structure that
 * contains the configuration for the Tx NCO test tone and additional parameters
 * related to phase of the signals for each tx.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_txNcoShifterSet(taliseDevice_t *device,
				taliseTxNcoShifterCfg_t *txNcoTestToneCfg);

/**
 * \brief Configures Gain steps and the GPIO inputs for Tx attenuation control
 *
 * This API function configures the GPIO input pin and step size to allow the
 * BBP to control attenuation changes in Tx signal chain.
 * A high pulse on the 'txAttenIncPin' in pin control mode will increment the
 * gain by the value set in 'stepSize'.
 * A high pulse on the 'txAttenDecPin' in pin control mode will decrement the
 * gain by the value set in 'stepSize'.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 *
 * \param txChannel taliseTxChannels_t enum type to select either Tx1 or Tx2
 *  channel for programming.
 *
 * \param txAttenCtrlPin Pointer to taliseTxAttenCtrlPin_t structure that
 * configures the Tx attenuation pin control.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setTxAttenCtrlPin(taliseDevice_t *device,
				  taliseTxChannels_t txChannel, taliseTxAttenCtrlPin_t *txAttenCtrlPin);

/**
 * \brief This API function returns the configuration (Gain steps and the GPIO
 * inputs) for Tx Attenuation pin control.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 *
 * \param txChannel taliseTxChannels_t enum type to select either Tx1 or Tx2
 *  channel for reading configuration from.
 *
 * \param txAttenCtrlPin Pointer to taliseTxAttenCtrlPin_t structure that
 * would contain configuration values for Tx attenuation pin control.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getTxAttenCtrlPin(taliseDevice_t *device,
				  taliseTxChannels_t txChannel, taliseTxAttenCtrlPin_t *txAttenCtrlPin);

/**
 * \brief Writes Talise device registers with settings for the PA Protection feature.
 *
 * This function sets up the PA Protection functionality and enables Tx sample
 * power measurements. It does not enable the ability to change Tx Attenuation
 * automatically.  To enable automatic TxAttenuation increase when the Tx sample
 * average power exceeds a threshold, follow this setup function with the
 * TALISE_enablePaProtection() function.
 *
 * The PA Protection feature allows for error flags to go high if the
 * average TX IQ sample power (before interpolation at JESD204 interface) in
 * the data path exceeds a programmable threshold level based on samples taken
 * in a programmable duration. Also, the peak power measurement mode allows
 * flagging an error when the instantaneous power exceeds a threshold more than a
 * programmable count value(Silicon Rev B1 0-30, Silicon Rev C0 0-31) within the
 * average duration.
 *
 * \pre Complete normal Talise initialization and init cals before running this function.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * peakThreshold Value |  dBFS threshold = 10 * Log10(peakThresholdVal/128)
 * --------------------|----------------------------------------------------------
 *                0    | Invalid
 *                1    | -21.07 dBFS
 *                2    | -18.06 dBFS
 *                3    | -16.30 dBFS
 *                4    | -15.05 dBFS
 *                5    | -14.08 dBFS
 *                6    | -13.29 dBFS
 *                7    | -12.62 dBFS
 *                8    | -12.04 dBFS
 *                ...  | Calulate with EQ above
 *                16   | -9.03 dBFS
 *                ...  | ...
 *                32   | -6.02 dBFS
 *                ...  | ...
 *                64   | -3.01 dBFS
 *                ...  | ...
 *                128  | 0 dBFS
 *                ...  | ...
 *                255  | 3.01 dBFS
 *
 *  Tx1PowerThreshold Value |  dBFS threshold = 10 * Log10(tx1PowerThresholdVal/4096)
 * -------------------------|----------------------------------------------------------
 *                     0    | Invalid
 *                     1    | -36.12 dBFS
 *                     2    | -33.11 dBFS
 *                     3    | -31.35 dBFS
 *                     4    | -30.10 dBFS
 *                     5    | -29.13 dBFS
 *                     6    | -28.34 dBFS
 *                     7    | -27.67 dBFS
 *                     8    | -27.09 dBFS
 *                     ...  | Calulate with EQ above
 *                     16   | -24.08 dBFS
 *                     ...  | ...
 *                     32   | -21.07 dBFS
 *                     ...  | ...
 *                     64   | -18.06 dBFS
 *                     ...  | ...
 *                     128  | -15.05 dBFS
 *                     ...  | ...
 *                     256  | -12.04 dBFS
 *                     ...  | ...
 *                     512  | -9.03 dBFS
 *
 * \param device Pointer to the device settings structure
 * \param txPaProtectCfg Structure holding the setup values for the Tx PA Protection feature
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setPaProtectionCfg(taliseDevice_t *device,
				   taliseTxPaProtectCfg_t *txPaProtectCfg);

/**
 * \brief Reads Talise device registers to retrieve settings for the PA Protection feature.
 *
 * The PA Protection feature allows for error flags to go high if the
 * average TX IQ sample power (before interpolation at JESD204 interface) in
 * the data path exceeds a programmable threshold level based on samples taken
 * in a programmable duration. Also, the peak power measurement mode allows
 * flagging an error when the instantaneous power exceeds a threshold more than a
 * programmable count value within the average duration.
 *
 * \pre Complete normal Talise initialization and init cals before running this function.
 *      Also, call TALISE_setPaProtectionCfg() to set initial configuration.
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param txPaProtectCfg Structure to return the setup values for the Tx PA Protection feature
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getPaProtectionCfg(taliseDevice_t *device,
				   taliseTxPaProtectCfg_t *txPaProtectCfg);

/**
 * \brief Enables/Disables ability of PA power protection to override Tx Attenuation.
 *
 * Currently not implemented. This function is a placeholder.  Currently,
 * this function will not enable the TxAttenuation change if the PA error flags
 * are asserted.
 *
 * \pre Complete normal Talise initialization and init cals, and call
 *      TALISE_setPaProtectionCfg() before using TALISE_enablePaProtection()
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param enableTxAttenCtrl 1=Allow PA Protection to reduce Tx output power when
 *        Tx sample power exceeds set thresholds, 0=Disable Tx PA protection
 *        control over TxAttenuation.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enablePaProtection(taliseDevice_t *device,
				   uint8_t enableTxAttenCtrl);

/**
 * \brief Read back Tx average IQ sample power (input samples at JESD204 port)
 *        based on settings from TALISE_setPaProtectionCfg()
 *
 * channelPower return value is not returned in dBFS.  To calculate the dBFS value
 * perform the follow calculation: TxAveragePower_dBFS = 10 * log10(channelPower/2^15)
 *
 * \pre Complete normal Talise initialization and init cals, and call
 *      TALISE_setPaProtectionCfg() before using TALISE_getTxSamplePower()
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param txChannel TAL_TX1(1)=Read power from Tx1, TAL_TX2(2)=Read power from Tx2
 * \param channelPower Most recent tx average IQ sample power for specified tx channel (linear power with 32768 = 0dB)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getTxSamplePower(taliseDevice_t *device,
				 taliseTxChannels_t txChannel, uint16_t *channelPower);

/**
 * \brief Read back Tx PA Protection error IRQ flags
 *
 * If Tx PA Protection is configured and enabled, the error flags per channel
 * can be read back to determine which transmitter exceeded the set peak or
 * average power threshold.  The PA protection error flags can also be set
 * to assert the GP interrupt pin for immediate feedback to the BBIC when the
 * Tx power into the PA is of concern.
 *
 * \pre Complete normal Talise initialization and init cals, and call
 *      TALISE_setPaProtectionCfg() first
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 * \param errorFlags Returns error flag per channel.  Bit0=1 if Tx1 sample
 *        power exceeded the set threshold. Bit1=1 if Tx2 sample power exceeded
 *        the set threshold.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getPaProtectErrorFlags(taliseDevice_t *device,
				       uint8_t *errorFlags);

/**
 * \brief Clears both Tx1 and Tx2 PA Protection error IRQ flags
 *
 * If Tx PA Protection is configured and enabled, the error flags per channel
 * can be read back using the TALISE_getPaProtectErrorFlags function. The
 * error flags will stay asserted until manually cleared.  They can be cleared
 * either using this function or the GP interrupt handler function.
 *
 * If the PA protection error flags are routed to the GP interrupt pin, it is
 * preferred that the GP interrupt handler function be used to clear the error
 * flags. If PA protection is not routed to assert the GP interrupt pin, this
 * function many be used to clear the PA protection error flags.
 *
 * Calling TALISE_setPaProtectionCfg() will also clear the error flags at the
 * end of the function to make sure PA protection is enabled with the error
 * flags in a known state.
 *
 * \pre Complete normal Talise initialization and init cals, and call
 *      TALISE_setPaProtectionCfg() first
 *
 * \dep_begin
 * \dep{device->halDevInfo}
 * \dep_end
 *
 * \param device Pointer to the device settings structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Invalid parameter passed
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_clearPaProtectErrorFlags(taliseDevice_t *device);

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

#endif /* TALISE_TX_H_ */
