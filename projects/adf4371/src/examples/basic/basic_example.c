/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example adf4371 project
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
	struct adf4371_dev *dev;
	int ret;

	pr_info("Enter basic example \n");

	struct no_os_uart_desc *uart_desc;
	struct no_os_gpio_desc *gpio_desc;

	ret = no_os_uart_init(&uart_desc, &adf4371_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = no_os_gpio_get(&gpio_desc, &adf4371_ce_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_output(gpio_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	ret = adf4371_init(&dev, &adf4371_ip);
	if (ret)
		goto error;

	ret = adf4371_clk_set_rate_chan(dev, 0, 80000000);
	if (ret)
		goto remove_adf4371;

	pr_info("Clock Frequency for RF8 has been successfully set to 80MHz\n");

remove_adf4371:
	adf4371_remove(dev);
error:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
