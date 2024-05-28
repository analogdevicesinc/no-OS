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

/* Default structures */
static struct xil_gpio_init_param xil_gpio_param = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS
};

static struct no_os_gpio_init_param default_gpio_param = {
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

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

int main()
{
	int32_t err;

	pr_debug("Hey, welcome to ad3552r_fmcz example\n");

	err = init_gpios_to_defaults();
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("init_gpios_to_defaults failed: %"PRIi32"\n", err);
		return err;
	}

	struct no_os_gpio_init_param ldac_param = default_gpio_param;
	struct no_os_gpio_init_param reset_param = default_gpio_param;

	ldac_param.number = GPIO_OFFSET + GPIO_LDAC_N;
	reset_param.number = GPIO_OFFSET + GPIO_RESET_N;

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.flags = 0
	};

	struct ad3552r_init_param default_ad3552r_param = {
		.chip_id = AD3552R_ID,
		.spi_param = {
			.device_id = SPI_DEVICE_ID,
			.chip_select = 0,
			.mode = NO_OS_SPI_MODE_0,
			.max_speed_hz = 20000000,
			.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
#ifdef XPAR_XSPI_NUM_INSTANCES
			.platform_ops = &xil_spi_pl_ops,
#else
			.platform_ops = &xil_spi_ops,
#endif
			.extra = &xil_spi_param
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
		},
		.crc_en = 0,
#ifdef XILINX_PLATFORM
		/*
		 * Zed board requires this option, spi instruction/addr + data
		 * must be sent in a single transfer.
		 */
		.single_transfer = 1,
#endif
	};

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

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

