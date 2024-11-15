/***************************************************************************//**
 *   @file   stm32_dma.h
 *   @brief  Platform independent function definitions and data types
 * 	     for the DMA API.
 * 	 @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_DMA_H_
#define STM32_DMA_H_

#include "no_os_dma.h"
#include "stm32_hal.h"

/**
 * @enum stm32_dma_data_alignment
 * @brief DMA Data alignment
 */
enum stm32_dma_data_alignment {
	DATA_ALIGN_BYTE = 0,
	DATA_ALIGN_HALF_WORD = 1,
	DATA_ALIGN_WORD = 2
};

/**
 * @enum stm32_dma_mode
 * @brief DMA Data Modes
 */
enum stm32_dma_mode {
	DMA_NORMAL_MODE = 0,
	DMA_CIRCULAR_MODE = 1
};

/**
 * @struct stm32_dma_channel
 * @brief STM32 DMA Channels
 */
struct stm32_dma_channel {
	/* DMA Handle */
	DMA_HandleTypeDef *hdma;
	/* Channel Number */
	uint32_t ch_num;
	/* Memory Increment */
	bool mem_increment;
	/* Peripheral Increment */
	bool per_increment;
	/* Memory Data Alignment */
	enum stm32_dma_data_alignment mem_data_alignment;
	/* Peripheral Data Alignment */
	enum stm32_dma_data_alignment per_data_alignment;
	/* DMA Mode */
	enum stm32_dma_mode dma_mode;
	/* Source Address for the data */
	uint8_t* src;
	/* Destination Address for the data */
	uint8_t* dst;
	/* Transfer length in Bytes */
	uint32_t length;
};

struct stm32_dma_init_param {
	/* DMA Channel descriptor */
	struct stm32_dma_channel *chn;
};

struct stm32_dma_desc {
	/* DMA Channel Descriptor */
	struct stm32_dma_channel* chn;
};

extern const struct no_os_dma_platform_ops stm32_dma_ops;

#endif

