/***************************************************************************//**
 * @file test_spi.c
 * @brief CAPI SPI loopback tests.
 *
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "capi_spi.h"
#include "common_data.h"
#include "test_framework.h"
#include "test_spi.h"


#define SPI_MODULE	"SPI"

/**
 * @brief Synchronous SPI loopback across many transfer shapes.
 *
 * Hardware assumption: MOSI is physically wired back to MISO (e.g. JC2->JC3
 * on ZedBoard). Every "*_MATCH" assertion relies on this jumper — without it
 * the transfer still returns 0 (SPI has no ACK) but the data check fails.
 *
 * One controller is initialised once and reused for every sub-case; only the
 * transfer descriptor and buffers change between them.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_loopback(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	int ret;

	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	/* Select controller for the device; the rest is decided by the platform. */
	spi_dev.controller = spi_handle;

	/* ----------------------------------------------------------------------
	 * Baseline: a simple 4-byte round trip proves the bus works at all.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("SPI baseline loopback");
	uint8_t tx[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
	uint8_t rx[4] = { 0 };
	struct capi_spi_transfer xfer = {
		.tx_buf = tx, .rx_buf = rx,
		.tx_size = 4, .rx_size = 4,
		.timeout = 0,
	};
	ret = capi_spi_transceive(&spi_dev, &xfer);
	TEST_ASSERT_EQ(ret, 0, "TRANSCEIVE_FUNC");
	TEST_ASSERT_EQ(memcmp(rx, tx, sizeof(tx)), 0, "LOOPBACK_SYNC");

	/* ----------------------------------------------------------------------
	 * Walking-1s: exactly one bit set per byte, marching across all eight
	 * positions. Catches a data line stuck low or shorted to a neighbour.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Walking-1s pattern");
	uint8_t walk[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	uint8_t walk_rx[8] = { 0 };
	struct capi_spi_transfer walk_xfer = {
		.tx_buf = walk, .rx_buf = walk_rx,
		.tx_size = sizeof(walk), .rx_size = sizeof(walk),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &walk_xfer), 0, "WALK1");
	TEST_ASSERT_EQ(memcmp(walk_rx, walk, sizeof(walk)), 0, "WALK1_MATCH");

	/* ----------------------------------------------------------------------
	 * Isolated extremes: an all-zero byte then an all-one byte. Confirms the
	 * two boundary values survive on their own (stuck-at-1 / stuck-at-0).
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Extreme values 0x00 / 0xFF");
	uint8_t zero_tx = 0x00, zero_rx = 0xFF;
	struct capi_spi_transfer zero_xfer = {
		.tx_buf = &zero_tx, .rx_buf = &zero_rx,
		.tx_size = 1, .rx_size = 1,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &zero_xfer), 0, "ZERO");
	TEST_ASSERT_EQ(zero_rx, 0x00, "ZERO_MATCH");

	uint8_t ff_tx = 0xFF, ff_rx = 0x00;
	struct capi_spi_transfer ff_xfer = {
		.tx_buf = &ff_tx, .rx_buf = &ff_rx,
		.tx_size = 1, .rx_size = 1,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &ff_xfer), 0, "MAX");
	TEST_ASSERT_EQ(ff_rx, 0xFF, "MAX_MATCH");

	/* ----------------------------------------------------------------------
	 * Single byte: the shortest possible transfer, exercising the minimum
	 * length path of the driver.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Single-byte transfer");
	uint8_t one_tx = 0xAB, one_rx = 0;
	struct capi_spi_transfer one_xfer = {
		.tx_buf = &one_tx, .rx_buf = &one_rx,
		.tx_size = 1, .rx_size = 1,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &one_xfer), 0, "ONE_BYTE");
	TEST_ASSERT_EQ(one_rx, one_tx, "ONE_BYTE_MATCH");

	/* ----------------------------------------------------------------------
	 * Large transfer: 64 bytes exceed the hardware FIFO depth, so the driver
	 * must refill mid-transfer. A ramp pattern makes any dropped/duplicated
	 * byte easy to spot.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Large transfer (FIFO refill)");
	uint8_t big_tx[64], big_rx[64];
	for (int i = 0; i < 64; i++)
		big_tx[i] = (uint8_t)i;
	memset(big_rx, 0, sizeof(big_rx));
	struct capi_spi_transfer big_xfer = {
		.tx_buf = big_tx, .rx_buf = big_rx,
		.tx_size = sizeof(big_tx), .rx_size = sizeof(big_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &big_xfer), 0, "BIG");
	TEST_ASSERT_EQ(memcmp(big_rx, big_tx, sizeof(big_tx)), 0, "BIG_MATCH");

	/* ----------------------------------------------------------------------
	 * tx_size < rx_size: the API allows rx_size to differ from tx_size. The
	 * transfer must succeed and the bytes that had real TX data must loop
	 * back. What is clocked out for the trailing RX-only bytes is backend
	 * defined (the header does not specify it), so only the TX-covered bytes
	 * are checked here.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Unequal length: tx < rx");
	uint8_t short_tx[2] = { 0xA5, 0x5A };
	uint8_t long_rx[5];
	memset(long_rx, 0xEE, sizeof(long_rx));
	struct capi_spi_transfer short_xfer = {
		.tx_buf = short_tx, .rx_buf = long_rx,
		.tx_size = sizeof(short_tx), .rx_size = sizeof(long_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &short_xfer), 0, "TX_SHORT");
	TEST_ASSERT_EQ(memcmp(long_rx, short_tx, sizeof(short_tx)), 0,
		       "TX_SHORT_MATCH");

	/* ----------------------------------------------------------------------
	 * tx_size > rx_size with guard bytes framing the RX buffer: proves the
	 * driver writes exactly rx_size bytes and never overruns either side.
	 * rx_guard[0] and rx_guard[3] must stay untouched; only [1] and [2] are
	 * the real RX target.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Unequal length: tx > rx (guard bytes)");
	uint8_t long_tx[5] = { 0x10, 0x20, 0x30, 0x40, 0x50 };
	uint8_t rx_guard[4] = { 0xCC, 0x00, 0x00, 0xDD };
	struct capi_spi_transfer long_xfer = {
		.tx_buf = long_tx, .rx_buf = &rx_guard[1],
		.tx_size = sizeof(long_tx), .rx_size = 2,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &long_xfer), 0, "TX_LONG");
	TEST_ASSERT_EQ(rx_guard[0], 0xCC, "GUARD_PRE");
	TEST_ASSERT_EQ(rx_guard[1], 0x10, "RX0");
	TEST_ASSERT_EQ(rx_guard[2], 0x20, "RX1");
	TEST_ASSERT_EQ(rx_guard[3], 0xDD, "GUARD_POST");

	/* ----------------------------------------------------------------------
	 * NULL tx_buf: the driver clocks out zeros instead of failing, so the
	 * loopback readback is all zeros.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("NULL tx_buf (sends zeros)");
	uint8_t null_tx_rx[4];
	memset(null_tx_rx, 0xA5, sizeof(null_tx_rx));
	struct capi_spi_transfer null_tx_xfer = {
		.tx_buf = NULL, .rx_buf = null_tx_rx,
		.tx_size = 0, .rx_size = sizeof(null_tx_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &null_tx_xfer), 0, "NULL_TX");
	uint8_t zero4[4] = { 0 };
	TEST_ASSERT_EQ(memcmp(null_tx_rx, zero4, sizeof(null_tx_rx)), 0,
		       "NULL_TX_MATCH");

	/* ----------------------------------------------------------------------
	 * NULL rx_buf: the driver discards received bytes without crashing.
	 * There is nothing to compare; success is simply returning 0.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("NULL rx_buf (discards data)");
	uint8_t discard_tx[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
	struct capi_spi_transfer null_rx_xfer = {
		.tx_buf = discard_tx, .rx_buf = NULL,
		.tx_size = sizeof(discard_tx), .rx_size = 0,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&spi_dev, &null_rx_xfer), 0, "NULL_RX");

	/* ----------------------------------------------------------------------
	 * Deinit: must return 0, and must leave the controller in a clean state.
	 * We prove "clean" by re-initialising on the same identifier — if deinit
	 * left the hardware busy or interrupts armed, this second init fails.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Deinit and re-init");
	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	spi_handle = NULL;
	TEST_ASSERT_EQ(capi_spi_init(&spi_handle, &spi_controller_config), 0,
		       "REINIT");
	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "REINIT_DEINIT");

	return 0;
}

/**
 * @brief Exercise the chip-select control API at the CAPI contract level.
 *
 * Only behaviour guaranteed by the CAPI layer for every backend is asserted
 * here:
 *   - the dispatcher rejects a NULL device with -EINVAL;
 *   - manual assert/deassert is either honoured (0) or reported unsupported
 *     (-ENOTSUP) — both are valid for a conformant backend.
 *
 * Backend-specific policies (rejecting AUTO, rejecting multi-bit CS masks,
 * register-level CS line checks) are intentionally NOT tested here — those
 * belong in a platform-specific test, not the portable self-test.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_cs_loopback(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	int ret;

	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	spi_dev.controller = spi_handle;

	/* ----------------------------------------------------------------------
	 * Manual assert/deassert: a backend may support manual CS (returns 0) or
	 * not (returns -ENOTSUP). Either is a valid CAPI response.
	 * -------------------------------------------------------------------- */
	TEST_SECTION("Manual CS assert/deassert");
	spi_dev.native_cs = 0x01;
	TEST_ASSERT_OK_OR_NOTSUP(
		capi_spi_set_cs(&spi_dev, CAPI_SPI_CS_MANUAL_ASSERT), "CS_ASSERT");
	TEST_ASSERT_OK_OR_NOTSUP(
		capi_spi_set_cs(&spi_dev, CAPI_SPI_CS_MANUAL_DEASSERT), "CS_DEASSERT");

	/* ----------------------------------------------------------------------
	 * General contract: the dispatcher rejects a NULL device for every
	 * backend (see capi_spi_set_cs() in the CAPI layer).
	 * -------------------------------------------------------------------- */
	TEST_SECTION("CS control rejects NULL device");
	TEST_ASSERT_EQ(capi_spi_set_cs(NULL, CAPI_SPI_CS_MANUAL_ASSERT), -EINVAL,
		       "CS_NULL_DEV_REJECT");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");
	return 0;
}

typedef int (*spi_test_func_t)(void);

struct spi_test_entry {
	const char *name;
	spi_test_func_t run;
};

int test_spi(void)
{
	static const struct spi_test_entry tests[] = {
		{ "LOOPBACK",    spi_loopback    },
		{ "CS_LOOPBACK", spi_cs_loopback },
	};

	for (unsigned int i = 0U; i < sizeof(tests) / sizeof(tests[0]); i++) {
		TEST_BEGIN(SPI_MODULE, tests[i].name);
		tests[i].run();
	}

	return 0;
}
