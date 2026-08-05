/***************************************************************************//**
 *   @file   digital_output.c
 *   @brief  Digital Output example code for ad74414h-pmdz project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

int control_doutput_with_gpio(struct ad74416h_desc *desc)
{
	int ret;
	//Configure GPIO_SELECT as DO
	ret = ad74416h_set_gpio_config(desc, 0, AD74416H_GPIO_CONFIG_DO);
	if (ret)
		return ret;
	//Configure GPIO as the DO source
	ret = ad74416h_reg_update(desc, AD74416H_DO_EXT_CONFIG(0),
				  AD74416H_DO_SRC_SEL_MSK, 1);
	if (ret)
		return ret;
	return 0;
}

/***************************************************************************//**
 * @brief Digital Output example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74414h_desc;
	int ret;

	ret = ad74416h_init(&ad74414h_desc, &ad74414h_ip);
	if (ret)
		goto error;

	pr_info("ad74414h successfully initialized!\r\n");

	//Configure Channel A as High Impedance
	ret = ad74416h_set_channel_function(ad74414h_desc, 0, AD74416H_HIGH_Z);
	if (ret) {
		pr_info("Error setting Channel 0 as high impedance\r\n");
		goto error_ad74414h;
	}

	//Select source capability by setting the DO_MODE bit
	ret = ad74416h_reg_update(ad74414h_desc, AD74416H_DO_EXT_CONFIG(0),
				  AD74416H_DO_MODE_MSK, 1);
	if (ret) {
		pr_info("Error setting DO Source mode\r\n");
		goto error_ad74414h;
	}

	ret = control_doutput_with_gpio(ad74414h_desc);
	if (ret) {
		pr_info("Error setting the GPIO as source for the DO\r\n");
		goto error_ad74414h;
	}

	//Enable the FET in channel 1. Comment the next block if the user wants to control the DO with the GPIO.
	ret = ad74416h_reg_update(ad74414h_desc, AD74416H_DO_EXT_CONFIG(0),
				  AD74416H_DO_DATA_MSK, 1);
	if (ret) {
		pr_info("Error enabling the FET in Channel 1\r\n");
		goto error_ad74414h;
	}

error_ad74414h:
	ad74416h_remove(ad74414h_desc);
	return ret;
error:
	pr_info("Error %d !\r\n", ret);
	return ret;
}
