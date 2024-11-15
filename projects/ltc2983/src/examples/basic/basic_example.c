/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ltc2983 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "basic_example.h"
#include "common_data.h"
#include "ltc2983.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct ltc2983_desc *dev;
	int ret, i;
	int val;

	if (ltc2983_ip.dev_type == ID_LTC2983)
		pr_info("\r\nRunning LTC2983 Basic Example\r\n");
	else if (ltc2983_ip.dev_type == ID_LTC2984)
		pr_info("\r\nRunning LTC2984 Basic Example\r\n");
	else if (ltc2983_ip.dev_type == ID_LTC2986)
		pr_info("\r\nRunning LTC2986 Basic Example\r\n");

	ret = ltc2983_init(&dev, &ltc2983_ip);
	if (ret)
		goto error;

	while (1) {
		for (i = 0; i < dev->max_channels_nr; i++) {
			if (!dev->sensors[i] ||
			    dev->sensors[i]->type == LTC2983_RSENSE)
				continue;
			ret = ltc2983_chan_read(dev, i + 1, &val);
			if (ret)
				goto free_dev;
			pr_info("Channel %d: temperature: %d mC\r\n", i + 1,
				val);
		}
		no_os_mdelay(1000);
	}

free_dev:
	ltc2983_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
