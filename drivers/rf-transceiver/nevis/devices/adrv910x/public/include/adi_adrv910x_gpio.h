/**
 * \file
 * \brief ADRV910X GPIO header file
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2023 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910X_GPIO_H_
#define _ADI_ADRV910X_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_gpio_types.h"

#ifndef NEVIS_PS2
/**
 * \brief Configure specified pin as manual input
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv910x	Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] pin       The GPIO pin to configure
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_gpio_ManualInput_Configure(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_GpioPin_e pin);

#endif /* NEVIS_PS2 */
/**
	* \brief Configure the ADRV910X GPIO for the specified signal
	*
	* \note Message type: \ref timing_mailbox "Mailbox command"
	*
	* \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
	*
	* \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure containing settings
	* \param[in] signal        The signal to configure
	* \param[in] gpioConfig    Desired GPIO configuration
	*
	* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	*/
int32_t adi_adrv910x_gpio_Ps1_Configure(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_GpioSignal_e signal,adi_adrv910x_GpioCfg_t *gpioConfig);
    
/**
	* \brief Retrieve the ADRV910X GPIO configuration for the requested signal
	*
	* \note Message type: \ref timing_mailbox "Mailbox command"
	*
	* \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
	*
	* \param[in]  adrv910x         Context variable - Pointer to the ADRV910X device data structure containing settings
	* \param[in]  signal           The signal for which to retrieve the GPIO configuration
	* \param[out] gpioConfig       The current GPIO configuration
	*
	* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	*/
int32_t adi_adrv910x_gpio_Ps1_Inspect(adi_adrv910x_Device_t *adrv910x,adi_adrv910x_GpioSignal_e signal,adi_adrv910x_GpioCfg_t *gpioConfig);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_GPIO_H_ */