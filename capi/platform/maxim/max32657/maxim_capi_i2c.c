/*******************************************************************************
 *   @file   maxim_capi_i2c.c
 *   @brief  Implementation of I2C functions with CAPI
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

#include <errno.h>
#include <string.h>
#include "capi_i2c.h"
#include "capi_dma.h"
#include "capi_irq.h"
#include "maxim_capi_i2c.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_irq.h"
#include "i3c.h"

/** Static variables **********************************************************/

static struct capi_i2c_controller_handle *i2c[MXC_CFG_I3C_INSTANCES] = {NULL};
static volatile bool dma_completed[MXC_CFG_I3C_INSTANCES] = {false};
static struct capi_dma_chan *dma_channel_rx[MXC_CFG_I3C_INSTANCES];
static struct capi_dma_chan *dma_channel_tx[MXC_CFG_I3C_INSTANCES];
static bool async_transfer_in_progress[MXC_CFG_I3C_INSTANCES] = {false};

/** Forward declarations ******************************************************/

void max_capi_i2c_isr(void *handle);
int max_capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle *handle,
				     enum capi_i2c_speed speed,
				     uint8_t duty_cycle);

/** Helper functions **********************************************************/

/**
 * @brief Set TX data for target mode
 * @param i2c_priv The I2C private data
 * @param data Pointer to TX data buffer
 * @param length Number of bytes to transmit
 */
static void _max_capi_i2c_target_set_tx_data(struct max_capi_i2c_priv *i2c_priv,
		const uint8_t *data, uint16_t length)
{
	i2c_priv->target->tx_buffer = data;
	i2c_priv->target->tx_length = length;
	i2c_priv->target->tx_count = 0;
}

/**
 * @brief Get RX data for target mode
 * @param i2c_priv The I2C private data
 * @param[out] data Pointer to RX data buffer
 * @param[out] length Number of bytes received
 */
static void _max_capi_i2c_target_get_rx_data(struct max_capi_i2c_priv *i2c_priv,
		uint8_t **data, uint16_t *length)
{
	*data = i2c_priv->target->rx_buffer;
	*length = i2c_priv->target->rx_count;
}


/**
 * @brief Get PP/OD frequencies for a given I2C speed
 * @param[in] i2c_freq Desired I2C frequency
 * @param[out] pp_freq PP frequency
 * @param[out] od_freq OD frequency
 * @return 0 on success, -EINVAL if unsupported frequency
 */
static int _max_capi_i2c_get_frequencies(uint32_t i2c_freq, uint32_t *pp_freq,
		uint32_t *od_freq)
{
	if (!pp_freq || !od_freq)
		return -EINVAL;

	/** Values obtained through trial-and-error by sweeping through
	    PP and OD frequencies and getting the I2C frequency.
	    Lowest frequencies chosen for power efficiency. */
	switch (i2c_freq) {
	case MAX_CAPI_I2C_SPEED_STANDARD:
		*pp_freq = 2500000;
		*od_freq =  500000;
		break;
	case MAX_CAPI_I2C_SPEED_FAST:
		*pp_freq = 12500000;
		*od_freq =  1390000;
		break;
	case MAX_CAPI_I2C_SPEED_FAST_PLUS:
		*pp_freq = 2500000;
		*od_freq = 2500000;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Configure I3C peripheral in I2C mode
 * @param i2c_priv The private handle
 * @param target_mode Target mode: true,
 * 		      Controller mode: false
 * @param addr_or_freq Address if target mode,
 * 		       Frequency if controller mode
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_configure_hardware(struct max_capi_i2c_priv *i2c_priv,
		bool target_mode,
		uint32_t addr_or_freq)
{
	struct max_capi_i2c_extra *i2c_extra = i2c_priv->extra;
	struct max_capi_i2c_target_state *target = i2c_priv->target;
	uint8_t i2c_id = i2c_priv->identifier;
	mxc_i3c_config_t i3c_config;
	mxc_gpio_cfg_t i3c_pins;
	int ret;
	bool target_allocated = false;

	ret = MXC_I3C_Shutdown(MXC_I3C_GET_I3C(i2c_id));
	if (ret != E_NO_ERROR)
		return -EINVAL;

	if (target_mode) {
		i3c_config = (mxc_i3c_config_t) {
			.target_mode = true,
			.static_addr = addr_or_freq,
			.i2c_hz = 0, /* These values are ignored in target mode */
			.od_hz = 0,
			.pp_hz = 0,
		};

		if (!target) {
			target = capi_calloc(1, sizeof(*target));
			if (!target)
				return -ENOMEM;
			i2c_priv->target = target;
			target_allocated = true;
		}
	} else {
		i3c_config = (mxc_i3c_config_t) {
			.target_mode = false,
			.static_addr = 0, /* This value is ignored in controller mode */
			.i2c_hz = addr_or_freq,
		};
		ret = _max_capi_i2c_get_frequencies(addr_or_freq,
						    &i3c_config.pp_hz,
						    &i3c_config.od_hz);
		if (ret)
			return ret;

		i2c_priv->freq = addr_or_freq;
	}

	ret = MXC_I3C_Init(MXC_I3C_GET_I3C(i2c_id), &i3c_config);
	if (ret != E_NO_ERROR) {
		ret = -EIO;
		if (target_mode && target_allocated)
			goto free_target;

		return ret;
	}

	i3c_pins = gpio_cfg_i3c;
	i3c_pins.vssel = (mxc_gpio_vssel_t)i2c_extra->vssel;
	MXC_GPIO_Config(&i3c_pins);

	return 0;

free_target:
	if (i2c_priv->target) {
		capi_free(target);
		i2c_priv->target = NULL;
	}

	return ret;
}

/**
 * @brief Handle target mode interrupts
 *
 * IMPORTANT: The callback is invoked from interrupt context and must:
 * - Execute quickly without blocking
 * - For CAPI_I2C_STXREQ event: Set target->tx_buffer and target->tx_length
 *   before returning, or set tx_buffer to NULL for underrun
 * - For CAPI_I2C_SRXREQ event: Prepare to receive data
 * - Return immediately
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral
 */
static void _max_capi_i2c_target_isr(struct max_capi_i2c_priv *i2c_priv,
				     mxc_i3c_regs_t *i3c)
{
	struct max_capi_i2c_target_state *target = i2c_priv->target;
	uint32_t flags = MXC_I3C_Target_GetFlags(i3c);
	capi_i2c_callback callback = i2c_priv->callback;
	void *callback_arg = i2c_priv->callback_arg;

	if (!target)
		return;

	/* START */
	if (flags & MXC_F_I3C_TARG_INTFL_START) {
		target->rx_count = 0;
		target->tx_count = 0;

		MXC_I3C_ClearRXFIFO(i3c);
		MXC_I3C_ClearTXFIFO(i3c);

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_START);
	}

	/* Address match */
	if (flags & MXC_F_I3C_TARG_INTFL_ADDRMATCH) {
		uint32_t status = i3c->targ_status;

		if (status & MXC_F_I3C_TARG_STATUS_RX_SDR) {
			/* Controller wants to write to us */
			target->state = MAX_CAPI_I2C_TARGET_STATE_RX;

			if (callback)
				callback(CAPI_I2C_SRXREQ, callback_arg, 0);

		} else if (status & MXC_F_I3C_TARG_STATUS_TX_SDR) {
			/* Controller wants to read from us */
			target->state = MAX_CAPI_I2C_TARGET_STATE_TX;

			if (callback)
				callback(CAPI_I2C_STXREQ, callback_arg, 0);
		}

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_ADDRMATCH);
	}

	/* RX ready - controller is writing data to us */
	if (flags & MXC_F_I3C_TARG_INTFL_RX_RDY) {
		if (target->state == MAX_CAPI_I2C_TARGET_STATE_RX) {
			while (i3c->targ_status & MXC_F_I3C_TARG_STATUS_RX_RDY) {
				if (target->rx_count < MAX_CAPI_I2C_TARGET_RX_BUFFER_SIZE) {
					uint8_t byte = (uint8_t)(i3c->targ_rxfifo8 & 0xFF);
					target->rx_buffer[target->rx_count++] = byte;
				} else {
					/* Buffer overflow - read and discard */
					(void)i3c->targ_rxfifo8;

					if (callback)
						callback(CAPI_I2C_RXOVER, callback_arg, 0);
				}
			}
		}

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_RX_RDY);
	}

	/* TX not full - controller is reading from us */
	if (flags & MXC_F_I3C_TARG_INTFL_TX_NFULL) {
		if (target->state == MAX_CAPI_I2C_TARGET_STATE_TX) {
			if (target->tx_buffer == NULL) {
				/* TX underrun */
				MXC_I3C_Target_DisableInt(i3c, MXC_F_I3C_TARG_INTCLR_TX_NFULL);

				if (callback)
					callback(CAPI_I2C_TXUNDER, callback_arg, 0);
			} else {
				while ((i3c->targ_status & MXC_F_I3C_TARG_STATUS_TX_NFULL) &&
				       (target->tx_count < target->tx_length)) {
					uint8_t byte = target->tx_buffer[target->tx_count++];
					i3c->targ_txfifo8 = byte;
				}

				/* Check if we've sent all data */
				if (target->tx_count >= target->tx_length)
					MXC_I3C_Target_DisableInt(i3c, MXC_F_I3C_TARG_INTCLR_TX_NFULL);
			}
		}

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_TX_NFULL);
	}



	/* STOP */
	if (flags & MXC_F_I3C_TARG_INTFL_STOP) {
		int event_extra = 0;

		if (target->state == MAX_CAPI_I2C_TARGET_STATE_RX) {
			/* Application should handle rx_buffer and rx_count
			   in callback */
			event_extra = target->rx_count;
		}

		target->state = MAX_CAPI_I2C_TARGET_STATE_IDLE;
		target->tx_buffer = NULL;
		target->tx_length = 0;

		MXC_I3C_Target_EnableInt(i3c, MXC_F_I3C_TARG_INTEN_TX_NFULL);

		if (callback)
			callback(CAPI_I2C_XFR_DONE, callback_arg, event_extra);

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_STOP);
	}

	/* Errors/warnings */
	if (flags & MXC_F_I3C_TARG_INTFL_ERRWARN) {
		uint32_t errwarn = i3c->targ_errwarn;

		/* Clear error */
		i3c->targ_errwarn = errwarn;
		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_ERRWARN);

		if (callback)
			callback(CAPI_I2C_NAKD, callback_arg, errwarn);
	}
}

/**
 * @brief Reset async state to IDLE
 * @param i2c_priv I2C private data
 */
static void _max_capi_i2c_reset_async_state(struct max_capi_i2c_priv *i2c_priv)
{
	if (!i2c_priv->async)
		return;

	i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_IDLE;
	i2c_priv->async->target_addr = 0;
	i2c_priv->async->subaddr_buf = NULL;
	i2c_priv->async->subaddr_len = 0;
	i2c_priv->async->subaddr_idx = 0;
	i2c_priv->async->data_buf = NULL;
	i2c_priv->async->data_len = 0;
	i2c_priv->async->data_idx = 0;
	i2c_priv->async->send_stop = false;
	i2c_priv->async->result = 0;
	i2c_priv->async->is_transmit = false;
}

/**
 * @brief Write bytes from buffer to TX FIFO
 * @param i3c I3C peripheral registers
 * @param buf Source buffer
 * @param len Number of bytes to write
 * @param idx Pointer to current index
 * @param is_last_chunk True if this is the last data chunk
 * @return Number of bytes actually written
 */
static uint16_t _max_capi_i2c_write_tx_fifo(mxc_i3c_regs_t *i3c,
		const uint8_t *buf, uint16_t len,
		uint16_t *idx, bool is_last_chunk)
{
	uint16_t written = 0;

	while ((*idx < len) &&
	       (MXC_I3C_Controller_GetTXCount(i3c) < MAX_CAPI_I2C_FIFO_DEPTH)) {
		bool is_last = (is_last_chunk && (*idx == (len - 1)));
		int ret = MXC_I3C_WriteTXFIFO(i3c, &buf[*idx], 1, is_last, 0);
		if (ret <= 0)
			break;
		(*idx)++;
		written++;
	}

	return written;
}

/**
 * @brief Read available bytes from RX FIFO to buffer
 * @param i3c I3C peripheral registers
 * @param buf Destination buffer
 * @param len Total buffer length
 * @param idx Pointer to current index
 * @return Number of bytes read
 */
static uint16_t _max_capi_i2c_read_rx_fifo(mxc_i3c_regs_t *i3c,
		uint8_t *buf, uint16_t len,
		uint16_t *idx)
{
	uint16_t available = MXC_I3C_Controller_GetRXCount(i3c);
	uint16_t to_read = (available < len - (*idx)) ? available : (len - (*idx));

	if (to_read > 0) {
		int ret = MXC_I3C_ReadRXFIFO(i3c, &buf[*idx], to_read, 0);
		if (ret > 0)
			*idx += ret;
		return ret > 0 ? ret : 0;
	}

	return 0;
}

/**
 * @brief Perform I2C transmit using FIFO (interrupt-driven)
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_transmit_fifo(mxc_i3c_regs_t *i3c,
				       struct max_capi_i2c_async_state *async)
{
	int ret;

	async->is_transmit = true;

	if (async->subaddr_buf)
		async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR;
	else
		async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;

	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
				async->target_addr, 0);
	if (ret != E_SUCCESS)
		return (ret == E_BUSY) ? -EBUSY : -EIO;

	if (async->state == MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR) {
		_max_capi_i2c_write_tx_fifo(i3c, async->subaddr_buf,
					    async->subaddr_len,
					    &async->subaddr_idx,
					    false);
		if (async->subaddr_idx >= async->subaddr_len)
			async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;
	}

	if (async->state == MAX_CAPI_I2C_ASYNC_STATE_TX_DATA) {
		_max_capi_i2c_write_tx_fifo(i3c, async->data_buf,
					    async->data_len,
					    &async->data_idx,
					    true);
	}

	MXC_I3C_Controller_ClearFlags(i3c, 0xFFFFFFFF);
	MXC_I3C_Controller_EnableInt(i3c, MXC_F_I3C_CONT_INTEN_TX_NFULL |
				     MXC_F_I3C_CONT_INTEN_DONE |
				     MXC_F_I3C_CONT_INTEN_ERRWARN);

	return 0;
}

/**
 * @brief Perform I2C receive using FIFO (interrupt-driven)
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_receive_fifo(mxc_i3c_regs_t *i3c,
				      struct max_capi_i2c_async_state *async)
{
	int ret;
	uint32_t int_mask;

	async->is_transmit = false;

	if (async->subaddr_buf) {
		async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR;

		ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
					async->target_addr, 0);
		if (ret != E_SUCCESS)
			return (ret == E_BUSY) ? -EBUSY : -EIO;

		_max_capi_i2c_write_tx_fifo(i3c, async->subaddr_buf,
					    async->subaddr_len,
					    &async->subaddr_idx,
					    true);

		int_mask = MXC_F_I3C_CONT_INTEN_TX_NFULL |
			   MXC_F_I3C_CONT_INTEN_DONE |
			   MXC_F_I3C_CONT_INTEN_ERRWARN;
	} else {
		async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_DATA;

		ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_READ,
					async->target_addr, async->data_len);
		if (ret != E_SUCCESS)
			return (ret == E_BUSY) ? -EBUSY : -EIO;

		int_mask = MXC_F_I3C_CONT_INTEN_RX_RDY |
			   MXC_F_I3C_CONT_INTEN_DONE |
			   MXC_F_I3C_CONT_INTEN_ERRWARN;
	}

	MXC_I3C_Controller_ClearFlags(i3c, 0xFFFFFFFF);
	MXC_I3C_Controller_EnableInt(i3c, int_mask);

	return 0;
}

/**
 * @brief Cleanup and deinit a DMA channel
 * @param channel The DMA channel
 */
void _max_capi_i2c_dma_cleanup_channel(struct capi_dma_chan **channel)
{
	if (channel && *channel) {
		capi_dma_xfer_abort(*channel);
		capi_dma_deinit_chan(*channel);
		*channel = NULL;
	}
}

/**
 * @brief DMA transfer completion callback
 * @param transfer The DMA transfer
 * @param ctx The user data passed (struct max_capi_i2c_priv)
 */
static void _max_capi_i2c_dma_complete_callback(struct capi_dma_transfer
		*transfer,
		void *ctx)
{
	struct max_capi_i2c_priv *i2c_priv = ctx;

	if (!i2c_priv || !i2c_priv->async)
		return;

	i2c_priv->callback_active = true;

	uint8_t i2c_id = i2c_priv->identifier;
	mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_id);
	capi_i2c_callback callback;
	void *callback_arg;

	callback = i2c_priv->callback;
	callback_arg = i2c_priv->callback_arg;

	dma_completed[i2c_id] = true;
	MXC_I3C_Controller_EnableInt(i3c, MXC_F_I3C_CONT_INTEN_DONE);

	if (i2c_priv->async->send_stop)
		MXC_I3C_EmitI2CStop(i3c);

	_max_capi_i2c_dma_cleanup_channel(&dma_channel_rx[i2c_id]);
	_max_capi_i2c_dma_cleanup_channel(&dma_channel_tx[i2c_id]);

	dma_completed[i2c_id] = false;
	async_transfer_in_progress[i2c_id] = false;

	if (i2c_priv->async->dma_allocated_buffer) {
		capi_free(i2c_priv->async->dma_allocated_buffer);
		i2c_priv->async->dma_allocated_buffer = NULL;
	}

	_max_capi_i2c_reset_async_state(i2c_priv);

	if (callback)
		callback(CAPI_I2C_XFR_DONE, callback_arg, 0);

	i2c_priv->callback_active = false;
}

/**
 * @brief Perform I2C transmit using DMA
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_transmit_dma(struct max_capi_i2c_priv *i2c_priv,
				      mxc_i3c_regs_t *i3c,
				      struct max_capi_i2c_async_state *async)
{
	int ret;
	struct capi_dma_handle *dma_handle = i2c_priv->dma_handle;
	uint8_t i2c_id = i2c_priv->identifier;
	uint8_t *tx_buffer;
	uint16_t tx_len;
	struct max_capi_dma_xfer_extra dma_tx_extra;
	struct capi_dma_transfer dma_tx_xfer;

	if (async_transfer_in_progress[i2c_id])
		return -EBUSY;

	async_transfer_in_progress[i2c_id] = true;

	if (async->subaddr_buf) {
		tx_len = async->subaddr_len + async->data_len;
		tx_buffer = capi_malloc(tx_len);
		if (!tx_buffer) {
			async_transfer_in_progress[i2c_id] = false;
			return -ENOMEM;
		}
		memcpy(tx_buffer, async->subaddr_buf, async->subaddr_len);
		memcpy(tx_buffer + async->subaddr_len, async->data_buf, async->data_len);
		i2c_priv->async->dma_allocated_buffer = tx_buffer;
	} else {
		tx_buffer = async->data_buf;
		tx_len = async->data_len;
		i2c_priv->async->dma_allocated_buffer = NULL;
	}

	async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;

	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
				async->target_addr, 0);
	if (ret != E_SUCCESS) {
		if (tx_buffer != async->data_buf)
			capi_free(tx_buffer);
		async_transfer_in_progress[i2c_id] = false;
		return -EIO;
	}

	i3c->cont_dmactrl = MXC_S_I3C_CONT_DMACTRL_TX_EN_EN;

	ret = capi_dma_init_chan(dma_handle, &dma_channel_tx[i2c_id], 0);
	if (ret)
		goto error_cleanup;

	dma_tx_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_I3C_TX_CONTROLLER,
	};

	dma_tx_xfer = (struct capi_dma_transfer) {
		.src = tx_buffer,
		.dst = &i3c->cont_txfifo8,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_NO_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.length = tx_len,
		.extra = &dma_tx_extra,
		.xfer_type = CAPI_DMA_MEM_TO_DEV,
		.user_data = i2c_priv,
	};

	ret = capi_dma_config_xfer(dma_channel_tx[i2c_id], &dma_tx_xfer);
	if (ret)
		goto error_deinit_tx;

	ret = capi_dma_register_complete_callback(dma_channel_tx[i2c_id],
			_max_capi_i2c_dma_complete_callback,
			i2c_priv);

	dma_completed[i2c_id] = false;

	ret = capi_dma_xfer_start(dma_channel_tx[i2c_id]);
	if (ret)
		goto error_deinit_tx;

	return 0;

error_deinit_tx:
	capi_dma_deinit_chan(dma_channel_tx[i2c_id]);
error_cleanup:
	if (tx_buffer != async->data_buf)
		capi_free(tx_buffer);
	async_transfer_in_progress[i2c_id] = false;

	return ret;
}

/**
 * @brief Perform I2C receive using DMA
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_receive_dma(struct max_capi_i2c_priv *i2c_priv,
				     mxc_i3c_regs_t *i3c,
				     struct max_capi_i2c_async_state *async)
{
	int ret;
	struct capi_dma_handle *dma_handle = i2c_priv->dma_handle;
	uint8_t i2c_id = i2c_priv->identifier;
	struct max_capi_dma_xfer_extra dma_rx_extra;
	struct capi_dma_transfer dma_rx_xfer;
	uint16_t i;
	bool is_last;
	uint32_t timeout = MAX_CAPI_I2C_DMA_TIMEOUT;

	if (async_transfer_in_progress[i2c_id])
		return -EBUSY;

	async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_DATA;

	if (async->subaddr_buf) {
		ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
					async->target_addr, 0);
		if (ret != E_SUCCESS) {
			async_transfer_in_progress[i2c_id] = false;
			return -EIO;
		}

		for (i = 0; i < async->subaddr_len; i++) {
			is_last = (i == (async->subaddr_len - 1));
			ret = MXC_I3C_WriteTXFIFO(i3c, &async->subaddr_buf[i],
						  1, is_last, 0);
			if (ret <= 0) {
				async_transfer_in_progress[i2c_id] = false;
				return -EIO;
			}
		}
		while (!(MXC_I3C_Controller_GetFlags(i3c) & MXC_F_I3C_CONT_INTFL_DONE)) {
			if (--timeout == 0)
				return -ETIMEDOUT;
		}
		MXC_I3C_Controller_ClearFlags(i3c, MXC_F_I3C_CONT_INTFL_DONE);
	}

	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_READ,
				async->target_addr, async->data_len);
	if (ret != E_SUCCESS) {
		async_transfer_in_progress[i2c_id] = false;
		return -EIO;
	}

	i3c->cont_dmactrl = MXC_S_I3C_CONT_DMACTRL_RX_EN_EN;

	ret = capi_dma_init_chan(dma_handle, &dma_channel_rx[i2c_id], 0);
	if (ret)
		goto error_cleanup;

	dma_rx_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_I3C_RX_CONTROLLER,
	};

	dma_rx_xfer = (struct capi_dma_transfer) {
		.src = &i3c->cont_rxfifo8,
		.dst = async->data_buf,
		.src_inc = CAPI_DMA_NO_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.length = async->data_len,
		.extra = &dma_rx_extra,
		.xfer_type = CAPI_DMA_DEV_TO_MEM,
		.user_data = i2c_priv,
	};

	ret = capi_dma_config_xfer(dma_channel_rx[i2c_id], &dma_rx_xfer);
	if (ret)
		goto error_deinit_rx;

	ret = capi_dma_register_complete_callback(dma_channel_rx[i2c_id],
			_max_capi_i2c_dma_complete_callback,
			i2c_priv);
	if (ret)
		goto error_deinit_rx;

	dma_completed[i2c_id] = false;

	ret = capi_dma_xfer_start(dma_channel_rx[i2c_id]);
	if (ret)
		goto error_deinit_rx;

	return 0;

error_deinit_rx:
	capi_dma_deinit_chan(dma_channel_rx[i2c_id]);
error_cleanup:
	async_transfer_in_progress[i2c_id] = false;

	return ret;
}

/**
 * @brief Handle controller mode interrupts
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral
 */
static void _max_capi_i2c_controller_isr(struct max_capi_i2c_priv *i2c_priv,
		mxc_i3c_regs_t *i3c)
{
	uint32_t flags;
	int ret, result;
	bool operation_complete;
	capi_i2c_callback callback;
	void *callback_arg;

	if (!i2c_priv->async || i2c_priv->async->state == MAX_CAPI_I2C_ASYNC_STATE_IDLE)
		return;

	operation_complete = false;
	flags = MXC_I3C_Controller_GetFlags(i3c);

	if (flags & MXC_F_I3C_CONT_INTFL_ERRWARN) {
		uint32_t errwarn = i3c->cont_errwarn;
		i3c->cont_errwarn = errwarn;

		i2c_priv->async->result = -EIO;
		i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_ERROR;

		if (i2c_priv->async->send_stop)
			MXC_I3C_EmitI2CStop(i3c);

		operation_complete = true;
		goto invoke_callback;
	}

	switch (i2c_priv->async->state) {
	case MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR:
		if (flags & MXC_F_I3C_CONT_INTFL_TX_NFULL) {
			_max_capi_i2c_write_tx_fifo(i3c,
						    i2c_priv->async->subaddr_buf,
						    i2c_priv->async->subaddr_len,
						    &i2c_priv->async->subaddr_idx,
						    false);

			if (i2c_priv->async->subaddr_idx >= i2c_priv->async->subaddr_len) {
				if (i2c_priv->async->is_transmit) {
					i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;
				} else {
					ret = MXC_I3C_EmitStart(i3c, true,
								MXC_I3C_TRANSFER_TYPE_READ,
								i2c_priv->async->target_addr,
								i2c_priv->async->data_len);
					if (ret != E_SUCCESS) {
						i2c_priv->async->result = -EIO;
						i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_ERROR;
						operation_complete = true;
						break;
					}

					MXC_I3C_Controller_DisableInt(i3c, MXC_F_I3C_CONT_INTEN_TX_NFULL);
					MXC_I3C_Controller_EnableInt(i3c, MXC_F_I3C_CONT_INTEN_RX_RDY);

					i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_DATA;
				}
			}
		}
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_TX_DATA:
		if (flags & MXC_F_I3C_CONT_INTFL_TX_NFULL) {
			_max_capi_i2c_write_tx_fifo(i3c,
						    i2c_priv->async->data_buf,
						    i2c_priv->async->data_len,
						    &i2c_priv->async->data_idx,
						    true);

			if (i2c_priv->async->data_idx >= i2c_priv->async->data_len) {
				i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_WAIT_DONE;
				MXC_I3C_Controller_DisableInt(i3c, MXC_F_I3C_CONT_INTEN_TX_NFULL);
			}
		}
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_RX_DATA:
		if (flags & MXC_F_I3C_CONT_INTFL_RX_RDY) {
			_max_capi_i2c_read_rx_fifo(i3c,
						   i2c_priv->async->data_buf,
						   i2c_priv->async->data_len,
						   &i2c_priv->async->data_idx);

			if (i2c_priv->async->data_idx >= i2c_priv->async->data_len) {
				i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_WAIT_DONE;
				MXC_I3C_Controller_DisableInt(i3c, MXC_F_I3C_CONT_INTEN_RX_RDY);
			}
		}
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_WAIT_DONE:
		if (flags & MXC_F_I3C_CONT_INTFL_DONE) {
			i2c_priv->async->result = 0;
			operation_complete = true;

			if (i2c_priv->async->send_stop)
				MXC_I3C_EmitI2CStop(i3c);
		}
		break;

	default:
		break;
	}

invoke_callback:
	if (operation_complete) {
		MXC_I3C_Controller_DisableInt(i3c, 0xFFFFFFFF);
		MXC_I3C_Controller_ClearFlags(i3c, 0xFFFFFFFF);

		result = i2c_priv->async->result;
		callback = i2c_priv->callback;
		callback_arg = i2c_priv->callback_arg;

		_max_capi_i2c_reset_async_state(i2c_priv);

		if (callback) {
			callback(CAPI_I2C_XFR_DONE, callback_arg, result);
		}
	}
}

/**
 * @brief Setup before an async transmit/receive function
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
int _max_capi_i2c_setup_async(struct capi_i2c_device *device,
			      struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	i2c_priv = device->controller->priv;
	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	/* Allocate async state on first use */
	if (!i2c_priv->async) {
		i2c_priv->async = capi_calloc(1, sizeof(*i2c_priv->async));
		if (!i2c_priv->async)
			return -ENOMEM;
	}

	if (i2c_priv->target && i2c_priv->target->enabled)
		return -ENOTSUP;

	if (i2c_priv->async->state != MAX_CAPI_I2C_ASYNC_STATE_IDLE)
		return -EBUSY;

	i2c_priv->async->target_addr = transfer->target_addr;
	i2c_priv->async->send_stop = !transfer->no_stop;
	i2c_priv->async->result = 0;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		i2c_priv->async->subaddr_buf = transfer->sub_address;
		i2c_priv->async->subaddr_len = transfer->sub_address_len;
		i2c_priv->async->subaddr_idx = 0;
	} else {
		i2c_priv->async->subaddr_buf = NULL;
		i2c_priv->async->subaddr_len = 0;
	}

	i2c_priv->async->data_buf = transfer->buf;
	i2c_priv->async->data_len = transfer->len;
	i2c_priv->async->data_idx = 0;

	MXC_I3C_ClearRXFIFO(i3c);
	MXC_I3C_ClearTXFIFO(i3c);

	ret = MXC_I3C_SetRXTXThreshold(i3c, MXC_I3C_RX_TH_NOT_EMPTY,
				       MXC_I3C_TX_TH_HALF_FULL);
	if (ret != E_SUCCESS) {
		_max_capi_i2c_reset_async_state(i2c_priv);
		return -EIO;
	}

	return 0;
}

/** I2C functions implementation **********************************************/

/**
 * @brief Initialize the I3C peripheral in I2C mode
 * @param handle The I2C handle
 * @param config The I2C initialization config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_init(struct capi_i2c_controller_handle **handle,
		      const struct capi_i2c_config *config)
{
	int ret;
	struct capi_i2c_controller_handle *i2c_handle;
	struct max_capi_i2c_priv *i2c_priv;
	struct max_capi_i2c_extra *i2c_extra;
	uint8_t i2c_id;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	if (config->identifier >= MXC_CFG_I3C_INSTANCES)
		return -EINVAL;

	if (i2c[config->identifier] != NULL) {
		*handle = i2c[config->identifier];
		return 0;
	}

	if (*handle == NULL) {
		i2c_handle = capi_calloc(1, sizeof(*i2c_handle));
		if (!i2c_handle)
			return -ENOMEM;
		i2c_handle->init_allocated = true;
	} else {
		i2c_handle = *handle;
		i2c_handle->init_allocated = false;
	}

	i2c_priv = capi_calloc(1, sizeof(*i2c_priv));
	if (!i2c_priv) {
		ret = -ENOMEM;
		goto free_handle;
	}

	i2c_handle->ops = config->ops;
	i2c_extra = config->extra;
	i2c_priv->identifier = config->identifier;
	i2c_priv->extra = i2c_extra;
	i2c_handle->priv = i2c_priv;
	i2c_id = i2c_priv->identifier;

	if (config->clk_freq_hz <= MAX_CAPI_I2C_SPEED_STANDARD) {
		i2c_priv->freq = MAX_CAPI_I2C_SPEED_STANDARD;
	} else if (config->clk_freq_hz <= MAX_CAPI_I2C_SPEED_FAST) {
		i2c_priv->freq = MAX_CAPI_I2C_SPEED_FAST;
	} else if (config->clk_freq_hz <= MAX_CAPI_I2C_SPEED_FAST_PLUS) {
		i2c_priv->freq = MAX_CAPI_I2C_SPEED_FAST_PLUS;
	} else {
		ret = -EINVAL;
		goto free_priv;
	}

	ret = _max_capi_i2c_configure_hardware(i2c_priv, !config->initiator,
					       config->initiator ? i2c_priv->freq : config->address);
	if (ret)
		goto shutdown_i2c;

	if (config->dma_handle && i2c_extra->dma_config) {
		i2c_priv->dma_handle = config->dma_handle;

		ret = capi_dma_init(&i2c_priv->dma_handle,
				    i2c_extra->dma_config);
		if (ret)
			goto shutdown_i2c;
	}

	struct capi_irq_config irq_config = {
		.irq_ctrl_id = 0,
	};
	ret = capi_irq_init(&irq_config);
	if (ret)
		goto shutdown_i2c;

	IRQn_Type i2c_irq = MXC_I3C_GET_IRQ(i2c_id);
	ret = capi_irq_connect(i2c_irq, max_capi_i2c_isr, i2c_handle);
	if (ret)
		goto shutdown_i2c;

	ret = capi_irq_enable(i2c_irq);
	if (ret)
		goto disconnect_irq;

	i2c[config->identifier] = i2c_handle;
	*handle = i2c_handle;
	i2c_priv->i2c_handle = i2c_handle;

	return 0;

disconnect_irq:
	capi_irq_connect(MXC_I3C_GET_IRQ(i2c_id), NULL, NULL);
shutdown_i2c:
	MXC_I3C_Shutdown(MXC_I3C_GET_I3C(i2c_id));
	if (i2c_priv->target) {
		capi_free(i2c_priv->target);
		i2c_priv->target = NULL;
	}
free_priv:
	capi_free(i2c_priv);
free_handle:
	if (i2c_handle->init_allocated)
		capi_free(i2c_handle);

	i2c[config->identifier] = NULL;

	return ret;
}

/**
 * @brief Deinitialize the I3C peripheral
 * @param handle The I2C handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	int ret;
	struct max_capi_i2c_priv *i2c_priv;
	uint8_t i2c_id;
	mxc_i3c_regs_t *i3c;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;
	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	MXC_I3C_Controller_DisableInt(i3c, 0xFFFFFFFF);
	MXC_I3C_Controller_ClearFlags(i3c, 0xFFFFFFFF);
	MXC_I3C_Target_DisableInt(i3c, 0xFFFFFFFF);
	MXC_I3C_Target_ClearFlags(i3c, 0xFFFFFFFF);

	if (i2c_priv->async) {
		if (i2c_priv->async->state != MAX_CAPI_I2C_ASYNC_STATE_IDLE) {
			if (dma_channel_rx[i2c_id])
				capi_dma_xfer_abort(dma_channel_rx[i2c_id]);
			if (dma_channel_tx[i2c_id])
				capi_dma_xfer_abort(dma_channel_tx[i2c_id]);

			uint32_t timeout = MAX_CAPI_I2C_DMA_CALLBACK_CLEANUP_TIMEOUT;
			while (i2c_priv->callback_active && --timeout);

			if (!timeout)
				i2c_priv->callback_active = false;

			MXC_I3C_EmitI2CStop(i3c);
		}

		capi_free(i2c_priv->async);
		i2c_priv->async = NULL;
	}

	if (i2c_priv->dma_handle) {
		_max_capi_i2c_dma_cleanup_channel(&dma_channel_rx[i2c_id]);
		_max_capi_i2c_dma_cleanup_channel(&dma_channel_tx[i2c_id]);
		dma_completed[i2c_id] = false;
		async_transfer_in_progress[i2c_id] = false;
	}

	capi_irq_disable(MXC_I3C_GET_IRQ(i2c_id));
	capi_irq_connect(MXC_I3C_GET_IRQ(i2c_id), NULL, NULL);

	ret = MXC_I3C_Shutdown(i3c);

	if (i2c_priv->target) {
		capi_free(i2c_priv->target);
		i2c_priv->target = NULL;
	}

	capi_free(i2c_priv);
	if (handle->init_allocated)
		capi_free(handle);

	i2c[i2c_id] = NULL;

	if (ret != E_NO_ERROR)
		return (ret == E_TIME_OUT ? -ETIMEDOUT : -EIO);

	return 0;
}

/**
 * @brief Transmit function
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_transmit(struct capi_i2c_device *device,
			  struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;
	uint8_t i2c_id;

	if (!device || !device->controller || !device->controller->priv || !transfer
	    || !transfer->buf)
		return -EINVAL;

	i2c_priv = device->controller->priv;
	i2c_id = i2c_priv->identifier;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - set TX data*/
		_max_capi_i2c_target_set_tx_data(i2c_priv, transfer->buf, transfer->len);

		return 0;
	} else {
		/* Controller mode - do the transmit */
		uint8_t *combined_buf = NULL;

		if (transfer->sub_address_len > 0 && !transfer->sub_address)
			return -EINVAL;

		mxc_i3c_req_t i2c_req = (mxc_i3c_req_t) {
			.is_i2c = true,
			.rx_buf = NULL,
			.rx_len = 0,
			.stop = !transfer->no_stop,
			.target_addr = transfer->target_addr ? transfer->target_addr : device->address,
		};

		/* If sub_address is filled, concatenate with data buffer */
		if (transfer->sub_address && transfer->sub_address_len > 0) {
			combined_buf = capi_malloc(transfer->sub_address_len + transfer->len);
			if (!combined_buf)
				return -ENOMEM;

			memcpy(combined_buf, transfer->sub_address, transfer->sub_address_len);
			memcpy(combined_buf + transfer->sub_address_len, transfer->buf, transfer->len);

			i2c_req.tx_buf = combined_buf;
			i2c_req.tx_len = transfer->sub_address_len + transfer->len;
		} else {
			i2c_req.tx_buf = transfer->buf;
			i2c_req.tx_len = transfer->len;
		}

		int ret = MXC_I3C_Controller_Transaction(MXC_I3C_GET_I3C(i2c_id), &i2c_req);

		if (combined_buf)
			capi_free(combined_buf);

		if (ret)
			return (ret == E_TIME_OUT ? -ETIMEDOUT : -EIO);

		return 0;
	}
}


/**
 * @brief Receive function
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_receive(struct capi_i2c_device *device,
			 struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_req_t i2c_req;
	uint8_t i2c_id;

	if (!device || !device->controller || !device->controller->priv || !transfer
	    || !transfer->buf)
		return -EINVAL;

	i2c_priv = device->controller->priv;
	i2c_id = i2c_priv->identifier;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - get RX data */
		uint8_t *rx_data;
		uint16_t rx_len;

		_max_capi_i2c_target_get_rx_data(i2c_priv, &rx_data, &rx_len);

		if (rx_len > transfer->len)
			return -EOVERFLOW;

		memcpy(transfer->buf, rx_data, rx_len);
		transfer->len = rx_len;

		return 0;
	} else {
		if (transfer->sub_address_len > 0 && !transfer->sub_address)
			return -EINVAL;

		i2c_req = (mxc_i3c_req_t) {
			.is_i2c = true,
			.rx_buf = transfer->buf,
			.rx_len = transfer->len,
			.stop = !transfer->no_stop,
			.target_addr = transfer->target_addr ? transfer->target_addr : device->address,
		};

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			i2c_req.tx_buf = transfer->sub_address;
			i2c_req.tx_len = transfer->sub_address_len;
		}

		int ret =  MXC_I3C_Controller_Transaction(MXC_I3C_GET_I3C(i2c_id), &i2c_req);
		if (ret != E_NO_ERROR)
			return (ret == E_TIME_OUT ? -ETIMEDOUT : -EIO);

		return 0;
	}
}

/**
 * @brief Register a callback
 * @param handle The I2C controller handle
 * @param callback The callback function
 * @param callback_arg Argument for the callback function
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_register_callback(struct capi_i2c_controller_handle *handle,
				   capi_i2c_callback const callback,
				   void *const callback_arg)
{
	struct max_capi_i2c_priv *i2c_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;
	i2c_priv->callback = callback;
	i2c_priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Configures the I2C bus speed
 * @param handle The I2C controller handle
 * @param speed The clock speed
 * @param duty_cycle Ignored - automatically determined by hardware
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle *handle,
				     enum capi_i2c_speed speed,
				     uint8_t duty_cycle)
{
	int ret;
	uint32_t pp_freq, od_freq, i2c_freq;
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;
	i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);

	switch (speed) {
	case CAPI_I2C_SPEED_STANDARD:
		i2c_freq = MAX_CAPI_I2C_SPEED_STANDARD;
		break;
	case CAPI_I2C_SPEED_FAST:
		i2c_freq = MAX_CAPI_I2C_SPEED_FAST;
		break;
	case CAPI_I2C_SPEED_FAST_PLUS:
		i2c_freq = MAX_CAPI_I2C_SPEED_FAST_PLUS;
		break;
	default:
		return -EINVAL;
	}

	ret = _max_capi_i2c_get_frequencies(i2c_freq, &pp_freq, &od_freq);
	if (ret)
		return ret;

	i2c_priv->freq = i2c_freq;
	ret = MXC_I3C_SetPPFrequency(i3c, pp_freq);
	if (ret < 0)
		return -EINVAL;
	ret = MXC_I3C_SetODFrequency(i3c, od_freq, false);
	if (ret < 0)
		return -EINVAL;
	ret = MXC_I3C_SetI2CFrequency(i3c, i2c_freq);
	if (ret < 0)
		return -EINVAL;

	return 0;
}

/**
 * @brief Async transmit function
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_transmit_async(struct capi_i2c_device *device,
				struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	i2c_priv = device->controller->priv;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - set TX data */
		_max_capi_i2c_target_set_tx_data(i2c_priv, transfer->buf, transfer->len);

		return 0;
	} else {
		/* Controller mode - do the transmit async */
		int ret = _max_capi_i2c_setup_async(device, transfer);
		if (ret)
			return ret;

		mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);

		if (i2c_priv->dma_handle)
			ret = _max_capi_i2c_transmit_dma(i2c_priv, i3c, i2c_priv->async);
		else
			ret = _max_capi_i2c_transmit_fifo(i3c, i2c_priv->async);

		if (ret)
			_max_capi_i2c_reset_async_state(i2c_priv);

		return ret;
	}
}

/**
 * @brief Async receive function
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_receive_async(struct capi_i2c_device *device,
			       struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	i2c_priv = device->controller->priv;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - get RX data */
		uint8_t *rx_data;
		uint16_t rx_len;

		_max_capi_i2c_target_get_rx_data(i2c_priv, &rx_data, &rx_len);

		if (rx_len > transfer->len)
			return -EOVERFLOW;

		memcpy(transfer->buf, rx_data, rx_len);
		transfer->len = rx_len;

		return 0;
	} else {
		/* Controller mode - do the receive async */
		int ret = _max_capi_i2c_setup_async(device, transfer);
		if (ret)
			return ret;

		mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);

		if (i2c_priv->dma_handle)
			ret = _max_capi_i2c_receive_dma(i2c_priv, i3c, i2c_priv->async);
		else
			ret = _max_capi_i2c_receive_fifo(i3c, i2c_priv->async);

		if (ret)
			_max_capi_i2c_reset_async_state(i2c_priv);

		return ret;
	}
}

/**
 * @brief Attempt to recover the I2C bus
 * @param handle The I2C controller handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_recover_bus(struct capi_i2c_controller_handle *handle)
{
	const struct max_capi_i2c_priv *i2c_priv;
	uint8_t i2c_id;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;
	i2c_id = i2c_priv->identifier;

	ret = MXC_I3C_Recover(MXC_I3C_GET_I3C(i2c_id));
	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Register the I2C controller in target mode
 * @param handle The I2C controller handle
 * @param addr Valid 7-bit or 10-bit I2C address
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_register_target(struct capi_i2c_controller_handle *handle,
				 uint16_t addr)
{
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	/* Validate address */
	if (addr > 0x3FF)
		return -EINVAL;
	if (addr <= 0x7F) {
		if (addr <= 0x07 || addr >= 0x78)
			return -EINVAL;
	}

	i2c_priv = handle->priv;
	i2c_id = i2c_priv->identifier;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled)
		return -EBUSY;

	ret = _max_capi_i2c_configure_hardware(i2c_priv, true, addr);
	if (ret)
		return ret;

	if (!i2c_priv->target)
		return -ENOMEM;

	i2c_priv->target->enabled = true;
	i2c_priv->target->address = addr;
	i2c_priv->target->b10addr = (addr > 0x7F);
	i2c_priv->target->state = MAX_CAPI_I2C_TARGET_STATE_IDLE;
	i2c_priv->target->rx_count = 0;
	i2c_priv->target->tx_count = 0;
	i2c_priv->target->tx_buffer = NULL;
	i2c_priv->target->tx_length = 0;

	i3c = MXC_I3C_GET_I3C(i2c_id);
	MXC_I3C_Target_EnableInt(i3c, MXC_F_I3C_TARG_INTEN_START |
				 MXC_F_I3C_TARG_INTEN_ADDRMATCH |
				 MXC_F_I3C_TARG_INTEN_STOP |
				 MXC_F_I3C_TARG_INTEN_RX_RDY |
				 MXC_F_I3C_TARG_INTEN_TX_NFULL |
				 MXC_F_I3C_TARG_INTEN_ERRWARN);

	return 0;
}

/**
 * @brief Unregister the I2C controller from target mode
 * @param handle The I2C controller handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_i2c_unregister_target(struct capi_i2c_controller_handle *handle)
{
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;

	if (i2c_priv->target == NULL || !i2c_priv->target->enabled)
		return -EINVAL;

	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	MXC_I3C_Target_DisableInt(i3c, MXC_F_I3C_TARG_INTCLR_START |
				  MXC_F_I3C_TARG_INTCLR_ADDRMATCH |
				  MXC_F_I3C_TARG_INTCLR_STOP |
				  MXC_F_I3C_TARG_INTCLR_RX_RDY |
				  MXC_F_I3C_TARG_INTCLR_TX_NFULL |
				  MXC_F_I3C_TARG_INTCLR_ERRWARN);

	i2c_priv->target->enabled = false;

	ret = _max_capi_i2c_configure_hardware(i2c_priv, false, i2c_priv->freq);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief The function called during an interrupt
 * @param handle The I2C handle
 */
void max_capi_i2c_isr(void *handle)
{
	struct capi_i2c_controller_handle *i2c_handle;
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;

	if (!handle)
		return;

	i2c_handle = (struct capi_i2c_controller_handle *)handle;
	if (!i2c_handle->priv)
		return;

	i2c_priv = i2c_handle->priv;
	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	if (i2c_priv->target && (MXC_I3C_Target_GetFlags(i3c) != 0)) {
		_max_capi_i2c_target_isr(i2c_priv, i3c);
	} else if (MXC_I3C_Controller_GetFlags(i3c) != 0) {
		_max_capi_i2c_controller_isr(i2c_priv, i3c);
	} else {
		uint32_t target_flags = MXC_I3C_Target_GetFlags(i3c);
		uint32_t controller_flags = MXC_I3C_Controller_GetFlags(i3c);

		if (target_flags)
			MXC_I3C_Target_ClearFlags(i3c, target_flags);
		if (controller_flags)
			MXC_I3C_Controller_ClearFlags(i3c, controller_flags);
	}
}

const struct capi_i2c_ops max_capi_i2c_ops = {
	.init = max_capi_i2c_init,
	.deinit = max_capi_i2c_deinit,
	.transmit = max_capi_i2c_transmit,
	.receive = max_capi_i2c_receive,
	.register_callback = max_capi_i2c_register_callback,
	.configure_bus_speed = max_capi_i2c_configure_bus_speed,
	.transmit_async = max_capi_i2c_transmit_async,
	.receive_async = max_capi_i2c_receive_async,
	.recover_bus = max_capi_i2c_recover_bus,
	.register_target = max_capi_i2c_register_target,
	.unregister_target = max_capi_i2c_unregister_target,
	.isr = max_capi_i2c_isr,
};

/** Platform-specific functions ***********************************************/

/**
 * @brief I3C interrupt handler
 */
void I3C_IRQHandler(void)
{
	MXC_I3C_IRQHandler(MXC_I3C);
}
