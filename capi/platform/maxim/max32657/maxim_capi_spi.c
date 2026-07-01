/*******************************************************************************
 *   @file   maxim_capi_spi.c
 *   @brief  Implementation of SPI functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include <stdio.h>
#include <errno.h>
#include "capi_spi.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_spi.h"
#include "maxim_capi_gpio.h"
#include "spi.h"

/** Forward declarations ******************************************************/

void max_capi_spi_isr(void *handle);

/** Static variables **********************************************************/

static struct capi_spi_controller_handle *spi[MXC_SPI_INSTANCES] = {NULL};
static volatile uint8_t dma_completed_count[MXC_SPI_INSTANCES] = {0};
static struct capi_dma_chan *dma_channel_rx[MXC_SPI_INSTANCES] = {NULL};
static struct capi_dma_chan *dma_channel_tx[MXC_SPI_INSTANCES] = {NULL};
/* In capi_spi_transfer, if the transmit buffer is NULL, zeroes will be
   transmitted and if the receive buffer is NULL, the data is discarded */
static uint8_t zero_tx[1] = {0};
static uint8_t zero_rx[1];
static bool transfer_in_progress[MXC_SPI_INSTANCES] = {false};
static bool async_transfer_in_progress[MXC_SPI_INSTANCES] = {false};

/** Helper functions **********************************************************/

/**
 * @brief Configure a SPI peripheral
 * @param handle The SPI handle
 * @return 0 in case of success, negative error code otherwise
 */
int _max_capi_spi_config(struct capi_spi_controller_handle *handle)
{
	int ret;
	struct max_capi_spi_priv *spi_priv;

	spi_priv = handle->priv;
	uint8_t spi_id = spi_priv->identifier;

	mxc_spi_pins_t spi_pins_config = {
		.clock = true,
		.ss0 = (spi_priv->extra.chip_select & (1 << 0)) ? true : false,
		.ss1 = (spi_priv->extra.chip_select & (1 << 1)) ? true : false,
		.ss2 = (spi_priv->extra.chip_select & (1 << 2)) ? true : false,
		.miso = true,
		.mosi = true,
		.sdio2 = false, /* sdio2 and sdio3 are ignored in spi_me30.c */
		.sdio3 = false,
		.vddioh = (spi_priv->extra.vssel == MAX_CAPI_GPIO_VSSEL_VDDIOH),
	};

	ret = MXC_SPI_Init(MXC_SPI_GET_SPI(spi_id),
			   spi_priv->extra.device_role,
			   (spi_priv->extra.bus_width == MAX_CAPI_SPI_BUS_WIDTH_QUAD) ? 1 : 0,
			   spi_priv->extra.num_targets,
			   spi_priv->extra.polarity_mask,
			   spi_priv->clock_freq,
			   spi_pins_config);
	if (ret < 0) {
		ret = -EINVAL;
		goto error;
	}

	mxc_spi_mode_t spi_mode;
	if (spi_priv->extra.clock_phase == MAX_CAPI_SPI_CLOCK_PHASE_0) {
		if (spi_priv->extra.clock_polarity == MAX_CAPI_SPI_CLOCK_POLARITY_0) {
			spi_mode = SPI_MODE_0;
		} else if (spi_priv->extra.clock_polarity == MAX_CAPI_SPI_CLOCK_POLARITY_1) {
			spi_mode = SPI_MODE_1;
		} else {
			ret = -EINVAL;
			goto error;
		}
	} else if (spi_priv->extra.clock_phase == MAX_CAPI_SPI_CLOCK_PHASE_1) {
		if (spi_priv->extra.clock_polarity == MAX_CAPI_SPI_CLOCK_POLARITY_0) {
			spi_mode = SPI_MODE_2;
		} else if (spi_priv->extra.clock_polarity == MAX_CAPI_SPI_CLOCK_POLARITY_1) {
			spi_mode = SPI_MODE_3;
		} else {
			ret = -EINVAL;
			goto error;
		}
	} else {
		ret = -EINVAL;
		goto error;
	}
	ret = MXC_SPI_SetMode(MXC_SPI_GET_SPI(spi_id), spi_mode);
	if (ret) {
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_SPI_SetWidth(MXC_SPI_GET_SPI(spi_id),
			       (mxc_spi_width_t)spi_priv->extra.bus_width);
	if (ret) {
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_SPI_SetDataSize(MXC_SPI_GET_SPI(spi_id), 8);
	if (ret) {
		ret = -EINVAL;
		goto error;
	}

	return 0;

error:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(spi_id));

	return ret;
}

/**
 * @brief Set the closest first and last SCLK delays to what was requested
 * @param device The SPI device
 * @param transfer The SPI transfer
 */
void _max_capi_delay_config(struct capi_spi_device *device,
			    struct capi_spi_transfer *transfer)
{
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *spi_priv;
	mxc_spi_regs_t *spi_reg;
	uint32_t spi_freq, ns_per_tick, tstime_cache;
	uint32_t delay_first_ns, delay_last_ns;
	uint32_t delay_first_ticks, delay_last_ticks;

	spi_handle = device->controller;
	spi_priv = spi_handle->priv;

	spi_reg = MXC_SPI_GET_SPI(spi_priv->identifier);
	tstime_cache = spi_reg->tstime;

	spi_freq = MXC_SPI_GetPeripheralClock(spi_reg);
	ns_per_tick = (1000000000UL / spi_freq);

	delay_first_ns = spi_priv->extra.platform_delays.cs_delay_first;
	delay_last_ns = spi_priv->extra.platform_delays.cs_delay_last;

	// TODO: Add transfer->xfer_delay_clk_cycles to these?
	delay_first_ticks = (delay_first_ns / ns_per_tick);
	delay_last_ticks = (delay_last_ns / ns_per_tick);

	/** The minimum number of delay ticks is 1. If 0 is written to the
	 *  tstime register, there would be a delay of 256 ticks.
	 */
	if (delay_first_ticks == 0)
		delay_first_ticks = 1;
	if (delay_first_ticks > MAX_DELAY_SCLK)
		goto error;
	spi_reg->tstime &= ~MXC_F_SPI_TSTIME_PRE;
	spi_reg->tstime |= (delay_first_ticks << MXC_F_SPI_TSTIME_PRE);

	if (delay_last_ticks == 0)
		delay_last_ticks = 1;
	if (delay_last_ticks > MAX_DELAY_SCLK)
		goto error;
	spi_reg->tstime &= ~MXC_F_SPI_TSTIME_POST;
	spi_reg->tstime |= (delay_last_ticks << MXC_F_SPI_TSTIME_POST);

	return;
error:
	spi_reg->tstime = tstime_cache;
}

/**
 * @brief Get chip select index based on bitmask
 * @param chip_select Bitmask for chip select from capi_spi_device
 * @return 0, 1, or 2 on success, negative error code otherwise
 */
int _max_capi_spi_get_cs_index(uint16_t chip_select)
{
	int cs_index;

	if (chip_select == 0 ||
	    (chip_select & (chip_select - 1)) != 0) {
		return -EINVAL;
	}

	cs_index = __builtin_ctz(chip_select);
	if (cs_index > 2) {
		return -EINVAL;
	}

	return cs_index;
}

/**
 * @brief Queues as many bytes as the TX FIFO allows, up to clk_len
 * @param spi_priv The private structure
 * @return
 */
static bool _max_capi_spi_fifo_fill_tx(struct max_capi_spi_priv *spi_priv)
{
	struct max_capi_spi_fifo_async *fifo_async = &spi_priv->fifo_async;
	mxc_spi_regs_t *spi_reg = MXC_SPI_GET_SPI(spi_priv->identifier);
	uint32_t written;

	while (fifo_async->tx_cnt < fifo_async->clk_len) {
		const uint8_t *src;
		size_t len;

		if (fifo_async->tx_buf && (fifo_async->tx_cnt < fifo_async->tx_size)) {
			src = &fifo_async->tx_buf[fifo_async->tx_cnt];
			len = fifo_async->tx_size - fifo_async->tx_cnt;
		} else {
			src = zero_tx;
			len = 1;
		}

		written = MXC_SPI_WriteTXFIFO(spi_reg, (uint8_t *)src, len);
		if (written == 0)
			break;

		fifo_async->tx_cnt += written;
	}

	return (fifo_async->tx_cnt >= fifo_async->clk_len);
}

/**
 * @brief Reads whatever is available in the RX FIFO into rx_buf
 * @param spi_priv The private structure
 * @return True when all expected RX bytes have been received (or when there is
 * 	   no RX buffer), False otherwise
 */
static bool _max_capi_spi_fifo_drain_rx(struct max_capi_spi_priv *spi_priv)
{
	struct max_capi_spi_fifo_async *fifo_async = &spi_priv->fifo_async;
	mxc_spi_regs_t *spi_reg = MXC_SPI_GET_SPI(spi_priv->identifier);
	uint32_t total = fifo_async->rx_skip + fifo_async->rx_size;

	if (!fifo_async->rx_buf)
		return true;

	while (fifo_async->rx_cnt < total) {
		uint32_t n;

		if (fifo_async->rx_cnt < fifo_async->rx_skip) {
			/* Discard RX */
			n = MXC_SPI_ReadRXFIFO(spi_reg, zero_rx, 1);
		} else  {
			uint32_t off = fifo_async->rx_cnt - fifo_async->rx_skip;

			n = MXC_SPI_ReadRXFIFO(spi_reg,
					       &fifo_async->rx_buf[off],
					       (total - fifo_async->rx_cnt));
		}

		if (n == 0)
			break; /* RX FIFO empty for now */

		fifo_async->rx_cnt += n;
	}

	return (fifo_async->rx_cnt >= total);
}

/**
 * @brief Do a SPI transfer using FIFO
 * @param device The SPI device
 * @param transfer The transfer
 * @param is_async Whether it's async or not
 * @param read_cmd Whether it's a read command or not
 * @return 0 on success, negative error code otherwise
 */
int _max_capi_spi_transceive_fifo(struct capi_spi_device *device,
				  struct capi_spi_transfer *transfer,
				  bool is_async, bool read_cmd)
{
	int32_t target_id;
	int ret;
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *spi_priv;
	struct max_capi_spi_fifo_async *fifo_async;
	mxc_spi_regs_t *spi_reg;
	uint8_t spi_id;
	uint32_t tx_effective, rx_effective, clk_len;
	uint32_t target_speed;
	uint32_t inten;
	int timeout;
	bool rx_done, tx_done;

	spi_handle = device->controller;
	spi_priv = spi_handle->priv;
	spi_reg = MXC_SPI_GET_SPI(spi_priv->identifier);
	spi_id = spi_priv->identifier;
	fifo_async = &spi_priv->fifo_async;
	timeout = transfer->timeout;

	if (is_async) {
		if (transfer_in_progress[spi_id])
			return -EBUSY;
	} else if (timeout == 0 || timeout == -1) {
		while (transfer_in_progress[spi_id]);
	} else {
		while (transfer_in_progress[spi_id]) {
			timeout--;
			if (timeout == 0) {
				return -ETIMEDOUT;
			}
			capi_wait_ms(1);
		}
	}
	transfer_in_progress[spi_id] = true;

	/* Validate only one CS is chosen */
	target_id = _max_capi_spi_get_cs_index(device->native_cs);
	if (target_id < 0) {
		transfer_in_progress[spi_id] = false;
		return target_id;
	}

	if (device->max_speed_hz > spi_priv->clock_freq) {
		transfer_in_progress[spi_id] = false;
		return -EINVAL;
	}

	target_speed = device->max_speed_hz
		       ? device->max_speed_hz
		       : spi_priv->clock_freq;

	if (target_speed != spi_priv->clock_freq) {
		ret = MXC_SPI_SetFrequency(spi_reg, target_speed);
		if (ret < 0) {
			transfer_in_progress[spi_id] = false;
			return -EIO;
		}
	} else {
		ret = MXC_SPI_SetFrequency(spi_reg, spi_priv->clock_freq);
		if (ret < 0) {
			transfer_in_progress[spi_id] = false;
			return -EIO;
		}
	}

	/* Assert CS when the SPI transaction is started */
	MXC_SPI_SetSlave(spi_reg, target_id);

	/* Flush the RX and TX FIFOs */
	spi_reg->dma |= MXC_F_SPI_DMA_RX_FLUSH | MXC_F_SPI_DMA_TX_FLUSH;
	/* Enable SPI */
	spi_reg->intfl |= MXC_F_SPI_INTFL_CONT_DONE;
	spi_reg->ctrl1 = 0;

	if (device->non_continuous_mode)
		spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_TS_CTRL;
	else
		spi_reg->ctrl0 |= MXC_F_SPI_CTRL0_TS_CTRL;

	_max_capi_delay_config(device, transfer);

	/*
	 * In standard full-duplex mode, the number of SCLK characters that are
	 * clocked out is governed by TX_NUM_CHAR. To receive more bytes than
	 * are transmitted (rx_size > tx_size), TX_NUM_CHAR must be extended to
	 * the larger of the two lengths and dummy bytes shifted out to generate
	 * the additional clocks.
	 */
	tx_effective = transfer->tx_buf ? transfer->tx_size : 0;
	rx_effective = transfer->rx_buf ? transfer->rx_size : 0;

	if (read_cmd) {
		/* Half duplex */
		clk_len = tx_effective + rx_effective;
		fifo_async->rx_skip = transfer->tx_size;
	} else {
		clk_len = (rx_effective > tx_effective) ? rx_effective : tx_effective;
		fifo_async->rx_skip = 0;
	}

	/* Populate shared transfer state */
	fifo_async->tx_buf = transfer->tx_buf;
	fifo_async->rx_buf = transfer->rx_buf;
	fifo_async->tx_size = transfer->tx_size;
	fifo_async->rx_size = transfer->rx_size;
	fifo_async->clk_len = clk_len;
	fifo_async->tx_cnt = 0;
	fifo_async->rx_cnt = 0;

	if (clk_len == 0) {
		if (is_async && spi_priv->callback)
			spi_priv->callback(CAPI_SPI_EVENT_XFR_DONE,
					   spi_priv->callback_arg, 0);
		transfer_in_progress[spi_id] = false;
		return 0;
	}

	if (is_async) {
		async_transfer_in_progress[spi_id] = true;
		fifo_async->active = true;
	}

	/* Set the number of characters to clock out (TX direction) */
	spi_reg->ctrl1 = clk_len;
	/* Enable the TX FIFO */
	spi_reg->dma |= MXC_F_SPI_DMA_TX_FIFO_EN;

	if (transfer->rx_buf) {
		uint32_t rx_num_char = read_cmd ? clk_len : transfer->rx_size;

		/* Set the transfer size in the RX direction */
		spi_reg->ctrl1 |= (rx_num_char << MXC_F_SPI_CTRL1_RX_NUM_CHAR_POS);
		/* Enable the RX FIFO */
		spi_reg->dma |= MXC_F_SPI_DMA_RX_FIFO_EN;
	}

	if (is_async) {
		MXC_SETFIELD(spi_reg->dma, MXC_F_SPI_DMA_TX_THD_VAL,
			     (1U << MXC_F_SPI_DMA_TX_THD_VAL_POS));
		MXC_SETFIELD(spi_reg->dma, MXC_F_SPI_DMA_RX_THD_VAL,
			     (0U << MXC_F_SPI_DMA_RX_THD_VAL_POS));
	}

	/* Prime the TX FIFO with the first bytes before starting */
	_max_capi_spi_fifo_fill_tx(spi_priv);

	if (is_async) {
		inten = MXC_F_SPI_INTEN_CONT_DONE |
			MXC_F_SPI_INTEN_TX_OV |
			MXC_F_SPI_INTEN_RX_OV |
			MXC_F_SPI_INTEN_ABORT;
		if (transfer->rx_buf)
			inten |= MXC_F_SPI_INTEN_RX_THD;
		if (fifo_async->tx_cnt < clk_len)
			inten |= MXC_F_SPI_INTEN_TX_THD;

		spi_reg->inten = inten;

		MXC_SPI_StartTransmission(spi_reg);
		return 0;
	}

	/* Start the transaction */
	MXC_SPI_StartTransmission(spi_reg);

	tx_done = false;
	rx_done = false;
	while (!(rx_done && tx_done)) {
		tx_done = _max_capi_spi_fifo_fill_tx(spi_priv);
		rx_done = _max_capi_spi_fifo_drain_rx(spi_priv);
	}

	/* Wait for the RX and TX FIFOs to be empty */
	while (!(spi_reg->intfl & MXC_F_SPI_INTFL_CONT_DONE));

	/* End the transaction */
	spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_START;

	/* Disable the RX and TX FIFOs */
	spi_reg->dma &= ~(MXC_F_SPI_DMA_RX_FIFO_EN | MXC_F_SPI_DMA_TX_FIFO_EN);

	transfer_in_progress[spi_id] = false;

	return 0;
}

/**
 * @brief Cleanup a DMA channel
 * @param channel The DMA channel
 */
void _max_capi_spi_dma_cleanup_channel(struct capi_dma_chan **channel)
{
	if (channel && *channel) {
		capi_dma_xfer_abort(*channel);
		capi_dma_deinit_chan(*channel);
		*channel = NULL;
	}
}

/**
 * @brief The callback called when a transfer is completed
 * @param transfer The DMA transfer
 * @param ctx The user data passed (struct max_capi_spi_priv)
 */
void _max_capi_spi_dma_complete_callback(uint32_t event, void *ctx)
{
	struct max_capi_spi_priv *priv = ctx;
	uint8_t spi_id = priv->identifier;

	dma_completed_count[spi_id]++;
	if (dma_completed_count[spi_id] == 2) {
		if (priv->callback) {
			priv->callback(CAPI_SPI_EVENT_XFR_DONE,
				       priv->callback_arg, 0);
		}

		dma_completed_count[spi_id] = 0;
		async_transfer_in_progress[spi_id] = false;
		transfer_in_progress[spi_id] = false;

		// _max_capi_spi_dma_cleanup_channel(&dma_channel_rx[spi_id]);
		// _max_capi_spi_dma_cleanup_channel(&dma_channel_tx[spi_id]);
	}
}

/**
 * @brief Do a SPI transfer using DMA
 * @param device The SPI device
 * @param transfer The SPI transfer
 * @param is_async Whether it's async or not
 * @return 0 on success, negative error code otherwise
 */
int _max_capi_spi_transceive_dma(struct capi_spi_device *device,
				 struct capi_spi_transfer *transfer,
				 bool is_async)
{
	int32_t target_id;
	int ret;
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *spi_priv;
	struct capi_dma_handle *dma_handle;
	struct max_capi_dma_xfer_extra dma_transfer_tx_extra, dma_transfer_rx_extra;
	struct capi_dma_transfer dma_transfer_tx, dma_transfer_rx;
	mxc_spi_regs_t *spi_reg;
	uint8_t spi_id;
	uint32_t target_speed;
	uint32_t rx_id, tx_id;
	int timeout;

	spi_handle = device->controller;
	spi_priv = spi_handle->priv;
	spi_reg = MXC_SPI_GET_SPI(spi_priv->identifier);
	dma_handle = spi_priv->dma_handle;
	spi_id = spi_priv->identifier;
	timeout = transfer->timeout;

	if (timeout == 0 || timeout == -1) {
		while (transfer_in_progress[spi_id]);
	} else {
		while (transfer_in_progress[spi_id]) {
			timeout--;
			if (timeout == 0) {
				return -ETIMEDOUT;
			}
			capi_wait_ms(1);
		}
	}
	transfer_in_progress[spi_id] = true;

	/* Validate only one CS is chosen */
	target_id = _max_capi_spi_get_cs_index(device->native_cs);
	if (target_id < 0) {
		transfer_in_progress[spi_id] = false;
		return target_id;
	}

	if (device->max_speed_hz > spi_priv->clock_freq)
		return -EINVAL;
	target_speed = device->max_speed_hz ? device->max_speed_hz :
		       spi_priv->clock_freq;
	if (target_speed != spi_priv->clock_freq) {
		ret = MXC_SPI_SetFrequency(spi_reg, target_speed);
		if (ret < 0) {
			transfer_in_progress[spi_id] = false;
			return -EIO;
		}
	} else {
		ret = MXC_SPI_SetFrequency(spi_reg, spi_priv->clock_freq);
		if (ret < 0) {
			transfer_in_progress[spi_id] = false;
			return -EIO;
		}
	}

	/* Assert CS spi_extra->chip_select when the SPI transaction is started */
	MXC_SPI_SetSlave(spi_reg, (int)target_id);

	/* Flush the RX and TX FIFOs */
	MXC_SPI_ClearRXFIFO(spi_reg);
	MXC_SPI_ClearTXFIFO(spi_reg);
	/* Enable SPI */
	spi_reg->intfl |= MXC_F_SPI_INTFL_CONT_DONE;

	if (device->non_continuous_mode)
		spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_TS_CTRL;
	else
		spi_reg->ctrl0 |= MXC_F_SPI_CTRL0_TS_CTRL;

	/* Enable the TX and RX FIFO */
	spi_reg->ctrl1 = transfer->tx_size;
	spi_reg->ctrl1 |= (transfer->rx_size << MXC_F_SPI_CTRL1_RX_NUM_CHAR_POS);
	spi_reg->dma |= MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN;

	_max_capi_delay_config(device, transfer);

	/* Allow DMA transfer for RX and TX SPI FIFOs */
	spi_reg->dma |= MXC_F_SPI_DMA_RX_EN | MXC_F_SPI_DMA_TX_EN;

	if (!dma_channel_rx[spi_id]) {
		ret = _max_capi_spi_get_cs_index(device->native_cs);
		if (ret < 0) {
			transfer_in_progress[spi_id] = false;
			return ret;
		}
		rx_id = ret;
		ret = capi_dma_init_chan(dma_handle, &dma_channel_rx[spi_id], rx_id * 2);
		if (ret) {
			transfer_in_progress[spi_id] = false;
			return ret;
		}
	}
	if (!dma_channel_tx[spi_id]) {
		ret = _max_capi_spi_get_cs_index(device->native_cs);
		if (ret < 0) {
			transfer_in_progress[spi_id] = false;
			return ret;
		}
		tx_id = ret;
		ret = capi_dma_init_chan(dma_handle, &dma_channel_tx[spi_id], tx_id * 2 + 1);
		if (ret)
			goto deinit_rx_chan;
	}

	dma_transfer_rx_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_SPI_RX,
	};
	dma_transfer_rx = (struct capi_dma_transfer) {
		.src = &spi_reg->fifo8,
		.dst = transfer->rx_buf ? transfer->rx_buf : zero_rx,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.src_inc = CAPI_DMA_NO_INCREMENT,
		.dst_inc = transfer->rx_buf ?
			   CAPI_DMA_BYTE_INCREMENT :
			   CAPI_DMA_NO_INCREMENT,
			   .length = transfer->rx_size,
			   .extra = &dma_transfer_rx_extra,
			   .xfer_type = CAPI_DMA_DEV_TO_MEM,
	};
	dma_transfer_tx_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_SPI_TX,
	};
	dma_transfer_tx = (struct capi_dma_transfer) {
		.src = transfer->tx_buf ? transfer->tx_buf : zero_tx,
		.dst = &spi_reg->fifo8,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.src_inc = transfer->tx_buf ?
			   CAPI_DMA_BYTE_INCREMENT :
			   CAPI_DMA_NO_INCREMENT,
			   .dst_inc = CAPI_DMA_NO_INCREMENT,
			   .length = transfer->tx_size,
			   .extra = &dma_transfer_tx_extra,
			   .xfer_type = CAPI_DMA_MEM_TO_DEV,
	};

	dma_completed_count[spi_id] = 0;

	ret = capi_dma_config_xfer(dma_channel_rx[spi_id], &dma_transfer_rx);
	if (ret)
		goto deinit_tx_chan;
	ret = capi_dma_config_xfer(dma_channel_tx[spi_id], &dma_transfer_tx);
	if (ret)
		goto abort_transfer;

	if (is_async) {
		async_transfer_in_progress[spi_id] = true;
		ret = capi_dma_register_complete_callback(dma_channel_rx[spi_id],
				_max_capi_spi_dma_complete_callback,
				spi_priv);
		if (ret)
			goto abort_transfer;
		ret = capi_dma_register_complete_callback(dma_channel_tx[spi_id],
				_max_capi_spi_dma_complete_callback,
				spi_priv);
		if (ret)
			goto abort_transfer;
	}

	ret = capi_dma_xfer_start(dma_channel_rx[spi_id]);
	if (ret)
		goto abort_transfer;
	ret = capi_dma_xfer_start(dma_channel_tx[spi_id]);
	if (ret)
		goto abort_transfer;

	/* Start the transaction */
	MXC_SPI_StartTransmission(spi_reg);

	if (!is_async) {
		while (!capi_dma_chan_is_completed(dma_channel_rx[spi_id]) ||
		       !capi_dma_chan_is_completed(dma_channel_tx[spi_id]));

		while (spi_reg->status & MXC_F_SPI_STATUS_BUSY);
		/* End the transaction */
		spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
		/* Disable the RX and TX FIFOs */
		spi_reg->dma &= ~(MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN);

		transfer_in_progress[spi_id] = false;
	}

	return 0;

abort_transfer:
	if (is_async)
		async_transfer_in_progress[spi_id] = false;
	capi_dma_xfer_abort(dma_channel_tx[spi_id]);
	capi_dma_xfer_abort(dma_channel_rx[spi_id]);
deinit_tx_chan:
	capi_dma_deinit_chan(dma_channel_tx[spi_id]);
	dma_channel_tx[spi_id] = NULL;
deinit_rx_chan:
	capi_dma_deinit_chan(dma_channel_rx[spi_id]);
	dma_channel_rx[spi_id] = NULL;

	transfer_in_progress[spi_id] = false;

	return ret;
}

/** SPI functions implementation **********************************************/
/**
 * @brief Initialize the SPI peripheral
 * @param handle The SPI controller handle
 * @param config The SPI initialization config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_init(struct capi_spi_controller_handle **handle,
		      const struct capi_spi_config *config)
{
	int ret;
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *spi_priv;
	uint8_t spi_id;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MXC_SPI_INSTANCES)
		return -EINVAL;

	if (spi[config->identifier] != NULL) {
		*handle = spi[config->identifier];
		return 0;
	}

	if (*handle == NULL) {
		spi_handle = capi_calloc(1, sizeof(*spi_handle));
		if (!spi_handle)
			return -ENOMEM;
		spi_handle->init_allocated = true;
	} else {
		spi_handle = *handle;
		spi_handle->init_allocated = false;
	}

	spi_priv = capi_calloc(1, sizeof(*spi_priv));
	if (!spi_priv) {
		ret = -ENOMEM;
		goto free_handle;
	}

	spi_handle->ops = config->ops;
	spi_priv->identifier = config->identifier;
	spi_priv->clock_freq = config->clk_freq_hz;
	spi_handle->priv = spi_priv;
	spi_id = spi_priv->identifier;

	/* Copy user config or set defaults */
	if (config->extra) {
		spi_priv->extra = *(struct max_capi_spi_extra *)config->extra;
	} else {
		spi_priv->extra.device_role = MAX_CAPI_SPI_DEVICE_ROLE_CONTROLLER;
		spi_priv->extra.bus_width = MAX_CAPI_SPI_BUS_WIDTH_STANDARD;
		spi_priv->extra.num_targets = 1;
		spi_priv->extra.polarity_mask = 0;
		spi_priv->extra.chip_select = MAX_CAPI_SPI_CS0;
		spi_priv->extra.vssel = MAX_CAPI_GPIO_VSSEL_VDDIO;
		spi_priv->extra.clock_phase = MAX_CAPI_SPI_CLOCK_PHASE_0;
		spi_priv->extra.clock_polarity = MAX_CAPI_SPI_CLOCK_POLARITY_0;
		spi_priv->extra.platform_delays.cs_delay_first = 0;
		spi_priv->extra.platform_delays.cs_delay_last = 0;
		spi_priv->extra.dma_config = NULL;
	}

	ret = _max_capi_spi_config(spi_handle);
	if (ret)
		goto shutdown_spi;

	if (config->dma_handle && spi_priv->extra.dma_config) {
		spi_priv->dma_handle = config->dma_handle;

		ret = capi_dma_init(&spi_priv->dma_handle,
				    spi_priv->extra.dma_config);
		if (ret)
			goto shutdown_spi;
	}

	struct capi_irq_config irq_config = {
		.irq_ctrl_id = 0,
	};
	ret = capi_irq_init(&irq_config);
	if (ret)
		goto deinit_dma;

	IRQn_Type spi_irq = MXC_SPI_GET_IRQ(spi_id);
	ret = capi_irq_connect(spi_irq, max_capi_spi_isr, spi_handle);
	if (ret)
		goto deinit_dma;

	ret = capi_irq_enable(spi_irq);
	if (ret)
		goto disconnect_irq;

	spi[config->identifier] = spi_handle;
	*handle = spi_handle;
	spi_priv->spi_handle = spi_handle;

	return 0;

disconnect_irq:
	capi_irq_connect(MXC_SPI_GET_IRQ(spi_id), NULL, NULL);
deinit_dma:
	_max_capi_spi_dma_cleanup_channel(&dma_channel_rx[spi_id]);
	_max_capi_spi_dma_cleanup_channel(&dma_channel_tx[spi_id]);
shutdown_spi:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(spi_id));
	capi_free(spi_priv);
free_handle:
	if (spi_handle->init_allocated)
		capi_free(spi_handle);

	spi[config->identifier] = NULL;
	dma_completed_count[spi_id] = 0;
	async_transfer_in_progress[spi_id] = false;
	transfer_in_progress[spi_id] = false;

	return ret;
}

/**
 * @brief Deinitialize the SPI peripheral
 * @param handle The SPI controller handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	int ret;
	struct max_capi_spi_priv *spi_priv;
	uint8_t spi_id;

	if (!handle)
		return -EINVAL;

	spi_priv = handle->priv;
	spi_id = spi_priv->identifier;

	ret = MXC_SPI_Shutdown(MXC_SPI_GET_SPI(spi_priv->identifier));
	if (ret != E_NO_ERROR)
		return -EIO;

	dma_completed_count[spi_id] = 0;
	async_transfer_in_progress[spi_id] = false;
	transfer_in_progress[spi_id] = false;

	_max_capi_spi_dma_cleanup_channel(&dma_channel_rx[spi_id]);
	_max_capi_spi_dma_cleanup_channel(&dma_channel_tx[spi_id]);

	capi_irq_disable(MXC_SPI_GET_IRQ(spi_id));
	capi_irq_connect(MXC_SPI_GET_IRQ(spi_id), NULL, NULL);

	capi_free(spi_priv);
	if (handle->init_allocated) {
		capi_free(handle);
	}

	spi[spi_id] = NULL;

	return 0;
}

/**
 * @brief Perform blocking SPI transfer
 * @param device The SPI device
 * @param transfer The SPI transfer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_transceive(struct capi_spi_device *device,
			    struct capi_spi_transfer *transfer)
{
	const struct max_capi_spi_priv *spi_priv;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	spi_priv = device->controller->priv;
	if (spi_priv->dma_handle)
		return _max_capi_spi_transceive_dma(device, transfer, false);
	else
		return _max_capi_spi_transceive_fifo(device, transfer, false, false);
}

/**
 * @brief Perform non-blocking SPI transfer
 * @param device The SPI device
 * @param transfer The SPI transfer
 * @param timeout timeout - not used
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_transceive_async(struct capi_spi_device *device,
				  struct capi_spi_transfer *transfer,
				  int timeout)
{
	const struct max_capi_spi_priv *spi_priv;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	spi_priv = device->controller->priv;
	if (spi_priv->dma_handle)
		return _max_capi_spi_transceive_dma(device, transfer, true);
	else
		return _max_capi_spi_transceive_fifo(device, transfer, true, false);
}

/**
 * @brief Register a callback for async transactions
 * @param handle The SPI controller handle
 * @param callback The callback function to register
 * @param callback_arg The argument for the callback function
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_register_callback(struct capi_spi_controller_handle *handle,
				   capi_spi_callback_t const callback,
				   void *callback_arg)
{
	struct max_capi_spi_priv *priv;

	if (!handle)
		return -EINVAL;

	priv = handle->priv;
	priv->callback = callback;
	priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Send a command/address, then reads data - all in one CS assertion,
 * 	  blocking
 * @param device The SPI device
 * @param transfer The SPI transfer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_read_command(struct capi_spi_device *device,
			      struct capi_spi_transfer *transfer)
{
	const struct max_capi_spi_priv *spi_priv;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	spi_priv = device->controller->priv;
	if (spi_priv->dma_handle)
		return -ENOTSUP;
	else
		return _max_capi_spi_transceive_fifo(device, transfer, false, true);
}

/**
 * @brief Send a command/address, then reads data - all in one CS assertion,
 * 	  non-blocking
 * @param device The SPI device
 * @param transfer The SPI transfer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_read_command_async(struct capi_spi_device *device,
				    struct capi_spi_transfer *transfer)
{
	const struct max_capi_spi_priv *spi_priv;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	spi_priv = device->controller->priv;
	if (spi_priv->dma_handle)
		return -ENOTSUP;
	else
		return _max_capi_spi_transceive_fifo(device, transfer, true, true);
}

/**
 * @brief Abort an async transaction
 * @param device The SPI device
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_abort_async(struct capi_spi_device *device)
{
	const struct max_capi_spi_priv *spi_priv;
	struct max_capi_spi_fifo_async *fifo_async;
	uint8_t spi_id;
	mxc_spi_regs_t *spi_reg;

	if (!device || !device->controller)
		return -EINVAL;

	spi_priv = device->controller->priv;
	spi_id = spi_priv->identifier;
	spi_reg = MXC_SPI_GET_SPI(spi_id);
	fifo_async = &spi_priv->fifo_async;

	_max_capi_spi_dma_cleanup_channel(&dma_channel_rx[spi_id]);
	_max_capi_spi_dma_cleanup_channel(&dma_channel_tx[spi_id]);

	spi_reg->inten &= ~(MXC_F_SPI_INTEN_TX_THD |
			    MXC_F_SPI_INTEN_RX_THD |
			    MXC_F_SPI_INTEN_CONT_DONE);
	fifo_async->active = false;

	spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
	spi_reg->dma &= ~(MXC_F_SPI_DMA_RX_FIFO_EN | MXC_F_SPI_DMA_TX_FIFO_EN);
	spi_reg->dma &= ~(MXC_F_SPI_DMA_RX_EN | MXC_F_SPI_DMA_TX_EN);

	dma_completed_count[spi_id] = 0;
	async_transfer_in_progress[spi_id] = false;
	transfer_in_progress[spi_id] = false;

	if (spi_priv->callback)
		spi_priv->callback(CAPI_SPI_EVENT_ERROR,
				   spi_priv->callback_arg, 0);

	return 0;
}

/**
 * @brief Manually assert/deassert chip select, or restore to auto mode
 * @param device The SPI device
 * @param cs_control The chip select mode
 * @return 0 on success, negative error code otherwise
 */
int max_capi_spi_set_cs(struct capi_spi_device *device,
			enum capi_spi_cs_control cs_control)
{
	struct max_capi_spi_priv *spi_priv;
	mxc_spi_regs_t *spi_reg;
	uint8_t spi_id;

	if (!device || !device->controller)
		return -EINVAL;

	spi_priv = device->controller->priv;
	spi_id = spi_priv->identifier;
	spi_reg = MXC_SPI_GET_SPI(spi_id);

	switch (cs_control) {
	case CAPI_SPI_CS_AUTO:
		/* Hardware controls CS */
		spi_reg->ctrl0 |= MXC_F_SPI_CTRL0_TS_CTRL;
		break;

	case CAPI_SPI_CS_MANUAL_ASSERT:
		spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_TS_CTRL;
		MXC_SPI_SetSlave(spi_reg, spi_priv->extra.chip_select);
		spi_reg->ctrl0 |= MXC_F_SPI_CTRL0_START;
		break;

	case CAPI_SPI_CS_MANUAL_DEASSERT:
		spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_TS_CTRL;
		spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

void max_capi_spi_isr(void *handle)
{
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *spi_priv;
	mxc_spi_regs_t *spi_reg;
	uint32_t interrupt_flags;
	uint8_t spi_id;
	bool has_error = false;

	if (!handle)
		return;

	spi_handle = (struct capi_spi_controller_handle *)handle;
	spi_priv = spi_handle->priv;
	spi_id = spi_priv->identifier;
	spi_reg = MXC_SPI_GET_SPI(spi_id);

	interrupt_flags = MXC_SPI_GetFlags(spi_reg);
	if (!interrupt_flags)
		return;

	/* RX FIFO overflow */
	if (interrupt_flags & MXC_F_SPI_INTFL_RX_OV) {
		has_error = true;
		if (spi_priv->callback) {
			spi_priv->callback(CAPI_SPI_EVENT_RX_FIFO_OVERFLOW,
					   spi_priv->callback_arg, 0);
		}
	}

	/* TX FIFO underflow */
	if (interrupt_flags & MXC_F_SPI_INTFL_TX_OV) {
		has_error = true;
		if (spi_priv->callback) {
			spi_priv->callback(CAPI_SPI_EVENT_TX_FIFO_UNDERFLOW,
					   spi_priv->callback_arg, 0);
		}
	}

	/* Check async */
	if (spi_priv->fifo_async.active) {
		struct max_capi_spi_fifo_async *fifo_async = &spi_priv->fifo_async;

		_max_capi_spi_fifo_drain_rx(spi_priv);
		_max_capi_spi_fifo_fill_tx(spi_priv);

		bool tx_complete = (fifo_async->tx_cnt >= fifo_async->clk_len);
		bool rx_complete = (!fifo_async->rx_buf) ||
				   (fifo_async->rx_cnt >=
				    (fifo_async->rx_skip + fifo_async->rx_size));

		if (tx_complete)
			spi_reg->inten &= ~MXC_F_SPI_INTEN_TX_THD;
		if (rx_complete)
			spi_reg->inten &= ~MXC_F_SPI_INTEN_RX_THD;

		if (tx_complete && rx_complete &&
		    (interrupt_flags & MXC_F_SPI_INTFL_CONT_DONE)) {
			spi_reg->inten &= ~(MXC_F_SPI_INTEN_TX_THD |
					    MXC_F_SPI_INTEN_RX_THD |
					    MXC_F_SPI_INTEN_CONT_DONE);
			spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
			spi_reg->dma &= ~(MXC_F_SPI_DMA_RX_FIFO_EN |
					  MXC_F_SPI_DMA_TX_FIFO_EN);
			fifo_async->active = false;
			async_transfer_in_progress[spi_id] = false;
			transfer_in_progress[spi_id] = false;
			if (spi_priv->callback)
				spi_priv->callback(CAPI_SPI_EVENT_XFR_DONE,
						   spi_priv->callback_arg, 0);
		}

		spi_reg->intfl = interrupt_flags;
		return;
	}

	/* Transfer complete */
	if (interrupt_flags & MXC_F_SPI_INTFL_CONT_DONE) {
		if (spi_priv->callback) {
			spi_priv->callback(CAPI_SPI_EVENT_XFR_DONE,
					   spi_priv->callback_arg, 0);
		}
	}

	/* Other errors */
	if (interrupt_flags & MXC_F_SPI_INTFL_ABORT) {
		has_error = true;
		if (spi_priv->callback) {
			spi_priv->callback(CAPI_SPI_EVENT_ERROR,
					   spi_priv->callback_arg, -ECANCELED);
		}
	}

	if (has_error && async_transfer_in_progress[spi_id]) {
		_max_capi_spi_dma_cleanup_channel(&dma_channel_rx[spi_id]);
		_max_capi_spi_dma_cleanup_channel(&dma_channel_tx[spi_id]);

		dma_completed_count[spi_id] = 0;
		async_transfer_in_progress[spi_id] = false;
		transfer_in_progress[spi_id] = false;

		spi_reg->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
		spi_reg->dma &= ~(MXC_F_SPI_DMA_RX_FIFO_EN | MXC_F_SPI_DMA_TX_FIFO_EN);
		spi_reg->dma &= ~(MXC_F_SPI_DMA_RX_EN | MXC_F_SPI_DMA_TX_EN);
	}

	/* Clear all interrupt flags that were handled */
	spi_reg->intfl = interrupt_flags;
}

const struct capi_spi_ops max_capi_spi_ops = {
	.init = max_capi_spi_init,
	.deinit = max_capi_spi_deinit,
	.transceive = max_capi_spi_transceive,
	.transceive_async = max_capi_spi_transceive_async,
	.register_callback = max_capi_spi_register_callback,
	.read_command = max_capi_spi_read_command,
	.read_command_async = max_capi_spi_read_command_async,
	.abort_async = max_capi_spi_abort_async,
	.set_cs = max_capi_spi_set_cs,
	.isr = max_capi_spi_isr,
};
