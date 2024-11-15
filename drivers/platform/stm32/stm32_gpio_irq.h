/***************************************************************************//**
 *   @file   stm32/stm32_gpio_irq.h
 *   @brief  Header file for stm32 gpio irq specifics.
 *   @author Ramona Bolboaca (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_GPIO_IRQ_H
#define STM32_GPIO_IRQ_H

#include "no_os_irq.h"
#include "stm32_hal.h"

/**
 * @struct stm32_gpio_irq_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific GPIO IRQ parameters.
 */
struct stm32_gpio_irq_init_param {
	uint8_t port_nb;
};

/**
 * @struct stm32_gpio_irq_desc
 * @brief stm32 platform specific GPIO IRQ descriptor
 */
struct stm32_gpio_irq_desc {
	/** EXTI line instance */
	EXTI_HandleTypeDef hexti;
	/** Port number */
	uint8_t port_nb;
};

/**
 * @brief stm32 platform specific irq platform ops structure
 */
extern const struct no_os_irq_platform_ops stm32_gpio_irq_ops;

#endif
