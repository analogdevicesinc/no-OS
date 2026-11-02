/*******************************************************************************
 *   @file   maxim_capi_uart.c
 *   @brief  Implementation of the MAX32650/MAX32690 CAPI UART functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "capi_alloc.h"
#include "capi_dma.h"
#include "capi_irq.h"
#include "maxim_capi_uart.h"
#include "maxim_capi_uart_priv.h"
#include "mxc_device.h"
#include "mxc_errors.h"
#include "mxc_pins.h"

#if TARGET_NUM == 32650
#define MAX_UART_STATUS(u)	((u)->stat)
#define MAX_UART_ST_TX_EM	MXC_F_UART_STAT_TX_EMPTY
#define MAX_UART_ST_RX_EM	MXC_F_UART_STAT_RX_EMPTY
#define MAX_UART_ST_TX_FULL	MXC_F_UART_STAT_TX_FULL
#define MAX_UART_ST_TX_BUSY	MXC_F_UART_STAT_TX_BUSY
#define MAX_UART_RX_LVL(u)	(((u)->stat & MXC_F_UART_STAT_RX_NUM) >>	\
				 MXC_F_UART_STAT_RX_NUM_POS)
#define MAX_UART_TX_LVL(u)	(((u)->stat & MXC_F_UART_STAT_TX_NUM) >>	\
				 MXC_F_UART_STAT_TX_NUM_POS)
#define MAX_UART_FL_FERR	MXC_F_UART_INT_FL_FRAME
#define MAX_UART_FL_PAR		MXC_F_UART_INT_FL_PARITY
#define MAX_UART_FL_OV		MXC_F_UART_INT_FL_RX_OVR
#define MAX_UART_FL_RX		MXC_F_UART_INT_FL_RX_FIFO_LVL
#define MAX_UART_FL_TX		(MXC_F_UART_INT_FL_TX_FIFO_AE |		\
				 MXC_F_UART_INT_FL_TX_FIFO_LVL)
#define MAX_UART_FL_CTS		MXC_F_UART_INT_FL_CTS_CHANGE
#define MAX_UART_FL_BREAK	MXC_F_UART_INT_FL_BREAK
#define MAX_UART_EN_ERR		(MXC_F_UART_INT_EN_RX_FRAME_ERROR |	\
				 MXC_F_UART_INT_EN_RX_PARITY_ERROR |	\
				 MXC_F_UART_INT_EN_RX_OVERRUN)
#define MAX_UART_EN_RX		MXC_F_UART_INT_EN_RX_FIFO_LVL
#define MAX_UART_EN_TX		MXC_F_UART_INT_EN_TX_FIFO_AE
#define MAX_UART_DMA_TX_EN	MXC_F_UART_DMA_TXDMA_EN
#define MAX_UART_DMA_RX_EN	MXC_F_UART_DMA_RXDMA_EN

static const mxc_gpio_cfg_t *const max_uart_pins[MXC_UART_INSTANCES] = {
	&gpio_cfg_uart0, &gpio_cfg_uart1, &gpio_cfg_uart2,
};

static const mxc_dma_reqsel_t max_uart_rx_req[MXC_UART_INSTANCES] = {
	MXC_DMA_REQUEST_UART0RX, MXC_DMA_REQUEST_UART1RX, MXC_DMA_REQUEST_UART2RX,
};

static const mxc_dma_reqsel_t max_uart_tx_req[MXC_UART_INSTANCES] = {
	MXC_DMA_REQUEST_UART0TX, MXC_DMA_REQUEST_UART1TX, MXC_DMA_REQUEST_UART2TX,
};
#elif TARGET_NUM == 32690
#define MAX_UART_STATUS(u)	((u)->status)
#define MAX_UART_ST_TX_EM	MXC_F_UART_STATUS_TX_EM
#define MAX_UART_ST_RX_EM	MXC_F_UART_STATUS_RX_EM
#define MAX_UART_ST_TX_FULL	MXC_F_UART_STATUS_TX_FULL
#define MAX_UART_ST_TX_BUSY	MXC_F_UART_STATUS_TX_BUSY
#define MAX_UART_RX_LVL(u)	(((u)->status & MXC_F_UART_STATUS_RX_LVL) >>	\
				 MXC_F_UART_STATUS_RX_LVL_POS)
#define MAX_UART_TX_LVL(u)	(((u)->status & MXC_F_UART_STATUS_TX_LVL) >>	\
				 MXC_F_UART_STATUS_TX_LVL_POS)
#define MAX_UART_FL_FERR	MXC_F_UART_INT_FL_RX_FERR
#define MAX_UART_FL_PAR		MXC_F_UART_INT_FL_RX_PAR
#define MAX_UART_FL_OV		MXC_F_UART_INT_FL_RX_OV
#define MAX_UART_FL_RX		MXC_F_UART_INT_FL_RX_THD
#define MAX_UART_FL_TX		(MXC_F_UART_INT_FL_TX_HE |		\
				 MXC_F_UART_INT_FL_TX_OB)
#define MAX_UART_FL_CTS		MXC_F_UART_INT_FL_CTS_EV
#define MAX_UART_FL_BREAK	0
#define MAX_UART_EN_ERR		(MXC_F_UART_INT_EN_RX_FERR |		\
				 MXC_F_UART_INT_EN_RX_PAR |		\
				 MXC_F_UART_INT_EN_RX_OV)
#define MAX_UART_EN_RX		MXC_F_UART_INT_EN_RX_THD
#define MAX_UART_EN_TX		MXC_F_UART_INT_EN_TX_HE
#define MAX_UART_DMA_TX_EN	MXC_F_UART_DMA_TX_EN
#define MAX_UART_DMA_RX_EN	MXC_F_UART_DMA_RX_EN

static const mxc_gpio_cfg_t *const max_uart_pins[MXC_UART_INSTANCES] = {
	&gpio_cfg_uart0, &gpio_cfg_uart1, &gpio_cfg_uart2, &gpio_cfg_uart3,
};

static const mxc_dma_reqsel_t max_uart_rx_req[MXC_UART_INSTANCES] = {
	MXC_DMA_REQUEST_UART0RX, MXC_DMA_REQUEST_UART1RX,
	MXC_DMA_REQUEST_UART2RX, MXC_DMA_REQUEST_UART3RX,
};

static const mxc_dma_reqsel_t max_uart_tx_req[MXC_UART_INSTANCES] = {
	MXC_DMA_REQUEST_UART0TX, MXC_DMA_REQUEST_UART1TX,
	MXC_DMA_REQUEST_UART2TX, MXC_DMA_REQUEST_UART3TX,
};
#else
#error "Unsupported target for the Maxim CAPI UART driver"
#endif

#define MAX_UART_FL_ERR		(MAX_UART_FL_FERR | MAX_UART_FL_PAR | MAX_UART_FL_OV)

static struct capi_uart_handle *uart_handles[MXC_UART_INSTANCES];
static int8_t stdio_index = -1;

static int max_uart_set_baud(mxc_uart_regs_t *uart, uint32_t baud, bool init)
{
	int ret;

#if TARGET_NUM == 32650
	ret = init ? MXC_UART_Init(uart, baud) : MXC_UART_SetFrequency(uart, baud);
#else
	ret = init ? MXC_UART_Init(uart, baud, MXC_UART_APB_CLK) :
	      MXC_UART_SetFrequency(uart, baud, MXC_UART_APB_CLK);
#endif

	return ret < 0 ? -EINVAL : 0;
}

static int max_uart_map_line_config(const struct capi_uart_line_config *cfg,
				    int *size, mxc_uart_parity_t *parity,
				    mxc_uart_stop_t *stop)
{
	switch (cfg->size) {
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

	switch (cfg->parity) {
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

	switch (cfg->stop_bits) {
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

static int max_uart_apply_line_config(struct max_capi_uart_priv *priv,
				      const struct capi_uart_line_config *cfg,
				      bool init)
{
	mxc_uart_parity_t parity;
	mxc_uart_stop_t stop;
	int size;
	int ret;

	ret = max_uart_map_line_config(cfg, &size, &parity, &stop);
	if (ret)
		return ret;

	ret = max_uart_set_baud(priv->uart, cfg->baudrate, init);
	if (ret)
		return ret;

	if (MXC_UART_SetDataSize(priv->uart, size) != E_NO_ERROR ||
	    MXC_UART_SetParity(priv->uart, parity) != E_NO_ERROR ||
	    MXC_UART_SetStopBits(priv->uart, stop) != E_NO_ERROR)
		return -EINVAL;

	priv->line_config = *cfg;

	return 0;
}

static void max_uart_msdk_callback(mxc_uart_req_t *req, int result)
{
	struct capi_uart_handle *handle = uart_handles[MXC_UART_GET_IDX(req->uart)];
	struct max_capi_uart_priv *priv;
	enum capi_uart_async_event event;
	bool is_tx;

	if (!handle || !handle->priv)
		return;

	priv = handle->priv;
	is_tx = req->txData != NULL;

	if (result == E_NO_ERROR)
		event = is_tx ? CAPI_UART_EVENT_TX_DONE : CAPI_UART_EVENT_RX_DONE;
	else if (result == E_ABORT && is_tx)
		event = CAPI_UART_EVENT_TX_ABORTED;
	else
		event = CAPI_UART_EVENT_INTERRUPT;

	priv->async_in_progress = false;

	if (priv->callback)
		priv->callback(event, priv->callback_arg, result);
}

static void max_uart_dma_done(uint32_t event, void *ctx, bool is_tx)
{
	struct max_capi_uart_priv *priv = ctx;

	(void)event;

	priv->uart->dma &= ~(MAX_UART_DMA_TX_EN | MAX_UART_DMA_RX_EN);
	priv->async_in_progress = false;

	if (priv->callback)
		priv->callback(is_tx ? CAPI_UART_EVENT_TX_DONE :
			       CAPI_UART_EVENT_RX_DONE, priv->callback_arg, 0);
}

static void max_uart_dma_tx_done(uint32_t event, void *ctx)
{
	max_uart_dma_done(event, ctx, true);
}

static void max_uart_dma_rx_done(uint32_t event, void *ctx)
{
	max_uart_dma_done(event, ctx, false);
}

static int max_uart_dma_init(struct max_capi_uart_priv *priv,
			     struct capi_dma_handle *dma,
			     const struct max_capi_uart_extra *extra)
{
	uint32_t rx_id = extra ? extra->rx_dma_ch_id : 0;
	uint32_t tx_id = extra ? extra->tx_dma_ch_id : 1;
	int ret;

	if (rx_id == tx_id) {
		rx_id = 0;
		tx_id = 1;
	}

	ret = capi_dma_init_chan(dma, &priv->rx_dma, rx_id);
	if (ret)
		return ret;

	ret = capi_dma_init_chan(dma, &priv->tx_dma, tx_id);
	if (ret)
		goto free_rx;

	ret = capi_dma_register_complete_callback(priv->rx_dma,
		max_uart_dma_rx_done, priv);
	if (ret)
		goto free_tx;

	ret = capi_dma_register_complete_callback(priv->tx_dma,
		max_uart_dma_tx_done, priv);
	if (ret)
		goto free_tx;

	priv->rx_req.reqsel = max_uart_rx_req[priv->id];
	priv->tx_req.reqsel = max_uart_tx_req[priv->id];

	return 0;

free_tx:
	capi_dma_deinit_chan(priv->tx_dma);
	priv->tx_dma = NULL;
free_rx:
	capi_dma_deinit_chan(priv->rx_dma);
	priv->rx_dma = NULL;

	return ret;
}

static int max_uart_dma_start(struct max_capi_uart_priv *priv, uint8_t *buf,
			      uint32_t len, bool is_tx)
{
	struct capi_dma_chan *chan = is_tx ? priv->tx_dma : priv->rx_dma;
	int ret;

	if (is_tx) {
		priv->dma_xfer = (struct capi_dma_transfer) {
			.src = (capi_dma_glbl_addr_t)buf,
			.dst = (capi_dma_glbl_addr_t)&priv->uart->fifo,
			.src_inc = CAPI_DMA_BYTE_INCREMENT,
			.dst_inc = CAPI_DMA_NO_INCREMENT,
			.xfer_type = CAPI_DMA_MEM_TO_DEV,
			.extra = &priv->tx_req,
		};
	} else {
		priv->dma_xfer = (struct capi_dma_transfer) {
			.src = (capi_dma_glbl_addr_t)&priv->uart->fifo,
			.dst = (capi_dma_glbl_addr_t)buf,
			.src_inc = CAPI_DMA_NO_INCREMENT,
			.dst_inc = CAPI_DMA_BYTE_INCREMENT,
			.xfer_type = CAPI_DMA_DEV_TO_MEM,
			.extra = &priv->rx_req,
		};
	}

	priv->dma_xfer.src_size = CAPI_DMA_XFER_SIZE_1_BYTE;
	priv->dma_xfer.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE;
	priv->dma_xfer.length = len;

	ret = capi_dma_config_xfer(chan, &priv->dma_xfer);
	if (ret)
		return ret;

	priv->async_in_progress = true;

	ret = capi_dma_xfer_start(chan);
	if (ret) {
		priv->async_in_progress = false;
		return ret;
	}

	priv->uart->dma |= is_tx ? MAX_UART_DMA_TX_EN : MAX_UART_DMA_RX_EN;

	return 0;
}

static void max_capi_uart_isr(void *handle)
{
	struct capi_uart_handle *uart_handle = handle;
	struct max_capi_uart_priv *priv;

	if (!uart_handle || !uart_handle->priv)
		return;

	priv = uart_handle->priv;
	MXC_UART_AsyncHandler(priv->uart);
}

static int max_capi_uart_init(struct capi_uart_handle **handle,
			      const struct capi_uart_config *config)
{
	const struct max_capi_uart_extra *extra;
	struct capi_uart_handle *uart_handle;
	struct max_capi_uart_priv *priv;
	struct capi_uart_line_config line_cfg = {
		.baudrate = MAX_CAPI_UART_DEFAULT_BAUD,
		.size = CAPI_UART_DATA_BITS_8,
		.parity = CAPI_UART_PARITY_NONE,
		.stop_bits = CAPI_UART_STOP_1_BIT,
	};
	mxc_gpio_cfg_t pins;
	int ret;

	if (!handle || !config || config->identifier >= MXC_UART_INSTANCES)
		return -EINVAL;

	if (config->clk_freq_hz && config->clk_freq_hz != PeripheralClock)
		return -ENOTSUP;

	if (uart_handles[config->identifier]) {
		*handle = uart_handles[config->identifier];
		return 0;
	}

	if (!*handle) {
		uart_handle = capi_calloc(1, sizeof(*uart_handle));
		if (!uart_handle)
			return -ENOMEM;

		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(uart_handle);
			return -ENOMEM;
		}

		uart_handle->priv = priv;
		uart_handle->init_allocated = true;
	} else {
		uart_handle = *handle;
		if (!uart_handle->priv)
			return -EINVAL;

		priv = uart_handle->priv;
		memset(priv, 0, sizeof(*priv));
		uart_handle->init_allocated = false;
	}

	extra = config->extra;
	priv->id = config->identifier;
	priv->uart = MXC_UART_GET_UART(config->identifier);

	if (config->line_config)
		line_cfg = *config->line_config;

	ret = max_uart_apply_line_config(priv, &line_cfg, true);
	if (ret)
		goto free_handle;

	pins = *max_uart_pins[priv->id];
	pins.vssel = extra ? (mxc_gpio_vssel_t)extra->vssel : MXC_GPIO_VSSEL_VDDIO;
	if (MXC_GPIO_Config(&pins) != E_NO_ERROR) {
		ret = -EINVAL;
		goto shutdown;
	}

	if (config->dma_handle) {
		ret = max_uart_dma_init(priv, config->dma_handle, extra);
		if (ret)
			goto shutdown;
	}

	if (extra && extra->use_irq) {
#if defined(CONFIG_CAPI_IRQ)
		uint32_t irq = MXC_UART_GET_IRQ(priv->id);

		ret = capi_irq_connect(irq, max_capi_uart_isr, uart_handle);
		if (ret)
			goto free_dma;

		ret = capi_irq_enable(irq);
		if (ret)
			goto free_dma;

		priv->use_irq = true;
#else
		ret = -ENOTSUP;
		goto free_dma;
#endif
	}

	uart_handle->ops = config->ops;
	uart_handles[priv->id] = uart_handle;
	*handle = uart_handle;

	return 0;

free_dma:
	if (priv->rx_dma) {
		capi_dma_deinit_chan(priv->tx_dma);
		capi_dma_deinit_chan(priv->rx_dma);
	}
shutdown:
	MXC_UART_Shutdown(priv->uart);
free_handle:
	if (uart_handle->init_allocated) {
		capi_free(priv);
		capi_free(uart_handle);
	}

	return ret;
}

static int max_capi_uart_deinit(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	priv->uart->dma &= ~(MAX_UART_DMA_TX_EN | MAX_UART_DMA_RX_EN);
	if (priv->rx_dma) {
		capi_dma_xfer_abort(priv->tx_dma);
		capi_dma_xfer_abort(priv->rx_dma);
		capi_dma_deinit_chan(priv->tx_dma);
		capi_dma_deinit_chan(priv->rx_dma);
	}

#if defined(CONFIG_CAPI_IRQ)
	if (priv->use_irq)
		capi_irq_disable(MXC_UART_GET_IRQ(priv->id));
#endif

	MXC_UART_Shutdown(priv->uart);

	if (stdio_index == (int8_t)priv->id)
		stdio_index = -1;

	uart_handles[priv->id] = NULL;

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	}

	return 0;
}

static int max_capi_uart_receive(struct capi_uart_handle *handle, uint8_t *buf,
				 uint32_t len)
{
	struct max_capi_uart_priv *priv;
	int n = len;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	priv = handle->priv;

	return MXC_UART_Read(priv->uart, buf, &n) == E_NO_ERROR ? 0 : -EIO;
}

static int max_capi_uart_transmit(struct capi_uart_handle *handle, uint8_t *buf,
				  uint32_t len)
{
	struct max_capi_uart_priv *priv;
	uint32_t i;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	priv = handle->priv;

	for (i = 0; i < len; i++) {
		while (MAX_UART_STATUS(priv->uart) & MAX_UART_ST_TX_FULL)
			;
		priv->uart->fifo = buf[i];
	}

	while (MAX_UART_STATUS(priv->uart) & MAX_UART_ST_TX_BUSY)
		;

	return 0;
}

static int max_uart_async(struct capi_uart_handle *handle, uint8_t *buf,
			  uint32_t len, bool is_tx)
{
	struct max_capi_uart_priv *priv;
	int ret;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	priv = handle->priv;
	if (priv->async_in_progress)
		return -EBUSY;

	if (priv->rx_dma)
		return max_uart_dma_start(priv, buf, len, is_tx);

	if (!priv->use_irq)
		return -ENOTSUP;

	priv->async_req = (mxc_uart_req_t) {
		.uart = priv->uart,
		.txData = is_tx ? buf : NULL,
		.txLen = is_tx ? len : 0,
		.rxData = is_tx ? NULL : buf,
		.rxLen = is_tx ? 0 : len,
		.callback = max_uart_msdk_callback,
	};

	priv->async_in_progress = true;

	ret = MXC_UART_TransactionAsync(&priv->async_req);
	if (ret != E_NO_ERROR) {
		priv->async_in_progress = false;
		return ret == E_BUSY ? -EBUSY : -EIO;
	}

	return 0;
}

static int max_capi_uart_transmit_async(struct capi_uart_handle *handle,
					uint8_t *buf, uint32_t len)
{
	return max_uart_async(handle, buf, len, true);
}

static int max_capi_uart_receive_async(struct capi_uart_handle *handle,
				       uint8_t *buf, uint32_t len)
{
	return max_uart_async(handle, buf, len, false);
}

static int max_capi_uart_register_callback(struct capi_uart_handle *handle,
		capi_uart_callback const callback,
		void *const callback_arg)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	priv->callback = callback;
	priv->callback_arg = callback_arg;

	return 0;
}

static int max_capi_uart_set_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	return max_uart_apply_line_config(handle->priv, line_config, false);
}

static int max_capi_uart_get_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	priv = handle->priv;
	*line_config = priv->line_config;
	line_config->baudrate = MXC_UART_GetFrequency(priv->uart);

	return 0;
}

static int max_capi_uart_flush_rx_fifo(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

#if TARGET_NUM == 32650
	MXC_UART_ClearRXFIFO(priv->uart);

	return 0;
#else
	return MXC_UART_ClearRXFIFO(priv->uart) == E_NO_ERROR ? 0 : -EIO;
#endif
}

static int max_capi_uart_flush_tx_fifo(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

#if TARGET_NUM == 32650
	MXC_UART_ClearTXFIFO(priv->uart);

	return 0;
#else
	return MXC_UART_ClearTXFIFO(priv->uart) == E_NO_ERROR ? 0 : -EIO;
#endif
}

static int max_capi_uart_get_rx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv || !count)
		return -EINVAL;

	priv = handle->priv;
	*count = MAX_UART_RX_LVL(priv->uart);

	return 0;
}

static int max_capi_uart_get_tx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv || !count)
		return -EINVAL;

	priv = handle->priv;
	*count = MAX_UART_TX_LVL(priv->uart);

	return 0;
}

static int max_capi_uart_get_interrupt_reason(struct capi_uart_handle *handle,
		enum capi_uart_interrupt_reason *reason)
{
	struct max_capi_uart_priv *priv;
	uint32_t flags;

	if (!handle || !handle->priv || !reason)
		return -EINVAL;

	priv = handle->priv;
	flags = priv->uart->int_fl & priv->uart->int_en;

	if (flags & (MAX_UART_FL_ERR | MAX_UART_FL_BREAK))
		*reason = CAPI_UART_INTR_RX_LINE_STATUS;
	else if (flags & MAX_UART_FL_RX)
		*reason = CAPI_UART_INTR_RX_BUFFER_FULL;
	else if (flags & MAX_UART_FL_TX)
		*reason = CAPI_UART_INTR_TX_BUFFER_EMPTY;
	else if (flags & MAX_UART_FL_CTS)
		*reason = CAPI_UART_INTR_MODEM_STATUS;
	else
		*reason = CAPI_UART_INTR_NONE;

	return 0;
}

static int max_capi_uart_get_line_status(struct capi_uart_handle *handle,
		uint32_t *status_flags)
{
	struct max_capi_uart_priv *priv;
	uint32_t status = 0;
	uint32_t flags;

	if (!handle || !handle->priv || !status_flags)
		return -EINVAL;

	priv = handle->priv;
	flags = priv->uart->int_fl & (MAX_UART_FL_ERR | MAX_UART_FL_BREAK);

	if (flags & MAX_UART_FL_FERR)
		status |= CAPI_UART_LINE_STAT_FRAMING_ERROR;
	if (flags & MAX_UART_FL_PAR)
		status |= CAPI_UART_LINE_STAT_PARITY_ERROR;
	if (flags & MAX_UART_FL_OV)
		status |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;
	if (flags & MAX_UART_FL_BREAK)
		status |= CAPI_UART_LINE_STAT_BREAK_IND;

	if (flags)
		MXC_UART_ClearFlags(priv->uart, flags);

	*status_flags = status;

	return 0;
}

static uint32_t max_capi_uart_read_byte(struct capi_uart_handle *handle,
					uint8_t *byte)
{
	struct max_capi_uart_priv *priv;
	int c;

	if (!handle || !handle->priv || !byte)
		return 0;

	priv = handle->priv;
	c = MXC_UART_ReadCharacterRaw(priv->uart);
	if (c < 0)
		return 0;

	*byte = (uint8_t)c;

	return 1;
}

static uint32_t max_capi_uart_write_byte(struct capi_uart_handle *handle,
		uint8_t byte)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv)
		return 0;

	priv = handle->priv;

	return MXC_UART_WriteCharacterRaw(priv->uart, byte) == E_NO_ERROR;
}

static int max_uart_set_irq(struct capi_uart_handle *handle, uint32_t mask,
			    bool enable)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	if (!priv->use_irq)
		return -ENOTSUP;

	if (enable)
		MXC_UART_EnableInt(priv->uart, mask);
	else
		MXC_UART_DisableInt(priv->uart, mask);

	return 0;
}

static int max_capi_uart_set_irq_tx(struct capi_uart_handle *handle,
				    bool enable)
{
	return max_uart_set_irq(handle, MAX_UART_EN_TX, enable);
}

static int max_capi_uart_set_irq_rx(struct capi_uart_handle *handle,
				    bool enable)
{
	return max_uart_set_irq(handle, MAX_UART_EN_RX, enable);
}

static int max_capi_uart_set_irq_err(struct capi_uart_handle *handle,
				     bool enable)
{
	return max_uart_set_irq(handle, MAX_UART_EN_ERR, enable);
}

static int max_capi_uart_irq_tx_ready(struct capi_uart_handle *handle,
				      bool *ready)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv || !ready)
		return -EINVAL;

	priv = handle->priv;
	*ready = !(MAX_UART_STATUS(priv->uart) & MAX_UART_ST_TX_FULL);

	return 0;
}

static int max_capi_uart_irq_tx_complete(struct capi_uart_handle *handle,
		bool *complete)
{
	struct max_capi_uart_priv *priv;
	uint32_t status;

	if (!handle || !handle->priv || !complete)
		return -EINVAL;

	priv = handle->priv;
	status = MAX_UART_STATUS(priv->uart);
	*complete = (status & MAX_UART_ST_TX_EM) && !(status & MAX_UART_ST_TX_BUSY);

	return 0;
}

static int max_capi_uart_irq_rx_ready(struct capi_uart_handle *handle,
				      bool *ready)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv || !ready)
		return -EINVAL;

	priv = handle->priv;
	*ready = !(MAX_UART_STATUS(priv->uart) & MAX_UART_ST_RX_EM);

	return 0;
}

static int max_capi_uart_is_irq_pending(struct capi_uart_handle *handle,
					bool *pending)
{
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv || !pending)
		return -EINVAL;

	priv = handle->priv;
	*pending = (priv->uart->int_fl & priv->uart->int_en) != 0;

	return 0;
}

static int max_capi_uart_enable_fifo(struct capi_uart_handle *handle,
				     bool enable)
{
	(void)handle;
	(void)enable;

	return -ENOSYS;
}

static int max_capi_uart_transmit_9bit(struct capi_uart_handle *handle,
				       uint16_t data, bool is_address)
{
	(void)handle;
	(void)data;
	(void)is_address;

	return -ENOSYS;
}

static int max_capi_uart_receive_9bit(struct capi_uart_handle *handle,
				      uint16_t *data, bool *is_address)
{
	(void)handle;
	(void)data;
	(void)is_address;

	return -ENOSYS;
}

static int max_capi_uart_set_flow_control_state(struct capi_uart_handle *handle,
		bool rts_state, bool cts_state)
{
	(void)handle;
	(void)rts_state;
	(void)cts_state;

	return -ENOSYS;
}

static int max_capi_uart_get_flow_control_state(struct capi_uart_handle *handle,
		bool *rts_state, bool *cts_state)
{
	(void)handle;
	(void)rts_state;
	(void)cts_state;

	return -ENOSYS;
}

const struct capi_uart_ops max_capi_uart_ops = {
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
	.enable_fifo = max_capi_uart_enable_fifo,
	.read_byte = max_capi_uart_read_byte,
	.write_byte = max_capi_uart_write_byte,
	.set_irq_tx = max_capi_uart_set_irq_tx,
	.irq_tx_ready = max_capi_uart_irq_tx_ready,
	.irq_tx_complete = max_capi_uart_irq_tx_complete,
	.set_irq_rx = max_capi_uart_set_irq_rx,
	.irq_rx_ready = max_capi_uart_irq_rx_ready,
	.set_irq_err = max_capi_uart_set_irq_err,
	.is_irq_pending = max_capi_uart_is_irq_pending,
	.transmit_9bit = max_capi_uart_transmit_9bit,
	.receive_9bit = max_capi_uart_receive_9bit,
	.set_flow_control_state = max_capi_uart_set_flow_control_state,
	.get_flow_control_state = max_capi_uart_get_flow_control_state,
};

/**
 * @brief Route stdio through a UART.
 *
 * Not available when the no-OS UART stdio driver provides the syscalls.
 *
 * @param handle - UART handle.
 * @return 0 on success, negative error code otherwise.
 */
int max_capi_uart_stdio_enable(struct capi_uart_handle *handle)
{
#if defined(CONFIG_UART_STDIO_MAXIM)
	(void)handle;

	return -ENOTSUP;
#else
	struct max_capi_uart_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	if (handle != uart_handles[priv->id])
		return -EINVAL;

	stdio_index = priv->id;
	setvbuf(stdout, NULL, _IONBF, 0);

	return 0;
#endif
}

#if !defined(CONFIG_UART_STDIO_MAXIM)
#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

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

	if (stdio_index < 0)
		return -1;

	if (file != STDOUT_FILENO && file != STDERR_FILENO) {
		errno = EBADF;
		return -1;
	}

	ret = max_capi_uart_transmit(uart_handles[stdio_index], (uint8_t *)ptr,
				     len);
	if (ret) {
		errno = -ret;
		return -1;
	}

	return len;
}

int _read(int file, char *ptr, int len)
{
	int ret;

	(void)len;

	if (stdio_index < 0)
		return -1;

	if (file != STDIN_FILENO) {
		errno = EBADF;
		return -1;
	}

	ret = max_capi_uart_receive(uart_handles[stdio_index], (uint8_t *)ptr, 1);
	if (ret) {
		errno = -ret;
		return -1;
	}

	return 1;
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
	(void)file;
	(void)offset;
	(void)whence;

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

	return -1;
}
#endif
