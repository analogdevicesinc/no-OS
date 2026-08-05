/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example eval-adf4377 project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "adf4377.h"

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int example_main()
{
	struct adf4377_dev *dev;
	struct no_os_gpio_desc *sdp_gpio;
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &adf4377_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("Enter basic example \n");

	ret = adf4377_init(&dev, &adf4377_ip);
	if (ret)
		goto error;
	pr_info("ADF4377 initialized\n");

	return ret;
error:
	pr_info("Error!\n");
	return ret;
}
