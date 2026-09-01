/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of ADF5611 project.
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/**
 * @brief Main function execution for STM32 platform.
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret;
	struct no_os_uart_desc *uart_desc;

	adf5611_spi_extra.get_input_clock = HAL_RCC_GetPCLK2Freq;

	stm32_init();

	ret = no_os_uart_init(&uart_desc, &adf5611_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	return example_main();
}
