/***************************************************************************//**
 *   @file   max14906/src/platform/maxim/parameters.c
 *   @brief  Definition of STM32 platform data used by max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct stm32_uart_init_param max14906_uart_extra_ip = {
	.huart = &huart2,
};

#ifdef BASIC_EXAMPLE
struct stm32_gpio_irq_init_param max14906_gpio_irq_extra_ip = {
	.port_nb = 0, /* Port A */
};
#endif

struct stm32_spi_init_param max14906_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};
