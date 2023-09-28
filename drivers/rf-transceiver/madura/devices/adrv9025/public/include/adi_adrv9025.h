/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025.h
* \brief Contains top level Adrv9025 related function prototypes for
*        adi_adrv9025.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_H_
#define _ADI_ADRV9025_H_

#include "adi_adrv9025_types.h"
#include "adi_common_macros.h"
#include "adi_adrv9025_user.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Performs a Hardware Initialization for ADRV9025 Device.
*
* This API shall call the ADI HAL function adi_hal_HwOpen for
* ADRV9025 Hardware initialization.  This HAL function initializes all the external
* hardware blocks required in the operation of the Adrv9025 device.
* This API will also set the HAL timeout limit for the HAL driver as per API
* requirements.
*
* \pre This function may be called after device->common.devHalInfo has been initialized with
* user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to ADRV9025 device data structure. Adrv9025Device_t member
*               devHalInfo shall be initialized with all the required information to initialize
*               external Hardware required for ADRV9025 operation for example
*               power, pull ups, SPI master etc
* \param spiSettings Pointer to adi_adrv9025_SpiSettings_t structure that contains SPI
*               configuration parameters.  These settings get copied into the device structure.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_HwOpen(adi_adrv9025_Device_t*      device,
                            adi_adrv9025_SpiSettings_t* spiSettings);

/**
* \brief Performs a hardware shutdown for ADRV9025 Device.
*
* This API shall call the ADI HAL function adi_hal_HwClose for
* ADRV9025 Hardware shutdown.  This HAL function shuts down all the external
* hardware blocks required in the operation of the Adrv9025 device.
*
* \pre This function may be called any time after device->common.devHalInfo has been
* initialized with user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to ADRV9025 device data structure. Adrv9025Device_t member
* devHalInfo shall be initialized with all the required information to initialize
* supporting Hardware for ADRV9025 operation for example
* power, pull ups, SPI master etc
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_HwClose(adi_adrv9025_Device_t* device);

/**
* \brief Performs a hard reset on the ADRV9025 DUT (Toggles RESETB pin on device)
*
* Toggles the ADRV9025 devices RESETB pin.  Only resets the device with
* the SPI chip select indicated in the device->spiSettings structure.
*
* \pre This function may be called any time after device->common.devHalInfo has been
* initialized with user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to ADRV9025 device data structure containing settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_WARN_RESET_GPIO Recovery action to warn user a GPIO reset may be required
* \retval ADI_COMMON_ACT_ERR_RESET_FEATURE Recovery action for user GPIO reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_HwReset(adi_adrv9025_Device_t* device);

/**
* \brief Initializes the ADRV9025 device based on the desired device settings.
*
* This function initializes the ADRV9025 device, digital clocks,
* JESD204b settings, FIR Filters, digital filtering.  It does not load the ARM
* or perform any of the ARM init calibrations.  It leaves the
* ADRV9025 in a state ready for multichip sync (which can bring up the JESD204 links), the
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
* \param device Pointer to ADRV9025 device data structure
* \param init Pointer to ADRV9025 initialization settings structures
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_Initialize(adi_adrv9025_Device_t* device,
                                adi_adrv9025_Init_t*   init);

/**
* \brief API To safely Shutdown Adrv9025
*
* The User should call this function to safely shutdown ADRV9025 Device.
* The function performs a hardware reset to reset the ADRV9025 Device into a safe
* state for shutdown or re-initialization.
*
* \pre This function may be called at any time but not before device->common.devHalInfo
* has been configured with user device settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_Shutdown(adi_adrv9025_Device_t* device);

/**
* \brief Sets up the transceiver to listen for incoming SYSREF pulses to synchronize the internal clock tree.
*
*  When working with multiple transceivers or even only one transceiver that
*  requires deterministic latency between the Tx and observation and or main
*  Rx JESD204B data path, Multichip sync is necessary.  This function should
*  be run after all transceivers have finished the Adrv9025_initialize(),
*  and after Adrv9025_ArmStart().
*
*  This function will reset the MCS state
*  machine in the ADRV9025 device.  Calling the function again, will reset
*  the state machine and expect the MCS sequence to start over. Since clocks
*  will be adjusted, the transceiver should not be transmitting or receiving
*  when this function is called.
*
*  Typical sequence:
*  1) Initialize all ADRV9025 devices in system using Adrv9025_initialize()
*  2) Run adi_adrv9025_MultichipSyncSet(). Make sure enableSync is 1.
*  3) Send at least 4 SYSREF pulses, and verify mcsStatus is set for each
*     MCS step
*  4) Use adi_adrv9025_MultichipSyncStatusGet() to verify MCS.
*  4) Run adi_adrv9025_MultichipSyncSet(). Make sure enableSync is 0
*     to indicate end of MCS sequence.
*  5) Continue with init sequnece ... int cals, etc
*
* \pre This function is called after the device has been initialized and PLL lock status has
* been verified.  ARM must be running before calling this function.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is a pointer to the device settings structure
* \param enableSync is a 0/1 variable used to indicate MultiChipSync start/stop.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_MultichipSyncSet(adi_adrv9025_Device_t* device,
                                      uint8_t                enableSync);

/**
* \brief Reads back the multi-chip sync status
*
*  After running adi_adrv9025_MultichipSyncSet(), this function can be used
*  to verify that all the SYSREF pulses were received by the transceiver.
*
*  Typical sequence:
*  1) Initialize all ADRV9025 devices in system using adi_adrv9025_initialize()
*  2) Run adi_adrv9025_MultichipSyncSet()
*  3) Send at least 4 SYSREF pulses, and verify mcsStatus is set for each
*     MCS step
*  4) Run adi_adrv9025_MultichipSyncStatusGet() verify mcsStatus
*  5) Continue with init sequnece ... int cals, etc
*
*  mcsStatus | bit Description
* -----------|--------------------------------------------------------
*       [0]  | MCS JESD SYSREF Status (1 = sync occurred)
*       [1]  | MCS Digital Clocks Sync Status (1 = sync occurred)
*       [2]  | MCS Clock Gen Sync Status (1 = sync occurred)
*       [3]  | MCS CLKPLL SDM Sync Status (1 = sync occurred)
*       [4]  | MCS Device Clock divider Sync Status (1 = sync occurred)
*
* \pre This function is called after the device has been initialized and PLL lock status has
* been verified.  ARM must be running before using the MCS functions.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device A pointer to the device settings structure
* \param mcsStatus Returns the mcsStatus word described in the table above.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_MultichipSyncStatusGet(adi_adrv9025_Device_t* device,
                                            uint32_t*              mcsStatus);

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
* This function uses Adrv9025_TxProfileVerify(), Adrv9025_RxProfileVerify(), and
* Adrv9025_OrxProfileVerify() as helper functions.
*
* \dep_begin
* \dep{device->devStateInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param init Pointer to ADRV9025 initialization settings structures
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ProfilesVerify(adi_adrv9025_Device_t* device,
                                    adi_adrv9025_Init_t*   init);

/**
* \brief Sets the ADRV9025 device SPI settings (3wire/4wire, msbFirst, etc).
*
* This function will use the settings in the passed spi structure parameter
* to set SPI stream mode, address auto increment direction, msbFirst/lsbfirst,
* and 3wire/4wire mode for the ADRV9025 SPI controller.  The ADRV9025 device
* always uses SPI MODE 0 (CPHA=0, CPOL=0) and a 16-bit instruction word.
*
* \pre This function is a helper function and does not need to be called
*      directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init->spiSettings->MSBFirst}
* \dep{init->spiSettings->enSpiStreaming}
* \dep{init->spiSettings->autoIncAddrUp}
* \dep{init->spiSettings->fourWireMode}
* \dep{init->spiSettings->cmosPadDrvStrength}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param spi Pointer to ADRV9025 SPI controller settings - not platform hardware SPI settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SpiCfgSet(adi_adrv9025_Device_t*      device,
                               adi_adrv9025_SpiSettings_t* spi);

/**
* \brief Gets the ADRV9025 device SPI settings (3wire/4wire, msbFirst, etc).
*
* This function will use the settings in the passed spi structure parameter
* to get SPI stream mode, address auto increment direction, msbFirst/lsbfirst,
* and 3wire/4wire mode for the ADRV9025 SPI controller.  The ADRV9025 device
* always uses SPI MODE 0 (CPHA=0, CPOL=0) and a 16-bit instruction word.
*
* \pre This function is a helper function and does not need to be called
*      directly by the user.
* \pre Can only call this function if config has been set or SpiVerify
*      returns with no errors
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init->spiSettings->MSBFirst}
* \dep{init->spiSettings->enSpiStreaming}
* \dep{init->spiSettings->autoIncAddrUp}
* \dep{init->spiSettings->fourWireMode}
* \dep{init->spiSettings->cmosPadDrvStrength}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param spi Pointer to ADRV9025 SPI controller settings - not platform hardware SPI settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SpiCfgGet(adi_adrv9025_Device_t*      device,
                               adi_adrv9025_SpiSettings_t* spi);

/**
* \brief Verifies whether the existing SPI settings work.
*
* This function checks the SPI settings set through adi_adrv9025_setSpiSettings for
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
*      directly by the user.
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for full chip reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_SpiVerify(adi_adrv9025_Device_t* device);

/**
* \brief Get API version number
*
* This function reads back the version number of the API
*
* \param device Pointer to the ADRV9025 data structure
* \param apiVersion Pointer to structure where API version information is returned
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION function completed successfully, no action required *
*/
int32_t adi_adrv9025_ApiVersionGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_ApiVersion_t* apiVersion);

/**
* \brief Reads back the silicon revision for the ADRV9025 Device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param siRevision Return value of the ADRV9025 silicon revision
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION function completed successfully, no action required *
*/
int32_t adi_adrv9025_DeviceRevGet(adi_adrv9025_Device_t* device,
                                  uint8_t*               siRevision);

/**
* \brief Reads back the Product ID for the ADRV9025 Device
*
*  productId  |  Description
* ------------|---------------
*       1     |  ...
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param productId Return value of the ADRV9025 product Id
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION function completed successfully, no action required *
*/
int32_t adi_adrv9025_ProductIdGet(adi_adrv9025_Device_t* device,
                                  uint8_t*               productId);

/**
* \brief Reads back the device hardware capability
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param devCapability Structure pointer to return device hardware capability
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION function completed successfully, no action required
*/
int32_t adi_adrv9025_DeviceCapabilityGet(adi_adrv9025_Device_t*          device,
                                         adi_adrv9025_CapabilityModel_t* devCapability);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_H_ */
