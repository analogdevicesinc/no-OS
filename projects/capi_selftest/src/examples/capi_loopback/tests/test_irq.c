/***************************************************************************//**
 * @file test_irq.c
 * @brief CAPI IRQ controller GPIO external-interrupt integration tests.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * These are integration tests, not unit tests: every case raises a REAL
 * interrupt by driving a GPIO output pin that is externally wired to a GPIO
 * input pin (the same JAx loopback pair used by test_gpio), and observes it
 * arriving through the CAPI IRQ contract. We test the CAPI idea -- connect a
 * callback, gate it with enable/disable and the global switch, deliver the
 * argument, re-point the handler -- not the underlying vendor BSP.
 *
 * To run this suite on a new platform, implement three hooks (declared in
 * common_data.h, defined in that platform's main.c -- not here, and not tied to
 * any vendor):
 *
 *   platform_gpio_irq_arm(&irq_line)  route the loopback input pin to an IRQ
 *                                     and return its CAPI line; -ENOTSUP if the
 *                                     board has no GPIO-IRQ path (suite skips).
 *   platform_gpio_irq_ack()           clear that pin as the source, called from
 *                                     inside the ISR.
 *   platform_gpio_irq_disarm()        mask the pin's interrupt again.
 *
 * Everything else here is pure CAPI.
 *******************************************************************************/

#include "parameters.h"
#include "test_framework.h"
#include "test_irq.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include "capi_irq.h"
#include "capi_gpio.h"
#include "common_data.h"

/*
 * The whole suite needs a root IRQ controller to bring up. Platforms whose BSP
 * maps no interrupt controller leave IRQ_CTRL_IDENTIFIER undefined; there the
 * file compiles out to a skipping stub (mirrors how test_gpio stays inert until
 * GPIO is mapped), so capi_loopback still links.
 */
#ifdef IRQ_CTRL_IDENTIFIER

#define IRQ_MODULE	"IRQ"

/*
 * Platform-supplied controller extra. Cascaded topologies (e.g. a PL INTC
 * cascaded into the PS GIC) pass their sub-controller descriptor here; a flat
 * single-controller board leaves it NULL.
 */
#ifndef IRQ_CTRL_EXTRA
#define IRQ_CTRL_EXTRA	NULL
#endif

/* Time budget to observe a fired interrupt reach the callback. */
#define IRQ_WAIT_US	100000U
#define IRQ_STEP_US	100U

/* Sentinel the callback argument carries, to prove arg delivery end-to-end. */
#define IRQ_ARG_TOKEN	0xA5A5u

/*
 * ISR <-> test shared state. volatile: the ISR writes asynchronously while the
 * test body polls. Grouped in one struct so a callback can be handed a pointer
 * to it as its CAPI argument.
 */
struct irq_probe {
	volatile uint32_t count;      /* times the callback ran */
	volatile bool pin_was_source; /* input pin was the interrupt source */
	volatile uint32_t seen_token; /* argument the callback received */
};

static struct irq_probe probe;

static void probe_reset(void)
{
	probe.count = 0U;
	probe.pin_was_source = false;
	probe.seen_token = 0U;
}

/**
 * @brief Callback under test. Acks the GPIO source and records what it saw.
 *
 * The bank line is multiplexed, so the callback asks the platform which pin
 * fired and clears it there (clearing only the controller line would let the
 * pin re-assert forever). It records the argument CAPI handed back so the test
 * can prove the connect() argument survived the round trip.
 */
static void irq_callback(void *arg)
{
	if (platform_gpio_irq_ack())
		probe.pin_was_source = true;

	if (arg != NULL)
		probe.seen_token = *(uint32_t *)arg;

	probe.count++;
}

/*
 * Small fixture shared by every case: bring the GIC up (main.c may already
 * have; take it to a known state and own it here), open the CAPI GPIO output
 * port, and arm the input pin's interrupt. Returns the CAPI IRQ line to
 * connect, or a negative code. On -ENOTSUP the platform has no GPIO-IRQ path.
 */
static struct capi_gpio_port_handle *fx_out;

static int fx_setup(uint32_t *irq_line)
{
	struct capi_irq_config cfg = {
		.irq_ctrl_id = IRQ_CTRL_IDENTIFIER,
		.extra = IRQ_CTRL_EXTRA,
	};
	int ret;

	probe_reset();
	fx_out = NULL;

	(void)capi_irq_global_disable();
	(void)capi_irq_deinit();
	ret = capi_irq_init(&cfg);
	if (ret != 0)
		return ret;

	/*
	 * Open the CAPI GPIO output port FIRST, then arm the input interrupt.
	 * Order matters: opening a CAPI GPIO port re-initializes the underlying
	 * GPIO controller, which masks all of that controller's pin interrupts.
	 * On this board the output and input pins live on the same controller,
	 * so arming before the port init would have its pin-interrupt enable
	 * wiped out. Arm last so the enable survives. Start the output low.
	 */
	ret = capi_gpio_port_init(&fx_out, &gpio_output_config);
	if (ret != 0)
		return ret;
	ret = capi_gpio_port_set_direction(fx_out, 0U);
	if (ret != 0)
		return ret;
	ret = capi_gpio_port_set_raw_value(fx_out, 0U);
	if (ret != 0)
		return ret;

	/* Arm the input pin; may report the board has no GPIO-IRQ path. */
	return platform_gpio_irq_arm(irq_line);
}

static void fx_teardown(uint32_t irq_line)
{
	(void)capi_irq_disable(irq_line);
	platform_gpio_irq_disarm();
	if (fx_out != NULL) {
		(void)capi_gpio_port_set_raw_value(fx_out, 0U);
		(void)capi_gpio_port_deinit(&fx_out);
	}
}

/*
 * Drive one low->high->low pulse on the loopback output through CAPI GPIO.
 *
 * The pin is returned to the idle low level rather than left high so that every
 * pulse costs the SAME number of input transitions no matter how many have run
 * before it: the leading set(0) is always a no-op, the rise and the fall are
 * always one change each. Leaving the pin high instead made the first pulse
 * (from the low state fx_setup leaves behind) one transition cheaper than the
 * rest, so a change-triggered input saw 2N-1 events for N pulses -- an awkward
 * count that reads exactly like duplicate delivery.
 *
 * A rising-edge input still sees precisely one interrupt per call; a
 * change-triggered one sees two. That ratio is GPIO_IRQ_EVENTS_PER_EDGE.
 */
static int fx_raise_edge(void)
{
	int ret = capi_gpio_port_set_raw_value(fx_out, 0U);

	if (ret != 0)
		return ret;
	ret = capi_gpio_port_set_raw_value(fx_out, 1ULL);
	if (ret != 0)
		return ret;
	return capi_gpio_port_set_raw_value(fx_out, 0U);
}

/*
 * Every case here brings a live interrupt up: it inits the controller, opens
 * the GPIO output port and arms the input pin's IRQ. An assertion (or a skip)
 * that returns early without undoing that would leave the line enabled and the
 * pin armed, so the NEXT test inherits a live interrupt and fails for the wrong
 * reason. CLEANUP routes every exit through fx_teardown(), which disables the
 * line, disarms the pin and closes the port; it is idempotent and safe to call
 * with irq_line still 0. The same teardown runs on the success path.
 */
#define CLEANUP	fx_teardown(irq_line)

/**
 * @brief Fire a real GPIO edge and confirm the connected callback receives it.
 *
 * The headline integration path: connect -> enable -> global enable -> drive
 * the wired output high -> the input pin's interrupt must reach our callback,
 * with the argument we registered intact and the input pin named as source.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int irq_fire_and_listen(void)
{
	static uint32_t token = IRQ_ARG_TOKEN;
	uint32_t irq_line = 0U;
	int ret;

	TEST_SECTION("FIRE_AND_LISTEN");

	ret = fx_setup(&irq_line);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_IRQ_ABSENT, "no GPIO interrupt path");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SETUP");

	ret = capi_irq_connect(irq_line, irq_callback, &token);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONNECT");
	ret = capi_irq_enable(irq_line);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "ENABLE");
	ret = capi_irq_global_enable();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "GLOBAL_ENABLE");

	/* Nothing may have fired before we drive the edge. */
	TEST_ASSERT_EQ_OR_CLEANUP(probe.count, 0U, "NO_SPURIOUS");

	ret = fx_raise_edge();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_EDGE");
	TEST_WAIT_UNTIL(probe.count > 0U, IRQ_WAIT_US, IRQ_STEP_US);

	TEST_ASSERT_GT_OR_CLEANUP(probe.count, 0U, "CALLBACK_FIRED");
	TEST_ASSERT_OR_CLEANUP(probe.pin_was_source, "PIN_WAS_SOURCE");
	TEST_ASSERT_EQ_OR_CLEANUP(probe.seen_token, IRQ_ARG_TOKEN, "ARG_DELIVERED");

	fx_teardown(irq_line);
	return 0;
}

/**
 * @brief A disabled line must stay silent; re-enabling restores delivery.
 *
 * Proves capi_irq_enable/disable actually gate the *same* connected callback,
 * rather than the connection alone being enough: drive an edge while disabled
 * (no callback), then enable and drive again (callback fires).
 *
 * @return 0 on pass, negative error code on failure.
 */
static int irq_enable_gates_delivery(void)
{
	uint32_t irq_line = 0U;
	uint32_t after_disabled;
	int ret;

	TEST_SECTION("ENABLE_GATES");

	ret = fx_setup(&irq_line);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_IRQ_ABSENT, "no GPIO interrupt path");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SETUP");

	ret = capi_irq_connect(irq_line, irq_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONNECT");
	ret = capi_irq_global_enable();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "GLOBAL_ENABLE");

	/* Line still disabled: an edge must not reach the callback. */
	ret = capi_irq_disable(irq_line);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DISABLE");
	(void)platform_gpio_irq_ack();  /* clear any latched pin status */
	probe.count = 0U;
	ret = fx_raise_edge();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_WHILE_DISABLED");
	TEST_WAIT_UNTIL(probe.count > 0U, IRQ_WAIT_US, IRQ_STEP_US);
	after_disabled = probe.count;
	TEST_ASSERT_EQ_OR_CLEANUP(after_disabled, 0U, "SILENT_WHILE_DISABLED");

	/*
	 * Re-arm the pin but leave the CAPI line disabled. Arming enables the
	 * GPIO-controller pin interrupt, but the CAPI enable is a separate gate:
	 * an edge here must still be silent. This isolates the CAPI enable from
	 * the pin arm -- if delivery came back now, "enable" below would be
	 * meaningless.
	 */
	(void)platform_gpio_irq_arm(&irq_line);
	(void)platform_gpio_irq_ack();  /* clear the status the arm edge latched */
	probe.count = 0U;
	ret = fx_raise_edge();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_ARMED_STILL_DISABLED");
	TEST_WAIT_UNTIL(probe.count > 0U, IRQ_WAIT_US, IRQ_STEP_US);
	TEST_ASSERT_EQ_OR_CLEANUP(probe.count, 0U, "SILENT_ARMED_BUT_DISABLED");

	/*
	 * Swallow the edge just latched at the peripheral (no assertion -- the
	 * ack is only housekeeping). Clearing it BEFORE enable means the fresh
	 * edge below is the only thing that can fire, so delivery there is
	 * attributable to capi_irq_enable alone.
	 */
	(void)platform_gpio_irq_ack();

	/* Now enable the CAPI line: the next edge finally gets through. */
	ret = capi_irq_enable(irq_line);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "ENABLE");
	probe.count = 0U;
	ret = fx_raise_edge();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_WHILE_ENABLED");
	TEST_WAIT_UNTIL(probe.count > 0U, IRQ_WAIT_US, IRQ_STEP_US);
	TEST_ASSERT_GT_OR_CLEANUP(probe.count, 0U, "DELIVERED_WHILE_ENABLED");

	fx_teardown(irq_line);
	return 0;
}

/**
 * @brief Re-connecting a new callback on the same line re-points delivery.
 *
 * Fire once so the first callback runs, then capi_irq_connect() a different
 * handler on the same line and fire again: only the second handler must run
 * for the second edge. Tests that connect() replaces the registration.
 *
 * @return 0 on pass, negative error code on failure.
 */
static volatile uint32_t second_count;
static void irq_second_callback(void *arg)
{
	(void)arg;
	(void)platform_gpio_irq_ack();
	second_count++;
}

static int irq_reconnect_repoints(void)
{
	uint32_t irq_line = 0U;
	uint32_t first_after;
	int ret;

	TEST_SECTION("RECONNECT");

	ret = fx_setup(&irq_line);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_IRQ_ABSENT, "no GPIO interrupt path");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SETUP");

	second_count = 0U;

	ret = capi_irq_connect(irq_line, irq_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONNECT_FIRST");
	ret = capi_irq_enable(irq_line);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "ENABLE");
	ret = capi_irq_global_enable();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "GLOBAL_ENABLE");

	ret = fx_raise_edge();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_FIRST");
	TEST_WAIT_UNTIL(probe.count > 0U, IRQ_WAIT_US, IRQ_STEP_US);
	TEST_ASSERT_GT_OR_CLEANUP(probe.count, 0U, "FIRST_FIRED");
	first_after = probe.count;

	/* Re-point the line to the second callback and re-arm the pin. */
	ret = capi_irq_connect(irq_line, irq_second_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONNECT_SECOND");
	(void)platform_gpio_irq_arm(&irq_line);
	ret = capi_irq_enable(irq_line);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RE_ENABLE");

	ret = fx_raise_edge();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_SECOND");
	TEST_WAIT_UNTIL(second_count > 0U, IRQ_WAIT_US, IRQ_STEP_US);

	/* Second handler took the second edge; first did not run again. */
	TEST_ASSERT_GT_OR_CLEANUP(second_count, 0U, "SECOND_FIRED");
	TEST_ASSERT_EQ_OR_CLEANUP(probe.count, first_after, "FIRST_NOT_REFIRED");

	fx_teardown(irq_line);
	return 0;
}

/**
 * @brief Multiple successive edges each deliver a callback.
 *
 * Drives a train of edges and requires the callback count to track them,
 * proving the line stays live across repeated real interrupts (ack path
 * clears cleanly each time rather than latching after the first).
 *
 * @return 0 on pass, negative error code on failure.
 */
static int irq_repeated_edges(void)
{
	const uint32_t edges = 5U;
	uint32_t irq_line = 0U;
	int ret;

	TEST_SECTION("REPEATED_EDGES");

	ret = fx_setup(&irq_line);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_IRQ_ABSENT, "no GPIO interrupt path");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SETUP");

	ret = capi_irq_connect(irq_line, irq_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONNECT");
	ret = capi_irq_enable(irq_line);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "ENABLE");
	ret = capi_irq_global_enable();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "GLOBAL_ENABLE");

	/*
	 * Interrupts one pulse raises on this board's input detector: 1 on a
	 * rising-edge input, 2 on a change-triggered one that also fires on the
	 * falling half. Wait for the full quota each iteration, or the loop would
	 * race ahead on the first delivery and leave the rest arriving during the
	 * next pulse.
	 */
	const uint32_t per_edge = GPIO_IRQ_EVENTS_PER_EDGE;

	for (uint32_t i = 0U; i < edges; i++) {
		uint32_t before = probe.count;

		ret = fx_raise_edge();
		TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DRIVE_EDGE");
		TEST_WAIT_UNTIL(probe.count >= before + per_edge, IRQ_WAIT_US,
				IRQ_STEP_US);
	}

	TEST_BEGIN(IRQ_MODULE, "REPEATED_EDGES");
	TEST_ASSERT_EQ_OR_CLEANUP(probe.count, edges * per_edge, "ALL_DELIVERED");

	fx_teardown(irq_line);
	return 0;
}

#undef CLEANUP

static const struct test_case irq_subtests[] = {
	{ "FIRE_AND_LISTEN", irq_fire_and_listen,      false },
	{ "ENABLE_GATES",    irq_enable_gates_delivery, false },
	{ "RECONNECT",       irq_reconnect_repoints,    false },
	{ "REPEATED_EDGES",  irq_repeated_edges,        false },
};

int test_irq(void)
{
	return test_framework_run_cases(IRQ_MODULE, irq_subtests,
					sizeof(irq_subtests) / sizeof(irq_subtests[0]));
}

#else /* IRQ_CTRL_IDENTIFIER */

/* No IRQ controller mapped on this platform: the suite compiles out. */
int test_irq(void)
{
	return 0;
}

#endif /* IRQ_CTRL_IDENTIFIER */
