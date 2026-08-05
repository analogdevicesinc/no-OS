/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for ad405x project
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_print_log.h"
#include "ad405x.h"

/***************************************************************************//**
 * @brief Basic example main executiont.
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	int32_t data;
	struct ad405x_dev *dev;
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ad405x_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart_desc);

	ret = ad405x_init(&dev, ad405x_ip);
	if (ret)
		goto error;

	ret = ad405x_test_scratch_pad(dev);
	if (ret)
		goto error_dev;

	/* Set GP0 as DRDY */
	ret = ad405x_set_gp_mode(dev, AD405X_GP_0, AD405X_GP_MODE_DRDY);

	/* Enter ADC Mode */
	ret = ad405x_set_operation_mode(dev, AD405X_ADC_MODE);
	while (1) {
		ret = ad405x_get_adc(dev, &data);
		if (ret)
			goto error_dev;
		pr_info("ADC: %ld\n\r", data);
	}

error_dev:
	ad405x_remove(dev);
error:
	pr_info("Error!\n");
	return ret;
}
