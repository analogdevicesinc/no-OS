/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example for eval-adxl366z project
 *   @author Marco Ramirez (marco.ramirez@analog.com)
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

#include "basic_example.h"
#include "common_data.h"
#include "adxl366.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/* ADXL366 scale factor for default ±1g range, 14-bit signed data
 * Full-scale range: 2g (-1g to +1g)
 * Resolution: 2g / 2^13 = 2g / 8192 = 0.000244 g/LSB
 */
#define ADXL366_SCALE_FACTOR_1G  0.000244f

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
 */
int basic_example_main()
{
	struct adxl366_dev *adxl366_desc;
	int ret;
	int16_t x, y, z;
	float x_g, y_g, z_g;
	uint8_t devid_ad, devid_mst, partid;

	pr_info("ADXL366 Basic Example\n");
	pr_info("=====================\n\n");

	/* Initialize ADXL366 */
	pr_info("Initializing ADXL366...\n");
	ret = adxl366_init(&adxl366_desc, adxl366_ip);
	if (ret) {
		pr_err("ADXL366 initialization failed: %d\n", ret);
		return ret;
	}

	/* Read device IDs */
	ret = adxl366_reg_read(adxl366_desc, ADXL366_REG_DEVID_AD, &devid_ad);
	if (ret)
		goto error;

	ret = adxl366_reg_read(adxl366_desc, ADXL366_REG_DEVID_MST, &devid_mst);
	if (ret)
		goto error;

	ret = adxl366_reg_read(adxl366_desc, ADXL366_REG_PARTID, &partid);
	if (ret)
		goto error;

	pr_info("Device IDs: 0x%02X 0x%02X 0x%02X\n", devid_ad, devid_mst, partid);

	if (devid_ad != 0xAD || devid_mst != 0x1D) {
		pr_err("Invalid device IDs!\n");
		pr_err("Expected: 0xAD 0x1D, Got: 0x%02X 0x%02X\n", devid_ad, devid_mst);
		ret = -1;
		goto error;
	}

	pr_info("ADXL366 initialized successfully!\n\n");

	/* Set measurement mode */
	ret = adxl366_set_power_mode(adxl366_desc, ADXL366_POWER_CTL_MEASURE);
	if (ret) {
		pr_err("Failed to set measurement mode: %d\n", ret);
		goto error;
	}

	pr_info("Reading accelerometer data...\n");
	pr_info("(Press Ctrl+C to stop)\n\n");

	/* Continuous data reading */
	while (1) {
		ret = adxl366_read_accel(adxl366_desc, &x, &y, &z);
		if (ret) {
			pr_err("Failed to read accelerometer data: %d\n", ret);
			goto error;
		}

		/* Convert to g (gravity) */
		x_g = (float)x * ADXL366_SCALE_FACTOR_1G;
		y_g = (float)y * ADXL366_SCALE_FACTOR_1G;
		z_g = (float)z * ADXL366_SCALE_FACTOR_1G;

		pr_info("X: %+7.4f g  Y: %+7.4f g  Z: %+7.4f g\n", x_g, y_g, z_g);

		no_os_mdelay(500);  /* 500ms delay between readings */
	}

error:
	adxl366_remove(adxl366_desc);
	if (ret)
		pr_err("Example failed with error: %d\n", ret);
	return ret;
}
