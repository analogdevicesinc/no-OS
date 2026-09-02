/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example code for eval-ad7191 project.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#include "ad7191.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * Cycles through both differential channels and the internal temperature
 * sensor, printing the raw code and the converted value for each.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad7191_dev *ad7191;
	uint32_t code;
	int64_t voltage_uv;
	int64_t temp_millideg;
	uint32_t gain;
	int ret;

	ret = ad7191_init(&ad7191, &ad7191_ip);
	if (ret) {
		pr_err("ad7191_init failed: %d\n", ret);
		return ret;
	}

	while (1) {
		/* Differential voltage channels. */
		for (enum ad7191_channel ch = AD7191_CH_AIN1_AIN2;
		     ch <= AD7191_CH_AIN3_AIN4; ch++) {
			ret = ad7191_single_conversion(ad7191, ch, &code);
			if (ret) {
				pr_err("conversion failed: %d\n", ret);
				goto error;
			}

			gain = ad7191_gain_table[ad7191->gain];
			/*
			 * Offset-binary code to microvolts:
			 *   V = (code - midscale) * vref / (gain * 2^23)
			 */
			voltage_uv = ((int64_t)code - AD7191_MIDSCALE) *
				     ad7191->vref_mv * 1000 /
				     ((int64_t)gain << (AD7191_RESOLUTION - 1));

			pr_info("AIN%d-AIN%d: code=0x%06lx  %lld uV\n",
				2 * ch + 1, 2 * ch + 2, (unsigned long)code,
				(long long)voltage_uv);
		}

		/* Internal temperature sensor. */
		ret = ad7191_single_conversion(ad7191, AD7191_CH_TEMP, &code);
		if (ret) {
			pr_err("conversion failed: %d\n", ret);
			goto error;
		}

		temp_millideg = ((int64_t)code - AD7191_MIDSCALE) * 1000 /
				AD7191_TEMP_CODES_PER_DEGREE - 273000;

		pr_info("Temp: code=0x%06lx  %lld mdegC\n",
			(unsigned long)code, (long long)temp_millideg);

		no_os_mdelay(1000);
	}

error:
	ad7191_remove(ad7191);
	return ret;
}
