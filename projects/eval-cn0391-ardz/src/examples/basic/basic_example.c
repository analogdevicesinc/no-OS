/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic (non-IIO) temperature readout example for eval-cn0391-ardz.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
	uint8_t fail_count = 0;
	int ret;

	ret = no_os_uart_init(&uart, &cn0391_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	ret = cn0391_init(&dev, &cn0391_ip);
	if (ret)
		goto error;

	pr_info("CN0391 Thermocouple Measurement\n");

	while (1) {
		for (uint8_t ch = CN0391_CH0_ID; ch < CN0391_NUM_IIO_CHANNELS; ch++) {
			ret = cn0391_read_temperature(dev, ch, &hot_temp, &cold_temp,
						      &tc_voltage, &rtd_res);
			if (ret) {
				pr_info("CH%u read failed: %d\n", ch, ret);
				if (++fail_count >= 5)
					goto free_dev;
				continue;
			}
			fail_count = 0;
			printf("CH%u: %.2f C\n", ch, hot_temp);
		}
		printf("---\n");

		no_os_mdelay(2000);
	}

free_dev:
	cn0391_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
