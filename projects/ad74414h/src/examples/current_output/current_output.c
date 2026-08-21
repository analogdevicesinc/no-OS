/***************************************************************************//**
 *   @file   current_output.c
 *   @brief  Current Output example code for ad74414h-pmdz project
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

/***************************************************************************//**
 * @brief Current output example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74414h_desc;
	int ret;
	uint16_t dac_code;

	ret = ad74416h_init(&ad74414h_desc, &ad74414h_ip);
	if (ret)
		goto error;

	pr_info("ad74414h successfully initialized!\r\n");

	//Configure Channel A as Current Output
	ret = ad74416h_set_channel_function(ad74414h_desc, 0, AD74416H_CURRENT_OUT);
	if (ret) {
		pr_info("Error setting Channel 0 as current output\r\n");
		goto error_ad74414h;
	}

	//Calculate the code for the DAC
	ret = ad74416h_dac_current_to_code(ad74414h_desc, 10000, &dac_code);
	if (ret) {
		pr_info("Error calculating the code for the DAC\r\n");
		goto error_ad74414h;
	}

	pr_info("The code for the dac for 10mA is %0x\r\n", dac_code);

	//Configure Channel A code to middle range
	ret = ad74416h_set_channel_dac_code(ad74414h_desc, 0, dac_code);
	if (ret) {
		pr_info("Error setting the dac code\r\n");
		goto error_ad74414h;
	}

error_ad74414h:
	ad74416h_remove(ad74414h_desc);
	return ret;
error:
	pr_info("Error %d!\r\n", ret);
	return ret;
}
