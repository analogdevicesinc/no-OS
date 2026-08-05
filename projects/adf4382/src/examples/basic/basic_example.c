/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example eval-adf4382 project
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int example_main()
{
	struct adf4382_dev *dev;
	int ret;

	pr_info("Enter basic example \n");

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adf4382_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adf4382_init(&dev, &adf4382_ip);
	if (ret)
		goto error;

	ret = adf4382_set_rfout(dev, 20000000000);
	if (ret)
		goto remove_adf4382;

	ret = adf4382_set_phase_adjust(dev, 1);

remove_adf4382:
	adf4382_remove(dev);
error:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
