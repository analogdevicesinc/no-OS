/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Hello World example for no-OS with SPI and GPIO tests.
 *
 *           This project demonstrates:
 *             1. Initialising UART, SPI, and GPIO peripherals using no-OS HAL
 *             2. Testing SPI init (validates SDT base_addr)
 *             3. Testing GPIO init and toggling (validates SDT base_addr)
 *             4. Printing messages with no-OS logging macros
 *
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <inttypes.h>
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "basic_example.h"

/***************************************************************************//**
 * @brief Hello World application entry point.
 *
 * Initialises UART, SPI, and GPIO peripherals to test SDT base_addr fixes.
 *
 * @return 0 on success, negative error code on failure.
*******************************************************************************/
int basic_example_main(void)
{
	struct no_os_uart_desc *uart_desc;
	struct no_os_spi_desc *spi_desc;
	struct no_os_gpio_desc *gpio_desc;
	int ret;
	uint32_t count = 0;

	struct no_os_uart_init_param uart_ip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	struct no_os_spi_init_param spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &xil_spi_ops,
		.extra = &hello_world_spi_extra_ip,
	};

	struct no_os_gpio_init_param gpio_ip = {
		.number = 0,
		.platform_ops = &xil_gpio_ops,
		.extra = &hello_world_gpio_extra_ip,
	};

	/* UART init */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("\n*** no-OS Hello World (SPI/GPIO test) ***\n\n");

	/* SPI init test */
	pr_info("Testing SPI init...\n");
	ret = no_os_spi_init(&spi_desc, &spi_ip);
	if (ret) {
		pr_err("SPI init FAILED: %d\n", ret);
	} else {
		pr_info("SPI init OK\n");
		no_os_spi_remove(spi_desc);
	}

	/* GPIO init test */
	pr_info("Testing GPIO init...\n");
	ret = no_os_gpio_get(&gpio_desc, &gpio_ip);
	if (ret) {
		pr_err("GPIO init FAILED: %d\n", ret);
	} else {
		pr_info("GPIO init OK\n");

		ret = no_os_gpio_direction_output(gpio_desc, 0);
		if (ret) {
			pr_err("GPIO direction set FAILED: %d\n", ret);
		} else {
			pr_info("GPIO direction set OK\n");
		}

		no_os_gpio_remove(gpio_desc);
	}

	pr_info("\n--- Peripheral tests complete ---\n\n");

	/* LED blinky - reuse gpio_desc for LED */
	struct no_os_gpio_init_param led_ip = {
		.number = LED_GPIO_PIN,
		.platform_ops = &xil_gpio_ops,
		.extra = &hello_world_gpio_extra_ip,
	};

	ret = no_os_gpio_get(&gpio_desc, &led_ip);
	if (ret) {
		pr_err("LED GPIO init FAILED: %d\n", ret);
		gpio_desc = NULL;
	} else {
		no_os_gpio_direction_output(gpio_desc, 0);
		pr_info("LED blinky on GPIO %d\n", LED_GPIO_PIN);
	}

	pr_info("Type any key to echo it back.\n");

	while (1) {
		uint8_t c;

		/* Poll the UART for a keypress (non-blocking). */
		if (no_os_uart_read_nonblocking(uart_desc, &c, 1) == 1)
			pr_info("You pressed: '%c' (0x%02x)\n", c, c);

		/* Blink the LED and print a heartbeat once per second. */
		if (++count % 10 == 0) {
			pr_info("Hello World #%"PRIu32"\n", count / 10);
			if (gpio_desc)
				no_os_gpio_set_value(gpio_desc, (count / 10) & 1);
		}

		no_os_mdelay(100);
	}

	no_os_uart_remove(uart_desc);

	return 0;
}
