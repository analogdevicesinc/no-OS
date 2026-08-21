/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic MAX42500 example
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdio.h>
#include "common_data.h"

/*******************************************************************************
 * @brief MAX42500 basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/

int example_main(void)
{
	int ret;
	struct no_os_uart_desc *uart_desc;
	struct max42500_dev *device;
	uint8_t off_stat;
	uint8_t ov_stat;
	uint8_t uv_stat;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto free_max42500;

	no_os_uart_stdio(uart_desc);

	printf("********** MAX42500 Voltage Monitor Tests **********\n\r");

	/* Initializing the device MAX42500 */
	ret = max42500_init(&device, &max42500_ip);
	if (ret)
		goto free_max42500;

	ret = max42500_set_nominal_voltage(device, MAX42500_VM1, 2);
	if (ret)
		goto free_max42500;

	ret = max42500_set_ov_thresh1(device, MAX42500_VM1, 10);
	if (ret)
		goto free_max42500;

	ret = max42500_set_uv_thresh1(device, MAX42500_VM1, 10);
	if (ret)
		goto free_max42500;

	while (1) {
		max42500_get_comp_status(device,
					 MAX42500_VM1, MAX42500_COMP_STAT_OFF,
					 &off_stat);
		max42500_get_comp_status(device,
					 MAX42500_VM1,
					 MAX42500_COMP_STAT_OV,
					 &ov_stat);
		max42500_get_comp_status(device,
					 MAX42500_VM1,
					 MAX42500_COMP_STAT_UV,
					 &uv_stat);

		if (off_stat) {
			printf("   IN%d status: Voltage is below OFF threshold\n\r",
			       MAX42500_VM1 + 1);
		} else if (uv_stat) {
			printf("   IN%d status: Undervoltage detected\n\r",
			       MAX42500_VM1 + 1);
		} else if (ov_stat) {
			printf("   IN%d status: Overvoltage detected\n\r",
			       MAX42500_VM1 + 1);
		} else {
			printf("   IN%d status: Voltage is within acceptable range\n\r",
			       MAX42500_VM1 + 1);
		}
	}

free_max42500:
	printf("Error!\n\r");
	max42500_remove(device);
	return 0;
}
