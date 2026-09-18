/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for ADAR300X: SPI and power validation.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdio.h>
#include "basic_example.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

int basic_example_main(void)
{
	struct adar300x_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &adar300x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("ADAR300X basic example\n");

	ret = adar300x_init(&dev, &adar300x_ip);
	if (ret) {
		pr_err("adar300x_init failed: %d\n", ret);
		goto error_uart;
	}

	adar300x_remove(dev);

error_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}
