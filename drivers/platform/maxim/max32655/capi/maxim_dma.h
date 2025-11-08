/***************************************************************************//**
 *   @file   maxim_dma.h
 *   @brief  Header file of the DMA driver for the maxim platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef _MAXIM_DMA_H_
#define _MAXIM_DMA_H_

#include <stdint.h>
#include <stdbool.h>
#include "capi_dma.h"
#include "dma.h"
#include "max32655.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DMA_CHANNELS        MXC_DMA_CHANNELS  /* MAX32655 DMA channels from MSDK */
#define MAX_DMA_CONTROLLERS     1                 /* Only one DMA controller */

/* DMA register bit definitions */
#define MAX_DMA_ENABLED         (1U << 0)
#define MAX_DMA_ENABLE          (1U << 0)
#define MAX_DMA_IRQ_CTZ_EN      (1U << 31)
#define MAX_DMA_REQSEL_SHIFT    4
#define MAX_DMA_REQSEL_MASK     (0x3FU << MAX_DMA_REQSEL_SHIFT)
#define MAX_DMA_DST_INC         (1U << 22)
#define MAX_DMA_SRC_INC         (1U << 18)

#define MAX_DMA_GCR_RST_POS     0

/**
 * @struct maxim_capi_dma_chan_handle
 * @brief Private data structure for MAX32655 DMA channel handle
 */
struct maxim_capi_dma_chan_handle {
	/** Channel ID */
	uint32_t channel_id;
	/** DMA controller registers */
	mxc_dma_regs_t *dma_regs;
	/** Channel registers */
	volatile mxc_dma_ch_regs_t *ch_regs;
	/** Transfer configuration */
	struct capi_dma_transfer *current_xfer;
	/** Transfer active flag */
	bool transfer_active;
	/** Completion callback */
	void (*xfer_complete_cb)(struct capi_dma_transfer *, void *);
	/** Callback context */
	void *callback_ctx;
	/** IRQ number for this channel */
	uint32_t irq_num;
};

/**
 * @struct maxim_capi_dma_handle
 * @brief Private data structure for MAX32655 DMA controller handle
 */
struct maxim_capi_dma_handle {
	/** DMA controller ID */
	uint64_t controller_id;
	/** Number of channels */
	uint32_t num_channels;
	/** DMA controller registers */
	mxc_dma_regs_t *dma_regs;
	/** Channel handles array */
	struct maxim_capi_dma_chan_handle *channels;
	/** Initialization status */
	bool initialized;
};

/* Static allocation macro for DMA handle */
#define CAPI_DMA_HANDLE &(struct capi_dma_handle){ \
	.extra = &(struct maxim_capi_dma_handle){} \
}

/* Static allocation macro for DMA channel */
#define CAPI_DMA_CHAN_HANDLE &(struct capi_dma_chan){ \
	.extra = &(struct maxim_capi_dma_chan_handle){} \
}

/**
 * @brief Get IRQ number for DMA channel on MAX32655
 * @param controller_id DMA controller ID (unused for MAX32655)
 * @param channel_id DMA channel ID
 * @return IRQ number
 */
static inline uint32_t maxim_dma_get_irq(uint32_t controller_id, uint32_t channel_id)
{
	(void)controller_id; /* Unused for MAX32655 */
	return MXC_DMA_CH_GET_IRQ(channel_id);
}

/**
 * @brief MAX32655 DMA CAPI operations structure
 */
extern const struct capi_dma_ops maxim_dma_ops;

#ifdef __cplusplus
}
#endif

#endif /* _MAXIM_DMA_H_ */