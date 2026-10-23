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
 *   4. On DMA completion (fpga_spi_dma_done): stop the trigger and the
 *      offload, unpack the RX words and raise CAPI_SPI_EVENT_XFR_DONE.
 *
 * Data framing: byte buffers are packed into data_width-bit words, MSB first.
 * The engine only shifts MSB first; lsb_first reverses the bits of each byte.
 *
 * init enables the SCLK clock (if given), pulses the core RESET and clamps
 * data_width to the width the core was built with.
 *
 * See fpga_capi_spi.h for configuration.
 */

#include <capi_spi.h>
#include <capi_clk.h>
#include <capi_timer.h>
#include <capi_dma.h>
#include "fpga_capi_spi_priv.h"
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
static inline void fpga_spi_wr(uint32_t base, uint32_t reg, uint32_t val)
{
	CAPI_PLATFORM_IO_WRITE32(base + reg, val);
}

/**
 * @brief Read a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @return Register value.
 */
static inline uint32_t fpga_spi_rd(uint32_t base, uint32_t reg)
{
	return CAPI_PLATFORM_IO_READ32(base + reg);
}

/* --- static prototypes ---------------------------------------------------- */

static int fpga_capi_spi_init(struct capi_spi_controller_handle **handle,
			      const struct capi_spi_config *config);
static int fpga_capi_spi_deinit(struct capi_spi_controller_handle *handle);
static int fpga_capi_spi_transceive(struct capi_spi_device *device,
				    struct capi_spi_transfer *transfer);
static int fpga_capi_spi_transceive_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer,
		int timeout);
static int fpga_capi_spi_read_command(struct capi_spi_device *device,
				      struct capi_spi_transfer *transfer);
static int fpga_capi_spi_abort_async(struct capi_spi_device *device);
static int fpga_capi_spi_register_callback(struct capi_spi_controller_handle
		*handle,
		capi_spi_callback_t const callback, void *callback_arg);
static int fpga_capi_spi_set_cs(struct capi_spi_device *device,
				enum capi_spi_cs_control cs);
static void fpga_spi_dma_done(uint32_t event, void *ctx);

const struct capi_spi_ops fpga_capi_spi_ops = {
	.init = fpga_capi_spi_init,
	.deinit = fpga_capi_spi_deinit,
	.transceive = fpga_capi_spi_transceive,
	.transceive_async = fpga_capi_spi_transceive_async,
	.read_command = fpga_capi_spi_read_command,
	.read_command_async = NULL,
	.abort_async = fpga_capi_spi_abort_async,
	.register_callback = fpga_capi_spi_register_callback,
	.set_cs = fpga_capi_spi_set_cs,
	/* Completion is delivered by the offload DMA backend's own interrupt via
	 * fpga_spi_dma_done(); the SPI Engine has no interrupt of its own here. */
	.isr = NULL,
};

/* --- helpers -------------------------------------------------------------- */

/**
 * @brief Compute the SCLK divider: SCLK = ref_clk / (2 * (div + 1)).
 * @param ref_hz - Engine reference clock in Hz.
 * @param speed_hz - Requested SCLK rate in Hz.
 * @return Divider value for the CONFIG clock register.
 */
static uint32_t fpga_spi_clk_div(uint32_t ref_hz, uint32_t speed_hz)
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
 * @return CONFIG register value.
 */
static uint32_t fpga_spi_cfg_value(uint8_t mode, uint8_t sdo_idle)
{
	uint32_t cfg = 0U;

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
static uint8_t fpga_spi_cs_index(uint16_t native_cs)
{
	uint8_t cs = 0U;

	if (native_cs == 0U)
		return 0U;
	while (cs < 15U && !(native_cs & (1U << cs)))
		cs++;

	return cs;
}

/**
 * @brief Pack word w of a byte stream MSB-first (zero-filled past tx_size).
 * @param tx - Transmit buffer (NULL sends zeros).
 * @param tx_size - Transmit buffer size in bytes.
 * @param word_len - Word length in bytes.
 * @param w - Word index.
 * @param lsb_first - Bit-reverse each byte (device LSB-first).
 * @return Packed word.
 */
/**
 * @brief Reverse the bit order of one byte.
 * @param v - Byte.
 * @return v with bit 0 and bit 7 (etc.) swapped.
 */
static uint8_t fpga_spi_rev8(uint8_t v)
{
	v = (uint8_t)(((v & 0xF0U) >> 4) | ((v & 0x0FU) << 4));
	v = (uint8_t)(((v & 0xCCU) >> 2) | ((v & 0x33U) << 2));
	return (uint8_t)(((v & 0xAAU) >> 1) | ((v & 0x55U) << 1));
}

static uint32_t fpga_spi_pack_word(const uint8_t *tx, uint16_t tx_size,
				   uint8_t word_len, uint16_t w, bool lsb_first)
{
	uint32_t word = 0U;
	uint8_t b;

	for (b = 0; b < word_len; b++) {
		uint16_t idx = (uint16_t)(w * word_len + b);
		uint8_t v = (tx && idx < tx_size) ? tx[idx] : 0U;
		if (lsb_first)
			v = fpga_spi_rev8(v);
		word = (word << 8) | v;
	}

	return word;
}

/**
 * @brief Unpack a received word MSB-first into word_len bytes at dst.
 * @param word - Received word.
 * @param dst - Destination byte buffer.
 * @param word_len - Word length in bytes.
 * @param lsb_first - Bit-reverse each byte (device LSB-first).
 */
static void fpga_spi_unpack_word(uint32_t word, uint8_t *dst, uint8_t word_len,
				 bool lsb_first)
{
	uint8_t b;

	for (b = 0; b < word_len; b++) {
		dst[b] = (uint8_t)(word >> (8U * (word_len - 1U - b)));
		if (lsb_first)
			dst[b] = fpga_spi_rev8(dst[b]);
	}
}

/**
 * @brief Push one command into the PIO command FIFO (bounded room wait).
 * @param regmap - Base address of the SPI Engine register map.
 * @param cmd - Encoded SPI Engine command word.
 * @return 0 on success, -ETIMEDOUT if the FIFO never had room.
 */
static int fpga_spi_push_cmd(uint32_t regmap, uint32_t cmd)
{
	uint32_t spins = FPGA_SPI_ENGINE_SPIN_MAX;

	while (fpga_spi_rd(regmap, FPGA_SPI_ENGINE_REG_CMD_FIFO_ROOM) == 0U)
		if (--spins == 0U)
			return -ETIMEDOUT;
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_CMD_FIFO, cmd);

	return 0;
}

/**
 * @brief Wait until the engine retires the SYNC command with the given id.
 * @param regmap - Base address of the SPI Engine register map.
 * @param sync - SYNC id.
 * @return 0 on success, -ETIMEDOUT otherwise.
 */
static int fpga_spi_wait_sync(uint32_t regmap, uint32_t sync)
{
	uint32_t spins = FPGA_SPI_ENGINE_SPIN_MAX;

	while (fpga_spi_rd(regmap, FPGA_SPI_ENGINE_REG_SYNC_ID) != sync)
		if (--spins == 0U)
			return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Pulse the core RESET. Recovers from a
 *        stalled command stream; the offload memories must be reloaded.
 * @param regmap - Base address of the SPI Engine register map.
 */
static void fpga_spi_core_reset(uint32_t regmap)
{
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_RESET, 0x01);
	capi_wait_us(FPGA_SPI_ENGINE_RESET_US);
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_RESET, 0x00);
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
static void fpga_spi_offload_stop(struct fpga_spi_priv_handle *priv)
{
	uint32_t regmap = priv->regmap_base;
	uint32_t spins = FPGA_SPI_ENGINE_STOP_SPINS;

	if (priv->trigger)
		(void)capi_timer_channel_disable(priv->trigger, priv->trigger_chan);

	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);
	while ((fpga_spi_rd(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_STATUS(0)) &
		FPGA_SPI_ENGINE_OFFLOAD_STATUS_ENABLED) && --spins)
		;
	if (spins == 0U)
		fpga_spi_core_reset(regmap);

	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 1U);
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 0U);
}

/**
 * @brief Reassemble a captured little-endian sample into a word.
 * @param buf - Captured sample bytes.
 * @param word_len - Word length in bytes.
 * @return Reassembled word.
 */
static uint32_t fpga_spi_sample_word(const uint8_t *buf, uint8_t word_len)
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
 *        set_cs() control the three ASSERT commands are left out.
 * @param priv - Pointer to the backend private handle.
 * @param words - Number of transfer words (1..256).
 * @param push - Callback that consumes each command (PIO FIFO or offload
 *               memory).
 * @param sink - Register map base passed through to push.
 * @param speed_hz - Requested SCLK rate in Hz.
 * @return 0 on success, or the first error returned by push.
 */
static int fpga_spi_emit_program(struct fpga_spi_priv_handle *priv,
				 uint16_t words,
				 int (*push)(uint32_t, uint32_t),
				 uint32_t sink, uint32_t speed_hz)
{
	uint32_t clk_div = fpga_spi_clk_div(priv->ref_clk_hz, speed_hz);
	uint32_t cfg = fpga_spi_cfg_value(priv->mode, priv->sdo_idle_state);
	uint8_t cs_low = (uint8_t)(0xFFU ^ (1U << priv->chip_select));
	uint32_t prog[FPGA_SPI_ENGINE_OFFLOAD_PROGRAM_LEN];
	uint32_t n = 0U, i;
	int ret;

	prog[n++] = FPGA_SPI_ENGINE_CMD_CONFIG(FPGA_SPI_ENGINE_CMD_REG_CLK_DIV,
					       clk_div);
	prog[n++] = FPGA_SPI_ENGINE_CMD_CONFIG(
			    FPGA_SPI_ENGINE_CMD_DATA_TRANSFER_LEN, priv->data_width);
	prog[n++] = FPGA_SPI_ENGINE_CMD_CONFIG(FPGA_SPI_ENGINE_CMD_REG_CONFIG,
					       cfg);
	if (!priv->cs_manual) {
		prog[n++] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, 0xFFU);
		prog[n++] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, cs_low);
	}
	prog[n++] = FPGA_SPI_ENGINE_CMD_TRANSFER(
			    FPGA_SPI_ENGINE_INSTRUCTION_TRANSFER_RW,
			    (uint8_t)(words - 1U));
	if (!priv->cs_manual)
		prog[n++] = FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, 0xFFU);

	for (i = 0; i < n; i++) {
		ret = push(sink, prog[i]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Write a command into offload 0 command memory (fpga_spi_emit_program
 *        sink for the offload path).
 * @param regmap - Base address of the SPI Engine register map.
 * @param cmd - Encoded SPI Engine command word.
 * @return 0.
 */
static int fpga_spi_sink_offload_cmd(uint32_t regmap, uint32_t cmd)
{
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), cmd);

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
static int fpga_spi_sync_xfer(struct fpga_spi_priv_handle *priv,
			      uint32_t speed_hz, const uint8_t *tx,
			      uint16_t tx_size, uint8_t *rx, uint16_t rx_size,
			      uint16_t n, uint16_t rx_offset)
{
	uint32_t regmap = priv->regmap_base;
	uint8_t word_len = priv->word_len;
	uint16_t words = (uint16_t)((n + word_len - 1U) / word_len);
	uint8_t rxtmp[FPGA_SPI_MAX_XFER_BYTES];
	uint32_t sync, spins;
	uint16_t tx_w = 0U, rx_w = 0U;
	bool progress;
	int ret;

	if (n == 0U)
		return 0;
	/* Whole words are framed: bound the rounded-up size, not n. */
	if ((uint32_t)words * word_len > FPGA_SPI_MAX_XFER_BYTES)
		return -EINVAL;
	if (priv->async_in_progress)
		return -EBUSY;

	sync = (priv->sync_id = (priv->sync_id + 1U) & 0xFFU);

	/* spi_engine_write_and_read: offload off while the FIFOs are used. */
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);

	ret = fpga_spi_emit_program(priv, words, fpga_spi_push_cmd, regmap,
				    speed_hz);
	if (!ret)
		ret = fpga_spi_push_cmd(regmap, FPGA_SPI_ENGINE_CMD_SYNC(sync));
	if (ret)
		goto timeout;

	/*
	 * Feed SDO and drain SDI together. The engine stalls once the SDI FIFO
	 * is full, so filling SDO first (as spi_engine_transfer_message does)
	 * only works up to the FIFO depth.
	 */
	spins = FPGA_SPI_ENGINE_SPIN_MAX;
	while (rx_w < words) {
		progress = false;
		if (tx_w < words &&
		    fpga_spi_rd(regmap, FPGA_SPI_ENGINE_REG_SDO_FIFO_ROOM)) {
			fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_SDO_DATA_FIFO,
				    fpga_spi_pack_word(tx, tx_size, word_len, tx_w,
						       priv->lsb_first));
			tx_w++;
			progress = true;
		}
		if (fpga_spi_rd(regmap, FPGA_SPI_ENGINE_REG_SDI_FIFO_LEVEL)) {
			fpga_spi_unpack_word(fpga_spi_rd(regmap,
							 FPGA_SPI_ENGINE_REG_SDI_DATA_FIFO),
					     &rxtmp[rx_w * word_len], word_len,
					     priv->lsb_first);
			rx_w++;
			progress = true;
		}
		if (progress)
			spins = FPGA_SPI_ENGINE_SPIN_MAX;
		else if (--spins == 0U)
			goto timeout;
	}

	if (fpga_spi_wait_sync(regmap, sync))
		goto timeout;

	if (rx && rx_size)
		memcpy(rx, &rxtmp[rx_offset], rx_size);

	return 0;

timeout:
	/* Leave the core usable for the next transfer. */
	fpga_spi_core_reset(regmap);
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
static int fpga_capi_spi_init(struct capi_spi_controller_handle **handle,
			      const struct capi_spi_config *config)
{
	struct capi_spi_controller_handle *h;
	struct fpga_spi_priv_handle *priv;
	const struct fpga_capi_spi_config *extra;
	uint32_t reg, aw;
	bool alloc;

	if (handle == NULL || config == NULL)
		return -EINVAL;

	extra = config->extra;
	if (extra == NULL)
		return -EINVAL;

	/* Re-init of a live handle is a caller bug, not a reset. */
	if (*handle != NULL &&
	    ((*handle)->ops != NULL ||
	     ((*handle)->priv != NULL &&
	      ((struct fpga_spi_priv_handle *)(*handle)->priv)->regmap_base != 0U)))
		return -EBUSY;

	alloc = (*handle == NULL);
	if (alloc) {
		h = capi_calloc(1, sizeof(*h));
		if (!h)
			return -ENOMEM;
		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(h);
			return -ENOMEM;
		}
		h->priv = priv;
	} else {
		h = *handle;
		if (!h->priv)
			return -EINVAL;
		priv = h->priv;
	}

	h->init_allocated = alloc;

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
	priv->callback = NULL;
	priv->callback_arg = NULL;

	/*
	 * Bring up the SCLK clock generator. The MMCM powers up held in reset, so
	 * with no bring-up the SCLK domain is dead and every TRANSFER stalls (SYNC
	 * never retires -> sync xfers time out). capi_clk_enable() releases the
	 * reset and waits for MMCM lock; it is idempotent with an app-level
	 * bring-up. A NULL clkgen means the SCLK is already running.
	 */
	if (priv->clkgen) {
		int ret = capi_clk_enable(priv->clkgen);

		if (ret) {
			if (alloc) {
				capi_free(priv);
				capi_free(h);
			}
			return ret;
		}
	}

	/* Reset, read the core's data width, clamp to it. */
	fpga_spi_wr(priv->regmap_base, FPGA_SPI_ENGINE_REG_RESET, 0x01);
	capi_wait_us(1000);
	fpga_spi_wr(priv->regmap_base, FPGA_SPI_ENGINE_REG_RESET, 0x00);

	/* Only the lower 16 bits are relevant for the actual data-width */
	priv->max_data_width = (uint8_t)FPGA_SPI_ENGINE_DATA_WIDTH(
				       fpga_spi_rd(priv->regmap_base,
						       FPGA_SPI_ENGINE_REG_DATA_WIDTH));
	if (priv->max_data_width && priv->data_width > priv->max_data_width)
		priv->data_width = priv->max_data_width;
	priv->word_len = (uint8_t)((priv->data_width + 7U) / 8U);

	priv->version = fpga_spi_rd(priv->regmap_base,
				    FPGA_SPI_ENGINE_REG_VERSION);

	/* Offload memory depths, so the async path never overruns them. */
	reg = fpga_spi_rd(priv->regmap_base,
			  FPGA_SPI_ENGINE_REG_OFFLOAD_MEM_ADDR_WIDTH);
	aw = FPGA_SPI_ENGINE_OFFLOAD_CMD_MEM_AW(reg);
	priv->offload_cmd_depth = 1U << (aw ? aw :
					 FPGA_SPI_ENGINE_OFFLOAD_MEM_AW_DEFAULT);
	aw = FPGA_SPI_ENGINE_OFFLOAD_SDO_MEM_AW(reg);
	priv->offload_sdo_depth = 1U << (aw ? aw :
					 FPGA_SPI_ENGINE_OFFLOAD_MEM_AW_DEFAULT);

	h->ops = config->ops ? config->ops : &fpga_capi_spi_ops;

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
static int fpga_capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	struct fpga_spi_priv_handle *priv;

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
		priv->async_in_progress = false;
		fpga_spi_offload_stop(priv);
		if (priv->offload_dma_chan)
			(void)capi_dma_register_complete_callback(
				priv->offload_dma_chan, NULL, NULL);
		fpga_spi_wr(priv->regmap_base, FPGA_SPI_ENGINE_REG_RESET, 1U);
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
static int fpga_capi_spi_transceive(struct capi_spi_device *device,
				    struct capi_spi_transfer *transfer)
{
	struct fpga_spi_priv_handle *priv;
	uint16_t n;

	if (!device || !transfer || !device->controller ||
	    !device->controller->priv)
		return -EINVAL;
	priv = device->controller->priv;
	priv->mode = (uint8_t)device->mode;
	priv->lsb_first = device->lsb_first;
	priv->chip_select = fpga_spi_cs_index(device->native_cs);

	n = transfer->tx_size > transfer->rx_size ?
	    transfer->tx_size : transfer->rx_size;

	return fpga_spi_sync_xfer(priv, device->max_speed_hz, transfer->tx_buf,
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
static int fpga_capi_spi_read_command(struct capi_spi_device *device,
				      struct capi_spi_transfer *transfer)
{
	struct fpga_spi_priv_handle *priv;
	uint16_t n;

	if (!device || !transfer || !device->controller ||
	    !device->controller->priv)
		return -EINVAL;
	priv = device->controller->priv;
	priv->mode = (uint8_t)device->mode;
	priv->lsb_first = device->lsb_first;
	priv->chip_select = fpga_spi_cs_index(device->native_cs);

	/* Tx then Rx in one CS frame: clock tx_size + rx_size bytes, keep the
	 * bytes received during the rx phase. */
	n = (uint16_t)(transfer->tx_size + transfer->rx_size);

	return fpga_spi_sync_xfer(priv, device->max_speed_hz, transfer->tx_buf,
				  transfer->tx_size, transfer->rx_buf,
				  transfer->rx_size, n, transfer->tx_size);
}

/**
 * @brief Start an asynchronous SPI transceive operation.
 * @note SPI Engine: spi_engine_offload_transfer order -- OFFLOAD_RESET, load
 *       offload 0 CMD/SDO memory, OFFLOAD_CTRL = 1, start the sample DMA,
 *       then enable the trigger timer channel.
 *
 * Non-blocking: needs an IRQ-driven offload DMA channel. Completion is
 * reported from the DMA end-of-transfer callback (fpga_spi_dma_done), which
 * fires CAPI_SPI_EVENT_XFR_DONE or an error event. The trigger period must be
 * longer than one program run, or the generator starts further runs before
 * completion stops it (they are discarded by fpga_spi_offload_stop()).
 * @param device - Pointer to the CAPI SPI device.
 * @param transfer - Pointer to the transfer description.
 * @param timeout - Unused; completion is reported by the DMA callback.
 * @return 0 on success, or:
 *         -EINVAL   bad arguments, or more words than the offload SDO memory
 *         -ENOTSUP  no offload DMA / trigger composed
 *         -EBUSY    an async transfer is already in flight
 *         other     capi_dma / capi_timer error
 */
static int fpga_capi_spi_transceive_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer,
		int timeout)
{
	struct fpga_spi_priv_handle *priv;
	uint32_t regmap;
	uint16_t n, words, w;
	uint32_t len;
	int ret;

	if (!device || !transfer || !device->controller ||
	    !device->controller->priv)
		return -EINVAL;

	priv = device->controller->priv;

	/* The offload path needs both the sample DMA and the trigger. */
	if (priv->offload_dma_chan == NULL || priv->trigger == NULL)
		return -ENOTSUP;
	if (priv->async_in_progress)
		return -EBUSY;

	n = transfer->tx_size > transfer->rx_size ?
	    transfer->tx_size : transfer->rx_size;
	if (n == 0U)
		return -EINVAL;

	words = (uint16_t)((n + priv->word_len - 1U) / priv->word_len);
	len = (uint32_t)words * priv->word_len;
	/* One offload program: every word must fit the offload SDO memory, the
	 * program the command memory. */
	if (len > FPGA_SPI_MAX_XFER_BYTES || words > priv->offload_sdo_depth ||
	    FPGA_SPI_ENGINE_OFFLOAD_PROGRAM_LEN > priv->offload_cmd_depth)
		return -EINVAL;

	regmap = priv->regmap_base;
	priv->mode = (uint8_t)device->mode;
	priv->lsb_first = device->lsb_first;
	priv->chip_select = fpga_spi_cs_index(device->native_cs);

	/* Program the offload command memory and SDO memory. */
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 1U);
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_RESET(0), 0U);

	(void)fpga_spi_emit_program(priv, words, fpga_spi_sink_offload_cmd,
				    regmap, device->max_speed_hz);
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
		    FPGA_SPI_ENGINE_CMD_SYNC(priv->sync_id));

	for (w = 0; w < words; w++)
		fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0),
			    fpga_spi_pack_word(transfer->tx_buf,
					       transfer->tx_size,
					       priv->word_len, w,
					       priv->lsb_first));

	/* Cache framing for the completion callback. */
	priv->async_rx = transfer->rx_buf;
	priv->async_rx_size = transfer->rx_size;
	priv->async_rx_offset = 0U;
	priv->async_words = words;
	priv->async_in_progress = true;

	/*
	 * Arm the offload sample-capture DMA (DEV_TO_MEM into sample_buf) through
	 * the composed capi_dma channel. Registering a completion callback puts the
	 * DMA backend on its interrupt path, so xfer_start returns immediately with
	 * the S2MM waiting for the streamed SDI samples; the DMA backend flushes and
	 * later invalidates sample_buf for us.
	 */
	priv->dma_xfer.src = 0U;
	priv->dma_xfer.dst = (capi_dma_glbl_addr_t)(uintptr_t)priv->sample_buf;
	priv->dma_xfer.length = len;
	priv->dma_xfer.xfer_type = CAPI_DMA_DEV_TO_MEM;
	priv->dma_xfer.src_inc = CAPI_DMA_NO_INCREMENT;
	priv->dma_xfer.dst_inc = CAPI_DMA_BYTE_INCREMENT;
	priv->dma_xfer.extra = NULL;

	ret = capi_dma_register_complete_callback(priv->offload_dma_chan,
			fpga_spi_dma_done, priv);
	if (ret)
		goto err;

	ret = capi_dma_config_xfer(priv->offload_dma_chan, &priv->dma_xfer);
	if (ret)
		goto err;

	/* spi_engine_offload_transfer order: enable offload, then start the
	 * sample DMA. Nothing runs until the trigger pulses. */
	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0),
		    FPGA_SPI_ENGINE_OFFLOAD_CTRL_ENABLE);

	ret = capi_dma_xfer_start(priv->offload_dma_chan);
	if (ret) {
		fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);
		goto err;
	}

	/* Start the trigger generator: without a pulse on the engine's trigger
	 * input the offload program never executes and the DMA never completes. */
	ret = capi_timer_channel_enable(priv->trigger, priv->trigger_chan);
	if (ret) {
		(void)capi_dma_xfer_abort(priv->offload_dma_chan);
		goto err;
	}

	return 0;

err:
	priv->async_in_progress = false;
	fpga_spi_offload_stop(priv);
	return ret;
}

/**
 * @brief Abort an in-flight asynchronous transfer.
 * @note SPI Engine: aborts the sample DMA, then fpga_spi_offload_stop()
 *       (trigger off, offload off, wait idle or core reset, OFFLOAD_RESET).
 * @param device - Pointer to the CAPI SPI device.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_spi_abort_async(struct capi_spi_device *device)
{
	struct fpga_spi_priv_handle *priv;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = device->controller->priv;

	if (priv->async_in_progress) {
		/* Halt the sample DMA, then stop the trigger and offload stream. */
		if (priv->offload_dma_chan)
			(void)capi_dma_xfer_abort(priv->offload_dma_chan);
		priv->async_in_progress = false;
		fpga_spi_offload_stop(priv);

		if (priv->callback)
			priv->callback(CAPI_SPI_EVENT_ERROR,
				       priv->callback_arg, 0);
	}

	return 0;
}

/**
 * @brief Register the asynchronous completion callback.
 * @note SPI Engine: stored on the handle; no register access.
 * @param handle - Pointer to the CAPI handle.
 * @param callback - Completion callback (NULL to unregister).
 * @param callback_arg - User argument passed to the callback.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_spi_register_callback(struct capi_spi_controller_handle
		*handle,
		capi_spi_callback_t const callback, void *callback_arg)
{
	struct fpga_spi_priv_handle *priv;

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
static int fpga_capi_spi_set_cs(struct capi_spi_device *device,
				enum capi_spi_cs_control cs)
{
	struct fpga_spi_priv_handle *priv;
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
		priv->chip_select = fpga_spi_cs_index(device->native_cs);
		mask = (uint8_t)(0xFFU ^ (1U << priv->chip_select));
		break;
	default:
		return -EINVAL;
	}

	regmap = priv->regmap_base;
	sync = (priv->sync_id = (priv->sync_id + 1U) & 0xFFU);

	fpga_spi_wr(regmap, FPGA_SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0U);
	ret = fpga_spi_push_cmd(regmap,
				FPGA_SPI_ENGINE_CMD_ASSERT(priv->cs_delay, mask));
	if (!ret)
		ret = fpga_spi_push_cmd(regmap, FPGA_SPI_ENGINE_CMD_SYNC(sync));
	if (!ret)
		ret = fpga_spi_wait_sync(regmap, sync);
	if (ret) {
		fpga_spi_core_reset(regmap);
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
 * the offload stream (fpga_spi_offload_stop), unpack the captured SDI samples (already made coherent by
 * the DMA backend's cache invalidate) into the caller's rx buffer, and fire one
 * CAPI_SPI_EVENT_XFR_DONE. A non-zero event is reported as an error.
 * @param event - DMA completion event (0 = success).
 * @param ctx - Callback context (the channel private data).
 */
static void fpga_spi_dma_done(uint32_t event, void *ctx)
{
	struct fpga_spi_priv_handle *priv = ctx;
	uint8_t word_len;
	uint8_t rxtmp[FPGA_SPI_MAX_XFER_BYTES];
	uint16_t w;

	if (!priv || !priv->async_in_progress)
		return;

	/* The DMA captured one run: stop the trigger and the offload stream. */
	priv->async_in_progress = false;
	fpga_spi_offload_stop(priv);

	if (event != 0U) {
		if (priv->callback)
			priv->callback(CAPI_SPI_EVENT_ERROR,
				       priv->callback_arg, 0);
		return;
	}

	word_len = priv->word_len;
	for (w = 0; w < priv->async_words; w++) {
		uint32_t word = fpga_spi_sample_word(
					&priv->sample_buf[w * word_len],
					word_len);
		fpga_spi_unpack_word(word, &rxtmp[w * word_len], word_len,
				     priv->lsb_first);
	}

	if (priv->async_rx && priv->async_rx_size)
		memcpy(priv->async_rx, &rxtmp[priv->async_rx_offset],
		       priv->async_rx_size);

	if (priv->callback)
		priv->callback(CAPI_SPI_EVENT_XFR_DONE, priv->callback_arg, 0);
}

#ifdef __cplusplus
}
#endif
