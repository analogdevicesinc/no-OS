/***************************************************************************//**
 *   @file   hello_world.c
 *   @brief  Hello World example for no-OS.
 *
 *           This project is a minimal starting point for learning how to build
 *           and run a bare-metal no-OS application.  It demonstrates:
 *
 *             1. Initialising a UART peripheral using the no-OS HAL
 *             2. Printing messages with the no-OS logging macros
 *             3. Using the no-OS delay API
 *             4. Proper error handling and resource cleanup
 *
 *           BUILD
 *           -----
 *           Place your board's .xsa file (exported from Vivado) in the project
 *           directory, then run:
 *
 *             make
 *
 *           The build system auto-detects the target architecture from the XSA
 *           and selects the right cross-compiler automatically.
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
#include "platform_includes.h"
#include "no_os_uart.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "hello_world.h"

/***************************************************************************//**
 * @brief Hello World application entry point.
 *
 * Initialises the UART peripheral, prints a banner, then loops forever
 * printing a counter message once per second.
 *
 * @return 0 on success, negative error code on failure.
*******************************************************************************/
int hello_world_main(void)
{
	/*
	 * no-OS uses a descriptor pattern for peripheral handles.
	 * no_os_uart_desc holds all state for an initialised UART instance.
	 */
	struct no_os_uart_desc *uart_desc;
	int ret;
	uint32_t count = 0;

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

	while (1) {
		count++;

		pr_info("Hello World #%"PRIu32"\n", count);

		/*
		 * no_os_mdelay() is the no-OS millisecond delay function.
		 * It uses the platform timer underneath, so the exact
		 * implementation differs per target but the API is identical.
		 */
		no_os_mdelay(1000);
	}

	/*
	 * Good practice: always release resources when done.
	 * In this example the loop never exits, but the pattern is shown here
	 * for reference -- real applications should call _remove() on every
	 * descriptor they own before returning.
	 */
	no_os_uart_remove(uart_desc);

	return 0;
}
