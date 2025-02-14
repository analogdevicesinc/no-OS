/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE7758 example.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
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
#include <stdio.h>
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "ade7758.h"
#include "common_data.h"
#include "platform.h"
#include "interrupt.h"

struct no_os_irq_ctrl_desc *ade7758_nvic_desc;

int main(void)
{
	uint8_t ret;
	/* counter for display values interval */
	uint16_t cnt;
	/* data value read */
	uint32_t data;

	/* parameters initialization structure */
	struct ade7758_init_param ade7758_ip;
	/* device structure */
	struct ade7758_dev *ade7758_dev;

	/* uart descriptor */
	struct no_os_uart_desc *uart_desc;
	/* gpio descriptor */
	struct no_os_gpio_desc *gpio_desc;
	/* spi descriptor */
	struct no_os_spi_desc *spi_desc;

	/* Initialize NVIC IRQ controller in order to be able to enable
	GPIO IRQ interrupt */
	struct no_os_irq_init_param ade7758_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&ade7758_nvic_desc,
				  &ade7758_nvic_ip);
	if (ret)
		goto error;

	ret = no_os_irq_set_priority(ade7758_nvic_desc,
				     GPIO2_IRQn, 1);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_enable(ade7758_nvic_desc,
			       GPIO2_IRQn);
	if (ret)
		goto remove_irq;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto remove_irq;

	ret = no_os_gpio_get_optional(&gpio_desc,
				      &gpio_led1_ip);
	if (ret)
		goto remove_uart;

	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_led;

	/* Initialize SPI */
	ade7758_ip.spi_init = &ade7758_spi_ip;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE7758 SPI example \n");

	/* Init ade7758 struct */
	ade7758_dev = (struct ade7758_dev *)no_os_calloc(1, sizeof(*ade7758_dev));
	if (!ade7758_dev)
		return -ENOMEM;

	/* ADE7758 dev IRQ init params */
	ade7758_ip.irq_ctrl = ade7758_dev->irq_ctrl;

	/* initialize the device with the values stored
	in the initialization structure */
	ret = ade7758_init(&ade7758_dev, ade7758_ip);
	if (ret)
		goto free_dev;

	/* setup the device */
	ret = ade7758_setup(ade7758_dev);

	/* Initialize irq interrupt */
	ret = inter_init_irq(ade7758_dev);
	if (ret)
		goto free_dev;

	/* reset the dispay counter */
	cnt = 0;
	/* reset the interrupt status */
	ret = ade7758_clear_irq_status(ade7758_dev, &data);
	if (ret)
		goto free_dev;
	reset_irq_flag_state();

	/* enable the interrupt */
	ret = no_os_irq_enable(ade7758_dev->irq_ctrl, GPIO_IRQ_PIN);
	if (ret)
		goto free_dev;

	while (1) {
		if (get_irq_flag_state()) {
			reset_irq_flag_state();
			cnt++;
			/* read & clear irq status */
			ret = ade7758_clear_irq_status(ade7758_dev, &data);
			if (ret)
				goto free_dev;
			/* save the irq status */
			ade7758_dev->irq_status = data;
			ret = read_measurements(ade7758_dev);
			if (ret)
				goto free_dev;
			/* enable the interrupt */
			ret = no_os_irq_enable(ade7758_dev->irq_ctrl, GPIO_IRQ_PIN);
			if (ret)
				goto free_dev;
		}

		if (cnt == PRINT_INTERVAL) {
			/* print the ade7758 measured values */
			ret = print_measurements(ade7758_dev);
			if (ret)
				goto free_dev;
			ret = interface_toggle_led(gpio_desc);
			if (ret)
				goto free_dev;
			cnt = 0;
		}
	}

free_dev:
	no_os_free(ade7758_dev);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);
remove_irq:
	no_os_irq_ctrl_remove(ade7758_nvic_desc);

error:
	pr_err("ERROR\n");
	return ret;

}
