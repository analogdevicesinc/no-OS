/***************************************************************************//**
 * @file test_timer.c
 * @brief CAPI timer counter/compare/IRQ tests.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "test_framework.h"
#include "test_timer.h"

#ifndef TIMER_OPS

int test_timer(void)
{
	static const struct test_case stub[] = {
		{ "NOT_CONFIGURED", NULL, false },
	};

	return test_framework_run_cases("TIMER", stub, 1U);
}

#else /* TIMER_OPS defined — full test implementation */

#include "capi_irq.h"
#include "capi_time.h"
#include "capi_timer.h"
#include "common_data.h"

#define TIMER_MODULE		"TIMER"
#define TIMER_IRQ_TIMEOUT_US	1000000U
#define TIMER_IRQ_STEP_US	1000U
#define TIMER_REINIT_COUNT	5U
#define TIMER_CHANNEL		0U

/*
 * The counter shape (direction, max, compare value), the rate-check window,
 * mask and tolerance, and the capability flags are all platform properties and
 * come from parameters.h.
 */

static volatile unsigned int timer_callback_count;
static volatile uint32_t timer_callback_event;
static volatile int timer_callback_extra;

static void timer_test_callback(uint32_t event, void *arg, int event_extra)
{
	(void)arg;

	timer_callback_event = event;
	timer_callback_extra = event_extra;
	timer_callback_count++;
}

/**
 * @brief Basic CAPI timer contract: init, configure counter, run, deinit.
 *
 * Configures the free-running counter, starts it, and proves the count value
 * advances between two reads. It then measures the tick rate against the CAPI
 * uptime clock (an independent time source): over a window of known length it
 * compares the timer tick delta to what nsec_to_ticks() predicts for the
 * elapsed uptime. A rate error in the timer under test cannot hide, because the
 * reference is a different clock.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_basic(void)
{
	struct capi_timer_handle *timer = NULL;
	uint32_t first = 0U;
	uint32_t second = 0U;
	uint64_t uptime_start = 0U;
	uint64_t uptime_end = 0U;
	uint32_t expected = 0U;
	int expected_ret;
	int ret;

	TEST_SECTION("BASIC");

	/* Bring the timer up. */
	ret = capi_timer_init(&timer, &timer_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	/* Free-running counter in the platform's counting direction, wrap on limit. */
	struct capi_timer_counter_config counter = {
		.direction = TIMER_DIRECTION,
		.min = 0U,
		.max = TIMER_COUNTER_MAX,
		.rollover = true,
		.extra = NULL,
	};
	TEST_ASSERT_EQ(capi_timer_counter_config(timer, &counter), 0,
		       "COUNTER_CONFIG");

	/* Start it and prove the counter is actually advancing (alive, not rate). */
	TEST_ASSERT_EQ(capi_timer_start(timer), 0, "START");
	TEST_ASSERT_EQ(capi_timer_counter_get(timer, &first), 0, "COUNTER_GET_1");
	TEST_ASSERT_EQ(capi_timer_counter_get(timer, &second), 0, "COUNTER_GET_2");
	TEST_ASSERT_COUNTER_DIR(first, second, TIMER_DIRECTION, "COUNTER_MOVED");

	/*
	 * Rate check: measure the timer's tick delta over a window timed by the
	 * independent CAPI uptime clock, then compare it to the tick count that
	 * elapsed time predicts. Two separate clocks cross-check each other, so a
	 * wrong timer rate cannot hide.
	 *
	 * The sampling below runs with NO framework output between the endpoints:
	 * one UART log line is hundreds of microseconds and would stretch the
	 * window past a counter rollover, so return codes are stashed and asserted
	 * only afterwards. The window is defined by polling uptime rather than
	 * calling capi_wait_us(): on some platforms (e.g. this Zynq BSP) the delay
	 * and uptime share one hardware timer, so sleeping mid-measurement would
	 * disturb the very clock being read. Polling is correct everywhere.
	 */

	/* t0: snapshot uptime and the counter. */
	int uptime_start_ret = capi_uptime(&uptime_start);
	int count_1_ret = capi_timer_counter_get(timer, &first);

	/* Busy-wait the window by re-reading uptime until it is wide enough. */
	do {
		ret = capi_uptime(&uptime_end);
	} while (ret == 0 && (uptime_end - uptime_start) < TIMER_RATE_WINDOW_US);

	/* t1: snapshot the counter at the end of the window. */
	int count_2_ret = capi_timer_counter_get(timer, &second);

	/* Now safe to emit output: verify every sample above succeeded. */
	TEST_ASSERT_EQ(uptime_start_ret, 0, "UPTIME_START");
	TEST_ASSERT_EQ(count_1_ret, 0, "RATE_COUNT_1");
	TEST_ASSERT_EQ(ret, 0, "UPTIME_END");
	TEST_ASSERT_EQ(count_2_ret, 0, "RATE_COUNT_2");

	/*
	 * Measured window length, and timer ticks in it. The magnitude of the
	 * step is direction-independent once masked to the counter width: an
	 * up-counter advances first->second, a down-counter second->first, and
	 * unsigned wrap makes (second - first) the up-delta and (first - second)
	 * the down-delta. Pick per the platform's direction so the value is
	 * always the positive number of ticks elapsed.
	 */
	uint32_t elapsed_us = (uint32_t)(uptime_end - uptime_start);
	uint32_t delta = ((TIMER_DIRECTION) == CAPI_TIMER_COUNT_UP ?
			  (second - first) : (first - second)) &
			 TIMER_RATE_COUNTER_MASK;

	/* Ticks the timer should have advanced in that measured time. */
	expected_ret = capi_timer_nsec_to_ticks(timer,
						(uint64_t)elapsed_us * 1000ULL,
						&expected);

	/*
	 * Tear down before asserting the rate: a failed rate assertion returns
	 * from here, so releasing the handle first prevents leaking a running
	 * timer that would fail every later init with -EIO.
	 */
	TEST_ASSERT_EQ(capi_timer_stop(timer), 0, "STOP");
	TEST_ASSERT_EQ(capi_timer_deinit(timer), 0, "DEINIT");

	/* Actual ticks must match the prediction within tolerance. */
	TEST_VALUE("TIMER.BASIC.elapsed_us", elapsed_us);
	TEST_VALUE("TIMER.BASIC.ticks", delta);
	TEST_ASSERT_EQ(expected_ret, 0, "RATE_EXPECTED");
	TEST_VALUE("TIMER.BASIC.expected", expected);
	TEST_ASSERT_NEAR(delta, expected,
			 expected * TIMER_RATE_TOLERANCE_PCT / 100U, "RATE_NEAR");

	return 0;
}

/**
 * @brief Tick/nanosecond conversion round-trip.
 *
 * Converts a nanosecond duration to ticks and back, proving the convenience
 * conversions agree within one tick of quantization error for a representative
 * duration. Independent of any counting; needs only the configured input clock.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_conversions(void)
{
	struct capi_timer_handle *timer = NULL;
	uint32_t ticks = 0U;
	uint32_t nsec = 0U;
	uint32_t one_tick_ns = 0U;
	int ret;

	TEST_SECTION("CONVERSIONS");
	ret = capi_timer_init(&timer, &timer_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	TEST_ASSERT_EQ(capi_timer_nsec_to_ticks(timer, 1000000U, &ticks), 0,
		       "NSEC_TO_TICKS");
	TEST_ASSERT_GT(ticks, 0U, "TICKS_NONZERO");
	TEST_ASSERT_EQ(capi_timer_ticks_to_nsec(timer, ticks, &nsec), 0,
		       "TICKS_TO_NSEC");
	TEST_VALUE("TIMER.CONVERSIONS.ticks", ticks);
	TEST_VALUE("TIMER.CONVERSIONS.nsec", nsec);

	/*
	 * The conversions must be inverses to within their unavoidable one-tick
	 * quantization: convert one tick to nanoseconds and use that as the NEAR
	 * tolerance for the 1 ms round-trip.
	 */
	TEST_ASSERT_EQ(capi_timer_ticks_to_nsec(timer, 1U, &one_tick_ns), 0,
		       "ONE_TICK_NS");
	TEST_ASSERT_NEAR(nsec, 1000000U, one_tick_ns, "ROUNDTRIP_NEAR");

	TEST_ASSERT_EQ(capi_timer_deinit(timer), 0, "DEINIT");

	return 0;
}

/**
 * @brief Output-compare channel: init, configure, set/get compare value.
 *
 * Brings up channel 0 in compare mode, programs a compare value, and reads it
 * back. Compare readback proves the value round-trips through the driver; the
 * enable/disable pair proves the channel lifecycle is wired.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_compare(void)
{
	struct capi_timer_handle *timer = NULL;
	uint32_t compare = 0U;
	int ret;

	TEST_SECTION("COMPARE");
	ret = capi_timer_init(&timer, &timer_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	TEST_ASSERT_EQ(capi_timer_channel_init(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_INIT");

	struct capi_timer_channel_config channel = {
		.mode = CAPI_TIMER_COMPARE_MODE,
		.config.compare = {
			.generate_pulse_on_match = false,
			.output_identifier = 0U,
			.polarity = CAPI_TIMER_ON_COMPARE_KEEP,
			.stop_enabled = false,
			.match_value = TIMER_COMPARE_VALUE,
			.extra = NULL,
		},
		.extra = NULL,
	};
	TEST_ASSERT_EQ(capi_timer_channel_config(timer, TIMER_CHANNEL, &channel),
		       0, "CHANNEL_CONFIG");

	TEST_ASSERT_EQ(capi_timer_channel_compare_set(timer, TIMER_CHANNEL,
			TIMER_COMPARE_VALUE), 0, "COMPARE_SET");
	TEST_ASSERT_EQ(capi_timer_channel_compare_get(timer, TIMER_CHANNEL,
			&compare), 0, "COMPARE_GET");
	TEST_ASSERT_EQ(compare, TIMER_COMPARE_VALUE, "COMPARE_MATCH");

	TEST_ASSERT_EQ(capi_timer_channel_enable(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_ENABLE");
	TEST_ASSERT_EQ(capi_timer_channel_disable(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_DISABLE");
	TEST_ASSERT_EQ(capi_timer_channel_deinit(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_DEINIT");

	TEST_ASSERT_EQ(capi_timer_deinit(timer), 0, "DEINIT");

	return 0;
}

/**
 * @brief Input-capture channel: configure capture mode and read a capture value.
 *
 * Brings up channel 0 in capture mode and reads the capture register. Only the
 * API contract is exercised (config accepted, capture_get returns a value); no
 * external edge source is assumed. Gated on TIMER_HAS_CAPTURE, so platforms
 * whose timer has no input capture skip this case rather than fail it.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_capture(void)
{
	struct capi_timer_handle *timer = NULL;
	uint32_t capture = 0U;
	int ret;

	TEST_SECTION("CAPTURE");
	ret = capi_timer_init(&timer, &timer_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	TEST_ASSERT_EQ(capi_timer_channel_init(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_INIT");

	struct capi_timer_channel_config channel = {
		.mode = CAPI_TIMER_CAPTURE_MODE,
		.config.capture = {
			.input_identifier = 0U,
			.edge = CAPI_TIMER_CAPTURE_RISING,
			.extra = NULL,
		},
		.extra = NULL,
	};
	TEST_ASSERT_EQ(capi_timer_channel_config(timer, TIMER_CHANNEL, &channel),
		       0, "CHANNEL_CONFIG");
	TEST_ASSERT_EQ(capi_timer_channel_enable(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_ENABLE");
	TEST_ASSERT_EQ(capi_timer_channel_capture_get(timer, TIMER_CHANNEL,
			&capture), 0, "CAPTURE_GET");
	TEST_ASSERT_EQ(capi_timer_channel_disable(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_DISABLE");

	TEST_ASSERT_EQ(capi_timer_channel_deinit(timer, TIMER_CHANNEL), 0,
		       "CHANNEL_DEINIT");

	TEST_ASSERT_EQ(capi_timer_deinit(timer), 0, "DEINIT");

	return 0;
}

/**
 * @brief IRQ-backed counter overflow: register callback, IRQ-driven event.
 *
 * Configures the free-running counter, registers the global event callback,
 * enables the counter-overflow interrupt, and starts the timer. The overflow
 * must be delivered through the IRQ callback within the timeout, carrying the
 * COUNTER_OVERFLOW event. On builds without IRQ this case is skipped.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_async_irq(void)
{
	struct capi_timer_handle *timer = NULL;
	int ret;

	TEST_SECTION("ASYNC_IRQ");
	ret = capi_timer_init(&timer, &timer_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	struct capi_timer_counter_config counter = {
		.direction = TIMER_DIRECTION,
		.min = 0U,
		.max = TIMER_COUNTER_MAX,
		.rollover = true,
		.extra = NULL,
	};
	TEST_ASSERT_EQ(capi_timer_counter_config(timer, &counter), 0,
		       "COUNTER_CONFIG");

	timer_callback_count = 0U;
	timer_callback_event = 0U;
	timer_callback_extra = 0;
	TEST_ASSERT_EQ(capi_timer_register_event_callback(timer,
			timer_test_callback, NULL), 0, "REGISTER_CALLBACK");
	TEST_ASSERT_EQ(capi_timer_event_irq_enable(timer,
			CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW), 0,
		       "EVENT_IRQ_ENABLE");

	TEST_ASSERT_EQ(capi_timer_start(timer), 0, "START");
	TEST_WAIT_UNTIL(timer_callback_count > 0U, TIMER_IRQ_TIMEOUT_US,
			TIMER_IRQ_STEP_US);
	TEST_ASSERT_GT(timer_callback_count, 0U, "IRQ_CB_COUNT");
	TEST_ASSERT_EQ(timer_callback_event,
		       CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW, "IRQ_CB_EVENT");
	TEST_ASSERT_EQ(timer_callback_extra, 0, "IRQ_CB_EXTRA");

	TEST_ASSERT_EQ(capi_timer_stop(timer), 0, "STOP");
	TEST_ASSERT_EQ(capi_timer_event_irq_disable(timer,
			CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW), 0,
		       "EVENT_IRQ_DISABLE");

	TEST_ASSERT_EQ(capi_timer_deinit(timer), 0, "DEINIT");

	return 0;
}

/**
 * @brief Stress-test init/deinit cycles to catch resource leaks and state corruption.
 *
 * Opens and closes the timer repeatedly. Catches leaked allocations, broken IRQ
 * reconnect handling, or other lifecycle bugs that only appear under repeated
 * init/deinit.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_reinit(void)
{
	struct capi_timer_handle *timer = NULL;
	int ret;

	TEST_SECTION("REINIT");
	for (uint32_t i = 0U; i < TIMER_REINIT_COUNT; i++) {
		ret = capi_timer_init(&timer, &timer_config);
		if (ret != 0)
			break;
		ret = capi_timer_deinit(timer);
		timer = NULL;
		if (ret != 0)
			break;
	}
	TEST_VALUE("TIMER.REINIT.iterations", TIMER_REINIT_COUNT);
	TEST_ASSERT_EQ(ret, 0, "REINIT_LOOP");

	return 0;
}

/**
 * @brief Deterministic API/driver validation: reject clearly invalid arguments.
 *
 * The CAPI dispatcher validates NULL handles and NULL ops, but driver-specific
 * validation (e.g., NULL output pointers on get_* calls, out-of-range channels)
 * is the driver's responsibility. This test covers that boundary with a matrix
 * of invalid inputs and expected -EINVAL returns.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_error_paths(void)
{
	struct capi_timer_handle *timer = NULL;
	struct capi_timer_handle *bad_handle = NULL;
	struct capi_timer_counter_config counter = {
		.direction = TIMER_DIRECTION,
		.min = 0U,
		.max = TIMER_COUNTER_MAX,
		.rollover = true,
		.extra = NULL,
	};
	uint32_t value = 0U;
	int ret;

	TEST_SECTION("ERROR_PATHS");
	TEST_ASSERT_EQ(capi_timer_init(&bad_handle, NULL), -EINVAL,
		       "INIT_NULL_CONFIG");
	TEST_ASSERT_EQ(capi_timer_deinit(NULL), -EINVAL, "DEINIT_NULL");
	TEST_ASSERT_EQ(capi_timer_start(NULL), -EINVAL, "START_NULL");
	TEST_ASSERT_EQ(capi_timer_stop(NULL), -EINVAL, "STOP_NULL");
	TEST_ASSERT_EQ(capi_timer_counter_config(NULL, &counter), -EINVAL,
		       "COUNTER_CONFIG_NULL");
	TEST_ASSERT_EQ(capi_timer_counter_get(NULL, &value), -EINVAL,
		       "COUNTER_GET_NULL");

	ret = capi_timer_init(&timer, &timer_config);
	TEST_ASSERT_EQ(ret, 0, "INIT_FOR_ERROR_PATHS");
	TEST_ASSERT_EQ(capi_timer_counter_config(timer, NULL), -EINVAL,
		       "COUNTER_CONFIG_NULL_CFG");
	TEST_ASSERT_EQ(capi_timer_counter_get(timer, NULL), -EINVAL,
		       "COUNTER_GET_NULL_PTR");
	TEST_ASSERT_EQ(capi_timer_channel_compare_get(timer, TIMER_CHANNEL, NULL),
		       -EINVAL, "COMPARE_GET_NULL_PTR");
	TEST_ASSERT_EQ(capi_timer_nsec_to_ticks(timer, 1000U, NULL), -EINVAL,
		       "NSEC_TO_TICKS_NULL_PTR");
	TEST_ASSERT_EQ(capi_timer_ticks_to_nsec(timer, 1000U, NULL), -EINVAL,
		       "TICKS_TO_NSEC_NULL_PTR");
	capi_timer_isr(NULL);
	test_framework_set_name("ISR_NULL_SAFE");
	TEST_PASS();
	TEST_ASSERT_EQ(capi_timer_deinit(timer), 0, "DEINIT_AFTER_ERRORS");
	timer = NULL;

	return 0;
}

/*
 * Capability-gated entries (mirrors the SPI HAS_* gating): TIMER_HAS_IRQ wires
 * the counter-overflow interrupt path, TIMER_HAS_CAPTURE the input-capture
 * channel mode. A build whose timer lacks either skips that entry while every
 * counter, compare and conversion case still runs.
 */
static const struct test_case timer_subtests[] = {
	{ "BASIC",       timer_basic,       false              },
	{ "CONVERSIONS", timer_conversions, false              },
	{ "COMPARE",     timer_compare,     false              },
	{ "CAPTURE",     timer_capture,     !TIMER_HAS_CAPTURE },
	{ "ASYNC_IRQ",   timer_async_irq,   !TIMER_HAS_IRQ     },
	{ "REINIT",      timer_reinit,      false              },
	{ "ERROR_PATHS", timer_error_paths, false              },
};

/**
 * @brief Exercise the CAPI timer against the mapped platform.
 *
 * Runs the timer subtest table: basic counter operation, tick/nanosecond
 * conversions, output-compare channel setup, input-capture channel setup, the
 * IRQ-driven counter-overflow path, controller re-init, and the
 * error/argument-validation paths. The capture and overflow entries are gated
 * on the platform's capability flags (see the note on the table above), so a
 * build whose timer lacks either skips that entry.
 *
 * API coverage:
 *   capi_timer_init()                  init, reinit, error
 *   capi_timer_deinit()                deinit, reinit, error
 *   capi_timer_counter_config()        basic, error
 *   capi_timer_counter_get()           basic, error
 *   capi_timer_start()/stop()          basic, error
 *   capi_timer_channel_*()             compare, capture
 *   capi_timer_event_irq_*()           overflow IRQ
 *   capi_timer_register_event_callback() overflow IRQ
 *   capi_timer_nsec_to_ticks()/ticks_to_nsec() conversions
 *
 * Setup assumption: common_data supplies one timer instance. No external wiring
 * is required; the overflow interrupt is self-generated by the counter.
 *
 * @return 0 on pass, first non-zero subtest error otherwise.
 */
int test_timer(void)
{
	return test_framework_run_cases(TIMER_MODULE, timer_subtests,
					sizeof(timer_subtests) / sizeof(timer_subtests[0]));
}

#endif /* TIMER_OPS */
