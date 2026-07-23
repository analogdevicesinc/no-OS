/***************************************************************************//**
 * @file test_spi.c
 * @brief CAPI SPI external-loopback tests.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "capi_irq.h"
#include "capi_spi.h"
#include "parameters.h"
#include "common_data.h"
#include "test_framework.h"
#include "test_spi.h"

#define SPI_MODULE		"SPI"
#define SPI_ASYNC_TIMEOUT_US	1000000U
#define SPI_ASYNC_STEP_US	1000U
#define SPI_REINIT_COUNT	5U
#define SPI_ABORT_SIZE		128U

static volatile unsigned int spi_callback_count;
static volatile enum capi_async_event spi_callback_event;
static volatile int spi_callback_extra;
static uint8_t spi_abort_tx[SPI_ABORT_SIZE];
static uint8_t spi_abort_rx[SPI_ABORT_SIZE];

static void spi_test_callback(enum capi_async_event event, void *arg,
			      int event_extra)
{
	(void)arg;

	spi_callback_event = event;
	spi_callback_extra = event_extra;
	spi_callback_count++;
}

/**
 * @brief Basic CAPI SPI contract: init, deinit, full-duplex and complex transfers.
 *
 * Hardware assumption: MOSI is physically wired to MISO. Every *_MATCH assertion
 * proves the transfer actually moved data through loopback.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_basic(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_device dev = spi_dev;
	int ret;

	TEST_SECTION("BASIC");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	dev.controller = spi_handle;

	uint8_t basic_tx[] = {
		0x00, 0xff, 0xa5, 0x5a, 0x01, 0x02, 0x04, 0x08,
		0x10, 0x20, 0x40, 0x80, 0xde, 0xad, 0xbe, 0xef,
	};
	uint8_t basic_rx[sizeof(basic_tx)];
	memset(basic_rx, 0, sizeof(basic_rx));
	struct capi_spi_transfer basic = {
		.tx_buf = basic_tx,
		.rx_buf = basic_rx,
		.tx_size = sizeof(basic_tx),
		.rx_size = sizeof(basic_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &basic), 0, "FULL_DUPLEX");
	TEST_ASSERT_EQ(memcmp(basic_rx, basic_tx, sizeof(basic_tx)), 0,
		       "FULL_DUPLEX_MATCH");

	uint8_t one_tx = 0x3c;
	uint8_t one_rx = 0x00;
	struct capi_spi_transfer one = {
		.tx_buf = &one_tx,
		.rx_buf = &one_rx,
		.tx_size = 1U,
		.rx_size = 1U,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &one), 0, "ONE_BYTE");
	TEST_ASSERT_EQ(one_rx, one_tx, "ONE_BYTE_MATCH");

	uint8_t short_tx[] = { 0x12, 0x34 };
	uint8_t long_rx[5];
	memset(long_rx, 0xee, sizeof(long_rx));
	struct capi_spi_transfer tx_short = {
		.tx_buf = short_tx,
		.rx_buf = long_rx,
		.tx_size = sizeof(short_tx),
		.rx_size = sizeof(long_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &tx_short), 0, "TX_SHORT");
	TEST_ASSERT_EQ(memcmp(long_rx, short_tx, sizeof(short_tx)), 0,
		       "TX_SHORT_MATCH");

	uint8_t long_tx[] = { 0x9a, 0xbc, 0xde, 0xf0 };
	uint8_t rx_guard[] = { 0xcc, 0x00, 0x00, 0xdd };
	struct capi_spi_transfer rx_short = {
		.tx_buf = long_tx,
		.rx_buf = &rx_guard[1],
		.tx_size = sizeof(long_tx),
		.rx_size = 2U,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &rx_short), 0, "RX_SHORT");
	TEST_ASSERT_EQ(rx_guard[0], 0xcc, "GUARD_PRE");
	TEST_ASSERT_EQ(rx_guard[1], long_tx[0], "RX0");
	TEST_ASSERT_EQ(rx_guard[2], long_tx[1], "RX1");
	TEST_ASSERT_EQ(rx_guard[3], 0xdd, "GUARD_POST");

	uint8_t null_tx_rx[4];
	uint8_t zero4[4] = { 0 };
	memset(null_tx_rx, 0xa5, sizeof(null_tx_rx));
	struct capi_spi_transfer null_tx = {
		.tx_buf = NULL,
		.rx_buf = null_tx_rx,
		.tx_size = 0U,
		.rx_size = sizeof(null_tx_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &null_tx), 0, "NULL_TX");
	TEST_ASSERT_EQ(memcmp(null_tx_rx, zero4, sizeof(null_tx_rx)), 0,
		       "NULL_TX_MATCH");

	uint8_t discard_tx[] = { 0xca, 0xfe, 0xba, 0xbe };
	struct capi_spi_transfer null_rx = {
		.tx_buf = discard_tx,
		.rx_buf = NULL,
		.tx_size = sizeof(discard_tx),
		.rx_size = 0U,
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &null_rx), 0, "NULL_RX");

	TEST_ASSERT_EQ(capi_spi_set_cs(&dev, CAPI_SPI_CS_MANUAL_ASSERT), 0,
		       "CS_ASSERT");
	TEST_ASSERT_EQ(capi_spi_set_cs(&dev, CAPI_SPI_CS_MANUAL_DEASSERT), 0,
		       "CS_DEASSERT");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief IRQ-backed async transfers: register callback, IRQ-driven completion.
 *
 * Exercises capi_spi_transceive_async(), callback registration, read_command
 * framing, and async completion via IRQ. The transfer must complete through
 * the IRQ callback, and received bytes must match the external loopback data.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_async_irq(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_device dev = spi_dev;
	int ret;

	TEST_SECTION("ASYNC_IRQ");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ(capi_spi_register_callback(spi_handle, spi_test_callback,
			NULL), 0, "REGISTER_CALLBACK");

	uint8_t async_tx[] = { 0x55, 0xaa, 0x33, 0xcc };
	uint8_t async_rx[sizeof(async_tx)];
	memset(async_rx, 0, sizeof(async_rx));
	struct capi_spi_transfer async = {
		.tx_buf = async_tx,
		.rx_buf = async_rx,
		.tx_size = sizeof(async_tx),
		.rx_size = sizeof(async_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive_async(&dev, &async), 0, "ASYNC_XFER");
	TEST_WAIT_UNTIL(spi_callback_count > 0U, SPI_ASYNC_TIMEOUT_US,
			SPI_ASYNC_STEP_US);
	TEST_ASSERT_EQ(spi_callback_count, 1U, "ASYNC_CB_COUNT");
	TEST_ASSERT_EQ(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
		       "ASYNC_CB_EVENT");
	TEST_ASSERT_EQ(spi_callback_extra, 0, "ASYNC_CB_EXTRA");
	TEST_ASSERT_EQ(memcmp(async_rx, async_tx, sizeof(async_tx)), 0,
		       "ASYNC_MATCH");

	uint8_t command[] = { 0x9f, 0x00 };
	uint8_t read_rx[3];
	uint8_t zero3[3] = { 0 };
	memset(read_rx, 0xa5, sizeof(read_rx));
	struct capi_spi_transfer read = {
		.tx_buf = command,
		.rx_buf = read_rx,
		.tx_size = sizeof(command),
		.rx_size = sizeof(read_rx),
	};
	TEST_ASSERT_EQ(capi_spi_read_command(&dev, &read), 0, "READ_COMMAND");
	TEST_ASSERT_EQ(memcmp(read_rx, zero3, sizeof(read_rx)), 0,
		       "READ_COMMAND_MATCH");

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	memset(read_rx, 0xa5, sizeof(read_rx));
	TEST_ASSERT_EQ(capi_spi_read_command_async(&dev, &read), 0,
		       "ASYNC_READ_COMMAND");
	TEST_WAIT_UNTIL(spi_callback_count > 0U, SPI_ASYNC_TIMEOUT_US,
			SPI_ASYNC_STEP_US);
	TEST_ASSERT_EQ(spi_callback_count, 1U, "ASYNC_READ_CB_COUNT");
	TEST_ASSERT_EQ(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
		       "ASYNC_READ_CB_EVENT");
	TEST_ASSERT_EQ(spi_callback_extra, 0, "ASYNC_READ_CB_EXTRA");
	TEST_ASSERT_EQ(memcmp(read_rx, zero3, sizeof(read_rx)), 0,
		       "ASYNC_READ_MATCH");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief User-managed ISR dispatch: manual capi_spi_isr() calls with global IRQs disabled.
 *
 * This is a valid CAPI pattern when interrupt vectors are user-managed. Disables
 * global interrupts, starts an async transfer, manually pumps capi_spi_isr() in
 * a loop until the callback fires, then re-enables interrupts. Proves the ISR
 * path works independently of the IRQ controller's automatic dispatch.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_manual_isr(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_device dev = spi_dev;
	int ret;

	TEST_SECTION("MANUAL_ISR");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ(capi_spi_register_callback(spi_handle, spi_test_callback,
			NULL), 0, "REGISTER_CALLBACK");

	TEST_ASSERT_EQ(capi_irq_global_disable(), 0, "IRQ_DISABLE_FOR_ISR");
	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	uint8_t isr_tx[] = { 0x11, 0x22, 0x44, 0x88 };
	uint8_t isr_rx[sizeof(isr_tx)];
	memset(isr_rx, 0, sizeof(isr_rx));
	struct capi_spi_transfer isr_async = {
		.tx_buf = isr_tx,
		.rx_buf = isr_rx,
		.tx_size = sizeof(isr_tx),
		.rx_size = sizeof(isr_rx),
	};
	ret = capi_spi_transceive_async(&dev, &isr_async);
	if (ret != 0)
		(void)capi_irq_global_enable();
	TEST_ASSERT_EQ(ret, 0, "ISR_ASYNC_XFER");
	for (uint32_t waited = 0U;
	     spi_callback_count == 0U && waited < SPI_ASYNC_TIMEOUT_US;
	     waited += SPI_ASYNC_STEP_US) {
		capi_spi_isr(spi_handle);
		test_framework_wait_us(SPI_ASYNC_STEP_US);
	}
	if (spi_callback_count == 0U)
		(void)capi_spi_abort_async(&dev);
	TEST_ASSERT_EQ(capi_irq_global_enable(), 0, "IRQ_REENABLE_AFTER_ISR");
	TEST_ASSERT_EQ(spi_callback_count, 1U, "ISR_CB_COUNT");
	TEST_ASSERT_EQ(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
		       "ISR_CB_EVENT");
	TEST_ASSERT_EQ(spi_callback_extra, 0, "ISR_CB_EXTRA");
	TEST_ASSERT_EQ(memcmp(isr_rx, isr_tx, sizeof(isr_tx)), 0, "ISR_MATCH");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief DMA-backed async transfers: async completion via DMA delivery.
 *
 * Exercises capi_spi_transceive_async() when async is delivered by DMA instead
 * of IRQ. Callback registration and completion semantics are identical to IRQ;
 * only the underlying delivery mechanism differs. On platforms without DMA,
 * this test is skipped.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_async_dma(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_device dev = spi_dev;
	int ret;

	TEST_SECTION("ASYNC_DMA");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ(capi_spi_register_callback(spi_handle, spi_test_callback,
			NULL), 0, "REGISTER_CALLBACK");

	uint8_t dma_tx[] = { 0x77, 0x88, 0x99, 0xaa };
	uint8_t dma_rx[sizeof(dma_tx)];
	memset(dma_rx, 0, sizeof(dma_rx));
	struct capi_spi_transfer dma_async = {
		.tx_buf = dma_tx,
		.rx_buf = dma_rx,
		.tx_size = sizeof(dma_tx),
		.rx_size = sizeof(dma_rx),
	};
	TEST_ASSERT_EQ(capi_spi_transceive_async(&dev, &dma_async), 0,
		       "DMA_ASYNC_XFER");
	TEST_WAIT_UNTIL(spi_callback_count > 0U, SPI_ASYNC_TIMEOUT_US,
			SPI_ASYNC_STEP_US);
	TEST_ASSERT_EQ(spi_callback_count, 1U, "DMA_ASYNC_CB_COUNT");
	TEST_ASSERT_EQ(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
		       "DMA_ASYNC_CB_EVENT");
	TEST_ASSERT_EQ(spi_callback_extra, 0, "DMA_ASYNC_CB_EXTRA");
	TEST_ASSERT_EQ(memcmp(dma_rx, dma_tx, sizeof(dma_tx)), 0,
		       "DMA_ASYNC_MATCH");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief Abort an in-flight async transfer, deterministically.
 *
 * Starts an async transfer and aborts it while it is genuinely in flight. To
 * make the abort deterministic, global interrupts are masked around the
 * start+abort: with delivery masked the transfer cannot self-complete via the
 * ISR, so capi_spi_abort_async() always aborts an active transfer and fires a
 * single ERROR callback. This also avoids the main-thread/ISR data race on the
 * controller registers that a live-IRQ abort would create.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_abort(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_device dev = spi_dev;
	int ret;

	TEST_SECTION("ABORT");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ(capi_spi_register_callback(spi_handle, spi_test_callback,
			NULL), 0, "REGISTER_CALLBACK");

	for (uint32_t i = 0U; i < sizeof(spi_abort_tx); i++)
		spi_abort_tx[i] = (uint8_t)(i ^ 0x5aU);
	memset(spi_abort_rx, 0, sizeof(spi_abort_rx));
	struct capi_spi_transfer abort_xfer = {
		.tx_buf = spi_abort_tx,
		.rx_buf = spi_abort_rx,
		.tx_size = sizeof(spi_abort_tx),
		.rx_size = sizeof(spi_abort_rx),
	};

	/*
	 * Mask delivery so the ISR cannot race the abort: the transfer stays
	 * in flight (no self-completion), abort_async() tears it down in the
	 * main thread alone, and exactly one ERROR callback is delivered.
	 */
	TEST_ASSERT_EQ(capi_irq_global_disable(), 0, "IRQ_DISABLE_FOR_ABORT");
	ret = capi_spi_transceive_async(&dev, &abort_xfer);
	if (ret == 0)
		ret = capi_spi_abort_async(&dev);
	else
		(void)capi_spi_abort_async(&dev);
	(void)capi_irq_global_enable();
	TEST_ASSERT_EQ(ret, 0, "ABORT_ACTIVE");
	TEST_ASSERT_EQ(spi_callback_count, 1U, "ABORT_CB_COUNT");
	TEST_ASSERT_EQ(spi_callback_event, CAPI_SPI_EVENT_ERROR,
		       "ABORT_TERMINAL_EVENT");
	TEST_ASSERT_EQ(spi_callback_extra, 0, "ABORT_CB_EXTRA");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief Stress-test init/deinit cycles to catch resource leaks and state corruption.
 *
 * Opens and closes the SPI controller repeatedly. Catches stale busy state,
 * leaked allocations, broken IRQ reconnect handling, or other lifecycle bugs
 * that only appear under repeated init/deinit.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_reinit(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	int ret;

	TEST_SECTION("REINIT");
	for (uint32_t i = 0U; i < SPI_REINIT_COUNT; i++) {
		ret = capi_spi_init(&spi_handle, &spi_controller_config);
		if (ret != 0)
			break;
		ret = capi_spi_deinit(spi_handle);
		spi_handle = NULL;
		if (ret != 0)
			break;
	}
	TEST_VALUE("SPI.REINIT.iterations", SPI_REINIT_COUNT);
	TEST_ASSERT_EQ(ret, 0, "REINIT_LOOP");

	return 0;
}

/**
 * @brief Deterministic API/driver validation: reject clearly invalid arguments.
 *
 * The CAPI dispatcher validates NULL handles and NULL ops, but driver-specific
 * validation (e.g., NULL output pointers on get_* calls, invalid buffer sizes)
 * is the driver's responsibility. This test covers that boundary with a matrix
 * of invalid inputs and expected -EINVAL returns.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_error_paths(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_controller_handle *bad_handle = NULL;
	struct capi_spi_device no_controller = spi_dev;
	struct capi_spi_transfer empty = { 0 };
	int ret;

	TEST_SECTION("ERROR_PATHS");
	TEST_ASSERT_EQ(capi_spi_init(NULL, &spi_controller_config), -EINVAL,
		       "INIT_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_spi_init(&bad_handle, NULL), -EINVAL,
		       "INIT_NULL_CONFIG");
	TEST_ASSERT_EQ(capi_spi_deinit(NULL), -EINVAL, "DEINIT_NULL");
	TEST_ASSERT_EQ(capi_spi_transceive(NULL, &empty), -EINVAL,
		       "XFER_NULL_DEV");
	TEST_ASSERT_EQ(capi_spi_transceive(&no_controller, &empty), -EINVAL,
		       "XFER_NO_CTRL");
	TEST_ASSERT_EQ(capi_spi_transceive_async(NULL, &empty), -EINVAL,
		       "ASYNC_XFER_NULL_DEV");
	TEST_ASSERT_EQ(capi_spi_read_command(NULL, &empty), -EINVAL,
		       "READ_NULL_DEV");
	TEST_ASSERT_EQ(capi_spi_read_command_async(NULL, &empty), -EINVAL,
		       "ASYNC_READ_NULL_DEV");
	TEST_ASSERT_EQ(capi_spi_abort_async(NULL), -EINVAL, "ABORT_NULL_DEV");
	TEST_ASSERT_EQ(capi_spi_register_callback(NULL, spi_test_callback, NULL),
		       -EINVAL, "CB_NULL_HANDLE");
	TEST_ASSERT_EQ(capi_spi_set_cs(NULL, CAPI_SPI_CS_MANUAL_ASSERT), -EINVAL,
		       "CS_NULL_DEV");

	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT_FOR_ERROR_PATHS");
	no_controller.controller = spi_handle;
	TEST_ASSERT_EQ(capi_spi_transceive(&no_controller, NULL), -EINVAL,
		       "XFER_NULL_DESC");
	TEST_ASSERT_EQ(capi_spi_transceive_async(&no_controller, NULL), -EINVAL,
		       "ASYNC_XFER_NULL_DESC");
	TEST_ASSERT_EQ(capi_spi_read_command(&no_controller, NULL), -EINVAL,
		       "READ_NULL_DESC");
	TEST_ASSERT_EQ(capi_spi_read_command_async(&no_controller, NULL), -EINVAL,
		       "ASYNC_READ_NULL_DESC");
	capi_spi_isr(NULL);
	test_framework_set_name("ISR_NULL_SAFE");
	TEST_PASS();
	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT_AFTER_ERRORS");
	spi_handle = NULL;

	return 0;
}

/*
 * SPI_HAS_IRQ and SPI_HAS_DMA are mutually exclusive: the controller is
 * configured for exactly one async delivery mode per build (via SPI_EXTRA_INIT
 * and dma_handle in common_data). Async and abort are therefore gated per mode,
 * so only the entries matching the configured delivery run; the rest skip.
 */
static const struct test_case spi_subtests[] = {
	{ "BASIC",       spi_basic,       false        },
	{ "ASYNC_IRQ",   spi_async_irq,   !SPI_HAS_IRQ },
	{ "MANUAL_ISR",  spi_manual_isr,  !SPI_HAS_IRQ },
	{ "ABORT_IRQ",   spi_abort,       !SPI_HAS_IRQ },
	{ "ASYNC_DMA",   spi_async_dma,   !SPI_HAS_DMA },
	{ "ABORT_DMA",   spi_abort,       !SPI_HAS_DMA },
	{ "REINIT",      spi_reinit,      false        },
	{ "ERROR_PATHS", spi_error_paths, false        },
};

/**
 * @brief Exercise the CAPI SPI controller against an external loopback.
 *
 * Runs the SPI subtest table: a basic synchronous transfer, the async delivery
 * paths (IRQ-driven completion, manual ISR pumping, and DMA), transfer abort,
 * controller re-init, and the error/argument-validation paths. Async and abort
 * entries are gated on the build's delivery mode (see the note on the table
 * above), so a build configured for one mode skips the other's cases.
 *
 * API coverage:
 *   capi_spi_init()            init, reinit, error
 *   capi_spi_deinit()          deinit, reinit, error
 *   capi_spi_transfer()        basic, error
 *   capi_spi_transfer_async()  async (IRQ/DMA), manual ISR, error
 *   capi_spi_transfer_abort()  abort
 *
 * Setup assumption: common_data supplies one SPI controller wired for external
 * loopback (MOSI tied to MISO). No board- or vendor-specific behavior is
 * assumed.
 *
 * @return 0 on pass, first non-zero subtest error otherwise.
 */
int test_spi(void)
{
	return test_framework_run_cases(SPI_MODULE, spi_subtests,
					sizeof(spi_subtests) / sizeof(spi_subtests[0]));
}
