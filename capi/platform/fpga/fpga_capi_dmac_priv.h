/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief AXI DMAC register map and private handles for fpga_capi_dmac.
 *
 * Register offsets and bits follow the hdl/library/axi_dmac register map,
 * prefixed FPGA_DMAC_.
 * The channel state keeps the detected capabilities and the in-flight
 * transfer (next addresses, bytes left, last submitted transfer id).
 */

#ifndef FPGA_CAPI_DMAC_PRIV_H_
#define FPGA_CAPI_DMAC_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include "fpga_capi_dmac.h"

/* IRQ registers (axi_dmac_regmap.v); bits shared by MASK/PENDING. */
#define FPGA_DMAC_REG_IRQ_MASK			0x80U /* IRQ_MASK, 1 = masked */
#define FPGA_DMAC_REG_IRQ_PENDING		0x84U /* IRQ_PENDING, W1C */
#define FPGA_DMAC_IRQ_SOT			(1U << 0) /* up_sot */
#define FPGA_DMAC_IRQ_EOT			(1U << 1) /* up_eot */

/* Interface description (axi_dmac_regmap.v word 0x004) decode. */
#define FPGA_DMAC_REG_INTF_DESC			0x010U /* read-only */
/* [3:0] log2 dest bytes per beat */
#define FPGA_DMAC_DMA_BPB_DEST(x)		((x) & 0xFU)
/* [5:4] DMA_TYPE_DEST */
#define FPGA_DMAC_DMA_TYPE_DEST(x)		(((x) >> 4) & 0x3U)
/* [11:8] log2 src bytes per beat */
#define FPGA_DMAC_DMA_BPB_SRC(x)		(((x) >> 8) & 0xFU)
/* [13:12] DMA_TYPE_SRC */
#define FPGA_DMAC_DMA_TYPE_SRC(x)		(((x) >> 12) & 0x3U)

/* CONTROL register, word 0x100 (axi_dmac_regmap.v). */
#define FPGA_DMAC_REG_CTRL			0x400U /* CONTROL */
#define FPGA_DMAC_CTRL_ENABLE			(1U << 0) /* ctrl_enable */
#define FPGA_DMAC_CTRL_DISABLE			0U /* all CONTROL bits clear */
#define FPGA_DMAC_CTRL_PAUSE			(1U << 1) /* ctrl_pause */
#define FPGA_DMAC_CTRL_HWDESC			(1U << 2) /* ctrl_hwdesc */

/* Transfer registers, words 0x101-0x11f (axi_dmac_regmap_request.v). */
#define FPGA_DMAC_REG_TRANSFER_ID		0x404U /* TRANSFER_ID */
#define FPGA_DMAC_TRANSFER_ID_MASK		0x3U /* 2-bit id */
#define FPGA_DMAC_REG_TRANSFER_SUBMIT		0x408U /* TRANSFER_SUBMIT */
#define FPGA_DMAC_TRANSFER_SUBMIT		(1U << 0) /* W: queue */
#define FPGA_DMAC_QUEUE_FULL			(1U << 0) /* R: req pending */
#define FPGA_DMAC_REG_FLAGS			0x40CU /* FLAGS */
#define FPGA_DMAC_REG_DEST_ADDRESS		0x410U /* DEST_ADDRESS */
#define FPGA_DMAC_REG_SRC_ADDRESS		0x414U /* SRC_ADDRESS */
#define FPGA_DMAC_REG_X_LENGTH			0x418U /* X_LENGTH: bytes - 1 */
#define FPGA_DMAC_REG_Y_LENGTH			0x41CU /* Y_LENGTH: rows - 1 */
#define FPGA_DMAC_REG_DEST_STRIDE		0x420U /* DEST_STRIDE: bytes */
#define FPGA_DMAC_REG_SRC_STRIDE		0x424U /* SRC_STRIDE: bytes */
#define FPGA_DMAC_REG_TRANSFER_DONE		0x428U /* TRANSFER_DONE */
#define FPGA_DMAC_REG_SG_ADDRESS		0x47CU /* SG_ADDRESS */
/* Debug status (axi_dmac_reset_manager dbg_status); reads 0 when the core is
 * built with DISABLE_DEBUG_REGISTERS. Bits [2:0] = reset-manager state. */
#define FPGA_DMAC_REG_DBG_STATUS		0x43CU
#define FPGA_DMAC_DBG_STATE_MASK		0x7U /* state, bits [2:0] */
#define FPGA_DMAC_DBG_STATE_SHUTDOWN		0x5U /* STATE_SHUTDOWN (3'h5) */
/* Bound on the post-disable drain wait in xfer_abort. */
#define FPGA_DMAC_ABORT_DRAIN_US		10000U

/* fpga_dmac_sg_desc.flags (dmac_sg.v MASK_LAST_HWDESC / MASK_EOT_IRQ). */
#define FPGA_DMAC_SG_FLAG_LAST			(1U << 0) /* MASK_LAST_HWDESC */
#define FPGA_DMAC_SG_FLAG_IRQ			(1U << 1) /* MASK_EOT_IRQ */

/* FLAGS register bits. */
#define FPGA_DMAC_DMA_CYCLIC			(1U << 0) /* up_dma_cyclic */
#define FPGA_DMAC_DMA_LAST			(1U << 1) /* up_dma_last */
/* up_dma_enable_tlen_reporting (transfer length FIFO) */
#define FPGA_DMAC_DMA_PARTIAL_REPORTING_EN	(1U << 2)

/* Outstanding burst ids tracked in software cyclic (TRANSFER_ID is 2 bit). */
#define FPGA_DMAC_PEND_MAX			4U

/* Default completion timeout (ms), used when the extra config gives 0. */

/**
 * @brief Transfer direction detected from the hardware.
 */
enum fpga_dmac_direction {
	FPGA_DMAC_DIR_INVALID = 0,
	FPGA_DMAC_DIR_DEV_TO_MEM = 1,	/**< stream -> memory (S2MM) */
	FPGA_DMAC_DIR_MEM_TO_DEV = 2,	/**< memory -> stream (MM2S) */
	FPGA_DMAC_DIR_MEM_TO_MEM = 3,	/**< memory -> memory */
};

/**
 * @brief Per-controller private handle. Lives in capi_dma_handle.priv.
 */
struct fpga_dmac_priv {
	/** Number of channels advertised at init. */
	uint32_t num_chans;
	/** Array of channel private-data pointers (indexed by channel id). */
	struct fpga_dmac_chan_priv **chan_privs;
	/** Base address of the DMAC register block (from capi_dma_config.id). */
	uint32_t base;
	/** CAPI IRQ id of the DMAC interrupt line (valid when use_irq). */
	uint32_t irq_id;
	/** True once the IRQ line is connected+enabled. */
	bool use_irq;
};

/**
 * @brief Per-channel private data (struct axi_dmac transfer state).
 */
struct fpga_dmac_chan_priv {
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
	/** Y_LENGTH is writable (DMA_2D_TRANSFER). */
	bool hw_2d;
	/** SG_ADDRESS is writable (DMA_SG_TRANSFER). */
	bool hw_sg;
	/** Transfer direction, detected at init. */
	enum fpga_dmac_direction direction;
	/** Cached transfer parameters (from config_xfer). */
	capi_dma_glbl_addr_t src;
	capi_dma_glbl_addr_t dst;
	size_t length;
	/** 2D: rows (0 => 1D) and row strides of the current transfer. */
	uint32_t rows;
	uint32_t src_stride;
	uint32_t dst_stride;
	/** Scatter-gather chain of the current transfer (NULL => not SG). */
	struct fpga_dmac_sg_desc *sg_desc;
	uint32_t sg_num;
	/** Burst bookkeeping: next addresses and bytes not yet submitted. */
	uint32_t next_src_addr;
	uint32_t next_dest_addr;
	uint32_t remaining_size;
	/** TRANSFER_ID of the last submitted burst (its TRANSFER_DONE bit). */
	uint32_t last_id;
	/** Current transfer is hardware cyclic (fpga_dmac_xfer_config). */
	bool cyclic;
	/**
	 * Current transfer is software cyclic: cyclic requested but the buffer
	 * does not fit one burst (or the core has no FLAGS.DMA_CYCLIC); the SOT
	 * interrupt rewinds to the buffer start (axi_dmac_mem_to_dev_isr).
	 */
	bool sw_cyclic;
	/** Software cyclic: ids of submitted, not yet retired bursts (FIFO). */
	uint8_t pend_id[FPGA_DMAC_PEND_MAX];
	/** Software cyclic: matching pend_id entry is the last burst of a pass. */
	bool pend_end[FPGA_DMAC_PEND_MAX];
	/** Software cyclic: index of the oldest pend_id entry. */
	uint8_t pend_head;
	/** Software cyclic: number of valid pend_id entries. */
	uint8_t pend_count;
	/** Completion flag returned by chan_is_completed. */
	volatile bool completed;
	/** Back-reference to the owning CAPI channel. */
	struct capi_dma_chan *capi_chan;
};

/**
 * @brief Static handle initializer for apps that avoid malloc.
 */
#define CAPI_DMA_HANDLE_FPGA_INIT()			\
	(&(struct capi_dma_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct fpga_dmac_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* FPGA_CAPI_DMAC_PRIV_H_ */
