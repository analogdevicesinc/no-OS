/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for lt7170 project.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "lt7170.h"

int example_main()
{
	struct lt7170_dev *dev;
	struct lt7170_status status;
	struct no_os_uart_desc *uart_desc;
	int ret, vals[4];

	ret = no_os_uart_init(&uart_desc, &lt7170_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("Running basic example.\n");

	ret = lt7170_init(&dev, &lt7170_ip);
	if (ret)
		goto exit;

	while (1) {
		ret = lt7170_read_value(dev, LT7170_VIN, &vals[0]);
		if (ret)
			goto exit;

		ret = lt7170_read_value(dev, LT7170_VOUT, &vals[1]);
		if (ret)
			goto exit;

		ret = lt7170_read_value(dev, LT7170_IOUT, &vals[2]);
		if (ret)
			goto exit;

		ret = lt7170_read_value(dev, LT7170_TEMP, &vals[3]);
		if (ret)
			goto exit;

		ret = lt7170_read_status(dev, LT7170_STATUS_ALL_TYPE,
					 &status);
		if (status.vout)
			pr_info("Status vout asserted.\n");
		if (status.iout)
			pr_info("Status iout asserted.\n");
		if (status.input)
			pr_info("Status input asserted.\n");
		if (status.temp)
			pr_info("Status temp asserted.\n");
		if (status.cml)
			pr_info("Status cml asserted.\n");
		if (status.mfr_specific)
			pr_info("Status mfr_specific asserted.\n");

		pr_info("vin = %d mV | vout = %d mV | iout = %d mA | temp = %d C\n",
			vals[0], vals[1], vals[2],
			vals[3] / 1000);

		pr_info("\n");
		no_os_mdelay(500);
	}

exit:
	pr_err("Error code: %d.\n", ret);
	lt7170_remove(dev);
	return ret;
}
