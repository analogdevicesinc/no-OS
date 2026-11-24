/**
 * \file
 * \brief ADRV910X GPIO header file
 *
 * Copyright (c) 2023 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_GPIO_H_
#define _ADI_ADRV910X_GPIO_H_

#include "adi_adrv910x_gpio_types.h"
#include "adi_adrv910x_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure specified pin as manual input
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] pin       The GPIO pin to configure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_gpio_ManualInput_Configure(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_GpioPin_e pin);

/**
 * \brief Configure the ADRV910X GPIO for the specified signal
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] adrv910x    Context variable - Pointer to the ADRV910X device data structure
 * \param[in] signal      The signal to configure
 * \param[in] gpioConfig  Desired GPIO configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_gpio_Ps1_Configure(adi_adrv910x_Device_t *adrv910x,
                                        adi_adrv910x_GpioSignal_e signal,
                                        adi_adrv910x_GpioCfg_t *gpioConfig);

/**
 * \brief Retrieve the ADRV910X GPIO configuration for the requested signal
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x    Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  signal      The signal for which to retrieve the GPIO configuration
 * \param[out] gpioConfig  The current GPIO configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_gpio_Ps1_Inspect(adi_adrv910x_Device_t *adrv910x,
                                      adi_adrv910x_GpioSignal_e signal,
                                      adi_adrv910x_GpioCfg_t *gpioConfig);

/**
 * \brief Reads the ADRV910X GPIO pin output levels for BITBANG mode
 *
 * This function allows reading the value that the GPIO output pins are
 * set to drive out the pins.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x         Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  pin              The pin for which to get the level
 * \param[out] gpioOutPinLevel  Current level for the output pin
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_gpio_OutputPinLevel_Get(adi_adrv910x_Device_t *adrv910x,
                                             adi_adrv910x_GpioPin_e pin,
                                             adi_adrv910x_GpioPinLevel_e *gpioOutPinLevel);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_GPIO_H_ */
