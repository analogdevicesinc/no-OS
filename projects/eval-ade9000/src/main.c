/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE9000 example.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
#include "ade9000.h"
#include "platform.h"

int main(void)
{
	uint8_t ret;

	// parameters initialization structure
	struct ade9000_init_param ade9000_ip;
	// device structure
	struct ade9000_dev *ade9000_dev;

	// uart descriptor
	struct no_os_uart_desc *uart_desc;
	// gpio descriptor
	struct no_os_gpio_desc *gpio_desc;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto error;

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
	ade9000_ip.spi_init = &ade9000_spi_ip;
	/* Enable the temperature sensor */
	ade9000_ip.temp_en = ENABLE;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE9000 SPI example \n");

	/* Init ade9000 struct */
	ade9000_dev = (struct ade9000_dev *)no_os_calloc(1, sizeof(*ade9000_dev));
	if (!ade9000_dev)
		return -ENOMEM;

	/* Initialize the device with the values stored
	in the initialization structure */
	ret = ade9000_init(&ade9000_dev, ade9000_ip);
	if (ret)
		goto free_dev;
	/* setup the ade9000 device */
	ret = ade9000_setup(ade9000_dev);
	if (ret)
		goto free_dev;

	no_os_mdelay(RESET_TIME);

	while(1) {
		// time delay between readings
		no_os_mdelay(READ_INTERVAL);
		/* read and print the ade9000 rms measured values for PHASE A */
		ret = ade9000_read_data_ph(ade9000_dev, ADE9000_PHASE_A);
		if (ret)
			goto free_dev;

		pr_info("Vrms: %d mV\n",ade9000_dev->vrms_val);
		pr_info("Irms: %d mA\n",ade9000_dev->irms_val);
		/* read the temperature value */
		ret = ade9000_read_temp(ade9000_dev);
		if (ret)
			goto free_dev;

		pr_info("Temperature: %d deg C \n",ade9000_dev->temp_deg);
		/* toggle the LED on the MCU board */
		ret = interface_toggle_led(gpio_desc);
		if (ret)
			goto free_dev;
	}

free_dev:
	no_os_free(ade9000_dev);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);

error:
	pr_err("ERROR\n");
	return ret;

}
