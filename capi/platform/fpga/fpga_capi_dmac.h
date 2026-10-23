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
 *   - ops        : &fpga_capi_dmac_ops
 *   - id         : DMAC base address
 *   - num_chans  : 1 (the DMAC has one channel)
 *   - extra      : struct fpga_dmac_config * or NULL (polled)
 *
 * The direction (MEM_TO_MEM, MEM_TO_DEV or DEV_TO_MEM), the bus widths and
 * the maximum burst are read from the core at init_chan; a transfer must
 * match the detected direction. Memory addresses must be aligned to the bus
 * width, and a memory side cannot use CAPI_DMA_NO_INCREMENT.
 *
 * With use_irq the DMAC interrupt drives the transfer and long transfers are
 * split into bursts; xfer_start returns at once when a completion callback is
 * registered. A one-shot transfer of any length (up to 4 GiB) into or out of a
 * caller buffer longer than the maximum burst therefore runs as chained
 * bursts, back to back, with a single completion at the end. Polled
 * transfers are limited to one burst (-EINVAL otherwise).
 *
 * Buffers must be visible to the DMAC: on a cached CPU the project overrides
 * fpga_dmac_cache_flush / fpga_dmac_cache_invalidate (weak no-ops here).
 */

#ifndef FPGA_CAPI_DMAC_H_
#define FPGA_CAPI_DMAC_H_

#include <stddef.h>
#include <stdint.h>
#include "capi_dma.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct fpga_dmac_config
 * @brief Optional controller-level platform config, passed via
 *        capi_dma_config.extra. May be NULL (defaults are used).
 */
struct fpga_dmac_config {
	/**
	 * CAPI IRQ id of the DMAC end-of-transfer line (built with
	 * CAPI_IRQ_XILINX_GIC/INTC or the platform equivalent). Only consulted
	 * when use_irq is true.
	 */
	uint32_t irq_id;
	/**
	 * When true the DMAC interrupt drives the transfer (axi_dmac IRQ_ENABLED):
	 * the ISR resubmits bursts on start-of-transfer and marks completion on
	 * the final end-of-transfer. When false the transfer is one burst and
	 * completion is polled (axi_dmac IRQ_DISABLED). Either way xfer_start
	 * returns once the first burst is submitted.
	 * A connect/enable failure fails init; there is no silent fallback.
	 */
	bool use_irq;
};

/**
 * @struct fpga_dmac_xfer_config
 * @brief Optional per-transfer platform config, passed via
 *        capi_dma_transfer.extra. May be NULL (one-shot transfer).
 */
struct fpga_dmac_xfer_config {
	/**
	 * Cyclic replay of the buffer until xfer_abort, MEM_TO_DEV only, as
	 * axi_dmac.c does for CYCLIC transfers:
	 *   - hardware cyclic (FLAGS.DMA_CYCLIC) when the core supports it and
	 *     the buffer fits one burst;
	 *   - otherwise software cyclic (axi_dmac_mem_to_dev_isr wrap): the
	 *     buffer runs as chained bursts and the SOT interrupt rewinds to the
	 *     buffer start after the last one. Needs use_irq; a polled channel
	 *     returns -ENOTSUP from config_xfer.
	 * xfer_start returns once submitted. The channel never reports
	 * completed until aborted. Per-pass complete callback: software cyclic
	 * only (once per pass, after the pass's last burst is done). Hardware
	 * cyclic raises no end-of-transfer interrupt (axi_dmac "Cyclic
	 * Transfers"), so it never calls back.
	 */
	bool cyclic;
	/**
	 * 2D transfer (axi_dmac DMA_2D_TRANSFER, Linux prep_interleaved): rows
	 * of capi_dma_transfer.length bytes each. 0 or 1 => 1D. Runs as one
	 * hardware submit, so length and rows must each fit one burst
	 * (fpga_dmac_get_max_burst). Needs FPGA_DMAC_FEAT_2D, else -ENOTSUP;
	 * not with cyclic.
	 */
	uint32_t rows;
	/** 2D: bytes between row starts in source memory (>= length). */
	uint32_t src_stride;
	/** 2D: bytes between row starts in destination memory (>= length). */
	uint32_t dst_stride;
};

/**
 * @struct fpga_dmac_sg_desc
 * @brief One hardware scatter-gather descriptor (axi_dmac DMA_SG_TRANSFER).
 *
 * Layout read by dmac_sg.v (six 64-bit beats) and used by Linux
 * dma-axi-dmac.c (struct axi_dmac_hw_desc); padded to 64 bytes so each
 * descriptor owns its cache line. The application supplies an array of
 * capi_dma_sg_config.num_blocks of these in capi_dma_sg_config.extra (array
 * mode only); capi_dma_config_scattergather_xfer() fills one per block, with
 * capi_dma_transfer.extra optionally a struct fpga_dmac_xfer_config for a 2D
 * block. The chain runs as a single transfer: one completion at its end.
 */
struct fpga_dmac_sg_desc {
	uint32_t flags;
	uint32_t id;
	uint64_t dest_addr;
	uint64_t src_addr;
	uint64_t next_sg_addr;
	uint32_t y_len;
	uint32_t x_len;
	uint32_t src_stride;
	uint32_t dst_stride;
	uint64_t pad[2];
} __attribute__((aligned(64)));

/** Hardware cyclic (FLAGS.DMA_CYCLIC writable). */
#define FPGA_DMAC_FEAT_CYCLIC	(1U << 0)
/** 2D transfers (Y_LENGTH writable). */
#define FPGA_DMAC_FEAT_2D	(1U << 1)
/** Hardware scatter-gather (SG_ADDRESS writable). */
#define FPGA_DMAC_FEAT_SG	(1U << 2)

/**
 * @brief Cache maintenance hooks for DMA buffers in coherent memory.
 *
 * Defined as weak no-ops here so the backend stays vendor-neutral. On a cached
 * baremetal target (e.g. Zynq PS DDR) the project must override these with the
 * BSP calls (Xil_DCacheFlushRange / Xil_DCacheInvalidateRange) so the CPU view
 * and the DMA view of the buffers agree. The default no-ops are correct only
 * when the DMA buffers live in uncached/coherent memory.
 *
 * fpga_dmac_cache_flush is called on the source (and destination) region before
 * the transfer; fpga_dmac_cache_invalidate is called on the destination region
 * after the transfer completes.
 */
void fpga_dmac_cache_flush(uintptr_t addr, size_t len);
void fpga_dmac_cache_invalidate(uintptr_t addr, size_t len);

/**
 * @brief Read the DMAC bus widths detected at init_chan.
 *
 * Bytes per beat of the source and destination interfaces, from INTF_DESC
 * (axi_dmac.c axi_dmac_init: width_src / width_dst). Memory addresses of a
 * transfer must be aligned to the memory side's width.
 * @param chan - Channel opened on fpga_capi_dmac_ops.
 * @param src_bytes - Output: source width in bytes (may be NULL).
 * @param dst_bytes - Output: destination width in bytes (may be NULL).
 * @return 0 on success, -EINVAL if chan is NULL or not an AXI DMAC channel.
 */
int fpga_dmac_get_width(const struct capi_dma_chan *chan, uint32_t *src_bytes,
			uint32_t *dst_bytes);

/**
 * @brief Read the longest single burst the DMAC accepts.
 *
 * X_LENGTH all-ones read-back + 1 (axi_dmac.c axi_dmac_init: max_length).
 * Longer transfers are chained (IRQ mode), and a cyclic buffer longer than
 * this runs as software cyclic.
 * @param chan - Channel opened on fpga_capi_dmac_ops.
 * @param bytes - Output: maximum burst length in bytes.
 * @return 0 on success, -EINVAL on bad arguments or a non-AXI DMAC channel.
 */
int fpga_dmac_get_max_burst(const struct capi_dma_chan *chan, uint32_t *bytes);

/**
 * @brief Read the optional features detected at init_chan.
 *
 * Register write/read-back probes as Linux dma-axi-dmac.c axi_dmac_detect_caps
 * (FLAGS, Y_LENGTH, SG_ADDRESS).
 * @param chan - Channel opened on fpga_capi_dmac_ops.
 * @param mask - Output: FPGA_DMAC_FEAT_* bits.
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_dmac_get_features(const struct capi_dma_chan *chan, uint32_t *mask);

/**
 * @brief Wait for the channel's one-shot transfer to finish.
 *
 * axi_dmac_transfer_wait_completion: xfer_start and start_scattergather_xfer
 * return once the transfer is submitted; this waits for its completion (the
 * ISR in IRQ mode, TRANSFER_DONE of the submitted id when polled). Polls
 * every 1 us. A timeout leaves the transfer running; abort it with
 * capi_dma_xfer_abort().
 * @param chan - Channel opened on fpga_capi_dmac_ops.
 * @param timeout_ms - Time to wait in ms; 0 waits forever.
 * @return 0 when completed (or idle), or:
 *         -EINVAL     bad arguments, or a cyclic transfer (never completes)
 *         -ETIMEDOUT  not completed within timeout_ms
 */
int fpga_dmac_wait_completion(struct capi_dma_chan *chan, uint32_t timeout_ms);

/**
 * @brief CAPI DMA operations table for the ADI AXI DMAC.
 */
extern const struct capi_dma_ops fpga_capi_dmac_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* FPGA_CAPI_DMAC_H_ */
