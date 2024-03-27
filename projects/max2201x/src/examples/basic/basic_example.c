/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "basic_example.h"
#include "common_data.h"
#include "max2201x.h"
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

	struct max2201x_desc *max2201x_desc;
	struct max2201x_gpio_init_param max2201x_gpio_ip;
	struct no_os_gpio_init_param max2201x_gpio0_ip = {
		.port = 0,
		.pull = NO_OS_PULL_NONE,
		.number = 0,
		.platform_ops = &max2201x_gpio_ops,
		.extra = &max2201x_gpio_ip,
	};
	struct no_os_gpio_desc *gpio0;

	ret = max2201x_init(&max2201x_desc, &max2201x_ip);
	if (ret)
		goto exit;

	/** Initializing GPIO 0 of MAX2201X. */
	max2201x_gpio_ip.max2201x_desc = max2201x_desc;
	ret = no_os_gpio_get(&gpio0, &max2201x_gpio0_ip);
	if (ret)
		goto remove_max2201x;

	/** Setting its value to HIGH. */
	ret = no_os_gpio_direction_output(gpio0, NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_gpio0;

	/** Changing operation mode of MAX2201X. */
	ret = max2201x_op_mode(max2201x_desc, MAX2201X_CH1,
			       MAX2201X_OUTPUT_VOLTAGE_NO_MONITORRING);
	if (ret)
		goto remove_gpio0;

	/** Changing configuration of MAX2201X. */
	ret = max2201x_config(max2201x_desc, MAX2201X_CH1, MAX2201X_UNIPOLAR,
			      MAX2201X_50_OHM_RESISTOR, false);
	if (ret)
		goto remove_gpio0;

	/** Changing slew-rate configuration of MAX2201X. */
	ret = max2201x_slew_rate(max2201x_desc, MAX2201X_CH1,
				 MAX2201X_STEP_SIZE_4096BITS, MAX2201X_UPDATE_RATE_4KHZ_2KHZ, true, false);
	if (ret)
		goto remove_gpio0;

	/** Reading the GAIN correction. */
	ret = max2201x_reg_read(max2201x_desc, MAX2201X_AO_GAIN_CORR_CH1,
				(uint32_t *)&data);
	if (ret)
		goto remove_gpio0;

	/* Creating a triangle waveform.
	 * Initial value is 0.
	 */
	data = 0;
	while (1) {
		ret = max2201x_set_data(max2201x_desc, MAX2201X_CH1, data);
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
remove_max2201x:
	max2201x_remove(max2201x_desc);
exit:
	if (ret)
		pr_err("Error!\n");

	return ret;
}
