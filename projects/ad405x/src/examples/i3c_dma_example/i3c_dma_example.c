/***************************************************************************//**
 *   @file   i3c_dma_example.c
 *   @brief  Basic example for ad405x project
 *   @author Swaroop P Kukkillaya (swaroop.kukkillaya@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "ad405x.h"

bool block_non_blocking = true; // A flag to demonstrate the non-blocking irq.
/***************************************************************************//**
 * @brief Basic callback to an irq event.
*******************************************************************************/
void complete_callback(void *ctx)
{
	/* Release the non-blocking state loop. */
	block_non_blocking = false;
}

/***************************************************************************//**
 * @brief Basic example main execution for I3C variant.
 * Loops through two states:
 * * 50 ADC readings with delay.
 * * Threshold event (non-blocking).
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	int32_t data;
	uint8_t data_reg = AD405X_REG_CONV_READ(3);
	struct ad405x_dev *dev;
	struct no_os_i3c_desc *i3c_desc;
	struct no_os_uart_desc *uart_desc;
	int i;

	if (ad405x_ip.comm_type != AD405X_I3C_COMM) {
		pr_info("This example is for I3C variant only!");
		return 0;
	}

	ret = no_os_uart_init(&uart_desc, &ad405x_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart_desc);

	/* Bring-up peripherals drivers */
	ret = ad405x_init(&dev, ad405x_ip);
	if (ret)
		goto error;

	ret = ad405x_test_scratch_pad(dev);
	if (ret)
		goto error_dev;

	/* Enter ADC Mode */
	ret = ad405x_set_operation_mode(dev, AD405X_ADC_MODE);
	if (ret)
		goto error_dev;

	struct no_os_i3c_msg i3c_dma_xfer[] = {
		{
			.tx_buff = (uint8_t *)&data_reg,
			.tx_size = 1,
			.rx_buff = NULL,
			.rx_size = 0,
		},
		{
			.tx_buff = NULL,
			.tx_size = 0,
			.rx_buff = (uint8_t *)&data,
			.rx_size = 4,
		},
	};

	i3c_desc = dev->com_desc.i3c_desc;
	i = 0;

	while (1) {
		pr_info("\n");
		ret = no_os_i3c_transfer_dma_async(i3c_desc, i3c_dma_xfer, 2, complete_callback,
						   NULL);
		if (ret)
			goto error_dev;

		while (block_non_blocking);
		block_non_blocking = true;

		data = no_os_get_unaligned_be32((uint8_t *)&data);

		pr_info("\rADC(%02d): %05ld", ++i, data);
		no_os_mdelay(50); // Cosmetic delay
	}

error_dev:
	ad405x_remove(dev);
error:
	pr_info("Error!\n");
	return ret;
}
