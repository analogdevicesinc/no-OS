/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief CAPI DMA backend for the ADI AXI DMAC core (axi_dmac).
 *
 * Runs transfers on an ADI AXI DMAC instance using only CAPI services;
 * registers are accessed through CAPI_PLATFORM_IO_*. The core is packaged for
 * both Xilinx and Intel fabric.
 *
 * Configuration (struct capi_dma_config):
 *   - ops        : &fpga_capi_dma_ops
 *   - id         : DMAC base address
 *   - num_chans  : 1 (the DMAC has one channel)
 *   - extra      : struct fpga_dma_extra_config * or NULL (polled, 1 s
 *                  timeout)
 *
 * The direction (MEM_TO_MEM, MEM_TO_DEV or DEV_TO_MEM), the bus widths and
 * the maximum burst are read from the core at init_chan; a transfer must
 * match the detected direction. Memory addresses must be aligned to the bus
 * width, and a memory side cannot use CAPI_DMA_NO_INCREMENT.
 *
 * With use_irq the DMAC interrupt drives the transfer and long transfers are
 * split into bursts; xfer_start returns at once when a completion callback is
 * registered. Polled transfers are limited to one burst.
 *
 * Buffers must be visible to the DMAC: on a cached CPU the project overrides
 * fpga_dma_cache_flush / fpga_dma_cache_invalidate (weak no-ops here).
 */

#ifndef FPGA_CAPI_DMA_H_
#define FPGA_CAPI_DMA_H_

#include <stddef.h>
#include <stdint.h>
#include "capi_dma.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct fpga_dma_extra_config
 * @brief Optional controller-level platform config, passed via
 *        capi_dma_config.extra. May be NULL (defaults are used).
 */
struct fpga_dma_extra_config {
	/** Polled-completion timeout in milliseconds. 0 => FPGA_DMA_DEFAULT_TIMEOUT_MS. */
	uint32_t timeout_ms;
	/**
	 * CAPI IRQ id of the DMAC end-of-transfer line (built with
	 * CAPI_IRQ_XILINX_GIC/INTC or the platform equivalent). Only consulted
	 * when use_irq is true.
	 */
	uint32_t irq_id;
	/**
	 * When true the DMAC interrupt drives the transfer (axi_dmac IRQ_ENABLED):
	 * the ISR resubmits bursts on start-of-transfer and marks completion on
	 * the final end-of-transfer. xfer_start returns immediately when a
	 * complete callback is registered, otherwise it waits for completion.
	 * When false, xfer_start polls for completion (axi_dmac IRQ_DISABLED).
	 * A connect/enable failure fails init; there is no silent fallback.
	 */
	bool use_irq;
};

/**
 * @struct fpga_dma_xfer_extra_config
 * @brief Optional per-transfer platform config, passed via
 *        capi_dma_transfer.extra. Reserved for future use (e.g. cyclic); the
 *        backend currently ignores it. A named field keeps the struct portable
 *        (no zero-size struct extension).
 */
struct fpga_dma_xfer_extra_config {
	uint32_t reserved;
};

/**
 * @brief Cache maintenance hooks for DMA buffers in coherent memory.
 *
 * Defined as weak no-ops here so the backend stays vendor-neutral. On a cached
 * baremetal target (e.g. Zynq PS DDR) the project must override these with the
 * BSP calls (Xil_DCacheFlushRange / Xil_DCacheInvalidateRange) so the CPU view
 * and the DMA view of the buffers agree. The default no-ops are correct only
 * when the DMA buffers live in uncached/coherent memory.
 *
 * fpga_dma_cache_flush is called on the source (and destination) region before
 * the transfer; fpga_dma_cache_invalidate is called on the destination region
 * after the transfer completes.
 */
void fpga_dma_cache_flush(uintptr_t addr, size_t len);
void fpga_dma_cache_invalidate(uintptr_t addr, size_t len);

/**
 * @brief CAPI DMA operations table for the ADI AXI DMAC.
 */
extern const struct capi_dma_ops fpga_capi_dma_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* FPGA_CAPI_DMA_H_ */
