/***************************************************************************//**
 *   @file   generic_spi_example.c
 *   @brief  Implementation of Main Function.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <inttypes.h>
#include "common_data.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "ad3552r.h"

#include <xparameters.h>
#include <xil_cache.h>

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_ad3552r.h"

static uint8_t data_buffer[MAX_BUFF_SAMPLES];

#endif

int32_t init_gpios_to_defaults()
{
	const uint8_t gpios_initial_value[][2] = {
		[GPIO_RESET_N] = {NO_OS_GPIO_OUT, NO_OS_GPIO_HIGH},
		[GPIO_LDAC_N] = {NO_OS_GPIO_OUT, NO_OS_GPIO_HIGH},
		[GPIO_SPI_QPI] = {NO_OS_GPIO_OUT, NO_OS_GPIO_LOW},
		[GPIO_ALERT_N] = {NO_OS_GPIO_IN, 0},
		[GPIO_9] = {NO_OS_GPIO_OUT, NO_OS_GPIO_HIGH},
		[GPIO_RED] = {NO_OS_GPIO_OUT, NO_OS_GPIO_HIGH},
		[GPIO_GREEN] = {NO_OS_GPIO_OUT, NO_OS_GPIO_HIGH},
		[GPIO_BLUE] = {NO_OS_GPIO_OUT, NO_OS_GPIO_HIGH},
	};
	struct no_os_gpio_desc *gpio;
	struct no_os_gpio_init_param param = default_gpio_param;
	uint32_t i;
	int32_t	 err;

	for (i = 0; i < TOTAL_GPIOS; i++) {
		param.number = GPIO_OFFSET + i;
		err = no_os_gpio_get(&gpio, &param);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
		if (gpios_initial_value[i][0] == NO_OS_GPIO_IN)
			err = no_os_gpio_direction_input(gpio);
		else
			err = no_os_gpio_direction_output(gpio,
							  gpios_initial_value[i][1]);

		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		no_os_gpio_remove(gpio);
	}

	return 0;
}

void set_power_up_success_led()
{
	struct no_os_gpio_desc *gpio;
	struct no_os_gpio_init_param param = default_gpio_param;

	param.number = GPIO_OFFSET + GPIO_GREEN;
	no_os_gpio_get(&gpio, &param);
	no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	no_os_gpio_remove(gpio);
}

extern const uint16_t no_os_sine_lut_16[512];

int32_t run_example(struct ad3552r_desc *dac)
{
	const uint32_t time_between_samples_us = 100;
	uint32_t nb_samples, i;
	uint16_t samples[2];
	int32_t err;

	nb_samples = NO_OS_ARRAY_SIZE(no_os_sine_lut_16);

	pr_debug("sending syn wave, %ld samples per period\n", nb_samples * 2);

	i = 0;
	do {
		samples[0] = no_os_sine_lut_16[i];
		samples[1] = no_os_sine_lut_16[(i + nb_samples / 2) % nb_samples];
		err = ad3552r_write_samples(dac, samples, 1,
					    AD3552R_MASK_ALL_CH,
					    AD3552R_WRITE_INPUT_REGS);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		no_os_udelay(time_between_samples_us);

		i = (i + 1) % nb_samples;
		err = ad3552r_ldac_trigger(dac, AD3552R_MASK_ALL_CH, false);
	} while (!NO_OS_IS_ERR_VALUE(err));

	return err;
}

int example_main()
{
	int32_t err;

	pr_debug("Hey, welcome to ad3552r_fmcz example\n");

	err = init_gpios_to_defaults();
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("init_gpios_to_defaults failed: %"PRIi32"\n", err);
		return err;
	}

#ifndef IIO_SUPPORT
	struct ad3552r_desc *dac;

	err = ad3552r_init(&dac, &default_ad3552r_param);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("ad3552r_init failed with code: %"PRIi32"\n", err);
		return err;
	}

	set_power_up_success_led();

	err = run_example(dac);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_debug("Example failed with code: %"PRIi32"\n", err);
		return err;
	}

	ad3552r_remove(dac);

#else //IIO_SUPPORT

	struct iio_ad3552r_desc *iio_dac;
	struct iio_device *iio_dac_descriptor;

	struct iio_data_buffer wr_buff = {
		.buff = data_buffer,
		.size = sizeof(data_buffer)
	};

	err = iio_ad3552r_init(&iio_dac, &default_ad3552r_param);
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("Error initializing iio_dac. Code: %"PRIi32"\n", err);
		return err;
	}

	set_power_up_success_led();

	iio_ad3552r_get_descriptor(iio_dac, &iio_dac_descriptor);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad3552r", iio_dac, iio_dac_descriptor, NULL,
			       &wr_buff, NULL)
	};

	err = 0;
	while (err >= 0) {
		err = iio_app_run(NULL, 0, devices, NO_OS_ARRAY_SIZE(devices));
	}

	iio_ad3552r_remove(iio_dac);
#endif

	pr_debug("Bye\n");

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

	return 0;
}

