/***************************************************************************//**
 *   @file   pico/pico_gpio.h
 *   @brief  Header file for pico gpio specifics.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _PICO_GPIO_H_
#define _PICO_GPIO_H_

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include "no_os_gpio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct pico_gpio_desc
 * @brief pico platform specific gpio descriptor
 */
struct pico_gpio_desc {
	/** Pin direction: True if input, false if output */
	bool input_enabled;
};

/**
 * @brief pico platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops pico_gpio_ops;

#endif
