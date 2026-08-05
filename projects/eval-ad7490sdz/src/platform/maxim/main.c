/********************************************************************************
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-ad7490sdz project.
 *   @author Radu Sabau (radu.sabau@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#include "common_data.h"

extern int example_main();

int main()
{
	struct no_os_uart_desc *uart_desc;
	int ret;

#ifdef IIO_SUPPORT
	return example_main();
#endif

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	return example_main();
}
