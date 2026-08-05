/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by MAX22915 project.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param max22915_uart_extra_ip = {
	.huart = UART_INSTANCE,
};

struct stm32_spi_init_param max22915_spi_extra_ip = {
	.chip_select_port =  GPIO_CS_PORT,
	.get_input_clock = HAL_RCC_GetPCLK2Freq,
};
