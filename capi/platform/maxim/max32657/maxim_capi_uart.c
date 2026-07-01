/*******************************************************************************
 *   @file   maxim_capi_uart.c
 *   @brief  Implementation of UART functions with CAPI.
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
#include <stdlib.h>
#include <stdio.h>
#include "capi_dma.h"
#include "capi_irq.h"
#include "capi_uart.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_uart.h"
#include <sys/stat.h>

/** Static declarations *******************************************************/

static struct capi_uart_handle *uart[MXC_UART_INSTANCES] = {NULL};
static int8_t stdio_index = -1;
static bool async_transfer_in_progress[MXC_UART_INSTANCES] = {false};
static volatile bool dma_completed[MXC_UART_INSTANCES] = {false};
static struct capi_dma_chan *dma_channel_tx[MXC_UART_INSTANCES] = {NULL};
static struct capi_dma_chan *dma_channel_rx[MXC_UART_INSTANCES] = {NULL};

/** Forward declarations ******************************************************/

void max_capi_uart_isr(void *handle);

/** Helper functions **********************************************************/

/**
 * @brief Configure the VDDIO for the UART pins.
 * @param vssel - the voltage level of the interface.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t _max_uart_pins_config(enum max_capi_gpio_vssel vssel)
{
	mxc_gpio_cfg_t uart_pins = gpio_cfg_uart;
	int ret;

	uart_pins = gpio_cfg_uart;

	uart_pins.vssel = (mxc_gpio_vssel_t)vssel;
	ret = MXC_GPIO_Config(&uart_pins);

	return ret;
}

/**
 * @brief Release a DMA channel
 * @param channel Pointer to a channel pointer
 */
static void _max_capi_uart_dma_cleanup_channel(struct capi_dma_chan **channel)
{
	if (channel && *channel) {
		capi_dma_xfer_abort(*channel);
		capi_dma_deinit_chan(*channel);
		*channel = NULL;
	}
}

/**
 * @brief MSDK completion callback function
 * @param req The UART
 * @param result Result
 */
static void _max_capi_uart_msdk_callback(mxc_uart_req_t *req, int result)
{
	uint32_t id = MXC_UART_GET_IDX(req->uart);
	struct capi_uart_handle *handle = uart[id];
	struct max_capi_uart_priv *uart_priv;
	enum capi_uart_async_event event;
	bool is_tx;

	if (!handle || !handle->priv)
		return;

	uart_priv = handle->priv;
	is_tx = (req->txData != NULL);

	if (result == E_NO_ERROR)
		event = is_tx ? CAPI_UART_EVENT_TX_DONE : CAPI_UART_EVENT_RX_DONE;
	else if (result == E_ABORT && is_tx)
		event = CAPI_UART_EVENT_TX_ABORTED;
	else
		event = CAPI_UART_EVENT_INTERRUPT;

	async_transfer_in_progress[id] = false;

	if (uart_priv->callback)
		uart_priv->callback(event, uart_priv->callback_arg, result);
}

/**
 * @brief DMA transfer complete callback function
 * @param transfer The DMA transfer
 * @param ctx The private struct passed into the callback
 */
static void _max_capi_uart_dma_complete(struct capi_dma_transfer *transfer,
					void *ctx)
{
	struct max_capi_uart_priv *uart_priv = (struct max_capi_uart_priv *)ctx;
	uint32_t id;
	bool is_tx;
	enum capi_uart_async_event event;
	capi_uart_callback callback;
	void *callback_arg;

	if (!uart_priv)
		return;

	id = uart_priv->id;
	is_tx = (transfer->xfer_type == CAPI_DMA_MEM_TO_DEV);
	event = is_tx ? CAPI_UART_EVENT_TX_DONE : CAPI_UART_EVENT_RX_DONE;

	callback = uart_priv->callback;
	callback_arg = uart_priv->callback_arg;

	dma_completed[id] = true;

	uart_priv->uart->dma &= ~(MXC_F_UART_DMA_TX_EN | MXC_F_UART_DMA_RX_EN);
	_max_capi_uart_dma_cleanup_channel(&dma_channel_rx[id]);
	_max_capi_uart_dma_cleanup_channel(&dma_channel_tx[id]);

	async_transfer_in_progress[id] = false;

	if (callback)
		callback(event, callback_arg, 0);
}

/**
 * @brief Receive with DMA
 * @param priv The private structure
 * @param buf The buffer to store the data to
 * @param len The length of the data
 */
static int _max_capi_uart_receive_dma(struct max_capi_uart_priv *priv,
				      uint8_t *buf, uint32_t len)
{
	int ret;

	ret = capi_dma_init_chan(priv->dma_handle, &dma_channel_rx[priv->id], 0);
	if (ret)
		goto error;

	priv->dma_xfer_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_UART_RX,
	};
	priv->dma_xfer = (struct capi_dma_transfer) {
		.src = &priv->uart->fifo,
		.dst = buf,
		.src_inc = CAPI_DMA_NO_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.length = len,
		.xfer_type = CAPI_DMA_DEV_TO_MEM,
		.user_data = priv,
	};

	priv->uart->dma |= MXC_F_UART_DMA_RX_EN;

	ret = capi_dma_config_xfer(dma_channel_rx[priv->id], &priv->dma_xfer);
	if (ret)
		goto error_deinit;

	ret = capi_dma_register_complete_callback(dma_channel_rx[priv->id],
			_max_capi_uart_dma_complete,
			priv);
	if (ret)
		goto error_deinit;

	dma_completed[priv->id] = false;
	async_transfer_in_progress[priv->id] = true;

	ret = capi_dma_xfer_start(dma_channel_rx[priv->id]);
	if (ret)
		goto error_deinit;

	return 0;

error_deinit:
	priv->uart->dma &= ~MXC_F_UART_DMA_RX_EN;
	_max_capi_uart_dma_cleanup_channel(&dma_channel_rx[priv->id]);
error:
	async_transfer_in_progress[priv->id] = false;

	return ret;
}

/**
 * @brief Transmit with DMA
 * @brief priv The private structure
 * @param buf The buffer to get data from
 * @param len The length of the data
 */
static int _max_capi_uart_transmit_dma(struct max_capi_uart_priv *priv,
				       uint8_t *buf, uint32_t len)
{
	int ret;

	ret = capi_dma_init_chan(priv->dma_handle, &dma_channel_tx[priv->id], 0);
	if (ret)
		goto error;

	priv->dma_xfer_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_UART_TX,
	};
	priv->dma_xfer = (struct capi_dma_transfer) {
		.src = buf,
		.dst = &priv->uart->fifo,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_NO_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.length = len,
		.xfer_type = CAPI_DMA_MEM_TO_DEV,
		.user_data = priv,
	};

	priv->uart->dma |= MXC_F_UART_DMA_TX_EN;

	ret = capi_dma_config_xfer(dma_channel_tx[priv->id], &priv->dma_xfer);
	if (ret)
		goto error_deinit;

	ret = capi_dma_register_complete_callback(dma_channel_rx[priv->id],
			_max_capi_uart_dma_complete,
			priv);
	if (ret)
		goto error_deinit;

	dma_completed[priv->id] = false;
	async_transfer_in_progress[priv->id] = true;

	ret = capi_dma_xfer_start(dma_channel_tx[priv->id]);
	if (ret)
		goto error_deinit;

	return 0;

error_deinit:
	priv->uart->dma &= ~MXC_F_UART_DMA_TX_EN;
	_max_capi_uart_dma_cleanup_channel(&dma_channel_tx[priv->id]);
error:
	async_transfer_in_progress[priv->id] = false;

	return ret;
}

/**
 * @brief Converts CAPI enum values to msdk values for line config
 * @param line_config The line config
 * @param[out] size Number of data bits
 * @param[out] parity Parity
 * @param[out] stop Stop bits
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_uart_map_line_config(const struct capi_uart_line_config
		*line_config,
		uint32_t *size,
		mxc_uart_parity_t *parity,
		mxc_uart_stop_t *stop)
{
	switch (line_config->size) {
	case CAPI_UART_DATA_BITS_5:
		*size = 5;
		break;
	case CAPI_UART_DATA_BITS_6:
		*size = 6;
		break;
	case CAPI_UART_DATA_BITS_7:
		*size = 7;
		break;
	case CAPI_UART_DATA_BITS_8:
		*size = 8;
		break;
	default:
		return -EINVAL;
	}

	switch (line_config->parity) {
	case CAPI_UART_PARITY_NONE:
		*parity = MXC_UART_PARITY_DISABLE;
		break;
	case CAPI_UART_PARITY_ODD:
		*parity = MXC_UART_PARITY_ODD_1;
		break;
	case CAPI_UART_PARITY_EVEN:
		*parity = MXC_UART_PARITY_EVEN_1;
		break;
	case CAPI_UART_PARITY_MARK:
	case CAPI_UART_PARITY_SPACE:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	switch (line_config->stop_bits) {
	case CAPI_UART_STOP_1_BIT:
		*stop = MXC_UART_STOP_1;
		break;
	case CAPI_UART_STOP_2_BIT:
		*stop = MXC_UART_STOP_2;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/** UART implementation *******************************************************/

/**
 * @brief Initialize the UART peripheral
 * @param handle Pointer to a UART handle pointer
 * @param config UART config struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_init(struct capi_uart_handle **handle,
		       const struct capi_uart_config *config)
{
	int ret;

	struct capi_uart_handle *uart_handle;
	struct max_capi_uart_priv *uart_priv;
	struct max_capi_uart_extra *uart_extra;
	mxc_uart_clock_t clk_src;
	uint32_t baud_rate, size;
	mxc_uart_parity_t parity;
	mxc_uart_stop_t stop;
	struct capi_uart_line_config line_cfg;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	if (config->identifier >= MXC_UART_INSTANCES)
		return -EINVAL;

	if (uart[config->identifier] != NULL) {
		*handle = uart[config->identifier];
		return 0;
	}

	if (*handle == NULL) {
		uart_handle = capi_calloc(1, sizeof(*uart_handle));
		if (!uart_handle)
			return -ENOMEM;
		uart_handle->init_allocated = true;
	} else {
		uart_handle = *handle;
		uart_handle->init_allocated = false;
	}

	uart_priv = capi_calloc(1, sizeof(*uart_priv));
	if (!uart_priv) {
		ret = -ENOMEM;
		goto free_handle;
	}

	uart_handle->priv = uart_priv;
	uart_handle->ops = config->ops;

	uart_extra = config->extra;

	uart_priv->uart = MXC_UART_GET_UART(config->identifier);
	uart_priv->id = config->identifier;

	if (config->clk_freq_hz == 0) {
		/** Default */
		clk_src = MXC_UART_APB_CLK;
	} else if (config->clk_freq_hz == PeripheralClock) {
		/** APB clock */
		clk_src = MXC_UART_APB_CLK;
	} else if (config->clk_freq_hz == IBRO_FREQ) {
		/** IBRO = 7.3728 MHz */
		clk_src = MXC_UART_IBRO_CLK;
	} else {
		ret = -ENOTSUP;
		goto free_priv;
	}
	uart_priv->clk_src = clk_src;

	baud_rate = MAX_CAPI_UART_DEFAULT_BAUD;
	if (config->line_config) {
		line_cfg = *config->line_config;
		baud_rate = config->line_config->baudrate;
	} else {
		line_cfg = (struct capi_uart_line_config) {
			.baudrate = MAX_CAPI_UART_DEFAULT_BAUD,
			.size = CAPI_UART_DATA_BITS_8,
			.parity = CAPI_UART_PARITY_NONE,
			.stop_bits = CAPI_UART_STOP_1_BIT,
		};
	}

	ret = _max_capi_uart_map_line_config(&line_cfg, &size, &parity, &stop);
	if (ret)
		goto free_priv;

	uart_priv->line_config = line_cfg;

	ret = MXC_UART_Init(uart_priv->uart, baud_rate, clk_src);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_priv;
	}

	ret = _max_uart_pins_config(uart_extra->vssel);
	if (ret)
		goto free_priv;

	ret = MXC_UART_SetDataSize(uart_priv->uart, size);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_priv;
	}

	ret = MXC_UART_SetParity(uart_priv->uart, parity);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_priv;
	}

	ret = MXC_UART_SetStopBits(uart_priv->uart, stop);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_priv;
	}

	if (config->dma_handle && uart_extra->dma_config) {
		uart_priv->dma_handle = config->dma_handle;
		ret = capi_dma_init(&uart_priv->dma_handle,
				    uart_extra->dma_config);
		if (ret)
			goto shutdown_uart;
	}

	struct capi_irq_config irq_config = {
		.irq_ctrl_id = 0,
	};
	ret = capi_irq_init(&irq_config);
	if (ret)
		goto cleanup_channels;

	IRQn_Type irq = MXC_UART_GET_IRQ(config->identifier);
	ret = capi_irq_connect(irq, max_capi_uart_isr, uart_handle);
	if (ret)
		goto cleanup_channels;

	ret = capi_irq_enable(irq);
	if (ret)
		goto disconnect_irq;

	uart[config->identifier] = uart_handle;
	*handle = uart_handle;

	return 0;

disconnect_irq:
	capi_irq_connect(MXC_UART_GET_IRQ(config->identifier), NULL, NULL);
cleanup_channels:
	_max_capi_uart_dma_cleanup_channel(&dma_channel_tx[config->identifier]);
	_max_capi_uart_dma_cleanup_channel(&dma_channel_rx[config->identifier]);
shutdown_uart:
	MXC_UART_Shutdown(uart_priv->uart);
free_priv:
	capi_free(uart_priv);
free_handle:
	if (uart_handle->init_allocated)
		capi_free(uart_handle);

	uart[config->identifier] = NULL;

	return ret;
}

/**
 * @brief Deinitialize the UART peripheral
 * @param handle The UART handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_deinit(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;
	uint8_t id;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;
	id = uart_priv->id;

	_max_capi_uart_dma_cleanup_channel(&dma_channel_rx[id]);
	_max_capi_uart_dma_cleanup_channel(&dma_channel_tx[id]);
	uart_priv->uart->dma &= ~(MXC_F_UART_DMA_RX_EN | MXC_F_UART_DMA_TX_EN);
	async_transfer_in_progress[id] = false;
	dma_completed[id] = false;

	capi_irq_disable(MXC_UART_GET_IRQ(id));
	capi_irq_connect(MXC_UART_GET_IRQ(id), NULL, NULL);

	MXC_UART_Shutdown(uart_priv->uart);
	capi_free(handle->priv);
	if (handle->init_allocated)
		capi_free(handle);

	if (stdio_index == id)
		stdio_index = -1;

	uart[id] = NULL;

	return 0;
}

/**
 * @brief Read data from the UART device. Blocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer containing data.
 * @param len - Number of bytes to read.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_receive(struct capi_uart_handle *handle, uint8_t *buf,
			  uint32_t len)
{
	int ret, n;
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv ||  !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;

	n = (int)len;

	ret = MXC_UART_Read(uart_priv->uart, buf, &n);
	if (ret != E_SUCCESS)
		return -EIO;

	return 0;
}

/**
 * @brief Write data to the UART device. Blocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer containing data.
 * @param len - Number of bytes to write.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_transmit(struct capi_uart_handle *handle, uint8_t *buf,
			   uint32_t len)
{
	struct max_capi_uart_priv *uart_priv;
	int32_t transferred = 0;
	int block_size, ret;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;

	while (len) {
		block_size = (MXC_UART_FIFO_DEPTH < len ? MXC_UART_FIFO_DEPTH : len);
		while (!(MXC_UART_GetStatus(uart_priv->uart) &
			 MXC_F_UART_STATUS_TX_EM));
		ret = MXC_UART_Write(uart_priv->uart,
				     (uint8_t *)(buf + transferred),
				     &block_size);
		transferred += block_size;
		len -= block_size;

		if (ret != E_SUCCESS)
			return -EIO;
	}

	return transferred;
}

/**
 * @brief Read data from the UART device. Nonblocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer containing data.
 * @param len - Number of bytes to read.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_receive_async(struct capi_uart_handle *handle, uint8_t *buf,
				uint32_t len)
{
	int ret;
	uint32_t id;
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;
	id = uart_priv->id;

	if (async_transfer_in_progress[id])
		return -EBUSY;

	if (uart_priv->dma_handle)
		return _max_capi_uart_receive_dma(uart_priv, buf, len);

	uart_priv->async_req = (mxc_uart_req_t) {
		.uart = uart_priv->uart,
		.rxData = buf,
		.rxLen = len,
		.rxCnt = 0,
		.txData = NULL,
		.txLen = 0,
		.txCnt = 0,
		.callback = _max_capi_uart_msdk_callback,
	};

	async_transfer_in_progress[id] = true;

	ret = MXC_UART_TransactionAsync(&uart_priv->async_req);
	if (ret != E_NO_ERROR) {
		async_transfer_in_progress[id] = false;
		return (ret == E_BUSY ? -EBUSY : -EIO);
	}

	return 0;
}

/**
 * @brief Write data from the UART device. Nonblocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer to store the data.
 * @param len - Number of bytes to write.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_transmit_async(struct capi_uart_handle *handle,
				 uint8_t *buf, uint32_t len)
{
	int ret;
	uint32_t id;
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;
	id = uart_priv->id;

	if (async_transfer_in_progress[id])
		return -EBUSY;

	if (uart_priv->dma_handle)
		return _max_capi_uart_transmit_dma(uart_priv, buf, len);

	uart_priv->async_req = (mxc_uart_req_t) {
		.uart = uart_priv->uart,
		.txData = buf,
		.txLen = len,
		.txCnt = 0,
		.rxData = NULL,
		.rxLen = 0,
		.rxCnt = 0,
		.callback = _max_capi_uart_msdk_callback,
	};

	async_transfer_in_progress[id] = true;

	ret = MXC_UART_TransactionAsync(&uart_priv->async_req);
	if (ret != E_NO_ERROR) {
		async_transfer_in_progress[id] = false;
		return (ret == E_BUSY ? -EBUSY : -EIO);
	}

	return 0;
}

/**
 * @brief Register a callback
 * @param handle The UART handle
 * @param callback The callback function
 * @param callback_arg Callback arg
 */
int max_capi_uart_register_callback(struct capi_uart_handle *handle,
				    capi_uart_callback const callback,
				    void *const callback_arg)
{
	struct max_capi_uart_priv *uart_priv;
	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	uart_priv->callback = callback;
	uart_priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Interrupt function for UART
 * @param handle The UART handle
 */
void max_capi_uart_isr(void *handle)
{
	struct capi_uart_handle *uart_handle = (struct capi_uart_handle *)handle;
	struct max_capi_uart_priv *uart_priv;

	if (!uart_handle || !uart_handle->priv)
		return;

	uart_priv = uart_handle->priv;

	MXC_UART_AsyncHandler(uart_priv->uart);
}

/**
 * @brief Set the line config for UART
 * @param handle The UART handle
 * @param line_config The line config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_set_line_config(struct capi_uart_handle *handle,
				  struct capi_uart_line_config *line_config)
{
	struct max_capi_uart_priv *uart_priv;
	mxc_uart_parity_t parity;
	mxc_uart_stop_t stop;
	int ret;
	uint32_t size;

	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	uart_priv = handle->priv;

	ret = _max_capi_uart_map_line_config(line_config, &size, &parity, &stop);
	if (ret)
		return ret;

	ret = MXC_UART_SetFrequency(uart_priv->uart, line_config->baudrate,
				    uart_priv->clk_src);
	if (ret < 0)
		return -EINVAL;

	ret = MXC_UART_SetDataSize(uart_priv->uart, size);
	if (ret != E_NO_ERROR)
		return -EINVAL;

	ret = MXC_UART_SetParity(uart_priv->uart, parity);
	if (ret != E_NO_ERROR)
		return -EINVAL;

	ret = MXC_UART_SetStopBits(uart_priv->uart, stop);
	if (ret != E_NO_ERROR)
		return -EINVAL;

	uart_priv->line_config = *line_config;

	return 0;
}

/**
 * @brief Get the line config for UART
 * @param handle The UART handle
 * @param line_config Where to store the line config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_line_config(struct capi_uart_handle *handle,
				  struct capi_uart_line_config *line_config)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	uart_priv = handle->priv;

	*line_config = uart_priv->line_config;
	line_config->baudrate = MXC_UART_GetFrequency(uart_priv->uart);

	return 0;
}

/**
 * @brief Flush the RX FIFO
 * @param handle The UART handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_flush_rx_fifo(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	return MXC_UART_ClearRXFIFO(uart_priv->uart) == E_NO_ERROR ? 0 : -EIO;
}

/**
 * @brief Flush the TX FIFO
 * @param handle The UART handle
 * @return 0 on succes, negative error code otherwise
 */
int max_capi_uart_flush_tx_fifo(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	return MXC_UART_ClearTXFIFO(uart_priv->uart) == E_NO_ERROR ? 0 : -EIO;
}

/**
 * @brief Get the number of bytes available in the RX FIFO
 * @param handle The UART handle
 * @param count Where to store the number of bytes
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_rx_fifo_count(struct capi_uart_handle *handle,
				    uint16_t *count)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	*count = MXC_UART_GetRXFIFOAvailable(uart_priv->uart);

	return 0;
}

/**
 * @brief Get the number of queued bytes in the TX FIFO
 * @param handle The UART handle
 * @param count Where to store the number of bytes
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_tx_fifo_count(struct capi_uart_handle *handle,
				    uint16_t *count)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	*count = MXC_UART_FIFO_DEPTH - MXC_UART_GetTXFIFOAvailable(uart_priv->uart);

	return 0;
}

/**
 * @brief Get the reason for the interrupt
 * @param handle The UART handle
 * @param reason Where to store the interrupt reason
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_interrupt_reason(struct capi_uart_handle *handle,
				       enum capi_uart_interrupt_reason *reason)
{
	struct max_capi_uart_priv *uart_priv;
	uint32_t flags;

	if (!handle || !handle->priv || !reason)
		return -EINVAL;

	uart_priv = handle->priv;

	flags = MXC_UART_GetFlags(uart_priv->uart) & uart_priv->uart->inten;

	if (flags & (MXC_F_UART_INTFL_RX_FERR | MXC_F_UART_INTFL_RX_PAR |
		     MXC_F_UART_INTFL_RX_OV))
		*reason = CAPI_UART_INTR_RX_LINE_STATUS;
	else if (flags & (MXC_F_UART_INTFL_RX_THD | MXC_F_UART_INTFL_RX_FULL))
		*reason = CAPI_UART_INTR_RX_BUFFER_FULL;
	else if (flags & (MXC_F_UART_INTFL_TX_THD | MXC_F_UART_INTFL_TX_OB))
		*reason = CAPI_UART_INTR_TX_BUFFER_EMPTY;
	else if (flags & MXC_F_UART_INTFL_CTS_EV)
		*reason = CAPI_UART_INTR_MODEM_STATUS;
	else
		return -ENODATA;

	return 0;
}

/**
 * @brief Get UART line status flags
 * @param handle The UART handle
 * @param status_flags Where to store the status flags
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_line_status(struct capi_uart_handle *handle,
				  uint32_t *status_flags)
{
	struct max_capi_uart_priv *uart_priv;
	uint32_t flags;
	uint32_t clear = 0;
	uint32_t status = 0;

	if (!handle || !handle->priv || !status_flags)
		return -EINVAL;

	uart_priv = handle->priv;

	flags = MXC_UART_GetFlags(uart_priv->uart);

	if (flags & MXC_F_UART_INTFL_RX_FERR) {
		status |= CAPI_UART_LINE_STAT_FRAMING_ERROR;
		clear |= MXC_F_UART_INTFL_RX_FERR;
	}
	if (flags & MXC_F_UART_INTFL_RX_PAR) {
		status |= CAPI_UART_LINE_STAT_PARITY_ERROR;
		clear |= MXC_F_UART_INTFL_RX_PAR;
	}
	if (flags & MXC_F_UART_INTFL_RX_OV) {
		status |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;
		clear |= MXC_F_UART_INTFL_RX_OV;
	}
	/* MAX32657 has no break-detect flag. BREAK_IND is never reported. */

	if (clear)
		MXC_UART_ClearFlags(uart_priv->uart, clear);

	*status_flags = status;

	return 0;
}

/**
 * @brief Enable/disable FIFO - not supported. UART FIFOs are always on
 * @param handle The UART handle
 * @param enable Whether to enable or disable
 * @return -ENOSYS
 */
int max_capi_uart_enable_fifo(struct capi_uart_handle *handle, bool enable)
{
	return -ENOSYS;
}

/**
 * @brief Transmit 9-bit data - not supported. 9-bit mode is not supported.
 * @param handle The UART handle
 * @param data The data to transmit
 * @param is_address Whether it is an address or data byte
 * @return -ENOSYS
 */
int max_capi_uart_transmit_9bit(struct capi_uart_handle *handle, uint16_t data,
				bool is_address)
{
	return -ENOSYS;
}

/**
 * @brief Receive 9-bit data - not supported. 9-bit mode is not supported.
 * @param handle The UART handle
 * @param data Where to store the data
 * @param is_address Whether it is an address or data byte
 * @return -ENOSYS
 */
int max_capi_uart_receive_9bit(struct capi_uart_handle *handle, uint16_t *data,
			       bool *is_address)
{
	return -ENOSYS;
}

/**
 * @brief Set flow control state - not supported. Flow control is not supported.
 * @param handle The UART handle
 * @param rts_state RTS signal state
 * @param cts_state CTS signal state
 * @return -ENOSYS
 */
int max_capi_uart_set_flow_control_state(struct capi_uart_handle *handle,
		bool rts_state, bool cts_state)
{
	return -ENOSYS;
}

/**
 * @brief Get flow control state - not supported. Flow control is not supported.
 * @param handle The UART handle
 * @param rts_state Where to store the RTS signal state
 * @param cts_state Where to store the CTS signal state
 * @return -ENOSYS
 */
int max_capi_uart_get_flow_control_state(struct capi_uart_handle *handle,
		bool *rts_state, bool *cts_state)
{
	return -ENOSYS;
}

struct capi_uart_ops max_capi_uart_ops = {
	.init = max_capi_uart_init,
	.deinit = max_capi_uart_deinit,
	.transmit = max_capi_uart_transmit,
	.receive = max_capi_uart_receive,
	.transmit_async = max_capi_uart_transmit_async,
	.receive_async = max_capi_uart_receive_async,
	.register_callback = max_capi_uart_register_callback,
	.isr = max_capi_uart_isr,
	.set_line_config = max_capi_uart_set_line_config,
	.get_line_config = max_capi_uart_get_line_config,
	.flush_rx_fifo = max_capi_uart_flush_rx_fifo,
	.flush_tx_fifo = max_capi_uart_flush_tx_fifo,
	.get_rx_fifo_count = max_capi_uart_get_rx_fifo_count,
	.get_tx_fifo_count = max_capi_uart_get_tx_fifo_count,
	.get_interrupt_reason = max_capi_uart_get_interrupt_reason,
	.get_line_status = max_capi_uart_get_line_status,
	/* Everything below cannot be implemented; returns -ENOSYS */
	.enable_fifo = max_capi_uart_enable_fifo,
	.transmit_9bit = max_capi_uart_transmit_9bit,
	.receive_9bit = max_capi_uart_receive_9bit,
	.set_flow_control_state = max_capi_uart_set_flow_control_state,
	.get_flow_control_state = max_capi_uart_get_flow_control_state,
};

/** Platform-specific functions ***********************************************/

int max_capi_uart_stdio_enable(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle)
		return -EINVAL;

	uart_priv = handle->priv;
	stdio_index = uart_priv->id;

	if (handle != uart[uart_priv->id])
		return -EINVAL;

	setvbuf(stdout, NULL, _IONBF, 0);

	return 0;
}

/** stdio Redirect implementation *********************************************/

#define STDIN_FILENO	0   /**> Definition of stdin */
#define STDOUT_FILENO   1   /**> Definition of stdout */
#define STDERR_FILENO   2   /**> Definition of stderr */

int _isatty(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}

int _write(int file, char *ptr, int len)
{
	int ret;

	if (stdio_index == -1)
		return -1;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		ret = max_capi_uart_transmit(uart[stdio_index], (uint8_t *)ptr, len);
		if (ret < 0) {

			errno = -ret;
			return -1;
		}

		return ret;
	}
	errno = EBADF;
	return -1;
}

int _close(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}

int _lseek(int file, off_t offset, int whence)
{
	(void) file;
	(void) offset;
	(void) whence;

	errno = EBADF;
	return -1;
}

int _read(int file, char *ptr, int len)
{
	int ret;

	if (stdio_index == -1)
		return -1;

	if (file == STDIN_FILENO) {
		ret = max_capi_uart_receive(uart[stdio_index], (uint8_t *)ptr, 1);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return ret;
	}
	errno = EBADF;
	return -1;
}

int _fstat(int file, struct stat *st)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;
	return 0;
}
