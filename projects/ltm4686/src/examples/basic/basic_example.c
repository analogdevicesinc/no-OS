/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltm4686 project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#include "common_data.h"
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "ltm4686.h"

int basic_example_main()
{
	struct ltm4686_dev *dev;
	int ret = 0, vals[4];
	uint8_t chan;

	ret = ltm4686_init(&dev, &ltm4686_ip);
	if (ret)
		goto exit;

	ret = ltm4686_set_operation(dev, LTM4686_CHAN_ALL,
				    LTM4686_OPERATION_ON);
	if (ret)
		goto exit;

	ret = ltm4686_pwm_mode(dev, LTM4686_CHAN_ALL,
			       LTM4686_PWM_FORCED_CONTINUOUS_MODE);
	if (ret)
		goto exit;

	while (1) {
		for (chan = LTM4686_CHAN_0; chan <= LTM4686_CHAN_1; chan++) {
			ret = ltm4686_read_value(dev, chan, LTM4686_VIN,
						 &vals[0]);
			if (ret)
				goto exit;

			ret = ltm4686_read_value(dev, chan, LTM4686_VOUT,
						 &vals[1]);
			if (ret)
				goto exit;

			ret = ltm4686_read_value(dev, chan, LTM4686_IOUT,
						 &vals[2]);
			if (ret)
				goto exit;

			ret = ltm4686_read_value(dev, chan, LTM4686_TEMP_IC,
						 &vals[3]);
			if (ret)
				goto exit;

			pr_info("vin = %d mV | vout = %d mV | iout = %d mA | temp_ic = %d C\n",
				vals[0], vals[1], vals[2], vals[3] / 1000);
		}

		pr_info("\n");
		no_os_mdelay(500);
	}

exit:
	ltm4686_remove(dev);

	return ret;
}
