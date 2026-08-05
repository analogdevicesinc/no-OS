/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by cn0565 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param cn0565_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param cn0565_spi_extra_ip = {
	.chip_select_port = SPI_CS_PORT,
	.get_input_clock = HAL_RCC_GetPCLK1Freq,
};

struct stm32_gpio_init_param cn0565_reset_gpio_extra_ip = {
	.mode = GPIO_MODE_OUTPUT_PP,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_gpio_init_param cn0565_gp0_gpio_extra_ip = {
	.mode = GPIO_MODE_INPUT,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_gpio_irq_init_param cn0565_gpio_irq_extra_ip = {
	.port_nb = 6, /* port G */
};
