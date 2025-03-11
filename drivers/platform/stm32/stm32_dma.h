/***************************************************************************//**
 *   @file   stm32_dma.h
 *   @brief  Platform independent function definitions and data types
 * 	     for the DMA API.
 * 	 @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
 * @enum stm32_dma_trig_mode
 * @brief DMA Trigger Modes
 */
enum stm32_dma_trig_mode {
	/** A Block Transfer is conditioned by trigger */
	STM32_DMA_BLOCK_XFER_MODE,
	/** A Repeated Block Transfer is conditioned by trigger */
	STM32_DMA_REP_BLOCK_XFER_MODE,
	/** A LLI Link Transfer is conditioned by trigger */
	STM32_DMA_LLI_LINK_XFER_MODE,
	/** A Single/Burst Transfer is conditioned by trigger */
	STM32_DMA_SINGLE_BURST_MODE
};

/**
 * @enum stm32_dma_trig_pol
 * @brief DMA Trigger Polarity
 */
enum stm32_dma_trig_pol {
	STM32_DMA_TRIG_MASKED,
	STM32_DMA_TRIG_RISING,
	STM32_DMA_TRIG_FALLING,
};

/**
 * @struct stm32_dma_trigger
 * @brief Trigger descriptor for triggering a DMA transfer
 */
struct stm32_dma_trigger {
	/** Trigger ID */
	uint32_t id;
	/** Mode of trigger */
	enum stm32_dma_trig_mode mode;
	/** Polarity of trigger */
	enum stm32_dma_trig_pol polarity;
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
	/* Trigger configuration */
	struct stm32_dma_trigger *trig;
	/* Source Address for the data */
	uint8_t* src;
	/* Destination Address for the data */
	uint8_t* dst;
	/* Transfer length in Bytes */
	uint32_t length;
	/* Private Channel Data */
	void *priv_data;
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
extern const struct no_os_dma_platform_ops stm32_gpdma_ops;

#endif

