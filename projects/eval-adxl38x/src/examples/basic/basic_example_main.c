/***************************************************************************//**
 *   @file   basic_example_main.c
 *   @brief  Main program for basic example eval-adxl38x project
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "basic_example_main.h"
#include "common_data.h"
#include "adxl38x.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include <errno.h>

/***************************************************************************//**
 * @brief Example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int basic_example_main()
{
	struct adxl38x_dev *adxl38x_desc;
	union adxl38x_sts_reg_flags device_flags;
	int ret;
	uint8_t reg_value[10] = {0};
	uint8_t device_range;
	uint8_t device_id;
	uint8_t opmode;
	uint16_t x, y, z, t;
	struct adxl38x_fractional_val xf, yf, zf;

	/* Initializing the device*/
	ret = adxl38x_init(&adxl38x_desc, adxl38x_ip);
	if (ret)
		goto error;
	ret = adxl38x_soft_reset(adxl38x_desc);
	if (ret == -EAGAIN)
		pr_info("Reset was not successful\n");
	else if (ret)
		goto error;
	ret = adxl38x_get_sts_reg(adxl38x_desc, &device_flags);
	if (ret)
		goto error;
	pr_info(" status value = 0x%08lx\n", (uint32_t)device_flags.value);

	ret = adxl38x_set_op_mode(adxl38x_desc, ADXL38X_MODE_LP);
	if (ret)
		goto error;
	ret = adxl38x_set_range(adxl38x_desc, ADXL382_RANGE_15G);
	if (ret)
		goto error;
	ret = adxl38x_get_range(adxl38x_desc, &device_range);
	if (ret)
		goto error;
	ret = adxl38x_get_deviceID(adxl38x_desc, &device_id);
	if (adxl38x_desc->dev_type == ID_ADXL382)
		pr_info(" Device Type = ADXL382\n");
	else
		pr_info(" Device Type = ADXL380\n");
	no_os_mdelay(500);

	while(1) {
		ret = adxl38x_get_sts_reg(adxl38x_desc, &device_flags);
		if (ret)
			goto error;
		ret = adxl38x_set_op_mode(adxl38x_desc, ADXL38X_MODE_HP);
		if (ret)
			goto error;
		ret = adxl38x_get_op_mode(adxl38x_desc, &opmode);
		if (ret)
			goto error;
		ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_DEVID_AD,
					       2, reg_value);
		if (ret)
			goto error;
		pr_info("Status value = 0x%08lx\n", device_flags.value);
		pr_info("OP_MODE = %d\n", opmode);
		pr_info("DEVICE_AD = 0x%02X\n", (int)reg_value[0]);
		pr_info("Device type = %d\n", (int)adxl38x_desc->dev_type);
		pr_info("Device range setting = %d\n", (int)adxl38x_desc->range);

		pr_info("RAW DATA in Gees (magnitude less than 1 and negative is displayed as absolute value)\n");
		pr_info("----------------\n");
		ret = adxl38x_get_xyz_gees(adxl38x_desc, ADXL38X_CH_EN_XYZT,
					   &xf, &yf, &zf);
		pr_info(" x = %lld.%07dg\n", xf.integer, abs(xf.fractional));
		pr_info(" y = %lld.%07dg\n", yf.integer, abs(yf.fractional));
		pr_info(" z = %lld.%07dg\n", zf.integer, abs(zf.fractional));

		no_os_mdelay(1000);

		ret = adxl38x_get_op_mode(adxl38x_desc, &opmode);
		if (ret)
			goto error;
		pr_info("OP_MODE = %d\n", opmode);
		pr_info("RAW DATA in LSBs\n");
		pr_info("----------------\n");
		ret = adxl38x_get_raw_data(adxl38x_desc, ADXL38X_CH_EN_XYZT,  &x, &y, &z, &t);
		if (ret)
			goto error;
		pr_info(" x = %d\n", (int)x);
		pr_info(" y = %d\n", (int)y);
		pr_info(" z = %d\n", (int)z);
		pr_info(" t = %d\n", (int)t);
		no_os_mdelay(1000);
	}

error:
	pr_info("Error!\n");
	adxl38x_remove(adxl38x_desc);
	return 0;
}

