/*******************************************************************************
 *   @file   ftd2xx_gpio.h
 *   @brief  Implementation of GPIO driver for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef FTD2XX_GPIO_H_
#define FTD2XX_GPIO_H_

#include <stdbool.h>

#include "no_os_gpio.h"
#include "ftd2xx.h"

#define FTD2XX_GPIO_PIN(x)		NO_OS_BIT(x)
#define FTD2XX_GPIO_READ_CMD(id)	(0x81 + (id))

#define FTD2XX_MAX_PIN_NB		7
#define FTD2XX_MAX_PORT_NB		4

extern uint8_t ftd2xx_gpio_pins_dir[FTD2XX_MAX_PORT_NB];
extern uint8_t ftd2xx_gpio_pins_val[FTD2XX_MAX_PORT_NB];

/**
 * @brief ftd2xx platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops ftd2xx_gpio_ops;

/**
 * @struct ftd2xx_gpio_init
 * @brief ftd2xx platform specific initialization parameter
 */
struct ftd2xx_gpio_init {
	uint8_t extra_pins_dir;
};

/**
 * @struct ftd2xx_gpio_desc
 * @brief ftd2xx platform specific gpio descriptor
 */
struct ftd2xx_gpio_desc {
	/** Specific device handle */
	FT_HANDLE *ftHandle;
};

#endif /* FTD2XX_GPIO_H*/
