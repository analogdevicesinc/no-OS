/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ad8460 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include "no_os_uart.h"
#include "ad8460.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int example_main()
{
	struct no_os_uart_desc *uart_desc;
	int _ret = no_os_uart_init(&uart_desc, &uip);
	if (_ret)
		return _ret;
	no_os_uart_stdio(uart_desc);

	struct ad8460_device *dev;
	int ret, i;
	uint16_t val;
	uint8_t flag;

	pr_info("\r\nRunning AD8460 Basic Example\r\n");

	ret = ad8460_init(&dev, &ad8460_ip);
	if (ret)
		goto error;

	ret = ad8460_reset(dev);
	if (ret)
		goto free_dev;

	/** Switch to APG mode */
	ret = ad8460_enable_apg_mode(dev, 1);
	if (ret)
		goto free_dev;

	/** Switch to AWG mode */
	ret = ad8460_enable_apg_mode(dev, 0);

	for (i = 0; i < 16; i++) {
		ret = ad8460_set_hvdac_word(dev, i, i);
		if (ret)
			goto free_dev;

		ret = ad8460_get_hvdac_word(dev, i, &val);
		if (ret)
			goto free_dev;

		pr_info("HVDAC[%d]: 0x%04X\r\n", i, val);
	}

	ret = ad8460_read_shutdown_flag(dev, &flag);
	if (ret)
		goto free_dev;

	pr_info("Shutdown flag: %d\r\n", flag);

	if (flag) {
		ret = ad8460_hv_reset(dev);
		if (ret)
			goto free_dev;
	}

	pr_info("AD8460 Basic Example Done\r\n");

	return 0;

free_dev:
	ad8460_remove(dev);
error:
	pr_info("Error!\r\n");
	return ret;
}
