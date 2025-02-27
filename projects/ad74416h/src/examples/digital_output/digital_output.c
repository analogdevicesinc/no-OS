/***************************************************************************//**
 *   @file   digital_output.c
 *   @brief  Digital Output example code for ad74416h-pmdz project
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
	struct ad74416h_desc *ad74416h_desc;
	int ret;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure Channel A as High Impedance
	ret = ad74416h_set_channel_function(ad74416h_desc, 0, AD74416H_HIGH_Z);
	if (ret) {
		pr_info("Error setting Channel 0 as high impedance\r\n");
		goto error_ad74416h;
	}

	//Select source capability by setting the DO_MODE bit
	ret = ad74416h_reg_update(ad74416h_desc, AD74416H_DO_EXT_CONFIG(0),
				  AD74416H_DO_MODE_MSK, 1);
	if (ret) {
		pr_info("Error setting DO Source mode\r\n");
		goto error_ad74416h;
	}

	ret = control_doutput_with_gpio(ad74416h_desc);
	if (ret) {
		pr_info("Error setting the GPIO as source for the DO\r\n");
		goto error_ad74416h;
	}

	//Enable the FET in channel 1. Comment the next block if the user wants to control the DO with the GPIO.
	ret = ad74416h_reg_update(ad74416h_desc, AD74416H_DO_EXT_CONFIG(0),
				  AD74416H_DO_DATA_MSK, 1);
	if (ret) {
		pr_info("Error enabling the FET in Channel 1\r\n");
		goto error_ad74416h;
	}

error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error %d !\r\n", ret);
	return 0;
}
