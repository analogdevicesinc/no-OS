/***************************************************************************//**
 *   @file   voltage_output.c
 *   @brief  Voltage Output example code for ad74416h-pmdz project
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

/***************************************************************************//**
 * @brief Voltage output example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74416h_desc;
	int ret;

	uint16_t reg_value = 0xAAAA;
	uint32_t dac_code = 0;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure voltage range to +-12 V
	ret = ad74416h_set_channel_vout_range(ad74416h_desc, 0,
					      AD74416H_VOUT_RANGE_NEG12_12V);
	if (ret) {
		pr_info("Error setting Channel 0 to range +-12 V");
		goto error_ad74416h;
	}

	//Configure Current Limit for channel A in Vout mode to 8mA
	ret = ad74416h_set_channel_i_limit(ad74416h_desc, 0, AD74416H_I_LIMIT1);
	if (ret) {
		pr_info("Error setting the current limit to 8mA");
		goto error_ad74416h;
	}

	//Configure Channel A as Voltage Output
	ret = ad74416h_set_channel_function(ad74416h_desc, 0, AD74416H_VOLTAGE_OUT);
	if (ret) {
		pr_info("Error setting Channel 0 as voltage output");
		goto error_ad74416h;
	}

	//Configure Channel A code to -6V output
	ad74416h_dac_voltage_to_code(ad74416h_desc, -6000, &dac_code, 0);
	pr_info("Calculated DAC CODE for -6V is: 0x%0x\r\n", dac_code);
	ret = ad74416h_set_channel_dac_code(ad74416h_desc, 0, dac_code);

	ret = ad74416h_reg_read(ad74416h_desc, AD74416H_DAC_CODE(0), &reg_value);
	pr_info("DAC CODE = 0x%0x\r\n", reg_value);

error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error %d !\r\n", ret);
	return 0;
}
