/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by ad74414h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param ad74414h_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param ad74414h_spi_extra = {
	.chip_select_port = SPI_CS_PORT,
};
