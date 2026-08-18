/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by the ADIN1140 project.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param adin1140_uart_extra_ip = {
	.huart = UART_INSTANCE,
};

/* SPI1 kernel clock is sourced from PLL2P (RCC_SPI1CLKSOURCE_PLL2P), not APB1. */
static uint32_t get_spi1_clock(void)
{
	return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1);
}

struct stm32_spi_init_param adin1140_spi_extra_ip = {
	.chip_select_port =  GPIO_CS_PORT,
	.get_input_clock = get_spi1_clock,
};

/* GPIO */
struct stm32_gpio_init_param gpio_init = {
	.mode = GPIO_MODE_INPUT,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_gpio_irq_init_param adin1140_gpio_irq_extra_ip = {
	.port_nb = 3, /* Port D */
};
