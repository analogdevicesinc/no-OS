/***************************************************************************//**
*   @file   basic_example.c
*   @brief  Basic example source file for lt3074 project.
*   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "lt3074.h"

int example_main()
{
	struct lt3074_dev *dev;
	struct lt3074_status status;
	struct no_os_uart_desc *uart_desc;
	int ret;
	int vals[5];

	ret = no_os_uart_init(&uart_desc, &lt3074_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("Running basic example.\n");

	ret = lt3074_init(&dev, &lt3074_ip);
	if (ret)
		goto exit;

	ret = lt3074_vout_margin(dev, LT3074_MARGIN_PERCENTAGE_10,
				 LT3074_MARGIN_PERCENTAGE_10);
	if (ret)
		goto exit;

	ret = lt3074_set_limit(dev, LT3074_VIN_OV_WARN_LIMIT_TYPE, 4500);
	if (ret)
		goto exit;

	while (1) {
		ret = lt3074_read_value(dev, LT3074_VIN, &vals[0]);
		if (ret)
			goto exit;

		ret = lt3074_read_value(dev, LT3074_VOUT, &vals[1]);
		if (ret)
			goto exit;

		ret = lt3074_read_value(dev, LT3074_VBIAS, &vals[2]);
		if (ret)
			goto exit;

		ret = lt3074_read_value(dev, LT3074_IOUT, &vals[3]);
		if (ret)
			goto exit;

		ret = lt3074_read_value(dev, LT3074_TEMP, &vals[4]);
		if (ret)
			goto exit;

		ret = lt3074_read_status(dev, LT3074_STATUS_ALL_TYPE, &status);
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

		pr_info("vin = %d mV | vout = %d mV | vbias = %d mV | iout = %d mA | temp = %d C\n",
			vals[0], vals[1], vals[2], vals[3], vals[4] / 1000);

		no_os_mdelay(500);
	}

exit:
	pr_err("Error code: %d.\n", ret);
	lt3074_remove(dev);
	return ret;
}
