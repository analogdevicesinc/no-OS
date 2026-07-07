/***************************************************************************//**
 * @file common_data.c
 * @brief Common data source file for capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

#ifdef GPIO_OUTPUT_OPS
/**
 * @brief Platform-specific private data for the output/readback GPIO port.
 */
static GPIO_OUTPUT_EXTRA gpio_output_extra = GPIO_OUTPUT_EXTRA_INIT;

/**
 * @brief Platform-specific private data for the input GPIO port.
 */
static GPIO_INPUT_EXTRA gpio_input_extra = GPIO_INPUT_EXTRA_INIT;

/**
 * @brief CAPI configuration for the output/readback GPIO port.
 */
const struct capi_gpio_port_config gpio_output_config = {
	.ops = GPIO_OUTPUT_OPS,
	.identifier = GPIO_OUTPUT_IDENTIFIER,
	.num_pins = GPIO_OUTPUT_NUM_PINS,
	.flags = NULL,
	.extra = &gpio_output_extra,
};

/**
 * @brief CAPI configuration for the input GPIO port.
 */
const struct capi_gpio_port_config gpio_input_config = {
	.ops = GPIO_INPUT_OPS,
	.identifier = GPIO_INPUT_IDENTIFIER,
	.num_pins = GPIO_INPUT_NUM_PINS,
	.flags = NULL,
	.extra = &gpio_input_extra,
};
#endif /* GPIO_OUTPUT_OPS */
