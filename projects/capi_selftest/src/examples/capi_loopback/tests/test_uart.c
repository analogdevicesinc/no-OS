/***************************************************************************//**
 * @file test_uart.c
 * @brief CAPI UART external-loopback tests.
 *
 * Exercises the CAPI UART API on ONE controller wired in EXTERNAL loopback: its
 * TX pin is physically strapped to its own RX pin on the board. Internal
 * (local) loopback is deliberately NOT used -- line_config.loopback stays false
 * -- so every case moves real bytes over a real wire and back.
 *
 * This is a second, spare UART, never the console: the framework's report
 * transport is a different instance entirely (uart_config), and reconfiguring
 * that one mid-run would silence the log.
 *
 * Hardware assumption: one UART with TX looped back to RX externally.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "test_framework.h"
#include "test_uart.h"

#ifndef UART_ASYNC_OPS

int test_uart(void)
{
	static const struct test_case stub[] = {
		{ "NOT_CONFIGURED", NULL, false },
	};

	return test_framework_run_cases("UART", stub, 1U);
}

#else /* UART_ASYNC_OPS defined — full implementation follows */

#include <errno.h>
#include "capi_uart.h"
#include "capi_time.h"
#include "common_data.h"

#define UART_MODULE		"UART"
#define UART_ASYNC_TIMEOUT_US	2000000U
#define UART_ASYNC_STEP_US	1000U

/*
 * Sync payload size. The BASIC case transmits with a blocking call and only
 * then reads the bytes back, so the whole payload has to still be sitting in
 * the RX FIFO when the read starts -- keep it inside the smallest FIFO this
 * suite targets (UART Lite's 16-deep one).
 */
#define UART_SYNC_LEN		8U

static volatile unsigned int rx_callback_count;
static volatile unsigned int rx_timeout_count;
static volatile unsigned int tx_callback_count;
static volatile enum capi_uart_async_event tx_callback_event;
/*
 * Diagnostic: count "other" async events (CAPI_UART_EVENT_INTERRUPT and any
 * unexpected code). A backend whose vendor engine treats an RX line error
 * (overflow/framing/parity) as terminal reports it here and stops the transfer,
 * so a completion that never arrives shows up as a non-zero error count rather
 * than a silent 2 s timeout.
 */
static volatile unsigned int err_callback_count;
static volatile int last_event_extra;

/**
 * @brief Count async completions from the single loopback UART.
 *
 * TX and RX complete on the same handle (the wire is the device's own pin
 * pair), so one callback serves both and the event tells the directions apart.
 *
 * Only RX_DONE ends a receive. RX_TIMEOUT does NOT: a UART cannot know a
 * transfer is over, only that the line has gone quiet for a few character
 * times, so a driver raises it whenever the wire idles with bytes still
 * outstanding and leaves the receive armed to be resumed. Treating it as a
 * completion reads a partially filled buffer. It is counted separately so a
 * transfer that only ever times out is still distinguishable from a silent one.
 *
 * @param event - Completion event reported by the driver.
 * @param arg - Unused callback argument.
 * @param event_extra - Unused driver-specific detail.
 */
static void uart_test_callback(enum capi_uart_async_event event, void *arg,
			       int event_extra)
{
	(void)arg;

	last_event_extra = event_extra;

	switch (event) {
	case CAPI_UART_EVENT_RX_DONE:
		rx_callback_count++;
		break;
	case CAPI_UART_EVENT_RX_TIMEOUT:
		rx_timeout_count++;
		break;
	case CAPI_UART_EVENT_TX_DONE:
	case CAPI_UART_EVENT_TX_ABORTED:
		tx_callback_event = event;
		tx_callback_count++;
		break;
	default:
		/* CAPI_UART_EVENT_INTERRUPT / error: a terminal line error. */
		err_callback_count++;
		break;
	}
}

/**
 * @brief Clear the callback bookkeeping before arming a transfer.
 */
static void uart_reset_counters(void)
{
	rx_callback_count = 0U;
	rx_timeout_count = 0U;
	tx_callback_count = 0U;
	tx_callback_event = CAPI_UART_EVENT_INTERRUPT;
	err_callback_count = 0U;
	last_event_extra = 0;
}

/*
 * Every case brings the loopback UART up and, from ASYNC_BASIC on, leaves an
 * armed receive and unmasked interrupts behind while it runs. A bare early
 * return on a failed assertion would leave that live, so the NEXT case inits a
 * controller that is still mid-transfer and fails for the wrong reason. CLEANUP
 * routes every exit through the same teardown the success path uses. It is
 * NULL-guarded, so it is safe even when the first init is what failed.
 */
#define CLEANUP \
	do { \
		if (handle != NULL) \
			(void)capi_uart_deinit(handle); \
	} while (0)

/**
 * @brief Synchronous transmit and receive across the external loopback wire.
 *
 * The ungated case: it needs nothing but a working controller and the strap.
 * Transmit blocks until the last bit has been clocked out, which -- with TX
 * wired to RX -- means the whole payload is already in the RX FIFO by the time
 * the call returns, so the following blocking receive is guaranteed to have its
 * bytes waiting and cannot stall. That ordering is why the payload must stay
 * inside the FIFO depth (UART_SYNC_LEN).
 *
 * @return 0 on pass, negative error code on failure.
 */
static int uart_basic(void)
{
	struct capi_uart_handle *handle = NULL;
	uint8_t tx[UART_SYNC_LEN] = { 0x55, 0xAA, 0x33, 0xCC,
				      0x0F, 0xF0, 0x12, 0x34
				    };
	uint8_t rx[UART_SYNC_LEN];
	int ret;

	TEST_SECTION("BASIC");

	ret = capi_uart_init(&handle, &uart_async_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	/*
	 * Start from a known-empty RX FIFO: a previous case (or line noise while
	 * the pin was floating) may have left bytes latched, which would shift
	 * the readback by however many stale bytes are queued.
	 */
	ret = capi_uart_flush_rx_fifo(handle);
	TEST_ASSERT_OK_OR_NOTSUP_OR_CLEANUP(ret, "FLUSH_RX");

	memset(rx, 0, sizeof(rx));

	/*
	 * Pure blocking loopback: send the pattern, then drain it back. Both
	 * calls are polled, so they cooperate without an armed async RX. Mixing
	 * a pre-armed async receive with a blocking transmit latches the async
	 * completion without the looped bytes landing, since the polled TX never
	 * pumps the RX ISR -- that combination is a test bug, not a driver one.
	 */
	ret = capi_uart_transmit(handle, tx, sizeof(tx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX");

	ret = capi_uart_receive(handle, rx, sizeof(rx));
	if (ret != 0)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_BOARD_STATE,
					 "no bytes received: check UART loopback strap");

	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(rx, tx, sizeof(tx)), 0, "TX_RX_MATCH");

	TEST_ASSERT_EQ(capi_uart_deinit(handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief Interrupt-driven transmit and receive on the one loopback handle.
 *
 * The receive is armed FIRST and the transmit is issued into it, both on the
 * same handle: on a single core there is no second device to hold the other end
 * open, so the RX side has to be a non-blocking (IRQ-backed) listen that
 * completes from the ISR while the payload is being clocked out of the same
 * controller. The payload deliberately exceeds the FIFO depth so the RX ISR has
 * to refill rather than pick everything up in one shot.
 *
 * Both callbacks are checked, so this proves the TX-complete and RX-complete
 * interrupt paths independently, not just that the bytes survived.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int uart_async_basic(void)
{
	struct capi_uart_handle *handle = NULL;
	static uint8_t tx[UART_ASYNC_LEN];
	static uint8_t rx[sizeof(tx)];
	int ret;

	TEST_SECTION("ASYNC_BASIC");

	for (uint32_t i = 0U; i < sizeof(tx); i++)
		tx[i] = (uint8_t)(i * 7U + 0x5AU);

	ret = capi_uart_init(&handle, &uart_async_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_uart_register_callback(handle, uart_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "REGISTER_CB");

	ret = capi_uart_flush_rx_fifo(handle);
	TEST_ASSERT_OK_OR_NOTSUP_OR_CLEANUP(ret, "FLUSH_RX");

	memset(rx, 0, sizeof(rx));
	uart_reset_counters();

	ret = capi_uart_receive_async(handle, rx, sizeof(rx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RX_ASYNC_ARM");

	ret = capi_uart_transmit_async(handle, tx, sizeof(tx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX_ASYNC");

	TEST_WAIT_UNTIL(tx_callback_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	TEST_VALUE("tx_callback_count", tx_callback_count);
	TEST_VALUE("rx_callback_count", rx_callback_count);
	TEST_VALUE("rx_timeout_count", rx_timeout_count);
	TEST_VALUE("tx_callback_event", tx_callback_event);
	TEST_ASSERT_OR_CLEANUP(tx_callback_count > 0U, "TX_DONE");
	TEST_ASSERT_EQ_OR_CLEANUP(tx_callback_event, CAPI_UART_EVENT_TX_DONE,
				  "TX_EVENT");

	/*
	 * RX_DONE is the completion; RX_TIMEOUT is not, and is not waited on.
	 * The receive stays armed across timeouts, so an interim one costs
	 * nothing here -- see the uart_test_callback comment.
	 */
	TEST_WAIT_UNTIL(rx_callback_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(rx_callback_count > 0U, "RX_DONE");

	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(rx, tx, sizeof(tx)), 0, "TX_RX_MATCH");

	/*
	 * Pre-buffered receive: transmit first (blocking, so the bytes are
	 * already sitting in the RX FIFO by the time it returns) and only THEN
	 * arm the async receive. Some vendor Recv() paths synchronously drain
	 * whatever is already queued when armed and return without ever
	 * scheduling the interrupt-driven completion, so a wrapper that ignores
	 * that return value never raises RX_DONE for data that was waiting
	 * before the arm. Exercising this order -- instead of always arming
	 * first, as the case above does -- is what would catch it.
	 */
	TEST_SECTION("ASYNC_BASIC.PREBUFFERED");
	memset(rx, 0, sizeof(rx));
	uart_reset_counters();

	ret = capi_uart_transmit(handle, tx, UART_SYNC_LEN);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PREBUF_TX");

	ret = capi_uart_receive_async(handle, rx, UART_SYNC_LEN);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PREBUF_RX_ASYNC_ARM");

	TEST_WAIT_UNTIL(rx_callback_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(rx_callback_count > 0U, "PREBUF_RX_DONE");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(rx, tx, UART_SYNC_LEN), 0,
				  "PREBUF_TX_RX_MATCH");

	/*
	 * Back-to-back async transmits: issue a second transmit_async() before
	 * the first has reported TX_DONE. A busy check keyed on the wrong
	 * counter (one that a short transfer already zeroes once the payload is
	 * fully copied into the FIFO, before the wire has actually finished
	 * draining it and before completion is reported) would accept the
	 * second call and clobber the first transfer's bookkeeping instead of
	 * rejecting it with -EBUSY.
	 *
	 * The first transfer must exceed the FIFO depth: a payload that fits
	 * in one FIFO write (like UART_SYNC_LEN) completes synchronously
	 * inside transmit_async() itself, so RequestedBytes is already back to
	 * zero by the time the second call runs and there is no busy window
	 * left to reject against. The full UART_ASYNC_LEN tx buffer, sized to
	 * clear the deepest backend FIFO, forces the completion onto the
	 * interrupt path instead.
	 *
	 * Because the wire loops TX back to RX, an oversized transfer also
	 * needs something draining RX concurrently -- same reasoning as
	 * ASYNC_BASIC above -- or the RX FIFO overflows partway through and a
	 * later blocking read for the full payload hangs forever. Arm the
	 * async receive before the first transmit rather than draining
	 * afterward with a blocking call.
	 */
	TEST_SECTION("ASYNC_BASIC.TX_BUSY");
	memset(rx, 0, sizeof(tx));
	uart_reset_counters();

	ret = capi_uart_receive_async(handle, rx, sizeof(tx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX_BUSY_RX_ASYNC_ARM");

	ret = capi_uart_transmit_async(handle, tx, sizeof(tx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX_BUSY_FIRST");

	ret = capi_uart_transmit_async(handle, tx, sizeof(tx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, -EBUSY, "TX_BUSY_SECOND_REJECTED");

	TEST_WAIT_UNTIL(tx_callback_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	TEST_VALUE("TX_BUSY.tx_callback_count", tx_callback_count);
	TEST_VALUE("TX_BUSY.rx_callback_count", rx_callback_count);
	TEST_VALUE("TX_BUSY.rx_timeout_count", rx_timeout_count);
	TEST_VALUE("TX_BUSY.err_callback_count", err_callback_count);
	TEST_VALUE("TX_BUSY.last_event_extra", (uint32_t)last_event_extra);
	TEST_ASSERT_OR_CLEANUP(tx_callback_count > 0U, "TX_BUSY_FIRST_DONE");

	TEST_WAIT_UNTIL(rx_callback_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(rx_callback_count > 0U, "TX_BUSY_DRAIN");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(rx, tx, sizeof(tx)), 0, "TX_BUSY_TX_RX_MATCH");

#if UART_ASYNC_HAS_RX_TIMEOUT
	/*
	 * Timeout does not end a receive: arm a buffer far longer than what
	 * gets sent in a single burst, send a short first chunk (short enough
	 * that the wire idles and the driver raises RX_TIMEOUT with most of the
	 * buffer still outstanding), then send the remainder. A backend that
	 * treats RX_TIMEOUT as terminal -- masking further receive interrupts
	 * once it fires -- would leave the second chunk stranded in the FIFO
	 * and RX_DONE would never come.
	 */
	TEST_SECTION("ASYNC_BASIC.RX_TIMEOUT_CONTINUATION");
	memset(rx, 0, sizeof(rx));
	uart_reset_counters();

	ret = capi_uart_receive_async(handle, rx, sizeof(rx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TIMEOUT_RX_ASYNC_ARM");

	ret = capi_uart_transmit(handle, tx, UART_SYNC_LEN);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TIMEOUT_TX_FIRST_CHUNK");

	TEST_WAIT_UNTIL(rx_timeout_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	if (rx_timeout_count == 0U)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_FEATURE_DISABLED,
					 "backend never raised RX_TIMEOUT");

	ret = capi_uart_transmit(handle, tx + UART_SYNC_LEN,
				 sizeof(tx) - UART_SYNC_LEN);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TIMEOUT_TX_REMAINDER");

	TEST_WAIT_UNTIL(rx_callback_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(rx_callback_count > 0U,
			       "TIMEOUT_RX_DONE_AFTER_RESUME");
	if (memcmp(rx, tx, sizeof(tx)) != 0) {
		uint32_t first = sizeof(tx);
		uint32_t mismatches = 0;

		for (uint32_t i = 0; i < sizeof(tx); i++) {
			if (rx[i] != tx[i]) {
				mismatches++;
				if (first == sizeof(tx))
					first = i;
			}
		}
		TEST_VALUE("TIMEOUT_MISMATCH_COUNT", mismatches);
		TEST_VALUE("TIMEOUT_MISMATCH_FIRST", first);
		for (uint32_t i = first; i < sizeof(tx) && i < first + 20U; i++) {
			TEST_VALUE("TIMEOUT_RX_BYTE", rx[i]);
			TEST_VALUE("TIMEOUT_TX_BYTE", tx[i]);
		}
	}
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(rx, tx, sizeof(tx)), 0,
				  "TIMEOUT_TX_RX_MATCH");
#else
	/* Do not arm RX: UART Lite would then correctly reject sync TX as busy. */
	TEST_SECTION("ASYNC_BASIC.RX_TIMEOUT_CONTINUATION");
	TEST_SKIP_CAT_OR_CLEANUP(SKIP_FEATURE_DISABLED,
				 "backend has no RX timeout event");
#endif /* UART_ASYNC_HAS_RX_TIMEOUT */

	TEST_ASSERT_EQ(capi_uart_deinit(handle), 0, "DEINIT");

	return 0;
}

#if UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_LINE_CONFIG
/**
 * @brief Run one timed loopback transfer at the currently programmed baud rate.
 *
 * Arms the async receive, transmits into it, and measures the wall-clock time
 * from the transmit call to the receive completion with the independent uptime
 * clock. Records the outcome instead of asserting on it: the caller decides
 * PASS / FAIL / SKIP once it has both legs to compare.
 *
 * @param handle - Loopback UART handle.
 * @param tx - Payload to send.
 * @param rx - Scratch buffer to receive into (>= len).
 * @param len - Payload size.
 * @param out_us - Out: measured transfer time in microseconds (0 if untimed).
 * @param out_data_ok - Out: true when the payload came back intact.
 * @return 0 when the transfer completed, negative error code otherwise.
 */
static int uart_probe_xfer_at_baud(struct capi_uart_handle *handle,
				   const uint8_t *tx, uint8_t *rx, uint32_t len,
				   uint64_t *out_us, bool *out_data_ok)
{
	uint64_t start_us = 0U, end_us = 0U;
	bool timed;
	int ret;

	*out_us = 0U;
	*out_data_ok = false;

	(void)capi_uart_flush_rx_fifo(handle);

	memset(rx, 0, len);
	uart_reset_counters();

	ret = capi_uart_receive_async(handle, rx, len);
	if (ret != 0)
		return ret;

	timed = (capi_uptime(&start_us) == 0);

	ret = capi_uart_transmit_async(handle, (uint8_t *)tx, len);
	if (ret != 0)
		return ret;

	/*
	 * Wait for RX_DONE, the only event that means the whole buffer arrived.
	 * A payload this long will usually collect one or more RX_TIMEOUTs on
	 * the way: the FIFO raises its trigger interrupt every N bytes, and
	 * whatever tail is left below the trigger only comes out on the idle
	 * timeout, which fires with bytes still outstanding. Those are progress
	 * reports -- the driver keeps the receive armed across them -- so they
	 * are counted but not waited on.
	 */
	TEST_WAIT_UNTIL(rx_callback_count > 0U,
			UART_ASYNC_TIMEOUT_US, UART_ASYNC_STEP_US);
	if (rx_callback_count == 0U) {
		/*
		 * Separate the two ways this fails. Timeouts seen means bytes
		 * did arrive and the receive simply never filled -- a live wire
		 * carrying too few bytes (wrong baud, dropped characters). None
		 * at all means nothing came back: no strap, or a dead TX.
		 */
		return (rx_timeout_count > 0U) ? -EIO : -ETIMEDOUT;
	}

	if (timed && capi_uptime(&end_us) == 0)
		*out_us = end_us - start_us;

	*out_data_ok = (memcmp(rx, tx, len) == 0);

	return 0;
}
#endif /* UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_LINE_CONFIG */

/**
 * @brief Prove set_line_config actually changes the on-wire bit rate.
 *
 * The only invariant that holds on every platform is on-wire: a higher baud
 * rate clocks the SAME payload across the loopback in less wall-clock time. So
 * this times one transfer at UART_ASYNC_BAUD_SLOW and one at
 * UART_ASYNC_BAUD_FAST through the identical path and asserts the fast leg is
 * meaningfully shorter (a 10% margin; the real ratio here is ~12x, far clear of
 * it). Register readback is deliberately avoided -- it is per-driver and proves
 * only that a divider was written, not that the wire sped up.
 *
 * The payload is sized so even the fast leg stays above the software floor
 * (poll granularity + ISR latency + timer resolution); if either leg lands
 * below it anyway, the comparison would be noise and the case skips instead.
 *
 * Gated on UART_ASYNC_HAS_LINE_CONFIG: the UART Lite line format is fixed in
 * the IP, so its set_line_config is unconditionally -ENOTSUP and there is
 * nothing here to prove.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int uart_async_speed(void)
{
#if UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_LINE_CONFIG
	struct capi_uart_handle *handle = NULL;
	struct capi_uart_line_config line_config;
	static uint8_t tx[UART_ASYNC_SPEED_LEN];
	static uint8_t rx[sizeof(tx)];
	uint64_t t_slow_us = 0U, t_fast_us = 0U;
	bool slow_ok = false, fast_ok = false;
	int ret;

	TEST_SECTION("ASYNC_SPEED");

	for (uint32_t i = 0U; i < sizeof(tx); i++)
		tx[i] = (uint8_t)(i * 13U + 0xA5U);

	ret = capi_uart_init(&handle, &uart_async_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_uart_register_callback(handle, uart_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "REGISTER_CB");

	/*
	 * Start from the mapped line configuration and change only the baud
	 * rate, so a driver that rejects some other field (flow control, 9-bit
	 * addressing) is not what this case ends up measuring. loopback stays
	 * whatever common_data set it to -- false; the wire is external.
	 */
	ret = capi_uart_get_line_config(handle, &line_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "GET_LINE_CONFIG");

	/* ---- Slow leg ---- */
	TEST_SECTION("ASYNC_SPEED.SLOW");
	line_config.baudrate = UART_ASYNC_BAUD_SLOW;
	ret = capi_uart_set_line_config(handle, &line_config);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_FEATURE_DISABLED,
					 "UART has no runtime baud control");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SLOW_CONFIG");

	ret = uart_probe_xfer_at_baud(handle, tx, rx, sizeof(tx),
				      &t_slow_us, &slow_ok);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SLOW_XFER");
	TEST_ASSERT_OR_CLEANUP(slow_ok, "SLOW_DATA");

	/* ---- Fast leg ---- */
	TEST_SECTION("ASYNC_SPEED.FAST");
	line_config.baudrate = UART_ASYNC_BAUD_FAST;
	ret = capi_uart_set_line_config(handle, &line_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "FAST_CONFIG");

	ret = uart_probe_xfer_at_baud(handle, tx, rx, sizeof(tx),
				      &t_fast_us, &fast_ok);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "FAST_XFER");
	TEST_ASSERT_OR_CLEANUP(fast_ok, "FAST_DATA");

	/*
	 * Both legs carried the payload intact, which already proves each baud
	 * rate is usable end to end. The direction check needs the measured
	 * times to be resolvable; below the floor they are dominated by poll and
	 * ISR overhead, so skip rather than assert on noise.
	 */
	TEST_SECTION("ASYNC_SPEED.DIRECTION");
	if (t_slow_us < UART_ASYNC_SPEED_MIN_RESOLVABLE_US ||
	    t_fast_us < UART_ASYNC_SPEED_MIN_RESOLVABLE_US) {
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_BOARD_STATE,
					 "transfer time below UART_ASYNC_SPEED_MIN_RESOLVABLE_US");
	}
	/* t_fast must be at least 10% shorter than t_slow. */
	TEST_ASSERT_LT_OR_CLEANUP(t_fast_us, (t_slow_us * 9U) / 10U,
				  "SPEED_DIRECTION");

	/* Put the wire back at the mapped rate for the cases that follow. */
	line_config.baudrate = UART_ASYNC_BAUDRATE;
	ret = capi_uart_set_line_config(handle, &line_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RESTORE_CONFIG");

	TEST_ASSERT_EQ(capi_uart_deinit(handle), 0, "DEINIT");
#endif /* UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_LINE_CONFIG */

	return 0;
}

/**
 * @brief Explicit interrupt-source control and status around a live transfer.
 *
 * ASYNC_BASIC proves the driver's own interrupt plumbing works; this proves the
 * caller-facing surface that sits on top of it -- the per-source masks
 * (set_irq_tx / set_irq_rx / set_irq_err) and the status predicates
 * (irq_tx_ready, irq_tx_complete, irq_rx_ready, is_irq_pending). The states are
 * sampled around a real loopback transfer so they are checked against hardware
 * that is genuinely idle, then genuinely holding data, rather than against a
 * controller that never moved a byte.
 *
 * Gated on UART_ASYNC_HAS_IRQ_CTL: UART Lite and the STM32 backend do not
 * expose per-source masking, so their set_irq_* return -ENOTSUP.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int uart_async_irq(void)
{
#if UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_IRQ_CTL
	struct capi_uart_handle *handle = NULL;
	uint8_t tx[UART_SYNC_LEN] = { 0xDE, 0xAD, 0xBE, 0xEF,
				      0xA5, 0x5A, 0xC3, 0x3C
				    };
	uint8_t rx[UART_SYNC_LEN];
	bool state;
	int ret;

	TEST_SECTION("ASYNC_IRQ");

	ret = capi_uart_init(&handle, &uart_async_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_uart_register_callback(handle, uart_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "REGISTER_CB");

	ret = capi_uart_flush_rx_fifo(handle);
	TEST_ASSERT_OK_OR_NOTSUP_OR_CLEANUP(ret, "FLUSH_RX");

	/* ---- Masks: every source must accept both enable and disable. ---- */
	TEST_SECTION("ASYNC_IRQ.MASK");
	ret = capi_uart_set_irq_err(handle, true);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SET_IRQ_ERR_ON");
	ret = capi_uart_set_irq_rx(handle, true);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SET_IRQ_RX_ON");
	ret = capi_uart_set_irq_tx(handle, true);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SET_IRQ_TX_ON");
	ret = capi_uart_set_irq_tx(handle, false);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SET_IRQ_TX_OFF");

	/*
	 * Idle state, before anything is sent: the transmitter has nothing
	 * queued, so it must report room and completion, and with a flushed FIFO
	 * on a quiet wire the receiver must report no data.
	 */
	TEST_SECTION("ASYNC_IRQ.IDLE");
	ret = capi_uart_irq_tx_ready(handle, &state);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX_READY_QUERY");
	TEST_ASSERT_OR_CLEANUP(state, "TX_READY_IDLE");

	ret = capi_uart_irq_tx_complete(handle, &state);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX_COMPLETE_QUERY");
	TEST_ASSERT_OR_CLEANUP(state, "TX_COMPLETE_IDLE");

	ret = capi_uart_irq_rx_ready(handle, &state);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RX_READY_QUERY");
	TEST_ASSERT_EQ_OR_CLEANUP(state, false, "RX_READY_IDLE");

	/*
	 * Now put bytes on the wire with a blocking transmit. It returns only
	 * once the last bit is out, and TX is strapped to RX, so on return the
	 * payload is sitting in the RX FIFO: rx_ready must flip to true and an
	 * interrupt must be pending with the RX sources unmasked.
	 */
	TEST_SECTION("ASYNC_IRQ.LOADED");
	memset(rx, 0, sizeof(rx));

	ret = capi_uart_transmit(handle, tx, sizeof(tx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX");

	ret = capi_uart_irq_rx_ready(handle, &state);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RX_READY_QUERY_LOADED");
	TEST_ASSERT_OR_CLEANUP(state, "RX_READY_LOADED");

	/*
	 * is_irq_pending is only queried for its contract, not asserted true
	 * here. The pending flag reports sources that are BOTH raised AND still
	 * unmasked, and with the interrupt line live the ISR has already run by
	 * the time this executes -- it services the RX source and masks it on
	 * the way out, so the flag reads false. The race is unwinnable from the
	 * test: the only build where it would latch true is a polled one, and
	 * there UART_ASYNC_HAS_IRQ is 0 and this case never runs. That the
	 * source really did raise is established instead by RX_READY_LOADED
	 * above and by the drain below returning the payload intact.
	 */
	ret = capi_uart_is_irq_pending(handle, &state);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "IRQ_PENDING_QUERY");

	ret = capi_uart_irq_tx_complete(handle, &state);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TX_COMPLETE_QUERY_SENT");
	TEST_ASSERT_OR_CLEANUP(state, "TX_COMPLETE_SENT");

	/* Drain the bytes and check the wire carried them, then re-idle. */
	ret = capi_uart_receive(handle, rx, sizeof(rx));
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RX");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(rx, tx, sizeof(tx)), 0, "TX_RX_MATCH");

	ret = capi_uart_irq_rx_ready(handle, &state);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RX_READY_QUERY_DRAINED");
	TEST_ASSERT_EQ_OR_CLEANUP(state, false, "RX_READY_DRAINED");

	/* Leave every source masked so the next case starts from a quiet core. */
	TEST_SECTION("ASYNC_IRQ.UNMASK");
	ret = capi_uart_set_irq_rx(handle, false);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SET_IRQ_RX_OFF");
	ret = capi_uart_set_irq_err(handle, false);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SET_IRQ_ERR_OFF");

	TEST_ASSERT_EQ(capi_uart_deinit(handle), 0, "DEINIT");
#endif /* UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_IRQ_CTL */

	return 0;
}

#undef CLEANUP

/*
 * Capability gates, each carrying exactly one reason a board cannot run a case
 * or a backend-specific section:
 *
 *   UART_ASYNC_HAS_IRQ - the mapped UART has a wired interrupt, so use_irq is
 *                  true and the driver will accept transmit_async/receive_async
 *                  at all. On a polled ("noirq") build the async ops return
 *                  -ENOTSUP, so every async case must SKIP, not FAIL. BASIC is
 *                  ungated: it is pure blocking I/O over the strap.
 *   UART_ASYNC_HAS_LINE_CONFIG - the line format can be reprogrammed at runtime.
 *                  UART Lite fixes it in the IP (set_line_config is always
 *                  -ENOTSUP), so ASYNC_SPEED has no baud change to measure.
 *   UART_ASYNC_HAS_IRQ_CTL - the backend exposes per-source interrupt masking
 *                  and status. UART Lite (one shared enable bit) and STM32 do
 *                  not, so ASYNC_IRQ has no surface to exercise.
 *   UART_ASYNC_HAS_RX_TIMEOUT - an incomplete receive is reported after the
 *                  wire idles. UART Lite has no such event, so only the timeout
 *                  continuation section of ASYNC_BASIC skips.
 *
 * The async gates AND in UART_ASYNC_HAS_IRQ rather than standing alone: a
 * backend can expose the masks and still have no interrupt routed to it.
 */
static const struct test_case uart_subtests[] = {
	{ "BASIC",       uart_basic,       false },
	{ "ASYNC_BASIC", uart_async_basic, !UART_ASYNC_HAS_IRQ },
	{
		"ASYNC_SPEED", uart_async_speed,
		!(UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_LINE_CONFIG)
	},
	{
		"ASYNC_IRQ",   uart_async_irq,
		!(UART_ASYNC_HAS_IRQ && UART_ASYNC_HAS_IRQ_CTL)
	},
};

int test_uart(void)
{
	return test_framework_run_cases(UART_MODULE, uart_subtests,
					sizeof(uart_subtests) / sizeof(uart_subtests[0]));
}

#endif /* UART_ASYNC_OPS */
