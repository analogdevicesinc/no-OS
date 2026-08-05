/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example EVAL-ADF4030 project
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_uart.h"
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
	struct adf4030_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &adf4030_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("Enter basic example \n");

	ret = adf4030_init(&dev, &adf4030_ip);
	if (ret)
		goto remove_adf4030;

	// Set CH 1 as TX
	ret = adf4030_set_channel_direction(dev, 1, true);
	if (ret)
		goto remove_adf4030;

	// Set CH 2 as TX
	ret = adf4030_set_channel_direction(dev, 2, true);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_tdc_source(dev, 1);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_channel_delay(dev, 2, 200000);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_single_ch_alignment(dev, 2);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_tdc_measurement(dev, 2);
	if (ret)
		goto remove_adf4030;

	int64_t tdc_res;
	ret = adf4030_get_tdc_measurement(dev, &tdc_res);
	if (ret)
		goto remove_adf4030;

	pr_info("tdc_res : %lld\n", tdc_res);

remove_adf4030:
	adf4030_remove(dev);

	if (ret)
		pr_info("Error!\n");
	return ret;
}
