/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltm4700 project.
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "ltm4700.h"
#include <stdlib.h>

#define TELEMETRY_DISPLAY_DELAY_MS	500

int example_main()
{
	struct ltm4700_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret = 0, vals[4];
	uint8_t chan;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	pr_info("\e[2J\e[H");
	pr_info("LTM4700 basic example.\n");

	ret = ltm4700_init(&dev, &ltm4700_ip);
	if (ret) {
		pr_err("\nLTM4700 init failed with error code: %d\n", ret);
		return ret;
	}
	pr_info("LTM4700 initialized successfully!\n"); // debug

	ret = ltm4700_set_operation(dev, LTM4700_CHAN_ALL,
				    LTM4700_OPERATION_ON);
	if (ret)
		goto exit;

	while (1) {
		for (chan = LTM4700_CHAN_0; chan <= LTM4700_CHAN_1; chan++) {
			ret = ltm4700_read_value(dev, chan, LTM4700_VIN,
						 &vals[0]);
			if (ret)
				goto exit;

			ret = ltm4700_read_value(dev, chan, LTM4700_VOUT,
						 &vals[1]);
			if (ret)
				goto exit;

			ret = ltm4700_read_value(dev, chan, LTM4700_IOUT,
						 &vals[2]);
			if (ret)
				goto exit;

			ret = ltm4700_read_value(dev, chan, LTM4700_TEMP_IC,
						 &vals[3]);
			if (ret)
				goto exit;

			pr_info("CH%d: vin = %d mV | vout = %d mV | iout = %d mA | temp_ic = %d mC\n",
				chan, vals[0], vals[1], vals[2], vals[3]);
		}

		pr_info("\n");
		no_os_mdelay(TELEMETRY_DISPLAY_DELAY_MS);
	}


exit:
	ltm4700_remove(dev);

	return ret;
}
