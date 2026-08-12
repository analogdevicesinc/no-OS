/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  ADIOL100 project platform-specific parameters (STM32).
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param adiol100_uart_extra = {
	.huart = &huart5,
};

struct stm32_spi_init_param adiol100_spi_extra = {
	.chip_select_port = SPI_CS_PORT,
};

struct stm32_gpio_irq_init_param adiol100_gpio_irq_extra = {
	.port_nb = IRQA_PORT,
};
