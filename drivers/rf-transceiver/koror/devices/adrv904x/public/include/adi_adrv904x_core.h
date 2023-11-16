/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_core.h
* \brief Contains top level ADRV904X related function prototypes for
*        adi_adrv904x_core.c
*
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADI_ADRV904X_CORE_H_
#define _ADI_ADRV904X_CORE_H_

#include "adi_adrv904x_error.h"

/**
 * \brief Acquires the device so that it can only be used only in the context of the calling
 * thread until such time as adi_adrv904x_Unlock is called.
 *
 * Generally there is no need to explicitly lock or unlock a device before or after calling any
 * API function with that device (each API function locks the device internally itself before
 * carrying out it's actions and unlocks the device before returning). Any exceptions to this rule
 * will be clearly documented.
 *
 * Uses adi_common_hal_Lock internally. See that function for important relevant information.
 *
 * \param[in,out] device Context variable -Pointer to ADRV904X device data structure containing settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 **/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Lock(adi_adrv904x_Device_t* const device);

/**
 * \brief Releases a device previously acquired by the calling thread.
 *
 * Only the thread that acquired the device can release the device. Attempting to release a device
 * that has not been acquired by the calling thread results in undefined behavior.
 *
 * Uses adi_common_hal_Unlock internally. See that function for important relevant information.
 *
 * \param[in,out] device Context variable -Pointer to ADRV904X device data structure containing settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 **/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Unlock(adi_adrv904x_Device_t* const device);

/**
* \brief Prepares the ADRV904X device for use.
*
* This function must be called on the device before any other function is called. The
* function must not be called more than once on the same device.
*
* Uses adi_common_hal_HwOpen internally. See that function for important relevant information.
*
* This API shall call the ADI HAL function adi_hal_HwOpen for
* ADRV904X Hardware initialization.  This HAL function initializes all the external
* hardware blocks required in the operation of the ADRV904X device.
* This API will also set the HAL timeout limit for the HAL driver as per API
* requirements.
*
* \pre device->common.devHalInfo must be initialized with user values before this function
* is called.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to ADRV904X device data structure. adi_adrv904x_Device_t member
*               devHalInfo shall be initialized with all the required information to initialize
*               external Hardware required for ADRV904X operation for example
*               power, pull ups, SPI master etc
* \param[in] spiSettings Pointer to adi_adrv9010_SpiSettings_t structure that contains SPI
*               configuration parameters.  These settings get copied into the device structure.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HwOpen(adi_adrv904x_Device_t* const            device,
                                                     const adi_adrv904x_SpiConfigSettings_t* const spiSettings);

/**
* \brief Performs a hardware shutdown for ADRV904X Device.
*
* This API shall call the ADI HAL function adi_hal_HwClose for
* ADRV904X Hardware shutdown.  This HAL function shuts down all the external
* hardware blocks required in the operation of the ADRV904X device.
*
* Uses adi_common_hal_HwClose internally. See that function for important relevant information.
*
* \pre This function may be called any time after device->common.devHalInfo has been
* initialized with user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to ADRV904X device data structure. adi_adrv904x_Device_t member
* devHalInfo shall be initialized with all the required information to initialize
* supporting Hardware for ADRV904X operation for example
* power, pull ups, SPI master etc
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HwClose(adi_adrv904x_Device_t* const device);

/**
*   \brief  Service to perform a HwReset for a device
*
*   Calls the HAL to send the HwReset Signal and 
*       then reload the SPI Configuration for a given device
*
* \pre This function may be called any time after device->common.devHalInfo has been
* initialized with user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to ADRV904X device data structure containing settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HwReset(adi_adrv904x_Device_t* const device);

/**
* \brief Initializes the ADRV904X device based on the desired device settings.
*
* This function initializes the ADRV904X device, digital clocks,
* JESD204b settings, FIR Filters, digital filtering.  It does not load the CPU
* or perform any of the CPU init calibrations.  It leaves the
* ADRV904X in a state ready for multichip sync (which can bring up the JESD204 links), the
* CPU to be loaded, and the init calibrations run.
*
* \pre This function is the very first API to be called by the user to configure the device
* after all dependent data structures have been initialized
*
* \dep_begin
* \dep{device (all members)}
* \dep{init (all members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to ADRV904X device data structure
* \param[in] init Pointer to ADRV904X initialization settings structures
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Initialize(adi_adrv904x_Device_t* const        device,
                                                         const adi_adrv904x_Init_t* const    init);

/**
* \brief API To safely Shutdown ADRV904X
*
* The User should call this function to safely shutdown ADRV904X Device.
* The function performs a hardware reset to reset the ADRV904X Device into a safe
* state for shutdown or re-initialization.
*
* \pre This function may be called at any time but not before device->common.devHalInfo
* has been configured with user device settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to ADRV904X device data structure containing settings
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Shutdown(adi_adrv904x_Device_t* const device);

/**
* \brief Sets up the transceiver to listen for incoming SYSREF pulses to synchronize the internal clock tree.
*
*  When working with multiple transceivers or even only one transceiver that
*  requires deterministic latency between the Tx and observation and or main
*  Rx JESD204B data path, Multichip sync is necessary.  This function should
*  be run after all transceivers have finished the adi_adrv904x_initialize(),
*  and after adi_adrv904x_CpuStart().
*
*  This function will reset the MCS state
*  machine in the ADRV904X device.  Calling the function again, will reset
*  the state machine and expect the MCS sequence to start over. Since clocks
*  will be adjusted, the transceiver should not be transmitting or receiving
*  when this function is called.
*
*  Typical sequence:
*  1) Initialize all ADRV904X devices in system using adi_adrv904x_initialize()
*  2) Run adi_adrv904x_MultichipSyncSet(). Make sure enableSync is 1.
*  3) Send at least 4 SYSREF pulses, and verify mcsStatus is set for each
*     MCS step
*  4) Use adi_adrv904x_MultichipSyncStatusGet() to verify MCS.
*  4) Run adi_adrv904x_MultichipSyncSet(). Make sure enableSync is 0
*     to indicate end of MCS sequence.
*  5) Continue with init sequence ... init cals, etc
*
*
* \pre This function is called after the device has been initialized and PLL lock status has
* been verified.  CPU must be running before calling this function.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -is a pointer to the device settings structure
*
* \param[in]     enableSync is a 0/1 variable used to indicate MultiChipSync start/stop.
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_MultichipSyncSet(adi_adrv904x_Device_t* const    device,
                                                               const uint8_t                   enableSync);
/**
* \brief Sets up the transceiver to perform MCS carrier reconfig.
*
*
*  This function will reconfig the MCS state
*
*
* \pre This function is called when performing MCS reconfig
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -is a pointer to the device settings structure
* \param[in]     mcsMode see adi_adrv904x_McsSyncMode_e
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_MultichipSyncSet_v2(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_McsSyncMode_e mcsMode);

/**
* \brief Reads back the multi-chip sync status
*
*  After running adi_adrv904x_MultichipSyncSet(), this function can be used
*  to verify that all the SYSREF pulses were received by the transceiver.
*
*  Typical sequence:
*  1) Initialize all ADRV904X devices in system using adi_adrv904x_initialize()
*  2) Run adi_adrv904x_MultichipSyncSet()
*  3) Send at least 4 SYSREF pulses, and verify mcsStatus is set for each
*     MCS step
*  4) Run adi_adrv904x_MultichipSyncStatusGet() verify mcsStatus
*  5) Continue with init sequence ... init cals, etc
*
* \pre This function is called after the device has been initialized and PLL lock status has
* been verified. CPU must be running before using the MCS functions.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -A pointer to the device settings structure
* \param[out]    mcsStatus bit0 = 0 - no sync,          bit0 = 1 - sync occurred; 
*                          bit1 = 0 - reconfig no sync, bit1 = 1 - reconfig sync occurred;
*
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_MultichipSyncStatusGet(adi_adrv904x_Device_t* const    device,
                                                                     uint32_t* const                 mcsStatus);
/**
* \brief Verifies the init structure profiles are valid combinations
*
* This function checks that the Rx/Tx/ORx profiles.
*
* \pre This function is a helper function and does not need to be called
*      directly by the user.
*
* \dep_begin
* \dep{device->devStateInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
* \param[in]     init Pointer to ADRV904X initialization settings structures
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ProfilesVerify(adi_adrv904x_Device_t* const        device,
                                                             const adi_adrv904x_Init_t* const    init);

/**
* \brief Sets the ADRV904X device SPI settings (3wire/4wire, msbFirst, etc).
*
* This function will use the settings in the passed spi structure parameter
* to set  msbFirst/lsbFirst and 3wire/4wire mode for the ADRV904X SPI controller.  
* The ADRV904X device always uses SPI MODE 0 (CPHA=0, CPOL=0) and a 16-bit 
* instruction word.
*
* \pre This function is a helper function and does not need to be called
*      directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init->spiSettings->MSBFirst}
* \dep{init->spiSettings->fourWireMode}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
* \param[in]        spiCtrlSettings Pointer to ADRV904X SPI controller settings - not platform hardware SPI settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE, if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiCfgSet(adi_adrv904x_Device_t* const            device,
                                                        const adi_adrv904x_SpiConfigSettings_t* const spiCtrlSettings);

/**
* \brief Gets the ADRV904X device SPI settings (3wire/4wire, msbFirst, etc).
*
* This function will use the settings in the passed spi structure parameter
* to get msbFirst/lsbFirst 3wire/4wire mode for the ADRV904X SPI controller.  The ADRV904X device
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
* \dep{init->spiSettings->fourWireMode}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
* \param[out] spi Pointer to ADRV904X SPI controller settings - not platform hardware SPI settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE, if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiCfgGet(adi_adrv904x_Device_t* const        device,
                                                        adi_adrv904x_SpiConfigSettings_t* const   spi);

/**
* \brief Enables/Disables ADRV904X Aux SPI interface. 
*
* If enabling Aux SPI, selected GPIO pins for interface are acquired and configured for the user. 
* If disabling Aux SPI, pin selections in the AuxSpiCfg input struct are ignored and any device 
* GPIO pins currently selected for the interface are released and set back to default UNUSED. 
* There are a limited set of GPIO pins available for use as Aux SPI Port pin. They are as follows:
* 
*       Aux CSB:    GPIO[3], GPIO[16]
*       Aux CLK:    GPIO[2], GPIO[15]
*       Aux SDIO:   GPIO[0], GPIO[13]
*       Aux SDO:    GPIO[1], GPIO[14], or ADI_ADRV904X_GPIO_INVALID (if user only wants 3-wire mode Aux SPI)
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
* \param[in] config Aux SPI Interface configuration to apply to device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE, if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AuxSpiCfgSet( adi_adrv904x_Device_t* const                device,
                                                            const adi_adrv904x_AuxSpiConfig_t* const    config);

/**
* \brief Retrieves currently programmed Aux SPI interface configuration.
*
*  This configuration includes whether the Aux SPI Interface is currently enabled or disabled. 
*  If AuxSPI is currently disabled, all pins in returned config struct will be set to 
*  ADI_ADRV904X_GPIO_INVALID. If AuxSPI is currently enabled, Aux CSB/CLK/SDIO/SDO pins in returned 
*  config struct will be set to the appropriate GPIO pins. If no GPIO pin is configured for a signal, 
*  the returned value will be ADI_ADRV904X_GPIO_INVALID.
*  
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
* \param[out] config Returned Aux SPI Interface configuration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE, if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AuxSpiCfgGet( adi_adrv904x_Device_t* const        device,
                                                            adi_adrv904x_AuxSpiConfig_t* const  config);

/**
* \brief Sets the ADRV904X device SPI runtime options, eg spi streaming, AHB modes.
*
* This function will return the current SPI runtime options that API functions use when deciding
* how a particular SPI transaction should be completed. For example, if a particular API is preparing
* to write a group of consecutive registers it will attempt to put the part in SPI streaming. However, by clearing
* the allowSpiStreaming bit this will prevent all APIs from using SPI streaming.
* 
* SPI Streaming: This mode will automatically increment the SPI address in between SPI data words. This allows for
*                faster SPI transactions since only a single address set is needed at the start of each transaction.
*                Useful only when large, contiguous registers are accessed.
*                
* AHB Auto Increment: Similar to SPI streaming, this mode automatically increments the register address accessed through
*                     the AHB indirect access (read-only). This allows for faster AHB transactions since only the start
*                     address is required to be written. The SPI address must be set each time the AHB data register is
*                     read, even if SPI Streaming is enabled.
*                     
* AHB FIFO Mode: Only available with SPI Streaming and AHB Auto Increment is enabled. This allows for zero wasted bytes during
*                an AHB indirect access (read-only) by eliminating the need to set the SPI register address each time the
*                AHB data word is read.
*
* \dep_begin
* \dep{spiOptions->allowAhbAutoIncrement}
* \dep{spiOptions->allowAhbSpiFifoMode}
* \dep{spiOptions->allowSpiStreaming}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
* \param[in] spiOptions Pointer to ADRV904X SPI runtime options settings that will return the current device settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE, if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiRuntimeOptionsSet(adi_adrv904x_Device_t* const            device,
                                                                   const adi_adrv904x_SpiOptions_t* const spiOptions);

/**
* \brief Gets the ADRV904X device SPI runtime options, eg spi streaming, AHB modes.
*
* This function will return the current SPI runtime options that API functions use when deciding
* how a particular SPI transaction should be completed. For example, if a particular API is preparing
* to write a group of consecutive registers it will attempt to put the part in SPI streaming. However, by clearing
* the allowSpiStreaming bit this will prevent all APIs from using SPI streaming.
*
* \dep_begin
* \dep{spiOptions->allowAhbAutoIncrement}
* \dep{spiOptions->allowAhbSpiFifoMode}
* \dep{spiOptions->allowSpiStreaming}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
* \param[out] spiOptions Pointer to ADRV904X SPI runtime options settings that will return the current device settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE, if successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiRuntimeOptionsGet(adi_adrv904x_Device_t* const            device,
                                                                   adi_adrv904x_SpiOptions_t* const spiOptions);

/**
* \brief Verifies current SPI Configuration & Platform Driver
*
* This function validates the Platform SPI Driver can work with the SPI Modes configured via
* adi_adrv904x_SpiCfgSet & adi_adrv904x_SpiRuntimeOptionsSet.
*
* Single Register Access
*   1. Reads read only register to check SPI read operation.
*   2. Writes scratchpad register with 10110110, reads back the data
*   3. Writes scratchpad register with 01001001, reads back the data
*
* Multi Register Access (PreMcsInit/Initialize() Is Required to be complete for this step to be executed)
*   1. Write multi register scratchpad, reads back the data
*
*   Depending on configuration provided to adi_adrv904x_SpiRuntimeOptionsSet,
*       can be Streaming or Single Instruction Buffered Used to perform the write.
*
* See adi_adrv904x_user.h for Compile Time Debug Option ADI_ADRV904X_SPI_VERIFY
*   This will enable SpiVerify() in PreMcsInit_NonBroadcast()
*
* \pre This function is a helper function and does not need to be called
*      directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to ADRV904X device data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiVerify(adi_adrv904x_Device_t* const device);

/**
* \brief Get API version number
*
* This function reads back the version number of the API
*
* \param[in,out] device Context variable -Pointer to the ADRV904X data structure
* \param[out] apiVersion Pointer to structure where API version information is returned
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ApiVersionGet(adi_adrv904x_Device_t* const        device,
                                                            adi_adrv904x_Version_t* const    apiVersion);

/**
* \brief Reads back the silicon revision for the ADRV904X Device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to the ADRV904X data structure containing settings
* \param[in,out] siRevision Return value of the ADRV904X silicon revision
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceRevGet(adi_adrv904x_Device_t* const device,
                                                           uint8_t* const siRevision);

/**
* \brief Reads back the Product ID for the ADRV904X Device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to the ADRV904X data structure containing settings
* \param[in,out] productId Return value of the ADRV904X product Id
*
*|       productId  |  Description   |
*|:----------------:|:---------------|
*|          TBD     |     TBD        |

*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ProductIdGet(adi_adrv904x_Device_t* const device,
                                                           uint8_t* const productId);

/**
* \brief Reads back the DeviceCapability for the ADRV904X Device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to the ADRV904X data structure containing settings
* \param[out] devCapability Structure pointer to return device hardware capability
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceCapabilityGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_CapabilityModel_t* const devCapability);

/**
* \brief Configures SPI DOUT pin drive strength. driveStrength is a 3 bit value where 0x00 lowest drive strength and
* 0x03 is highest strength
*
* \deprecated
*
* This function will be called with adi_adrv904x_SpiConfigSettings_t->cmosPadDrvStrength during HW opening.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable -Structure pointer to the ADRV904X data structure containing settings
* \param[in] driveStrength Drive strength value to be set. IMPORTANT: adi_adrv904x_CmosPadDrvStr_e
*     must be used to set these values due to backward compatiblity requirements.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiDoutPadDriveStrengthSet(adi_adrv904x_Device_t* const   device,
                                                                         const uint8_t                  driveStrength);
/**
 * \brief Sets the Hysteresis for the SPI Feature
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] enable Enable Flag
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiHysteresisSet( adi_adrv904x_Device_t* const            device,
                                                                const uint32_t                          enable);

/**
 * \brief Sets the Hysteresis for the All Digital Pin Features
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
 * \param[in] enable Enable Flag
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DigitalHysteresisSet( adi_adrv904x_Device_t* const            device,
                                                                    const uint32_t                          enable);

#endif
