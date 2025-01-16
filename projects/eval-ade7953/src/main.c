/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE7953 example.
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
#include "ade7953.h"
#include "common_data.h"
#include "platform.h"
#include "interrupt.h"

struct no_os_irq_ctrl_desc *ade7953_nvic_desc;

int main(void)
{
	uint8_t ret;
	/* counter for display values interval */
	uint16_t cnt;

	/* parameters initialization structure */
	struct ade7953_init_param ade7953_ip;
	/* device structure */
	struct ade7953_dev *ade7953_dev;

	/* uart descriptor */
	struct no_os_uart_desc *uart_desc;
	/* gpio descriptor */
	struct no_os_gpio_desc *gpio_desc;
	/* reset descriptor */
	struct no_os_gpio_desc *reset_desc;
	/* spi descriptor */
	struct no_os_spi_desc *spi_desc;

	/* Initialize NVIC IRQ controller in order to be able to enable
	GPIO IRQ interrupt */
	struct no_os_irq_init_param ade7953_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&ade7953_nvic_desc,
				  &ade7953_nvic_ip);
	if (ret)
		goto error;

	ret = no_os_irq_set_priority(ade7953_nvic_desc,
				     GPIO2_IRQn, 1);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_enable(ade7953_nvic_desc,
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

	ret = no_os_gpio_get_optional(&reset_desc,
				      &gpio_reset_ip);
	if (ret)
		goto remove_led;

	if (reset_desc)
		ret = no_os_gpio_direction_output(reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_reset;

	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_reset;

	/* Initialize SPI */
	ade7953_ip.spi_init = &ade7953_spi_ip;

	/* Enable ADE7953 24 bit register operation*/
	ade7953_ip.en_24_bit = ENABLE;

	/* Init the reset */
	ade7953_ip.gpio_reset = reset_desc;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE7953 SPI example \n");

	/* Init ade7953 struct */
	ade7953_dev = (struct ade7953_dev *)no_os_calloc(1, sizeof(*ade7953_dev));
	if (!ade7953_dev)
		return -ENOMEM;

	/* Initialize interrupt */
	ret = inter_init_irq(ade7953_dev);
	if (ret)
		goto free_dev;

	/* Hard reset the ADE */
	ade7953_dev->gpio_reset = ade7953_ip.gpio_reset;
	ret = ade7953_hw_reset(ade7953_dev);
	if (ret)
		goto free_dev;
	/* Wait for device to power up */
	while (!(get_irq_flag_state())) {
	}

	/* ADE7953 dev DATA_RDY init params */
	ade7953_ip.irq_ctrl = ade7953_dev->irq_ctrl;

	/* initialize the device with the values stored
	in the initialization structure */
	ret = ade7953_init(&ade7953_dev, ade7953_ip);
	if (ret)
		goto free_dev;

	/* Initialize zx interrupt */
	ret = inter_init_zx(ade7953_dev);
	if (ret)
		goto free_dev;

	/* reset the dispay counter */
	cnt = 0;

	while (1) {

		if (get_zx_flag_state()) {
			reset_zx_flag_state();
			cnt++;
			ret = interface_toggle_led(gpio_desc);
			if (ret)
				goto free_dev;
			ret = no_os_irq_enable(ade7953_dev->zx_ctrl, GPIO_ZX_PIN);
			if (ret)
				goto free_dev;
		}

		if (cnt == READ_INTERVAL) {
			/* read and print the ade7953 measured values */
			ret = read_measurements(ade7953_dev, ADE7953_I_CH2);
			if (ret)
				goto free_dev;
			cnt = 0;
		}
	}

free_dev:
	no_os_free(ade7953_dev);
remove_reset:
	no_os_gpio_remove(reset_desc);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);
remove_irq:
	no_os_irq_ctrl_remove(ade7953_nvic_desc);

error:
	pr_err("ERROR\n");
	return ret;

}
