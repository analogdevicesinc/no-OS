/***************************************************************************//**
 *   @file   voltage_input.c
 *   @brief  Voltage input example code for ad74416h-pmdz project
 *   @author Raquel Grau (raquel.grau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "voltage_input.h"
#include "common_data.h"
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include <math.h>

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Voltage input example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int voltage_input_example_main()
{
	struct ad74416h_desc *ad74416h_desc;
	int ret;

	union ad74416h_live_status status;
	uint32_t adc_value = 0;
	double calculated_voltage = 0;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure Channel A as Voltage Input
	ret = ad74416h_set_channel_function(ad74416h_desc, 0, AD74416H_VOLTAGE_IN);
	if (ret) {
		pr_info("Error setting Channel 0 as voltage input\r\n");
		goto error_ad74416h;
	}

	//Configure ADC Conversions
	//Configure the ADC sample rate
	ret = ad74416h_set_adc_rate(ad74416h_desc, 0, AD74416H_20SPS_50_60HZ_REJECTION);
	if (ret)
	{
		pr_info("Error setting sampling rate to 20SPS\r\n");
		goto error_ad74416h;
	}

	//Enable ADC A
	ret = ad74416h_set_adc_channel_enable(ad74416h_desc, 0, 1);
	if (ret)
	{
		pr_info("Error enabling ADC A\r\n");
		goto error_ad74416h;
	}
	//Set ADC A to continuous conversion
	ret = ad74416h_set_adc_conv_seq(ad74416h_desc, AD74416H_START_CONT);
	if (ret)
	{
		pr_info("Error enabling continuous conversions in ADC A\r\n");
		goto error_ad74416h;
	}
	
	//This example checks if there is available data in the ADC polling the status register
	while(1)
	{
		//Check if there is data ready in the ADC
		ret = ad74416h_get_live(ad74416h_desc, &status);
		if (ret)
		{
			pr_info("Error reading the live status register\r\n");
			goto error_ad74416h;
		}
		//If data is ready, read the ADC result
		if (status.status_bits.ADC_DATA_RDY == 1)
		{
			ret = ad74416h_get_raw_adc_result(ad74416h_desc, 0, &adc_value);
			if (ret)
			{
				pr_info("Error getting raw adc result in ADC A\r\n");
				goto error_ad74416h;
			}
			pr_info("ADC Input value = %0x\r\n", adc_value);
			//To calculate the Voltage use the formula
			//Voltage = V0 + (ADC_CODE/2^16) * voltage_range
			calculated_voltage = (double)adc_value/pow(2,24)*12;
			pr_info("Calculated voltage = %.5f V\r\n", calculated_voltage);
		}
	}

error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error!\r\n");
	return 0;
}
