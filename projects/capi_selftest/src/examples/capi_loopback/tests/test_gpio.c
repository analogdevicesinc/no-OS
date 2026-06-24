/***************************************************************************//**
 * @file test_gpio.c
 * @brief CAPI GPIO loopback tests.
 *
 * Hardware assumption: the output port pins are wired to the input port pins
 * (e.g. JA2->JA1 on ZedBoard). Every test that drives the output and reads
 * the input relies on this physical connection.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stdint.h>
#include <errno.h>
#include "capi_gpio.h"
#include "common_data.h"
#include "test_framework.h"
#include "test_gpio.h"

#define GPIO_MODULE	"GPIO"

/**
 * @brief Drive the output port and verify the input port reads the same value.
 *
 * This is the core loopback test. It proves that:
 *   1. Both ports initialize and accept direction configuration.
 *   2. Writing a value on the output actually changes the physical line
 *      (confirmed by reading the input).
 *   3. The driver correctly returns 0 (all low) when we drive low.
 *
 * Two patterns are tested: all pins high, then all pins low. This catches
 * stuck-at-one and stuck-at-zero faults on any pin.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int gpio_loopback(void)
{
	struct capi_gpio_port_handle *out = NULL;
	struct capi_gpio_port_handle *in = NULL;
	uint64_t num_pins = gpio_output_config.num_pins;
	uint64_t all_high = (1ULL << num_pins) - 1ULL;
	uint64_t value;
	int ret;

	/* Open both ports and configure directions. */
	ret = capi_gpio_port_init(&out, &gpio_output_config);
	TEST_ASSERT_EQ(ret, 0, "OUT_INIT");
	ret = capi_gpio_port_init(&in, &gpio_input_config);
	TEST_ASSERT_EQ(ret, 0, "IN_INIT");

	/* Direction bitmask: 0 = output, 1 = input (CAPI convention). */
	ret = capi_gpio_port_set_direction(out, 0U);
	TEST_ASSERT_EQ(ret, 0, "OUT_DIR_ALL_OUTPUT");
	ret = capi_gpio_port_set_direction(in, all_high);
	TEST_ASSERT_EQ(ret, 0, "IN_DIR_ALL_INPUT");

	/* Drive all pins high, expect input to read all high. */
	TEST_BEGIN(GPIO_MODULE, "LOOPBACK_HIGH");
	ret = capi_gpio_port_set_raw_value(out, all_high);
	TEST_ASSERT_EQ(ret, 0, "DRIVE_HIGH");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ(ret, 0, "READ_HIGH");
	TEST_ASSERT_EQ(value & all_high, all_high, "HIGH_READBACK");

	/* Drive all pins low, expect input to read all low. */
	TEST_BEGIN(GPIO_MODULE, "LOOPBACK_LOW");
	ret = capi_gpio_port_set_raw_value(out, 0U);
	TEST_ASSERT_EQ(ret, 0, "DRIVE_LOW");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ(ret, 0, "READ_LOW");
	TEST_ASSERT_EQ(value & all_high, 0U, "LOW_READBACK");

	ret = capi_gpio_port_deinit(&out);
	TEST_ASSERT_EQ(ret, 0, "OUT_DEINIT");
	ret = capi_gpio_port_deinit(&in);
	TEST_ASSERT_EQ(ret, 0, "IN_DEINIT");

	return 0;
}

/**
 * @brief Verify that the driver rejects clearly invalid arguments.
 *
 * The CAPI dispatcher validates NULL handles and NULL ops, but it does not
 * check output pointers on get_* calls or num_pins on init — those are the
 * driver's responsibility. This test covers that boundary.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int gpio_error_paths(void)
{
	struct capi_gpio_port_config cfg = gpio_output_config;
	struct capi_gpio_port_handle *gpio = NULL;
	int ret;

	/* num_pins = 0: dispatcher passes it through, driver must reject it. */
	TEST_BEGIN(GPIO_MODULE, "ERROR_ZERO_PINS");
	cfg.num_pins = 0U;
	ret = capi_gpio_port_init(&gpio, &cfg);
	TEST_ASSERT_EQ(ret, -EINVAL, "ZERO_PINS");

	/* NULL output pointer on get_direction: dispatcher does not check this. */
	TEST_BEGIN(GPIO_MODULE, "ERROR_NULL_OUT_DIR");
	ret = capi_gpio_port_init(&gpio, &gpio_output_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	ret = capi_gpio_port_get_direction(gpio, NULL);
	TEST_ASSERT_EQ(ret, -EINVAL, "GET_DIR_NULL_OUT");

	/* NULL output pointer on get_raw_value: same, dispatcher skips it. */
	TEST_BEGIN(GPIO_MODULE, "ERROR_NULL_OUT_VAL");
	ret = capi_gpio_port_get_raw_value(gpio, NULL);
	TEST_ASSERT_EQ(ret, -EINVAL, "GET_VAL_NULL_OUT");

	ret = capi_gpio_port_deinit(&gpio);
	TEST_ASSERT_EQ(ret, 0, "DEINIT");

	return 0;
}

/**
 * @brief Repeatedly initialize, use and deinitialize the output port.
 *
 * Catches stale handle state: a driver that leaks memory or forgets to
 * zero internal fields on deinit will typically crash or misbehave
 *
 * @return 0 on pass, negative error code on failure.
 */
static int gpio_reinit(void)
{
	struct capi_gpio_port_handle *gpio;
	int failures = 0;
	int ret;

	for (uint32_t i = 0U; i < 5U; i++) {
		gpio = NULL;

		ret = capi_gpio_port_init(&gpio, &gpio_output_config);
		if (ret != 0) {
			failures++;
			continue;
		}

		/* Set a non-trivial output value so we exercise the data path. */
		(void)capi_gpio_port_set_direction(gpio, 0U);
		(void)capi_gpio_port_set_raw_value(gpio, i & 1U);

		/* Always drive low before deinit — leave pins in a safe state. */
		(void)capi_gpio_port_set_raw_value(gpio, 0U);

		if (capi_gpio_port_deinit(&gpio) != 0)
			failures++;
	}

	TEST_BEGIN(GPIO_MODULE, "REINIT");
	TEST_VALUE("iterations", 5U);
	TEST_ASSERT_EQ(failures, 0, "NO_FAILURES");

	return 0;
}

typedef int (*gpio_test_func_t)(void);

struct gpio_test_entry {
	const char *name;
	gpio_test_func_t run;
};

int test_gpio(void)
{
	static const struct gpio_test_entry tests[] = {
		{ "LOOPBACK",    gpio_loopback    },
		{ "ERROR_PATHS", gpio_error_paths },
		{ "REINIT",      gpio_reinit      },
	};

	for (unsigned int i = 0U; i < sizeof(tests) / sizeof(tests[0]); i++) {
		TEST_BEGIN(GPIO_MODULE, tests[i].name);
		tests[i].run();
	}

	return 0;
}
