/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example eval-adf4382 project
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "basic_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int basic_example_main()
{
	struct adf4382_dev *dev;
	int ret;

	pr_info("Enter basic example \n");

	ret = adf4382_init(&dev, &adf4382_ip);
	if (ret)
		goto error;

	ret = adf4382_set_rfout(dev, 3100000000);
	if (ret)
		goto remove_adf4382;

	ret = adf4382_set_phase_adjust(dev, 1);

remove_adf4382:
	adf4382_remove(dev);
error:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
