/***************************************************************************//**
 *   @file   chibios/chibios_gpio.h
 *   @brief  Header file for chibios gpio specifics.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CHIBIOS_GPIO_H_
#define CHIBIOS_GPIO_H_

#include <stdint.h>
#include "no_os_gpio.h"
#include "hal.h"

/**
 * @struct chibios_gpio_init_param
 * @brief Structure holding the initialization parameters for chibios os
 */
struct chibios_gpio_init_param {
	/** Port */
	ioportid_t port;
	/** Mask */
	iopadid_t pad;
	/** Mode */
	iomode_t mode;
};

/**
 * @struct chibios_gpio_desc
 * @brief chibios platform specific gpio descriptor
 */
struct chibios_gpio_desc {
	/** Port */
	ioportid_t port;
	/** Mask */
	iopadid_t pad;
	/** Mode */
	iomode_t mode;
};

/**
 * @brief chibios platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops chibios_gpio_ops;

#endif
