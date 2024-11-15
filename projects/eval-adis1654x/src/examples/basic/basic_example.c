/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example header for eval-adis1654x project
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "basic_example.h"
#include "common_data.h"
#include "adis1654x.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_units.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

static const char * const output_data[] = {
	"angular velocity x axis: ",
	"angular velocity y axis: ",
	"angular velocity z axis: ",
	"acceleration x axis    : ",
	"acceleration y axis    : ",
	"acceleration z axis    : ",
	"temperature            : "
};

static const char * const output_unit[] = {
	"rad/s",
	"rad/s",
	"rad/s",
	"m/s^2",
	"m/s^2",
	"m/s^2",
	"milli °C"
};

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

/**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int basic_example_main()
{
	struct adis_dev *adis1654x_desc;
	int ret;
	int32_t val[7];
	struct adis_scale_fractional accl_scale;
	struct adis_scale_fractional anglvel_scale;
	struct adis_scale_fractional temp_scale;

	ret = adis_init(&adis1654x_desc, &adis1654x_ip);
	if (ret)
		goto exit;

	ret = adis_get_accl_scale(adis1654x_desc, &accl_scale);
	if (ret)
		goto exit;

	ret = adis_get_anglvel_scale(adis1654x_desc, &anglvel_scale);
	if (ret)
		goto exit;

	ret = adis_get_temp_scale(adis1654x_desc, &temp_scale);
	if (ret)
		goto exit;

	float output_scale[] = {
		(float)anglvel_scale.dividend / anglvel_scale.divisor,
		(float)anglvel_scale.dividend / anglvel_scale.divisor,
		(float)anglvel_scale.dividend / anglvel_scale.divisor,
		(float)accl_scale.dividend / accl_scale.divisor,
		(float)accl_scale.dividend / accl_scale.divisor,
		(float)accl_scale.dividend / accl_scale.divisor,
		(float)temp_scale.dividend / temp_scale.divisor,
	};

	while(1) {
		pr_info ("while loop \n");
		no_os_mdelay(1000);
		ret = adis_read_x_gyro(adis1654x_desc, &val[0]);
		if (ret)
			goto exit;
		ret = adis_read_y_gyro(adis1654x_desc, &val[1]);
		if (ret)
			goto exit;
		ret = adis_read_z_gyro(adis1654x_desc, &val[2]);
		if (ret)
			goto exit;
		ret = adis_read_x_accl(adis1654x_desc, &val[3]);
		if (ret)
			goto exit;
		ret = adis_read_y_accl(adis1654x_desc, &val[4]);
		if (ret)
			goto exit;
		ret = adis_read_z_accl(adis1654x_desc, &val[5]);
		if (ret)
			goto exit;
		ret = adis_read_temp_out(adis1654x_desc, &val[6]);
		if (ret)
			goto exit;

		for (uint8_t i = 0; i < 7; i++)
			pr_info("%s %.5f %s \n", output_data[i], val[i] * output_scale[i],
				output_unit[i]);
	}
exit:
	adis_remove(adis1654x_desc);
	if (ret)
		pr_info("Error!\n");
	return ret;
}
