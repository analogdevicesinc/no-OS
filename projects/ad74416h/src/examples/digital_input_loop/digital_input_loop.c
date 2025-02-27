/***************************************************************************//**
 *   @file   digital_input_loop.c
 *   @brief  Digital Input Loop Powered example code for ad74416h-pmdz project
 *	     This example configures Channel A as a Digital Input.
 *	     It prints in terminal the DIN value in Channel A
 *	     GPIO_A LED is used as a representation of the DIN value in channel A
 *   @author Raquel Grau (raquel.grau@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "common_data.h"
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/*******************************************************************************
 * @brief This function configures the digital input as a Type1/Type3 Digital
 *        input
 * @param desc - The device structure
 * @return ret - Result of the execution.
*******************************************************************************/
int configure_di_type1_3(struct ad74416h_desc *desc, uint32_t ch)
{
	int ret;
	//Set a fixed threshold mode
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch),
				  AD74416H_DIN_THRESH_MODE_MSK, 1);
	if (ret)
		return ret;
	//Set the threshold to 8.5V
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch),
				  AD74416H_COMP_THRESH_MSK, 0x37);
	if (ret)
		return ret;
	//Configure the current sink to 2.4mA
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch),
				  AD74416H_DIN_SINK_RANGE_MSK, 0);
	if (ret)
		return ret;
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch), AD74416H_DIN_SINK_MSK,
				  0x14);
	if (ret)
		return ret;
	return 0;
}

/*******************************************************************************
 * @brief This function configures the digital input as a Type2 Digital input
 * @param desc - The device structure
 * @return ret - Result of the execution.
*******************************************************************************/
int configure_di_type2(struct ad74416h_desc *desc, uint32_t ch)
{
	int ret;
	//Set a fixed threshold mode
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch),
				  AD74416H_DIN_THRESH_MODE_MSK, 1);
	if (ret)
		return ret;
	//Set the threshold to 8V
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch),
				  AD74416H_COMP_THRESH_MSK, 0x37);
	if (ret)
		return ret;
	//Configure the current sink to 6.96mA
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch),
				  AD74416H_DIN_SINK_RANGE_MSK, 1);
	if (ret)
		return ret;
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch), AD74416H_DIN_SINK_MSK,
				  0x1D);
	if (ret)
		return ret;
	return 0;
}

/***************************************************************************//**
 * @brief Digital Input Loop example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74416h_desc;
	int ret;

	uint16_t current_code = 0;
	uint8_t dig_input = 0;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure Channel A as Digital Input Loop mode
	ret = ad74416h_set_channel_function(ad74416h_desc, 0,
					    AD74416H_DIGITAL_INPUT_LOOP);
	if (ret) {
		pr_info("Error setting Channel 0 as digital input logic mode\r\n");
		goto error_ad74416h;
	}

	//Configure the current output limit
	ret = ad74416h_dac_current_to_code(ad74416h_desc, 9000, &current_code);
	if (ret) {
		pr_info("Error calculating DAC code for desired current\r\n");
		goto error_ad74416h;
	}
	ret = ad74416h_set_channel_dac_code(ad74416h_desc, 0, current_code);
	if (ret) {
		pr_info("Error loading current limit into DAC A\r\n");
		goto error_ad74416h;
	}

	//Link GPIO_A to DIN0
	ret = ad74416h_set_gpio_config(ad74416h_desc, 0, AD74416H_GPIO_CONFIG_COMP);
	if (ret) {
		pr_info("Error configuring GPIO_A as the output of the DIN comparator\r\n");
		goto error_ad74416h;
	}

	//Uncomment the desired function depending on which type of DI is desired
	ret = configure_di_type1_3(ad74416h_desc, 0);
	//ret = configure_di_type2(ad74416h_desc, 0);
	if (ret) {
		pr_info("Error configuring the DI type\r\n");
		goto error_ad74416h;
	}


	while (1) {
		no_os_mdelay(1000);
		ret = ad74416h_gpio_get(ad74416h_desc, 0, &dig_input);
		if (ret) {
			pr_info("Error getting DIN value\r\n");
			goto error_ad74416h;
		}
		pr_info("DIN Value = %d\r\n", dig_input);
	}

error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error %d !\r\n", ret);
	return 0;
}
