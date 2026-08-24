/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltm4700 project.
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
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
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "ltm4700.h"
#include <stdlib.h>

#define TELEMETRY_DISPLAY_DELAY_MS	500

int example_main()
{
	struct ltm4700_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret = 0, vals[4];
	uint8_t chan;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	pr_info("\e[2J\e[H");
	pr_info("LTM4700 basic example.\n");

	ret = ltm4700_init(&dev, &ltm4700_ip);
	if (ret) {
		pr_err("\nLTM4700 init failed with error code: %d\n", ret);
		return ret;
	}
	pr_info("LTM4700 initialized successfully!\n"); // debug

	ret = ltm4700_set_operation(dev, LTM4700_CHAN_ALL,
				    LTM4700_OPERATION_ON);
	if (ret)
		goto exit;

	while (1) {
		for (chan = LTM4700_CHAN_0; chan <= LTM4700_CHAN_1; chan++) {
			ret = ltm4700_read_value(dev, chan, LTM4700_VIN,
						 &vals[0]);
			if (ret)
				goto exit;

			ret = ltm4700_read_value(dev, chan, LTM4700_VOUT,
						 &vals[1]);
			if (ret)
				goto exit;

			ret = ltm4700_read_value(dev, chan, LTM4700_IOUT,
						 &vals[2]);
			if (ret)
				goto exit;

			ret = ltm4700_read_value(dev, chan, LTM4700_TEMP_IC,
						 &vals[3]);
			if (ret)
				goto exit;

			pr_info("CH%d: vin = %d mV | vout = %d mV | iout = %d mA | temp_ic = %d mC\n",
				chan, vals[0], vals[1], vals[2], vals[3]);
		}

		pr_info("\n");
		no_os_mdelay(TELEMETRY_DISPLAY_DELAY_MS);
	}


exit:
	ltm4700_remove(dev);

	return ret;
}
