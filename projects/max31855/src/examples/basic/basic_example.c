/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for max31855 project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "basic_example.h"
#include "common_data.h"
#include "max31855.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct max31855_decimal thermocouple_temp;
	struct max31855_decimal ambient_temp;
	struct max31855_dev *dev;
	int ret;

	ret = max31855_init(&dev, &max31855_ip);
	if (ret)
		goto error;

	while (1) {
		pr_info("%c", 27);
		pr_info("%c", '[');
		pr_info("%c", '2');
		pr_info("%c", 'J');

		ret = max31855_read_temp(dev, &thermocouple_temp, &ambient_temp);
		if (ret)
			goto free_dev;

		pr_info("Thermocouple temperature %.02d.%.02d\n", thermocouple_temp.integer,
			thermocouple_temp.decimal);
		pr_info("Ambient temperature %.02d.%.02d\n", ambient_temp.integer,
			ambient_temp.decimal);

		no_os_mdelay(2000);
	}

free_dev:
	max31855_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
