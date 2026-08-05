/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ltc4162l project
 *   @author Marc Paolo Sosa <marcpaolo.sosa@analog.com>
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include "no_os_uart.h"
#include "ltc4162l.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_units.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/

//create example code for ltc4162l project
int example_main()
{
	struct no_os_uart_desc *uart_desc;
	int _ret = no_os_uart_init(&uart_desc, &uip);
	if (_ret)
		return _ret;
	no_os_uart_stdio(uart_desc);

	struct ltc4162l_desc *dev;
	uint16_t val;
	int ret;

	pr_info("LTC4162L Basic Example\n");

	ret = ltc4162l_init(&dev, &ltc4162l_ip);
	if (ret)
		goto error;

	while (1) {

		ret = ltc4162l_get_cell_count(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("Cell count: %d\n", val);

		ret = ltc4162l_get_chem_type(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("Chem type: %d\n", val);

		ret = ltc4162l_read_vbat(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("VBAT: %d\n", val);

		ret = ltc4162l_read_avg_vbat(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("Average VBAT: %d\n", val);

		ret = ltc4162l_read_ibat(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("IBAT: %d\n", val);
	}

	pr_info("\n");
	no_os_mdelay(500);

free_dev:
	ltc4162l_remove(dev);

error:
	pr_err("Error code: %d.\n", ret);
	return ret;

}
