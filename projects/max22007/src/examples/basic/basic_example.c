/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for MAX22007 project
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "common_data.h"
#include "max22007.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct max22007_dev *dev;
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &max22007_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart_desc);

	ret = max22007_init(&dev, max22007_ip);
	if (ret)
		goto error;

	ret = max22007_write_channel_data(dev, 0, 0x28F);
	if (ret)
		goto error;

	ret = max22007_write_channel_data(dev, 3, 0x51E);
	if (ret)
		goto error;

	ret = max22007_write_ldac(dev, 0x09);
	if (ret)
		goto error;

	while (1) {
		// Main loop - DAC channels are now set to test values
		no_os_mdelay(1000);
	}

error:
	pr_info("Error init!\n");
	return ret;
}
