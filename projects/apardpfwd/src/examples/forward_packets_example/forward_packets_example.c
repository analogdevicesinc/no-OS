/***************************************************************************//**
 *   @file   forward_packets_example.c
 *   @brief  Implementation of the forward packets example.
 *   @author Aron Kis (kis.aron@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include "common_data.h"

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "adin1110.h"

/***************************************************************************//**
 * @brief Configure the output port of the AD-APARDPFWD-SL then ping the
 *        downstream device.
 * @return ret - Result of the example execution.
*******************************************************************************/

int example_main()
{
	struct no_os_uart_desc *uart_desc;
	struct adin1110_desc *adin1110;

	uint32_t device_id;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		pr_err("UART initialization failed (%d)\n", ret);
		return ret;
	}

	no_os_uart_stdio(uart_desc);

	ret = spi_cfg_0(adin1110_spi_cfg_0);
	if (ret) {
		pr_err("ADIN2111 SPI configuration failed (%d)\n", ret);
		goto remove_uart;
	}

	pr_info("AD-APARDPFWD FORWARD PACKETS EXAMPLE.\n");

	ret = port2_cfg(port2_cfg_0, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("AD-APARDPFWD output port configuration failed (%d)\n", ret);
		goto remove_uart;
	}

	ret = adin1110_init(&adin1110, &adin1110_ip);
	if (ret) {
		pr_err("Error during ADIN1110 config (%d)\n", ret);
		goto remove_uart;
	}

	ret = adin1110_reg_read(adin1110, ADIN1110_PHY_ID_REG, &device_id);
	if (ret) {
		pr_err("Error reading the ADIN1110's device id (%d)\n", ret);
		goto remove_adin;
	}

	pr_info("Got device id 0x%X\n", device_id);

remove_adin:
	adin1110_remove(adin1110);

remove_uart:
	no_os_uart_remove(uart_desc);

	return 0;
}
