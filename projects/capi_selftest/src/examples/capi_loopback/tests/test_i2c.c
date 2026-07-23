/***************************************************************************//**
 * @file test_i2c.c
 * @brief CAPI I2C single-board loopback tests.
 *
 * Exercises the CAPI I2C API using two buses on the same board: I2C1 as
 * initiator and I2C2 as target. Both sides go through CAPI — no
 * platform-specific fixture is needed for the data path.
 *
 * Hardware assumption: two I2C buses with SCL and SDA cross-wired.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
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

#define I2C_MODULE		"I2C"
#define I2C_ASYNC_TIMEOUT_US	1000000U
#define I2C_ASYNC_STEP_US	1000U
#define I2C_REINIT_COUNT	5U

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

#ifdef I2C_TARGET_OPS
/**
 * @brief Synchronous initiator TX -> target RX and target TX -> initiator RX.
 *
 * Initialises both buses through CAPI. The target side uses async transfers
 * (interrupt-driven) so that the blocking initiator call and the target
 * listen can overlap on the single thread.
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
	TEST_ASSERT_EQ(ret, 0, "MASTER_INIT");
	dev.controller = init_handle;

	ret = I2C_PLATFORM_INIT();
	TEST_ASSERT_EQ(ret, 0, "PLATFORM_INIT");

	ret = capi_i2c_init(&tgt_handle, &i2c_target_config);
	TEST_ASSERT_EQ(ret, 0, "TARGET_INIT");
	tgt_dev.controller = tgt_handle;

	I2C_PLATFORM_SET_TARGET(tgt_handle);

	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;
	ret = capi_i2c_register_callback(tgt_handle, target_test_callback, NULL);
	TEST_ASSERT_EQ(ret, 0, "TARGET_REGISTER_CB");

	/* Initiator TX -> target RX */
	memset(target_rx, 0, sizeof(target_rx));
	struct capi_i2c_transfer tgt_rx_xfer = {
		.buf = target_rx,
		.len = sizeof(target_rx),
	};
	ret = capi_i2c_receive_async(&tgt_dev, &tgt_rx_xfer);
	TEST_ASSERT_EQ(ret, 0, "TARGET_LISTEN_RX");

	struct capi_i2c_transfer tx_xfer = {
		.buf = tx_data,
		.target_addr = I2C_TARGET_ADDR,
		.len = sizeof(tx_data),
	};
	ret = capi_i2c_transmit(&dev, &tx_xfer);
	TEST_ASSERT_EQ(ret, 0, "MASTER_TX");

	TEST_WAIT_UNTIL(target_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT(target_callback_count > 0U, "TARGET_RX_DONE");
	TEST_ASSERT_EQ(target_callback_event, CAPI_I2C_XFR_DONE,
		       "TARGET_RX_EVENT");
	TEST_ASSERT_EQ(memcmp(target_rx, tx_data, sizeof(tx_data)), 0,
		       "TX_RX_MATCH");

	/* Target TX -> initiator RX */
	target_callback_count = 0U;
	target_callback_event = CAPI_I2C_NONE;
	memset(master_rx, 0, sizeof(master_rx));

	struct capi_i2c_transfer tgt_tx_xfer = {
		.buf = target_tx,
		.len = sizeof(target_tx),
	};
	ret = capi_i2c_transmit_async(&tgt_dev, &tgt_tx_xfer);
	TEST_ASSERT_EQ(ret, 0, "TARGET_LISTEN_TX");

	struct capi_i2c_transfer rx_xfer = {
		.buf = master_rx,
		.target_addr = I2C_TARGET_ADDR,
		.len = sizeof(master_rx),
	};
	ret = capi_i2c_receive(&dev, &rx_xfer);
	TEST_ASSERT_EQ(ret, 0, "MASTER_RX");

	TEST_WAIT_UNTIL(target_callback_count > 0U,
			I2C_ASYNC_TIMEOUT_US, I2C_ASYNC_STEP_US);
	TEST_ASSERT(target_callback_count > 0U, "TARGET_TX_DONE");
	TEST_ASSERT_EQ(memcmp(master_rx, target_tx, sizeof(target_tx)), 0,
		       "RX_TX_MATCH");

	I2C_PLATFORM_SET_TARGET(NULL);
	TEST_ASSERT_EQ(capi_i2c_deinit(tgt_handle), 0, "TARGET_DEINIT");
	TEST_ASSERT_EQ(capi_i2c_deinit(init_handle), 0, "MASTER_DEINIT");
	I2C_PLATFORM_DEINIT();

	return 0;
}
#endif /* I2C_TARGET_OPS */

/**
 * @brief Exercise register_target / unregister_target CAPI API.
 *
 * These calls reconfigure the controller's own address. The test verifies
 * the API returns success — no bus traffic is generated.
 */
static int i2c_register_target(void)
{
	struct capi_i2c_controller_handle *i2c_handle = NULL;
	int ret;

	TEST_SECTION("REGISTER_TARGET");

	ret = capi_i2c_init(&i2c_handle, &i2c_master_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	ret = capi_i2c_register_target(i2c_handle, 0x50U);
	TEST_ASSERT_EQ(ret, 0, "REGISTER");

	ret = capi_i2c_unregister_target(i2c_handle);
	TEST_ASSERT_EQ(ret, 0, "UNREGISTER");

	TEST_ASSERT_EQ(capi_i2c_deinit(i2c_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief Stress-test init/deinit cycles to catch resource leaks.
 */
static int i2c_reinit(void)
{
	struct capi_i2c_controller_handle *i2c_handle = NULL;
	int ret = 0;

	TEST_SECTION("REINIT");

	for (uint32_t i = 0U; i < I2C_REINIT_COUNT; i++) {
		ret = capi_i2c_init(&i2c_handle, &i2c_master_config);
		if (ret != 0)
			break;
		ret = capi_i2c_deinit(i2c_handle);
		i2c_handle = NULL;
		if (ret != 0)
			break;
	}
	TEST_VALUE("I2C.REINIT.iterations", I2C_REINIT_COUNT);
	TEST_ASSERT_EQ(ret, 0, "REINIT_LOOP");

	return 0;
}

/**
 * @brief Reject invalid arguments: NULL handles, NULL configs, NULL devices.
 */
static int i2c_error_paths(void)
{
	struct capi_i2c_controller_handle *i2c_handle = NULL;
	struct capi_i2c_controller_handle *bad_handle = NULL;
	struct capi_i2c_device no_controller = i2c_dev;
	struct capi_i2c_transfer empty = { 0 };
	int ret;

	TEST_SECTION("ERROR_PATHS");

	TEST_ASSERT_EQ(capi_i2c_init(NULL, &i2c_master_config), -EINVAL,
		       "INIT_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_i2c_init(&bad_handle, NULL), -EINVAL,
		       "INIT_NULL_CONFIG");
	TEST_ASSERT_EQ(capi_i2c_deinit(NULL), -EINVAL, "DEINIT_NULL");
	TEST_ASSERT_EQ(capi_i2c_transmit(NULL, &empty), -EINVAL,
		       "TX_NULL_DEV");
	TEST_ASSERT_EQ(capi_i2c_transmit(&no_controller, &empty), -EINVAL,
		       "TX_NO_CTRL");
	TEST_ASSERT_EQ(capi_i2c_receive(NULL, &empty), -EINVAL,
		       "RX_NULL_DEV");
	TEST_ASSERT_EQ(capi_i2c_receive(&no_controller, &empty), -EINVAL,
		       "RX_NO_CTRL");
	TEST_ASSERT_EQ(capi_i2c_transmit_async(NULL, &empty), -EINVAL,
		       "ASYNC_TX_NULL_DEV");
	TEST_ASSERT_EQ(capi_i2c_receive_async(NULL, &empty), -EINVAL,
		       "ASYNC_RX_NULL_DEV");
	TEST_ASSERT_EQ(capi_i2c_register_callback(NULL, NULL, NULL), -EINVAL,
		       "CB_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_i2c_register_target(NULL, 0x50U), -EINVAL,
		       "REG_TARGET_NULL");
	TEST_ASSERT_EQ(capi_i2c_unregister_target(NULL), -EINVAL,
		       "UNREG_TARGET_NULL");
	TEST_ASSERT_EQ(capi_i2c_recover_bus(NULL), -EINVAL,
		       "RECOVER_NULL");

	ret = capi_i2c_init(&i2c_handle, &i2c_master_config);
	TEST_ASSERT_EQ(ret, 0, "INIT_FOR_ERROR_PATHS");

	no_controller.controller = i2c_handle;
	TEST_ASSERT_EQ(capi_i2c_transmit(&no_controller, NULL), -EINVAL,
		       "TX_NULL_XFER");
	TEST_ASSERT_EQ(capi_i2c_receive(&no_controller, NULL), -EINVAL,
		       "RX_NULL_XFER");
	TEST_ASSERT_EQ(capi_i2c_transmit_async(&no_controller, NULL), -EINVAL,
		       "ASYNC_TX_NULL_XFER");
	TEST_ASSERT_EQ(capi_i2c_receive_async(&no_controller, NULL), -EINVAL,
		       "ASYNC_RX_NULL_XFER");

	capi_i2c_isr(NULL);
	test_framework_set_name("ISR_NULL_SAFE");
	TEST_PASS();

	TEST_ASSERT_EQ(capi_i2c_deinit(i2c_handle), 0, "DEINIT_AFTER_ERRORS");

	return 0;
}

#ifndef I2C_TARGET_OPS
#define I2C_HAS_TARGET 0
#else
#define I2C_HAS_TARGET 1
#endif

#ifndef I2C_HAS_IRQ
#define I2C_HAS_IRQ 0
#endif

static const struct test_case i2c_subtests[] = {
#ifdef I2C_TARGET_OPS
	{ "BASIC",           i2c_basic,           false },
#else
	{ "BASIC",           NULL,                false },
#endif
	{ "REGISTER_TARGET", i2c_register_target, false },
	{ "REINIT",          i2c_reinit,          false },
	{ "ERROR_PATHS",     i2c_error_paths,     false },
};

int test_i2c(void)
{
	return test_framework_run_cases(I2C_MODULE, i2c_subtests,
					sizeof(i2c_subtests) / sizeof(i2c_subtests[0]));
}

#endif /* I2C_OPS */
