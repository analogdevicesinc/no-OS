/***************************************************************************//**
 * @file common_data.c
 * @brief Common data source file for capi_selftest project.
 * @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
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

#include <stdint.h>
#include "common_data.h"
#include "parameters.h"
#include "test_framework.h"

/**
 * @brief Write test-framework bytes through the configured CAPI UART.
 * @param context - CAPI UART handle.
 * @param data - Bytes to transmit.
 * @param length - Number of bytes to transmit.
 * @return 0 in case of success, negative error code otherwise.
 */
/*
 * The polled CAPI transmit fills the UART TX FIFO in one shot; a request
 * larger than the FIFO leaves bytes undrained and wedges the console. Send in
 * FIFO-sized chunks so every call completes cleanly.
 */
#define TEST_UART_TX_CHUNK	32U

static int test_uart_write(void *context, const uint8_t *data, uint32_t length)
{
	struct capi_uart_handle *uart = (struct capi_uart_handle *)context;

	for (uint32_t off = 0U; off < length; off += TEST_UART_TX_CHUNK) {
		uint32_t chunk = length - off;

		if (chunk > TEST_UART_TX_CHUNK)
			chunk = TEST_UART_TX_CHUNK;

		int ret = capi_uart_transmit(uart, (uint8_t *)data + off, chunk);
		if (ret != 0)
			return ret;
	}

	return 0;
}

/**
 * @brief Default console UART line configuration.
 */
static struct capi_uart_line_config uart_line_config = {
	.baudrate = UART_BAUDRATE,
	.size = CAPI_UART_DATA_BITS_8,
	.parity = CAPI_UART_PARITY_NONE,
	.stop_bits = CAPI_UART_STOP_1_BIT,
	.flow_control = CAPI_UART_FLOW_CONTROL_NONE,
	.address_mode = CAPI_UART_ADDRESS_MODE_DISABLED,
	.device_address = 0U,
	.sticky_parity = false,
	.loopback = false,
};

/**
 * @brief CAPI UART configuration used as the test report transport.
 */
const struct capi_uart_config uart_config = {
	.identifier = UART_IDENTIFIER,
	.dma_handle = NULL,
	.clk_freq_hz = 0U,
	.line_config = &uart_line_config,
	.extra = NULL,
	.ops = UART_OPS,
};

/**
 * @brief Short platform label printed by TEST_RUN_START().
 */
const char platform_name[] = PLATFORM_NAME;

void get_test_framework_config(struct test_framework_config *config,
			       struct capi_uart_handle *uart)
{
	*config = (struct test_framework_config) {
		.run_label = platform_name,
		.write = test_uart_write,
		.write_context = uart,
	};
}
