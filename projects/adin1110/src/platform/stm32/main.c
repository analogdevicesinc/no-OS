/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of the ADIN1110 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

	stm32_init();

	ret = no_os_uart_init(&uart_desc, &adin1110_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	return example_main();
}
