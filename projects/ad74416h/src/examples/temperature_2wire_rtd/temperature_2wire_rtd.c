/***************************************************************************//**
 *   @file   temperature_2wire_rtd.c
 *   @brief  Temperature measurement 2-Wire RTD Pt-1000 example code for ad74416h-pmdz project
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
#include <math.h>

/***************************************************************************//**
 * @brief RTD 2-wire example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74416h_desc;
	int ret;

	union ad74416h_live_status status;
	uint32_t adc_value = 0;
	double Rrtd = 0.0;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure Channel A as Resistance Measurement
	ret = ad74416h_set_channel_function(ad74416h_desc, 0, AD74416H_RESISTANCE);
	if (ret) {
		pr_info("Error setting Channel 0 as Resistance Measurement\r\n");
		goto error_ad74416h;
	}

	//Select SENSELF to AGND in the ADC input multiplexer
	ret = ad74416h_set_adc_conv_mux(ad74416h_desc, 0, AD74416H_MUX_LF_TO_AGND);
	if (ret)
		goto error;

	//Configure the RTD mode to 2-wire RTD, External RTD reference of 1V, RTD current 500uA
	ret = ad74416h_reg_write(ad74416h_desc, AD74416H_RTD_CONFIG(0), 0xC);
	if (ret)
		goto error;

	//Configure the ADC range to 0 to 12V
	ret = ad74416h_set_adc_range(ad74416h_desc, 0, AD74416H_RNG_0_12_V);
	if (ret)
		goto error;

	//Configure the ADC conversion rate
	ret = ad74416h_get_adc_rate(ad74416h_desc, 0, AD74416H_20SPS_50_60HZ_REJECTION);
	if (ret)
		goto error;

	//Enable ADC A
	ret = ad74416h_set_adc_channel_enable(ad74416h_desc, 0, 1);
	if (ret) {
		pr_info("Error enabling ADC A\r\n");
		goto error_ad74416h;
	}
	//Set ADC A to continuous conversion
	ret = ad74416h_set_adc_conv_seq(ad74416h_desc, AD74416H_START_CONT);
	if (ret) {
		pr_info("Error enabling continuous conversions in ADC A\r\n");
		goto error_ad74416h;
	}

	/* The following functions needs to be in the appropriate place for the
	   application (while loop, interrupt handler, etc.) */
	while (1) {
		//Check if there is data ready in the ADC
		ret = ad74416h_get_live(ad74416h_desc, &status);
		if (ret) {
			pr_info("Error reading the live status register\r\n");
			goto error_ad74416h;
		}
		//If data is ready, read the ADC result
		if (status.status_bits.ADC_DATA_RDY == 1) {
			ret = ad74416h_get_raw_adc_result(ad74416h_desc, 0, &adc_value);
			if (ret) {
				pr_info("Error getting raw adc result in ADC A\r\n");
				goto error_ad74416h;
			}
			pr_info("ADC Input value = 0x%0x\r\n", adc_value);
			//Interpret ADC Data
			//RTD = ADC_CODE * R_ref / (2^24 * ADC_gain)
			//For ADC range 0-12V --> ADC_gain = 1/4.8
			//This calculation is done in two steps to avoid a variable overflow
			Rrtd = (double)(adc_value) / pow(2, 24);
			Rrtd = Rrtd * 2012 * 4.8;
			pr_info("Rrtd value = %f\r\n", Rrtd);
		}
	}

error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error %d !\r\n", ret);
	return 0;
}
