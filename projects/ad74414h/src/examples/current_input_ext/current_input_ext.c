/***************************************************************************//**
 *   @file   current_input_ext.c
 *   @brief  Current input in External power mode example for ad74414h-pmdz
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
#include "math.h"

/***************************************************************************//**
 * @brief Current input External power example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74414h_desc;
	int ret;

	union ad74416h_live_status status;
	uint32_t adc_value = 0;
	double calculated_current = 0;

	ret = ad74416h_init(&ad74414h_desc, &ad74414h_ip);
	if (ret)
		goto error;

	pr_info("ad74414h successfully initialized!\r\n");

	//Configure Channel A as Current Input externally powered
	ret = ad74416h_set_channel_function(ad74414h_desc, 0, AD74416H_CURRENT_IN_EXT);
	if (ret) {
		pr_info("Error setting Channel 0 as current input ext powered\r\n");
		goto error_ad74414h;
	}

	//Configure the ADC sample rate
	ret = ad74416h_set_adc_rate(ad74414h_desc, 0, AD74416H_20SPS_50_60HZ_REJECTION);
	if (ret) {
		pr_info("Error setting sampling rate to 20SPS\r\n");
		goto error_ad74414h;
	}

	//Enable ADC A
	ret = ad74416h_set_adc_channel_enable(ad74414h_desc, 0, 1);
	if (ret) {
		pr_info("Error enabling ADC A\r\n");
		goto error_ad74414h;
	}
	//Set ADC A to continuous conversion
	ret = ad74416h_set_adc_conv_seq(ad74414h_desc, AD74416H_START_CONT);
	if (ret) {
		pr_info("Error enabling continuous conversions in ADC A\r\n");
		goto error_ad74414h;
	}

	while (1) {
		//Check if there is data ready in the ADC
		ret = ad74416h_get_live(ad74414h_desc, &status);
		if (ret) {
			pr_info("Error reading the live status register\r\n");
			goto error_ad74414h;
		}
		//If data is ready, read the ADC result
		if (status.status_bits.ADC_DATA_RDY == 1) {
			ret = ad74416h_get_raw_adc_result(ad74414h_desc, 0, &adc_value);
			if (ret) {
				pr_info("Error getting raw adc result in ADC A\r\n");
				goto error_ad74414h;
			}
			pr_info("The ADC input value is %0x\r\n", adc_value);
			calculated_current = (adc_value / pow(2, 24)) * 0.3125 / 12;
			pr_info("Calculated current = %.6f mA\r\n", calculated_current * 1000);
		}
	}

error_ad74414h:
	ad74416h_remove(ad74414h_desc);
	return ret;
error:
	pr_info("Error %d !\r\n", ret);
	return ret;
}
