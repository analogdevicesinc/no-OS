/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Hello World example for no-OS.
 *
 *           This project is a minimal starting point for learning how to build
 *           and run a bare-metal no-OS application.  It demonstrates:
 *
 *             1. Initialising a UART peripheral using the no-OS HAL
 *             2. Printing messages with the no-OS logging macros
 *             3. Reading from the UART without blocking the main loop
 *             4. Driving a GPIO to blink an LED, on the boards that have one
 *             5. Using the no-OS delay API
 *             6. Proper error handling and resource cleanup
 *
 *           See the README.rst of the project for the build instructions.
 *
 *           EXPECTED OUTPUT (115200 8N1 on the USB-UART port)
 *           --------------------------------------------------
 *             *** no-OS Hello World ***
 *             Hello World #1
 *             Hello World #2
 *             ...
 *
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <inttypes.h>
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "basic_example.h"

/* The main loop polls the UART every POLL_PERIOD_MS milliseconds. */
#define POLL_PERIOD_MS		100
/* Number of main loop iterations between two "Hello World" messages. */
#define POLLS_PER_MESSAGE	(1000 / POLL_PERIOD_MS)

/***************************************************************************//**
 * @brief Hello World application entry point.
 *
 * Initialises the UART peripheral, prints a banner, then loops forever
 * printing a counter message once per second. Any character received on the
 * UART is echoed back. On the boards that define LED_GPIO_PIN, an LED is
 * toggled with every message.
 *
 * @return 0 on success, negative error code on failure.
*******************************************************************************/
int basic_example_main(void)
{
	/*
	 * no-OS uses a descriptor pattern for peripheral handles.
	 * no_os_uart_desc holds all state for an initialised UART instance.
	 */
	struct no_os_uart_desc *uart_desc;
	struct no_os_gpio_desc *led_desc = NULL;
	uint32_t polls = 0;
	uint32_t count = 0;
	uint8_t c;
	int ret;

	/*
	 * Initialisation parameters for the UART peripheral.
	 *
	 * All platform-specific values (device ID, baud rate, ops pointer, and
	 * the extra platform init struct) are defined in parameters.h so that
	 * this file stays platform-agnostic.
	 *
	 * no_os_uart_init_param is the generic descriptor; the platform driver
	 * reads the 'extra' field for any hardware-specific settings.
	 */
	struct no_os_uart_init_param uart_ip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate  = UART_BAUDRATE,
		.size       = NO_OS_UART_CS_8,  /* 8 data bits */
		.parity     = NO_OS_UART_PAR_NO, /* no parity   */
		.stop       = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra      = UART_EXTRA,
	};

#ifdef LED_GPIO_PIN
	/*
	 * Initialisation parameters for the LED GPIO. The pattern is the same
	 * as for the UART: a generic structure, the platform ops and the
	 * platform-specific 'extra' field, all coming from parameters.h.
	 */
	struct no_os_gpio_init_param led_ip = {
		.number = LED_GPIO_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};
#endif

	/*
	 * no_os_uart_init() configures the hardware and returns a descriptor.
	 * Always check the return value: no-OS functions return 0 on success
	 * and a negative errno-style code on failure (e.g. -EINVAL, -EIO).
	 */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		/*
		 * If UART init fails we cannot print anything useful over
		 * serial, so just return the error to the caller.
		 */
		return ret;
	}

	/*
	 * Redirect printf() to the initialised UART so that pr_info() and
	 * friends produce visible output on the serial terminal.
	 */
	no_os_uart_stdio(uart_desc);

	/*
	 * pr_info() is one of the no-OS logging macros defined in
	 * no_os_print_log.h.  They map to printf() and are filtered by the
	 * NO_OS_LOG_LEVEL compile-time setting.  Available levels (lowest to
	 * highest priority): pr_debug, pr_info, pr_warning, pr_err.
	 */
	pr_info("\n*** no-OS Hello World ***\n\n");

#ifdef LED_GPIO_PIN
	/*
	 * no_os_gpio_get() claims the pin and no_os_gpio_direction_output()
	 * configures it as an output, driven low. The LED is not essential,
	 * so on failure the example reports the error and carries on.
	 */
	ret = no_os_gpio_get(&led_desc, &led_ip);
	if (!ret)
		ret = no_os_gpio_direction_output(led_desc, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("LED GPIO init failed: %d\n", ret);
		no_os_gpio_remove(led_desc);
		led_desc = NULL;
	} else {
		pr_info("Blinking the LED on GPIO %d\n", LED_GPIO_PIN);
	}
#endif

	pr_info("Press any key to have it echoed back\n\n");

	while (1) {
		/*
		 * no_os_uart_read_nonblocking() returns the number of bytes
		 * that were read, which is 0 when nothing has been received.
		 * Unlike no_os_uart_read(), it does not wait for data, so the
		 * loop keeps running while the user is not typing.
		 */
		if (no_os_uart_read_nonblocking(uart_desc, &c, 1) == 1)
			pr_info("You pressed: '%c' (0x%02x)\n", c, c);

		/* Once per second: print the message and toggle the LED. */
		if (++polls == POLLS_PER_MESSAGE) {
			polls = 0;
			count++;

			pr_info("Hello World #%"PRIu32"\n", count);

			if (led_desc)
				no_os_gpio_set_value(led_desc, count % 2);
		}

		/*
		 * no_os_mdelay() is the no-OS millisecond delay function.
		 * It uses the platform timer underneath, so the exact
		 * implementation differs per target but the API is identical.
		 */
		no_os_mdelay(POLL_PERIOD_MS);
	}

	/*
	 * Good practice: always release resources when done.
	 * In this example the loop never exits, but the pattern is shown here
	 * for reference -- real applications should call _remove() on every
	 * descriptor they own before returning.
	 */
	no_os_gpio_remove(led_desc);
	no_os_uart_remove(uart_desc);

	return 0;
}
