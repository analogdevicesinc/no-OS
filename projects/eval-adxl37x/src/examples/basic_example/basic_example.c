/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of no-OS driver for eval-adxl37x project.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "adxl37x.h"
#include "common_data.h"
#include "no_os_util.h"

int example_main()
{
	/* adxl37x init and start SPI */
	struct adxl37x_dev *dev;
	struct adxl37x_init_param dev_init = adxl37x_init_param;
	int32_t ret;
	struct no_os_uart_desc *uart;
	struct adxl37x_xyz_accel_data recorded_data[MAX_SAMPLES];
	struct adxl37x_xyz_accel_data test_data[MAX_SAMPLES];
	uint16_t numberSamples = EXTRACTED_SAMPLES;
	uint8_t pwr_ctl;
	struct adxl37x_xyz_accel_data accel_data;
	uint8_t offset_data[3];
	int16_t x, y, z;



	int8_t off_x, off_y, off_z;
	int16_t raw_x, raw_y, raw_z;
	int32_t ms2_x, ms2_y, ms2_z;
	int32_t ma_x, ma_y, ma_z;

	ret = no_os_uart_init(&uart, &adxl37x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	ret = adxl37x_init(&dev, dev_init);
	if (ret)
		goto error_uart;

	ret = adxl37x_write_mask(dev, ADXL37X_POWER_CTL, ADXL37X_POWER_CTL_HPF_DIS_MSK,
				 ADXL37X_POWER_CTL_HPF_DIS_MODE(1));
	if (ret)
		goto error_dev;

	ret = adxl37x_read_reg(dev, ADXL37X_POWER_CTL, &pwr_ctl);
	if (ret)
		goto error_dev;

	pr_info("adxl37x initialized succesfully : pwr_ctl reg=%02X\n\r",
		pwr_ctl);

	ret = adxl37x_set_internal_sync(dev);
	if (ret) {
		pr_info("internal sync unsuccessful!\n\r");
		goto error_dev;
	}

	ret = adxl37x_write_reg(dev, ADXL37X_OFFSET_X, -1 & 0x0F);
	if (ret)
		goto error_dev;
	ret = adxl37x_write_reg(dev, ADXL37X_OFFSET_Y, 0 & 0x0F);
	if (ret)
		goto error_dev;
	ret = adxl37x_write_reg(dev, ADXL37X_OFFSET_Z, 1 & 0x0F);
	if (ret)
		goto error_dev;

	pr_info("offset trims: OFFSET_X=%d , OFFSET_Y=%d , OFFSET_Z=%d \n\r",
		off_x, off_y, off_z);

	while (1) {
		no_os_mdelay(1000);

		ret = adxl37x_get_xyz_data(dev, &accel_data);
		if (ret)
			goto error_dev;

		ms2_x = ((int32_t)(accel_data.x) * 980665) / 10000;
		ms2_y = ((int32_t)(accel_data.y) * 980665) / 10000;
		ms2_z = ((int32_t)(accel_data.z) * 980665) / 10000;

		ma_x = (ms2_x < 0) ? -ms2_x : ms2_x;
		ma_y = (ms2_y < 0) ? -ms2_y : ms2_y;
		ma_z = (ms2_z < 0) ? -ms2_z : ms2_z;

		pr_info("x: raw=%d (%s%d.%02d m/s^2) | y: raw=%d (%s%d.%02d m/s^2) | z: raw=%d (%s%d.%02d m/s^2)\n\r",
			(accel_data.x), (ms2_x < 0) ? "-" : "", (int)(ma_x / 100), (int)(ma_x % 100),
			(accel_data.y), (ms2_y < 0) ? "-" : "", (int)(ma_y / 100), (int)(ma_y % 100),
			(accel_data.z), (ms2_z < 0) ? "-" : "", (int)(ma_z / 100), (int)(ma_z % 100));
	}

error_dev:
	adxl37x_remove(dev);
error_uart:
	no_os_uart_remove(uart);
	pr_info("Error!\n");

	return ret;
}
