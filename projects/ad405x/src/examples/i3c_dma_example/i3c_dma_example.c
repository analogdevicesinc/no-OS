/***************************************************************************//**
 *   @file   i3c_dma_example.c
 *   @brief  Basic example for ad405x project
 *   @author Swaroop P Kukkillaya (swaroop.kukkillaya@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
