/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic (non-IIO) temperature readout example for eval-cn0391-ardz.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
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
#include "cn0391.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

int example_main(void)
{
	struct cn0391_dev *dev;
	struct no_os_uart_desc *uart;
	double hot_temp, cold_temp, tc_voltage, rtd_res;
	int ret;

	ret = no_os_uart_init(&uart, &cn0391_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	ret = cn0391_init(&dev, &cn0391_ip);
	if (ret) {
		pr_info("CN0391 init failed: %d\n", ret);
		return ret;
	}

	pr_info("CN0391 Thermocouple Measurement\n");

	while (1) {
		for (uint8_t ch = CN0391_CH0_ID; ch < CN0391_NUM_IIO_CHANNELS; ch++) {
			ret = cn0391_read_temperature(dev, ch, &hot_temp, &cold_temp,
						      &tc_voltage, &rtd_res);
			if (ret) {
				pr_info("CH%u read failed: %d\n", ch, ret);
				continue;
			}
			printf("CH%u: %.2f C\n", ch, hot_temp);
		}
		printf("---\n");

		no_os_mdelay(2000);
	}

	cn0391_remove(dev);

	return 0;
}
