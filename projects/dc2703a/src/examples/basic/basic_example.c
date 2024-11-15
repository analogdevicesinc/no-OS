/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for lt8491 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "basic_example.h"
#include "common_data.h"
#include "lt8491.h"
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
	struct lt8491_desc *dev;
	int32_t val;
	uint32_t uval, uvals[3];
	int ret;

	pr_info("\r\nRunning LT8491 Basic Example\r\n");

	ret = lt8491_init(&dev, &lt8491_ip);
	if (ret)
		goto error;

	pr_info("LT8491 Initialized\r\n");

	ret = lt8491_restart_chip(dev);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Chip Restarted\r\n");

	ret = lt8491_write_enable(dev, false);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Charging Disabled\r\n");

	ret = lt8491_write_scratch(dev, 0x1234);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Scratch Written: 0x1234\r\n");

	ret = lt8491_read_scratch(dev, &uval);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Scratch Read: 0x%04X\r\n", uval);

	ret = lt8491_read_serial_id(dev, uvals);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Serial ID: %d %d %d\r\n", uvals[0], uvals[1],
		uvals[2]);

	ret = lt8491_configure_telemetry(dev, &lt8491_ip);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Telemetry Configured\r\n");

	ret = lt8491_write_enable(dev, true);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Charging Enabled\r\n");

	ret = lt8491_read_enable(dev, &uval);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Charging: %s\r\n", uval ? "Enabled" :
		"Disabled");

	ret = lt8491_read_charging_status(dev, &uval);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Charging Status: %s\r\n", uval ? "Charging" :
		"Not Charging");

	ret = lt8491_read_charging_stage(dev, &val);
	if (ret)
		goto free_dev;

	pr_info("LT8491 Charging Stage: %c\r\n", (char)val);

	while (1) {
		ret = lt8491_read_tbat(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("LT8491 TBAT: %d\r\n", val);

		ret = lt8491_read_pout(dev, &uval);
		if (ret)
			goto free_dev;

		pr_info("LT8491 POUT: %d\r\n", uval);

		ret = lt8491_read_pin(dev, &uval);
		if (ret)
			goto free_dev;

		pr_info("LT8491 PIN: %d\r\n", uval);

		ret = lt8491_read_efficiency(dev, &uval);
		if (ret)
			goto free_dev;

		pr_info("LT8491 Efficiency: %d\r\n", uval);

		ret = lt8491_read_iout(dev, &uval);
		if (ret)
			goto free_dev;

		pr_info("LT8491 IOUT: %d\r\n", uval);

		ret = lt8491_read_iin(dev, &uval);
		if (ret)
			goto free_dev;

		pr_info("LT8491 IIN: %d\r\n", uval);

		ret = lt8491_read_vbat(dev, &uval);
		if (ret)
			goto free_dev;

		pr_info("LT8491 VBAT: %d\r\n", uval);

		ret = lt8491_read_vin(dev, &uval);
		if (ret)
			goto free_dev;

		pr_info("LT8491 VIN: %d\r\n", uval);

		no_os_mdelay(5000);
	}

	return 0;

free_dev:
	lt8491_remove(dev);
error:
	pr_info("Error!\r\n");
	return ret;
}
