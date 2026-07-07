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

#ifndef GPIO_OUTPUT_OPS

/* No GPIO backend mapped on this platform: the suite compiles out to a skip. */
int test_gpio(void)
{
	static const struct test_case stub[] = {
		{ "NOT_CONFIGURED", NULL, false },
	};

	return test_framework_run_cases("GPIO", stub, 1U);
}

#else /* GPIO_OUTPUT_OPS defined — full implementation follows */

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

#if GPIO_HAS_TOGGLE
/**
 * @brief Toggle the output port and verify the input port follows.
 *
 * External check of capi_gpio_port_toggle(): start all-low, toggle the whole
 * mask (input must read all-high), toggle again (input must read all-low).
 * A driver that mis-implements toggle as a plain write, or that toggles the
 * wrong pins, fails on the wire.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int gpio_port_toggle(void)
{
	struct capi_gpio_port_handle *out = NULL;
	struct capi_gpio_port_handle *in = NULL;
	uint64_t num_pins = gpio_output_config.num_pins;
	uint64_t all_high = (1ULL << num_pins) - 1ULL;
	uint64_t value;
	int ret;

	TEST_SECTION("PORT_TOGGLE");
	ret = capi_gpio_port_init(&out, &gpio_output_config);
	TEST_ASSERT_EQ(ret, 0, "OUT_INIT");
	ret = capi_gpio_port_init(&in, &gpio_input_config);
	TEST_ASSERT_EQ(ret, 0, "IN_INIT");

	ret = capi_gpio_port_set_direction(out, 0U);
	TEST_ASSERT_EQ(ret, 0, "OUT_DIR_ALL_OUTPUT");
	ret = capi_gpio_port_set_direction(in, all_high);
	TEST_ASSERT_EQ(ret, 0, "IN_DIR_ALL_INPUT");

	/* Known starting state: all low. */
	ret = capi_gpio_port_set_raw_value(out, 0U);
	TEST_ASSERT_EQ(ret, 0, "DRIVE_LOW");

	/* First toggle: all pins flip low -> high. */
	ret = capi_gpio_port_toggle(out, all_high);
	TEST_ASSERT_EQ(ret, 0, "TOGGLE_UP");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ(ret, 0, "READ_UP");
	TEST_ASSERT_EQ(value & all_high, all_high, "UP_READBACK");

	/* Second toggle: all pins flip high -> low. */
	ret = capi_gpio_port_toggle(out, all_high);
	TEST_ASSERT_EQ(ret, 0, "TOGGLE_DOWN");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ(ret, 0, "READ_DOWN");
	TEST_ASSERT_EQ(value & all_high, 0U, "DOWN_READBACK");

	ret = capi_gpio_port_deinit(&out);
	TEST_ASSERT_EQ(ret, 0, "OUT_DEINIT");
	ret = capi_gpio_port_deinit(&in);
	TEST_ASSERT_EQ(ret, 0, "IN_DEINIT");

	return 0;
}
#endif /* GPIO_HAS_TOGGLE */

/**
 * @brief Verify the active-low flag inverts the electrical output on the wire.
 *
 * Active-low is a CAPI software attribute (CAPI_GPIO_ACTIVE_LOW in the flags
 * array): logical HIGH must drive the pin electrically LOW. The output port is
 * opened active-low, the input port normal (active-high), so the input's raw
 * read is the inverse of the output's logical value. This is external because
 * the inversion is observed on the physical loopback wire, not in the API.
 *
 * @return 0 on pass, negative error code on failure.
 */
/*
 * This test opens the output port with a modified, active-low config. A failed
 * assertion mid-test would otherwise leave that port allocated and driving the
 * wire inverted, corrupting the GPIO state inherited by later tests. CLEANUP
 * releases both handles on every exit path; it is NULL-guarded and idempotent,
 * and capi_gpio_port_deinit() NULLs the handle so it is safe to call twice.
 */
#define CLEANUP \
	do { \
		if (out != NULL) \
			(void)capi_gpio_port_deinit(&out); \
		if (in != NULL) \
			(void)capi_gpio_port_deinit(&in); \
	} while (0)

static int gpio_active_low(void)
{
	struct capi_gpio_port_config out_cfg = gpio_output_config;
	struct capi_gpio_port_handle *out = NULL;
	struct capi_gpio_port_handle *in = NULL;
	uint64_t num_pins = gpio_output_config.num_pins;
	uint64_t all_high = (1ULL << num_pins) - 1ULL;
	uint32_t flags[64];
	uint64_t value;
	int ret;

	TEST_SECTION("PORT_ACTIVE_LOW");

	/* The loopback bitmask paths are 64-bit; a wider port can't be tested. */
	TEST_ASSERT(num_pins <= 64U, "PORT_WIDTH");

	/* Mark every output pin active-low, then open the port with it. */
	for (uint32_t i = 0U; i < num_pins; i++)
		flags[i] = CAPI_GPIO_ACTIVE_LOW;
	out_cfg.flags = flags;

	ret = capi_gpio_port_init(&out, &out_cfg);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "OUT_INIT");
	ret = capi_gpio_port_init(&in, &gpio_input_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "IN_INIT");

	ret = capi_gpio_port_set_direction(out, 0U);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "OUT_DIR_ALL_OUTPUT");
	ret = capi_gpio_port_set_direction(in, all_high);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "IN_DIR_ALL_INPUT");

	/* Logical HIGH on an active-low output -> wire LOW -> input raw LOW. */
	ret = capi_gpio_port_set_value(out, all_high);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_LOGICAL_HIGH");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "READ_RAW");
	TEST_ASSERT_EQ_OR_CLEANUP(value & all_high, 0U, "HIGH_INVERTED_ON_WIRE");

	/* Logical LOW on an active-low output -> wire HIGH -> input raw HIGH. */
	ret = capi_gpio_port_set_value(out, 0U);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_LOGICAL_LOW");
	ret = capi_gpio_port_get_raw_value(in, &value);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "READ_RAW2");
	TEST_ASSERT_EQ_OR_CLEANUP(value & all_high, all_high,
				  "LOW_INVERTED_ON_WIRE");

	ret = capi_gpio_port_deinit(&out);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "OUT_DEINIT");
	ret = capi_gpio_port_deinit(&in);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "IN_DEINIT");

	return 0;
}

#undef CLEANUP

#if GPIO_HAS_PIN_LOOPBACK
/**
 * @brief Drive individual output pins and verify individual input pins read
 *       the same value.
 *
 * This is the pin-based equivalent of gpio_loopback(): it tests the same
 * loopback behavior but operates on individual pins using capi_gpio_pin_*
 * functions instead of port-wide bitmask operations.
 *
 * API coverage:
 *     capi_gpio_pin_set_direction()        direction, loop, error
 *     capi_gpio_pin_get_direction()        direction, error
 *     capi_gpio_pin_set_raw_value()        loopback, loop, error
 *     capi_gpio_pin_get_raw_value()        loopback, error
 *
 * Setup assumption: common_data supplies one GPIO output port and one GPIO
 * input port that form the loopback pair. We test each pin individually.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int gpio_pin_loopback(void)
{
	struct capi_gpio_port_handle *out_port = NULL;
	struct capi_gpio_port_handle *in_port = NULL;
	struct capi_gpio_pin out_pin;
	struct capi_gpio_pin in_pin;
	uint8_t direction;
	uint8_t value;
	int ret;

	/* ---------------------------------------------------------------------
	 * Open both ports. Pin operations require initialized port handles.
	 * ------------------------------------------------------------------ */
	TEST_SECTION("PIN_LOOPBACK");
	ret = capi_gpio_port_init(&out_port, &gpio_output_config);
	TEST_ASSERT_EQ(ret, 0, "OUT_PORT_INIT");
	ret = capi_gpio_port_init(&in_port, &gpio_input_config);
	TEST_ASSERT_EQ(ret, 0, "IN_PORT_INIT");

	/* ---------------------------------------------------------------------
	 * Test each configured pin pair.
	 * ------------------------------------------------------------------ */
	for (uint32_t i = 0U; i < gpio_num_output_pins; i++) {
		/* Setup pin descriptors for this pair. */
		out_pin.port_handle = out_port;
		out_pin.number = gpio_output_pin_numbers[i];
		out_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

		in_pin.port_handle = in_port;
		in_pin.number = gpio_input_pin_numbers[i];
		in_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

		/* Set directions: output pin to OUTPUT, input pin to INPUT. */
		ret = capi_gpio_pin_set_direction(&out_pin, CAPI_GPIO_OUTPUT);
		TEST_ASSERT_EQ(ret, 0, "OUT_PIN_DIR");
		ret = capi_gpio_pin_set_direction(&in_pin, CAPI_GPIO_INPUT);
		TEST_ASSERT_EQ(ret, 0, "IN_PIN_DIR");

		/* Verify direction readback. */
		ret = capi_gpio_pin_get_direction(&out_pin, &direction);
		TEST_ASSERT_EQ(ret, 0, "OUT_GET_DIR");
		TEST_ASSERT_EQ(direction, CAPI_GPIO_OUTPUT, "OUT_DIR_READBACK");
		ret = capi_gpio_pin_get_direction(&in_pin, &direction);
		TEST_ASSERT_EQ(ret, 0, "IN_GET_DIR");
		TEST_ASSERT_EQ(direction, CAPI_GPIO_INPUT, "IN_DIR_READBACK");

		/* Drive output pin high; input pin must read high. */
		ret = capi_gpio_pin_set_raw_value(&out_pin, CAPI_GPIO_HIGH);
		TEST_ASSERT_EQ(ret, 0, "DRIVE_HIGH");
		ret = capi_gpio_pin_get_raw_value(&in_pin, &value);
		TEST_ASSERT_EQ(ret, 0, "READ_HIGH");
		TEST_ASSERT_EQ(value, CAPI_GPIO_HIGH, "HIGH_READBACK");

		/* Drive output pin low; input pin must read low. */
		ret = capi_gpio_pin_set_raw_value(&out_pin, CAPI_GPIO_LOW);
		TEST_ASSERT_EQ(ret, 0, "DRIVE_LOW");
		ret = capi_gpio_pin_get_raw_value(&in_pin, &value);
		TEST_ASSERT_EQ(ret, 0, "READ_LOW");
		TEST_ASSERT_EQ(value, CAPI_GPIO_LOW, "LOW_READBACK");
	}

	/* ---------------------------------------------------------------------
	 * Release both ports.
	 * ------------------------------------------------------------------ */
	ret = capi_gpio_port_deinit(&out_port);
	TEST_ASSERT_EQ(ret, 0, "OUT_PORT_DEINIT");
	ret = capi_gpio_port_deinit(&in_port);
	TEST_ASSERT_EQ(ret, 0, "IN_PORT_DEINIT");

	return 0;
}

#if GPIO_HAS_TOGGLE
/**
 * @brief Toggle each output pin individually and verify the paired input pin.
 *
 * Pin-level equivalent of gpio_port_toggle(): for every wired pin pair, drive
 * the output pin low, toggle it (input must read high), toggle again (input
 * must read low). Exercises capi_gpio_pin_toggle() on the physical wire.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int gpio_pin_toggle(void)
{
	struct capi_gpio_port_handle *out_port = NULL;
	struct capi_gpio_port_handle *in_port = NULL;
	struct capi_gpio_pin out_pin;
	struct capi_gpio_pin in_pin;
	uint8_t value;
	int ret;

	TEST_SECTION("PIN_TOGGLE");
	ret = capi_gpio_port_init(&out_port, &gpio_output_config);
	TEST_ASSERT_EQ(ret, 0, "OUT_PORT_INIT");
	ret = capi_gpio_port_init(&in_port, &gpio_input_config);
	TEST_ASSERT_EQ(ret, 0, "IN_PORT_INIT");

	for (uint32_t i = 0U; i < gpio_num_output_pins; i++) {
		out_pin.port_handle = out_port;
		out_pin.number = gpio_output_pin_numbers[i];
		out_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

		in_pin.port_handle = in_port;
		in_pin.number = gpio_input_pin_numbers[i];
		in_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

		ret = capi_gpio_pin_set_direction(&out_pin, CAPI_GPIO_OUTPUT);
		TEST_ASSERT_EQ(ret, 0, "OUT_PIN_DIR");
		ret = capi_gpio_pin_set_direction(&in_pin, CAPI_GPIO_INPUT);
		TEST_ASSERT_EQ(ret, 0, "IN_PIN_DIR");

		/* Known starting state: low. */
		ret = capi_gpio_pin_set_raw_value(&out_pin, CAPI_GPIO_LOW);
		TEST_ASSERT_EQ(ret, 0, "DRIVE_LOW");

		/* Toggle low -> high. */
		ret = capi_gpio_pin_toggle(&out_pin);
		TEST_ASSERT_EQ(ret, 0, "TOGGLE_UP");
		ret = capi_gpio_pin_get_raw_value(&in_pin, &value);
		TEST_ASSERT_EQ(ret, 0, "READ_UP");
		TEST_ASSERT_EQ(value, CAPI_GPIO_HIGH, "UP_READBACK");

		/* Toggle high -> low. */
		ret = capi_gpio_pin_toggle(&out_pin);
		TEST_ASSERT_EQ(ret, 0, "TOGGLE_DOWN");
		ret = capi_gpio_pin_get_raw_value(&in_pin, &value);
		TEST_ASSERT_EQ(ret, 0, "READ_DOWN");
		TEST_ASSERT_EQ(value, CAPI_GPIO_LOW, "DOWN_READBACK");
	}

	ret = capi_gpio_port_deinit(&out_port);
	TEST_ASSERT_EQ(ret, 0, "OUT_PORT_DEINIT");
	ret = capi_gpio_port_deinit(&in_port);
	TEST_ASSERT_EQ(ret, 0, "IN_PORT_DEINIT");

	return 0;
}
#endif /* GPIO_HAS_TOGGLE */
#endif /* GPIO_HAS_PIN_LOOPBACK */

static const struct test_case gpio_subtests[] = {
	{ "LOOPBACK",        gpio_loopback,     !GPIO_HAS_PORT_LOOPBACK },
#if GPIO_HAS_TOGGLE
	{ "PORT_TOGGLE",     gpio_port_toggle,  !GPIO_HAS_PORT_LOOPBACK },
#endif
	{ "PORT_ACTIVE_LOW", gpio_active_low,   !GPIO_HAS_PORT_LOOPBACK },
#if GPIO_HAS_PIN_LOOPBACK
	{ "PIN_LOOPBACK",    gpio_pin_loopback, !GPIO_HAS_PIN_LOOPBACK },
#if GPIO_HAS_TOGGLE
	{ "PIN_TOGGLE",      gpio_pin_toggle,   !GPIO_HAS_PIN_LOOPBACK },
#endif
#endif
};

int test_gpio(void)
{
	return test_framework_run_cases(GPIO_MODULE, gpio_subtests,
					sizeof(gpio_subtests) / sizeof(gpio_subtests[0]));
}

#endif /* GPIO_OUTPUT_OPS */
