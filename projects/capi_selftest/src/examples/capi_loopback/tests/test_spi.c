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

	/*
	 * Manual CS around a real transfer: hold CS asserted, move data, then
	 * deassert. The loopback MATCH proves the bus actually transacted while
	 * CS was under manual control (native CS state itself isn't observable
	 * over MOSI-tied-to-MISO; the transferred bytes are the only oracle).
	 */
	uint8_t cs_tx[] = { 0x5a, 0xa5, 0x0f, 0xf0 };
	uint8_t cs_rx[sizeof(cs_tx)];
	memset(cs_rx, 0, sizeof(cs_rx));
	struct capi_spi_transfer cs_xfer = {
		.tx_buf = cs_tx,
		.rx_buf = cs_rx,
		.tx_size = sizeof(cs_tx),
		.rx_size = sizeof(cs_rx),
	};
	TEST_ASSERT_EQ(capi_spi_set_cs(&dev, CAPI_SPI_CS_MANUAL_ASSERT), 0,
		       "CS_ASSERT");
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &cs_xfer), 0, "CS_XFER");
	TEST_ASSERT_EQ(memcmp(cs_rx, cs_tx, sizeof(cs_tx)), 0, "CS_XFER_MATCH");
	TEST_ASSERT_EQ(capi_spi_set_cs(&dev, CAPI_SPI_CS_MANUAL_DEASSERT), 0,
		       "CS_DEASSERT");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/*
 * The async/IRQ cases below bring a live controller up and start interrupt- or
 * DMA-driven transfers. An assertion that returned early without deinit would
 * leak the controller handle -- and any in-flight transfer -- into the next
 * test. CLEANUP releases the handle on every exit path; deinit tears down any
 * pending transfer, and it is NULL-guarded so it is safe before init succeeds.
 * The global-IRQ mask used by the manual-ISR and abort cases is re-enabled
 * inline before any assertion can fail, so it needs no undo here.
 */
#define CLEANUP \
	do { \
		if (spi_handle != NULL) \
			(void)capi_spi_deinit(spi_handle); \
	} while (0)

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
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_register_callback(spi_handle,
				  spi_test_callback, NULL), 0, "REGISTER_CALLBACK");

	uint8_t async_tx[] = { 0x55, 0xaa, 0x33, 0xcc };
	uint8_t async_rx[sizeof(async_tx)];
	memset(async_rx, 0, sizeof(async_rx));
	struct capi_spi_transfer async = {
		.tx_buf = async_tx,
		.rx_buf = async_rx,
		.tx_size = sizeof(async_tx),
		.rx_size = sizeof(async_rx),
	};
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_transceive_async(&dev, &async), 0,
				  "ASYNC_XFER");
	TEST_WAIT_UNTIL(spi_callback_count > 0U, SPI_ASYNC_TIMEOUT_US,
			SPI_ASYNC_STEP_US);
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_count, 1U, "ASYNC_CB_COUNT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
				  "ASYNC_CB_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_extra, 0, "ASYNC_CB_EXTRA");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(async_rx, async_tx, sizeof(async_tx)), 0,
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
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_read_command(&dev, &read), 0,
				  "READ_COMMAND");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(read_rx, zero3, sizeof(read_rx)), 0,
				  "READ_COMMAND_MATCH");

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	memset(read_rx, 0xa5, sizeof(read_rx));
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_read_command_async(&dev, &read), 0,
				  "ASYNC_READ_COMMAND");
	TEST_WAIT_UNTIL(spi_callback_count > 0U, SPI_ASYNC_TIMEOUT_US,
			SPI_ASYNC_STEP_US);
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_count, 1U, "ASYNC_READ_CB_COUNT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
				  "ASYNC_READ_CB_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_extra, 0, "ASYNC_READ_CB_EXTRA");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(read_rx, zero3, sizeof(read_rx)), 0,
				  "ASYNC_READ_MATCH");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_deinit(spi_handle), 0, "DEINIT");

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
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_register_callback(spi_handle,
				  spi_test_callback, NULL), 0, "REGISTER_CALLBACK");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_irq_global_disable(), 0,
				  "IRQ_DISABLE_FOR_ISR");
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
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "ISR_ASYNC_XFER");
	for (uint32_t waited = 0U;
	     spi_callback_count == 0U && waited < SPI_ASYNC_TIMEOUT_US;
	     waited += SPI_ASYNC_STEP_US) {
		capi_spi_isr(spi_handle);
		test_framework_wait_us(SPI_ASYNC_STEP_US);
	}
	if (spi_callback_count == 0U)
		(void)capi_spi_abort_async(&dev);
	TEST_ASSERT_EQ_OR_CLEANUP(capi_irq_global_enable(), 0,
				  "IRQ_REENABLE_AFTER_ISR");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_count, 1U, "ISR_CB_COUNT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
				  "ISR_CB_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_extra, 0, "ISR_CB_EXTRA");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(isr_rx, isr_tx, sizeof(isr_tx)), 0,
				  "ISR_MATCH");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_deinit(spi_handle), 0, "DEINIT");

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
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_register_callback(spi_handle,
				  spi_test_callback, NULL), 0, "REGISTER_CALLBACK");

	uint8_t dma_tx[] = { 0x77, 0x88, 0x99, 0xaa };
	uint8_t dma_rx[sizeof(dma_tx)];
	memset(dma_rx, 0, sizeof(dma_rx));
	struct capi_spi_transfer dma_async = {
		.tx_buf = dma_tx,
		.rx_buf = dma_rx,
		.tx_size = sizeof(dma_tx),
		.rx_size = sizeof(dma_rx),
	};
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_transceive_async(&dev, &dma_async), 0,
				  "DMA_ASYNC_XFER");
	TEST_WAIT_UNTIL(spi_callback_count > 0U, SPI_ASYNC_TIMEOUT_US,
			SPI_ASYNC_STEP_US);
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_count, 1U, "DMA_ASYNC_CB_COUNT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_event, CAPI_SPI_EVENT_XFR_DONE,
				  "DMA_ASYNC_CB_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_extra, 0, "DMA_ASYNC_CB_EXTRA");
	TEST_ASSERT_EQ_OR_CLEANUP(memcmp(dma_rx, dma_tx, sizeof(dma_tx)), 0,
				  "DMA_ASYNC_MATCH");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_deinit(spi_handle), 0, "DEINIT");

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
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "INIT");
	dev.controller = spi_handle;

	spi_callback_count = 0U;
	spi_callback_event = 0;
	spi_callback_extra = 0;
	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_register_callback(spi_handle,
				  spi_test_callback, NULL), 0, "REGISTER_CALLBACK");

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
	TEST_ASSERT_EQ_OR_CLEANUP(capi_irq_global_disable(), 0,
				  "IRQ_DISABLE_FOR_ABORT");
	ret = capi_spi_transceive_async(&dev, &abort_xfer);
	if (ret == 0)
		ret = capi_spi_abort_async(&dev);
	else
		(void)capi_spi_abort_async(&dev);
	(void)capi_irq_global_enable();
	TEST_ASSERT_EQ_OR_CLEANUP(ret, 0, "ABORT_ACTIVE");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_count, 1U, "ABORT_CB_COUNT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_event, CAPI_SPI_EVENT_ERROR,
				  "ABORT_TERMINAL_EVENT");
	TEST_ASSERT_EQ_OR_CLEANUP(spi_callback_extra, 0, "ABORT_CB_EXTRA");

	TEST_ASSERT_EQ_OR_CLEANUP(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

#undef CLEANUP

/**
 * @brief Every SPI clock mode moves data intact over the loopback.
 *
 * Runs a full-duplex transfer in each of CAPI_SPI_MODE_0..3, re-initializing
 * the device with the mode under test. Under MOSI-tied-to-MISO loopback the
 * shift-out and shift-in use the same clock, so a correct clock polarity/phase
 * configuration round-trips the bytes unchanged; a broken mode setup corrupts
 * or drops them. Purely external: the only oracle is the returned data.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_modes(void)
{
	static const uint8_t modes[] = {
		CAPI_SPI_MODE_0, CAPI_SPI_MODE_1,
		CAPI_SPI_MODE_2, CAPI_SPI_MODE_3,
	};
	static const char *const names[] = {
		"MODE_0", "MODE_1", "MODE_2", "MODE_3",
	};
	struct capi_spi_controller_handle *spi_handle = NULL;
	int ret;

	TEST_SECTION("MODES");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");

	uint8_t tx[] = {
		0x00, 0xff, 0xa5, 0x5a, 0x0f, 0xf0, 0xcc, 0x33,
	};
	uint8_t rx[sizeof(tx)];

	for (uint32_t i = 0U; i < sizeof(modes); i++) {
		struct capi_spi_device dev = spi_dev;
		dev.controller = spi_handle;
		dev.mode = modes[i];

		memset(rx, 0, sizeof(rx));
		struct capi_spi_transfer xfer = {
			.tx_buf = tx,
			.rx_buf = rx,
			.tx_size = sizeof(tx),
			.rx_size = sizeof(rx),
		};
		TEST_ASSERT_EQ(capi_spi_transceive(&dev, &xfer), 0, names[i]);
		TEST_ASSERT_EQ(memcmp(rx, tx, sizeof(tx)), 0, names[i]);
	}

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief LSB-first bit order round-trips over the loopback.
 *
 * With dev.lsb_first the controller shifts the least-significant bit first on
 * both TX and RX. Over the external loopback the two directions use the same
 * bit order, so the bytes must return unchanged. Confirms the lsb_first config
 * takes effect on real hardware rather than corrupting the stream.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_lsb_first(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_device dev = spi_dev;
	int ret;

	TEST_SECTION("LSB_FIRST");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	dev.controller = spi_handle;
	dev.lsb_first = true;

	uint8_t tx[] = { 0x01, 0x80, 0xa5, 0x5a, 0x0f, 0xf0, 0x7e, 0xe7 };
	uint8_t rx[sizeof(tx)];
	memset(rx, 0, sizeof(rx));
	struct capi_spi_transfer xfer = {
		.tx_buf = tx,
		.rx_buf = rx,
		.tx_size = sizeof(tx),
		.rx_size = sizeof(rx),
	};
	ret = capi_spi_transceive(&dev, &xfer);
	if (ret == -ENOTSUP)
		TEST_SKIP_CAT(SKIP_FEATURE_DISABLED, "lsb_first unsupported");
	TEST_ASSERT_EQ(ret, 0, "LSB_XFER");
	TEST_ASSERT_EQ(memcmp(rx, tx, sizeof(tx)), 0, "LSB_MATCH");

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/**
 * @brief Data-integrity sweep: bit patterns and transfer sizes over loopback.
 *
 * Drives fixed patterns (all-ones, all-zeros, alternating 0xAA/0x55), a
 * walking-ones sequence, and a range of transfer sizes (1, 32, 64 bytes)
 * through the loopback, asserting rx == tx each time. Catches stuck bits,
 * FIFO width/count bugs, and byte-ordering errors that a single fixed transfer
 * would miss. Fully external — verified only by the returned bytes.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int spi_data(void)
{
	static const struct {
		const char *name;
		uint8_t val;
	} patterns[] = {
		{ "FF", 0xff }, { "00", 0x00 }, { "AA", 0xaa }, { "55", 0x55 },
	};
	static const uint32_t sizes[] = { 1U, 32U, 64U };
	struct capi_spi_controller_handle *spi_handle = NULL;
	struct capi_spi_device dev = spi_dev;
	uint8_t tx[64];
	uint8_t rx[64];
	int ret;

	TEST_SECTION("DATA");
	ret = capi_spi_init(&spi_handle, &spi_controller_config);
	TEST_ASSERT_EQ(ret, 0, "INIT");
	dev.controller = spi_handle;

	for (uint32_t p = 0U; p < sizeof(patterns) / sizeof(patterns[0]); p++) {
		memset(tx, patterns[p].val, sizeof(tx));
		memset(rx, (uint8_t)~patterns[p].val, sizeof(rx));
		struct capi_spi_transfer xfer = {
			.tx_buf = tx,
			.rx_buf = rx,
			.tx_size = sizeof(tx),
			.rx_size = sizeof(rx),
		};
		TEST_ASSERT_EQ(capi_spi_transceive(&dev, &xfer), 0,
			       patterns[p].name);
		TEST_ASSERT_EQ(memcmp(rx, tx, sizeof(tx)), 0, patterns[p].name);
	}

	uint8_t walk[] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
	};
	uint8_t walk_rx[sizeof(walk)];
	memset(walk_rx, 0, sizeof(walk_rx));
	struct capi_spi_transfer walk_xfer = {
		.tx_buf = walk,
		.rx_buf = walk_rx,
		.tx_size = sizeof(walk),
		.rx_size = sizeof(walk),
	};
	TEST_ASSERT_EQ(capi_spi_transceive(&dev, &walk_xfer), 0, "WALK1");
	TEST_ASSERT_EQ(memcmp(walk_rx, walk, sizeof(walk)), 0, "WALK1_MATCH");

	for (uint32_t s = 0U; s < sizeof(sizes) / sizeof(sizes[0]); s++) {
		uint32_t n = sizes[s];
		for (uint32_t i = 0U; i < n; i++)
			tx[i] = (uint8_t)(i ^ 0x5aU);
		memset(rx, 0, n);
		struct capi_spi_transfer xfer = {
			.tx_buf = tx,
			.rx_buf = rx,
			.tx_size = n,
			.rx_size = n,
		};
		TEST_ASSERT_EQ(capi_spi_transceive(&dev, &xfer), 0, "SIZE");
		TEST_ASSERT_EQ(memcmp(rx, tx, n), 0, "SIZE_MATCH");
	}

	TEST_ASSERT_EQ(capi_spi_deinit(spi_handle), 0, "DEINIT");

	return 0;
}

/*
 * SPI_HAS_IRQ and SPI_HAS_DMA are mutually exclusive: the controller is
 * configured for exactly one async delivery mode per build (via SPI_EXTRA_INIT
 * and dma_handle in common_data). Async and abort are therefore gated per mode,
 * so only the entries matching the configured delivery run; the rest skip.
 */
static const struct test_case spi_subtests[] = {
	{ "BASIC",      spi_basic,      false        },
	{ "MODES",      spi_modes,      false        },
	{ "LSB_FIRST",  spi_lsb_first,  false        },
	{ "DATA",       spi_data,       false        },
	{ "ASYNC_IRQ",  spi_async_irq,  !SPI_HAS_IRQ },
	{ "MANUAL_ISR", spi_manual_isr, !SPI_HAS_IRQ },
	{ "ABORT_IRQ",  spi_abort,      !SPI_HAS_IRQ },
	{ "ASYNC_DMA",  spi_async_dma,  !SPI_HAS_DMA },
	{ "ABORT_DMA",  spi_abort,      !SPI_HAS_DMA },
};

/**
 * @brief Exercise the CAPI SPI controller against an external loopback.
 *
 * Runs the SPI subtest table: a basic synchronous transfer, every clock mode,
 * LSB-first bit order, a data-integrity sweep, the async delivery paths
 * (IRQ-driven completion, manual ISR pumping, and DMA), and transfer abort.
 * Every case verifies behavior through the external loopback (rx == tx); none
 * inspect internal state or argument validation. Async and abort entries are
 * gated on the build's delivery mode (see the note on the table above), so a
 * build configured for one mode skips the other's cases.
 *
 * API coverage:
 *   capi_spi_init()            init
 *   capi_spi_deinit()          deinit
 *   capi_spi_transceive()      basic, modes, lsb_first, data
 *   capi_spi_transceive_async() async (IRQ/DMA), manual ISR
 *   capi_spi_abort_async()     abort
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
