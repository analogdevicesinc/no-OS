/*******************************************************************************
 *   @file   maxim_capi_dma.h
 *   @brief  Header file for the MAX32650/MAX32690 CAPI DMA functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_DMA_H_
#define MAXIM_CAPI_DMA_H_

#include <stdbool.h>
#include <stdint.h>
#include "capi_dma.h"
#include "dma.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_dma_xfer_extra
 * @brief Platform specific DMA transfer configuration
 */
struct max_capi_dma_xfer_extra {
	/** DMA request, MXC_DMA_REQUEST_MEMTOMEM for memory to memory */
	mxc_dma_reqsel_t reqsel;
};

/**
 * @struct max_capi_dma_extra
 * @brief Platform specific DMA controller configuration
 */
struct max_capi_dma_extra {
	/** Do not install the channel interrupt handlers */
	bool no_irq;
	/** NVIC priority of the channel interrupts */
	uint32_t irq_priority;
	/** Called at the end of every channel interrupt, may be NULL */
	void (*irq_notify)(void *ctx);
	void *irq_notify_ctx;
};

extern const struct capi_dma_ops max_capi_dma_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_DMA_H_ */
