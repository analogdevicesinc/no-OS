/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE7978 example.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
#include "no_os_alloc.h"
#include "no_os_init.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "ade7978.h"
#include "platform.h"
#include "common_data.h"
#include "interrupt.h"

/* nvic descriptor */
struct no_os_irq_ctrl_desc *ade7978_nvic_desc;

int main(void)
{
	int ret;
	/* print count (multiple of dready) */
	int32_t print_cnt = 0;

	/* parameters initialization structure */
	struct ade7978_init_param ade7978_ip;
	/* device structure */
	struct ade7978_dev *ade7978_dev;
	/* read values */
	struct measurements *vals;

	/* uart descriptor */
	struct no_os_uart_desc *uart_desc;
	/* reset descriptor */
	struct no_os_gpio_desc *gpio_reset_desc;
	/* gpio descriptor */
	struct no_os_gpio_desc *gpio_desc;

	ret = no_os_init();
	if (ret)
		return ret;

	/* Initialize NVIC IRQ controller in order
	to be able to enable GPIO IRQ interrupt */
	struct no_os_irq_init_param ade7978_nvic_ip = {
		.platform_ops = &max_irq_ops
	};
	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&ade7978_nvic_desc,
				  &ade7978_nvic_ip);
	if (ret)
		goto error;

	ret = no_os_irq_set_priority(ade7978_nvic_desc,
				     GPIO2_IRQn, 1);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_enable(ade7978_nvic_desc,
			       GPIO2_IRQn);
	if (ret)
		goto remove_irq;

	/* Initialize UART */
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

	ret = no_os_gpio_get_optional(&gpio_reset_desc,
				      &gpio_reset_ip);
	if (ret)
		goto remove_led;

	if (gpio_reset_desc)
		ret = no_os_gpio_direction_output(gpio_reset_desc,
						  NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_reset;

	/* Initialize SPI */
	ade7978_ip.spi_init = &ade7978_spi_ip;
	/* Initialize Reset */
	ade7978_ip.reset_desc = gpio_reset_desc;
	/* Initialize temperature read */
	/* If temperature read is disabled than the channel
	is used as the second voltage channel */
	ade7978_ip.temp_en = ENABLE;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE7978 SPI example \n\n");

	/* Init ade7978 struct */
	ade7978_dev = (struct ade7978_dev *)no_os_calloc(1, sizeof(*ade7978_dev));
	if (!ade7978_dev)
		return -ENOMEM;

	/* Init measurements struct */
	vals = (struct measurements *)no_os_calloc(1, sizeof(*vals));
	if (!vals) {
		ret = -ENOMEM;
		goto free_dev;
	}

	/* Initialize the device with the values stored
	in the initialization structure */
	ret = ade7978_init(&ade7978_dev, ade7978_ip);
	if (ret)
		goto free_dev2;
	/* setup the ade7978 device */
	ret = ade7978_setup(ade7978_dev);
	if (ret)
		goto free_dev2;

	/* Initialize data ready interrupt */
	ret = inter_init(ade7978_dev);
	if (ret)
		goto free_dev2;
	/* Clear data ready flag */
	ret = ade7978_update_bits(ade7978_dev, ADE7978_REG_STATUS0,
				  ADE7978_STATUS0_DREADY,
				  no_os_field_prep(ADE7978_STATUS0_DREADY, ENABLE));
	if (ret)
		goto free_dev2;

	while (1) {
		if (print_cnt >= 1000) {
			/* print the ade7978 rms measured values for PHASE A */
			pr_info("V1_rms: %3.2f mV\n", vals->v1_rms);

			pr_info("I_rms %3.2f mA\n", vals->i_rms);

			if (ade7978_dev->temp_en)
				pr_info("Temperature %3.2f °C\n", vals->temperature_c);
			else
				pr_info("V2_rms: %3.2f mV\n", vals->v2_rms);
			print_cnt = 0;

			pr_info("\n");
			/* toggle the LED on the MCU board */
			ret = interface_toggle_led(gpio_desc);
			if (ret)
				goto free_dev2;
		}

		if (get_drdy_flag_state() >= 1) {
			print_cnt++;
			/* read the ade7978 rms measured values for PHASE A */
			ret = rms_adc_values_read(ade7978_dev, vals, ADE7978_PHASE_A);
			if (ret)
				goto free_dev2;
			reset_drdy_low_flag_state();
			/* enable interrupt */
			ret = no_os_irq_enable(ade7978_dev->irq_ctrl, GPIO_RDY_PIN);
			if (ret)
				goto free_dev2;
			/* clear dready flag */
			ret = ade7978_update_bits(ade7978_dev, ADE7978_REG_STATUS0,
						  ADE7978_STATUS0_DREADY,
						  no_os_field_prep(ADE7978_STATUS0_DREADY, ENABLE));
			if (ret)
				goto free_dev2;
		}
	}

free_dev2:
	no_os_free(vals);
free_dev:
	no_os_free(ade7978_dev);
remove_reset:
	no_os_gpio_remove(gpio_reset_desc);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);
remove_irq:
	no_os_irq_ctrl_remove(ade7978_nvic_desc);

error:
	pr_err("ERROR\n");
	return ret;

}
