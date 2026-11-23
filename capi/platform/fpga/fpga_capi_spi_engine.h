/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief CAPI SPI backend for the ADI SPI Engine (axi_spi_engine,
 *        spi_engine_execution, spi_engine_offload).
 *
 * Runs SPI transfers on an ADI SPI Engine using only CAPI services;
 * registers are accessed through CAPI_PLATFORM_IO_*. The cores are packaged
 * for both Xilinx and Intel fabric.
 *
 * Configuration (struct capi_spi_config):
 *   - ops   : &fpga_capi_spi_engine_ops
 *   - extra : struct fpga_spi_engine_config * (required)
 *
 * The engine's neighbours in the block design are not owned here. They are
 * peer CAPI handles that the application initializes and passes in:
 *   - clkgen           : SCLK clock (capi_clk), enabled at init; NULL if the
 *                        SCLK is a running fixed clock
 *   - trigger          : offload trigger, a capi_timer PWM channel
 *   - offload_dma_chan : IRQ-driven DEV_TO_MEM capi_dma channel that captures
 *                        the offload SDI samples
 *
 * transceive runs through the command/SDO/SDI FIFOs and needs only the SCLK.
 * transceive_async uses offload mode and needs trigger and offload_dma_chan:
 * the transfer must fit the offload memories, and completion comes from the
 * DMA callback as CAPI_SPI_EVENT_XFR_DONE.
 */

#ifndef FPGA_CAPI_SPI_ENGINE_H_
#define FPGA_CAPI_SPI_ENGINE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <capi_spi.h>
#include <capi_clk.h>
#include <capi_timer.h>
#include <capi_dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct fpga_spi_engine_config
 * @brief Platform-specific SPI Engine configuration, passed via config->extra.
 *
 * Only the SPI Engine base is a raw address; the clock, trigger and offload DMA
 * are composed as references to peer CAPI handles the application owns.
 */
struct fpga_spi_engine_config {
	/** SPI Engine AXI register-map base address. */
	uint32_t regmap_baseaddr;
	/**
	 * SCLK clock generator handle. The SPI Engine's SCLK domain MMCM powers up
	 * held in reset; init enables this clock so TRANSFER instructions can
	 * retire. NULL means the SCLK is already running (no bring-up needed).
	 */
	struct capi_clk_handle *clkgen;
	/**
	 * Offload trigger timer handle (PWM mode). In offload mode the engine
	 * executes its command program only when pulsed on its 'trigger' input;
	 * the async path enables channel trigger_chan per transfer and disables
	 * it on completion. The channel must already be initialized and
	 * configured in CAPI_TIMER_PWM_MODE, with a period longer than one
	 * program run plus the completion latency (each rising edge starts a
	 * run). NULL disables the async path.
	 */
	struct capi_timer_handle *trigger;
	/** Timer channel wired to the engine's trigger input. */
	uint32_t trigger_chan;
	/**
	 * Offload sample-capture S2MM DMA channel. The async path captures the SDI
	 * samples into the backend's sample buffer through this channel and takes
	 * completion from its callback, so the channel must be IRQ-driven (a
	 * polled channel would block in xfer_start before the trigger is on and
	 * time out). NULL disables the async (offload) path.
	 */
	struct capi_dma_chan *offload_dma_chan;
	/** SPI Engine module reference clock in Hz (drives the SCLK divider). */
	uint32_t ref_clk_hz;
	/** Word width in bits (8/16/24/32). Byte buffers are framed into words. */
	uint8_t data_width;
	/** Chip-select assert delay (0..3) for the ASSERT command (cs_delay). */
	uint8_t cs_delay;
	/** SDO idle line state (0 or 1) programmed into the engine CONFIG reg. */
	uint8_t sdo_idle_state;
	/**
	 * Offload directions, FPGA_SPI_ENGINE_OFFLOAD_* (spi_engine_offload_init
	 * offload_config). RX_EN DMAs SDI samples into rx_buf through
	 * offload_dma_chan; TX_EN feeds SDO from a MEM_TO_DEV channel.
	 * FPGA_SPI_ENGINE_OFFLOAD_DISABLED: fpga_spi_engine_offload_capture()
	 * refuses to run unless the message sets its own offload_config.
	 */
	uint8_t offload_config;
};

/**
 * @name Offload directions (fpga_spi_engine_config.offload_config,
 *       spi_engine.h OFFLOAD_*)
 * @{
 */
#define FPGA_SPI_ENGINE_OFFLOAD_DISABLED	0x00U
/* offload SDO stream. */
#define FPGA_SPI_ENGINE_OFFLOAD_TX_EN		(1U << 0)
/* offload SDI stream. */
#define FPGA_SPI_ENGINE_OFFLOAD_RX_EN		(1U << 1)
/* both streams. */
#define FPGA_SPI_ENGINE_OFFLOAD_TX_RX_EN	(FPGA_SPI_ENGINE_OFFLOAD_TX_EN | \
						 FPGA_SPI_ENGINE_OFFLOAD_RX_EN)
/** @} */

/**
 * @name Program option flags (struct fpga_spi_engine_device_config.flags)
 * @{
 */
/** TRANSFER is read-only (spi_engine READ): no SDO words are consumed, SDO
 *  stays at sdo_idle_state. tx_buf is ignored. */
#define FPGA_SPI_ENGINE_FLAG_READ_ONLY		(1U << 0)
/** TRANSFER is write-only (spi_engine WRITE): no SDI words are produced, rx_buf
 *  is left untouched. Not valid for offload capture or transceive_async. */
#define FPGA_SPI_ENGINE_FLAG_WRITE_ONLY	(1U << 1)
/** No ASSERT commands: the program leaves CS as it is (ad713x READ(n) without
 *  CS_LOW/CS_HIGH, CS held by the caller or the HDL). */
#define FPGA_SPI_ENGINE_FLAG_NO_CS		(1U << 2)
/** @} */

/**
 * @struct fpga_spi_engine_device_config
 * @brief Optional per-device program options, passed via
 *        capi_spi_device.extra. NULL means a full-duplex CS-framed TRANSFER.
 */
struct fpga_spi_engine_device_config {
	/** FPGA_SPI_ENGINE_FLAG_* bitmask. READ_ONLY and WRITE_ONLY are exclusive. */
	uint32_t flags;
};

/**
 * @name Offload message commands (spi_engine.h WRITE/READ/WRITE_READ/SLEEP/
 *       CS_HIGH/CS_LOW)
 *
 * Default encoding of fpga_spi_engine_offload_msg.cmds, translated when the
 * program is loaded exactly as spi_engine_write_cmd() does:
 * - TRANSFER: the argument is a byte count (1..255), sent as
 *   ceil(bytes / word bytes) words of the transfer width.
 * - CS_LOW / CS_HIGH: delay and mask are placeholders; the controller
 *   cs_delay and the device chip select are filled in.
 * - SLEEP: the argument is in ns (0..255), converted with the device SCLK
 *   rate (fpga_spi_engine_sleep_arg()).
 * - CONFIG and SYNC are written unchanged.
 * @{
 */
#define FPGA_SPI_ENGINE_WRITE(bytes)		FPGA_SPI_ENGINE_CMD(0U, 0x1U, (bytes))
/* TRANSFER r, raw n field. */
#define FPGA_SPI_ENGINE_READ(bytes)		FPGA_SPI_ENGINE_CMD(0U, 0x2U, (bytes))
/* TRANSFER r + w, raw n field. */
#define FPGA_SPI_ENGINE_WRITE_READ(bytes)	FPGA_SPI_ENGINE_CMD(0U, 0x3U, (bytes))
/* MISC SLEEP, t = prescaler cycles - 1. */
#define FPGA_SPI_ENGINE_SLEEP(ns)		FPGA_SPI_ENGINE_CMD(3U, 0x1U, (ns))
/* CHIPSELECT delay 3, all CS high. */
#define FPGA_SPI_ENGINE_CS_HIGH			FPGA_SPI_ENGINE_CMD(1U, 0x3U, 0xFFU)
/* CHIPSELECT delay 3, all CS low. */
#define FPGA_SPI_ENGINE_CS_LOW			FPGA_SPI_ENGINE_CMD(1U, 0x3U, 0x00U)
/** @} */

/**
 * @name Raw SPI Engine instructions (hdl/library/spi_engine encoding)
 *
 * Written unchanged when fpga_spi_engine_offload_msg.raw_cmds is set: n is a
 * word count (1..256) of the current transfer width, CS and SLEEP arguments
 * go to the core as given.
 * @{
 */
/* spi_engine_execution: instruction cmd[14:12], arg1 cmd[10:8], arg2 cmd[7:0]. */
#define FPGA_SPI_ENGINE_CMD(inst, arg1, arg2)					\
	((uint16_t)((((uint32_t)(inst) & 0x07U) << 12) |		\
		    (((uint32_t)(arg1) & 0x07U) << 8) |			\
		    ((uint32_t)(arg2) & 0xFFU)))
/** TRANSFER of n words, write only (SDO). */
#define FPGA_SPI_ENGINE_CMD_WRITE(n)		FPGA_SPI_ENGINE_CMD(0U, 0x1U, (n) - 1U)
/** TRANSFER of n words, read only (SDI). */
#define FPGA_SPI_ENGINE_CMD_READ(n)		FPGA_SPI_ENGINE_CMD(0U, 0x2U, (n) - 1U)
/** TRANSFER of n words, full duplex. */
#define FPGA_SPI_ENGINE_CMD_WRITE_READ(n)	FPGA_SPI_ENGINE_CMD(0U, 0x3U, (n) - 1U)
/** ASSERT: drive the 8 CS lines to mask (bit = 1 is high) after delay
 *  (0..3). */
#define FPGA_SPI_ENGINE_CMD_ASSERT(delay, mask) FPGA_SPI_ENGINE_CMD(1U, (delay), (mask))
/** CS index cs low (spi_engine CS_LOW for one line), others high. */
#define FPGA_SPI_ENGINE_CMD_CS_LOW(cs)		FPGA_SPI_ENGINE_CMD_ASSERT(0U,		\
		(0xFFU ^ (1U << (cs))))
/** All CS lines high (spi_engine CS_HIGH). */
#define FPGA_SPI_ENGINE_CMD_CS_HIGH		FPGA_SPI_ENGINE_CMD_ASSERT(0U, 0xFFU)
/** SLEEP for (t + 1) SCLK periods at the CLK_DIV in effect (t = 0..255). */
#define FPGA_SPI_ENGINE_CMD_SLEEP(t)		FPGA_SPI_ENGINE_CMD(3U, 0x1U, (t))
/** @} */

/**
 * @struct fpga_spi_engine_offload_msg
 * @brief One offload program plus the sample buffers it streams
 *        (spi_engine_offload_message).
 *
 * The program runs once per trigger pulse. Every run reads words_per_run SDI
 * words, which leave through the offload sample stream; n_samples runs are
 * DMAed straight into rx_buf (no bounce buffer). Each SDI word takes the
 * RX DMA stream width in rx_buf (legacy offload_rx_dma->width_src; little
 * endian, right aligned).
 */
struct fpga_spi_engine_offload_msg {
	/**
	 * Command program: FPGA_SPI_ENGINE_WRITE/READ/... message commands, or
	 * FPGA_SPI_ENGINE_CMD_* raw words when raw_cmds is set. The backend
	 * prefixes CLK_DIV, XFER_LEN and CONFIG and appends SYNC, as
	 * spi_engine_compile_message does. NULL: one TRANSFER of @ref words
	 * words framed like a CAPI transfer (device CS, mode,
	 * FPGA_SPI_ENGINE_FLAG_* options).
	 */
	const uint16_t *cmds;
	/** Number of entries in cmds. */
	uint32_t n_cmds;
	/** cmds are raw core instructions: written without translation. */
	bool raw_cmds;
	/** Words of the TRANSFER when cmds is NULL (1..256). */
	uint32_t words;
	/** SDO memory words (data_width wide, right aligned), replayed every
	 *  run (spi_engine_offload_message.commands_data). May be NULL when the
	 *  program writes nothing or SDO is DMA fed. */
	const uint32_t *sdo;
	/** Number of entries in sdo (at most 2^SDO_MEM_ADDRESS_WIDTH - 1).
	 *  0 with sdo set: one entry per TRANSFER word of the program, as legacy
	 *  (offload_tx_len). */
	uint32_t n_sdo;
	/** Program runs (trigger pulses) to capture. */
	uint32_t n_samples;
	/** Caller DMA destination (OFFLOAD_RX_EN), n_samples * SDI words per run
	 *  * RX DMA stream width bytes. */
	void *rx_buf;
	/** Directions for this message, FPGA_SPI_ENGINE_OFFLOAD_*; 0 uses
	 *  fpga_spi_engine_config.offload_config. */
	uint8_t offload_config;
	/**
	 * MEM_TO_DEV capi_dma channel feeding the offload SDO stream
	 * (OFFLOAD_TX_EN; spi_engine_offload with SDO_STREAMING = 1).
	 */
	struct capi_dma_chan *tx_dma_chan;
	/** TX DMA source, n_samples * SDO words per run * TX DMA stream width
	 *  bytes (legacy offload_tx_dma length). */
	const void *tx_buf;
	/** Forwarded to capi_dma_transfer.extra of the TX transfer. NULL: the
	 *  backend selects hardware cyclic unless tx_oneshot. */
	void *tx_xfer_extra;
	/** TX DMA runs tx_buf once. Default (false) is cyclic, as legacy
	 *  dma_flags = 0 / DMA_CYCLIC: the stream replays until
	 *  fpga_spi_engine_offload_stream_stop(). */
	bool tx_oneshot;
};

/**
 * @brief SPI Engine operations table. Select via config->ops.
 */
extern const struct capi_spi_ops fpga_capi_spi_engine_ops;

/**
 * @brief Change the word width used by later transfers.
 * @note spi_engine_set_transfer_width(): a width above the core DATA_WIDTH is
 *       clamped to it.
 * @param handle - SPI controller handle.
 * @param bits - Word width in bits (1..32).
 * @return 0 on success, -EINVAL on bad arguments, -EBUSY while an offload
 *         transfer is running.
 */
int fpga_spi_engine_set_transfer_width(struct capi_spi_controller_handle
				       *handle,
				       uint8_t bits);

/**
 * @brief Word width in bits used by transfers.
 * @param handle - SPI controller handle.
 * @return Width in bits, 0 if handle is invalid.
 */
uint8_t fpga_spi_engine_get_transfer_width(struct capi_spi_controller_handle
		*handle);

/**
 * @brief Widest word the core supports (spi_engine_desc.max_data_width).
 * @param handle - SPI controller handle.
 * @return DATA_WIDTH register [15:0] in bits, 0 if handle is invalid.
 */
uint8_t fpga_spi_engine_get_max_data_width(struct capi_spi_controller_handle
		*handle);

/**
 * @brief Number of SDI lanes the core was built with (DATA_WIDTH [23:16]).
 * @param handle - SPI controller handle.
 * @return Lane count, 0 if handle is invalid.
 */
uint8_t fpga_spi_engine_get_num_sdi(struct capi_spi_controller_handle *handle);

/**
 * @brief Bytes one offload SDI sample takes in a capture buffer (the width
 *        of the offload sample stream; legacy offload_rx_dma->width_src).
 * @param handle - SPI controller handle.
 * @return Bytes per sample, 0 if handle is invalid.
 */
uint8_t fpga_spi_engine_get_sample_len(struct capi_spi_controller_handle
				       *handle);

/**
 * @brief SPI Engine VERSION register read at init.
 * @param handle - SPI controller handle.
 * @return VERSION value, 0 if handle is invalid.
 */
uint32_t fpga_spi_engine_get_version(struct capi_spi_controller_handle *handle);

/**
 * @brief Convert a delay to the argument of FPGA_SPI_ENGINE_CMD_SLEEP().
 * @note spi_gen_sleep_ns(): one SLEEP tick is one SCLK period at the CLK_DIV
 *       derived from speed_hz; the result is rounded up and saturated at 255.
 * @param handle - SPI controller handle.
 * @param speed_hz - SCLK rate the program runs at (device max_speed_hz).
 * @param ns - Delay in ns.
 * @return SLEEP argument t (sleeps t + 1 SCLK periods).
 */
uint8_t fpga_spi_engine_sleep_arg(struct capi_spi_controller_handle *handle,
				  uint32_t speed_hz, uint32_t ns);

/**
 * @brief Set the SCLK rate of later transfers (spi_engine_set_speed()).
 * @note The rate is the device's: it sets device->max_speed_hz, which every
 *       program converts to CLK_DIV.
 * @param device - SPI device.
 * @param speed_hz - SCLK rate in Hz.
 * @return 0 on success, -EINVAL on bad arguments (NULL, 0 Hz).
 */
int fpga_spi_engine_set_speed(struct capi_spi_device *device,
			      uint32_t speed_hz);

/**
 * @brief Write an SPI Engine register (spi_engine_write()).
 * @note Raw access for drivers that program the core directly (ad5766
 *       offload memory); offsets are the FPGA_SPI_ENGINE_REG_* values.
 * @param handle - SPI controller handle.
 * @param reg - Register byte offset.
 * @param val - Value.
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_spi_engine_reg_write(struct capi_spi_controller_handle *handle,
			      uint32_t reg, uint32_t val);

/**
 * @brief Read an SPI Engine register (spi_engine_read()).
 * @param handle - SPI controller handle.
 * @param reg - Register byte offset.
 * @param val - Output value.
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_spi_engine_reg_read(struct capi_spi_controller_handle *handle,
			     uint32_t reg, uint32_t *val);

/**
 * @brief Offload RX DMA channel (legacy spi_engine_desc.offload_rx_dma).
 * @note Its stream width (fpga_dmac_get_width() src) is the bytes per SDI
 *       sample in rx_buf, used to size cache maintenance (ad738x).
 * @param handle - SPI controller handle.
 * @return The channel, NULL if none or the handle is invalid.
 */
struct capi_dma_chan *fpga_spi_engine_get_offload_dma(
	struct capi_spi_controller_handle *handle);

/**
 * @brief Load an offload program into offload 0 without starting it.
 * @note spi_engine_offload_transfer() load phase: OFFLOAD_RESET, CMD memory
 *       (CLK_DIV, XFER_LEN, CONFIG, program, SYNC), SDO memory. The offload
 *       stays disabled.
 * @param device - SPI device (speed, mode, CS, options via extra).
 * @param msg - Program description; only the program fields are used.
 * @return 0 on success, -EINVAL if the program or SDO words do not fit the
 *         offload memories, -EBUSY while an offload transfer is running.
 */
int fpga_spi_engine_offload_load(struct capi_spi_device *device,
				 const struct fpga_spi_engine_offload_msg *msg);

/**
 * @brief Run an offload program and DMA its samples
 *        (spi_engine_offload_transfer()).
 * @note As legacy: load the program, enable the offload, start the TX DMA
 *       (OFFLOAD_TX_EN, cyclic unless tx_oneshot) and the RX DMA
 *       (OFFLOAD_RX_EN), and wait for the RX DMA only. The offload (and the
 *       trigger timer, if composed) stays running on return, and so does a
 *       TX stream, until fpga_spi_engine_offload_stream_stop(), the next
 *       capture or a sync transfer (which disables the offload). Without a
 *       trigger handle the engine runs on an external trigger. A running
 *       offload is stopped before the next one is loaded.
 *       Differs from legacy (legacy bugs): DMA lengths count only the words
 *       that direction moves (legacy counted every TRANSFER word, so a mixed
 *       program timed out); TX uses the stream-side DMA width; a read
 *       program without OFFLOAD_RX_EN is refused (legacy stalled the
 *       engine).
 * @param device - SPI device (speed, mode, CS, options via extra).
 * @param msg - Program and buffers.
 * @param timeout_us - RX completion timeout in us; 0 = 500 ms (legacy).
 * @return 0 on success, or:
 *         -EINVAL     bad arguments / does not fit / offload disabled
 *         -ENOTSUP    RX_EN without an offload DMA channel
 *         -ETIMEDOUT  no RX completion in time (offload stopped)
 *         other       capi_dma / capi_timer error
 */
int fpga_spi_engine_offload_capture(struct capi_spi_device *device,
				    const struct fpga_spi_engine_offload_msg *msg,
				    uint32_t timeout_us);

/**
 * @brief Stop the offload, trigger and TX stream left running by
 *        fpga_spi_engine_offload_capture().
 * @param device - SPI device.
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_spi_engine_offload_stream_stop(struct capi_spi_device *device);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_CAPI_SPI_ENGINE_H_ */
