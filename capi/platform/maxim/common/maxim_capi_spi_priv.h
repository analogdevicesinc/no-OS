/*******************************************************************************
 *   @file   maxim_capi_spi_priv.h
 *   @brief  Private data of the MAX32650/MAX32690 CAPI SPI driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_SPI_PRIV_H_
#define MAXIM_CAPI_SPI_PRIV_H_

#include <stdbool.h>
#include "capi_dma.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_spi.h"
#include "spi_reva_regs.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_spi_priv
 * @brief SPI controller private data
 */
struct max_capi_spi_priv {
	mxc_spi_reva_regs_t *regs;
	uint32_t id;
	uint32_t clock_freq;
	/** The last transaction released SS, so the block needs a reset */
	bool ss_released;
	/** CS kept asserted between transfers by capi_spi_set_cs() */
	bool cs_hold;

	/** Device configuration applied to the controller */
	bool cfg_valid;
	uint32_t cfg_speed;
	enum capi_spi_mode cfg_mode;
	uint16_t cfg_cs;
	uint32_t cfg_delay_first;
	uint32_t cfg_delay_last;
	/** Register state the configuration produced, to catch other users */
	uint32_t cfg_clkctrl;
	uint32_t cfg_ctrl2;
	uint32_t cfg_ss_active;

	/** DMA channels, NULL when DMA is not configured */
	struct capi_dma_chan *rx_dma;
	struct capi_dma_chan *tx_dma;
	/** DMA descriptors, reused for every transfer */
	struct capi_dma_transfer dma_rx;
	struct capi_dma_transfer dma_tx;
	struct max_capi_dma_xfer_extra rx_req;
	struct max_capi_dma_xfer_extra tx_req;
	uint32_t dma_min_len;

	/** In-flight DMA sequence */
	struct capi_spi_device *xfer_device;
	struct capi_spi_transfer *seq;
	uint32_t seq_left;
	bool xfer_async;
	volatile bool xfer_done;
	volatile int xfer_status;
	volatile bool async_in_progress;
	/** Two-transfer sequence built by read_command */
	struct capi_spi_transfer cmd_seq[2];

	capi_spi_callback_t callback;
	void *callback_arg;
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
