/***************************************************************************//**
 * @file test_dma.c
 * @brief CAPI DMA memory-to-memory tests.
 *
 * Exercises the DMA driver using memory-to-memory transfers that require
 * no external hardware.  On STM32F7 only DMA2 supports mem-to-mem; the
 * platform layer (parameters.h) selects the correct stream/channel.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
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
#define DMA_CHAN_ID	0U
#define DMA_REINIT_COUNT 5U

#ifndef DMA_PLATFORM_INIT
#define DMA_PLATFORM_INIT()	do { } while (0)
#endif

/**
 * @brief Basic memory-to-memory DMA: fill src, transfer, verify dst matches.
 *
 * Allocates source and destination buffers, fills the source with a known
 * pattern, configures a mem-to-mem transfer in polling mode, and verifies
 * every byte was copied correctly.
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
	TEST_ASSERT_EQ(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT(dst != NULL, "ALLOC_DST");

	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
		src[i] = (uint8_t)(i & 0xFFU);

	struct capi_dma_transfer xfer = {
		.src = (capi_dma_glbl_addr_t)src,
		.dst = (capi_dma_glbl_addr_t)dst,
		.length = DMA_XFER_SIZE,
		.xfer_type = CAPI_DMA_MEM_TO_MEM,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.extra = &dma_xfer_extra,
	};

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ(ret, 0, "CONFIG_XFER");

	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ(ret, 0, "XFER_START");

	TEST_ASSERT(capi_dma_chan_is_completed(chan), "XFER_COMPLETED");

	int mismatch = memcmp(src, dst, DMA_XFER_SIZE);
	TEST_ASSERT_EQ(mismatch, 0, "DATA_MATCH");

	capi_free(src);
	capi_free(dst);
	capi_dma_deinit_chan(chan);
	capi_dma_deinit(dma);

	return 0;
}

/**
 * @brief Transfer with different data patterns to catch stuck bits or aliasing.
 *
 * Uses 0xFF fill and 0xAA/0x55 alternating pattern to verify the DMA engine
 * does not mask or alias any bit positions.
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
	TEST_ASSERT_EQ(ret, 0, "INIT");

	ret = capi_dma_init_chan(dma, &chan, DMA_CHAN_ID);
	TEST_ASSERT_EQ(ret, 0, "INIT_CHAN");

	src = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT(src != NULL, "ALLOC_SRC");
	dst = (uint8_t *)capi_calloc(1, DMA_XFER_SIZE);
	TEST_ASSERT(dst != NULL, "ALLOC_DST");

	memset(src, 0xFF, DMA_XFER_SIZE);

	struct capi_dma_transfer xfer = {
		.src = (capi_dma_glbl_addr_t)src,
		.dst = (capi_dma_glbl_addr_t)dst,
		.length = DMA_XFER_SIZE,
		.xfer_type = CAPI_DMA_MEM_TO_MEM,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.extra = &dma_xfer_extra,
	};

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ(ret, 0, "CONFIG_XFER_FF");

	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ(ret, 0, "XFER_START_FF");
	TEST_ASSERT(capi_dma_chan_is_completed(chan), "XFER_COMPLETED_FF");
	TEST_ASSERT_EQ(memcmp(src, dst, DMA_XFER_SIZE), 0, "DATA_MATCH_FF");

	for (uint32_t i = 0U; i < DMA_XFER_SIZE; i++)
		src[i] = (i & 1U) ? 0xAAU : 0x55U;
	memset(dst, 0, DMA_XFER_SIZE);

	xfer.src = (capi_dma_glbl_addr_t)src;
	xfer.dst = (capi_dma_glbl_addr_t)dst;

	ret = capi_dma_config_xfer(chan, &xfer);
	TEST_ASSERT_EQ(ret, 0, "CONFIG_XFER_AA55");

	ret = capi_dma_xfer_start(chan);
	TEST_ASSERT_EQ(ret, 0, "XFER_START_AA55");
	TEST_ASSERT(capi_dma_chan_is_completed(chan), "XFER_COMPLETED_AA55");
	TEST_ASSERT_EQ(memcmp(src, dst, DMA_XFER_SIZE), 0, "DATA_MATCH_AA55");

	capi_free(src);
	capi_free(dst);
	capi_dma_deinit_chan(chan);
	capi_dma_deinit(dma);

	return 0;
}

/**
 * @brief Stress-test init/deinit cycles to catch resource leaks.
 */
static int dma_reinit(void)
{
	struct capi_dma_handle *dma = NULL;
	int ret = 0;

	TEST_SECTION("REINIT");

	DMA_PLATFORM_INIT();

	for (uint32_t i = 0U; i < DMA_REINIT_COUNT; i++) {
		ret = capi_dma_init(&dma, &dma_config);
		if (ret != 0)
			break;
		ret = capi_dma_deinit(dma);
		dma = NULL;
		if (ret != 0)
			break;
	}
	TEST_VALUE("DMA.REINIT.iterations", DMA_REINIT_COUNT);
	TEST_ASSERT_EQ(ret, 0, "REINIT_LOOP");

	return 0;
}

/**
 * @brief Reject invalid arguments: NULL handles, NULL configs.
 */
static int dma_error_paths(void)
{
	struct capi_dma_handle *dma = NULL;
	struct capi_dma_chan *chan = NULL;
	int ret;

	TEST_SECTION("ERROR_PATHS");

	TEST_ASSERT_EQ(capi_dma_init(NULL, &dma_config), -EINVAL,
		       "INIT_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_dma_init(&dma, NULL), -EINVAL,
		       "INIT_NULL_CONFIG");
	TEST_ASSERT_EQ(capi_dma_deinit(NULL), -EINVAL, "DEINIT_NULL");

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&dma, &dma_config);
	TEST_ASSERT_EQ(ret, 0, "INIT_FOR_ERROR_PATHS");

	TEST_ASSERT_EQ(capi_dma_init_chan(NULL, &chan, 0U), -EINVAL,
		       "INIT_CHAN_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_dma_init_chan(dma, NULL, 0U), -EINVAL,
		       "INIT_CHAN_NULL_PTR");
	TEST_ASSERT_EQ(capi_dma_init_chan(dma, &chan, 99U), -EINVAL,
		       "INIT_CHAN_BAD_ID");
	TEST_ASSERT_EQ(capi_dma_config_xfer(NULL, NULL), -EINVAL,
		       "CONFIG_XFER_NULL");
	TEST_ASSERT_EQ(capi_dma_xfer_start(NULL), -EINVAL,
		       "XFER_START_NULL");
	TEST_ASSERT_EQ(capi_dma_xfer_abort(NULL), -EINVAL,
		       "XFER_ABORT_NULL");
	TEST_ASSERT_EQ(capi_dma_deinit_chan(NULL), -EINVAL,
		       "DEINIT_CHAN_NULL");

	capi_dma_deinit(dma);

	return 0;
}

static const struct test_case dma_subtests[] = {
	{ "BASIC",       dma_basic,       false },
	{ "PATTERN",     dma_pattern,     false },
	{ "REINIT",      dma_reinit,      false },
	{ "ERROR_PATHS", dma_error_paths, false },
};

/**
 * @brief Exercise the CAPI DMA driver with memory-to-memory transfers.
 *
 * Runs the DMA subtest table: basic mem-to-mem copy, pattern verification,
 * controller re-init stress, and error/argument-validation paths.  No external
 * hardware is required — all transfers stay inside SRAM.
 *
 * @return 0 on pass, first non-zero subtest error otherwise.
 */
int test_dma(void)
{
	return test_framework_run_cases(DMA_MODULE, dma_subtests,
					sizeof(dma_subtests) / sizeof(dma_subtests[0]));
}

#endif /* DMA_OPS */
