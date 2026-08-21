/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of ad74414h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;
	struct no_os_uart_desc *uart_desc;

	ad74414h_ip.spi_ip = ad74414h_spi_ip;
	ad74414h_spi_extra.get_input_clock = HAL_RCC_GetPCLK2Freq;

	stm32_init();

	ret = no_os_uart_init(&uart_desc, &ad74414h_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	return example_main();
}
