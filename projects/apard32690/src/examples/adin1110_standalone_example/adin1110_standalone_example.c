/***************************************************************************//**
 *   @file   adin1110_standalone_example.c
 *   @brief  Implementation of the ADIN1110 standalone example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "common_data.h"

#include "no_os_error.h"
#include "adin1110.h"

/***************************************************************************//**
 * @brief Configure the ADIN1110 and read the device ID.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int adin1110_standalone_example_main()
{
	struct no_os_uart_desc *uart_desc;
	struct adin1110_desc *adin1110;
	uint32_t device_id;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adin1110_init(&adin1110, &adin1110_ip);
	if (ret) {
		printf("Error during ADIN1110 config (%d)\n", ret);
		return ret;
	}

	ret = adin1110_reg_read(adin1110, 0x1, &device_id);
	if (ret) {
		printf("Error reading the ADIN1110's device id (%d)\n", ret);
		goto out;
	}

	printf("Got device id 0x%X\n", device_id);

out:
	adin1110_remove(adin1110);

	return 0;
}
