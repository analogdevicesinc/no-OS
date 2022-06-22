// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise.h
 * \brief Contains top level Talise related function prototypes for
 *        talise.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_H_
#define TALISE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */
/**
 * \brief Performs a Hardware Initialization for Talise Device.
 *
 * This API shall call the ADI HAL function ADIHAL_openHw for
 * Talise Hardware initialization.  This HAL function initializes all the external
 * hardware blocks required in the operation of the Talise device.
 * This API will also set the HAL timeout limit for the HAL driver as per API
 * requirements.
 *
 * \pre This function may be called after device->devHalInfo has been initialized with
 * user values
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to Talise device data structure. taliseDevice_t member
 *               devHalInfo shall be initialized with all the required information to initialize
 *               external Hardware required for Talise operation for example
 *               power, pull ups, SPI master etc
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_GPIO Recovery action for user GPIO reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_openHw(taliseDevice_t *device);

/**
 * \brief Performs a hardware shutdown for Talise Device.
 *
 * This API shall call the ADI HAL function ADIHAL_closeHw for
 * Talise Hardware shutdown.  This HAL function shuts down all the external
 * hardware blocks required in the operation of the Talise device.
 *
 * \pre This function may be called any time after device->devHalInfo has been
 * initialized with user values
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to Talise device data structure. taliseDevice_t member
 * devHalInfo shall be initialized with all the required information to initialize
 * supporting Hardware for Talise operation for example
 * power, pull ups, SPI master etc
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_GPIO Recovery action for user GPIO reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_closeHw(taliseDevice_t *device);

/**
 * \brief Performs a hard reset on the TALISE DUT (Toggles RESETB pin on device)
 *
 * Toggles the Talise devices RESETB pin.  Only resets the device with
 * the SPI chip select indicated in the device->spiSettings structure.
 *
 * \pre This function may be called any time after device->devHalInfo has been
 * initialized with user values
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to Talise device data structure containing settings
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_WARN_RESET_GPIO Recovery action to warn user a GPIO reset may be required
 * \retval TALACT_ERR_RESET_GPIO Recovery action for user GPIO reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_resetDevice(taliseDevice_t *device);

/**
 * \brief Initializes the Talise device based on the desired device settings.
 *
 * This function initializes the Talise device, setting up the CLKPLL, digital clocks,
 * JESD204b settings, FIR Filters, digital filtering.  It does not load the ARM
 * or perform any of the ARM init calibrations. It also sets the Rx Manual gain indexes and
 * TxAttenuation settings to the initial values found in the device data structure.  It leaves the
 * Talise in a state ready for multichip sync (which can bring up the JESD204 links), the
 * ARM to be loaded, and the init calibrations run.
 *
 * \pre This function is the very first API to be called by the user to configure the device
 * after all dependent data structures have been initialized
 *
 * \dep_begin
 * \dep{device (all members)}
 * \dep{init (all members)}
 * \dep_end
 *
 * \param device Pointer to Talise device data structure
 * \param init Pointer to Talise initialization settings structures
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_initialize(taliseDevice_t *device, taliseInit_t *init);

/**
 * \brief API To safely Shutdown Talise
 *
 * The User should call this function to safely shutdown Talise Device.
 * The function performs a hardware reset to reset the Talise Device into a safe
 * state for shutdown or re-initialization.
 *
 * \pre This function may be called at any time but not before device->devHalInfo
 * has been configured with user device settings
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_shutdown(taliseDevice_t *device);

/**
 * \brief Reads back the multi-chip sync status
 *
 * This function returns the status of the Talise MCS state machine via the mcsStatus pointer.
 * The 4 LSBs of the uint8_t value at mcsStatus represent the sync status
 * of JESD, Digital Clocks, CLK PLL and Device Clock divider.

 *  mcsStatus | bit Description
 * -----------|--------------------------------------------------------
 *       [0]  | MCS JESD SYSREF Status
 *       [1]  | MCS Digital Clocks Sync Status
 *       [2]  | MCS CLKPLL SDM Sync Status
 *       [3]  | MCS Device Clock divider Sync Status
 *
 * A bit value of 1 indicates that the sync occured
 * A bit value of 0 indicates that the sync has not occured.
 *
 *
 * \pre This function can be called any time after the device has been initialized and PLL lock status has
 * been verified.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param mcsStatus Returns the mcsStatus word described in the table above.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_getMultiChipSyncStatus(taliseDevice_t *device,
				       uint8_t *mcsStatus);

/**
 * \brief Sets up the chip for multichip sync, and cleans up after MCS.
 *
 *  When working with multiple transceivers or even only one transceiver that
 *  requires deterministic latency between the Tx and observation and or main
 *  Rx JESD204B data path, Multichip sync is necessary.  This function should
 *  be run after all transceivers have finished the TALISE_initialize(),
 *  and before TALISE_initArm().
 *
 *  When the enableMcs parameter = 1, this function will reset the MCS state
 *  machine in the Talise device.  Calling the function again, will reset
 *  the state machine and expect the MCS sequence to start over.
 *
 *  When the enableMcs parameter = 0, the MCS state machine is not changed,
 *  allowing the user to read back the MCS status in the mcsStatus parameter.
 *
 *  Typical sequence:
 *  1) Initialize all Talise devices in system using TALISE_initialize()
 *  2) Run TALISE_enableMultichipSync with enableMcs = 1
 *  3) Send at least 3 SYSREF pulses, and verify mcsStatus is set for each
 *     MCS step
 *  4) Run TALISE_enableMultichipSync with enableMcs = 0 and verify mcsStatus
 *     returns a x0B (CLKPLL in interger mode), or x0F (CLKPLL in fractional
 *     mode).
 *  5) Continue with init sequnece ...Run initARM, loadArm, etc
 *
 *  mcsStatus | bit Description
 * -----------|--------------------------------------------------------
 *       [0]  | MCS JESD SYSREF Status (1 = sync occurred)
 *       [1]  | MCS Digital Clocks Sync Status (1 = sync occurred)
 *       [2]  | MCS CLKPLL SDM Sync Status (1 = sync occurred)
 *       [3]  | MCS Device Clock divider Sync Status (1 = sync occurred)
 *
 * \pre This function is called after the device has been initialized and PLL lock status has
 * been verified
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param enableMcs =1 will enable/reset the MCS state machine
 * \param mcsStatus optional parameter, if pointer is not null, the function will return the mcsStatus word described in the table above.
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableMultichipSync(taliseDevice_t *device, uint8_t enableMcs,
				    uint8_t *mcsStatus);

/**
 * \brief Resets the JESD204 serializer
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_serializerReset(taliseDevice_t *device);

/**
 * \brief Sets up the chip for multichip LOs Phase synchronization
 *
 *  LOs on multiple chips can be phase synchronized to support active
 *  antenna system and beam forming applications.This function should
 *  be run after all transceivers have finished the TALISE_setRfPllFrequency(),
 *  and before TALISE_runInitCals().
 *
 *  When the enableDigTestClk parameter = 1, this function will reset the MCS state
 *  machine in the Talise device and switch the ARM to run on device clock scaled instead of HSDIGCLK
 *
 *  When the enableDigTestClk parameter = 0, this function will enable Mcs Digital Clocks Sync and JESD204 sysref,
 *  switch the ARM back the HSDIGCLK.
 *
 *  Typical sequence:
 *  1) Initialize all Talise devices in system using TALISE_initialize(),load the ARM and stream processor
 *  2) Set the RF PLL frequency with TALISE_setRfPllFrequency
 *  3) Run TALISE_enableMultichipRfLOPhaseSync with enableDigTestClk = 1 before TALISE_runInitCals()
 *  4) Send at least 4 SYSREF pulses
 *  5) Run TALISE_enableMultichipRfLOPhaseSync with enableDigTestClk = 0
 *  6) Send at least 4 SYSREF pulses
 *  7) Continue with init sequence ...Run initCals, bring up JESD204, etc
 *
 * \pre This function is called after the device has been initialized, ARM is loaded and PLL lock status has
 * been verified
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is a pointer to the device settings structure
 * \param enableDigTestClk =1 will enable/reset the MCS state machine and switch between device clock scaled and
 * HSDIGCLK
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_enableMultichipRfLOPhaseSync(taliseDevice_t *device,
		uint8_t enableDigTestClk);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/****************************************************************************
 * Helper functions
 ****************************************************************************
 */
/**
 * \brief Configures one or more FIR filters in the device
 *
 * The device stores up to 5 FIR filter banks (2Rx | 2Obs Rx | Loopback & 2Tx).
 * Rx filters can have 24, 48, or 72 taps.
 * Tx filters can have 20, 40, 60, or 80 taps.
 *
 * The function is not called directly. It is called as a part of the TALISE_initialize() function.
 * The function could be used to change A FIR filter later, but would require calibrations to be rerun.
 *
 * \pre This function is called during device initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise data structure
 * \param filterToProgram Name of the desired filter to program
 * \param firFilter Pointer to the filter to write into the device
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_programFir(taliseDevice_t *device,
			   talisefirName_t filterToProgram, taliseFir_t *firFilter);

/**
 * \brief Calculates high speed digital clocks used in the Talise device
 *
 * This is a helper function called in TALISE_initialize() to calculate the internal
 * digital clocks in Talise for use throughout other API functions.  The calculated
 * values are stored in the Talise API device structure with other other run-time state
 * information for the API.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise device data structure containing settings
 * \param digClocks Pointer to the Talise digital clocks initialization data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_calculateDigitalClocks(taliseDevice_t *device,
				       taliseDigClocks_t *digClocks);

/**
 * \brief Verifies the init structure profiles are valid combinations
 *
 * This function checks that the Rx/Tx/ORx profiles have valid clock rates in
 * order to operate together.  Rx/Tx and ORx share a common high speed digital
 * clock. If an invalid combination of profiles is detected, an error will be
 * returned. If a profile in the init structure is unused, the user should zero
 * out all members of that particular profile structure.  If a Rx/Tx/ORx profile
 * has an IQ rate = 0, it is assumed that the profile is disabled.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * This function uses TALISE_verifyTxProfile(), TALISE_verifyRxProfile(), and
 * TALISE_verifyOrxProfile() as helper functions.
 *
 * \dep_begin
 * \dep{device->devStateInfo}
 * \dep{init-> (most members)}
 * \dep_end
 *
 * \param device Structure pointer to Talise device data structure
 * \param init Pointer to Talise initialization settings structures
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_verifyProfiles(taliseDevice_t *device, taliseInit_t *init);

/**
 * \brief Sets the Talise device SPI settings (3wire/4wire, MSBFirst, etc).
 *
 * This function will use the settings in the passed spi structure parameter
 * to set SPI stream mode, address auto increment direction, MSBFirst/LSBfirst,
 * and 3wire/4wire mode for the Talise SPI controller.  The Talise device
 * always uses SPI MODE 0 (CPHA=0, CPOL=0) and a 16-bit instruction word.
 *
 * \pre This function is a helper function and does not need to be called
 *      directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{init->spiSettings->MSBFirst}
 * \dep{init->spiSettings->enSpiStreaming}
 * \dep{init->spiSettings->autoIncAddrUp}
 * \dep{init->spiSettings->fourWireMode}
 * \dep_end
 *
 * \param device Structure pointer to Talise device data structure
 * \param spi Pointer to Talise SPI controller settings - not platform hardware SPI settings
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_setSpiSettings(taliseDevice_t *device,
			       taliseSpiSettings_t *spi);

/**
 * \brief Verifies whether the existing SPI settings work.
 *
 * This function checks the SPI settings set through TALISE_setSpiSettings for
 * correct functionality. The function performs the following function:
 *
 * 1. Reads readonly register to check SPI read operation.
 * 2. Writes scratchpad register with 10110110, reads back the data
 * 3. Writes scratchpad register with 01001001, reads back the data
 *
 * The function performs the above operation on registers at the lower end of
 * the register address space, and on the upper end of the register address
 * space.
 *
 * \pre This function is a helper function and does not need to be called
 *      directly by the user.  This function is called automatically at
 *      the end of TALISE_setSpiSettings()
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to Talise device data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_RESET_FULL Recovery action for full chip reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_verifySpiReadWrite (taliseDevice_t *device);

/**
 * \brief Sets the CLKPLL output frequency
 *
 * This function updates the Synth and Loop filter settings based on a VCO
 * frequency LUT. The VCO frequency break points for the Synth LUT can be
 * found in an array called vcoFreqArrayMhz.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the TALISE data structure
 * \param clockSettings pointer to clock setting structure holding information about CLKPLL configuration
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for bad wait timer check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required

 */
uint32_t TALISE_initDigitalClocks(taliseDevice_t *device,
				  taliseDigClocks_t *clockSettings);
/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

/**
 * \brief Sets the digital Tx PFIR SYNC clock divider.
 *
 * This function is a helper function.  It is called automatically in
 * TALISE_initialize() and should not need to be called by the BBIC.
 *
 * This function sets the digital clock divider that is used to synchronize the
 * Tx PFIR each time the Tx channel power up.  The Sync clock must
 * be set to the FIRs (output clock / 2) or slower.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param txProfile txProfile settings
 *
 * \retval TALACT_NO_ACTION Function completed successfully
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 */
uint32_t TALISE_setTxPfirSyncClk(taliseDevice_t *device,
				 taliseTxProfile_t *txProfile);

/**
 * \brief Sets the digital Rx PFIR SYNC clock divider.
 *
 * This function is a helper function.  It is called automatically in
 * TALISE_initialize() and should not need to be called by the BBIC.
 *
 * This function sets the digital clock divider that is used to synchronize the
 * Rx/ORx PFIRs each time the channels power up. The Sync clock must
 * be set to the slowest of Rx/ORx PFIR (output clock / 2).
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param rxProfile rxProfile settings
 * \param orxProfile orxProfile settings
 *
 * \retval TALACT_NO_ACTION Function completed successfully
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 */
uint32_t TALISE_setRxPfirSyncClk(taliseDevice_t *device,
				 taliseRxProfile_t *rxProfile, taliseORxProfile_t *orxProfile);

/**
 * \brief Get API version number
 *
 * This function reads back the version number of the API
 *
 * \param device Pointer to the Talise data structure
 * \param siVer A pointer to the current silicon version number.
 * \param majorVer A pointer to the current major version number.
 * \param minorVer A pointer to the current minor version number.
 * \param buildVer A pointer to the current build version number.
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_NO_ACTION function completed successfully, no action required *
 */
uint32_t TALISE_getApiVersion (taliseDevice_t *device, uint32_t *siVer,
			       uint32_t *majorVer, uint32_t *minorVer, uint32_t *buildVer);

/**
 * \brief Reads back the silicon revision for the Talise Device
 *
 * revision's bit  |  Description
 * ----------------|-----------------
 *       3:0       |  minor revision
 *       7:4       |  major revision
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param revision Return value of the Talise silicon revision in hex where
        upper nibble (4 bits) indicates the major revision and the lower nibble
        indicates the minor revision.
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_NO_ACTION function completed successfully, no action required *
 */
uint32_t TALISE_getDeviceRev(taliseDevice_t *device, uint8_t *revision);

/**
 * \brief Reads back the Product ID for the Talise Device
 *
 *  productId  |  Description
 * ------------|---------------
 *       1     |  AD9379   (TDD device)
 *       2     |  AD9378-1 (Rx Only Device)
 *       3     |  AD9378-2 (Tx /ORx Device)
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param productId Return value of the Talise product Id
 *
 * \retval TALACT_WARN_RESET_LOG recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM recovery action for bad parameter check
 * \retval TALACT_NO_ACTION function completed successfully, no action required *
 */
uint32_t TALISE_getProductId(taliseDevice_t *device, uint8_t *productId);

#ifdef __cplusplus
}
#endif

#endif /* TALISE_H_ */
