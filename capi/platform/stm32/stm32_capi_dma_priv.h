/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STM32_CAPI_DMA_PRIV_H_
#define _STM32_CAPI_DMA_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "stm32_capi_dma.h"

/**
 * @brief STM32 platform specific DMA private handle
 */
struct stm32_dma_priv_handle {
	/** Number of channels */
	uint32_t num_chans;
	/** Array of channel private data pointers */
	struct stm32_dma_chan_priv **chan_privs;
};

/**
 * @brief STM32 platform specific DMA channel private data
 */
struct stm32_dma_chan_priv {
	/** DMA HAL Handle */
	DMA_HandleTypeDef hdma;
	/** Channel Number */
	uint32_t ch_num;
	/** Memory Increment */
	bool mem_increment;
	/** Peripheral Increment */
	bool per_increment;
	/** Memory Data Alignment */
	enum stm32_capi_dma_data_alignment mem_data_alignment;
	/** Peripheral Data Alignment */
	enum stm32_capi_dma_data_alignment per_data_alignment;
	/** DMA Mode */
	enum stm32_capi_dma_mode dma_mode;
	/** Trigger configuration */
	struct stm32_capi_dma_trigger *trig;
	/** Source Address for the data */
	capi_dma_glbl_addr_t src;
	/** Destination Address for the data */
	capi_dma_glbl_addr_t dst;
	/** Transfer length in Bytes */
	size_t length;
	/** Transfer completed flag */
	volatile bool completed;
	/** Back-reference to the CAPI channel */
	struct capi_dma_chan *capi_chan;
};

#define CAPI_DMA_HANDLE_STM32_INIT()                                           \
	(&(struct capi_dma_handle){                                            \
		.ops = NULL,                                                   \
		.init_allocated = false,                                       \
		.priv = &(struct stm32_dma_priv_handle){0}})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_DMA_PRIV_H_ */
