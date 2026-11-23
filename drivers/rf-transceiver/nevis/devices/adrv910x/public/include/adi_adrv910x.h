/**
 * \file
 * \brief ADRV910X core functionality
 * 
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef _ADI_ADRV910X_H_
#define _ADI_ADRV910X_H_


#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

#include "adi_adrv910x_types.h"
#include "adi_common_types.h"
#include "device_profile_top_t.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NEVIS_PS2
/**
 * \brief Performs a Hardware Initialization for ADRV910X Device.
 *
 * This API calls the ADI HAL function adi_hal_HwOpen for
 * ADRV910X Hardware initialization.  This HAL function initializes all the external
 * hardware blocks required in the operation of the ADRV910X device.
 * This API sets the HAL timeout limit for the HAL driver as per API
 * requirements.
 *
 * \pre This function may be called after device->common.devHalInfo has been initialized with
 * user values
 *
 * \param[in] adrv910x		Context variable - Pointer to ADRV910X device data structure. Adrv910XDevice_t member
 *							devHalInfo must be initialized with all the required information to initialize
 *							external Hardware required for ADRV910X operation for example
 *							power, pull ups, SPI master etc
 * \param[in] spiSettings	Pointer to ADRV910X SPI controller settings - not platform hardware SPI settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_HwOpen(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_SpiSettings_t *spiSettings);

/*********************************************************************************************************/
/**
 * \brief Performs a Hardware Initialization for ADRV910X Device without resetting the part.
 *
 * This API calls the ADI HAL function adi_hal_HwOpen for
 * ADRV910X Hardware initialization.  This HAL function initializes all the external
 * hardware blocks required in the operation of the ADRV910X device.
 * This API sets the HAL timeout limit for the HAL driver as per API
 * requirements.
 * This API will not reset the hardware.
 *
 * \pre This function may be called after device->common.devHalInfo has been initialized with
 * user values
 *
 * \param[in] adrv910x	Context variable - Pointer to ADRV910X device data structure. Adrv910XDevice_t member
 *						devHalInfo must be initialized with all the required information to initialize
 *						external Hardware required for ADRV910X operation for example
 *						power, pull ups, SPI master etc
 * \param[in] spiSettings	Pointer to ADRV910X SPI controller settings - not platform hardware SPI settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_HwOpenNoReset(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_SpiSettings_t *spiSettings);

/**
 * \brief Performs a hardware shutdown for ADRV910X Device.
 *
 * This API shall call the ADI HAL function adi_hal_HwClose for
 * ADRV910X Hardware shutdown.  This HAL function shuts down all the external
 * hardware blocks required in the operation of the ADRV910X device.
 *
 * \pre This function may be called any time after device->common.devHalInfo has been
 * initialized with user values
 *
 * \param[in] adrv910x	Context variable - Pointer to ADRV910x device data structure. Adrv910XDevice_t member
 *						devHalInfo must be initialized with all the required information to initialize
 *						external Hardware required for ADRV910X operation for example
 *						power, pull ups, SPI master etc
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_HwClose(adi_adrv910x_Device_t *adrv910x);

/**
 * \brief Performs a hard reset on the ADRV910X DUT (Toggles RESETB pin on device)
 *
 * Toggles the ADRV910X devices RESETB pin.  Only resets the device with
 * the SPI chip select indicated in the device->spiSettings structure.
 *
 * \pre This function may be called any time after device->common.devHalInfo has been
 * initialized with user values
 *
 * \param[in] adrv910x Context variable - Pointer to ADRV910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_HwReset(adi_adrv910x_Device_t *adrv910x);
	
/**
 * \brief API To safely Shutdown ADRV910X
 *
 * The User should call this function to safely shutdown ADRV910X Device.
 * The function performs a hardware reset to reset the ADRV910X Device into a safe
 * state for shutdown or re-initialization.
 *
 * \pre This function may be called at any time but not before device->common.devHalInfo
 * has been configured with user device settings
 *
 * \param[in] adrv910x		Context variable - Pointer to the ADRV910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Shutdown(adi_adrv910x_Device_t *adrv910x);

/**
 * \brief Sets the ADRV910X device SPI settings (3wire/4wire, msbFirst, etc).
 *
 * This function will use the settings in the passed SPI structure parameter
 * to set SPI stream mode, address auto increment direction, msbFirst/lsbFirst,
 * and 3wire/4wire mode for the ADRV910X SPI controller.  The ADRV910X device
 * always uses SPI MODE 0 (CPHA=0, CPOL=0) and a 16-bit instruction word.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is a helper function and does not need to be called
 *      directly by the user.
 *
 * \param[in] adrv910x		Context variable - Pointer to the ADRV910X device data structure
 * \param[in] spi			Pointer to ADRV910X SPI controller settings - not platform hardware SPI settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_spi_Configure(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_SpiSettings_t *spi);

/**
 * \brief Gets the ADRV910X device SPI settings (3wire/4wire, msbFirst, etc).
 *
 * This function will use the settings in the passed SPI structure parameter
 * to get SPI stream mode, address auto increment direction, msbFirst/lsbFirst,
 * and 3wire/4wire mode for the ADRV910X SPI controller.  The ADRV910X device
 * always uses SPI MODE 0 (CPHA=0, CPOL=0) and a 16-bit instruction word.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is a helper function and does not need to be called
 *      directly by the user.
 * \pre Can only call this function if config has been set or SpiVerify
 *      returns with no errors
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device data structure
 * \param[out] spi			Pointer to ADRV910X SPI controller settings - not platform hardware SPI settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_spi_Inspect(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_SpiSettings_t *spi);

/**
 * \brief Verifies whether the existing SPI settings work.
 *
 * This function checks the SPI settings set through adi_adrv910x_setSpiSettings for
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
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is a helper function and does not need to be called
 *      directly by the user.
 *
 * \param[in] adrv910x		Context variable - Pointer to the ADRV910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_spi_Verify(adi_adrv910x_Device_t *adrv910x);

#endif /* NEVIS_PS2 */
/**
 * \brief Get API version number
 * 
 * \param[in]  adrv910x        Context variable
 * \param[out] apiVersion   The current version of the ADRV910X API
 * 
 * \returns 0 to indicate success
 */
int32_t adi_adrv910x_apiVersion_get(adi_adrv910x_Device_t *adrv910x, adi_common_ApiVersion_t *apiVersion);

/**
 * \brief Reads back the part number for the ADRV910X variant device
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x         Context variable - Pointer to the ADRV910X device data structure
 * \param[out] partNumber       Return value of the ADRV910X part number
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_PartNumber_Get(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_PartNumber_e *partNumber);

#ifndef NEVIS_PS2

/**
 * \brief Initializes the ADRV910X device based on the desired device settings.
 *
 * This function initializes the ADRV910X device, analog clocks.
 * It does not load the ARM or perform any of the ARM init calibrations.  It leaves the
 * ADRV910X in a state ready for the ARM to be loaded, and the init calibrations run.
 *
 * \pre This function is the very first API to be called by the user to configure the device
 * after all dependent data structures have been initialized
 *
 * \param[in] adrv910x						Context variable - Pointer to the ADRV910X device data structure
 * \param[in] profile                       Pointer to ADRV910X profile settings structures
 * \param[in] adrv910xDeviceClockOutDivisor ADRV910X device clock output divisor; An enum type ranging from 0 to 6,
 *                                          the divisor value will be 2^N (1, 2, 4, 8, 16, 32, 64)"
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_InitAnalog(adi_adrv910x_Device_t *adrv910x,
                                deviceProfile_t *profile,
                                adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor);

#else /* NEVIS_PS2 */

/**
 * \brief Initializes the ADRV910X device based on the desired device settings.
 *
 * This function initializes the ADRV910X device structure.  The device structure
 * is used for validation if API validation is enabled.  It also contains error
 * information for failed API calls.  If profile is null the adrv910x device strcucture
 * is left un-initialized.  In this case validation for some API functions may not be
 * possible.
 *
 * \pre This function is the very first API to be called by the user to configure the device
 * after all dependent data structures have been initialized
 *
 * \param[in] adrv910x			    Context variable - Pointer to the ADRV910X device data structure
 * \param[in] profile                       Pointer to ADRV910X profile settings structures
 * \param[in] timerCallback                 Pointer to the timer callback function, may be NULL to use default TIMER3
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_InitDevPS2(adi_adrv910x_Device_t *adrv910x,
                                deviceProfile_t *profile, halTimer timerCallback);

#endif /* NEVIS_PS2 */
/**
* \brief Configure device clock output divider
*
* This function initializes the ADRV910X device output clock divider
*
*
* \param[in] adrv910x						Context variable - Pointer to the ADRV910X device data structure
* \param[in] adrv910xDeviceClockOutDivisor ADRV910X device clock output divisor; An enum type ranging from 0 to 6,
*                                          the divisor value will be 2^N (1, 2, 4, 8, 16, 32, 64)"
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv910x_SetDeviceClockOutDivider(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor);
	
#ifdef __cplusplus
}
#endif

#endif // !_ADI_ADRV910X_H_
