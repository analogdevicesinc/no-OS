/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-adxl38x project.
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

	adxl38x_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	adxl38x_ip.comm_init.spi_init = adxl38x_spi_ip;

	stm32_init();

	ret = no_os_uart_init(&uart_desc, &adxl38x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = example_main();

	no_os_uart_remove(uart_desc);
	return ret;
}

