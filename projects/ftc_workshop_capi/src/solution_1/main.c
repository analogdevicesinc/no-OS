/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "capi_gpio.h"
#include "capi_uart.h"

#include "platform.h"

int main()
{
	uint8_t button1_val;
	uint8_t button2_val;
	int ret;
	
	/* Port specific handle - statically allocated */
	struct capi_gpio_port_handle *gpio_port0 = CAPI_GPIO_HANDLE;

	/* Configuration for the GPIO port */
	struct capi_gpio_port_config gpio_port0_config = {
		.identifier = 0,
		.num_pins = 32,
		.ops = &maxim_gpio_ops,
	};

	/* GPIO line handle for button 1 */
	struct capi_gpio_pin gpio2 = {
		.port_handle = gpio_port0,
		.number = 2,
	};

	/* GPIO line handle for button 2 */
	struct capi_gpio_pin gpio3 = {
		.port_handle = gpio_port0,
		.number = 3,
	};

	/* UART instance specific handle - statically allocated */
	struct capi_uart_handle *uart = CAPI_UART_HANDLE;

	/* Configuration for the UART instance */
	struct capi_uart_config uart_config = {
		.identifier = 0,
		.clk_freq_hz = 115200,
		.ops = &maxim_uart_ops,
	};

	/* Configure the UART hardware */
	ret = capi_uart_init(&uart, &uart_config);
	if (ret)
		return ret;

	/* 
	 * Configure the libc stub layer for print functions. 
	 * From now on, printf/scanf functions will call capi_uart_transmit()/capi_uart_receive()
	 * with the "uart" handle.
	 * 
	 * UART is a point-to-point protocol, thus it doesn't follow the instance handle/device
	 * driver model.
	 */
	capi_uart_stdio(uart);

	/* Port device initialization */
	ret = capi_gpio_port_init(&gpio_port0, &gpio_port0_config);
	if (ret)
		return ret;

	while (1){
		/* 
		 * gpio_port0 is a descriptor for the whole port.
		 * Interracting with individual GPIO lines can be done in two ways:
		 * 
		 * 1. Using the port descriptor and calling its subset of the GPIO API:
		 * 	- capi_gpio_port_set_direction()
		 * 	- capi_gpio_port_set_value()
		 * 	- capi_gpio_port_set_raw_value()
		 *
		 * This set of functions allow you to configure and control all the pins
		 * in a specific port using one API call. They use bit mask parameters:
		 * 
		 * int capi_gpio_port_get_value(struct capi_gpio_port_handle *handle, uint64_t *value_bitmask);
		 * 
		 * 2. Using the capi_gpio_pin_ API, taking a struct capi_gpio_pin type as parameter,
		 * which represents a single GPIO line:
		 * 	- capi_gpio_pin_set_direction()
		 * 	- capi_gpio_pin_set_value()
		 * 	- capi_gpio_pin_set_raw_value()
		 * 
		 * The struct capi_gpio_pin type doesn't have a specific constructor, it has to be defined
		 * by the user.
		 */

		/* 
		 * TODO: read the GPIO line for button 1 (gpio2 capi_gpio_pin) into the button1_val variable.
		 * Hint: Check the capi_gpio.h header for the function to read the state of the pin.
		 */
		ret = capi_gpio_pin_get_value(&gpio2, &button1_val);
		if (ret)
			printf("Error reading the state of button 1 (%d)\n", ret);

		/* 
		 * TODO: read the GPIO line for button 2 (gpio3 capi_gpio_pin) into the button2_val variable.
		 * Hint: Check the capi_gpio.h header for the function to read the state of the pin.
		 */
		ret = capi_gpio_pin_get_value(&gpio3, &button2_val);
		if (ret)
			printf("Error reading the state of button 2 (%d)\n", ret);

		printf("Button state: %d %d\n", button1_val, button2_val);

		no_os_mdelay(1000);
	}

	return 0;
}