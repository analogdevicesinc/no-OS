/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "basic_example.h"
#include "common_data.h"
#include "max22017.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

int basic_example_main()
{
	int ret, data, sign = 1;

	/** Maximum value of 3V. */
	int max_value = 0x3DAE;

	/** The increment of the AO1 to be used (needs to be smaller then the SR step-size). */
	int increment = 0x100;

	/** Frequency of the signal. */
	int freq = 1000;

	struct max22017_desc *max22017_desc;
	struct max22017_gpio_init_param max22017_gpio_ip;
	struct no_os_gpio_init_param max22017_gpio0_ip = {
		.port = 0,
		.pull = NO_OS_PULL_NONE,
		.number = 0,
		.platform_ops = &max22017_gpio_ops,
		.extra = &max22017_gpio_ip,
	};
	struct no_os_gpio_desc *gpio0;

	ret = max22017_init(&max22017_desc, &max22017_ip);
	if (ret)
		goto exit;

	/** Initializing GPIO 0 of MAX22017. */
	max22017_gpio_ip.max22017_desc = max22017_desc;
	ret = no_os_gpio_get(&gpio0, &max22017_gpio0_ip);
	if (ret)
		goto remove_max22017;

	/** Setting its value to HIGH. */
	ret = no_os_gpio_direction_output(gpio0, NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_gpio0;

	/** Changing operation mode of MAX22017. */
	ret = max22017_op_mode(max22017_desc, MAX22017_CH1,
			       MAX22017_OUTPUT_VOLTAGE_NO_MONITORRING);
	if (ret)
		goto remove_gpio0;

	/** Changing configuration of MAX22017. */
	ret = max22017_config(max22017_desc, MAX22017_CH1, MAX22017_UNIPOLAR,
			      MAX22017_50_OHM_RESISTOR, false);
	if (ret)
		goto remove_gpio0;

	/** Changing slew-rate configuration of MAX22017. */
	ret = max22017_slew_rate(max22017_desc, MAX22017_CH1,
				 MAX22017_STEP_SIZE_4096BITS, MAX22017_UPDATE_RATE_4KHZ_2KHZ, true, false);
	if (ret)
		goto remove_gpio0;

	/** Reading the GAIN correction. */
	ret = max22017_reg_read(max22017_desc, MAX22017_AO_GAIN_CORR_CH1,
				(uint32_t *)&data);
	if (ret)
		goto remove_gpio0;

	/* Creating a triangle waveform.
	 * Initial value is 0.
	 */
	data = 0;
	while (1) {
		ret = max22017_set_data(max22017_desc, MAX22017_CH1, data);
		if (ret)
			goto remove_gpio0;

		if (data <= 0)
			sign = 1;

		if (data >= max_value)
			sign = -1;

		data += sign * increment;

		no_os_mdelay(1000 / freq);
	}

remove_gpio0:
	no_os_gpio_remove(gpio0);
remove_max22017:
	max22017_remove(max22017_desc);
exit:
	if (ret)
		pr_err("Error!\n");

	return ret;
}
