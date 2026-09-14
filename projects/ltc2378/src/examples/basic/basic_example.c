/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ltc2378 project
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "common_data.h"
#include "ltc2378.h"
#include "parameters.h"
#include <stdlib.h>

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/

int example_main()
{
	struct ltc2378_dev *dev;
	uint32_t raw;
	int32_t voltage_uv;
	int ret;

	pr_info("Enter basic example \n");

	ret = ltc2378_init(&dev, &ltc2378_ip);
	if (ret) {
		pr_info("Init failed: %d\n", ret);
		return ret;
	}

	pr_info("VREF: %lu uV, Mode: %s\n",
		dev->vref_uv,
		(dev->input_mode == LTC2378_UNIPOLAR) ? "Unipolar" : "Bipolar");

	while (1) {
		ret = ltc2378_read_raw(dev, &raw);
		if (ret) {
			pr_info("Read failed: %d\n", ret);
			break;
		}

		ret = ltc2378_raw_to_uv(dev, raw, &voltage_uv);
		if (ret) {
			pr_info("Convert failed: %d\n", ret);
			break;
		}

		pr_info("Raw: %lu, Voltage: %ld uV\n", raw, voltage_uv);

		no_os_mdelay(BASIC_EXAMPLE_DELAY_MS);
	}

	ltc2378_remove(dev);
	return ret;
}
