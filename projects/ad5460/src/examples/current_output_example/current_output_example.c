/***************************************************************************//**
 *   @file   current_output_example.c
 *   @brief  implementation of current output configuration for eval-ad5460 project
 *   @author Akila Marimuthu (akila.marimuthu@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "current_output_example.h"
#include "common_data.h"
#include "ad5460.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Current output example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int current_output_example_main()
{
	struct ad5460_desc *ad5460_desc;
	int ret;
	uint16_t dac_code0, val;
	int32_t output_in_uamps_ch0 = 10000;

	ret = ad5460_init(&ad5460_desc, &ad5460_ip);
	if (ret)
		goto error;

	pr_info("ad5460 successfully initialized!\r\n");

	/* Set channel function */
	ret = ad5460_set_channel_function(ad5460_desc, 0, AD5460_CURRENT_OUT);
	if (ret)
		goto error_ad5460;

	//set output range
	ret = ad5460_set_channel_iout_range(ad5460_desc, 0, AD5460_IOUT_RANGE_0_25MA);
	if (ret)
		goto error_ad5460;

	//Set channel 0 output
	ret = ad5460_dac_current_to_code(ad5460_desc, output_in_uamps_ch0, &dac_code0,
					 0);
	if (ret)
		goto error_ad5460;

	ret = ad5460_set_channel_dac_code(ad5460_desc, 0, dac_code0);
	if (ret)
		goto error_ad5460;

	pr_info("For channel 0, expected output = %d uA \n DAC code = %d \n",
		output_in_uamps_ch0, dac_code0);

	ret = ad5460_reg_read(ad5460_desc, AD5460_DAC_ACTIVE(0), &val);
	if (ret)
		goto error_ad5460;

	pr_info("DAC ACTIVE CODE of channel 0 = %d \n", val);

	return 0;

error_ad5460:
	ad5460_remove(ad5460_desc);
error:
	pr_info("AD5460 Error!\r\n");
	return ret;
}
