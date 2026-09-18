/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for ADAR300X: SPI and power validation.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdio.h>
#include "basic_example.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

/*
 * adar300x_init() has already proven the SPI link and checked CHIPTYPE; report
 * which part answered, with its product ID and SPI revision.
 */
static int adar300x_identify_demo(struct adar300x_dev *dev)
{
	uint8_t pid_l, pid_h, rev;
	int ret;

	ret = adar300x_reg_read(dev, ADAR300X_REG_PRODUCT_ID_L, &pid_l);
	if (ret)
		return ret;

	ret = adar300x_reg_read(dev, ADAR300X_REG_PRODUCT_ID_H, &pid_h);
	if (ret)
		return ret;

	ret = adar300x_reg_read(dev, ADAR300X_REG_SPI_REV, &rev);
	if (ret)
		return ret;

	pr_info("%s initialized: PRODUCT_ID 0x%02X%02X, SPI_REV 0x%02X\n",
		dev->chip_info->name, pid_h, pid_l, rev);

	return 0;
}

int basic_example_main(void)
{
	struct adar300x_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &adar300x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("ADAR300X basic example\n");

	ret = adar300x_init(&dev, &adar300x_ip);
	if (ret) {
		pr_err("adar300x_init failed: %d\n", ret);
		goto error_uart;
	}

	ret = adar300x_identify_demo(dev);

	adar300x_remove(dev);

error_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}
