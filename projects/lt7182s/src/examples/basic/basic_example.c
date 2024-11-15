/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for lt7182s project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "lt7182s.h"

int basic_example_main()
{
	struct lt7182s_dev *dev;
	struct lt7182s_status status;
	int ret, chan, vals[4];

	pr_info("Running basic example.\n");

	ret = lt7182s_init(&dev, &lt7182s_ip);
	if (ret)
		goto exit;

	while(1) {
		for (chan = LT7182S_CHAN_0; chan <= LT7182S_CHAN_1; chan++) {
			ret = lt7182s_read_value(dev, chan,
						 LT7182S_VIN, &vals[0]);
			if (ret)
				goto exit;

			ret = lt7182s_read_value(dev, chan,
						 LT7182S_VOUT, &vals[1]);
			if (ret)
				goto exit;

			ret = lt7182s_read_value(dev, chan,
						 LT7182S_IOUT, &vals[2]);
			if (ret)
				goto exit;

			ret = lt7182s_read_value(dev, chan,
						 LT7182S_TEMP, &vals[3]);
			if (ret)
				goto exit;

			ret = lt7182s_read_status(dev, chan,
						  LT7182S_STATUS_ALL_TYPE,
						  &status);
			if (status.vout)
				pr_info("Status vout asserted.\n");
			if (status.iout)
				pr_info("Status iout asserted.\n");
			if (status.input)
				pr_info("Status input asserted.\n");
			if (status.temp)
				pr_info("Status temp asserted.\n");
			if (status.cml)
				pr_info("Status cml asserted.\n");
			if (status.mfr_specific)
				pr_info("Status mfr_specific asserted.\n");

			pr_info("Channel: %d: vin = %d mV | vout = %d mV | iout = %d mA | temp = %d C\n",
				chan, vals[0], vals[1], vals[2],
				vals[3] / 1000);
		}

		pr_info("\n");
		no_os_mdelay(500);
	}

exit:
	pr_err("Error code: %d.\n", ret);
	lt7182s_remove(dev);
	return ret;
}
