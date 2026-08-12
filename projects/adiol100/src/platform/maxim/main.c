/***************************************************************************//**
 *   @file   main.c
 *   @brief  ADIOL100 project main entry point (Maxim platform).
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "maxim_uart_stdio.h"
#include "no_os_uart.h"

#ifdef CONFIG_ADIOL100_BASIC_EXAMPLE
extern int basic_example_main(void);
#endif

int main(void)
{
	struct no_os_uart_desc *uart;
	no_os_uart_init(&uart, &adiol100_uart_ip);
	no_os_uart_stdio(uart);

#ifdef CONFIG_ADIOL100_BASIC_EXAMPLE
	return basic_example_main();
#endif
}
