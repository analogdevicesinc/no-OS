/***************************************************************************//**
 *   @file   axi_qspi_example.c
 *   @brief  Implementation of Main Function.
 *   @author Angelo Dureghello (adureghello@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#define NUM_CYCLES 8

extern const uint16_t no_os_sine_lut_16[512];

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

int32_t run_example(struct ad3552r_desc *dac)
{
	int32_t err;
	uint16_t samples[2] __attribute__ ((aligned));

	samples[0] = 65534;
	samples[1] = 0;

	pr_info("Writing raw samples, ch0/1 %d/%d, using LDAC\n",
		samples[0], samples[1]);

	err = ad3552r_write_samples(dac, samples, 1,
				    AD3552R_MASK_ALL_CH,
				    AD3552R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC);
	if (err) {
		pr_info("error writing samples\n");
		return err;
	}

	no_os_mdelay(1000);

	samples[0] = 0;
	samples[1] = 65534;

	pr_info("Writing raw samples, ch0/1 %d/%d, direct DAC REG write\n",
		samples[0], samples[1]);

	err = ad3552r_write_samples(dac, samples, 1,
				    AD3552R_MASK_ALL_CH,
				    AD3552R_WRITE_DAC_REGS);
	if (err) {
		pr_info("error writing samples\n");
		return err;
	}

	no_os_mdelay(1000);

	pr_info("Fast cyclic dma transfer starts now, for 20 seconds ...\n");

	/*
	 * Setting 20 seconds in this example,
	 * use cyclic_secs to 0 if continuous cycling is desired.
	 */
	return ad3552r_axi_write_data(dac, (uint32_t *)no_os_sine_lut_16,
				      256, true, 20);
}

int example_main()
{
	int32_t err;
	struct ad3552r_desc *dac;

	pr_info("Hey, welcome to ad3552r_fmcz AXI example\n");

	err = init_gpios_to_defaults();
	if (NO_OS_IS_ERR_VALUE(err)) {
		pr_err("init_gpios_to_defaults failed: %"PRIi32"\n", err);
		return err;
	}

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

	pr_info("Example completed, bye !\n");

	return 0;
}

