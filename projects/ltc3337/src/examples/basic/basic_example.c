/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ltc3337 project
 *   @author Brent Kowal (brent.kowal@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "basic_example.h"
#include "common_data.h"
#include "ltc3337.h"
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
	struct ltc3337_dev *dev;
	struct charge_count_t charge;
	uint16_t value16;
	uint32_t value32;
	int16_t temp_value;

	int ret;

	ret = ltc3337_init(&dev, &ltc3337_ip);
	if (ret)
		goto error;

	while (1) {
		ret = ltc3337_get_accumulated_charge(dev, &charge, &value16);
		if(ret)
			goto free_dev;

		pr_info("Accumulated Charge %d.%09d (0x%04X)\n", charge.a_hr, charge.na_hr,
			value16);

		ret = ltc3337_get_voltage_mv(dev, BAT_IN_IPEAK_OFF, &value32);
		if(ret)
			goto free_dev;

		pr_info("Batt In, iPeak Off %d mV\n", value32);

		ret = ltc3337_get_voltage_mv(dev, BAT_IN_IPEAK_ON, &value32);
		if(ret)
			goto free_dev;

		pr_info("Batt In, iPeak On %d mV\n", value32);

		ret = ltc3337_get_voltage_mv(dev, BAT_OUT_IPEAK_OFF, &value32);
		if(ret)
			goto free_dev;

		pr_info("Batt Out, iPeak Off %d mV\n", value32);

		ret = ltc3337_get_voltage_mv(dev, BAT_OUT_IPEAK_ON, &value32);
		if(ret)
			goto free_dev;

		pr_info("Batt Out, iPeak On %d mV\n", value32);


		ret = ltc3337_get_temperature_c(dev, &temp_value);
		if(ret)
			goto free_dev;

		pr_info("Die Temp %d C\n", temp_value);

		no_os_mdelay(2000);
	}

free_dev:
	ltc3337_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
