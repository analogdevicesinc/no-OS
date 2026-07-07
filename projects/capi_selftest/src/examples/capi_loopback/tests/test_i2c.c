/***************************************************************************//**
 * @file test_i2c.c
 * @brief CAPI I2C single-board loopback tests.
 *
 * Exercises the CAPI I2C API using two buses on the same board: I2C1 as
 * initiator and I2C2 as target. Both sides go through CAPI — no
 * platform-specific fixture is needed for the data path. Every case moves real
 * bytes over the wire and checks them at the other end; the target side runs
 * async so the interrupt path is exercised implicitly (its completion callback
 * is what the initiator's blocking call is proven against).
 *
 * Hardware assumption: two I2C buses with SCL and SDA cross-wired.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "test_framework.h"
#include "test_i2c.h"

#ifndef I2C_OPS

int test_i2c(void)
{
	static const struct test_case stub[] = {
		{ "NOT_CONFIGURED", NULL, false },
	};

	return test_framework_run_cases("I2C", stub, 1U);
}

#else /* I2C_OPS defined — full implementation follows */

#include "capi_i2c.h"
#include "common_data.h"
#include "capi_time.h"

#define I2C_MODULE		"I2C"
#define I2C_ASYNC_TIMEOUT_US	1000000U
#define I2C_ASYNC_STEP_US	1000U

/*
 * I2C_TARGET_ALT_ADDR, I2C_SPEED_ALT and I2C_DUTY_CYCLE come from common_data.h
 * (defaulted there, overridable per platform).
 */

static volatile unsigned int target_callback_count;
static volatile enum capi_i2c_async_event target_callback_event;

static void target_test_callback(enum capi_i2c_async_event event, void *arg,
				 int event_extra)
{
	(void)arg;
	(void)event_extra;

	target_callback_event = event;
	target_callback_count++;
}

static volatile unsigned int master_callback_count;
static volatile enum capi_i2c_async_event master_callback_event;

static void master_test_callback(enum capi_i2c_async_event event, void *arg,
				 int event_extra)
{
	(void)arg;
	(void)event_extra;

	master_callback_event = event;
	master_callback_count++;
}

#ifdef I2C_TARGET_OPS
/**
 * @brief Drive one initiator TX -> target RX transfer and verify the bytes.
 *
 * Arms the target with an async receive, transmits @p len bytes from @p tx on
 * the initiator, waits for the target's completion callback, and checks the
 * received buffer matches. Shared by the basic and data-pattern cases.
 *
 * @param dev - Initiator device (addresses the target).
 * @param tgt_dev - Target device.
 * @param tgt_handle - Target controller handle (callback source).
 * @param tx - Bytes to send.
 * @param rx - Scratch buffer the target receives into (>= len).
 * @param len - Number of bytes.
 * @param addr - 7-bit address the initiator addresses.
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_xfer_check(struct capi_i2c_device *dev,
			  struct capi_i2c_device *tgt_dev,
			  struct capi_i2c_controller_handle *tgt_handle,
			  const uint8_t *tx, uint8_t *rx, uint32_t len,
			  uint16_t addr)
{
	int ret;

	(void)tgt_handle;

	memset(rx, 0, len);
	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;

	struct capi_i2c_transfer tgt_rx_xfer = {
		.buf = rx,
		.len = len,
	};
	ret = capi_i2c_receive_async(tgt_dev, &tgt_rx_xfer);
	TEST_ASSERT_EQ(ret, 0, "TARGET_LISTEN_RX");

	struct capi_i2c_transfer tx_xfer = {
		.buf = (uint8_t *)tx,
		.target_addr = addr,
		.len = len,
	};
	ret = capi_i2c_transmit(dev, &tx_xfer);
	TEST_ASSERT_EQ(ret, 0, "MASTER_TX");

	TEST_WAIT_UNTIL(target_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT(target_callback_count > 0U, "TARGET_RX_DONE");
	TEST_ASSERT_EQ(target_callback_event, CAPI_I2C_XFR_DONE,
		       "TARGET_RX_EVENT");
	TEST_ASSERT_EQ(memcmp(rx, tx, len), 0, "TX_RX_MATCH");

	return 0;
}

/**
 * @brief Drive one target TX -> initiator RX transfer and verify the bytes.
 *
 * The mirror of i2c_xfer_check: arms the target with an async transmit, then
 * the initiator does a blocking receive addressed to the target. The target's
 * completion callback is what proves the bytes were clocked out, so this drives
 * the target-transmit direction (the weaker-tested one) over the same buses.
 *
 * @param dev - Initiator device (addresses the target).
 * @param tgt_dev - Target device.
 * @param tx - Bytes the target sends.
 * @param rx - Scratch buffer the initiator receives into (>= len).
 * @param len - Number of bytes.
 * @param addr - 7-bit address the initiator addresses.
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_xfer_check_target_tx(struct capi_i2c_device *dev,
				    struct capi_i2c_device *tgt_dev,
				    const uint8_t *tx, uint8_t *rx,
				    uint32_t len, uint16_t addr)
{
	int ret;

	memset(rx, 0, len);
	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;

	struct capi_i2c_transfer tgt_tx_xfer = {
		.buf = (uint8_t *)tx,
		.len = len,
	};
	ret = capi_i2c_transmit_async(tgt_dev, &tgt_tx_xfer);
	TEST_ASSERT_EQ(ret, 0, "TARGET_LISTEN_TX");

	struct capi_i2c_transfer rx_xfer = {
		.buf = rx,
		.target_addr = addr,
		.len = len,
	};
	ret = capi_i2c_receive(dev, &rx_xfer);
	TEST_ASSERT_EQ(ret, 0, "MASTER_RX");

	TEST_WAIT_UNTIL(target_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT(target_callback_count > 0U, "TARGET_TX_DONE");
	TEST_ASSERT_EQ(target_callback_event, CAPI_I2C_XFR_DONE,
		       "TARGET_TX_EVENT");
	TEST_ASSERT_EQ(memcmp(rx, tx, len), 0, "RX_TX_MATCH");

	return 0;
}

/*
 * Every case below brings both buses fully live: it inits the initiator and
 * target controllers, runs I2C_PLATFORM_INIT, programs the target's address-
 * match register (I2C_PLATFORM_SET_TARGET) and arms an async receive. A bare
 * early return on a failed assertion would leave the target still ACKing its
 * address and the initiator holding the bus, so the NEXT case sees a busy bus
 * and fails for the wrong reason. CLEANUP routes every exit through the same
 * teardown the success path uses: drop the target match, deinit both handles,
 * and I2C_PLATFORM_DEINIT(). It is guarded on each handle != NULL and the
 * platform hooks are idempotent no-ops when nothing was set up, so it is safe
 * on any exit path (including one where the first init failed).
 *
 * Only the mid-body asserts (init, setup, transfers) use the _OR_CLEANUP form:
 * those are the ones that can bail with hardware still live. The success-path
 * TARGET_DEINIT/MASTER_DEINIT asserts stay plain -- they run after everything
 * passed and ARE the teardown, so there is nothing left to rescue.
 */
#define CLEANUP \
	do { \
		if (tgt_handle != NULL) { \
			I2C_PLATFORM_SET_TARGET(NULL); \
			(void)capi_i2c_deinit(tgt_handle); \
		} \
		if (init_handle != NULL) \
			(void)capi_i2c_deinit(init_handle); \
		I2C_PLATFORM_DEINIT(); \
	} while (0)

/**
 * @brief Synchronous initiator TX -> target RX and target TX -> initiator RX.
 *
 * Initialises both buses through CAPI. The target side uses async transfers
 * (interrupt-driven) so that the blocking initiator call and the target
 * listen can overlap on the single thread — the completion interrupt is what
 * the matching data is proven against, so this exercises the IRQ path too.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_basic(void)
{
	struct capi_i2c_controller_handle *init_handle = NULL;
	struct capi_i2c_controller_handle *tgt_handle = NULL;
	struct capi_i2c_device dev = i2c_dev;
	struct capi_i2c_device tgt_dev = i2c_target_dev;
	uint8_t tx_data[] = { 0x55, 0xAA, 0x33, 0xCC };
	uint8_t target_rx[sizeof(tx_data)];
	uint8_t target_tx[] = { 0xDE, 0xAD, 0xBE, 0xEF };
	uint8_t master_rx[sizeof(target_tx)];
	int ret;

	TEST_SECTION("BASIC");

	ret = capi_i2c_init(&init_handle, &i2c_master_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);

	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_REGISTER_CB");

	/* Initiator TX -> target RX. */
	ret = i2c_xfer_check(&dev, &tgt_dev, tgt_handle, tx_data, target_rx,
			     sizeof(tx_data), I2C_TARGET_ADDR);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_TX");

	/* Target TX -> initiator RX. */
	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;
	memset(master_rx, 0, sizeof(master_rx));

	struct capi_i2c_transfer tgt_tx_xfer = {
		.buf = target_tx,
		.len = sizeof(target_tx),
	};
	ret = capi_i2c_transmit_async(&tgt_dev, &tgt_tx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_LISTEN_TX");

	struct capi_i2c_transfer rx_xfer = {
		.buf = master_rx,
		.target_addr = I2C_TARGET_ADDR,
		.len = sizeof(master_rx),
	};
	ret = capi_i2c_receive(&dev, &rx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_RX");

	TEST_WAIT_UNTIL(target_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(target_callback_count > 0U, "TARGET_TX_DONE");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(master_rx, target_tx, sizeof(target_tx)), 0,
				  "RX_TX_MATCH");

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}

/**
 * @brief Data integrity across bit patterns and transfer sizes.
 *
 * Sends a set of patterns (all-ones, all-zeros, alternating, walking-one) at
 * several sizes from the initiator to the target and verifies each arrives
 * intact. Catches stuck data lines and length/off-by-one bugs a single fixed
 * payload would miss.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_data(void)
{
	struct capi_i2c_controller_handle *init_handle = NULL;
	struct capi_i2c_controller_handle *tgt_handle = NULL;
	struct capi_i2c_device dev = i2c_dev;
	struct capi_i2c_device tgt_dev = i2c_target_dev;
	static const uint8_t fills[] = { 0xFFU, 0x00U, 0xAAU, 0x55U };
	static const uint32_t sizes[] = { 1U, 16U, 32U };
	uint8_t tx[32];
	uint8_t rx[32];
	static const uint8_t bounds_tx[8] = { 0x11, 0x22, 0x33, 0x44,
					      0x55, 0x66, 0x77, 0x88
					    };
	int ret;

	TEST_SECTION("DATA");

	ret = capi_i2c_init(&init_handle, &i2c_master_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);
	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_REGISTER_CB");

	/* Constant-fill patterns at each size. */
	for (uint32_t s = 0U; s < sizeof(sizes) / sizeof(sizes[0]); s++) {
		for (uint32_t p = 0U; p < sizeof(fills); p++) {
			memset(tx, fills[p], sizes[s]);
			ret = i2c_xfer_check(&dev, &tgt_dev, tgt_handle, tx, rx,
					     sizes[s], I2C_TARGET_ADDR);
			TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_FILL");
		}
	}

	/* Walking-one over a full 32-byte payload. */
	for (uint32_t bit = 0U; bit < 8U; bit++) {
		memset(tx, (uint8_t)(1U << bit), sizeof(tx));
		ret = i2c_xfer_check(&dev, &tgt_dev, tgt_handle, tx, rx,
				     sizeof(tx), I2C_TARGET_ADDR);
		TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_WALK");
	}

	/*
	 * Overlong-frame bounds check: the target arms a receive with a buffer
	 * SHORTER than what the initiator actually sends. A compliant target
	 * either stops at its own buffer length or reports an error -- it must
	 * never write past the end of the caller's buffer. A canary placed
	 * immediately after the target's buffer catches a backend (e.g. one
	 * whose vendor slave-receive path ignores the requested length and
	 * writes until the initiator issues STOP) that overruns it.
	 */
	TEST_SECTION("DATA.BOUNDS");
	{
		static const uint32_t bounds_target_len = 4U;
		static const uint8_t canary_pattern = 0xE5U;
		/*
		 * Sized to hold the whole overlong frame plus slack, so a
		 * backend that ignores the requested length and writes every
		 * byte the initiator sends still lands inside this buffer --
		 * the canary check below is what catches the overrun, not a
		 * crash from writing past the array.
		 */
		uint8_t bounds_buf[sizeof(bounds_tx) + 4U];

		memset(bounds_buf, canary_pattern, sizeof(bounds_buf));
		target_callback_count = 0U;
		target_callback_event = CAPI_I2C_NONE;

		struct capi_i2c_transfer tgt_rx_xfer = {
			.buf = bounds_buf,
			.len = bounds_target_len,
		};
		ret = capi_i2c_receive_async(&tgt_dev, &tgt_rx_xfer);
		TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "BOUNDS_TARGET_LISTEN");

		struct capi_i2c_transfer tx_xfer = {
			.buf = (uint8_t *)bounds_tx,
			.target_addr = I2C_TARGET_ADDR,
			.len = sizeof(bounds_tx),
		};
		ret = capi_i2c_transmit(&dev, &tx_xfer);
		TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "BOUNDS_MASTER_TX");

		TEST_WAIT_UNTIL(target_callback_count > 0U,
				I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
		TEST_ASSERT_OR_CLEANUP(target_callback_count > 0U,
				       "BOUNDS_TARGET_DONE");

		/*
		 * Whatever the target reported, the bytes beyond its requested
		 * length must be untouched. A vendor path that keeps writing
		 * past ByteCount stamps the initiator's tail bytes over the
		 * canary here.
		 */
		for (uint32_t i = bounds_target_len; i < sizeof(bounds_buf); i++)
			TEST_ASSERT_EQ_OR_CLEANUP(bounds_buf[i], canary_pattern,
						  "BOUNDS_NO_OVERRUN");
	}

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}

/**
 * @brief Runtime target readdressing, proven over the bus.
 *
 * register_target() rewrites the controller's own address-match register. This
 * moves the target to a second address at runtime, then proves a transfer
 * addressed to the new value is received while the original address is no
 * longer matched — externally observable evidence the register took effect,
 * not just a return code.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_readdress(void)
{
	struct capi_i2c_controller_handle *init_handle = NULL;
	struct capi_i2c_controller_handle *tgt_handle = NULL;
	struct capi_i2c_device dev = i2c_dev;
	struct capi_i2c_device tgt_dev = i2c_target_dev;
	uint8_t tx_data[] = { 0x0F, 0xF0, 0x12, 0x34 };
	uint8_t target_rx[sizeof(tx_data)];
	int ret;

	TEST_SECTION("READDRESS");

	ret = capi_i2c_init(&init_handle, &i2c_master_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);
	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_REGISTER_CB");

	/* Move the target to the alternate address. */
	ret = capi_i2c_register_target(tgt_handle, I2C_TARGET_ALT_ADDR);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "REGISTER_TARGET");

	/* A transfer to the new address must now be received and match. */
	ret = i2c_xfer_check(&dev, &tgt_dev, tgt_handle, tx_data, target_rx,
			     sizeof(tx_data), I2C_TARGET_ALT_ADDR);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_ALT_ADDR");

	/*
	 * Negative half of the readdress proof: the OLD address must no longer
	 * be answered. Arm the target, transmit to I2C_TARGET_ADDR, and require
	 * the target callback to stay silent. A transmit that NACKs (no target
	 * at that address) may itself return an error -- that is also acceptable
	 * evidence the old address is gone, so the transmit's return is not
	 * asserted; only the absence of a completed target receive is. Without
	 * this, a driver that ADDED the new address without dropping the old one
	 * would still pass XFER_ALT_ADDR above.
	 */
	memset(target_rx, 0, sizeof(target_rx));
	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;

	struct capi_i2c_transfer old_rx_xfer = {
		.buf = target_rx,
		.len = sizeof(tx_data),
	};
	ret = capi_i2c_receive_async(&tgt_dev, &old_rx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "OLD_ADDR_LISTEN");

	struct capi_i2c_transfer old_tx_xfer = {
		.buf = tx_data,
		.target_addr = I2C_TARGET_ADDR,
		.len = sizeof(tx_data),
	};
	(void)capi_i2c_transmit(&dev, &old_tx_xfer);

	TEST_WAIT_UNTIL(target_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT_EQ_OR_CLEANUP(target_callback_count, 0U, "OLD_ADDR_SILENT");

	/*
	 * The target's async receive is still armed (nothing addressed it), and
	 * a failed initiator transmit may have left the bus held. recover_bus
	 * clears both so the teardown and later cases start clean; its own return
	 * is best-effort (see the driver note) so it is not asserted.
	 *
	 * Cancelling that armed receive invokes the target callback with a
	 * non-success status, which would otherwise make the count checked above
	 * look like a late delivery to the old address. The OLD_ADDR_SILENT
	 * assertion has already run, so reset the counters here and let the
	 * cancellation land against a clean slate.
	 */
	(void)capi_i2c_recover_bus(tgt_handle);
	(void)capi_i2c_recover_bus(init_handle);

	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;

	/*
	 * With the in-flight receive cancelled, the role change must now succeed.
	 * This is the real assertion of the case's cleanup contract: a target that
	 * cannot be unregistered after a recovery would leave every later case
	 * failing -EBUSY on a controller stuck in target mode.
	 */
	ret = capi_i2c_unregister_target(tgt_handle);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "UNREGISTER_TARGET");

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}

/**
 * @brief Sub-address concatenation on the wire.
 *
 * The initiator transmits with a sub_address plus a data payload. The driver
 * must put [sub_address..., buf...] on the bus as one write. The target, which
 * just receives raw bytes, checks the concatenated stream arrives intact —
 * externally observable proof of the sub-address build path, with no register
 * device needed.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_subaddr(void)
{
	struct capi_i2c_controller_handle *init_handle = NULL;
	struct capi_i2c_controller_handle *tgt_handle = NULL;
	struct capi_i2c_device dev = i2c_dev;
	struct capi_i2c_device tgt_dev = i2c_target_dev;
	uint8_t sub_addr[] = { 0x1A, 0x2B };
	uint8_t payload[] = { 0xDE, 0xAD, 0xBE, 0xEF };
	uint8_t expected[sizeof(sub_addr) + sizeof(payload)];
	uint8_t target_rx[sizeof(expected)];
	int ret;

	TEST_SECTION("SUBADDR");

	ret = capi_i2c_init(&init_handle, &i2c_master_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);
	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_REGISTER_CB");

	memcpy(expected, sub_addr, sizeof(sub_addr));
	memcpy(expected + sizeof(sub_addr), payload, sizeof(payload));

	memset(target_rx, 0, sizeof(target_rx));
	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;

	struct capi_i2c_transfer tgt_rx_xfer = {
		.buf = target_rx,
		.len = sizeof(target_rx),
	};
	ret = capi_i2c_receive_async(&tgt_dev, &tgt_rx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_LISTEN_RX");

	struct capi_i2c_transfer tx_xfer = {
		.sub_address = sub_addr,
		.sub_address_len = sizeof(sub_addr),
		.buf = payload,
		.target_addr = I2C_TARGET_ADDR,
		.len = sizeof(payload),
	};
	ret = capi_i2c_transmit(&dev, &tx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_TX");

	TEST_WAIT_UNTIL(target_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(target_callback_count > 0U, "TARGET_RX_DONE");
	TEST_ASSERT_EQ_OR_CLEANUP(target_callback_event, CAPI_I2C_XFR_DONE,
				  "TARGET_RX_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(target_rx, expected, sizeof(expected)), 0,
				  "SUBADDR_MATCH");

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}

/**
 * @brief Target-transmit direction across transfer sizes.
 *
 * i2c_data covers initiator TX -> target RX; this drives the reverse direction,
 * where the interesting variable is length rather than content. The target send
 * path fills a hardware FIFO from its completion interrupt, so the sizes that
 * matter are below, exactly at, and above the FIFO depth -- a transfer that
 * fits in one load exercises none of the refill logic that a longer one needs.
 * Two patterns are enough to catch a stuck line; i2c_data already sweeps the
 * rest over the same wires.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_target_tx(void)
{
	struct capi_i2c_controller_handle *init_handle = NULL;
	struct capi_i2c_controller_handle *tgt_handle = NULL;
	struct capi_i2c_device dev = i2c_dev;
	struct capi_i2c_device tgt_dev = i2c_target_dev;
	/*
	 * A narrower matrix than i2c_data's: that case already proves every bit
	 * pattern survives the bus, so repeating the full sweep here would only
	 * re-test the wire. What is specific to the target-transmit path is the
	 * FIFO handling, so the sizes are what matter -- one byte, exactly the
	 * FIFO depth, and past it (the refill boundary).
	 */
	static const uint8_t fills[] = { 0xAAU, 0x55U };
	static const uint32_t sizes[] = { 1U, 16U, 32U };
	uint8_t tx[32];
	uint8_t rx[32];
	int ret;

	TEST_SECTION("TARGET_TX");

	ret = capi_i2c_init(&init_handle, &i2c_master_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);
	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_REGISTER_CB");

	for (uint32_t s = 0U; s < sizeof(sizes) / sizeof(sizes[0]); s++) {
		for (uint32_t p = 0U; p < sizeof(fills); p++) {
			memset(tx, fills[p], sizes[s]);
			ret = i2c_xfer_check_target_tx(&dev, &tgt_dev, tx, rx,
						       sizes[s], I2C_TARGET_ADDR);
			TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "XFER_FILL");
		}
	}

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}

/**
 * @brief Initiator-side async transfers, completed on the master's callback.
 *
 * Every other case drives the master synchronously (polled) and waits on the
 * target's completion callback. That leaves the master's OWN async path -- its
 * transmit_async/receive_async and the interrupt-driven completion behind them
 * -- with no coverage at all. Here the initiator runs async in both directions
 * and the test waits on the MASTER's callback, so the master ISR completion
 * path is what each assertion is proven against.
 *
 * The target still listens async (its RX must be armed for the master's TX to
 * land, and vice versa); both sides being async is fine on the single thread
 * because neither call blocks -- completion arrives via interrupt on each side
 * independently. This case needs an IRQ-backed master AND target, so it is
 * gated on both flags.
 *
 * Scope: this covers direct master async TX and RX. It deliberately does NOT
 * exercise the master sub-address register-read (the RX_SUBADDR state machine),
 * which needs a peer that re-arms mid-frame -- not expressible against this
 * one-shot-armed loopback target. That path belongs in a driver unit test.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int i2c_master_async(void)
{
	struct capi_i2c_controller_handle *init_handle = NULL;
	struct capi_i2c_controller_handle *tgt_handle = NULL;
	struct capi_i2c_device dev = i2c_dev;
	struct capi_i2c_device tgt_dev = i2c_target_dev;
	uint8_t tx_data[] = { 0x11, 0x22, 0x33, 0x44 };
	uint8_t target_rx[sizeof(tx_data)];
	uint8_t target_tx[] = { 0xA5, 0x5A, 0xC3, 0x3C };
	uint8_t master_rx[sizeof(target_tx)];
	int ret;

	TEST_SECTION("MASTER_ASYNC");

	ret = capi_i2c_init(&init_handle, &i2c_master_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);
	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_REGISTER_CB");
	ret = capi_i2c_register_callback(init_handle, master_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_REGISTER_CB");

	/* Master async TX -> target async RX; wait on the MASTER callback. */
	memset(target_rx, 0, sizeof(target_rx));
	target_callback_count = 0U;
	master_callback_count = 0U;
	master_callback_event = CAPI_I2C_NONE;

	struct capi_i2c_transfer tgt_rx_xfer = {
		.buf = target_rx,
		.len = sizeof(target_rx),
	};
	ret = capi_i2c_receive_async(&tgt_dev, &tgt_rx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_LISTEN_RX");

	struct capi_i2c_transfer tx_xfer = {
		.buf = tx_data,
		.target_addr = I2C_TARGET_ADDR,
		.len = sizeof(tx_data),
	};
	ret = capi_i2c_transmit_async(&dev, &tx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_TX_ASYNC");

	TEST_WAIT_UNTIL(master_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(master_callback_count > 0U, "MASTER_TX_DONE");
	TEST_ASSERT_EQ_OR_CLEANUP(master_callback_event, CAPI_I2C_XFR_DONE,
				  "MASTER_TX_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(target_rx, tx_data, sizeof(tx_data)), 0,
				  "MASTER_TX_MATCH");

	/* Target async TX -> master async RX; wait on the MASTER callback. */
	memset(master_rx, 0, sizeof(master_rx));
	target_callback_count = 0U;
	master_callback_count = 0U;
	master_callback_event = CAPI_I2C_NONE;

	struct capi_i2c_transfer tgt_tx_xfer = {
		.buf = target_tx,
		.len = sizeof(target_tx),
	};
	ret = capi_i2c_transmit_async(&tgt_dev, &tgt_tx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_LISTEN_TX");

	struct capi_i2c_transfer rx_xfer = {
		.buf = master_rx,
		.target_addr = I2C_TARGET_ADDR,
		.len = sizeof(master_rx),
	};
	ret = capi_i2c_receive_async(&dev, &rx_xfer);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_RX_ASYNC");

	TEST_WAIT_UNTIL(master_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT_OR_CLEANUP(master_callback_count > 0U, "MASTER_RX_DONE");
	TEST_ASSERT_EQ_OR_CLEANUP(master_callback_event, CAPI_I2C_XFR_DONE,
				  "MASTER_RX_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(master_rx, target_tx, sizeof(target_tx)), 0,
				  "MASTER_RX_MATCH");

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}

/*
 * Prove capi_i2c_configure_bus_speed() actually changes the SCL rate, portably
 * across platforms (Maxim / STM32 / Xilinx / ...).
 *
 * The only invariant that holds on every platform is on-wire: a faster SCL
 * clock clocks the SAME payload across the bus in less wall-clock time. So the
 * test times one large transfer at STANDARD (100 kHz) and one at FAST (400 kHz)
 * through the identical path and asserts the FAST time is meaningfully shorter
 * (t_fast < t_slow * 9/10, a 10% margin -- a real speed change is ~3-4x, well
 * clear of it). Register readback is deliberately avoided: it is per-driver and
 * proves only that a divider was written, not that the wire sped up.
 *
 * Speed is a property of the clock-GENERATING side, so ONLY the initiator is
 * reprogrammed; the target never drives SCL. If the initiator cannot change
 * speed at all (configure returns -ENOTSUP -- e.g. a PL AXI IIC with no
 * input_clock_hz), there is nothing to prove and the whole test SKIPs. If both
 * probes carry data but their times fall below I2C_SPEED_MIN_RESOLVABLE_US, the
 * software floor (async poll granularity + ISR latency + timer resolution)
 * swamps the wire time and the direction cannot be resolved -- the test SKIPs
 * rather than asserting on noise.
 *
 * Duty cycle is gated on I2C_DUTY_CYCLE_SUPPORTED (see common_data.h): it is a
 * master-only clock property and no Xilinx master honors it, so the sub-block
 * compiles out by default.
 *
 * Every outcome is PASS / FAIL / SKIP.
 */

/*
 * Ask one controller for a speed and log the raw return. label distinguishes
 * the MASTER (PS) and TARGET (PL) sides in the transcript. Returns the raw ret
 * so the caller can assert OK_OR_NOTSUP on it and decide whether to re-verify a
 * transfer -- the re-verify lives in the caller because it must route failures
 * through that function's CLEANUP (which owns the handles).
 */
static int i2c_try_speed(struct capi_i2c_controller_handle *ctrl,
			 const char *label, enum capi_i2c_speed speed,
			 uint8_t duty_cycle)
{
	(void)label;

	return capi_i2c_configure_bus_speed(ctrl, speed, duty_cycle);
}

/*
 * Drive ONE real PL(initiator)->PS(target) data transfer at whatever bus speed
 * the initiator is currently programmed to, and RECORD the outcome instead of
 * asserting on it. This is the on-wire half of the bus-speed proof: unlike the
 * THIGH/TLOW register readback (which only proves the divider was rewritten),
 * this actually clocks bytes across SCL/SDA at the new rate and tells us whether
 * the two controllers genuinely interoperate there.
 *
 * Why it must not assert: the whole point is to measure the outcome, then let
 * the caller decide PASS/FAIL/SKIP. A non-zero return can mean several distinct
 * things -- a target whose sampling divider was not resized to this speed (the
 * common one; the caller reconfigures the target each leg to rule it out), a
 * FIFO/HOLD servicing race in the target at the faster rate, or genuine wiring
 * trouble -- and the PL backend collapses all of them to -EIO. This function
 * only records the raw result; disambiguating the cause is the caller's job.
 *
 * On any non-zero return the target's armed async receive is left dangling and
 * the controller may hold the bus, so both sides are recovered before returning.
 * recover_bus resets the controller and (for the PS target) re-arms its slave
 * address, so the next probe / the final STANDARD transfer starts clean.
 *
 * Returns the transfer's raw ret (0 = data crossed the wire at this speed;
 * negative = it did not). Never fails the test on its own.
 */
static int i2c_probe_xfer_at_speed(struct capi_i2c_device *dev,
				   struct capi_i2c_device *tgt_dev,
				   struct capi_i2c_controller_handle *init_handle,
				   struct capi_i2c_controller_handle *tgt_handle,
				   const char *label, const char *time_label,
				   const uint8_t *tx, uint8_t *rx, uint32_t len,
				   uint64_t *out_us, bool *out_data_ok)
{
	uint64_t start_us = 0U, end_us = 0U;
	bool timed = false;
	int ret;

	if (out_us != NULL)
		*out_us = 0U;
	if (out_data_ok != NULL)
		*out_data_ok = false;

	(void)label;

	memset(rx, 0, len);
	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;

	struct capi_i2c_transfer tgt_rx_xfer = {
		.buf = rx,
		.len = len,
	};
	ret = capi_i2c_receive_async(tgt_dev, &tgt_rx_xfer);
	if (ret != 0) {
		/* Could not even arm the target -- nothing crossed the wire. */
		(void)capi_i2c_recover_bus(tgt_handle);
		(void)capi_i2c_recover_bus(init_handle);
		return ret;
	}

	struct capi_i2c_transfer tx_xfer = {
		.buf = (uint8_t *)tx,
		.target_addr = I2C_TARGET_ADDR,
		.len = len,
	};
	timed = (time_label != NULL && capi_uptime(&start_us) == 0);
	ret = capi_i2c_transmit(dev, &tx_xfer);

	if (ret == 0) {
		/* Initiator says it clocked out; confirm the target actually got it. */
		TEST_WAIT_UNTIL(target_callback_count > 0U,
				I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
		if (timed && capi_uptime(&end_us) == 0) {
			if (out_us != NULL)
				*out_us = end_us - start_us;
		}

		if (target_callback_count > 0U &&
		    target_callback_event == CAPI_I2C_XFR_DONE &&
		    memcmp(rx, tx, len) == 0) {
			if (out_data_ok != NULL)
				*out_data_ok = true;
		} else {
			/* TX returned 0 but data did not land: treat as an on-wire fail. */
			ret = -EIO;
		}
	}

	/*
	 * Whether TX failed outright or the data check failed, tear the transient
	 * state down: recover the target (cancels its armed receive, re-arms its
	 * slave address) and the initiator (releases any held bus) so the next
	 * probe and the final STANDARD transfer are unaffected.
	 */
	(void)capi_i2c_recover_bus(tgt_handle);
	(void)capi_i2c_recover_bus(init_handle);

	return ret;
}

static int i2c_bus_speed(void)
{
	struct capi_i2c_controller_handle *init_handle = NULL;
	struct capi_i2c_controller_handle *tgt_handle = NULL;
	struct capi_i2c_device dev = i2c_dev;
	struct capi_i2c_device tgt_dev = i2c_target_dev;
	/*
	 * A large payload is deliberate: at four bytes the on-wire time (tens of
	 * microseconds) is buried under the per-transfer software floor (the
	 * async-completion poll granularity, milliseconds), so timing cannot
	 * resolve the speed change. At ~1 KB the bus time dominates at STANDARD/
	 * FAST/FAST_PLUS, so xfer_time drops monotonically as the SCL clock rises
	 * -- the on-wire proof that the speed actually changed.
	 */
	static uint8_t tx_data[1024];
	static uint8_t target_rx[sizeof(tx_data)];
	int ret;

	for (uint32_t i = 0U; i < sizeof(tx_data); i++)
		tx_data[i] = (uint8_t)(i * 7U + 0x5AU);

	TEST_SECTION("BUS_SPEED");

	ret = capi_i2c_init(&init_handle, &i2c_master_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);
	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "TARGET_REGISTER_CB");

	uint64_t t_slow_us = 0U, t_fast_us = 0U;
	bool slow_ok = false, fast_ok = false;

	/* Baseline transfer at whatever speed init left the bus in. */
	ret = i2c_xfer_check(&dev, &tgt_dev, tgt_handle, tx_data, target_rx,
			     sizeof(tx_data), I2C_TARGET_ADDR);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "BASELINE_XFER");

	/*
	 * ---- Slow leg: STANDARD (100 kHz) ----
	 * If the initiator cannot even be set to STANDARD, speed control is not
	 * supported at all on this platform: there is nothing to prove, so SKIP
	 * the whole test (releasing the hardware first).
	 */
	TEST_SECTION("BUS_SPEED.SLOW");
	ret = i2c_try_speed(init_handle, "STANDARD_100k",
			    CAPI_I2C_SPEED_STANDARD, 0);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_FEATURE_DISABLED,
					 "initiator has no runtime speed control");
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SLOW_CONFIG");

	/*
	 * Match the TARGET's sampling divider to the initiator's clock. On some
	 * controllers (PS XIicPs) a target never drives SCL, but its SetSClk-derived
	 * clock-enable is what the FSM uses to synchronise and sample SDA; leaving it
	 * at the init-time 100 kHz while the initiator moves to FAST is a real config
	 * mismatch, not a wire limit, so it must be re-sized every leg.
	 *
	 * Accept -ENOTSUP here: a target whose slave path samples off a fixed
	 * oversample clock (PL AXI IIC oversamples against the 100 MHz fabric clock;
	 * STM32 derives sampling from a fixed TIMINGR) has no master-speed-derived
	 * window to size, and correctly reports target speed control as unsupported.
	 * That is a legitimate "nothing to do", not a failure -- the on-wire probe
	 * that follows still proves data crosses at this speed.
	 */
	ret = i2c_try_speed(tgt_handle, "TARGET_STANDARD_100k",
			    CAPI_I2C_SPEED_STANDARD, 0);
	TEST_ASSERT_OK_OR_NOTSUP_OR_CLEANUP(ret, "SLOW_TARGET_CONFIG");

	ret = i2c_probe_xfer_at_speed(&dev, &tgt_dev, init_handle, tgt_handle,
				      "PROBE_XFER_100k", "xfer_time_100k_us",
				      tx_data, target_rx, sizeof(tx_data),
				      &t_slow_us, &slow_ok);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "SLOW_XFER");
	TEST_ASSERT_OR_CLEANUP(slow_ok, "SLOW_DATA");

	/*
	 * ---- Fast leg: FAST (400 kHz) ----
	 * FAST must be accepted once STANDARD was (both are the two universally
	 * supported rates); a reject here is a real contract failure, not a SKIP.
	 */
	TEST_SECTION("BUS_SPEED.FAST");
	ret = i2c_try_speed(init_handle, "FAST_400k", CAPI_I2C_SPEED_FAST, 0);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "FAST_CONFIG");

	/* Move the target's sampling divider to FAST as well (see SLOW leg). */
	ret = i2c_try_speed(tgt_handle, "TARGET_FAST_400k",
			    CAPI_I2C_SPEED_FAST, 0);
	TEST_ASSERT_OK_OR_NOTSUP_OR_CLEANUP(ret, "FAST_TARGET_CONFIG");

	ret = i2c_probe_xfer_at_speed(&dev, &tgt_dev, init_handle, tgt_handle,
				      "PROBE_XFER_400k", "xfer_time_400k_us",
				      tx_data, target_rx, sizeof(tx_data),
				      &t_fast_us, &fast_ok);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "FAST_XFER");
	TEST_ASSERT_OR_CLEANUP(fast_ok, "FAST_DATA");

#if I2C_DUTY_CYCLE_SUPPORTED
	/*
	 * ---- Duty leg (master-only, gated) ----
	 * Only reached on a platform whose master honors a non-50% SCL duty. The
	 * request must be accepted and the bus must still carry data at the new
	 * duty. Compiled out entirely on Xilinx (no master honors duty).
	 */
	TEST_SECTION("BUS_SPEED.DUTY");
	ret = i2c_try_speed(init_handle, "FAST_DUTY", CAPI_I2C_SPEED_FAST,
			    I2C_DUTY_CYCLE);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DUTY_CONFIG");

	ret = i2c_probe_xfer_at_speed(&dev, &tgt_dev, init_handle, tgt_handle,
				      "PROBE_XFER_DUTY", "xfer_time_duty_us",
				      tx_data, target_rx, sizeof(tx_data),
				      NULL, &fast_ok);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "DUTY_XFER");
	TEST_ASSERT_OR_CLEANUP(fast_ok, "DUTY_DATA");
#endif /* I2C_DUTY_CYCLE_SUPPORTED */

	/* Restore the initiator to STANDARD before the final transfer. */
	TEST_SECTION("BUS_SPEED.DIRECTION");
	ret = i2c_try_speed(init_handle, "RESTORE_STANDARD",
			    CAPI_I2C_SPEED_STANDARD, 0);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "RESTORE_CONFIG");

	/* Bring the target divider back to STANDARD to match (see SLOW leg). */
	ret = i2c_try_speed(tgt_handle, "TARGET_RESTORE_STANDARD",
			    CAPI_I2C_SPEED_STANDARD, 0);
	TEST_ASSERT_OK_OR_NOTSUP_OR_CLEANUP(ret, "RESTORE_TARGET_CONFIG");

	/*
	 * The portable proof: FAST clocked the same payload across the wire in
	 * less time than STANDARD. Guard against the software floor -- if either
	 * timed leg is below the resolution floor, the wire time is buried under
	 * poll/ISR/timer overhead and the comparison is noise, so SKIP the
	 * direction assert (the data-carried asserts above still passed).
	 */
	if (t_slow_us < I2C_SPEED_MIN_RESOLVABLE_US ||
	    t_fast_us < I2C_SPEED_MIN_RESOLVABLE_US) {
		TEST_SKIP_CAT_OR_CLEANUP(SKIP_BOARD_STATE,
					 "transfer time below I2C_SPEED_MIN_RESOLVABLE_US");
	}
	/* t_fast must be at least 10% shorter than t_slow. */
	TEST_ASSERT_LT_OR_CLEANUP(t_fast_us, (t_slow_us * 9U) / 10U,
				  "SPEED_DIRECTION");

	/* Final data-integrity transfer, initiator back at STANDARD. */
	ret = i2c_xfer_check(&dev, &tgt_dev, tgt_handle, tx_data, target_rx,
			     sizeof(tx_data), I2C_TARGET_ADDR);
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "FINAL_XFER");

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}

#undef CLEANUP

#endif /* I2C_TARGET_OPS */

/*
 * Every case needs the second (target) bus to observe traffic, so the whole
 * table is gated on I2C_TARGET_OPS. A build with only an initiator mapped runs
 * the single stub entry.
 *
 * Gating is split by what each case is actually about:
 *   I2C_PAIR_TARGET_ASYNC - the plain transfer cases (BASIC..TARGET_TX) gate on
 *                  this ALONE: both roles mapped AND the target can arm an async
 *                  listen. The single-core topology forces it -- one CPU is both
 *                  master and target and the master call blocks. A blocking
 *                  target receive would have to run before that call, but then
 *                  the CPU parks in it and never reaches the master. So the
 *                  target must arm a non-blocking (async, IRQ-backed) listen,
 *                  THEN block in the master call, completion firing from the ISR.
 *                  A sync target is fine elsewhere (separate cores/devices); it
 *                  just cannot self-loopback on one core. A board with no target
 *                  IRQ has this evaluate to 0 and the table collapses to the stub.
 *   I2C_MASTER_ASYNC - MASTER_ASYNC also runs the INITIATOR async, so it ANDs
 *                  this (the initiator's IRQ) onto the pair flag above.
 */
static const struct test_case i2c_subtests[] = {
#ifdef I2C_TARGET_OPS
	{ "BASIC",        i2c_basic,        !I2C_PAIR_TARGET_ASYNC},
	{ "DATA",         i2c_data,         !I2C_PAIR_TARGET_ASYNC},
	{ "READDRESS",    i2c_readdress,    !I2C_PAIR_TARGET_ASYNC},
	{ "SUBADDR",      i2c_subaddr,      !I2C_PAIR_TARGET_ASYNC},
	{ "BUS_SPEED",    i2c_bus_speed,    !I2C_PAIR_TARGET_ASYNC},
	{ "TARGET_TX",    i2c_target_tx,    !I2C_PAIR_TARGET_ASYNC},
	{ "MASTER_ASYNC", i2c_master_async, !(I2C_PAIR_TARGET_ASYNC && I2C_MASTER_ASYNC) },
#else
	{ "BASIC",        NULL,             false },
#endif
};

int test_i2c(void)
{
	return test_framework_run_cases(I2C_MODULE, i2c_subtests,
					sizeof(i2c_subtests) / sizeof(i2c_subtests[0]));
}

#endif /* I2C_OPS */
