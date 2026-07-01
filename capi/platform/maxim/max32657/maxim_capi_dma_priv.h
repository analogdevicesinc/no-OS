/*******************************************************************************
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

#include "maxim_capi_dma.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_dma_priv
 * @brief DMA private structure
 */
struct max_capi_dma_priv {
	/** Identifier */
	uint64_t id;
	/** Array of pointers to channels */
	struct capi_dma_chan **channels;
	/** Number of channels storage */
	uint8_t num_channels;
	/** Whether channel IRQs are connected/enabled */
	bool use_irq;
};

/**
 * @struct max_capi_dma_ch_priv
 * @brief DMA channel private structure
 */
struct max_capi_dma_ch_priv {
	/* MXC_DMA_AcquireChannel returns a channel ID and is stored here */
	uint32_t hw_channel_id;
	/* DMA completion flag */
	volatile bool completed;
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
