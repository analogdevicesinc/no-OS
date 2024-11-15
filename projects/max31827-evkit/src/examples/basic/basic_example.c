/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for max31827 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "basic_example.h"
#include "common_data.h"
#include "max31827.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct max31827_device *dev;
	int ret;
	int32_t val;
	int idx;

	pr_info("\r\nRunning MAX31827 Basic Example\r\n");

	ret = max31827_init(&dev, &max31827_ip);
	if (ret)
		goto error;

	while (1) {
		ret = max31827_read_temp_input(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("Temperature: %d mC\r\n", val);

		no_os_mdelay(1000);
	}

free_dev:
	max31827_remove(dev);
error:
	pr_info("Error!\r\n");
	return ret;
}
