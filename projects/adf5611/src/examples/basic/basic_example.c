/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example eval-adf5611 project
 *   @author Josemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common_data.h"
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
	struct adf5611_dev *dev;
	int ret;

	pr_info("Enter basic example \n");

	ret = adf5611_init(&dev, &adf5611_ip);
	if (ret)
		goto error;

	ret = adf5611_set_rfout(dev, 12000000000);
	if (ret)
		goto error;

	return ret;
error:
	pr_info("Error!\n");
	return ret;
}
