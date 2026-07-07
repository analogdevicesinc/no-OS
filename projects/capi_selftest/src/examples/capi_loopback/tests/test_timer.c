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
#include "capi_irq.h"
#include "capi_time.h"
#include "capi_timer.h"
#include "parameters.h"
#include "common_data.h"
#include "test_framework.h"
#include "test_timer.h"

#define TIMER_MODULE		"TIMER"
#define TIMER_IRQ_TIMEOUT_US	1000000U
#define TIMER_IRQ_STEP_US	1000U
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

/*
 * Every case here brings a timer up, and the IRQ cases also start it counting
 * and unmask the overflow interrupt. A bare early return on a failed assertion
 * would leave that timer running with its IRQ live -- and a running timer fails
 * the NEXT test's capi_timer_init() with -EIO, so one failure cascades into all
 * the timer tests that follow. CLEANUP routes every exit through stop + IRQ
 * disable + deinit; it is guarded on timer != NULL, and the stop/irq-disable are
 * harmless no-ops (return discarded) for a case that never started or armed one.
 *
 * capi_timer_deinit() takes the handle by value and cannot NULL our local, so a
 * case that deinits and then keeps asserting sets timer = NULL by hand first to
 * keep CLEANUP from touching a freed handle (see timer_basic's rate check).
 */
#define CLEANUP \
	do { \
		if (timer != NULL) { \
			(void)capi_timer_stop(timer); \
			(void)capi_timer_event_irq_disable(timer, \
					CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW); \
			(void)capi_timer_deinit(timer); \
		} \
	} while (0)

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
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	/* Free-running counter in the platform's counting direction, wrap on limit. */
	struct capi_timer_counter_config counter = {
		.direction = TIMER_DIRECTION,
		.min = 0U,
		.max = TIMER_COUNTER_MAX,
		.rollover = true,
		.extra = NULL,
	};
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_counter_config(timer, &counter), 0,
				  "COUNTER_CONFIG");

	/* Start it and prove the counter is actually advancing (alive, not rate). */
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_start(timer), 0, "START");

	/*
	 * Read both endpoints with NO framework output between them, then assert.
	 * A single UART log line is longer than a narrow counter's rollover (the
	 * 16-bit TTC wraps in ~1.2 ms), so emitting a PASS line between the two
	 * reads can straddle a wrap and make second < first on an up-counter,
	 * breaking the direction check. Back-to-back register reads are a handful
	 * of ticks apart, far inside any counter's span.
	 */
	int get_1_ret = capi_timer_counter_get(timer, &first);
	int get_2_ret = capi_timer_counter_get(timer, &second);
	TEST_ASSERT_EQ_OR_CLEANUP(get_1_ret, 0, "COUNTER_GET_1");
	TEST_ASSERT_EQ_OR_CLEANUP(get_2_ret, 0, "COUNTER_GET_2");
	TEST_ASSERT_COUNTER_DIR_OR_CLEANUP(first, second, TIMER_DIRECTION,
					   "COUNTER_MOVED");

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
	TEST_ASSERT_EQ_OR_CLEANUP(uptime_start_ret, 0, "UPTIME_START");
	TEST_ASSERT_EQ_OR_CLEANUP(count_1_ret, 0, "RATE_COUNT_1");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "UPTIME_END");
	TEST_ASSERT_EQ_OR_CLEANUP(count_2_ret, 0, "RATE_COUNT_2");

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
	 * timer that would fail every later init with -EIO. After a successful
	 * deinit, NULL the handle so CLEANUP on the rate asserts below does not
	 * touch the freed timer -- the teardown here already released it.
	 */
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_stop(timer), 0, "STOP");
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_deinit(timer), 0, "DEINIT");
	timer = NULL;

	/* Actual ticks must match the prediction within tolerance. */
	TEST_VALUE("TIMER.BASIC.elapsed_us", elapsed_us);
	TEST_VALUE("TIMER.BASIC.ticks", delta);
	TEST_ASSERT_EQ_OR_CLEANUP(expected_ret, 0, "RATE_EXPECTED");
	TEST_VALUE("TIMER.BASIC.expected", expected);
	TEST_ASSERT_NEAR_OR_CLEANUP(delta, expected,
				    expected * TIMER_RATE_TOLERANCE_PCT / 100U,
				    "RATE_NEAR");

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
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	struct capi_timer_counter_config counter = {
		.direction = TIMER_DIRECTION,
		.min = 0U,
		.max = TIMER_COUNTER_MAX,
		.rollover = true,
		.extra = NULL,
	};
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_counter_config(timer, &counter), 0,
				  "COUNTER_CONFIG");

	timer_callback_count = 0U;
	timer_callback_event = 0U;
	timer_callback_extra = 0;
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_register_event_callback(timer,
				  timer_test_callback, NULL), 0, "REGISTER_CALLBACK");
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_event_irq_enable(timer,
				  CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW), 0,
				  "EVENT_IRQ_ENABLE");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_start(timer), 0, "START");
	TEST_WAIT_UNTIL(timer_callback_count > 0U, TIMER_IRQ_TIMEOUT_US,
			TIMER_IRQ_STEP_US);
	TEST_ASSERT_GT_OR_CLEANUP(timer_callback_count, 0U, "IRQ_CB_COUNT");
	TEST_ASSERT_EQ_OR_CLEANUP(timer_callback_event,
				  CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW, "IRQ_CB_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(timer_callback_extra, 0, "IRQ_CB_EXTRA");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_stop(timer), 0, "STOP");
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_event_irq_disable(timer,
				  CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW), 0,
				  "EVENT_IRQ_DISABLE");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_deinit(timer), 0, "DEINIT");

	return 0;
}

/**
 * @brief IRQ-count check: fire a periodic interrupt and count exactly N of them.
 *
 * The plain-language contract: configure the timer to interrupt every
 * TIMER_IRQ_PERIOD_US, run for TIMER_IRQ_EXPECTED_COUNT periods, and you must
 * have counted exactly that many interrupts (e.g. 1 ms period over 200 ms => 200
 * interrupts). ASYNC_IRQ only proves one arrives; this proves the rate.
 *
 * Counting discrete events is an integer question, not a percentage one: firing
 * every period means N periods produce N events, full stop. The only honest slop
 * is one interrupt per window edge: the opening edge (timer starts a few
 * instructions before uptime is snapshotted) and the closing edge (the poll loop
 * exits a hair past the window, so a last period can complete before we read the
 * count). Two edges => a fixed +/-2 events, NOT a percent. A larger miss is a
 * real defect: too few = a missed/late re-arm, too many = a spurious or
 * double-fired interrupt (the compare/overflow aliasing this fix targets). A
 * percentage band would hide exactly those.
 *
 * The period is given in real time and converted to counter ticks at runtime via
 * the timer's own tick rate, so the test is platform/clock agnostic; each board
 * only supplies a period its counter width can hold (parameters.h). The window
 * is polled on the independent uptime clock, not slept, because on this Zynq BSP
 * the delay and uptime share one hardware timer.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int timer_irq_rate(void)
{
	struct capi_timer_handle *timer = NULL;
	uint64_t uptime_start = 0U;
	uint64_t uptime_end = 0U;
	uint32_t period_ticks = 0U;
	int ret;

	/* Run time = one period per interrupt we expect to count. */
	const uint32_t window_us = TIMER_IRQ_PERIOD_US * TIMER_IRQ_EXPECTED_COUNT;

	TEST_SECTION("IRQ_RATE");
	ret = capi_timer_init(&timer, &timer_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	/*
	 * Translate the real-time period into counter ticks for this timer's clock,
	 * then roll over every period_ticks so one overflow == one period. This is
	 * what keeps the test clock-agnostic; a board whose counter cannot hold the
	 * period fails COUNTER_CONFIG below (period too wide), not silently.
	 */
	int period_ret = capi_timer_nsec_to_ticks(timer,
			 (uint64_t)TIMER_IRQ_PERIOD_US * 1000ULL, &period_ticks);
	TEST_ASSERT_EQ_OR_CLEANUP(period_ret, 0, "PERIOD_TO_TICKS");

	struct capi_timer_counter_config counter = {
		.direction = TIMER_DIRECTION,
		.min = 0U,
		.max = period_ticks,
		.rollover = true,
		.extra = NULL,
	};
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_counter_config(timer, &counter), 0,
				  "COUNTER_CONFIG");

	timer_callback_count = 0U;
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_register_event_callback(timer,
				  timer_test_callback, NULL), 0, "REGISTER_CALLBACK");
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_event_irq_enable(timer,
				  CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW), 0,
				  "EVENT_IRQ_ENABLE");

	/* Run the interrupt for the window; count callbacks over it. */
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_start(timer), 0, "START");
	int uptime_start_ret = capi_uptime(&uptime_start);
	do {
		ret = capi_uptime(&uptime_end);
	} while (ret == 0 && (uptime_end - uptime_start) < window_us);
	unsigned int fired = timer_callback_count;

	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_stop(timer), 0, "STOP");
	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_event_irq_disable(timer,
				  CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW), 0,
				  "EVENT_IRQ_DISABLE");
	TEST_ASSERT_EQ_OR_CLEANUP(uptime_start_ret, 0, "UPTIME_START");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "UPTIME_END");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_timer_deinit(timer), 0, "DEINIT");
	timer = NULL;

	/*
	 * Exact-count assertion: N periods must produce N interrupts, tolerating
	 * only the one event on the window boundary. No percentage band.
	 */
	TEST_VALUE("TIMER.IRQ_RATE.fired", fired);
	TEST_VALUE("TIMER.IRQ_RATE.expected", TIMER_IRQ_EXPECTED_COUNT);
	TEST_ASSERT_NEAR_OR_CLEANUP(fired, TIMER_IRQ_EXPECTED_COUNT, 2U,
				    "COUNT_EXACT");

	return 0;
}

#undef CLEANUP

/*
 * Capability-gated entries (mirrors the SPI HAS_* gating): TIMER_HAS_IRQ wires
 * the counter-overflow interrupt path. A build whose timer lacks IRQ skips
 * that entry while every counter, compare and basic rate case still runs.
 */
static const struct test_case timer_subtests[] = {
	{ "BASIC",       timer_basic,       false          },
	{ "COMPARE",     timer_compare,     !TIMER_HAS_COMPARE },
	{ "ASYNC_IRQ",   timer_async_irq,   !TIMER_HAS_IRQ },
	{ "IRQ_RATE",    timer_irq_rate,    !TIMER_HAS_IRQ },
};

/**
 * @brief Exercise the CAPI timer against the mapped platform.
 *
 * Runs the timer subtest table: basic counter operation with cross-clock rate
 * verification, output-compare channel setup, and the IRQ-driven counter-
 * overflow path. Every case exercises the real timer hardware (counter,
 * compare registers, overflow interrupt); none inspect internal state or
 * argument validation. The overflow entry is gated on the platform's IRQ
 * capability flag, so a build without overflow IRQ skips that entry.
 *
 * API coverage:
 *   capi_timer_init()                  init
 *   capi_timer_deinit()                deinit
 *   capi_timer_counter_config()        basic
 *   capi_timer_counter_get()           basic
 *   capi_timer_start()/stop()          basic
 *   capi_timer_channel_*()             compare
 *   capi_timer_event_irq_*()           overflow IRQ
 *   capi_timer_register_event_callback() overflow IRQ
 *   capi_timer_nsec_to_ticks()         basic rate check
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
