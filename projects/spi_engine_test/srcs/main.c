/***************************************************************************//**
 *   @file   ad3552r_fmcz/srcs/main.c
 *   @brief  Implementation of Main Function.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include "app_config.h"
#include "parameters.h"
#include "no-os/error.h"
#include "no-os/print_log.h"
#include "no-os/spi.h"
#include "no-os/gpio.h"
#include "no-os/util.h"
#include "no-os/delay.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "ad3552r.h"

#include <xparameters.h>
#include <xil_cache.h>

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_ad3552r.h"

static uint8_t data_buffer[MAX_BUFF_SAMPLES];

#endif

/* Default structures */
static struct xil_gpio_init_param xil_gpio_param = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS
};

static struct gpio_init_param default_gpio_param = {
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

int32_t init_gpios_to_defaults()
{
	const uint8_t gpios_initial_value[][2] = {
		[GPIO_RESET_N] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_LDAC_N] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_SPI_QPI] = {GPIO_OUT, GPIO_LOW},
		[GPIO_ALERT_N] = {GPIO_IN, 0},
		[GPIO_SYNC_EVENTS] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_RED] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_GREEN] = {GPIO_OUT, GPIO_HIGH},
		[GPIO_BLUE] = {GPIO_OUT, GPIO_HIGH},
	};
	struct gpio_desc *gpio;
	struct gpio_init_param param = default_gpio_param;
	uint32_t i;
	int32_t	 err;

	for (i = 0; i < TOTAL_GPIOS; i++) {
		param.number = GPIO_OFFSET + i;
		err = gpio_get(&gpio, &param);
		if (IS_ERR_VALUE(err))
			return err;
		if (gpios_initial_value[i][0] == GPIO_IN)
			err = gpio_direction_input(gpio);
		else
			err = gpio_direction_output(gpio,
						    gpios_initial_value[i][1]);

		if (IS_ERR_VALUE(err))
			return err;

		gpio_remove(gpio);
	}

	return SUCCESS;
}

void set_power_up_success_led()
{
	struct gpio_desc *gpio;
	struct gpio_init_param param = default_gpio_param;

	param.number = GPIO_OFFSET + GPIO_GREEN;
	gpio_get(&gpio, &param);
	gpio_direction_output(gpio, GPIO_LOW);
	gpio_remove(gpio);
}

extern const uint16_t sine_lut_16[512];

int32_t run_example(struct ad3552r_desc *dac)
{
	const uint32_t time_between_samples_us = 100;
	uint32_t nb_samples, i;
	uint16_t samples[2];
	int32_t err;

	nb_samples = ARRAY_SIZE(sine_lut_16);
	i = 0;
	do {
		samples[0] = sine_lut_16[i];
		samples[1] = sine_lut_16[(i + nb_samples / 2) % nb_samples];
		err = ad3552r_write_samples(dac, samples, 1,
					    AD3552R_MASK_ALL_CH,
					    AD3552R_WRITE_INPUT_REGS);
		if (IS_ERR_VALUE(err))
			return err;

		udelay(time_between_samples_us);

		i = (i + 1) % nb_samples;
		err = ad3552r_ldac_trigger(dac, AD3552R_MASK_ALL_CH);
	} while (!IS_ERR_VALUE(err));

	return err;
}

int main()
{
	int32_t err;

	pr_debug("Hey, welcome to ad3552r_fmcz example\n");

	err = init_gpios_to_defaults();
	if (IS_ERR_VALUE(err)) {
		pr_err("init_gpios_to_defaults failed: %"PRIi32"\n", err);
		return err;
	}

	struct gpio_init_param ldac_param = default_gpio_param;
	struct gpio_init_param reset_param = default_gpio_param;

	ldac_param.number = GPIO_OFFSET + GPIO_LDAC_N;
	reset_param.number = GPIO_OFFSET + GPIO_RESET_N;

	struct ad3552r_init_param default_ad3552r_param = {
		.chip_id = AD3542R_ID,
		.spi_param = {
			.device_id = SPI_DEVICE_ID,
			.chip_select = 0,
			.mode = SPI_MODE_0,
			.bit_order = SPI_BIT_ORDER_MSB_FIRST,
			.platform_ops = &xil_spi_reg_ops_pl,
			.extra = NULL
		},
		.ldac_gpio_param_optional = &ldac_param,
		.reset_gpio_param_optional = &reset_param,
		.channels = {
			[0] = {
				.en = 1,
				.range = AD3542R_CH_OUTPUT_RANGE_0__2P5V
			},
			[1] = {
				.en = 1,
				.range = AD3542R_CH_OUTPUT_RANGE_0__2P5V
			}
		}
	};

#ifndef IIO_SUPPORT
	struct ad3552r_desc *dac;

	err = ad3552r_init(&dac, &default_ad3552r_param);
	if (IS_ERR_VALUE(err)) {
		pr_err("ad3552r_init failed with code: %"PRIi32"\n", err);
		return err;
	}

	set_power_up_success_led();

	err = run_example(dac);
	if (IS_ERR_VALUE(err)) {
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
	if (IS_ERR_VALUE(err)) {
		pr_err("Error initializing iio_dac. Code: %"PRIi32"\n", err);
		return err;
	}

	set_power_up_success_led();

	iio_ad3552r_get_descriptor(iio_dac, &iio_dac_descriptor);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad3552r", iio_dac, iio_dac_descriptor, NULL,
			       &wr_buff)
	};

	err = 0;
	while (err >= 0) {
		err = iio_app_run(devices, ARRAY_SIZE(devices));
	}

	iio_ad3552r_remove(iio_dac);
#endif

	pr_debug("Bye\n");

	return 0;
}

