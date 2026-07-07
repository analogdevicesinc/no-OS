/***************************************************************************//**
 * @file test_gpio.c
 * @brief CAPI GPIO loopback tests.
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
 * This is the core loopback test: it opens the configured output/input pair,
 * checks direction and raw value paths, verifies high/low loopback, and covers
 * compact init/deinit and error cases.
 *
 * Two output patterns are tested: all pins high, then all pins low. This keeps
 * the suite small while still catching stuck-at-one and stuck-at-zero faults.
 *
 * API coverage:
 *   capi_gpio_port_init()              init, loop, error
 *   capi_gpio_port_deinit()            deinit, loop, error
 *   capi_gpio_port_set_direction()     direction, loop, error
 *   capi_gpio_port_get_direction()     direction, error
 *   capi_gpio_port_set_raw_value()     loopback, loop, error
 *   capi_gpio_port_get_raw_value()     loopback, error
 *
 * Setup assumption: common_data supplies one GPIO output and one GPIO input
 * that form the loopback pair. No board- or vendor-specific behavior is
 * assumed.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int gpio_loopback(void)
{
	struct capi_gpio_port_handle *out = NULL;
	struct capi_gpio_port_handle *in = NULL;
	uint64_t num_pins = gpio_output_config.num_pins;
	uint64_t all_high = (1ULL << num_pins) - 1ULL;
	uint64_t direction;
	uint64_t value;
	int ret;

	/* ----------------------------------------------------------------------
	 * Open both ports and configure directions.
	 * Direction bitmask: 0 = output, 1 = input (CAPI convention).
	 * -------------------------------------------------------------------- */
	TEST_SECTION("LOOPBACK");
	ret = capi_gpio_port_init(&out, &gpio_output_config);
	TEST_ASSERT_EQ(ret, 0, "OUT_INIT");
	ret = capi_gpio_port_init(&in, &gpio_input_config);
	TEST_ASSERT_EQ(ret, 0, "IN_INIT");

	ret = capi_gpio_port_set_direction(out, 0U);
	TEST_ASSERT_EQ(ret, 0, "OUT_DIR_ALL_OUTPUT");
	ret = capi_gpio_port_set_direction(in, all_high);
	TEST_ASSERT_EQ(ret, 0, "IN_DIR_ALL_INPUT");
	ret = capi_gpio_port_get_direction(out, &direction);
	TEST_ASSERT_EQ(ret, 0, "OUT_GET_DIR");
	TEST_ASSERT_EQ(direction & all_high, 0U, "OUT_DIR_READBACK");
	ret = capi_gpio_port_get_direction(in, &direction);
	TEST_ASSERT_EQ(ret, 0, "IN_GET_DIR");
	TEST_ASSERT_EQ(direction & all_high, all_high, "IN_DIR_READBACK");

	/* ----------------------------------------------------------------------
	 * Drive all pins high; the input port must read them all high.
	 * -------------------------------------------------------------------- */
	ret = capi_gpio_port_set_raw_value(out, all_high);
	TEST_ASSERT_EQ(ret, 0, "DRIVE_HIGH");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ(ret, 0, "READ_HIGH");
	TEST_ASSERT_EQ(value & all_high, all_high, "HIGH_READBACK");

	/* ----------------------------------------------------------------------
	 * Drive all pins low; the input port must read them all low.
	 * -------------------------------------------------------------------- */
	ret = capi_gpio_port_set_raw_value(out, 0U);
	TEST_ASSERT_EQ(ret, 0, "DRIVE_LOW");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ(ret, 0, "READ_LOW");
	TEST_ASSERT_EQ(value & all_high, 0U, "LOW_READBACK");

	/* ----------------------------------------------------------------------
	 * Release both ports.
	 * -------------------------------------------------------------------- */
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
	struct capi_gpio_port_handle *bad_handle = NULL;
	uint64_t value;
	int ret;

	/* ----------------------------------------------------------------------
	 * Dispatcher-level NULL checks. These are cheap and deterministic, and
	 * prove the thin CAPI layer rejects missing handles/configs before the
	 * platform driver sees them.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("ERROR_PATHS");
	TEST_ASSERT_EQ(capi_gpio_port_init(NULL, &gpio_output_config), -EINVAL,
		       "INIT_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_gpio_port_init(&bad_handle, NULL), -EINVAL,
		       "INIT_NULL_CONFIG");
	TEST_ASSERT_EQ(capi_gpio_port_deinit(NULL), -EINVAL,
		       "DEINIT_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_gpio_port_deinit(&bad_handle), -EINVAL,
		       "DEINIT_NULL_PORT");
	TEST_ASSERT_EQ(capi_gpio_port_set_direction(NULL, 0U), -EINVAL,
		       "SET_DIR_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_gpio_port_get_direction(NULL, &value), -EINVAL,
		       "GET_DIR_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_gpio_port_set_raw_value(NULL, 0U), -EINVAL,
		       "SET_RAW_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_gpio_port_get_raw_value(NULL, &value), -EINVAL,
		       "GET_RAW_NULL_HANDLE");

	/* ----------------------------------------------------------------------
	 * num_pins = 0: the dispatcher passes it through, so the driver itself
	 * must reject the zero-width port.
	 * -------------------------------------------------------------------- */
	cfg.num_pins = 0U;
	ret = capi_gpio_port_init(&gpio, &cfg);
	TEST_ASSERT_EQ(ret, -EINVAL, "ZERO_PINS");

	/* ----------------------------------------------------------------------
	 * NULL output pointer on get_direction: the dispatcher does not check
	 * this, so the driver must return -EINVAL.
	 * -------------------------------------------------------------------- */
	ret = capi_gpio_port_init(&gpio, &gpio_output_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	ret = capi_gpio_port_get_direction(gpio, NULL);
	TEST_ASSERT_EQ(ret, -EINVAL, "GET_DIR_NULL_OUT");

	/* ----------------------------------------------------------------------
	 * NULL output pointer on get_raw_value: same contract as above.
	 * -------------------------------------------------------------------- */
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

	TEST_SECTION("REINIT");
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

static const struct test_case gpio_subtests[] = {
	{ "LOOPBACK",    gpio_loopback,    false },
	{ "ERROR_PATHS", gpio_error_paths, false },
	{ "REINIT",      gpio_reinit,      false },
};

int test_gpio(void)
{
	return test_framework_run_cases(GPIO_MODULE, gpio_subtests,
					sizeof(gpio_subtests) / sizeof(gpio_subtests[0]));
}
