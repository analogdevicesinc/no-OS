/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"

#include "capi/capi_spi.h"
#include "capi/capi_uart.h"
#include "adxl355.h"

#include "platform.h"

int main()
{
	struct adxl355_frac_repr x_accel, y_accel, z_accel;
	struct adxl355_frac_repr temp;
	struct adxl355_dev *adxl355;

	/* Handle for the SPI controller. Will be dynamically allocated. */
	struct capi_spi_controller_handle *spi_controller = NULL;
	struct capi_uart_handle *uart = CAPI_UART_HANDLE;
	int ret;

	/* Initialize UART for printf output */
	struct capi_uart_config uart_config = {
		.identifier = 0,
		.clk_freq_hz = 115200,
		.ops = &maxim_uart_ops,
	};

	no_os_init();

	/* Initialize the UART controller for printf. */
	ret = capi_uart_init(&uart, &uart_config);
	if (ret) {
		return ret;
	}

	capi_uart_stdio(uart);
	printf("ADXL355 Accelerometer Exercise 2\n");

	/* Initialize SPI controller */
	struct capi_spi_config adxl355_spi_config = {
		.identifier = 0,
		.clk_freq_hz = 1000000,
		.ops = &maxim_spi_ops,
	};

	/* 
	 * Initialize the SPI controller. The adxl355_spi_config struct
	 * doesn't specify many of the parameters used during SPI transactions
	 * (e.g the mode, the chip select control type, delays etc.). This is
	 * because there could be multiple devices connected to the same SPI
	 * bus, each requiring a different set of parameters. Thus, those parameters
	 * are given using the capi_spi_device struct, for each transaction. Depending
	 * on the platform SPI driver implementation, the SPI controller will be reconfigured
	 * on every transfer.
	 */
	ret = capi_spi_init(&spi_controller, &adxl355_spi_config);
	if (ret) {
		printf("Error initializing SPI: %d\n", ret);
		return ret;
	}

	/* SPI parameters for the transfers to the ADXL355 */
	struct capi_spi_device adxl355_spi_dev = {
		.controller = spi_controller,
		.native_cs = 0,
		.mode = CAPI_SPI_MODE_0,
		.max_speed_hz = 1000000,
	};

	/* 
	 * Initialization parameters specific for the ADXL355 driver.
	 * The SPI device is passed in this struct.
	 */
	struct adxl355_init_param adxl355_param = {
		.spi_controller = spi_controller,
		.comm_param = adxl355_spi_config,
		.dev_type = ID_ADXL355,
		.spi_dev = &adxl355_spi_dev,
	};

	/* 
	 * TODO: Initialize the adxl355 descriptor.
	 * Hint: Check the adxl355.h file for the required function.
	 */
	ret = adxl355_init(&adxl355, adxl355_param);
	if (ret) {
		printf("Error initializing ADXL355: %d\n", ret);
		return ret;
	}

	printf("ADXL355 initialized successfully\n");

	/* Perform soft reset */
	ret = adxl355_soft_reset(adxl355);
	if (ret) {
		printf("Error performing soft reset: %d\n", ret);
		return ret;
	}

	/* Configure output data rate */
	ret = adxl355_set_odr_lpf(adxl355, ADXL355_ODR_62_5HZ);
	if (ret) {
		printf("Error setting ODR: %d\n", ret);
		return ret;
	}

	/* Set operation mode to measurement with temperature on */
	ret = adxl355_set_op_mode(adxl355, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret) {
		printf("Error setting operation mode: %d\n", ret);
		return ret;
	}

	printf("ADXL355 configured. Starting data acquisition...\n\n");

	/* Main loop - read and print acceleration and temperature data */
	while (1) {
		/* 
		 * TODO: Read acceleration data.
		 * Hint: Look into the adxl355.h header for the adxl355_get_xyz() function
		 */
		ret = adxl355_get_xyz(adxl355, &x_accel, &y_accel, &z_accel);
		if (ret) {
			printf("Error reading acceleration data: %d\n", ret);
		} else {
			/* Print acceleration data in m/s^2 */
			printf("X: %lld.%03d m/s^2, ",
			       (long long)x_accel.integer,
			       abs(x_accel.fractional));
			printf("Y: %lld.%03d m/s^2 ",
			       (long long)y_accel.integer,
			       abs(y_accel.fractional));
			printf("Z: %lld.%03d m/s^2",
			       (long long)z_accel.integer,
			       abs(z_accel.fractional));
		}

		/* Read temperature data */
		ret = adxl355_get_temp(adxl355, &temp);
		if (ret) {
			printf(" | Temp: Error %d\n", ret);
		} else {
			/* Print temperature in Celsius */
			printf(" | Temp: %lld.%03d C\n",
			       (long long)temp.integer / 1000,
			       abs((int)(temp.integer % 1000)));
		}

		/* Delay between readings */
		no_os_mdelay(500);
	}

	/* Cleanup (this code is never reached in the infinite loop) */
	adxl355_remove(adxl355);

	return 0;
}
