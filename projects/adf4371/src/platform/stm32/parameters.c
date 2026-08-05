/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by adf4371 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct stm32_uart_init_param adf4371_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_gpio_init_param adf4371_gpio_extra_ip = {
	.mode = GPIO_MODE_OUTPUT_PP,
	.speed = GPIO_SPEED_HIGH,
};

struct stm32_spi_init_param adf4371_spi_extra_ip = {
	.chip_select_port = SPI_CS_PORT,
};
