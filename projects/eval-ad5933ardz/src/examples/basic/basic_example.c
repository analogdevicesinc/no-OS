/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for eval-ad5933ardz project
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
 ********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include "ad5933.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include <stdio.h>

/* Sweep definition: start frequency, increment and number of increments. */
#define BASIC_START_FREQ_HZ 2000  /* 30 kHz start frequency. */
#define BASIC_FREQ_INC_HZ   100    /* 1 kHz per step.         */
#define BASIC_INC_NUM       10 	/* 10 steps (11 points).   */

#define AD5933_CALIBRATION_RESISTOR_OHM 500  /* calibration resistor value used. */

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int example_main(void)
{
	struct ad5933_dev *dev;
	float temperature;
	double impedance;
	double gain_factor;
	int imp_milli;
	int freq;
	int ret;
	uint8_t freq_function;

	printf("*** Starting AD5933 Example ***\r\n");
	printf("Initializing AD5933...\r\n");

	/* init_param is passed by pointer now. */
	ret = ad5933_init(&dev, &ad5933_user_init);

	if (ret) {
		printf("ERROR: AD5933 init failed: %d\r\n", ret);
		goto error;
	}

	printf("AD5933 init successful\r\n");

	ret = ad5933_setup(dev);

	if (ret)
		goto error_remove;

	printf("Requesting temperature conversion...\r\n");

	ret = ad5933_get_temperature(dev, &temperature);

	if (ret) {
		printf("ERROR: Temperature read failed: %d\r\n", ret);
		goto error_remove;
	}

	printf("AD5933 on-chip temperature: %.2f degC\r\n", temperature);

	ret = ad5933_config_sweep(dev, BASIC_START_FREQ_HZ,
				  BASIC_FREQ_INC_HZ, BASIC_INC_NUM);

	if (ret)
		goto error_remove;

	ret = ad5933_initialize_sweep(dev);
	if (ret)
		goto error_remove;

	ret = ad5933_start_sweep(dev);
	if (ret)
		goto error_remove;

	ret = ad5933_wait_status(dev, AD5933_STAT_DATA_VALID, NULL);
	if (ret)
		goto error_remove;

	if (ret)
		goto error_remove;

	for (int i = 0; i < BASIC_INC_NUM; i++) {
		freq = i == 0 ? BASIC_START_FREQ_HZ : BASIC_START_FREQ_HZ +
		       (BASIC_FREQ_INC_HZ * i);

		if (i == 0) {
			ret = ad5933_repeat_freq(dev);
		} else {
			ret = ad5933_increment_freq(dev);
		}

		ret = ad5933_wait_status(dev, AD5933_STAT_DATA_VALID, NULL);
		if (ret)
			goto error_remove;

		ret = ad5933_calculate_gain_factor(dev, &gain_factor,
						   AD5933_CALIBRATION_RESISTOR_OHM);
		if (ret)
			goto error_remove;

		ret = ad5933_repeat_freq(dev);
		if (ret)
			goto error_remove;

		ret = ad5933_wait_status(dev, AD5933_STAT_DATA_VALID, NULL);
		if (ret)
			goto error_remove;


		ret = ad5933_calculate_impedance(dev, gain_factor, &impedance);

		if (ret)
			goto error_remove;
		imp_milli = (int)(impedance * 1000.0 + 0.5);
		pr_info("Frequency: %d Hz, Impedance: %d.%03d\n", freq, imp_milli / 1000,
			imp_milli % 1000);
	}

	ad5933_remove(dev);

	return 0;

error_remove:
	ad5933_remove(dev);
error:
	printf("ERROR: Exiting example\r\n");
	return ret;
}
