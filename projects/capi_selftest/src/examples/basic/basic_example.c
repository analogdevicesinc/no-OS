/***************************************************************************//**
 * @file basic_example.c
 * @brief Basic example source file for capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include "common_data.h"
#include "capi_uart.h"
#include "test_framework.h"

/**
 * @brief Write test framework output through the configured CAPI UART.
 * @param context - CAPI UART handle.
 * @param data - Data to transmit.
 * @param length - Number of bytes to transmit.
 * @return 0 in case of success, negative error code otherwise.
 */
/*
 * The polled CAPI transmit fills the UART TX FIFO in one shot; a request
 * larger than the FIFO leaves bytes undrained and wedges the console. Send in
 * FIFO-sized chunks so every call completes cleanly.
 */
#define TEST_UART_TX_CHUNK	32U

static int test_uart_write(void *context, const uint8_t *data,
			   uint32_t length)
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
 * @brief Exercise the UART as a special case.
 *
 * The framework output travels over this same UART, so a hang here would lose
 * the report. TEST_BEGIN_UART() announces and checkpoints each step BEFORE it
 * runs, so the whole-run watchdog can attribute a hang to the exact step. We
 * never reconfigure the console UART line settings here.
 *
 * The assertion and skip macros return from the enclosing function on failure
 * or skip, so each logical step lives in its own helper.
 *
 * @param uart - Initialized CAPI UART handle carrying the test output.
 * @return 0 on pass/skip, negative on assertion failure.
 */
static int test_uart_transmit(struct capi_uart_handle *uart)
{
	static uint8_t message[] = "Hello World from CAPI!\r\n";
	int ret;

	/* Plain transmit: proves the transport the report itself relies on. */
	TEST_BEGIN_UART("PS", "TRANSMIT");
	TEST_CHECKPOINT();
	ret = capi_uart_transmit(uart, message, sizeof(message) - 1U);
	TEST_ASSERT_EQ(ret, 0, "transmit_ret");

	return 0;
}

static int test_uart_get_line_config(struct capi_uart_handle *uart)
{
	struct capi_uart_line_config line_config;
	int ret;

	/* Read back the current line config (non-destructive). */
	TEST_BEGIN_UART("PS", "GET_LINE_CONFIG");
	TEST_CHECKPOINT();
	ret = capi_uart_get_line_config(uart, &line_config);
	TEST_ASSERT_EQ(ret, 0, "get_line_config_ret");
	TEST_VALUE("baudrate", line_config.baudrate);

	return 0;
}

static int test_uart_tx_fifo_count(struct capi_uart_handle *uart)
{
	uint16_t tx_count = 0U;
	int ret;

	/* FIFO depth query is optional on some backends. */
	TEST_BEGIN_UART("PS", "TX_FIFO_COUNT");
	TEST_CHECKPOINT();
	ret = capi_uart_get_tx_fifo_count(uart, &tx_count);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT(SKIP_NOT_IMPLEMENTED, "tx fifo count unsupported");
	TEST_ASSERT_EQ(ret, 0, "tx_fifo_count_ret");

	return 0;
}

/**
 * @brief Exercise the framework facilities themselves.
 *
 * Proves, in a platform-neutral way, that pass / value / range / skip all work
 * and are counted, independent of any peripheral.
 *
 * @return 0 on pass/skip, negative on assertion failure.
 */
static int test_framework_facilities(void)
{
	TEST_BEGIN("FRAMEWORK", "PASS");
	TEST_PASS();

	TEST_BEGIN("FRAMEWORK", "ASSERT_RANGE");
	TEST_ASSERT_RANGE(115200, 9600, 921600, "baud_in_range");

	TEST_BEGIN("FRAMEWORK", "SKIP");
	TEST_SKIP_CAT(SKIP_NOT_IMPLEMENTED, "placeholder for a future test");

	return 0;
}

/**
 * @brief Main function for basic example.
 * @return 0 in case of success, negative error code otherwise.
 */
int example_main(void)
{
	struct capi_uart_handle *uart = NULL;
	struct test_framework_config framework_config;
	int remove_ret;
	int ret;

	ret = capi_uart_init(&uart, &uart_config);
	if (ret)
		return ret;
	if (uart == NULL)
		return -EINVAL;

	framework_config.run_label = platform_name;
	framework_config.write = test_uart_write;
	framework_config.write_context = uart;
	framework_config.wait_us = NULL;
	framework_config.wait_context = NULL;
	/* 0 selects TEST_FRAMEWORK_DEFAULT_TIMEOUT_MS. Watchdog hooks are left
	 * NULL until the platform wires a hardware timer; the run then has no
	 * enforced timeout but behaves identically otherwise. */
	framework_config.timeout_ms = 0U;
	framework_config.watchdog_arm = NULL;
	framework_config.watchdog_disarm = NULL;
	framework_config.watchdog_context = NULL;

	ret = test_framework_init(&framework_config);
	if (ret) {
		capi_uart_deinit(uart);
		return ret;
	}

	TEST_RUN_START();

	/* A failed assertion is already recorded in the counts; we keep going
	 * so every test runs and the final tally is complete. */
	(void)test_uart_transmit(uart);
	(void)test_uart_get_line_config(uart);
	(void)test_uart_tx_fifo_count(uart);
	(void)test_framework_facilities();

	TEST_RUN_END();
	test_framework_remove();

	remove_ret = capi_uart_deinit(uart);

	return remove_ret;
}
