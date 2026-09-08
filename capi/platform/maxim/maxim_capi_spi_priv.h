/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_spi_priv.h
 * @brief Private SPI handle for the common Maxim backend.
 *
 * Async is driven by the MSDK interrupt engine (MXC_SPI_MasterTransactionAsync
 * + MXC_SPI_AsyncHandler), so this backend keeps a single in-flight request
 * slot plus the resolved config it needs at transfer time.
 */

#ifndef MAXIM_CAPI_SPI_PRIV_H_
#define MAXIM_CAPI_SPI_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "spi.h"
#include "maxim_capi_spi.h"

/**
 * @struct max_capi_spi_priv
 * @brief Private structure for the common Maxim SPI backend.
 */
struct max_capi_spi_priv {
	/** SPI controller handle (back-pointer). */
	struct capi_spi_controller_handle *spi_handle;
	/** MSDK register block for this instance. */
	mxc_spi_regs_t *regs;
	/** SPI instance index (MXC_SPI_GET_IDX). */
	uint32_t identifier;
	/** OPTIONAL - DMA controller handle (reserved). */
	struct capi_dma_handle *dma_handle;
	/** Platform-specific config (copied at init). */
	struct max_capi_spi_extra extra;
	/** Bus clock frequency in Hz. */
	uint32_t clock_freq;
	/** CAPI completion callback. */
	capi_spi_callback_t callback;
	/** Callback argument. */
	void *callback_arg;
	/** In-flight async request (owned by MSDK while active). */
	mxc_spi_req_t async_req;
	/** Zero-filled dummy-TX pad allocated for an async read_command RX
	 *  phase (clocks the bus, like the tx_pad in transceive); freed on
	 *  completion/abort. NULL when the async request owns no pad. */
	void *async_tx_pad;
	/** Async transfer in progress. */
	volatile bool async_in_progress;
	/** Manual CS held asserted across transfers (set_cs MANUAL_ASSERT). */
	bool cs_hold;
};

#define CAPI_SPI_HANDLE_MAXIM_INIT()			\
	(&(struct capi_spi_controller_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_spi_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_SPI_PRIV_H_ */
