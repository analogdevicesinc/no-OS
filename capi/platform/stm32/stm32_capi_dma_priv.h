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

/** Maximum number of channels per DMA handle (channel IDs 0 .. N-1) */
#define STM32_CAPI_DMA_MAX_CHANNELS	16

/*
 * Register-level kick and ISR for the DMA_Channel_TypeDef IP (CCR/CNDTR/CPAR/
 * CMAR, flags in ISR/IFCR). Other families use the HAL_DMA_Start_IT() path.
 */
#if defined(STM32L4)
#define STM32_CAPI_DMA_FAST_CHANNEL	1
#endif

/**
 * @brief STM32 platform specific DMA private handle
 */
struct stm32_dma_priv_handle {
	/** Number of channels */
	uint32_t num_chans;
	/** Channel private data, indexed by channel ID */
	struct stm32_dma_chan_priv *chan_privs[STM32_CAPI_DMA_MAX_CHANNELS];
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
	/** Channel extra config last applied, so a repeated one is not recopied */
	const struct stm32_dma_chan_extra_config *applied_extra;
	/** HAL_DMA_Init() has run for the current hdma.Init */
	bool hw_ready;
	/** Transfer shape hdma.Init was built from, to skip rebuilding it */
	bool cfg_valid;
	enum capi_dma_xfer_type cfg_type;
	enum capi_dma_addr_inc cfg_src_inc;
	enum capi_dma_addr_inc cfg_dst_inc;
	/** Channel storage, used when the channel is not owned by the app */
	struct capi_dma_chan chan_storage;
};

#define CAPI_DMA_HANDLE_STM32_INIT() \
	(&(struct capi_dma_handle){ \
		.ops = NULL, \
		.init_allocated = false, \
		.priv = &(struct stm32_dma_priv_handle){0}})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_DMA_PRIV_H_ */
