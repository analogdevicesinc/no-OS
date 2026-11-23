/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief SPI Engine register map, command encoding and private handle for
 *        fpga_capi_spi_engine.
 *
 * Register offsets, the offload registers and the instruction encoding
 * (TRANSFER, ASSERT, CONFIG, MISC: SYNC / SLEEP) follow hdl/library/spi_engine,
 * prefixed FPGA_SPI_ENGINE_.
 */

#ifndef FPGA_CAPI_SPI_ENGINE_PRIV_H_
#define FPGA_CAPI_SPI_ENGINE_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>
#include <capi_dma.h>
#include "fpga_capi_spi_engine.h"
#include "fpga_capi_dmac.h"

/* --- spi_engine_private.h register map, FPGA_ prefixed --- */
#define FPGA_SPI_ENGINE_REG_VERSION		0x00U
/* word 0x03: NUM_OF_SDIO, DATA_WIDTH. */
#define FPGA_SPI_ENGINE_REG_DATA_WIDTH		0x0CU
/* axi_spi_engine word 0x04. */
#define FPGA_SPI_ENGINE_REG_OFFLOAD_MEM_ADDR_WIDTH	0x10U
/* word 0x05: sdi/sdo/sync/cmd AW. */
#define FPGA_SPI_ENGINE_REG_FIFO_ADDR_WIDTH	0x14U
/* up_sw_reset (word 0x10). */
#define FPGA_SPI_ENGINE_REG_RESET		0x40U
/* up_irq_mask (word 0x20). */
#define FPGA_SPI_ENGINE_REG_INT_ENABLE		0x80U
/* up_irq_pending (word 0x21), W1C. */
#define FPGA_SPI_ENGINE_REG_INT_PENDING		0x84U
/* up_irq_source (word 0x22). */
#define FPGA_SPI_ENGINE_REG_INT_SOURCE		0x88U
#define FPGA_SPI_ENGINE_REG_SYNC_ID		0xC0U /* sync_id (word 0x30) */
/* offload_sync_id (word 0x31). */
#define FPGA_SPI_ENGINE_REG_OFFLOAD_SYNC_ID	0xC4U
/* cmd_fifo_room (word 0x34). */
#define FPGA_SPI_ENGINE_REG_CMD_FIFO_ROOM	0xD0U
/* sdo_fifo_room (word 0x35). */
#define FPGA_SPI_ENGINE_REG_SDO_FIFO_ROOM	0xD4U
/* sdi_level (word 0x36). */
#define FPGA_SPI_ENGINE_REG_SDI_FIFO_LEVEL	0xD8U
/* CMD FIFO write (word 0x38). */
#define FPGA_SPI_ENGINE_REG_CMD_FIFO		0xE0U
/* SDO FIFO write (word 0x39). */
#define FPGA_SPI_ENGINE_REG_SDO_DATA_FIFO	0xE4U
/* SDI FIFO pop (word 0x3a). */
#define FPGA_SPI_ENGINE_REG_SDI_DATA_FIFO	0xE8U
/* SDI FIFO peek, read without pop (word 0x3c). */
#define FPGA_SPI_ENGINE_REG_SDI_DATA_FIFO_PEEK	0xF0U

/* DATA_WIDTH reg NUM_OF_SDIO [23:16]. */
#define FPGA_SPI_ENGINE_REG_NUM_OF_SDI_MSK	0x00FF0000U
/** Chip selects the 8-bit CS instruction mask addresses. */
#define FPGA_SPI_ENGINE_NUM_CS				8U
/* DATA_WIDTH reg DATA_WIDTH [15:0], bits. */
#define FPGA_SPI_ENGINE_REG_DATA_WIDTH_MSK	0x0000FFFFU
/* word width in bits from DATA_WIDTH reg. */
#define FPGA_SPI_ENGINE_DATA_WIDTH(x)		((x) & FPGA_SPI_ENGINE_REG_DATA_WIDTH_MSK)
/* OFFLOAD_MEM_ADDR_WIDTH: [7:0] command memory, [15:8] SDO memory (log2). */
#define FPGA_SPI_ENGINE_OFFLOAD_CMD_MEM_AW(x)	((x) & 0xFFU)
/* offload_sdo_mem_address_width. */
#define FPGA_SPI_ENGINE_OFFLOAD_SDO_MEM_AW(x)	(((x) >> 8) & 0xFFU)
/* Pre-1.x cores do not have the register (reads 0): HDL default width. */
#define FPGA_SPI_ENGINE_OFFLOAD_MEM_AW_DEFAULT	4U

/* execution CMD_TRANSFER. */
#define FPGA_SPI_ENGINE_INST_TRANSFER		0x00U
/* execution CMD_CHIPSELECT. */
#define FPGA_SPI_ENGINE_INST_ASSERT		0x01U
#define FPGA_SPI_ENGINE_INST_CONFIG		0x02U /* execution CMD_WRITE */
#define FPGA_SPI_ENGINE_INST_SYNC_SLEEP		0x03U /* execution CMD_MISC */
#define FPGA_SPI_ENGINE_INST_MISC		0x03U /* execution CMD_MISC */
/* REG_CLK_DIV: prescaler div. */
#define FPGA_SPI_ENGINE_CMD_REG_CLK_DIV		0x00U
/* REG_CONFIG: CPHA/CPOL/3-wire/SDO idle. */
#define FPGA_SPI_ENGINE_CMD_REG_CONFIG		0x01U
/* REG_WORD_LENGTH, bits. */
#define FPGA_SPI_ENGINE_CMD_DATA_TRANSFER_LEN	0x02U
#define FPGA_SPI_ENGINE_MISC_SYNC		0x00U /* MISC_SYNC (cmd[8]) */
#define FPGA_SPI_ENGINE_MISC_SLEEP		0x01U /* MISC_SLEEP (cmd[8]) */
/* SYNC id marker (driver convention). */
#define FPGA_SPI_ENGINE_SYNC_TRANSFER_BEGIN	(1U << 1)
/* SYNC id marker (driver convention). */
#define FPGA_SPI_ENGINE_SYNC_TRANSFER_END	(1U << 2)
/* up_cmd_fifo_almost_empty. */
#define FPGA_SPI_ENGINE_INT_CMD_ALMOST_EMPTY	(1U << 0)
/* up_sdo_fifo_almost_empty. */
#define FPGA_SPI_ENGINE_INT_SDO_ALMOST_EMPTY	(1U << 1)
/* up_sdi_fifo_almost_full. */
#define FPGA_SPI_ENGINE_INT_SDI_ALMOST_FULL	(1U << 2)
#define FPGA_SPI_ENGINE_INT_SYNC		(1U << 3) /* sync_id_pending */
/* offload0_enable_reg. */
#define FPGA_SPI_ENGINE_OFFLOAD_CTRL_ENABLE	(1U << 0)
/* offload0_enabled_s. */
#define FPGA_SPI_ENGINE_OFFLOAD_STATUS_ENABLED	(1U << 0)
#define FPGA_SPI_ENGINE_CONFIG_CPHA		(1U << 0) /* cpha */
#define FPGA_SPI_ENGINE_CONFIG_CPOL		(1U << 1) /* cpol */
#define FPGA_SPI_ENGINE_CONFIG_3WIRE		(1U << 2) /* three_wire */
#define FPGA_SPI_ENGINE_CONFIG_SDO_IDLE		(1U << 3) /* sdo_idle_state */
/* VERSION [23:16]. */
#define FPGA_SPI_ENGINE_VERSION_MAJOR(x)	(((x) >> 16) & 0xFFU)
/* VERSION [15:8]. */
#define FPGA_SPI_ENGINE_VERSION_MINOR(x)	(((x) >> 8) & 0xFFU)
/* VERSION [7:0]. */
#define FPGA_SPI_ENGINE_VERSION_PATCH(x)	((x) & 0xFFU)

#define FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_W	0x01U /* TRANSFER w bit */
#define FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_R	0x02U /* TRANSFER r bit */
#define FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_RW	0x03U /* TRANSFER r + w */

/* offload0 word 0x40; 0x20 stride. */
#define FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(x)	(0x100U + (0x20U * (x)))
/* offload0 word 0x41. */
#define FPGA_SPI_ENGINE_REG_OFFLOAD_STATUS(x)	(0x104U + (0x20U * (x)))
/* offload0_mem_reset (word 0x42). */
#define FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(x)	(0x108U + (0x20U * (x)))
/* offload0 CMD mem write (word 0x44). */
#define FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(x)	(0x110U + (0x20U * (x)))
/* offload0 SDO mem write (word 0x45). */
#define FPGA_SPI_ENGINE_REG_OFFLOAD_SDO_MEM(x)	(0x114U + (0x20U * (x)))

/* FPGA_SPI_ENGINE_CMD, _ASSERT and _SLEEP come from fpga_capi_spi_engine.h. */
#define FPGA_SPI_ENGINE_CMD_TRANSFER(readwrite, n)			\
	FPGA_SPI_ENGINE_CMD(FPGA_SPI_ENGINE_INST_TRANSFER, (readwrite), (n))
/* Configuration write: reg cmd[10:8], value cmd[7:0]. */
#define FPGA_SPI_ENGINE_CMD_CONFIG(reg, val)				\
	FPGA_SPI_ENGINE_CMD(FPGA_SPI_ENGINE_INST_CONFIG, (reg), (val))
/* SYNC with event id (MISC, cmd[8] = 0). */
#define FPGA_SPI_ENGINE_CMD_SYNC(id)					\
	FPGA_SPI_ENGINE_CMD(FPGA_SPI_ENGINE_INST_MISC,			\
			    FPGA_SPI_ENGINE_MISC_SYNC, (id))

/** Bounded FIFO / SYNC poll (-ETIMEDOUT when exceeded). */
#define FPGA_SPI_ENGINE_SPIN_MAX		10000000U
/** Reset pulse width. */
#define FPGA_SPI_ENGINE_RESET_US		1000U
/** Commands in one offload program: CLK_DIV, XFER_LEN, CONFIG, 3x ASSERT,
 *  TRANSFER, SYNC. */
#define FPGA_SPI_ENGINE_OFFLOAD_PROGRAM_LEN	8U
/** Bounded wait for a stopped offload to go idle before the core is reset
 *  (also polled from the DMA completion callback, so kept short). */
#define FPGA_SPI_ENGINE_STOP_SPINS		10000U

/**
 * @brief Largest byte count a single transfer will frame. The self-test's
 *        biggest SPI buffer is SPI_ABORT_SIZE (128 bytes); round up for headroom
 *        and keep the DMA sample buffer statically owned + cache-line aligned.
 */
#define FPGA_SPI_ENGINE_MAX_XFER_BYTES		256U
/** Largest offload program (OFFLOAD_MEM_ADDR_WIDTH 8: 256 CMD entries). */
#define FPGA_SPI_ENGINE_MAX_PROG		256U
/** Offload RX wait when the caller passes 0 (spi_engine_offload_transfer
 *  axi_dmac_transfer_wait_completion(..., 500)). */
#define FPGA_SPI_ENGINE_CAPTURE_TIMEOUT_US	500000U
/* Sample-buffer alignment of a non-final async chunk (covers 128-bit DMAs). */
#define FPGA_SPI_ENGINE_CHUNK_ALIGN		16U

/**
 * @brief Per-controller private handle. Lives in capi_spi_controller_handle.priv.
 *
 * One SPI Engine instance backs one CAPI SPI controller. The SPI Engine's own
 * register state is register-derived; the clock, trigger and offload-DMA are
 * composed via the referenced peer handles.
 */
struct fpga_spi_engine_priv {
	/** SPI Engine AXI register-map base address. */
	uint32_t regmap_base;

	/** SCLK clock generator handle (capi_clk), or NULL if SCLK already runs. */
	struct capi_clk_handle *clkgen;
	/** Offload trigger timer handle (capi_timer, PWM mode), or NULL if none. */
	struct capi_timer_handle *trigger;
	/** Timer channel wired to the engine's trigger input. */
	uint32_t trigger_chan;
	/** Offload sample-capture S2MM DMA channel (capi_dma), or NULL if none. */
	struct capi_dma_chan *offload_dma_chan;

	/** SPI Engine module reference clock in Hz. */
	uint32_t ref_clk_hz;
	/** Word width in bits (from config). */
	uint8_t data_width;
	/** DATA_WIDTH register [15:0]: widest word the core supports. */
	uint8_t max_data_width;
	/** Bytes per captured offload sample in sample_buf (core DATA_WIDTH). */
	uint8_t sample_len;
	/** ASSERT command delay field (0..3), spi_engine cs_delay. */
	uint8_t cs_delay;
	/** DATA_WIDTH register [23:16]: SDI lanes. */
	uint8_t num_sdi;
	/** CONFIG 3WIRE bit, from capi_spi_config.three_pin_mode. */
	bool three_wire;
	/** FPGA_SPI_ENGINE_FLAG_* options of the transfer being built. */
	uint32_t xfer_flags;
	/** FPGA_SPI_ENGINE_OFFLOAD_* directions from the config. */
	uint8_t offload_config;
	/** Offload capture (fpga_spi_engine_offload_capture) state. */
	bool capture_active;
	/** A capture left the offload (trigger, TX stream) running. */
	bool offload_running;
	volatile bool capture_rx_done;
	volatile uint32_t capture_event;
	/** TX DMA channel of a running capture / stream, or NULL. */
	struct capi_dma_chan *stream_tx_chan;
	/** Persistent TX DMA descriptor for the capture. */
	struct capi_dma_transfer dma_tx_xfer;
	/** Hardware-cyclic TX transfer option (default TX, legacy CYCLIC). */
	struct fpga_dmac_xfer_config tx_cyclic_cfg;
	/** VERSION register read at init. */
	uint32_t version;
	/** Offload 0 command / SDO entries a program may use: 2^AW - 1. */
	uint32_t offload_cmd_depth;
	uint32_t offload_sdo_depth;
	/** CS under set_cs() control: programs leave the CS line alone. */
	bool cs_manual;
	/** Bytes per word = ceil(data_width / 8). */
	uint8_t word_len;
	/**
	 * Device bit order for the transfer in progress. The core only shifts
	 * MSB-first, so LSB-first is each byte bit-reversed on pack and unpack.
	 */
	bool lsb_first;
	/** SDO idle line state programmed into the engine CONFIG register. */
	uint8_t sdo_idle_state;
	/** SPI mode (CPOL/CPHA) of the active device, cached at transceive. */
	uint8_t mode;
	/** Chip-select index of the active device. */
	uint8_t chip_select;

	/** Rolling SYNC command id for PIO (sync) transfers. */
	uint32_t sync_id;

	/** Async delivery state. */
	capi_spi_callback_t callback;
	void *callback_arg;
	volatile bool async_in_progress;

	/** Framing cached for the in-flight async transfer, used on completion. */
	uint8_t *async_rx;		/**< caller rx buffer (may be NULL) */
	uint16_t async_rx_size;		/**< bytes to copy back to async_rx */
	uint16_t async_rx_offset;	/**< first captured byte to copy (read_command) */
	uint16_t async_words;		/**< number of words captured by the DMA */
	const uint8_t *async_tx;	/**< caller tx buffer (may be NULL) */
	uint16_t async_tx_size;		/**< bytes valid in async_tx */
	uint16_t async_pos;		/**< first word of the running chunk */
	uint16_t async_chunk;		/**< words in the running chunk */
	uint32_t async_speed_hz;	/**< SCLK rate of the async transfer */

	/** Persistent transfer descriptor handed to capi_dma_config_xfer(); the
	 *  DMA layer keeps a pointer to it until the transfer completes. */
	struct capi_dma_transfer dma_xfer;

	/**
	 * DMA sample-capture buffer. The offload S2MM DMA writes one data_width-wide
	 * SDI sample per transferred word here; the completion callback unpacks it
	 * into async_rx. Cache-line aligned; the DMA backend flushes it before the
	 * transfer and invalidates it on completion.
	 */
	uint8_t sample_buf[FPGA_SPI_ENGINE_MAX_XFER_BYTES] __attribute__((aligned(64)));
};

/**
 * @brief Static handle initializer for apps that avoid malloc.
 */
#define CAPI_SPI_CONTROLLER_HANDLE_FPGA_INIT()			\
	(&(struct capi_spi_controller_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct fpga_spi_engine_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* FPGA_CAPI_SPI_ENGINE_PRIV_H_ */
