/***************************************************************************//**
 *   @file   altera/altera_gpio.h
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ALTERA_GPIO_H_
#define ALTERA_GPIO_H_

#include "no_os_gpio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Altera platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops altera_gpio_ops;

/**
 * @enum gpio_type
 * @brief Altera platform architecture types
 */
enum gpio_type {
	/** Nios II architecture */
	NIOS_II_GPIO
};

/**
 * @struct altera_gpio_init_param
 * @brief Structure holding the initialization parameters for Altera platform
 * specific GPIO parameters.
 */
struct altera_gpio_init_param {
	/** Altera architecture type */
	enum gpio_type	type;
	/** Device ID */
	uint32_t	device_id;
	/** GPIO base address */
	uint32_t	base_address;
};

/**
 * @struct altera_gpio_desc
 * @brief Altera platform specific GPIO descriptor
 */
struct altera_gpio_desc {
	/** Altera architecture type */
	enum gpio_type	type;
	/** Device ID */
	uint32_t		device_id;
	/** GPIO base address */
	uint32_t base_address;
};

#endif /* ALTERA_GPIO_H_ */
