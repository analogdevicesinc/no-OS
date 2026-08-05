/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Mbed platform data used by admt4000 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_gpio_init_param gpio_extra_ip = {
	.mode = GPIO_MODE_OUTPUT_OD,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_gpio_init_param gpio_extra_pp_ip = {
	.mode = GPIO_MODE_OUTPUT_PP,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_gpio_init_param gpio_in_extra_ip = {
	.mode = GPIO_MODE_INPUT,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_spi_init_param spi_extra_ip = {
	.chip_select_port = SPI_CS_PORT,
};

#if (TMC)
struct stm32_spi_init_param spi_tmc_extra_ip = {
	.chip_select_port = TMC_CS_PORT,
};
#endif

struct stm32_i2c_init_param i2c_extra_ip = {
	.i2c_timing = I2C_TIMING
};

struct stm32_gpio_irq_init_param trigger_gpio_irq_ip = {
	.port_nb = 0, /* Port A */
};
