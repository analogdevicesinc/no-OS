// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_agc.h
 * \brief Contains Talise API AGC function prototypes for talise_agc.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_AGC_H_
#define TALISE_AGC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_agc_types.h"

/**
 * \brief Sets up the device Rx Automatic Gain Control (AGC) registers
 *
 * The instantiated AGC setting structure (taliseAgcCfg_t) must be initialized with
 * valid settings before this function may be used due to the dependencies
 *
 * \pre This function is called only if AGC will be used, and only after the
 * initialization process (TALISE_initialize)
 *
 * NOTE : AGC IIP3 feature cannot be enabled on C0 or lower version of silicon.
 *
 * The agcSlowLoopSettlingDelay member in the taliseAgcCfg_t is range checked
 * in this function.  The minimum value allowed is based on the ratio of the
 * (ADC_CLK / AGG_CLK).  Min agcSlowLoopSettlingDelay = 65 / (ADC_CLK/ AGC_CLK).
 * This minimum range check is a sanity check to ensure AGC stability.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{rxAgcCtrl}
 * \dep{rxAgcCtrl->agcPeak}
 * \dep{rxAgcCtrl->agcPower}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure containing settings
 * \param rxAgcCtrl Pointer to AGC data structure containing settings
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_setupRxAgc(taliseDevice_t *device, taliseAgcCfg_t *rxAgcCtrl);

/**
 * \brief Gets the value of the AGC related registers from the Talise.
 *
 * This function is used to get value of the AGC registers. Reads back main AGC structure and
 * if included in the project sub structures for peak to peak and power measurements
 *
 * \pre This function may be used only if AGC is active
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{rxAgcCtrl (most members)}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param agcCtrl Pointer to the agcCtrl structure. Function will update values in the structure to return the current setup.
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getAgcCtrlRegisters(taliseDevice_t *device,
				    taliseAgcCfg_t *agcCtrl);

/**
 * \brief Gets the value of the AGC Peak related registers from the Talise.
 *
 * This function is used to get value of the AGC peak registers
 *
 * \pre This function may be used only if AGC is active
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{agcPeak-> (most members)}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 * \param agcPeak Pointer to the Talise AGC peak settings data structure
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getAgcPeakRegisters(taliseDevice_t *device,
				    taliseAgcPeak_t *agcPeak);

/**
 * \brief Gets the value of the AGC Power measurement related registers from the Talise.
 *
 * This function is used to get value of the AGC Power registers
 *
 * \pre This function may be used only if AGC is active
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 * \param agcPower Pointer to the Talise AGC power settings data structure
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getAgcPowerRegisters(taliseDevice_t *device,
				     taliseAgcPower_t *agcPower);

/**
 * \brief This function will setup the AGC for the dualband mode.
 *
 * This function compliments the TALISE_setupRxAgc() function to set up the
 * AGC for the dualband mode. The function also sets up the 3.3V GPIO's 7-0,
 * to enable the device to control external LNA's.
 *
 * \pre This function may be used only if AGC is active. This function can be
 * used in radio off state.  Device must be initialized to dual band mode as
 * a part of TALISE_initialize()
 *
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1, Rx2,
 * Rx1 + Rx2 for programming the dualband AGC.
 * \param rxAgcCtrlDualBand Pointer to the Talise AGC dualband settings data
 * structure
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_setupDualBandRxAgc( taliseDevice_t *device,
				    taliseRxChannels_t rxChannel, taliseAgcDualBandCfg_t *rxAgcCtrlDualBand);

/**
 * \brief This function returns the current values of the LNA controls for the
 * dualband mode.
 *
 * This function can be used if the user is not controlling the external LNA's
 * through the 3.3V GPIO pins of the device. The LNA controls are programmed by
 * the user in the dualband LNA gain table. The AGC then indexes to one of the
 * entries in the table based on its algorithm.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1 or Rx2
 * \param rxDualBandLnaControls Pointer to the AGC dualband LNA settings data
 * structure
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_getDualBandLnaControls (taliseDevice_t *device,
					taliseRxChannels_t rxChannel,
					taliseDualBandLnaControls_t *rxDualBandLnaControls);

/**
 * \brief This function sets the min/max gain indexes for AGC in the MAIN RX channel.
 *
 * This function can be used to set the min/max gains of RX channel in runtime.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 * \param rxChannel taliseRxChannels_t enum type to select either Rx1 or Rx2 or both Rx1 and Rx2
 * \param maxGainIndex AGC Max gain index setting
 * \param minGainIndex AGC Min gain index setting
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_setRxAgcMinMaxGainIndex(taliseDevice_t *device,
					taliseRxChannels_t rxChannel, uint8_t maxGainIndex, uint8_t minGainIndex);

/**
 * \brief This function resets all the state machines in the Gain Control Block.
 *
 * resets all state machines within the gain control block to state 0
 * and maximum gain (for slow, fast attack, and hybrid AGC loops)
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI recovery action for SPI reset required
 * \retval TALACT_NO_ACTION function completed successfully, no action required
 */
uint32_t TALISE_resetRxAgc(taliseDevice_t *device);

#ifdef __cplusplus
}
#endif

#endif /* TALISE_AGC_H_ */
