/***************************************************************************//**
 *   @file   maxim_spi.c
 *   @brief  Implementation of SPI driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "spi.h"
#include "mxc_errors.h"
#include "mxc_pins.h"
#include "maxim_spi.h"
#include "maxim_dma.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_units.h"
#include "no_os_dma.h"

#define SPI_MASTER_MODE	1
#define SPI_SINGLE_MODE	0

#define MAX_DELAY_SCLK	255
#define NS_PER_US	1000

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
struct max_dma_spi_xfer_data {
	struct no_os_spi_desc *spi;
	struct no_os_dma_ch *tx_ch;
	struct no_os_dma_ch *rx_ch;

	/* Used in order to free the memory allocated for the DMA transfer structs */
	struct no_os_dma_xfer_desc *first_xfer_tx;
	struct no_os_dma_xfer_desc *first_xfer_rx;

	/* The callback provided as a parameter in the async transfer case. */
	void (*cb)(void *);
	void *ctx;
};

/**
 * @brief Configure the SPI peripheral for the next DMA transfer.
 * @param old_xfer - The last completed transfer.
 * @param next_xfer - The next transfer in the SG list.
 * @param ctx - .
 */
static void max_dma_xfer_cycle(struct no_os_dma_xfer_desc *old_xfer,
			       struct no_os_dma_xfer_desc *next_xfer,
			       void *ctx)
{
	struct max_dma_spi_xfer_data *data = ctx;
	struct max_spi_state *max_spi_state = data->spi->extra;
	mxc_spi_regs_t *spi = MXC_SPI_GET_SPI(data->spi->device_id);

	/*
	 * Since this callback is shared, waait for both the RX and TX
	 * channels to finish the current transfer.
	 */
	while (no_os_dma_in_progress(max_spi_state->dma, data->rx_ch) &&
	       no_os_dma_in_progress(max_spi_state->dma, data->tx_ch));

	/* Wait for the SPI transfer to finish. */
	while(spi->stat & 1);

	if (!next_xfer) {
		if (data->cb)
			data->cb(data->ctx);

		no_os_dma_chan_unlock(data->tx_ch);
		no_os_dma_chan_unlock(data->rx_ch);
		no_os_free(data->first_xfer_tx);
		no_os_free(data->first_xfer_rx);
		no_os_free(data);

		return;
	}

	spi->ctrl1 |= next_xfer->length;
	spi->ctrl1 |= no_os_field_prep(MXC_F_SPI_CTRL1_RX_NUM_CHAR,
				       next_xfer->length);

	spi->ctrl0 |= MXC_F_SPI_CTRL0_START;
}

/**
 * @brief Set the reqsel field of the cfg (DMA channel specific) register
 * @param desc - SPI descriptor
 * @return void
 */
static void _max_dma_set_req(struct no_os_spi_desc *desc)
{
	struct max_spi_state *st = desc->extra;

	switch (desc->device_id) {
	case 0:
		st->dma_req_tx = MXC_V_DMA_CFG_REQSEL_SPI0TX;
		st->dma_req_rx = MXC_V_DMA_CFG_REQSEL_SPI0RX;
		break;
	case 1:
		st->dma_req_tx = MXC_V_DMA_CFG_REQSEL_SPI1TX;
		st->dma_req_rx = MXC_V_DMA_CFG_REQSEL_SPI1RX;
		break;
	case 2:
		st->dma_req_tx = MXC_V_DMA_CFG_REQSEL_SPI2TX;
		st->dma_req_rx = MXC_V_DMA_CFG_REQSEL_SPI2RX;
		break;
	case 3:
		st->dma_req_tx = MXC_V_DMA_CFG_REQSEL_SPI3TX;
		st->dma_req_rx = MXC_V_DMA_CFG_REQSEL_SPI3RX;
		break;
	default:
		return;
	}
}

/**
 * @brief Configure the VDDIO level for a SPI interface
 * @param desc - the SPI descriptor
 * @return 0 in case of success, -EINVAL otherwise
 */
static int32_t _max_spi_config_pins(struct no_os_spi_desc *desc)
{
	struct max_spi_state *st = desc->extra;
	mxc_gpio_cfg_t spi_pins;
	mxc_gpio_cfg_t cs;

	switch(desc->device_id) {
	case 0:
		spi_pins = gpio_cfg_spi0_1;

		switch(desc->chip_select) {
		case 0:
			cs = gpio_cfg_spi0_0;
			break;
		default:
			return -EINVAL;
		}
		break;
	case 1:
		spi_pins = gpio_cfg_spi1;

		switch(desc->chip_select) {
		case 0:
			cs = gpio_cfg_spi1_ss0;
			break;
		case 1:
			cs = gpio_cfg_spi1_ss1;
			break;
		case 2:
			cs = gpio_cfg_spi1_ss2;
			break;
		case 3:
			cs = gpio_cfg_spi1_ss3;
			break;
		default:
			return -EINVAL;
		}
		break;

	case 2:
		spi_pins = gpio_cfg_spi2;

		switch(desc->chip_select) {
		case 0:
			cs = gpio_cfg_spi2_ss0;
			break;
		case 1:
			cs = gpio_cfg_spi2_ss1;
			break;
		case 2:
			cs = gpio_cfg_spi2_ss2;
			break;
		case 3:
			cs = gpio_cfg_spi2_ss3;
			break;
		default:
			return -EINVAL;
		}
		break;
	case 3:
		spi_pins = gpio_cfg_spi3;

		switch(desc->chip_select) {
		case 0:
			cs = gpio_cfg_spi2_ss0;
			break;
		case 1:
			cs = gpio_cfg_spi2_ss1;
			break;
		case 2:
			cs = gpio_cfg_spi2_ss2;
			break;
		case 3:
			cs = gpio_cfg_spi3_ss3;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	spi_pins.vssel = st->init_param->vssel;
	cs.vssel = st->init_param->vssel;
	MXC_GPIO_Config(&spi_pins);
	MXC_GPIO_Config(&cs);

	return 0;
}

/**
 * @brief Set the closest first and last SCLK delays to what was requested
 * @param desc - SPI descriptor
 * @param msg - The message for which the delays will be set
 * @return void
 */
static void _max_delay_config(struct no_os_spi_desc *desc,
			      struct no_os_spi_msg *msg)
{
	struct max_spi_state *st = desc->extra;
	uint32_t sstime_cache;
	uint32_t ticks_delay;
	mxc_spi_regs_t *spi;
	uint32_t clk_rate;
	uint32_t ticks_ns;
	uint32_t delay_first_ns;
	uint32_t delay_last_ns;

	delay_first_ns = msg->cs_delay_first * 1000 +
			 desc->platform_delays.cs_delay_first;
	delay_last_ns = msg->cs_delay_last * 1000 + desc->platform_delays.cs_delay_last;

	if (delay_first_ns == st->cs_delay_first &&
	    delay_last_ns == st->cs_delay_last)
		return;

	spi = MXC_SPI_GET_SPI(desc->device_id);
	sstime_cache = spi->ss_time;
	clk_rate = MXC_SPI_GetPeripheralClock(spi);
	ticks_ns = NO_OS_DIV_ROUND_CLOSEST(NANO, clk_rate);

	if (delay_first_ns != st->cs_delay_first) {
		/**
		 * The minimum number of delay ticks is 1. If 0 is written to the
		 * sstime register, there would be a delay of 256 ticks.
		 */
		if (delay_first_ns == 0)
			ticks_delay = 1;
		else
			ticks_delay = delay_first_ns / ticks_ns;

		if (ticks_delay > MAX_DELAY_SCLK) {
			pr_warning("cs_delay_first value is too high\n");
			goto error;
		}

		spi->ss_time &= ~MXC_F_SPI_SS_TIME_SSACT1;
		spi->ss_time |= no_os_field_prep(MXC_F_SPI_SS_TIME_SSACT1, ticks_delay);
	}
	if (delay_last_ns != st->cs_delay_last) {
		/**
		 * The minimum number of delay ticks is 1. If 0 is written to the
		 * sstime register, there would be a delay of 256 ticks.
		 */
		if (delay_last_ns == 0)
			ticks_delay = 1;
		else
			ticks_delay = delay_last_ns / ticks_ns;

		if (ticks_delay > MAX_DELAY_SCLK) {
			pr_warning("cs_delay_last value is too high\n");
			goto error;
		}

		spi->ss_time &= ~MXC_F_SPI_SS_TIME_SSACT2;
		spi->ss_time |= no_os_field_prep(MXC_F_SPI_SS_TIME_SSACT2, ticks_delay);
	}

	st->cs_delay_first = delay_first_ns;
	st->cs_delay_last = delay_last_ns;

	return;

error:
	spi->ss_time = sstime_cache;
}

/**
 * @brief Configure a SPI peripheral
 * @param spi - The SPI descriptor which stores the configuration parameters.
 * @return 0 in case of success, a negative error code otherwise
 */
static int _max_spi_config(struct no_os_spi_desc *desc)
{
	struct max_spi_init_param *eparam;
	struct max_spi_state *st;
	int32_t ret;

	st = desc->extra;
	eparam = st->init_param;

	ret = MXC_SPI_Init(MXC_SPI_GET_SPI(desc->device_id), SPI_MASTER_MODE,
			   SPI_SINGLE_MODE,
			   eparam->num_slaves, eparam->polarity, desc->max_speed_hz);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = _max_spi_config_pins(desc);
	if (ret)
		goto err_init;

	ret = MXC_SPI_SetMode(MXC_SPI_GET_SPI(desc->device_id), desc->mode);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = MXC_SPI_SetWidth(MXC_SPI_GET_SPI(desc->device_id), SPI_WIDTH_STANDARD);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = MXC_SPI_SetDataSize(MXC_SPI_GET_SPI(desc->device_id), 8);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	return 0;
err_init:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(desc->device_id));

	return ret;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_init(struct no_os_spi_desc **desc,
		     const struct no_os_spi_init_param *param)
{
	int32_t ret;
	struct no_os_spi_desc *descriptor;
	struct max_spi_init_param *eparam;
	struct max_spi_state *st;

	if (!param || !param->extra)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	st = no_os_calloc(1, sizeof(*st));
	if (!st) {
		ret = -ENOMEM;
		goto err;
	}

	eparam = param->extra;
	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->bit_order = param->bit_order;

	st->init_param = eparam;
	descriptor->extra = st;

	if (descriptor->device_id >= MXC_SPI_INSTANCES) {
		ret = -EINVAL;
		goto err;
	}

	ret = _max_spi_config(descriptor);
	if (ret)
		goto err;

	if (eparam->dma_param) {
		/*
		 * The RX complete interrupt needs to have higher priority,
		 * otherwise there is race condition, since for short transfers, the TX
		 * channel will start a transfer which will finish before the RX has chance
		 * to start.
		 */
		if (eparam->dma_rx_priority >= eparam->dma_tx_priority) {
			ret = -EINVAL;
			goto err;
		}

		ret = no_os_dma_init(&st->dma, eparam->dma_param);
		if (ret)
			goto err;

		_max_dma_set_req(descriptor);
	}

	*desc = descriptor;

	return 0;
err:
	no_os_free(st);
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(desc->device_id));
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Configure and start a series of transfers using DMA.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @param callback - Function to be invoked once the transfers are done.
 * @param ctx - User defined parameter for the callback function.
 * @param is_async - Whether or not the function should wait for the completion.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_config_dma_and_start(struct no_os_spi_desc *desc,
					struct no_os_spi_msg *msgs,
					uint32_t len,
					void (*callback)(void *),
					void *ctx, bool is_async)
{
	mxc_spi_regs_t *spi = MXC_SPI_GET_SPI(desc->device_id);
	static uint32_t last_slave_id[MXC_SPI_INSTANCES];
	struct max_dma_spi_xfer_data *sync_xfer_data;
	struct max_spi_state *max_spi = desc->extra;
	struct no_os_dma_xfer_desc *rx_ch_xfer;
	struct no_os_dma_xfer_desc *tx_ch_xfer;
	struct no_os_dma_ch *tx_ch;
	struct no_os_dma_ch *rx_ch;
	uint32_t slave_id;
	size_t i = 0;
	int32_t ret;

	slave_id = desc->chip_select;
	if (slave_id != last_slave_id[desc->device_id]) {
		ret = _max_spi_config(desc);
		if (ret)
			return ret;

		last_slave_id[desc->device_id] = slave_id;
	}

	/* Assert CS desc->chip_select when the SPI transaction is started */
	spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_SEL;
	spi->ctrl0 |= no_os_field_prep(MXC_F_SPI_CTRL0_SS_SEL,
				       NO_OS_BIT(desc->chip_select));

	rx_ch_xfer = no_os_calloc(len, sizeof(*rx_ch_xfer));
	if (!rx_ch_xfer)
		return -ENOMEM;

	tx_ch_xfer = no_os_calloc(len, sizeof(*tx_ch_xfer));
	if (!tx_ch_xfer) {
		ret = -ENOMEM;
		goto free_rx_ch_xfer;
	}

	sync_xfer_data = no_os_calloc(1, sizeof(*sync_xfer_data));
	if (!sync_xfer_data) {
		ret = -ENOMEM;
		goto free_tx_ch_xfer;
	}

	/* Flush the RX and TX FIFOs */
	spi->dma |= MXC_F_SPI_DMA_RX_FIFO_CLEAR | MXC_F_SPI_DMA_TX_FIFO_CLEAR;
	/* Enable SPI */
	spi->int_fl |= MXC_F_SPI_INT_FL_M_DONE;

	if (msgs[0].cs_change)
		spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_CTRL;
	else
		spi->ctrl0 |= MXC_F_SPI_CTRL0_SS_CTRL;

	spi->ctrl1 = msgs[0].bytes_number;
	/* Enable the TX FIFO */
	spi->dma |= MXC_F_SPI_DMA_TX_FIFO_EN;
	spi->ctrl1 |= no_os_field_prep(MXC_F_SPI_CTRL1_RX_NUM_CHAR,
				       msgs[0].bytes_number);
	/* Enable the RX FIFO */
	spi->dma |= MXC_F_SPI_DMA_RX_FIFO_EN;

	/* Allow DMA transfers for RX and TX SPI FIFOs */
	spi->dma |= MXC_F_SPI_DMA_RX_DMA_EN | MXC_F_SPI_DMA_TX_DMA_EN;

	ret = no_os_dma_acquire_channel(max_spi->dma, &tx_ch);
	if (ret)
		goto free_sync_xfer_data;

	ret = no_os_dma_acquire_channel(max_spi->dma, &rx_ch);
	if (ret)
		goto release_tx_ch;

	sync_xfer_data->spi = desc;
	sync_xfer_data->rx_ch = rx_ch;
	sync_xfer_data->tx_ch = tx_ch;
	sync_xfer_data->first_xfer_tx = tx_ch_xfer;
	sync_xfer_data->first_xfer_rx = rx_ch_xfer;

	if (is_async) {
		sync_xfer_data->cb = callback;
		sync_xfer_data->ctx = ctx;
	}

	for (i = 0; i < len; i++) {
		tx_ch_xfer[i].src = msgs[i].tx_buff;
		tx_ch_xfer[i].dst = (uint8_t *)max_spi->dma_req_tx;
		tx_ch_xfer[i].length = msgs[i].bytes_number;
		tx_ch_xfer[i].periph = NO_OS_DMA_IRQ;
		tx_ch_xfer[i].xfer_complete_cb = max_dma_xfer_cycle;
		tx_ch_xfer[i].xfer_complete_ctx = sync_xfer_data;
		tx_ch_xfer[i].xfer_type = MEM_TO_DEV;
		tx_ch_xfer[i].irq_priority = max_spi->init_param->dma_tx_priority;

		rx_ch_xfer[i].dst = msgs[i].rx_buff;
		rx_ch_xfer[i].src = (uint8_t *)max_spi->dma_req_rx;
		rx_ch_xfer[i].length = msgs[i].bytes_number;
		rx_ch_xfer[i].periph = NO_OS_DMA_IRQ;
		rx_ch_xfer[i].xfer_type = DEV_TO_MEM;
		rx_ch_xfer[i].irq_priority = max_spi->init_param->dma_rx_priority;
	}

	ret = no_os_dma_config_xfer(max_spi->dma, tx_ch_xfer, len, tx_ch);
	if (ret)
		goto release_rx_ch;

	ret = no_os_dma_config_xfer(max_spi->dma, rx_ch_xfer, len, rx_ch);
	if (ret)
		goto abort_rx_tx;

	/* Start the transaction */
	spi->ctrl0 |= MXC_F_SPI_CTRL0_START;

	ret = no_os_dma_xfer_start(max_spi->dma, rx_ch);
	if (ret)
		goto abort_rx_tx;

	ret = no_os_dma_xfer_start(max_spi->dma, tx_ch);
	if (ret)
		goto abort_rx_tx;

	if (!is_async) {
		while(!no_os_dma_is_completed(max_spi->dma, rx_ch) ||
		      !no_os_dma_is_completed(max_spi->dma, tx_ch));

		while(spi->stat & 1);
		/* End the transaction */
		spi->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
		/* Disable the RX and TX FIFOs */
		spi->dma &= ~(MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN);
	}

	return 0;

abort_rx_tx:
	no_os_dma_xfer_abort(max_spi->dma, tx_ch);
	no_os_dma_xfer_abort(max_spi->dma, rx_ch);
release_rx_ch:
	no_os_dma_release_channel(max_spi->dma, rx_ch);
release_tx_ch:
	no_os_dma_release_channel(max_spi->dma, tx_ch);
free_sync_xfer_data:
	no_os_free(sync_xfer_data);
free_tx_ch_xfer:
	no_os_free(tx_ch_xfer);
free_rx_ch_xfer:
	no_os_free(rx_ch_xfer);

	return ret;
}


/**
 * @brief Configure and start a series of transfers using DMA. Wait for the
 * 	  completion before returning.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_spi_dma_transfer_sync(struct no_os_spi_desc *desc,
		struct no_os_spi_msg *msgs,
		uint32_t len)
{
	return max_config_dma_and_start(desc, msgs, len, NULL, NULL, false);
}

/**
 * @brief Configure and start a series of transfers using DMA.
 * 	  Return immediately, and invoke a callback once they're completed.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @param callback - Function to be invoked once the transfers are done.
 * @param ctx - User defined parameter for the callback function.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_spi_dma_transfer_async(struct no_os_spi_desc *desc,
		struct no_os_spi_msg *msgs,
		uint32_t len,
		void (*callback)(void *),
		void *ctx)
{
	return max_config_dma_and_start(desc, msgs, len, callback, ctx, true);
}

/**
 * @brief Write/read multiple messages to/from SPI.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_transfer(struct no_os_spi_desc *desc,
			 struct no_os_spi_msg *msgs,
			 uint32_t len)
{
	mxc_spi_regs_t *spi = MXC_SPI_GET_SPI(desc->device_id);
	static uint32_t last_slave_id[MXC_SPI_INSTANCES];
	uint32_t tx_cnt;
	uint32_t rx_cnt;
	bool rx_done = true;
	bool tx_done = true;
	uint32_t slave_id;
	size_t i = 0;
	int32_t ret;

	if (!desc || !msgs)
		return -EINVAL;

	slave_id = desc->chip_select;
	if (slave_id != last_slave_id[desc->device_id]) {
		ret = _max_spi_config(desc);
		if (ret)
			return ret;

		last_slave_id[desc->device_id] = slave_id;
	}

	/* Assert CS desc->chip_select when the SPI transaction is started */
	spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_SEL;
	spi->ctrl0 |= no_os_field_prep(MXC_F_SPI_CTRL0_SS_SEL,
				       NO_OS_BIT(desc->chip_select));

	for (i = 0; i < len; i++) {
		/* Flush the RX and TX FIFOs */
		spi->dma |= MXC_F_SPI_DMA_RX_FIFO_CLEAR | MXC_F_SPI_DMA_TX_FIFO_CLEAR;
		/* Enable SPI */
		spi->int_fl |= MXC_F_SPI_INT_FL_M_DONE;
		spi->ctrl1 = 0;

		rx_cnt = 0;
		tx_cnt = 0;

		if (msgs[i].cs_change)
			spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_CTRL;
		else
			spi->ctrl0 |= MXC_F_SPI_CTRL0_SS_CTRL;

		_max_delay_config(desc, &msgs[i]);

		if (msgs[i].tx_buff) {
			/* Set the transfer size in the TX direction */
			spi->ctrl1 = msgs[i].bytes_number;
			// spi->ctrl1 = msgs[i].bytes_number;
			tx_done = false;
			/* Enable the TX FIFO */
			spi->dma |= MXC_F_SPI_DMA_TX_FIFO_EN;
			tx_cnt += MXC_SPI_WriteTXFIFO(spi, &msgs[i].tx_buff[tx_cnt],
						      msgs[i].bytes_number - tx_cnt);
			tx_done = (tx_cnt == msgs[i].bytes_number) ? true : false;
		}
		if (msgs[i].rx_buff) {
			/* Set the transfer size in the RX direction */
			spi->ctrl1 |= no_os_field_prep(MXC_F_SPI_CTRL1_RX_NUM_CHAR,
						       msgs[i].bytes_number);
			/* Enable the RX FIFO */
			spi->dma |= MXC_F_SPI_DMA_RX_FIFO_EN;
			rx_done = false;
		}

		/* Start the transaction */
		spi->ctrl0 |= MXC_F_SPI_CTRL0_START;
		while (!(rx_done && tx_done)) {
			if (msgs[i].tx_buff && tx_cnt < msgs[i].bytes_number) {
				tx_cnt += MXC_SPI_WriteTXFIFO(spi, &msgs[i].tx_buff[tx_cnt],
							      msgs[i].bytes_number - tx_cnt);
				tx_done = (tx_cnt == msgs[i].bytes_number) ? true : false;
			}
			if (msgs[i].rx_buff && rx_cnt < msgs[i].bytes_number) {
				rx_cnt += MXC_SPI_ReadRXFIFO(spi, &msgs[i].rx_buff[rx_cnt],
							     msgs[i].bytes_number - rx_cnt);
				rx_done = (rx_cnt == msgs[i].bytes_number) ? true : false;
			}
		}

		/* Wait for the RX and TX FIFOs to empty */
		while (!(spi->int_fl & MXC_F_SPI_INT_FL_M_DONE));

		/* End the transaction */
		spi->ctrl0 &= ~MXC_F_SPI_CTRL0_START;

		/* Disable the RX and TX FIFOs */
		spi->dma &= ~(MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN);

		no_os_udelay(msgs[i].cs_change_delay);
	}

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_write_and_read(struct no_os_spi_desc *desc,
			       uint8_t *data,
			       uint16_t bytes_number)
{
	struct no_os_spi_msg xfer = {
		.rx_buff = data,
		.tx_buff = data,
		.bytes_number = bytes_number,
		.cs_change = 1,
	};

	return max_spi_transfer(desc, &xfer, 1);
}

/**
 * @brief maxim platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops max_spi_ops = {
	.init = &max_spi_init,
	.write_and_read = &max_spi_write_and_read,
	.transfer = &max_spi_transfer,
	.dma_transfer_sync = &max_spi_dma_transfer_sync,
	.dma_transfer_async = &max_spi_dma_transfer_async,
	.remove = &max_spi_remove
};
