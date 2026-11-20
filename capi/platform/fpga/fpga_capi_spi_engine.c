/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Implementation of the SPI Engine CAPI SPI backend.
 *
 * The SPI Engine is not register-per-setting like most SPI controllers: it
 * executes a stream of 16-bit instructions written to its command FIFO. TX
 * data goes to a separate SDO (MOSI) FIFO and RX data comes back in the SDI
 * (MISO) FIFO. One CAPI transfer is compiled into:
 *   - CONFIG: clock divider, transfer length, SPI mode, SDO idle level.
 *   - ASSERT: chip select active, after cs_delay.
 *   - TRANSFER: write and/or read of N words.
 *   - ASSERT: chip select inactive.
 *   - SYNC: end marker carrying an id; the engine copies the id to the
 *     SYNC_ID register when it reaches it, so reading SYNC_ID tells that
 *     every instruction before it has finished.
 *
 * Blocking transfers (transceive): the instructions go to the command FIFO
 * and the TX words to the SDO FIFO; the backend waits (bounded, -ETIMEDOUT)
 * for SYNC_ID, then reads the RX words from the SDI FIFO.
 *
 * Non-blocking transfers (transceive_async) use the offload: a block next
 * to the engine that stores one instruction stream plus its TX words in
 * on-chip memory and replays them into the engine on every pulse of its
 * trigger input, with no CPU involvement. RX words leave through an AXI
 * stream to a DMA channel.
 *
 *   1. Reset the offload and load the instructions and TX words into its
 *      memories.
 *   2. Enable the offload and start the capture DMA channel.
 *   3. Enable the PWM trigger; each rising edge runs the stored transfer.
 *   4. On DMA completion (fpga_spi_engine_dma_done): stop the trigger and the
 *      offload, unpack the RX words and raise CAPI_SPI_EVENT_XFR_DONE.
 *
 * Data framing: byte buffers are packed into data_width-bit words, MSB first.
 * The engine only shifts MSB first; lsb_first reverses the bits of each byte.
 *
 * init enables the SCLK clock (if given), pulses the core RESET and clamps
 * data_width to the width the core was built with.
 *
 * See fpga_capi_spi_engine.h for configuration.
 */

#include <capi_spi.h>
#include <capi_clk.h>
#include <capi_timer.h>
#include <capi_dma.h>
#include "fpga_capi_spi_engine_priv.h"
#include "fpga_capi_dmac.h"
#include "capi_alloc.h"
#include "capi_time.h"
#include <errno.h>
#include <string.h>
#include "capi_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --- register access ------------------------------------------------------ */

/**
 * @brief Write a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @param val - Value to write.
 */
static inline void fpga_spi_engine_wr(uint32_t base, uint32_t reg, uint32_t val)
{
	CAPI_PLATFORM_IO_WRITE32(base + reg, val);
}

/**
 * @brief Read a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @return Register value.
 */
static inline uint32_t fpga_spi_engine_rd(uint32_t base, uint32_t reg)
{
	return CAPI_PLATFORM_IO_READ32(base + reg);
}

/* --- static prototypes ---------------------------------------------------- */

static int fpga_capi_spi_engine_init(struct capi_spi_controller_handle **handle,
				     const struct capi_spi_config *config);
static int fpga_capi_spi_engine_deinit(struct capi_spi_controller_handle
				       *handle);
static int fpga_capi_spi_engine_transceive(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer);
static int fpga_capi_spi_engine_transceive_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer);
static int fpga_capi_spi_engine_read_command(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer);
static int fpga_capi_spi_engine_read_command_async(struct capi_spi_device
		*device,
		struct capi_spi_transfer *transfer);
static int fpga_capi_spi_engine_abort_async(struct capi_spi_device *device);
static void fpga_capi_spi_engine_isr(void *handle);
static int fpga_capi_spi_engine_register_callback(struct
		capi_spi_controller_handle
		*handle,
		capi_spi_callback_t const callback, void *callback_arg);
static int fpga_capi_spi_engine_set_cs(struct capi_spi_device *device,
				       enum capi_spi_cs_control cs);
static void fpga_spi_engine_dma_done(uint32_t event, void *ctx);
static void fpga_spi_engine_capture_end(struct fpga_spi_engine_priv *priv);

const struct capi_spi_ops fpga_capi_spi_engine_ops = {
	.init = fpga_capi_spi_engine_init,
	.deinit = fpga_capi_spi_engine_deinit,
	.transceive = fpga_capi_spi_engine_transceive,
	.transceive_async = fpga_capi_spi_engine_transceive_async,
	.read_command = fpga_capi_spi_engine_read_command,
	.read_command_async = fpga_capi_spi_engine_read_command_async,
	.abort_async = fpga_capi_spi_engine_abort_async,
	.register_callback = fpga_capi_spi_engine_register_callback,
	.set_cs = fpga_capi_spi_engine_set_cs,
	/* Completion is delivered by the offload DMA backend's interrupt via
	 * fpga_spi_engine_dma_done(); .isr services that DMA channel for callers that
	 * dispatch interrupts themselves. */
	.isr = fpga_capi_spi_engine_isr,
};

/* --- helpers -------------------------------------------------------------- */

/**
 * @brief Compute the SCLK divider: SCLK = ref_clk / (2 * (div + 1)).
 * @param ref_hz - Engine reference clock in Hz.
 * @param speed_hz - Requested SCLK rate in Hz.
 * @return Divider value for the CONFIG clock register.
 */
static uint32_t fpga_spi_engine_clk_div(uint32_t ref_hz, uint32_t speed_hz)
{
	uint32_t div;

	if (speed_hz == 0U)
		speed_hz = ref_hz / 16U;
	if (speed_hz == 0U || (2U * speed_hz) >= ref_hz)
		return 0U;

	div = ref_hz / (2U * speed_hz);
	if (div > 0U)
		div -= 1U;
	if (div > 0xFFU)
		div = 0xFFU;

	return div;
}

/**
 * @brief Translate a CAPI SPI mode into the engine CONFIG register value.
 * @param mode - CAPI SPI mode (CPOL/CPHA).
 * @param sdo_idle - SDO idle level (0 or 1).
 * @param three_wire - Set the 3WIRE bit (capi_spi_config.three_pin_mode).
 * @return CONFIG register value.
 */
static uint32_t fpga_spi_engine_cfg_value(uint8_t mode, uint8_t sdo_idle,
		bool three_wire)
{
	uint32_t cfg = 0U;

	if (three_wire)
		cfg |= FPGA_SPI_ENGINE_CONFIG_3WIRE;

	if (mode & CAPI_SPI_CPHA)
		cfg |= FPGA_SPI_ENGINE_CONFIG_CPHA;
	if (mode & CAPI_SPI_CPOL)
		cfg |= FPGA_SPI_ENGINE_CONFIG_CPOL;
	if (sdo_idle)
		cfg |= FPGA_SPI_ENGINE_CONFIG_SDO_IDLE;

	return cfg;
}

/**
 * @brief First set chip-select index from a native-CS bitmask (0 if none).
 * @param native_cs - Native chip-select bitmask.
 * @return Chip-select index.
 */
static uint8_t fpga_spi_engine_cs_index(uint16_t native_cs)
{
	uint8_t cs = 0U;

	if (native_cs == 0U)
		return 0U;
	while (cs < 15U && !(native_cs & (1U << cs)))
		cs++;

	return cs;
}

/**
 * @brief Reverse the bit order of one byte.
 * @param v - Byte.
 * @return v with bit 0 and bit 7 (etc.) swapped.
 */
static uint8_t fpga_spi_engine_rev8(uint8_t v)
{
	v = (uint8_t)(((v & 0xF0U) >> 4) | ((v & 0x0FU) << 4));
	v = (uint8_t)(((v & 0xCCU) >> 2) | ((v & 0x33U) << 2));
	return (uint8_t)(((v & 0xAAU) >> 1) | ((v & 0x55U) << 1));
}

/**
 * @brief Bit position of byte b inside a data_width-bit word.
 * @note spi_engine_write_and_read(): byte b sits at data_width - (b + 1) * 8,
 *       so the first byte is MSB-aligned. When data_width is not a multiple
 *       of 8 the last byte only partly fits; legacy shifts by a negative
 *       count there (undefined), here its top bits fill the word's low bits.
 * @param data_width - Word width in bits.
 * @param b - Byte index in the word.
 * @return Left shift (>= 0) or the negated right shift (< 0).
 */
static int fpga_spi_engine_byte_shift(uint8_t data_width, uint8_t b)
{
	return (int)data_width - (int)(b + 1U) * 8;
}

/**
 * @brief Pack word w of a byte stream MSB-first (zero-filled past tx_size).
 * @param tx - Transmit buffer (NULL sends zeros).
 * @param tx_size - Transmit buffer size in bytes.
 * @param data_width - Word width in bits.
 * @param word_len - Word length in bytes.
 * @param w - Word index.
 * @param lsb_first - Bit-reverse each byte (device LSB-first).
 * @return Packed word.
 */
static uint32_t fpga_spi_engine_pack_word(const uint8_t *tx, uint16_t tx_size,
		uint8_t data_width, uint8_t word_len, uint16_t w,
		bool lsb_first)
{
	uint32_t word = 0U;
	uint8_t b;
	int sh;

	for (b = 0; b < word_len; b++) {
		uint16_t idx = (uint16_t)(w * word_len + b);
		uint32_t v = (tx && idx < tx_size) ? tx[idx] : 0U;
		if (lsb_first)
			v = fpga_spi_engine_rev8((uint8_t)v);
		sh = fpga_spi_engine_byte_shift(data_width, b);
		word |= sh >= 0 ? v << sh : v >> -sh;
	}

	return word;
}

/**
 * @brief Unpack a received word MSB-first into word_len bytes at dst.
 * @param word - Received word.
 * @param dst - Destination byte buffer.
 * @param data_width - Word width in bits.
 * @param word_len - Word length in bytes.
 * @param lsb_first - Bit-reverse each byte (device LSB-first).
 */
static void fpga_spi_engine_unpack_word(uint32_t word, uint8_t *dst,
					uint8_t data_width, uint8_t word_len,
					bool lsb_first)
{
	uint8_t b;
	int sh;

	for (b = 0; b < word_len; b++) {
		sh = fpga_spi_engine_byte_shift(data_width, b);
		dst[b] = (uint8_t)(sh >= 0 ? word >> sh : word << -sh);
		if (lsb_first)
			dst[b] = fpga_spi_engine_rev8(dst[b]);
	}
}

/**
 * @brief Push one command into the PIO command FIFO (bounded room wait).
 * @param regmap - Base address of the SPI Engine register map.
 * @param cmd - Encoded SPI Engine command word.
 * @return 0 on success, -ETIMEDOUT if the FIFO never had room.
 */
static int fpga_spi_engine_push_cmd(uint32_t regmap, uint32_t cmd)
{
	uint32_t spins = FPGA_SPI_ENGINE_SPIN_MAX;

	while (fpga_spi_engine_rd(regmap, FPGA_SPI_ENGINE_REG_CMD_FIFO_ROOM) == 0U)
		if (--spins == 0U)
			return -ETIMEDOUT;
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_CMD_FIFO, cmd);

	return 0;
}

/**
 * @brief Wait until the engine retires the SYNC command with the given id.
 * @param regmap - Base address of the SPI Engine register map.
 * @param sync - SYNC id.
 * @return 0 on success, -ETIMEDOUT otherwise.
 */
static int fpga_spi_engine_wait_sync(uint32_t regmap, uint32_t sync)
{
	uint32_t spins = FPGA_SPI_ENGINE_SPIN_MAX;

	while (fpga_spi_engine_rd(regmap, FPGA_SPI_ENGINE_REG_SYNC_ID) != sync)
		if (--spins == 0U)
			return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Pulse the core RESET. Recovers from a
 *        stalled command stream; the offload memories must be reloaded.
 * @param regmap - Base address of the SPI Engine register map.
 */
static void fpga_spi_engine_core_reset(uint32_t regmap)
{
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_RESET, 0x01);
	capi_wait_us(FPGA_SPI_ENGINE_RESET_US);
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_RESET, 0x00);
}

/**
 * @brief Stop offload 0 and leave the core idle.
 *
 * The trigger is a free-running generator: every rising edge while offload is
 * enabled starts another run of the program. Disable the trigger, then the
 * offload, and wait (bounded) for OFFLOAD_STATUS to drop. A run that cannot
 * finish (its samples have no DMA transfer to go to) is cleared by a core
 * reset. Finally clear the offload memories.
 * @param priv - Pointer to the backend private handle.
 */
static void fpga_spi_engine_offload_stop(struct fpga_spi_engine_priv *priv)
{
	uint32_t regmap = priv->regmap_base;
	uint32_t spins = FPGA_SPI_ENGINE_STOP_SPINS;

	if (priv->trigger)
		(void)capi_timer_channel_disable(priv->trigger, priv->trigger_chan);

	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);
	while ((fpga_spi_engine_rd(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_STATUS(0)) &
		FPGA_SPI_ENGINE_OFFLOAD_STATUS_ENABLED) && --spins)
		;
	if (spins == 0U)
		fpga_spi_engine_core_reset(regmap);

	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 1U);
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 0U);
}

/**
 * @brief Reassemble a captured little-endian sample into a word.
 * @param buf - Captured sample bytes.
 * @param word_len - Word length in bytes.
 * @return Reassembled word.
 */
static uint32_t fpga_spi_engine_sample_word(const uint8_t *buf,
		uint8_t word_len)
{
	uint32_t word = 0U;
	uint8_t b;

	for (b = 0; b < word_len; b++)
		word |= (uint32_t)buf[b] << (8U * b);

	return word;
}

/**
 * @brief Emit the transfer program (spi_engine_compile_message): CLK_DIV,
 *        XFER_LEN, CONFIG, CS high, CS low, TRANSFER, CS high. With CS under
 *        set_cs() control or FPGA_SPI_ENGINE_FLAG_NO_CS the three ASSERT commands are
 *        left out; FPGA_SPI_ENGINE_FLAG_READ_ONLY / _WRITE_ONLY pick the TRANSFER
 *        direction (spi_engine READ / WRITE). A frame
 *        split over several programs opens CS in the first one only and
 *        closes it in the last one only.
 * @param priv - Pointer to the backend private handle.
 * @param words - Number of transfer words (1..256).
 * @param cs_begin - Assert CS at the start of the program.
 * @param cs_end - Deassert CS at the end of the program.
 * @param push - Callback that consumes each command (PIO FIFO or offload
 *               memory).
 * @param sink - Register map base passed through to push.
 * @param speed_hz - Requested SCLK rate in Hz.
 * @return 0 on success, or the first error returned by push.
 */
static int fpga_spi_engine_emit_program(struct fpga_spi_engine_priv *priv,
					uint16_t words, bool cs_begin, bool cs_end,
					int (*push)(uint32_t, uint32_t),
					uint32_t sink, uint32_t speed_hz)
{
	uint32_t clk_div = fpga_spi_engine_clk_div(priv->ref_clk_hz, speed_hz);
	uint32_t cfg = fpga_spi_engine_cfg_value(priv->mode, priv->sdo_idle_state,
			priv->three_wire);
	uint8_t cs_low = (uint8_t)(0xFFU ^ (1U << priv->chip_select));
	uint32_t prog[FPGA_SPI_ENGINE_OFFLOAD_PROGRAM_LEN];
	uint32_t n = 0U, i;
	bool cs = !priv->cs_manual &&
		  !(priv->xfer_flags & FPGA_SPI_ENGINE_FLAG_NO_CS);
	uint32_t rw = FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_RW;
	int ret;

	/* spi_engine READ / WRITE / WRITE_READ. */
	if (priv->xfer_flags & FPGA_SPI_ENGINE_FLAG_READ_ONLY)
		rw = FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_R;
	else if (priv->xfer_flags & FPGA_SPI_ENGINE_FLAG_WRITE_ONLY)
		rw = FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_W;

	prog[n++] = FPGA_SPI_ENGINE_CMD_CONFIG(FPGA_SPI_ENGINE_CMD_REG_CLK_DIV,
					       clk_div);
	prog[n++] = FPGA_SPI_ENGINE_CMD_CONFIG(
			    FPGA_SPI_ENGINE_CMD_DATA_TRANSFER_LEN, priv->data_width);
	prog[n++] = FPGA_SPI_ENGINE_CMD_CONFIG(FPGA_SPI_ENGINE_CMD_REG_CONFIG,
					       cfg);
	if (cs && cs_begin) {
		prog[n++] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, 0xFFU);
		prog[n++] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, cs_low);
	}
	prog[n++] = FPGA_SPI_ENGINE_CMD_TRANSFER(rw, (uint8_t)(words - 1U));
	if (cs && cs_end)
		prog[n++] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, 0xFFU);

	for (i = 0; i < n; i++) {
		ret = push(sink, prog[i]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Take a device's framing (mode, bit order, CS, program options) for
 *        the next program.
 * @param priv - Pointer to the backend private handle.
 * @param device - SPI device; extra is an optional
 *                 struct fpga_spi_engine_device_config.
 * @param allowed - FPGA_SPI_ENGINE_FLAG_* options valid for the caller.
 * @return 0 on success, -EINVAL for a bad CS or option, -EBUSY while an
 *         offload transfer is running (its re-arm still uses the framing).
 */
static int fpga_spi_engine_dev_bind(struct fpga_spi_engine_priv *priv,
				    const struct capi_spi_device *device,
				    uint32_t allowed)
{
	const struct fpga_spi_engine_device_config *dc = device->extra;
	uint32_t flags = dc ? dc->flags : 0U;

	if (priv->async_in_progress)
		return -EBUSY;
	if ((flags & ~allowed) ||
	    ((flags & FPGA_SPI_ENGINE_FLAG_READ_ONLY) &&
	     (flags & FPGA_SPI_ENGINE_FLAG_WRITE_ONLY)))
		return -EINVAL;
	if (fpga_spi_engine_cs_index(device->native_cs) >= FPGA_SPI_ENGINE_NUM_CS)
		return -EINVAL;

	priv->mode = (uint8_t)device->mode;
	priv->lsb_first = device->lsb_first;
	priv->chip_select = fpga_spi_engine_cs_index(device->native_cs);
	priv->xfer_flags = flags;

	return 0;
}

/**
 * @brief Write a command into offload 0 command memory (fpga_spi_engine_emit_program
 *        sink for the offload path).
 * @param regmap - Base address of the SPI Engine register map.
 * @param cmd - Encoded SPI Engine command word.
 * @return 0.
 */
static int fpga_spi_engine_sink_offload_cmd(uint32_t regmap, uint32_t cmd)
{
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), cmd);

	return 0;
}

/**
 * @brief Synchronous PIO transceive covering n framed bytes; copy rx_size bytes
 *        starting at rx_offset back to rx.
 * @param priv - Pointer to the backend private handle.
 * @param speed_hz - Requested SCLK rate in Hz.
 * @param tx - Transmit buffer (NULL sends zeros).
 * @param tx_size - Transmit buffer size in bytes.
 * @param rx - Receive buffer (NULL discards).
 * @param rx_size - Number of bytes to copy back to rx.
 * @param n - Number of bytes framed on the bus.
 * @param rx_offset - Offset of the first byte copied back to rx.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_spi_engine_sync_xfer(struct fpga_spi_engine_priv *priv,
				     uint32_t speed_hz, const uint8_t *tx,
				     uint16_t tx_size, uint8_t *rx, uint16_t rx_size,
				     uint16_t n, uint16_t rx_offset)
{
	uint32_t regmap = priv->regmap_base;
	uint8_t word_len = priv->word_len;
	uint16_t words = (uint16_t)((n + word_len - 1U) / word_len);
	uint8_t rxtmp[FPGA_SPI_ENGINE_MAX_XFER_BYTES];
	uint32_t sync, spins;
	uint16_t tx_w = 0U, rx_w = 0U, rx_words = words;
	bool progress;
	int ret;

	/* READ consumes no SDO words, WRITE produces no SDI words. */
	if (priv->xfer_flags & FPGA_SPI_ENGINE_FLAG_READ_ONLY)
		tx_w = words;
	if (priv->xfer_flags & FPGA_SPI_ENGINE_FLAG_WRITE_ONLY)
		rx_words = 0U;

	if (n == 0U)
		return 0;
	/* Whole words are framed: bound the rounded-up size, not n. */
	if ((uint32_t)words * word_len > FPGA_SPI_ENGINE_MAX_XFER_BYTES)
		return -EINVAL;
	if (priv->async_in_progress)
		return -EBUSY;

	sync = (priv->sync_id = (priv->sync_id + 1U) & 0xFFU);

	/* spi_engine_write_and_read: offload off while the FIFOs are used. */
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);

	ret = fpga_spi_engine_emit_program(priv, words, true, true,
					   fpga_spi_engine_push_cmd,
					   regmap, speed_hz);
	if (!ret)
		ret = fpga_spi_engine_push_cmd(regmap, FPGA_SPI_ENGINE_CMD_SYNC(sync));
	if (ret)
		goto timeout;

	/*
	 * Feed SDO and drain SDI together. The engine stalls once the SDI FIFO
	 * is full, so filling SDO first (as spi_engine_transfer_message does)
	 * only works up to the FIFO depth.
	 */
	spins = FPGA_SPI_ENGINE_SPIN_MAX;
	while (tx_w < words || rx_w < rx_words) {
		progress = false;
		if (tx_w < words &&
		    fpga_spi_engine_rd(regmap, FPGA_SPI_ENGINE_REG_SDO_FIFO_ROOM)) {
			fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_SDO_DATA_FIFO,
					   fpga_spi_engine_pack_word(tx, tx_size,
							   priv->data_width, word_len, tx_w,
							   priv->lsb_first));
			tx_w++;
			progress = true;
		}
		if (rx_w < rx_words &&
		    fpga_spi_engine_rd(regmap, FPGA_SPI_ENGINE_REG_SDI_FIFO_LEVEL)) {
			fpga_spi_engine_unpack_word(fpga_spi_engine_rd(regmap,
						    FPGA_SPI_ENGINE_REG_SDI_DATA_FIFO),
						    &rxtmp[rx_w * word_len],
						    priv->data_width, word_len,
						    priv->lsb_first);
			rx_w++;
			progress = true;
		}
		if (progress)
			spins = FPGA_SPI_ENGINE_SPIN_MAX;
		else if (--spins == 0U)
			goto timeout;
	}

	if (fpga_spi_engine_wait_sync(regmap, sync))
		goto timeout;

	if (rx && rx_size && rx_words)
		memcpy(rx, &rxtmp[rx_offset], rx_size);

	return 0;

timeout:
	/* Leave the core usable for the next transfer. */
	fpga_spi_engine_core_reset(regmap);
	return -ETIMEDOUT;
}

/* --- ops ------------------------------------------------------------------ */

/**
 * @brief Initialize the CAPI backend instance.
 * @note SPI Engine: enables the SCLK clkgen (if given), then
 *       RESET pulse, DATA_WIDTH (clamps data_width), VERSION, and the offload
 *       memory depths.
 * @param handle - Pointer to the CAPI handle; allocated if *handle is NULL.
 * @param config - Pointer to the CAPI configuration.
 * @return 0 on success, negative errno on failure.
 */
/* Takes the caller's static handle or allocates one; *alloc tells which. */
static int fpga_spi_engine_handle_get(struct capi_spi_controller_handle
				      **handle,
				      struct capi_spi_controller_handle **h,
				      bool *alloc)
{
	struct fpga_spi_engine_priv *priv;

	/* Re-init of a live handle is a caller bug, not a reset. */
	if (*handle != NULL &&
	    ((*handle)->ops != NULL ||
	     ((*handle)->priv != NULL &&
	      ((struct fpga_spi_engine_priv *)(*handle)->priv)->regmap_base != 0U)))
		return -EBUSY;

	*alloc = (*handle == NULL);
	if (!*alloc) {
		*h = *handle;
		return (*h)->priv ? 0 : -EINVAL;
	}

	*h = capi_calloc(1, sizeof(**h));
	if (!*h)
		return -ENOMEM;
	priv = capi_calloc(1, sizeof(*priv));
	if (!priv) {
		capi_free(*h);
		return -ENOMEM;
	}
	(*h)->priv = priv;

	return 0;
}

/* Copies the board config into priv and clears the run-time state. */
static void fpga_spi_engine_apply_config(struct fpga_spi_engine_priv *priv,
		const struct capi_spi_config *config,
		const struct fpga_spi_engine_config *extra)
{
	priv->regmap_base = extra->regmap_baseaddr;
	priv->clkgen = extra->clkgen;
	priv->trigger = extra->trigger;
	priv->trigger_chan = extra->trigger_chan;
	priv->offload_dma_chan = extra->offload_dma_chan;
	priv->ref_clk_hz = extra->ref_clk_hz ? extra->ref_clk_hz :
			   config->clk_freq_hz;
	priv->data_width = extra->data_width ? extra->data_width : 8U;
	priv->word_len = (uint8_t)((priv->data_width + 7U) / 8U);
	priv->sdo_idle_state = extra->sdo_idle_state;
	priv->cs_delay = extra->cs_delay & 0x3U;
	priv->sync_id = 0U;
	priv->async_in_progress = false;
	priv->cs_manual = false;
	priv->three_wire = config->three_pin_mode;
	priv->xfer_flags = 0U;
	priv->capture_active = false;
	priv->offload_running = false;
	priv->stream_tx_chan = NULL;
	priv->offload_config = extra->offload_config;
	priv->callback = NULL;
	priv->callback_arg = NULL;
}

/* Resets the core and reads what it was synthesized with. */
static void fpga_spi_engine_core_probe(struct fpga_spi_engine_priv *priv)
{
	uint32_t reg, aw;

	/* Reset, read the core's data width, clamp to it. */
	fpga_spi_engine_wr(priv->regmap_base, FPGA_SPI_ENGINE_REG_RESET, 0x01);
	capi_wait_us(1000);
	fpga_spi_engine_wr(priv->regmap_base, FPGA_SPI_ENGINE_REG_RESET, 0x00);

	/* Only the lower 16 bits are relevant for the actual data-width */
	reg = fpga_spi_engine_rd(priv->regmap_base, FPGA_SPI_ENGINE_REG_DATA_WIDTH);
	priv->max_data_width = (uint8_t)FPGA_SPI_ENGINE_DATA_WIDTH(reg);
	priv->num_sdi = (uint8_t)((reg & FPGA_SPI_ENGINE_REG_NUM_OF_SDI_MSK) >>
				  16);
	if (priv->max_data_width && priv->data_width > priv->max_data_width)
		priv->data_width = priv->max_data_width;
	priv->word_len = (uint8_t)((priv->data_width + 7U) / 8U);
	/* The offload SDI stream carries one DATA_WIDTH-wide beat per word. */
	priv->sample_len = (uint8_t)(((priv->max_data_width ?
				       priv->max_data_width : priv->data_width) + 7U) / 8U);

	priv->version = fpga_spi_engine_rd(priv->regmap_base,
					   FPGA_SPI_ENGINE_REG_VERSION);

	/*
	 * Usable offload memory depths, so no program overruns them. The write
	 * pointers are AW bits wide (spi_engine_offload.v): filling all 2^AW
	 * entries wraps ctrl_*_wr_addr to 0, which the core reads as an empty
	 * memory (SDO never valid, last_cmd wrong) and the run stalls. One
	 * entry less than 2^AW is the most a program may use.
	 */
	reg = fpga_spi_engine_rd(priv->regmap_base,
				 FPGA_SPI_ENGINE_REG_OFFLOAD_MEM_ADDR_WIDTH);
	aw = FPGA_SPI_ENGINE_OFFLOAD_CMD_MEM_AW(reg);
	priv->offload_cmd_depth = (1U << (aw ? aw :
					  FPGA_SPI_ENGINE_OFFLOAD_MEM_AW_DEFAULT)) - 1U;
	aw = FPGA_SPI_ENGINE_OFFLOAD_SDO_MEM_AW(reg);
	priv->offload_sdo_depth = (1U << (aw ? aw :
					  FPGA_SPI_ENGINE_OFFLOAD_MEM_AW_DEFAULT)) - 1U;
}

static int fpga_capi_spi_engine_init(struct capi_spi_controller_handle **handle,
				     const struct capi_spi_config *config)
{
	struct capi_spi_controller_handle *h;
	struct fpga_spi_engine_priv *priv;
	bool alloc;
	int ret;

	if (handle == NULL || config == NULL || config->extra == NULL)
		return -EINVAL;

	ret = fpga_spi_engine_handle_get(handle, &h, &alloc);
	if (ret)
		return ret;
	h->init_allocated = alloc;
	priv = h->priv;
	fpga_spi_engine_apply_config(priv, config, config->extra);

	/*
	 * Bring up the SCLK clock generator. The MMCM powers up held in reset, so
	 * with no bring-up the SCLK domain is dead and every TRANSFER stalls (SYNC
	 * never retires -> sync xfers time out). capi_clk_enable() releases the
	 * reset and waits for MMCM lock; it is idempotent with an app-level
	 * bring-up. A NULL clkgen means the SCLK is already running.
	 */
	if (priv->clkgen) {
		ret = capi_clk_enable(priv->clkgen);
		if (ret) {
			if (alloc) {
				capi_free(priv);
				capi_free(h);
			}
			return ret;
		}
	}

	fpga_spi_engine_core_probe(priv);
	h->ops = config->ops ? config->ops : &fpga_capi_spi_engine_ops;
	*handle = h;

	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note SPI Engine: aborts an in-flight offload DMA, stops the trigger and
 *       offload 0, unregisters the DMA callback, holds the core in RESET.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_spi_engine_deinit(struct capi_spi_controller_handle
				       *handle)
{
	struct fpga_spi_engine_priv *priv;

	if (!handle)
		return -EINVAL;

	priv = handle->priv;
	if (priv && priv->regmap_base) {
		/*
		 * Release every use of the peer handles before priv goes away:
		 * stop an in-flight transfer and drop the DMA callback whose
		 * context is priv. The clock, trigger and DMA handles themselves
		 * belong to the application.
		 */
		if (priv->async_in_progress && priv->offload_dma_chan)
			(void)capi_dma_xfer_abort(priv->offload_dma_chan);
		/* A capture left the offload / TX stream running. */
		if (priv->offload_running)
			fpga_spi_engine_capture_end(priv);
		priv->async_in_progress = false;
		priv->capture_active = false;
		fpga_spi_engine_offload_stop(priv);
		if (priv->offload_dma_chan)
			(void)capi_dma_register_complete_callback(
				priv->offload_dma_chan, NULL, NULL);
		fpga_spi_engine_wr(priv->regmap_base, FPGA_SPI_ENGINE_REG_RESET, 1U);
		priv->regmap_base = 0U;
	}

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	} else {
		handle->ops = NULL;
	}

	return 0;
}

/**
 * @brief Run a synchronous SPI transceive operation.
 * @note SPI Engine: PIO through CMD/SDO/SDI FIFOs, completion via SYNC_ID.
 * @param device - Pointer to the CAPI SPI device.
 * @param transfer - Pointer to the transfer description.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_spi_engine_transceive(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	struct fpga_spi_engine_priv *priv;
	uint16_t n;
	int ret;

	if (!device || !transfer || !device->controller ||
	    !device->controller->priv)
		return -EINVAL;
	priv = device->controller->priv;
	ret = fpga_spi_engine_dev_bind(priv, device, FPGA_SPI_ENGINE_FLAG_READ_ONLY |
				       FPGA_SPI_ENGINE_FLAG_WRITE_ONLY | FPGA_SPI_ENGINE_FLAG_NO_CS);
	if (ret)
		return ret;

	n = transfer->tx_size > transfer->rx_size ?
	    transfer->tx_size : transfer->rx_size;

	return fpga_spi_engine_sync_xfer(priv, device->max_speed_hz, transfer->tx_buf,
					 transfer->tx_size, transfer->rx_buf,
					 transfer->rx_size, n, 0U);
}

/**
 * @brief Run a write-then-read in one chip-select frame.
 * @note SPI Engine: PIO, clocks tx_size + rx_size bytes and keeps the rx phase.
 * @param device - Pointer to the CAPI SPI device.
 * @param transfer - Pointer to the transfer description.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_spi_engine_read_command(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	struct fpga_spi_engine_priv *priv;
	uint16_t n;
	int ret;

	if (!device || !transfer || !device->controller ||
	    !device->controller->priv)
		return -EINVAL;
	priv = device->controller->priv;
	ret = fpga_spi_engine_dev_bind(priv, device, FPGA_SPI_ENGINE_FLAG_NO_CS);
	if (ret)
		return ret;

	/* Tx then Rx in one CS frame: clock tx_size + rx_size bytes, keep the
	 * bytes received during the rx phase. */
	n = (uint16_t)(transfer->tx_size + transfer->rx_size);

	return fpga_spi_engine_sync_xfer(priv, device->max_speed_hz, transfer->tx_buf,
					 transfer->tx_size, transfer->rx_buf,
					 transfer->rx_size, n, transfer->tx_size);
}

/**
 * @brief Arm one offload run: the chunk of async_chunk words at async_pos.
 * @note SPI Engine: spi_engine_offload_transfer order -- OFFLOAD_RESET, load
 *       offload 0 CMD/SDO memory, start the sample DMA into the chunk's slice
 *       of sample_buf, OFFLOAD_CTRL = 1, then enable the trigger timer channel.
 *       CS is opened by the first chunk and closed by the last, so a transfer
 *       longer than the offload SDO memory stays one chip-select frame.
 * @param priv - Pointer to the backend private handle.
 * @return 0 on success, negative errno on failure (offload left stopped).
 */
static int fpga_spi_engine_async_arm(struct fpga_spi_engine_priv *priv)
{
	uint32_t regmap = priv->regmap_base;
	uint16_t pos = priv->async_pos;
	uint16_t chunk = priv->async_words - pos;
	uint16_t end;
	uint16_t w;
	int ret;

	/*
	 * A non-final chunk is cut so the next one's slice of sample_buf starts
	 * on a FPGA_SPI_ENGINE_CHUNK_ALIGN boundary: the sample DMA refuses a
	 * destination that is not aligned to its memory-side beat (64-bit on
	 * the usual offload DMAC).
	 */
	if (chunk > priv->offload_sdo_depth) {
		chunk = (uint16_t)(((priv->offload_sdo_depth * priv->sample_len) /
				    FPGA_SPI_ENGINE_CHUNK_ALIGN) *
				   FPGA_SPI_ENGINE_CHUNK_ALIGN / priv->sample_len);
		if (chunk == 0U)
			return -EINVAL;
	}
	priv->async_chunk = chunk;
	end = (uint16_t)(pos + chunk);

	/* Program the offload command memory and SDO memory. */
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 1U);
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 0U);

	(void)fpga_spi_engine_emit_program(priv, chunk, pos == 0U,
					   end == priv->async_words,
					   fpga_spi_engine_sink_offload_cmd, regmap,
					   priv->async_speed_hz);
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			   FPGA_SPI_ENGINE_CMD_SYNC(priv->sync_id));

	for (w = pos; w < end; w++)
		fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0),
				   fpga_spi_engine_pack_word(priv->async_tx,
						   priv->async_tx_size,
						   priv->data_width,
						   priv->word_len, w,
						   priv->lsb_first));

	/*
	 * Arm the offload sample-capture DMA (DEV_TO_MEM into this chunk's slice
	 * of sample_buf) through the composed capi_dma channel. With a completion
	 * callback registered the DMA backend is on its interrupt path, so
	 * xfer_start returns immediately with the S2MM waiting for the streamed
	 * SDI samples; the DMA backend flushes and later invalidates the buffer.
	 */
	priv->dma_xfer.src = 0U;
	priv->dma_xfer.dst = (capi_dma_glbl_addr_t)(uintptr_t)
			     &priv->sample_buf[(uint32_t)pos * priv->sample_len];
	priv->dma_xfer.length = (uint32_t)chunk * priv->sample_len;
	priv->dma_xfer.xfer_type = CAPI_DMA_DEV_TO_MEM;
	priv->dma_xfer.src_inc = CAPI_DMA_NO_INCREMENT;
	priv->dma_xfer.dst_inc = CAPI_DMA_BYTE_INCREMENT;
	priv->dma_xfer.extra = NULL;

	ret = capi_dma_config_xfer(priv->offload_dma_chan, &priv->dma_xfer);
	if (ret)
		goto err;

	/* spi_engine_offload_transfer order: enable offload, then start the
	 * sample DMA. Nothing runs until the trigger pulses. */
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0),
			   FPGA_SPI_ENGINE_OFFLOAD_CTRL_ENABLE);

	ret = capi_dma_xfer_start(priv->offload_dma_chan);
	if (ret)
		goto err;

	/* Start the trigger generator: without a pulse on the engine's trigger
	 * input the offload program never executes and the DMA never completes. */
	ret = capi_timer_channel_enable(priv->trigger, priv->trigger_chan);
	if (ret) {
		(void)capi_dma_xfer_abort(priv->offload_dma_chan);
		goto err;
	}

	return 0;

err:
	fpga_spi_engine_offload_stop(priv);
	return ret;
}

/**
 * @brief Start an asynchronous transfer of n framed bytes; on completion copy
 *        rx_size bytes starting at rx_offset back to rx.
 * @param device - Pointer to the CAPI SPI device.
 * @param tx - Transmit buffer (NULL sends zeros).
 * @param tx_size - Transmit buffer size in bytes.
 * @param rx - Receive buffer (NULL discards).
 * @param rx_size - Number of bytes to copy back to rx.
 * @param n - Number of bytes framed on the bus.
 * @param rx_offset - Offset of the first byte copied back to rx.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_spi_engine_async_start(struct capi_spi_device *device,
				       const uint8_t *tx, uint16_t tx_size,
				       uint8_t *rx, uint16_t rx_size,
				       uint32_t n, uint16_t rx_offset)
{
	struct fpga_spi_engine_priv *priv = device->controller->priv;
	uint32_t words;
	int ret;

	/* The offload path needs both the sample DMA and the trigger. */
	if (priv->offload_dma_chan == NULL || priv->trigger == NULL)
		return -ENOTSUP;
	if (priv->async_in_progress)
		return -EBUSY;
	if (n == 0U)
		return -EINVAL;

	/* The whole capture must fit sample_buf, each offload program the
	 * command memory; longer than the SDO memory runs in chunks. */
	words = (n + priv->word_len - 1U) / priv->word_len;
	if (words * priv->sample_len > FPGA_SPI_ENGINE_MAX_XFER_BYTES ||
	    priv->offload_sdo_depth == 0U ||
	    FPGA_SPI_ENGINE_OFFLOAD_PROGRAM_LEN > priv->offload_cmd_depth)
		return -EINVAL;

	/* No SDI words in a WRITE: the sample DMA would never complete. */
	ret = fpga_spi_engine_dev_bind(priv, device, FPGA_SPI_ENGINE_FLAG_READ_ONLY |
				       FPGA_SPI_ENGINE_FLAG_NO_CS);
	if (ret)
		return ret;

	/* Cache framing for the chunk re-arm and the completion callback. */
	priv->async_tx = tx;
	priv->async_tx_size = tx_size;
	priv->async_rx = rx;
	priv->async_rx_size = rx_size;
	priv->async_rx_offset = rx_offset;
	priv->async_words = (uint16_t)words;
	priv->async_pos = 0U;
	priv->async_speed_hz = device->max_speed_hz;

	ret = capi_dma_register_complete_callback(priv->offload_dma_chan,
			fpga_spi_engine_dma_done, priv);
	if (ret)
		return ret;

	priv->async_in_progress = true;
	ret = fpga_spi_engine_async_arm(priv);
	if (ret)
		priv->async_in_progress = false;

	return ret;
}

/**
 * @brief Start an asynchronous SPI transceive operation.
 * @note SPI Engine: offload 0 replays a CMD/SDO program per trigger pulse and
 *       streams the SDI samples to the offload DMA (fpga_spi_engine_async_arm). A
 *       transfer longer than the offload SDO memory runs as consecutive
 *       offload programs inside one chip-select frame.
 *
 * Non-blocking: needs an IRQ-driven offload DMA channel. Completion is
 * reported from the DMA end-of-transfer callback (fpga_spi_engine_dma_done), which
 * fires CAPI_SPI_EVENT_XFR_DONE or an error event. The trigger period must be
 * longer than one program run, or the generator starts further runs before
 * completion stops it (they are discarded by fpga_spi_engine_offload_stop()).
 * @param device - Pointer to the CAPI SPI device.
 * @param transfer - Pointer to the transfer description.
 * @return 0 on success, or:
 *         -EINVAL   bad arguments, or more bytes than FPGA_SPI_ENGINE_MAX_XFER_BYTES
 *         -ENOTSUP  no offload DMA / trigger composed
 *         -EBUSY    an async transfer is already in flight
 *         other     capi_dma / capi_timer error
 */
static int fpga_capi_spi_engine_transceive_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	uint16_t n;

	if (!device || !transfer || !device->controller ||
	    !device->controller->priv)
		return -EINVAL;

	n = transfer->tx_size > transfer->rx_size ?
	    transfer->tx_size : transfer->rx_size;

	return fpga_spi_engine_async_start(device, transfer->tx_buf, transfer->tx_size,
					   transfer->rx_buf, transfer->rx_size, n, 0U);
}

/**
 * @brief Start an asynchronous write-then-read in one chip-select frame.
 * @note SPI Engine: the offload path of transceive_async, clocking
 *       tx_size + rx_size bytes and keeping the rx phase.
 * @param device - Pointer to the CAPI SPI device.
 * @param transfer - Pointer to the transfer description.
 * @return 0 on success, negative errno on failure (see transceive_async).
 */
static int fpga_capi_spi_engine_read_command_async(struct capi_spi_device
		*device,
		struct capi_spi_transfer *transfer)
{
	if (!device || !transfer || !device->controller ||
	    !device->controller->priv)
		return -EINVAL;

	return fpga_spi_engine_async_start(device, transfer->tx_buf, transfer->tx_size,
					   transfer->rx_buf, transfer->rx_size,
					   (uint32_t)transfer->tx_size + transfer->rx_size,
					   transfer->tx_size);
}

/**
 * @brief Abort an in-flight asynchronous transfer.
 * @note SPI Engine: aborts the sample DMA, then fpga_spi_engine_offload_stop()
 *       (trigger off, offload off, wait idle or core reset, OFFLOAD_RESET).
 * @param device - Pointer to the CAPI SPI device.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_spi_engine_abort_async(struct capi_spi_device *device)
{
	struct fpga_spi_engine_priv *priv;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = device->controller->priv;

	if (priv->async_in_progress) {
		/* Halt the sample DMA, then stop the trigger and offload stream. */
		if (priv->offload_dma_chan)
			(void)capi_dma_xfer_abort(priv->offload_dma_chan);
		if (priv->stream_tx_chan) {
			(void)capi_dma_xfer_abort(priv->stream_tx_chan);
			priv->stream_tx_chan = NULL;
		}
		priv->async_in_progress = false;
		priv->capture_active = false;
		fpga_spi_engine_offload_stop(priv);

		if (priv->callback)
			priv->callback(CAPI_SPI_EVENT_ERROR,
				       priv->callback_arg, 0);
	}

	return 0;
}

/**
 * @brief SPI interrupt handler (for a user-managed vector or polling).
 * @note SPI Engine: the engine raises no interrupt of its own on this path;
 *       completion is the offload DMA end-of-transfer, so this services that
 *       DMA channel, which runs fpga_spi_engine_dma_done() when the run is over.
 * @param handle - Pointer to the CAPI handle.
 */
static void fpga_capi_spi_engine_isr(void *handle)
{
	struct capi_spi_controller_handle *h = handle;
	struct fpga_spi_engine_priv *priv;

	if (!h || !h->priv)
		return;

	priv = h->priv;
	if (priv->async_in_progress && priv->offload_dma_chan)
		(void)capi_dma_isr_chan(priv->offload_dma_chan);
}

/**
 * @brief Register the asynchronous completion callback.
 * @note SPI Engine: stored on the handle; no register access.
 * @param handle - Pointer to the CAPI handle.
 * @param callback - Completion callback (NULL to unregister).
 * @param callback_arg - User argument passed to the callback.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_spi_engine_register_callback(struct
		capi_spi_controller_handle
		*handle,
		capi_spi_callback_t const callback, void *callback_arg)
{
	struct fpga_spi_engine_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	priv->callback = callback;
	priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Set chip-select state.
 * @note SPI Engine: issues an ASSERT command through the PIO command FIFO and
 *       waits for its SYNC. MANUAL_ASSERT / MANUAL_DEASSERT drive the device's
 *       CS line and make later transfers leave CS alone; AUTO deasserts CS and
 *       returns CS framing to the transfer programs.
 * @param device - Pointer to the CAPI SPI device.
 * @param cs - Requested chip-select state.
 * @return 0 on success, or:
 *         -EINVAL     bad arguments or unknown cs value
 *         -EBUSY      an async transfer is in flight
 *         -ETIMEDOUT  the command was not executed
 */
static int fpga_capi_spi_engine_set_cs(struct capi_spi_device *device,
				       enum capi_spi_cs_control cs)
{
	struct fpga_spi_engine_priv *priv;
	uint32_t regmap, sync;
	uint8_t mask = 0xFFU;
	int ret;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = device->controller->priv;
	if (priv->async_in_progress)
		return -EBUSY;

	switch (cs) {
	case CAPI_SPI_CS_AUTO:
	case CAPI_SPI_CS_MANUAL_DEASSERT:
		break;
	case CAPI_SPI_CS_MANUAL_ASSERT:
		if (fpga_spi_engine_cs_index(device->native_cs) >= FPGA_SPI_ENGINE_NUM_CS)
			return -EINVAL;
		priv->chip_select = fpga_spi_engine_cs_index(device->native_cs);
		mask = (uint8_t)(0xFFU ^ (1U << priv->chip_select));
		break;
	default:
		return -EINVAL;
	}

	regmap = priv->regmap_base;
	sync = (priv->sync_id = (priv->sync_id + 1U) & 0xFFU);

	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);
	ret = fpga_spi_engine_push_cmd(regmap,
				       FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, mask));
	if (!ret)
		ret = fpga_spi_engine_push_cmd(regmap, FPGA_SPI_ENGINE_CMD_SYNC(sync));
	if (!ret)
		ret = fpga_spi_engine_wait_sync(regmap, sync);
	if (ret) {
		fpga_spi_engine_core_reset(regmap);
		priv->cs_manual = false;
		return ret;
	}

	priv->cs_manual = (cs != CAPI_SPI_CS_AUTO);

	return 0;
}

/**
 * @brief Offload sample-capture DMA completion callback (capi_dma).
 *
 * Runs from the DMA backend's interrupt on end-of-transfer: stop the trigger and
 * the offload stream (fpga_spi_engine_offload_stop), unpack the captured SDI samples (already made coherent by
 * the DMA backend's cache invalidate) into the caller's rx buffer, and fire one
 * CAPI_SPI_EVENT_XFR_DONE. A non-zero event is reported as an error.
 * @param event - DMA completion event (0 = success).
 * @param ctx - Callback context (the channel private data).
 */
static void fpga_spi_engine_dma_done(uint32_t event, void *ctx)
{
	struct fpga_spi_engine_priv *priv = ctx;
	uint8_t word_len;
	uint8_t rxtmp[FPGA_SPI_ENGINE_MAX_XFER_BYTES];
	uint16_t w;

	if (!priv || !priv->async_in_progress)
		return;

	/* The DMA captured one run: stop the trigger and the offload stream. */
	fpga_spi_engine_offload_stop(priv);

	/* More words left: run the next chunk inside the same CS frame. */
	if (event == 0U) {
		priv->async_pos += priv->async_chunk;
		if (priv->async_pos < priv->async_words) {
			if (!fpga_spi_engine_async_arm(priv))
				return;
			event = 1U;
		}
	}

	priv->async_in_progress = false;
	if (event != 0U) {
		if (priv->callback)
			priv->callback(CAPI_SPI_EVENT_ERROR,
				       priv->callback_arg, 0);
		return;
	}

	word_len = priv->word_len;
	for (w = 0; w < priv->async_words; w++) {
		uint32_t word = fpga_spi_engine_sample_word(
					&priv->sample_buf[w * priv->sample_len],
					word_len);
		fpga_spi_engine_unpack_word(word, &rxtmp[w * word_len],
					    priv->data_width, word_len,
					    priv->lsb_first);
	}

	if (priv->async_rx && priv->async_rx_size)
		memcpy(priv->async_rx, &rxtmp[priv->async_rx_offset],
		       priv->async_rx_size);

	if (priv->callback)
		priv->callback(CAPI_SPI_EVENT_XFR_DONE, priv->callback_arg, 0);
}

/* --- public helpers ------------------------------------------------------- */

/**
 * @brief Private handle of a valid controller handle.
 * @param handle - SPI controller handle.
 * @return Private handle, or NULL.
 */
static struct fpga_spi_engine_priv *
fpga_spi_engine_to_priv(struct capi_spi_controller_handle *handle)
{
	struct fpga_spi_engine_priv *priv;

	if (!handle || !handle->priv)
		return NULL;
	priv = handle->priv;

	return priv->regmap_base ? priv : NULL;
}

int fpga_spi_engine_set_transfer_width(struct capi_spi_controller_handle
				       *handle,
				       uint8_t bits)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	if (!priv || bits == 0U || bits > 32U)
		return -EINVAL;
	if (priv->async_in_progress)
		return -EBUSY;

	/* spi_engine_set_transfer_width: clamp to the core DATA_WIDTH. */
	if (priv->max_data_width && bits > priv->max_data_width)
		bits = priv->max_data_width;
	priv->data_width = bits;
	priv->word_len = (uint8_t)((bits + 7U) / 8U);

	return 0;
}

uint8_t fpga_spi_engine_get_transfer_width(struct capi_spi_controller_handle
		*handle)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	return priv ? priv->data_width : 0U;
}

uint8_t fpga_spi_engine_get_max_data_width(struct capi_spi_controller_handle
		*handle)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	return priv ? priv->max_data_width : 0U;
}

uint8_t fpga_spi_engine_get_num_sdi(struct capi_spi_controller_handle *handle)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	return priv ? priv->num_sdi : 0U;
}

uint8_t fpga_spi_engine_get_sample_len(struct capi_spi_controller_handle
				       *handle)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	return priv ? priv->sample_len : 0U;
}

uint32_t fpga_spi_engine_get_version(struct capi_spi_controller_handle *handle)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	return priv ? priv->version : 0U;
}

/**
 * @brief SLEEP argument for ns at speed_hz (fpga_spi_engine_sleep_arg()).
 * @param priv - Pointer to the backend private handle.
 * @param speed_hz - SCLK rate the program runs at.
 * @param ns - Delay in ns.
 * @return SLEEP argument t (sleeps t + 1 SCLK periods).
 */
static uint8_t fpga_spi_engine_sleep_ticks(const struct fpga_spi_engine_priv
		*priv, uint32_t speed_hz, uint32_t ns)
{
	uint64_t period_ns, t;

	if (priv->ref_clk_hz == 0U)
		return 0U;

	/* One SCLK period = 2 * (CLK_DIV + 1) reference clocks. */
	period_ns = (2ULL * (fpga_spi_engine_clk_div(priv->ref_clk_hz, speed_hz) + 1U) *
		     1000000000ULL) / priv->ref_clk_hz;
	if (period_ns == 0U)
		period_ns = 1U;
	t = ((uint64_t)ns + period_ns - 1U) / period_ns;
	if (t > 0U)
		t--;

	return t > 0xFFU ? 0xFFU : (uint8_t)t;
}

uint8_t fpga_spi_engine_sleep_arg(struct capi_spi_controller_handle *handle,
				  uint32_t speed_hz, uint32_t ns)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	return priv ? fpga_spi_engine_sleep_ticks(priv, speed_hz, ns) : 0U;
}

int fpga_spi_engine_set_speed(struct capi_spi_device *device, uint32_t speed_hz)
{
	if (!device || speed_hz == 0U)
		return -EINVAL;
	device->max_speed_hz = speed_hz;

	return 0;
}

int fpga_spi_engine_reg_write(struct capi_spi_controller_handle *handle,
			      uint32_t reg, uint32_t val)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	if (!priv)
		return -EINVAL;
	fpga_spi_engine_wr(priv->regmap_base, reg, val);

	return 0;
}

int fpga_spi_engine_reg_read(struct capi_spi_controller_handle *handle,
			     uint32_t reg, uint32_t *val)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	if (!priv || !val)
		return -EINVAL;
	*val = fpga_spi_engine_rd(priv->regmap_base, reg);

	return 0;
}

struct capi_dma_chan *fpga_spi_engine_get_offload_dma(
	struct capi_spi_controller_handle *handle)
{
	struct fpga_spi_engine_priv *priv = fpga_spi_engine_to_priv(handle);

	return priv ? priv->offload_dma_chan : NULL;
}

/**
 * @brief Translate offload message commands to core instructions.
 * @note spi_engine_write_cmd(): TRANSFER bytes -> ceil(bytes / word bytes)
 *       words; CS_HIGH (0xFF) / CS_LOW (0x00) -> ASSERT with cs_delay and the
 *       device CS; SLEEP ns -> SCLK periods; CONFIG and SYNC unchanged.
 *       Differs from legacy (legacy bugs): other ASSERT masks and SYNC/SLEEP
 *       modifiers are kept (legacy dropped them), a 0-byte TRANSFER is
 *       refused (legacy wrote 255 + 1 words), SLEEP uses the 0..255 ns
 *       argument through the ref-clock divider (legacy masked it to 0..15 and
 *       divided by the SCLK divider twice).
 * @param priv - Pointer to the backend private handle (device bound).
 * @param speed_hz - Device SCLK rate.
 * @param in - Message commands.
 * @param n - Number of commands.
 * @param out - Receives n instructions.
 * @return 0 on success, -EINVAL on a 0-byte TRANSFER or an unknown opcode.
 */
static int fpga_spi_engine_translate(const struct fpga_spi_engine_priv *priv,
				     uint32_t speed_hz, const uint16_t *in,
				     uint32_t n, uint16_t *out)
{
	uint32_t i, inst, mod, arg, words;

	for (i = 0; i < n; i++) {
		inst = (in[i] >> 12) & 0xFU;
		mod = (in[i] >> 8) & 0xFU;
		arg = in[i] & 0xFFU;
		switch (inst) {
		case FPGA_SPI_ENGINE_INST_TRANSFER:
			if (arg == 0U)
				return -EINVAL;
			words = (arg + priv->word_len - 1U) / priv->word_len;
			out[i] = FPGA_SPI_ENGINE_CMD(inst, mod, words - 1U);
			break;
		case FPGA_SPI_ENGINE_INST_ASSERT:
			if (arg == 0xFFU)
				out[i] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, 0xFFU);
			else if (arg == 0x00U)
				out[i] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay,
								    0xFFU ^ (1U << priv->chip_select));
			else
				out[i] = in[i];
			break;
		case FPGA_SPI_ENGINE_INST_SYNC_SLEEP:
			out[i] = (mod == 0x1U) ?
				 FPGA_SPI_ENGINE_CMD_SLEEP(fpga_spi_engine_sleep_ticks(priv,
							   speed_hz, arg)) : in[i];
			break;
		case FPGA_SPI_ENGINE_INST_CONFIG:
			out[i] = in[i];
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * @brief SDO / SDI words one program run moves.
 * @param cmds - Raw program.
 * @param n - Number of commands.
 * @param sdo - Receives the SDO words written per run.
 * @param sdi - Receives the SDI words read per run.
 * @param total - Receives all TRANSFER words per run (legacy offload_tx_len).
 */
static void fpga_spi_engine_prog_words(const uint16_t *cmds, uint32_t n,
				       uint32_t *sdo, uint32_t *sdi,
				       uint32_t *total)
{
	uint32_t i, w;

	*sdo = 0U;
	*sdi = 0U;
	*total = 0U;
	for (i = 0; i < n; i++) {
		if (((cmds[i] >> 12) & 0x7U) != FPGA_SPI_ENGINE_INST_TRANSFER)
			continue;
		w = (cmds[i] & 0xFFU) + 1U;
		*total += w;
		if (cmds[i] & (FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_W << 8))
			*sdo += w;
		if (cmds[i] & (FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_R << 8))
			*sdi += w;
	}
}

/**
 * @brief Load the offload program and SDO memory; report words per run.
 * @param priv - Pointer to the backend private handle (device bound).
 * @param device - SPI device.
 * @param msg - Program description.
 * @param sdo_words - Receives the SDO words per run.
 * @param sdi_words - Receives the SDI words per run.
 * @return 0 on success, -EINVAL if it does not fit.
 */
static int fpga_spi_engine_offload_program(struct fpga_spi_engine_priv *priv,
		const struct capi_spi_device *device,
		const struct fpga_spi_engine_offload_msg *msg,
		uint32_t *sdo_words, uint32_t *sdi_words)
{
	uint32_t regmap = priv->regmap_base;
	uint16_t prog[FPGA_SPI_ENGINE_MAX_PROG];
	const uint16_t *cmds = msg->cmds;
	uint32_t i, mask, total, n_sdo;
	int ret;

	if (cmds) {
		/* CLK_DIV, XFER_LEN, CONFIG + program + SYNC. */
		if (msg->n_cmds == 0U ||
		    msg->n_cmds + 4U > priv->offload_cmd_depth ||
		    msg->n_cmds > FPGA_SPI_ENGINE_MAX_PROG)
			return -EINVAL;
		if (!msg->raw_cmds) {
			ret = fpga_spi_engine_translate(priv, device->max_speed_hz,
							msg->cmds, msg->n_cmds, prog);
			if (ret)
				return ret;
			cmds = prog;
		}
		fpga_spi_engine_prog_words(cmds, msg->n_cmds, sdo_words,
					   sdi_words, &total);
	} else {
		if (msg->words == 0U || msg->words > 256U ||
		    FPGA_SPI_ENGINE_OFFLOAD_PROGRAM_LEN > priv->offload_cmd_depth)
			return -EINVAL;
		*sdo_words = (priv->xfer_flags & FPGA_SPI_ENGINE_FLAG_READ_ONLY) ?
			     0U : msg->words;
		*sdi_words = (priv->xfer_flags & FPGA_SPI_ENGINE_FLAG_WRITE_ONLY) ?
			     0U : msg->words;
		total = msg->words;
	}
	/* legacy pushes offload_tx_len (all TRANSFER words) SDO entries. */
	n_sdo = (msg->n_sdo == 0U && msg->sdo) ? total : msg->n_sdo;
	if (n_sdo > priv->offload_sdo_depth || (n_sdo && !msg->sdo))
		return -EINVAL;
	/* An SDO memory shorter than the program would stall the engine. */
	if (!msg->tx_dma_chan && n_sdo < *sdo_words)
		return -EINVAL;

	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 1U);
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 0U);

	if (msg->cmds) {
		/* spi_engine_compile_message prefix. */
		fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
				   FPGA_SPI_ENGINE_CMD_CONFIG(
					   FPGA_SPI_ENGINE_CMD_REG_CLK_DIV,
					   fpga_spi_engine_clk_div(priv->ref_clk_hz,
							   device->max_speed_hz)));
		fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
				   FPGA_SPI_ENGINE_CMD_CONFIG(
					   FPGA_SPI_ENGINE_CMD_DATA_TRANSFER_LEN,
					   priv->data_width));
		fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
				   FPGA_SPI_ENGINE_CMD_CONFIG(
					   FPGA_SPI_ENGINE_CMD_REG_CONFIG,
					   fpga_spi_engine_cfg_value(priv->mode,
							   priv->sdo_idle_state,
							   priv->three_wire)));
		for (i = 0; i < msg->n_cmds; i++)
			fpga_spi_engine_wr(regmap,
					   FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
					   cmds[i]);
	} else {
		(void)fpga_spi_engine_emit_program(priv, (uint16_t)msg->words, true,
						   true, fpga_spi_engine_sink_offload_cmd,
						   regmap, device->max_speed_hz);
	}
	fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
			   FPGA_SPI_ENGINE_CMD_SYNC(priv->sync_id));

	mask = priv->data_width >= 32U ? 0xFFFFFFFFU :
	       ((1U << priv->data_width) - 1U);
	for (i = 0; i < n_sdo; i++)
		fpga_spi_engine_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0),
				   msg->sdo[i] & mask);

	return 0;
}

int fpga_spi_engine_offload_load(struct capi_spi_device *device,
				 const struct fpga_spi_engine_offload_msg *msg)
{
	struct fpga_spi_engine_priv *priv;
	uint32_t sdo_words, sdi_words;
	int ret;

	if (!device || !msg || !device->controller)
		return -EINVAL;
	priv = fpga_spi_engine_to_priv(device->controller);
	if (!priv)
		return -EINVAL;

	ret = fpga_spi_engine_dev_bind(priv, device, FPGA_SPI_ENGINE_FLAG_READ_ONLY |
				       FPGA_SPI_ENGINE_FLAG_WRITE_ONLY | FPGA_SPI_ENGINE_FLAG_NO_CS);
	if (ret)
		return ret;

	return fpga_spi_engine_offload_program(priv, device, msg, &sdo_words,
					       &sdi_words);
}

/**
 * @brief Capture RX DMA completion (fpga_spi_engine_offload_capture).
 * @param event - DMA completion event (0 = success).
 * @param ctx - Backend private handle.
 */
static void fpga_spi_engine_capture_rx_done(uint32_t event, void *ctx)
{
	struct fpga_spi_engine_priv *priv = ctx;

	if (!priv || !priv->capture_active)
		return;
	if (event)
		priv->capture_event = event;
	priv->capture_rx_done = true;
}

/**
 * @brief Stop what a capture left running: TX stream, trigger, offload.
 * @note The TX DMAC is aborted with the offload (and trigger) still running:
 *       a stream destination only finishes disabling once its pending beat
 *       is taken, and the offload takes SDO only while it executes. A sync
 *       transfer may have disabled the offload since, so it is re-enabled
 *       for the drain.
 * @param priv - Pointer to the backend private handle.
 */
static void fpga_spi_engine_capture_end(struct fpga_spi_engine_priv *priv)
{
	if (priv->stream_tx_chan) {
		fpga_spi_engine_wr(priv->regmap_base,
				   FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0),
				   FPGA_SPI_ENGINE_OFFLOAD_CTRL_ENABLE);
		if (priv->trigger)
			(void)capi_timer_channel_enable(priv->trigger,
							priv->trigger_chan);
		(void)capi_dma_xfer_abort(priv->stream_tx_chan);
		(void)capi_dma_register_complete_callback(priv->stream_tx_chan,
				NULL, NULL);
		priv->stream_tx_chan = NULL;
	}
	if (priv->offload_dma_chan)
		(void)capi_dma_register_complete_callback(priv->offload_dma_chan,
				NULL, NULL);
	fpga_spi_engine_offload_stop(priv);
	priv->offload_running = false;
	priv->capture_active = false;
	priv->async_in_progress = false;
}

/**
 * @brief DMA stream width of a capture channel (legacy width_src).
 * @param chan - Channel.
 * @param dev_side_src - The stream is the channel's source (RX).
 * @param fallback - Width when the channel is not an AXI DMAC.
 * @return Bytes per stream beat.
 */
static uint32_t fpga_spi_engine_dma_width(const struct capi_dma_chan *chan,
		bool dev_side_src, uint32_t fallback)
{
	uint32_t src = 0U, dst = 0U;

	if (fpga_dmac_get_width(chan, &src, &dst))
		return fallback;
	src = dev_side_src ? src : dst;

	return src ? src : fallback;
}

/* Validates a capture request; returns the offload direction bits or < 0. */
static int fpga_spi_engine_capture_check(struct fpga_spi_engine_priv *priv,
		const struct fpga_spi_engine_offload_msg *msg)
{
	uint8_t dir = msg->offload_config ? msg->offload_config :
		      priv->offload_config;

	/* legacy: offload disabled -> -1. */
	if (!(dir & FPGA_SPI_ENGINE_OFFLOAD_TX_RX_EN) || msg->n_samples == 0U ||
	    ((dir & FPGA_SPI_ENGINE_OFFLOAD_RX_EN) && !msg->rx_buf) ||
	    ((dir & FPGA_SPI_ENGINE_OFFLOAD_TX_EN) &&
	     (!msg->tx_dma_chan || !msg->tx_buf)))
		return -EINVAL;
	if ((dir & FPGA_SPI_ENGINE_OFFLOAD_RX_EN) && !priv->offload_dma_chan)
		return -ENOTSUP;

	return dir;
}

/* legacy: width_src * offload_tx_len * no_samples; 0 words or > 4 GiB fail. */
static int fpga_spi_engine_capture_len(const struct capi_dma_chan *chan,
				       bool dev_side_src, uint32_t words,
				       uint32_t n_samples, uint32_t sample_len,
				       uint64_t *len)
{
	*len = (uint64_t)words * n_samples *
	       fpga_spi_engine_dma_width(chan, dev_side_src, sample_len);

	return (words == 0U || *len > UINT32_MAX) ? -EINVAL : 0;
}

/* Registers the completion callback and programs one DMA transfer. */
static int fpga_spi_engine_dma_prepare(struct capi_dma_chan *chan,
				       struct capi_dma_transfer *xfer,
				       capi_dma_xfer_complete_cb cb, void *arg)
{
	int ret;

	ret = capi_dma_register_complete_callback(chan, cb, arg);
	if (ret)
		return ret;

	return capi_dma_config_xfer(chan, xfer);
}

/* legacy waits for the RX DMA only; TX keeps streaming. */
static int fpga_spi_engine_capture_wait_rx(struct fpga_spi_engine_priv *priv,
		uint32_t timeout_us)
{
	uint32_t waited;

	/* is_completed also finishes a polled-mode DMAC transfer. */
	for (waited = 0U; waited < timeout_us &&
	     !(capi_dma_chan_is_completed(priv->offload_dma_chan) &&
	       priv->capture_rx_done); waited += 10U)
		capi_wait_us(10U);
	if (!priv->capture_rx_done)
		return -ETIMEDOUT;
	if (priv->capture_event)
		return -EIO;

	return 0;
}

int fpga_spi_engine_offload_capture(struct capi_spi_device *device,
				    const struct fpga_spi_engine_offload_msg *msg,
				    uint32_t timeout_us)
{
	struct fpga_spi_engine_priv *priv;
	uint32_t sdo_words, sdi_words;
	uint64_t rx_len = 0U, tx_len = 0U;
	bool rx, tx;
	int ret;

	if (!device || !msg || !device->controller)
		return -EINVAL;
	priv = fpga_spi_engine_to_priv(device->controller);
	if (!priv)
		return -EINVAL;
	ret = fpga_spi_engine_capture_check(priv, msg);
	if (ret < 0)
		return ret;
	rx = ret & FPGA_SPI_ENGINE_OFFLOAD_RX_EN;
	tx = ret & FPGA_SPI_ENGINE_OFFLOAD_TX_EN;
	if (timeout_us == 0U)
		timeout_us = FPGA_SPI_ENGINE_CAPTURE_TIMEOUT_US;

	/* A previous capture left the offload running: stop it first. */
	if (priv->offload_running)
		fpga_spi_engine_capture_end(priv);

	ret = fpga_spi_engine_dev_bind(priv, device, FPGA_SPI_ENGINE_FLAG_READ_ONLY |
				       FPGA_SPI_ENGINE_FLAG_WRITE_ONLY | FPGA_SPI_ENGINE_FLAG_NO_CS);
	if (ret)
		return ret;

	ret = fpga_spi_engine_offload_program(priv, device, msg, &sdo_words,
					      &sdi_words);
	if (ret)
		return ret;

	/* SDI words with no RX DMA back-pressure the engine: use WRITE_ONLY. */
	if (!rx && sdi_words)
		return -EINVAL;
	if (rx) {
		ret = fpga_spi_engine_capture_len(priv->offload_dma_chan, true,
						  sdi_words, msg->n_samples,
						  priv->sample_len, &rx_len);
		if (ret)
			return ret;
	}
	if (tx) {
		ret = fpga_spi_engine_capture_len(msg->tx_dma_chan, false,
						  sdo_words, msg->n_samples,
						  priv->sample_len, &tx_len);
		if (ret)
			return ret;
	}

	priv->capture_active = true;
	priv->async_in_progress = true;
	priv->capture_rx_done = false;
	priv->capture_event = 0U;

	/* spi_engine_offload_transfer: OFFLOAD_CTRL = 1, then the DMAs. */
	fpga_spi_engine_wr(priv->regmap_base, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0),
			   FPGA_SPI_ENGINE_OFFLOAD_CTRL_ENABLE);
	priv->offload_running = true;

	if (tx) {
		priv->tx_cyclic_cfg = (struct fpga_dmac_xfer_config) {
			.cyclic = true,
		};
		priv->dma_tx_xfer = (struct capi_dma_transfer) {
			.src = (capi_dma_glbl_addr_t)(uintptr_t)msg->tx_buf,
			.length = (size_t)tx_len,
			.xfer_type = CAPI_DMA_MEM_TO_DEV,
			.src_inc = CAPI_DMA_BYTE_INCREMENT,
			.dst_inc = CAPI_DMA_NO_INCREMENT,
			.extra = msg->tx_xfer_extra ? msg->tx_xfer_extra :
				 (msg->tx_oneshot ? NULL : &priv->tx_cyclic_cfg),
		};
		/* Nothing waits on the TX completion (legacy). */
		ret = fpga_spi_engine_dma_prepare(msg->tx_dma_chan,
						  &priv->dma_tx_xfer, NULL, NULL);
		if (!ret) {
			priv->stream_tx_chan = msg->tx_dma_chan;
			ret = capi_dma_xfer_start(msg->tx_dma_chan);
		}
		if (ret)
			goto err;
	}

	if (rx) {
		priv->dma_xfer = (struct capi_dma_transfer) {
			.dst = (capi_dma_glbl_addr_t)(uintptr_t)msg->rx_buf,
			.length = (size_t)rx_len,
			.xfer_type = CAPI_DMA_DEV_TO_MEM,
			.src_inc = CAPI_DMA_NO_INCREMENT,
			.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		};
		ret = fpga_spi_engine_dma_prepare(priv->offload_dma_chan,
						  &priv->dma_xfer,
						  fpga_spi_engine_capture_rx_done,
						  priv);
		if (!ret)
			ret = capi_dma_xfer_start(priv->offload_dma_chan);
		if (ret)
			goto err;
	}

	/* Each trigger rising edge runs the program once (none composed: the
	 * engine runs on its external trigger, as legacy). */
	if (priv->trigger) {
		ret = capi_timer_channel_enable(priv->trigger, priv->trigger_chan);
		if (ret)
			goto err_rx;
	}

	if (rx) {
		ret = fpga_spi_engine_capture_wait_rx(priv, timeout_us);
		if (ret)
			goto err_rx;
		(void)capi_dma_register_complete_callback(priv->offload_dma_chan,
				NULL, NULL);
	}

	/* The offload stays enabled, as spi_engine_offload_transfer leaves it. */
	priv->capture_active = false;
	priv->async_in_progress = false;

	return 0;

err_rx:
	if (rx)
		(void)capi_dma_xfer_abort(priv->offload_dma_chan);
err:
	fpga_spi_engine_capture_end(priv);
	return ret;
}

int fpga_spi_engine_offload_stream_stop(struct capi_spi_device *device)
{
	struct fpga_spi_engine_priv *priv;

	if (!device || !device->controller)
		return -EINVAL;
	priv = fpga_spi_engine_to_priv(device->controller);
	if (!priv)
		return -EINVAL;

	if (priv->offload_running)
		fpga_spi_engine_capture_end(priv);

	return 0;
}

#ifdef __cplusplus
}
#endif
