/***************************************************************************//**
 *   @file   mbed_gpio.h
 *   @brief  Header containing extra types required for GPIO interface
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MBED_GPIO_H
#define MBED_GPIO_H

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/

/*
 * Note: The structure members are not strongly typed, as this file is included
 *       in application specific '.c' files. The mbed code structure does not
 *       allow inclusion of mbed driver files (e.g. mbed.h) into '.c' files.
 *       All the members are hence typecasted to mbed specific type during
 *       gpio init and read/write operations.
 **/

/**
* @struct mbed_gpio_init_param
* @brief Structure holding the GPIO init parameters for mbed platform.
*/
struct mbed_gpio_init_param {
	uint8_t pin_mode; 		// GPIO pin mode (PinMode)
};

/**
* @struct mbed_gpio_desc
* @brief GPIO specific descriptor for the mbed platform.
*/
struct mbed_gpio_desc {
	uint8_t direction;
	void *gpio_pin;     	// GPIO pin instance (DigitalIn/DigitalOut)
	uint8_t pin_mode;
};

/**
 * @brief Mbed specific GPIO platform ops structure
 */
extern const struct no_os_gpio_platform_ops mbed_gpio_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /* MBED_GPIO_H */
