/***************************************************************************//**
 *   @file   dummy_example.c
 *   @brief  DUMMY example for ad719x project
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "dummy_example.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int dummy_example_main()
{
	/* Device AD719X instance. */
	struct ad719x_dev *dev;
	float temp, v;
	int ret;
	uint32_t avg, i;

	ret = ad719x_init(&dev, ad7190_dev_ip);
	if (ret)
		return ret;

	ret = ad719x_temperature_read(dev, &temp);
	if (ret)
		return ret;

	ret = ad719x_continuous_read_avg(dev, 100, &avg);
	if (ret)
		return ret;

	v = ad719x_convert_to_volts(dev, avg, 3.3);

	pr_info("Temperature = %.6f C/n", temp);
	pr_info("Average read from 100 samples = %.6f Volts.", avg);

	return 0;
}
