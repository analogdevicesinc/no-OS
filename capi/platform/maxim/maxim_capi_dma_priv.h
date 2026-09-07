/***************************************************************************//**
 *   @file   maxim_capi_dma_priv.h
 *   @brief  Header file for the DMA private handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_DMA_PRIV_H_
#define MAXIM_CAPI_DMA_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include "maxim_capi_dma.h"

/**
 * @struct max_capi_dma_priv_handle
 * @brief Maxim platform-specific DMA controller private handle.
 */
struct max_capi_dma_priv_handle {
	/** MSDK DMA instance driven by this backend (MXC_DMA1_S on the dual-secure
	 *  part, the sole MXC_DMA on single-controller parts). */
	mxc_dma_regs_t *dma;
	/** Number of logical channels the controller was configured for. */
	uint32_t num_chans;
	/** Array of per-channel private data pointers, indexed by CAPI id. */
	struct max_capi_dma_chan_priv **chan_privs;
};

/**
 * @struct max_capi_dma_chan_priv
 * @brief Maxim platform-specific DMA channel private data.
 */
struct max_capi_dma_chan_priv {
	/** MSDK hardware channel acquired for this CAPI channel (0..3). */
	int msdk_ch;
	/** DMA1_CH<msdk_ch>_IRQn for this channel. */
	uint32_t irq_num;
	/** Transfer completed flag. */
	volatile bool completed;
	/** Back-reference to the CAPI channel. */
	struct capi_dma_chan *capi_chan;
};

#define CAPI_DMA_HANDLE_MAXIM_INIT()				\
	(&(struct capi_dma_handle) {				\
		.ops = NULL,					\
		.init_allocated = false,			\
		.priv = &(struct max_capi_dma_priv_handle){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_DMA_PRIV_H_ */
