/***************************************************************************//**
 *   @file   maxim_gpio.h
 *   @brief  Header file for maxim gpio specifics.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MAXIM_GPIO_H_
#define MAXIM_GPIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "no_os_irq.h"
#include "no_os_gpio.h"
#include "max78000.h"
#include "gpio.h"

#define N_PINS	MXC_CFG_GPIO_PINS_PORT
#define N_PORTS	MXC_CFG_GPIO_INSTANCES

/**
 * @brief maxim platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops max_gpio_ops;
/**
 * @brief maxim platform specific gpio irq platform ops structure
 */
extern const struct no_os_irq_platform_ops max_gpio_irq_ops;

struct max_gpio_init_param {
	/** GPIO's voltage level */
	mxc_gpio_vssel_t vssel;
};

#endif
