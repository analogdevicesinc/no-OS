/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example eval-adf4368 project
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
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
	struct adf4368_dev *dev;
	int ret;
	struct no_os_uart_desc *uart_desc;

	pr_info("Enter basic example \n");

	ret = no_os_uart_init(&uart_desc, &adf4368_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adf4368_init(&dev, &adf4368_ip);
	if (ret)
		goto error;

	ret = adf4368_set_rfout(dev, 8000000000);
	if (ret)
		goto remove_adf4368;

	ret = adf4368_set_phase(dev, 4000, true);

remove_adf4368:
	adf4368_remove(dev);
error:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
