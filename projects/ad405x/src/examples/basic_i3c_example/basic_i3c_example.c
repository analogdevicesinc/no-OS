/***************************************************************************//**
 *   @file   basic_i3c_example.c
 *   @brief  Basic example for ad405x project
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "ad405x.h"

bool block_non_blocking = false; // A flag to demonstrate the non-blocking irq.
/***************************************************************************//**
 * @brief Basic callback to an irq event.
*******************************************************************************/
void event_callback(struct no_os_i3c_desc* desc, uint32_t data,
		    uint32_t data_len)
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
	char loading_wheel[4] = "-\\|/";
	struct ad405x_dev *dev;
	struct no_os_i3c_desc *i3c_desc;
	struct no_os_i3c_bus_desc *i3c_bus;
	struct no_os_uart_desc *uart_desc;
	uint8_t status_flags = 0;
	uint16_t ibi_en_mask = 0;

	ret = no_os_uart_init(&uart_desc, &ad405x_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart_desc);

	uint8_t irq_events = NO_OS_I3C_IRQ_IBI;
	struct ad405x_thresh thresh = {
		.min_thresh_val = -100,
		.max_thresh_val =  100,
		.min_thresh_hyst = 0,
		.max_thresh_hyst = 0
	};

	if (ad405x_ip.comm_type != AD405X_I3C_COMM) {
		pr_info("This example is for I3C variant only!");
		return 0;
	}

	/* Bring-up peripherals drivers */
	ret = ad405x_init(&dev, ad405x_ip);
	if (ret)
		goto error;

	i3c_desc = dev->com_desc.i3c_desc;
	no_os_i3c_attach_callback(i3c_desc, &event_callback);
	i3c_bus = i3c_desc->bus;
	ret = no_os_i3c_conf_irq(i3c_bus, irq_events, 1);
	if (ret)
		goto error_dev;

	ret = ad405x_test_scratch_pad(dev);
	if (ret)
		goto error_dev;

	ret = ad405x_set_threshold(dev, &thresh);
	if (ret)
		goto error_dev;

	/* Configure IBI mask */
	ibi_en_mask |= AD405X_IBI_EN_MAX_SET(1);
	ibi_en_mask |= AD405X_IBI_EN_MIN_SET(1);
	ret = ad405x_set_ibi_mask(dev, ibi_en_mask);
	if (ret)
		goto error_dev;
	ret = ad405x_set_ibi_enable(dev, true);
	if (ret)
		goto error_dev;

	int i;
	while (1) {
		/* Enter ADC Mode */
		ret = ad405x_set_operation_mode(dev, AD405X_ADC_MODE);
		i = 0;
		pr_info("\n");
		while (i < 50) {
			ret = ad405x_get_adc(dev, &data);
			pr_info("\rADC(%02d): %05ld", ++i, data);
			no_os_mdelay(50); // Cosmetic delay
		}

		pr_info("\nWaiting threshold event .");
		/* Enter Non-Persistent Autonomous Mode */
		ret = ad405x_set_operation_mode(dev, AD405X_NONPERSISTENT_AUTO_MODE_OP);
		if (ret)
			goto error_dev;
		/* Non-blocking wait IBI threshold event */
		ret = no_os_i3c_async_irq(i3c_bus, true);
		if (ret)
			goto error_dev;
		block_non_blocking = true;
		while (block_non_blocking) {
			// Other task...
			pr_info("\b%c", loading_wheel[++i & 0x3]);
			no_os_mdelay(100); // Cosmetic delay
		};
		/* Left wait loop, continue execution */
		pr_info("\rGot threshold event!     ");
		/* Clear flags to not cause threshold event on the next run. */
		ret = ad405x_get_sts_reg(dev, &status_flags, NULL);
		if (ret)
			goto error_dev;
		ret = ad405x_clear_sts_reg(dev, &status_flags, NULL);
		if (ret)
			goto error_dev;
	}

error_dev:
	ad405x_remove(dev);
error:
	pr_info("Error!\n");
	return ret;
}
