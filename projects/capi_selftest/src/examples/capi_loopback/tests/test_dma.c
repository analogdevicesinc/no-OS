/***************************************************************************//**
 * @file test_dma.c
 * @brief CAPI DMA memory-to-memory tests.
 *
 * Exercises the DMA driver using memory-to-memory transfers that require
 * no external hardware.  On STM32F7 only DMA2 supports mem-to-mem; the
 * platform layer (parameters.h) selects the correct stream/channel.
 *
 * Every case moves real bytes through the DMA engine and verifies the
 * destination with memcmp against an oracle held in SRAM -- the tests check
 * observable transfer behavior, never the driver's internal state or argument
 * validation. All transfers stay inside SRAM, so the suite needs no wiring.
 *
 * API coverage:
 *   capi_dma_init / capi_dma_deinit             - BASIC, LIFECYCLE
 *   capi_dma_init_chan / capi_dma_deinit_chan   - BASIC, LIFECYCLE
 *   capi_dma_config_xfer                        - all cases
 *   capi_dma_xfer_start                         - all cases (polling path)
 *   capi_dma_chan_is_completed                  - BASIC (lifecycle), all cases
 *   capi_dma_xfer_abort                         - ABORT
 *   capi_dma_register_complete_callback         - ASYNC (skipped when no IRQ)
 *
 * Behavior verified:
 *   BASIC     - incrementing copy + is_completed goes false on config, true
 *               after a polled start.
 *   PATTERN   - 0x00 / 0xFF / 0xAA55 / walking-ones survive intact (no stuck
 *               or aliased bits).
 *   SIZES     - a length sweep copies exactly N bytes and never overruns the
 *               destination (guard bytes past N stay untouched).
 *   SRC_FIXED - src_inc = NO_INCREMENT replays src[0] into every dst byte.
 *   DST_FIXED - dst_inc = NO_INCREMENT lands the last source byte in dst[0]
 *               and leaves the rest of dst untouched.
 *   REUSE     - one channel accepts many config+start cycles back to back.
 *   ABORT     - abort after completion returns 0 and the channel still works.
 *   LIFECYCLE - deinit/re-init resets the singleton; a second init_chan on a
 *               live id reports -EBUSY and an out-of-range id reports -EINVAL.
 *   ASYNC     - IRQ-delivered completion via a registered callback (only on
 *               platforms whose backend has an IRQ completion path).
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "test_framework.h"
#include "test_dma.h"

#ifndef DMA_OPS

int test_dma(void)
{
	static const struct test_case stub[] = {
		{ "NOT_CONFIGURED", NULL, false },
	};

	return test_framework_run_cases("DMA", stub, 1U);
}

#else /* DMA_OPS defined — full implementation follows */

#include "capi_alloc.h"
#include "capi_dma.h"
#include "common_data.h"

#define DMA_MODULE	"DMA"

#ifndef DMA_PLATFORM_INIT
#define DMA_PLATFORM_INIT()	do { } while (0)
#endif

/*
 * Shared teardown for every DMA case. Each test declares dma/chan/src/dst and
 * NULLs the ones it does not use; the guards keep a single definition valid at
 * every exit point and idempotent across the success and failure paths. Return
 * codes are dropped on purpose: the failure that triggered CLEANUP is the one
 * worth reporting.
 */
#define CLEANUP \
	do { \
		if (src != NULL) { \
			capi_free(src); \
			src = NULL; \
		} \
		if (dst != NULL) { \
			capi_free(dst); \
			dst = NULL; \
		} \
		if (chan != NULL) { \
			(void)capi_dma_deinit_chan(chan); \
			chan = NULL; \
		} \
		if (dma != NULL) { \
			(void)capi_dma_deinit(dma); \
			dma = NULL; \
		} \
	} while (0)

/**
 * @brief Build a mem-to-mem transfer descriptor with byte increments.
 *
 * Fills the common fields shared by most cases; callers override src_inc /
 * dst_inc / length as needed. src_size and dst_size are set for portability
 * even though the STM32 backend derives element width from the channel extra.
 */
static struct capi_dma_transfer dma_make_xfer(const uint8_t *src,
		uint8_t *dst, uint32_t length)
{
	struct capi_dma_transfer xfer = {
		.src = (capi_dma_glbl_addr_t)src,
		.dst = (capi_dma_glbl_addr_t)dst,
		.length = length,
		.xfer_type = CAPI_DMA_MEM_TO_MEM,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.extra = &dma_xfer_extra,
	};

	return xfer;
}

/**
 * @brief Basic memory-to-memory DMA plus the is_completed lifecycle.
 *
 * Fills the source with an incrementing pattern, confirms is_completed reads
 * true on a freshly opened channel, goes false once a transfer is configured,
 * and returns to true after the polled start; then checks every byte copied.
 */
static int dma_basic(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	int ret;

	TEST_SECTION("BASIC");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	/* A channel that has never run a transfer reports completed. */
	TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan), "IDLE_COMPLETED");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
		src[i] = (uint8_t)(i & 0xFFU);

	struct capi_dma_transfer xfer = dma_make_xfer(src, dst, DMA_XFER_SIZE);

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONFIG_XFER");

	/* Configuring a transfer arms it: not yet complete. */
	TEST_ASSERT_OR_CLEANUP(!capi_dma_chan_is_completed(chan),
			       "ARMED_NOT_COMPLETED");

	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_START");

	TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan),
			       "XFER_COMPLETED");

	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(src, dst, DMA_XFER_SIZE), 0,
				  "DATA_MATCH");

	CLEANUP;
	return 0;
}

/**
 * @brief Transfer several bit patterns to catch stuck bits or aliasing.
 *
 * Runs 0x00, 0xFF, alternating 0xAA/0x55 and a walking-ones pattern through the
 * same channel back to back, verifying each lands intact. If any data line were
 * stuck or aliased, at least one of these patterns would mismatch.
 */
static int dma_pattern(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	int ret;

	TEST_SECTION("PATTERN");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	for (uint32_t p = 0U; p < 4U; p++) {
		for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++) {
			switch (p) {
			case 0U:
				src[i] = 0x00U;
				break;
			case 1U:
				src[i] = 0xFFU;
				break;
			case 2U:
				src[i] = (i & 1U) ? 0xAAU : 0x55U;
				break;
			default:
				src[i] = (uint8_t)(1U << (i & 7U));
				break;
			}
		}
		/* Poison dst so a no-op "transfer" cannot pass by accident. */
		memset(dst, (int)(~p & 0xFFU), DMA_XFER_SIZE);

		struct capi_dma_transfer xfer =
			dma_make_xfer(src, dst, DMA_XFER_SIZE);

		ret = capi_dma_config_xfer(chan, &xfer);
		TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONFIG_XFER");

		ret = capi_dma_xfer_start(chan);
		TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_START");

		TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan),
				       "XFER_COMPLETED");
		TEST_ASSERT_EQ_OR_CLEANUP(memcmp(src, dst, DMA_XFER_SIZE), 0,
					  "DATA_MATCH");
	}

	CLEANUP;
	return 0;
}

/**
 * @brief Sweep transfer lengths and confirm the DMA never overruns.
 *
 * Copies 1, 2, 3, 7, 16, 31, 64 and 255-byte transfers through the same
 * channel. For each length the copied region must match the source, and the
 * guard byte just past the region must keep its poison value -- proving the
 * engine wrote exactly @c length bytes and not one more.
 */
static int dma_sizes(void)
{
	static const uint32_t lengths[] = { 1U, 2U, 3U, 7U, 16U, 31U, 64U, 255U };
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	int ret;

	TEST_SECTION("SIZES");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_MAX_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_MAX_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	for (uint32_t i = 0U; i < DMA_MAX_XFER_SIZE; i++)
		src[i] = (uint8_t)((i * 7U + 1U) & 0xFFU);

	/*
	 * Sweep every length but emit a single PASS for the whole sweep. On the
	 * first failure, record the stage that failed and the offending length
	 * so the one logged line still pins down what broke.
	 */
	bool sizes_ok = true;
	const char *fail_stage = "ALL_SIZES";
	uint32_t fail_len = 0U;

	for (uint32_t n = 0U; n < sizeof(lengths) / sizeof(lengths[0]); n++) {
		uint32_t len = lengths[n];

		if (len > DMA_MAX_XFER_SIZE)
			continue;

		/* Fill dst with a poison the source never contains. */
		memset(dst, 0xC3, DMA_MAX_XFER_SIZE);

		struct capi_dma_transfer xfer = dma_make_xfer(src, dst, len);

		if (capi_dma_config_xfer(chan, &xfer) != 0) {
			fail_stage = "CONFIG_XFER";
			fail_len = len;
			sizes_ok = false;
			break;
		}
		if (capi_dma_xfer_start(chan) != 0) {
			fail_stage = "XFER_START";
			fail_len = len;
			sizes_ok = false;
			break;
		}
		if (!capi_dma_chan_is_completed(chan)) {
			fail_stage = "XFER_COMPLETED";
			fail_len = len;
			sizes_ok = false;
			break;
		}
		if (memcmp(src, dst, len) != 0) {
			fail_stage = "DATA_MATCH";
			fail_len = len;
			sizes_ok = false;
			break;
		}
		/* The byte past the region must be untouched (no overrun). */
		if (len < DMA_MAX_XFER_SIZE && dst[len] != 0xC3) {
			fail_stage = "NO_OVERRUN";
			fail_len = len;
			sizes_ok = false;
			break;
		}
	}

	if (!sizes_ok)
		TEST_VALUE("FAIL_LEN", fail_len);
	TEST_ASSERT_OR_CLEANUP(sizes_ok, fail_stage);

	CLEANUP;
	return 0;
}

/**
 * @brief Fixed source address: src_inc = NO_INCREMENT replays src[0].
 *
 * With the source increment disabled the engine reads the same source byte for
 * the whole transfer, so every destination byte must equal src[0].
 */
static int dma_src_fixed(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	int ret;

	TEST_SECTION("SRC_FIXED");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	/* Distinct bytes throughout so a stuck increment would show up. */
	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
		src[i] = (uint8_t)(0x80U + i);
	memset(dst, 0x00, DMA_XFER_SIZE);

	struct capi_dma_transfer xfer = dma_make_xfer(src, dst, DMA_XFER_SIZE);
	xfer.src_inc = CAPI_DMA_NO_INCREMENT;

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONFIG_XFER");

	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_START");
	TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan),
			       "XFER_COMPLETED");

	/* Verify every byte once, but log a single PASS for the whole check. */
	bool all_eq_src0 = true;
	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++) {
		if (dst[i] != src[0]) {
			all_eq_src0 = false;
			break;
		}
	}
	TEST_ASSERT_OR_CLEANUP(all_eq_src0, "DST_EQ_SRC0");

	CLEANUP;
	return 0;
}

/**
 * @brief Fixed destination address: dst_inc = NO_INCREMENT collapses to dst[0].
 *
 * With the destination increment disabled every source byte is written to the
 * same destination address, so the last source byte wins in dst[0] and the rest
 * of the destination buffer stays at its poison value.
 */
static int dma_dst_fixed(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	int ret;

	TEST_SECTION("DST_FIXED");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
		src[i] = (uint8_t)(0x80U + i);
	memset(dst, 0x5A, DMA_XFER_SIZE);

	struct capi_dma_transfer xfer = dma_make_xfer(src, dst, DMA_XFER_SIZE);
	xfer.dst_inc = CAPI_DMA_NO_INCREMENT;

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONFIG_XFER");

	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_START");
	TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan),
			       "XFER_COMPLETED");

	/* Last source byte landed in dst[0]. */
	TEST_ASSERT_EQ_OR_CLEANUP(dst[0], src[DMA_XFER_SIZE - 1U],
				  "DST0_EQ_LAST");
	/* Nothing past dst[0] was written; log a single PASS for the whole check. */
	bool rest_untouched = true;
	for (uint32_t i = 1U; i < DMA_XFER_SIZE; i++) {
		if (dst[i] != 0x5A) {
			rest_untouched = false;
			break;
		}
	}
	TEST_ASSERT_OR_CLEANUP(rest_untouched, "REST_UNTOUCHED");

	CLEANUP;
	return 0;
}

/**
 * @brief Reuse one channel for many back-to-back transfers.
 *
 * Runs a batch of config+start cycles on a single channel, each with a
 * different byte pattern, to prove the channel is re-armable without a
 * deinit/init in between and that no state leaks across transfers.
 */
static int dma_reuse(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	int ret;

	TEST_SECTION("REUSE");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	/*
	 * Run all re-arm cycles but emit a single PASS for the batch. On the
	 * first failure, record the failing stage and iteration so the one
	 * logged line still identifies which cycle broke.
	 */
	bool reuse_ok = true;
	const char *fail_stage = "REUSE_ALL";
	uint32_t fail_iter = 0U;

	for (uint32_t iter = 0U; iter < 16U; iter++) {
		for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
			src[i] = (uint8_t)((i + iter * 13U) & 0xFFU);
		memset(dst, (int)(iter & 0xFFU), DMA_XFER_SIZE);

		struct capi_dma_transfer xfer =
			dma_make_xfer(src, dst, DMA_XFER_SIZE);

		if (capi_dma_config_xfer(chan, &xfer) != 0) {
			fail_stage = "CONFIG_XFER";
			fail_iter = iter;
			reuse_ok = false;
			break;
		}
		if (capi_dma_xfer_start(chan) != 0) {
			fail_stage = "XFER_START";
			fail_iter = iter;
			reuse_ok = false;
			break;
		}
		if (!capi_dma_chan_is_completed(chan)) {
			fail_stage = "XFER_COMPLETED";
			fail_iter = iter;
			reuse_ok = false;
			break;
		}
		if (memcmp(src, dst, DMA_XFER_SIZE) != 0) {
			fail_stage = "DATA_MATCH";
			fail_iter = iter;
			reuse_ok = false;
			break;
		}
	}

	if (!reuse_ok)
		TEST_VALUE("FAIL_ITER", fail_iter);
	TEST_ASSERT_OR_CLEANUP(reuse_ok, fail_stage);

	CLEANUP;
	return 0;
}

/**
 * @brief Abort a completed transfer, then prove the channel still works.
 *
 * The polling backend finishes the copy inside xfer_start, so this aborts an
 * already-completed transfer: the abort must return 0 (and leave the channel
 * marked completed), and a following transfer must still copy correctly. This
 * exercises the abort path and channel recovery without needing to catch a
 * transfer mid-flight.
 */
static int dma_abort(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	int ret;

	TEST_SECTION("ABORT");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
		src[i] = (uint8_t)(i & 0xFFU);

	struct capi_dma_transfer xfer = dma_make_xfer(src, dst, DMA_XFER_SIZE);

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONFIG_XFER");
	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_START");
	TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan),
			       "XFER_COMPLETED");

	/* Abort of a settled channel is a well-defined no-op that returns 0. */
	ret = capi_dma_xfer_abort(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "ABORT_OK");
	TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan),
			       "ABORT_COMPLETED");

	/* Recovery: the channel must still copy after an abort. */
	memset(dst, 0x00, DMA_XFER_SIZE);
	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RECOVER_CONFIG");
	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RECOVER_START");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(src, dst, DMA_XFER_SIZE), 0,
				  "RECOVER_MATCH");

	CLEANUP;
	return 0;
}

/**
 * @brief Handle lifecycle: deinit/re-init and channel-open error reporting.
 *
 * Opens the controller and a channel, checks that a second init_chan on the
 * same id reports -EBUSY and an out-of-range id reports -EINVAL, then tears the
 * controller fully down and brings it back up to prove the singleton resets.
 */
static int dma_lifecycle(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	struct capi_dma_chan *chan_dup = NULL;
	uint8_t *src = NULL;	/* unused; keeps CLEANUP uniform */
	uint8_t *dst = NULL;	/* unused; keeps CLEANUP uniform */
	int ret;

	TEST_SECTION("LIFECYCLE");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	/* A second open of the live id must be rejected, not double-allocated. */
	ret = capi_dma_init_chan(dma, &chan_dup, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, -EBUSY, "DUP_CHAN_EBUSY");
	TEST_ASSERT_OR_CLEANUP(chan_dup == NULL, "DUP_CHAN_NULL");

	/* An id at/above num_chans is invalid. */
	ret = capi_dma_init_chan(dma, &chan_dup, DMA_NUM_CHANS);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, -EINVAL, "BAD_ID_EINVAL");

	/* Full teardown. */
	ret = capi_dma_deinit_chan(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DEINIT_CHAN");
	chan = NULL;
	ret = capi_dma_deinit(dma);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DEINIT");
	dma = NULL;

	/* Re-init after a full deinit must succeed (singleton was reset). */
	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "REINIT");
	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "REINIT_CHAN");

	CLEANUP;
	return 0;
}

/**
 * @brief Completion callback for the ASYNC case: counts invocations.
 * @param event - Completion event code from the backend (unused here).
 * @param ctx   - Points at the test's volatile int invocation counter.
 */
static void dma_async_cb(uint32_t event, void *ctx)
{
	(void)event;

	if (ctx)
		(*(volatile int *)ctx)++;
}

/**
 * @brief IRQ-delivered completion via a registered callback.
 *
 * Only meaningful on a backend that raises a completion IRQ and implements
 * register_complete_callback (DMA_HAS_IRQ). The case is table-skipped when the
 * platform runs mem-to-mem in polling mode, so it never runs on STM32.
 */
static int dma_async(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	volatile int cb_count = 0;
	int ret;

	TEST_SECTION("ASYNC");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT_OR_CLEANUP(dst != NULL, "ALLOC_DST");

	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
		src[i] = (uint8_t)(i & 0xFFU);

	ret = capi_dma_register_complete_callback(chan, dma_async_cb,
			(void *)&cb_count);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "REGISTER_CB");

	struct capi_dma_transfer xfer = dma_make_xfer(src, dst, DMA_XFER_SIZE);

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "CONFIG_XFER");

	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_START");

	TEST_WAIT_UNTIL(capi_dma_chan_is_completed(chan), 1000000U, 100U);

	TEST_ASSERT_OR_CLEANUP(capi_dma_chan_is_completed(chan),
			       "XFER_COMPLETED");
	TEST_ASSERT_EQ_OR_CLEANUP(cb_count, 1, "CB_FIRED_ONCE");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(src, dst, DMA_XFER_SIZE), 0,
				  "DATA_MATCH");

	CLEANUP;
	return 0;
}

#undef CLEANUP

static const struct test_case dma_subtests[] = {
	{ "BASIC",     dma_basic,     false },
	{ "PATTERN",   dma_pattern,   false },
	{ "SIZES",     dma_sizes,     false },
	{ "SRC_FIXED", dma_src_fixed, false },
	{ "DST_FIXED", dma_dst_fixed, false },
	{ "REUSE",     dma_reuse,     false },
	{ "ABORT",     dma_abort,     false },
	{ "LIFECYCLE", dma_lifecycle, false },
	{ "ASYNC",     dma_async,     !DMA_HAS_IRQ },
};

/**
 * @brief Exercise the CAPI DMA driver with memory-to-memory transfers.
 *
 * Runs the DMA subtest table. Every executed case moves real bytes through the
 * DMA engine and verifies the destination with memcmp; the ASYNC case is
 * skipped on platforms without an IRQ completion path. No external hardware is
 * required -- all transfers stay inside SRAM.
 *
 * @return 0 on pass, first non-zero subtest error otherwise.
 */
int test_dma(void)
{
	return test_framework_run_cases(DMA_MODULE, dma_subtests,
					sizeof(dma_subtests) / sizeof(dma_subtests[0]));
}

#endif /* DMA_OPS */
