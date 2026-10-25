/*******************************************************************************
 *   @file   maxim_capi_dma_priv.h
 *   @brief  Private data of the MAX32650/MAX32690 CAPI DMA driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_DMA_PRIV_H_
#define MAXIM_CAPI_DMA_PRIV_H_

#include "maxim_capi_dma.h"
#include "mxc_device.h"
#include "dma_reva_regs.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_dma_ch_priv
 * @brief DMA channel private data
 */
struct max_capi_dma_ch_priv {
	/** Hardware channel, from MXC_DMA_AcquireChannel() */
	int hw_ch;
	mxc_dma_reva_ch_regs_t *regs;
	/** CTRL value for the current transfer shape, without EN and IE bits */
	uint32_t ctrl;
	/** Transfer shape ctrl was built from */
	bool cfg_valid;
	enum capi_dma_xfer_type cfg_type;
	enum capi_dma_addr_inc cfg_src_inc;
	enum capi_dma_addr_inc cfg_dst_inc;
	enum capi_dma_xfer_size cfg_src_size;
	enum capi_dma_xfer_size cfg_dst_size;
	mxc_dma_reqsel_t cfg_reqsel;
	uint32_t src;
	uint32_t dst;
	uint32_t cnt;
	volatile bool completed;
	struct capi_dma_chan *capi_chan;
	/** Channel storage, used when the channel is not owned by the app */
	struct capi_dma_chan chan_storage;
};

/**
 * @struct max_capi_dma_priv
 * @brief DMA controller private data
 */
struct max_capi_dma_priv {
	uint32_t num_chans;
	bool use_irq;
	uint32_t irq_priority;
	void (*irq_notify)(void *ctx);
	void *irq_notify_ctx;
	/** Channels, indexed by CAPI channel ID */
	struct max_capi_dma_ch_priv *chans[MXC_DMA_CHANNELS];
};

#define CAPI_DMA_HANDLE_MAXIM_INIT()			\
	(&(struct capi_dma_handle) {			\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_dma_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_DMA_PRIV_H_ */
