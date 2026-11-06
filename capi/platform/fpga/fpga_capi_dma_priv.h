/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief AXI DMAC register map and private handles for fpga_capi_dma.
 *
 * Register offsets and bits follow the hdl/library/axi_dmac register map,
 * prefixed FPGA_DMAC_.
 * The channel state keeps the detected capabilities and the in-flight
 * transfer (next addresses, bytes left, last submitted transfer id).
 */

#ifndef _FPGA_CAPI_DMA_PRIV_H_
#define _FPGA_CAPI_DMA_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include "fpga_capi_dma.h"

#define FPGA_DMAC_REG_IRQ_MASK			0x80U
#define FPGA_DMAC_REG_IRQ_PENDING		0x84U
#define FPGA_DMAC_IRQ_SOT			(1U << 0)
#define FPGA_DMAC_IRQ_EOT			(1U << 1)

#define FPGA_DMAC_REG_INTF_DESC			0x010U
#define FPGA_DMAC_DMA_BPB_DEST(x)		((x) & 0xFU)
#define FPGA_DMAC_DMA_TYPE_DEST(x)		(((x) >> 4) & 0x3U)
#define FPGA_DMAC_DMA_BPB_SRC(x)		(((x) >> 8) & 0xFU)
#define FPGA_DMAC_DMA_TYPE_SRC(x)		(((x) >> 12) & 0x3U)

#define FPGA_DMAC_REG_CTRL			0x400U
#define FPGA_DMAC_CTRL_ENABLE			(1U << 0)
#define FPGA_DMAC_CTRL_DISABLE			0U
#define FPGA_DMAC_CTRL_PAUSE			(1U << 1)

#define FPGA_DMAC_REG_TRANSFER_ID		0x404U
#define FPGA_DMAC_TRANSFER_ID_MASK		0x3U
#define FPGA_DMAC_REG_TRANSFER_SUBMIT		0x408U
#define FPGA_DMAC_TRANSFER_SUBMIT		(1U << 0)
#define FPGA_DMAC_QUEUE_FULL			(1U << 0)
#define FPGA_DMAC_REG_FLAGS			0x40CU
#define FPGA_DMAC_REG_DEST_ADDRESS		0x410U
#define FPGA_DMAC_REG_SRC_ADDRESS		0x414U
#define FPGA_DMAC_REG_X_LENGTH			0x418U
#define FPGA_DMAC_REG_Y_LENGTH			0x41CU
#define FPGA_DMAC_REG_DEST_STRIDE		0x420U
#define FPGA_DMAC_REG_SRC_STRIDE		0x424U
#define FPGA_DMAC_REG_TRANSFER_DONE		0x428U

/* FLAGS register bits. */
#define FPGA_DMAC_DMA_CYCLIC			(1U << 0)
#define FPGA_DMAC_DMA_LAST			(1U << 1)
#define FPGA_DMAC_DMA_PARTIAL_REPORTING_EN	(1U << 2)

/* Default completion timeout (ms), used when the extra config gives 0. */
#define FPGA_DMAC_DEFAULT_TIMEOUT_MS		1000U

/**
 * @brief Transfer direction detected from the hardware.
 */
enum fpga_dma_direction {
	FPGA_DMA_DIR_INVALID = 0,
	FPGA_DMA_DIR_DEV_TO_MEM = 1,	/**< stream -> memory (S2MM) */
	FPGA_DMA_DIR_MEM_TO_DEV = 2,	/**< memory -> stream (MM2S) */
	FPGA_DMA_DIR_MEM_TO_MEM = 3,	/**< memory -> memory */
};

/**
 * @brief Per-controller private handle. Lives in capi_dma_handle.priv.
 */
struct fpga_dma_priv_handle {
	/** Number of channels advertised at init. */
	uint32_t num_chans;
	/** Array of channel private-data pointers (indexed by channel id). */
	struct fpga_dma_chan_priv **chan_privs;
	/** Base address of the DMAC register block (from capi_dma_config.id). */
	uint32_t base;
	/** Completion timeout in milliseconds. */
	uint32_t timeout_ms;
	/** CAPI IRQ id of the DMAC interrupt line (valid when use_irq). */
	uint32_t irq_id;
	/** True once the IRQ line is connected+enabled. */
	bool use_irq;
};

/**
 * @brief Per-channel private data (struct axi_dmac transfer state).
 */
struct fpga_dma_chan_priv {
	/** DMAC register block base address. */
	uint32_t base;
	/** Max single-burst X_LENGTH (bytes-1), detected at init. */
	uint32_t max_length;
	/** Source data-path width in bytes, detected at init. */
	uint32_t width_src;
	/** Destination data-path width in bytes, detected at init. */
	uint32_t width_dst;
	/** FLAGS.DMA_CYCLIC is writable (hardware cyclic support). */
	bool hw_cyclic;
	/** Transfer direction, detected at init. */
	enum fpga_dma_direction direction;
	/** Cached transfer parameters (from config_xfer). */
	capi_dma_glbl_addr_t src;
	capi_dma_glbl_addr_t dst;
	size_t length;
	/** Burst bookkeeping: next addresses and bytes not yet submitted. */
	uint32_t next_src_addr;
	uint32_t next_dest_addr;
	uint32_t remaining_size;
	/** TRANSFER_ID of the last submitted burst (its TRANSFER_DONE bit). */
	uint32_t last_id;
	/** Completion flag returned by chan_is_completed. */
	volatile bool completed;
	/** Back-reference to the owning CAPI channel. */
	struct capi_dma_chan *capi_chan;
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _FPGA_CAPI_DMA_PRIV_H_ */
