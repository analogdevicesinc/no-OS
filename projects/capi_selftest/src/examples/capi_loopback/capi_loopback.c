/***************************************************************************//**
 * @file capi_loopback.c
 * @brief Minimal CAPI loopback test runner.
 *
 * The example owns the top-level test registry and the test framework lifetime.
 * Hardware mappings and framework setup data are supplied by common_data.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stdbool.h>
#include "capi_uart.h"
#include "common_data.h"
#include "test_framework.h"
#include "tests/test_gpio.h"
#include "tests/test_spi.h"
#include "tests/test_timer.h"
#include "tests/test_dma.h"
/**
 * @brief Common function signature for a top-level test group.
 */
typedef int (*test_func_t)(void);

/**
 * @brief Top-level test group registered by the example.
 */
struct test_entry {
	/** Optional menu key for a future interactive runner. IF
	* if you want to test them in a different order/repeating
	*/
	char key;
	/** Short group name printed or selected by the runner. */
	const char *name;
	/** Function that executes the group. */
	test_func_t func;
	/** If true, the group is omitted from the default all-run. */
	bool skip_all;
};

/**
 * @brief Test groups included by the loopback example.
 */
static const struct test_entry tests[] = {
	{ 0, "GPIO", test_gpio, false },
	{ 0, "SPI", test_spi, false },
	{ 0, "Timer", test_timer, false },
	{ 0, "DMA", test_dma, false },
};

#define NUM_TESTS	(sizeof(tests) / sizeof(tests[0]))

/**
 * @brief Run each test group that participates in the default all-run.
 *
 * A failing group is remembered but does not stop the remaining groups. This
 * keeps the final summary useful when multiple peripherals are tested.
 *
 * @return 0 if every executed group passed, first non-zero test error otherwise.
 */
static int run_all_tests(void)
{
	int first_error = 0;

	for (unsigned int i = 0U; i < NUM_TESTS; i++) {
		int ret;

		if (tests[i].skip_all)
			continue;

		TEST_SECTION(tests[i].name);
		ret = tests[i].func();
		if (ret != 0 && first_error == 0)
			first_error = ret;
	}

	return first_error;
}

/**
 * @brief Initialize UART/framework state, execute the registry and clean up.
 *
 * The platform ``main.c`` is the real program entry point. It performs
 * platform startup, then calls this no-OS example entry.
 *
 * @return 0 on pass, first test/runtime error otherwise.
 */
int example_main(void)
{
	struct capi_uart_handle *uart = NULL;
	struct test_framework_config framework_config;
	int first_error = 0;
	int ret;

	ret = capi_uart_init(&uart, &uart_config);
	if (ret != 0)
		return ret;

	get_test_framework_config(&framework_config, uart);
	ret = test_framework_init(&framework_config);
	if (ret != 0) {
		capi_uart_deinit(uart);
		return ret;
	}

	TEST_RUN_START();
	first_error = run_all_tests();
	TEST_RUN_END();

	test_framework_remove();
	ret = capi_uart_deinit(uart);
	if (first_error != 0)
		return first_error;

	return ret;
}
