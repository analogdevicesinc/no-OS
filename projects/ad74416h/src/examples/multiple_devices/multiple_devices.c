/***************************************************************************//**
 *   @file   multiple_devices.c
 *   @brief  Example code with multiple AD74416H devices for ad74416h-pmdz project
 *   @author Raquel Grau (raquel.grau@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/***************************************************************************//**
 * @brief Multiple Devices example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74416h_desc, *ad74416h_ad1_desc;
	int ret;
	uint16_t reg_value;
	uint16_t dac_code;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip); //Device address 0
	if (ret)
		goto error;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ad1_ip); //Device address 1
	if (ret)
		goto error_ad74416h;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure voltage range to +-12 V
	ret = ad74416h_set_channel_vout_range(ad74416h_desc, 0,
					      AD74416H_VOUT_RANGE_NEG12_12V);
	if (ret) {
		pr_info("Error setting Channel 0 to range +-12 V");
		goto error_ad74416h_ad1;
	}

	ret = ad74416h_set_channel_vout_range(ad74416h_ad1_desc, 0,
					      AD74416H_VOUT_RANGE_NEG12_12V);
	if (ret) {
		pr_info("Error setting Channel 0 to range +-12 V");
		goto error_ad74416h_ad1;
	}

	//Configure Current Limit for channel A in Vout mode to 8mA
	ret = ad74416h_set_channel_i_limit(ad74416h_desc, 0, AD74416H_I_LIMIT1);
	if (ret) {
		pr_info("Error setting the current limit to 8mA");
		goto error_ad74416h_ad1;
	}

	ret = ad74416h_set_channel_i_limit(ad74416h_ad1_desc, 0, AD74416H_I_LIMIT1);
	if (ret) {
		pr_info("Error setting the current limit to 8mA");
		goto error_ad74416h_ad1;
	}

	//Configure Channel A as Voltage Output
	ret = ad74416h_set_channel_function(ad74416h_desc, 0, AD74416H_VOLTAGE_OUT);
	if (ret) {
		pr_info("Error setting Channel 0 as voltage output");
		goto error_ad74416h_ad1;
	}

	ret = ad74416h_set_channel_function(ad74416h_ad1_desc, 0, AD74416H_VOLTAGE_OUT);
	if (ret) {
		pr_info("Error setting Channel 0 as voltage output");
		goto error_ad74416h_ad1;
	}

	//Configure Channel A code to -6V output
	ad74416h_dac_voltage_to_code(ad74416h_desc, -6000, &dac_code, 0);
	pr_info("Calculated DAC CODE for -6V is: 0x%0x\r\n", dac_code);
	ret = ad74416h_set_channel_dac_code(ad74416h_desc, 0, dac_code);
	if (ret)
		goto error_ad74416h_ad1;

	ret = ad74416h_reg_read(ad74416h_desc, AD74416H_DAC_CODE(0), &reg_value);
	if (ret)
		goto error_ad74416h_ad1;

	pr_info("DAC CODE = 0x%0x\r\n", reg_value);

error_ad74416h_ad1:
	ad74416h_remove(ad74416h_desc);
error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error %d !\r\n", ret);
	return 0;
}
