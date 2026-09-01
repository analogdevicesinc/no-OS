/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for eval-ad5933ardz project
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
 ********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include "ad5933.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include <stdio.h>

/* Sweep definition: start frequency, increment and number of increments. */
#define BASIC_START_FREQ_HZ 				30000  	/* 30 kHz start frequency. */
#define BASIC_FREQ_INC_HZ   				1000    /* 1 kHz per step.         */
#define BASIC_INC_NUM       				10 		/* 10 steps (11 points).   */

/*****************************************************************************
 * @brief Basic example main execution. If successful, will print impedances
 * read across the frequency points in the configured sweep parameters to the
 * serial monitor.
 *
 * @return ret - Result of the example execution.
 *******************************************************************************/
int example_main(void)
{
	struct ad5933_dev *dev;
	float temperature;
	double impedance;
	double gain_factor;
	int16_t real, imag;
	int imp_milli;
	int freq;
	int ret;

	double calibration_impedance;
	enum meas_mode measurement_mode = AD5933_MEAS_MODE_2W;

	printf("*** Starting AD5933 Example ***\r\n");
	printf("Initializing AD5933...\r\n");

	/* init_param is passed by pointer now. */
	ret = ad5933_init(&dev, &ad5933_user_init);

	if (ret) {
		printf("ERROR: AD5933 init failed: %d\r\n", ret);
		goto error;
	}

	printf("AD5933 init successful\r\n\r\n");

	ret = ad5933_setup(dev);

	if (ret)
		goto error_remove;

	printf("Requesting temperature conversion...\r\n");

	ret = ad5933_get_temperature(dev, &temperature);

	if (ret) {
		printf("ERROR: Temperature read failed: %d\r\n", ret);
		goto error_remove;
	}

	printf("AD5933 on-chip temperature: %.2f degC\r\n\r\n", temperature);

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

	if (measurement_mode == AD5933_MEAS_MODE_2W) {
		printf("Measurement mode: 2-wire\r\n");
		printf("Make sure to put J3 jumper in I_MEASURE position.\r\n");

		/* Available: 1, 10, 100, 1k, 10k, 100k */
		calibration_impedance = 1000;
	} else {
		printf("Measurement mode: 4-wire\r\n");
		printf("Make sure to put J3 jumper in V_MEASURE position.\r\n");

		/* Available: 1, 10, 100, 1k */
		calibration_impedance = 10;
	}

	for (int i = 0; i <= BASIC_INC_NUM; i++) {
		freq = i == 0 ? BASIC_START_FREQ_HZ : BASIC_START_FREQ_HZ +
		       (BASIC_FREQ_INC_HZ * i);

		if (i == 0) {
			ret = ad5933_repeat_freq(dev);
			if (ret)
				goto error_remove;
		} else {
			ret = ad5933_increment_freq(dev);
			if (ret)
				goto error_remove;
		}

		ret = ad5933_wait_status(dev, AD5933_STAT_DATA_VALID, NULL);
		if (ret)
			goto error_remove;

		ret = ad5933_calculate_gain_factor(dev, &gain_factor,
						   calibration_impedance, measurement_mode);
		if (ret)
			goto error_remove;

		printf("Gain Factor: %.12f\n", gain_factor);

		ret = ad5933_repeat_freq(dev);
		if (ret)
			goto error_remove;

		ret = ad5933_wait_status(dev, AD5933_STAT_DATA_VALID, NULL);
		if (ret)
			goto error_remove;

		ret = ad5933_get_current_data(dev, &real, &imag);
		if (ret)
			goto error_remove;

		ret = ad5933_calculate_impedance(dev, gain_factor, &impedance,
						 measurement_mode);
		if (ret)
			goto error_remove;

		printf("Real: %d, Imag: %d\r\n", real, imag);

		imp_milli = (int)(impedance * 1000.0 + 0.5);
		pr_info("Frequency: %d Hz, Impedance: %d.%03d\r\n\r\n", freq, imp_milli / 1000,
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
