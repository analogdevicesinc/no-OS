/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_CAPI_DMA_H_
#define STM32_CAPI_DMA_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi_dma.h"

/**
 * @enum stm32_capi_dma_data_alignment
 * @brief DMA Data alignment
 */
enum stm32_capi_dma_data_alignment {
	CAPI_DMA_DATA_ALIGN_BYTE = 0,
	CAPI_DMA_DATA_ALIGN_HALF_WORD = 1,
	CAPI_DMA_DATA_ALIGN_WORD = 2
};

/**
 * @enum stm32_capi_dma_mode
 * @brief DMA Data Modes
 */
enum stm32_capi_dma_mode {
	CAPI_DMA_NORMAL_MODE = 0,
	CAPI_DMA_CIRCULAR_MODE = 1
};

/**
 * @enum stm32_capi_dma_trig_mode
 * @brief DMA Trigger Modes
 */
enum stm32_capi_dma_trig_mode {
	STM32_CAPI_DMA_BLOCK_XFER_MODE,
	STM32_CAPI_DMA_REP_BLOCK_XFER_MODE,
	STM32_CAPI_DMA_LLI_LINK_XFER_MODE,
	STM32_CAPI_DMA_SINGLE_BURST_MODE
};

/**
 * @enum stm32_capi_dma_trig_pol
 * @brief DMA Trigger Polarity
 */
enum stm32_capi_dma_trig_pol {
	STM32_CAPI_DMA_TRIG_MASKED,
	STM32_CAPI_DMA_TRIG_RISING,
	STM32_CAPI_DMA_TRIG_FALLING,
};

/**
 * @struct stm32_capi_dma_trigger
 * @brief Trigger descriptor for triggering a DMA transfer
 */
struct stm32_capi_dma_trigger {
	uint32_t id;
	enum stm32_capi_dma_trig_mode mode;
	enum stm32_capi_dma_trig_pol polarity;
};

/**
 * @struct stm32_dma_extra_config
 * @brief STM32 platform specific DMA configuration for CAPI interface
 */
struct stm32_dma_extra_config {
	/** DMA Handle - can be NULL for auto-init */
	DMA_HandleTypeDef *hdma;
	/** On F2/F4/F7: DMA_CHANNEL_x (channel selector); on others: DMA request number */
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
	/** Trigger configuration (optional) */
	struct stm32_capi_dma_trigger *trig;
};

/**
 * @struct stm32_dma_chan_extra_config
 * @brief STM32 platform specific DMA channel configuration
 */
struct stm32_dma_chan_extra_config {
	/** DMA Handle */
	DMA_HandleTypeDef *hdma;
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
	/** Trigger configuration (optional) */
	struct stm32_capi_dma_trigger *trig;
};

/**
 * @brief STM32 CAPI DMA operations structure
 */
extern const struct capi_dma_ops stm32_capi_dma_ops;

#endif /* STM32_CAPI_DMA_H_ */
