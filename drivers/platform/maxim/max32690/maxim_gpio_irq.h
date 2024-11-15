/***************************************************************************//**
 *   @file   maxim_gpio_irq.h
 *   @brief  Header file of GPIO IRQ interface for Maxim platform.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_GPIO_IRQ_H_
#define MAXIM_GPIO_IRQ_H

#include "no_os_irq.h"

/**
 * @brief Maxim platform specific GPIO irq extra param (optional for now)
 */
struct max_gpio_irq_param {
	struct no_os_irq_ctrl_desc *parent;
};

/**
 * @brief maxim platform specific gpio irq platform ops structure
 */
extern const struct no_os_irq_platform_ops max_gpio_irq_ops;

#endif
