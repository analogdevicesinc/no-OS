/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-ad74416h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
	ad74416h_ip.spi_ip = ad74416h_spi_ip;
	ad74416h_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	int ret;

	stm32_init();

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ad74416h_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	return example_main();
}
