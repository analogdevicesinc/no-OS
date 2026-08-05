/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for eval-adg2404 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "adg2404.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

/**
 * @brief Main function for basic example.
 * @return 0 in case of success, negative error code otherwise.
 */
int example_main(void)
{
	int ret;
	int i;
	struct adg2404_dev *adg2404_dev = NULL;
	struct no_os_uart_desc *uart_desc;
	enum adg2404_channel channels[] = {
		ADG2404_S1,
		ADG2404_S2,
		ADG2404_S3,
		ADG2404_S4,
		ADG2404_CH_OFF
	};

	/* Initialize UART */
	ret = no_os_uart_init(&uart_desc, &adg2404_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	/* Initialize mux by creating the descriptors */
	ret = adg2404_init(&adg2404_dev, &adg2404_ip);
	if (ret)
		goto error_uart;

	no_os_udelay(1);

	/* Disable mux to start in a known state */
	ret = adg2404_enable(adg2404_dev, false);
	if (ret)
		goto error;

	no_os_udelay(1);

	/* Enable mux */
	ret = adg2404_enable(adg2404_dev, true);
	if (ret)
		goto error;

	no_os_udelay(1);

	/* Cycle through channels to cover truth table */
	for (i = 0; i < 5; i++) {
		ret = adg2404_select_channel(adg2404_dev, channels[i]);
		if (ret)
			goto error;
		no_os_mdelay(3000);
	}

	/* Disable mux */
	ret = adg2404_enable(adg2404_dev, false);
	if (ret)
		goto error;

	no_os_udelay(1);

	adg2404_remove(adg2404_dev);
	no_os_uart_remove(uart_desc);
	return 0;

error:
	pr_info("Error!\n");
	adg2404_remove(adg2404_dev);
error_uart:
	no_os_uart_remove(uart_desc);
	return ret;
}
