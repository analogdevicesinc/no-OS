/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_uart.c
 * @brief MAX32655 platform-specific UART CAPI implementation
 * @author Claude Code (noreply@anthropic.com)
 */

#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "capi_uart.h"
#include "maxim_uart.h"
#include "maxim_irq.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "no_os_lf256fifo.h"

#include "uart.h"
#include "max32655.h"
#include "mxc_errors.h"
#include "mxc_sys.h"
#include "gpio.h"

extern struct capi_isr_table isr_table[200];
static struct lf256fifo *async_rx_fifo[MXC_UART_INSTANCES];
static uint8_t rx_char[MXC_UART_INSTANCES];
static bool is_callback[MXC_UART_INSTANCES];

void UART0_IRQHandler()
{
	if (isr_table[UART0_IRQn].isr)
		isr_table[UART0_IRQn].isr(isr_table[UART0_IRQn].arg);
}

void UART1_IRQHandler()
{
	if (isr_table[UART1_IRQn].isr)
		isr_table[UART1_IRQn].isr(isr_table[UART1_IRQn].arg);
}

void UART2_IRQHandler()
{
	if (isr_table[UART2_IRQn].isr)
		isr_table[UART2_IRQn].isr(isr_table[UART2_IRQn].arg);
}

/* Forward declarations */
void maxim_uart_callback(mxc_uart_req_t *req, int result);

static void maxim_uart_read_one(enum capi_uart_async_event event, void *arg, int event_extra)
{
	struct capi_uart_handle *dev = arg;
	uint8_t ch;

	capi_uart_receive_async(dev, &ch, 1);
}

/**
 * @brief Convert CAPI data bits to character size
 */
static int capi_to_maxim_data_bits(enum capi_uart_data_bits data_bits)
{
	switch (data_bits) {
	case CAPI_UART_DATA_BITS_5:
		return 5;
	case CAPI_UART_DATA_BITS_6:
		return 6;
	case CAPI_UART_DATA_BITS_7:
		return 7;
	case CAPI_UART_DATA_BITS_8:
	default:
		return 8;
	}
}

/**
 * @brief Convert character size to CAPI data bits
 */
static enum capi_uart_data_bits maxim_to_capi_data_bits(int size)
{
	switch (size) {
	case 5:
		return CAPI_UART_DATA_BITS_5;
	case 6:
		return CAPI_UART_DATA_BITS_6;
	case 7:
		return CAPI_UART_DATA_BITS_7;
	case 8:
	default:
		return CAPI_UART_DATA_BITS_8;
	}
}

/**
 * @brief Convert CAPI parity to Maxim parity
 */
static mxc_uart_parity_t capi_to_maxim_parity(enum capi_uart_parity parity)
{
	switch (parity) {
	case CAPI_UART_PARITY_NONE:
		return MXC_UART_PARITY_DISABLE;
	case CAPI_UART_PARITY_ODD:
		return MXC_UART_PARITY_ODD_1;
	case CAPI_UART_PARITY_EVEN:
		return MXC_UART_PARITY_EVEN_1;
	case CAPI_UART_PARITY_MARK:
	case CAPI_UART_PARITY_SPACE:
	default:
		/* Maxim doesn't support mark/space parity, default to none */
		return MXC_UART_PARITY_DISABLE;
	}
}

/**
 * @brief Convert Maxim parity to CAPI parity
 */
static enum capi_uart_parity maxim_to_capi_parity(mxc_uart_parity_t parity)
{
	switch (parity) {
	case MXC_UART_PARITY_DISABLE:
		return CAPI_UART_PARITY_NONE;
	case MXC_UART_PARITY_ODD_0:
	case MXC_UART_PARITY_ODD_1:
		return CAPI_UART_PARITY_ODD;
	case MXC_UART_PARITY_EVEN_0:
	case MXC_UART_PARITY_EVEN_1:
		return CAPI_UART_PARITY_EVEN;
	default:
		return CAPI_UART_PARITY_NONE;
	}
}

/**
 * @brief Convert CAPI stop bits to Maxim stop bits
 */
static mxc_uart_stop_t capi_to_maxim_stop_bits(enum capi_uart_stop_bits stop_bits)
{
	switch (stop_bits) {
	case CAPI_UART_STOP_1_BIT:
		return MXC_UART_STOP_1;
	case CAPI_UART_STOP_2_BIT:
		return MXC_UART_STOP_2;
	default:
		return MXC_UART_STOP_1;
	}
}

/**
 * @brief Convert Maxim stop bits to CAPI stop bits
 */
static enum capi_uart_stop_bits maxim_to_capi_stop_bits(mxc_uart_stop_t stop_bits)
{
	switch (stop_bits) {
	case MXC_UART_STOP_1:
		return CAPI_UART_STOP_1_BIT;
	case MXC_UART_STOP_2:
		return CAPI_UART_STOP_2_BIT;
	default:
		return CAPI_UART_STOP_1_BIT;
	}
}

/**
 * @brief Convert CAPI flow control to Maxim flow control
 */
static mxc_uart_flow_t capi_to_maxim_flow_control(enum capi_uart_flow_control flow_control)
{
	switch (flow_control) {
	case CAPI_UART_FLOW_CONTROL_NONE:
		return MXC_UART_FLOW_DIS;
	case CAPI_UART_FLOW_CONTROL_RTS:
	case CAPI_UART_FLOW_CONTROL_CTS:
	case CAPI_UART_FLOW_CONTROL_RTSCTS:
		return MXC_UART_FLOW_EN;
	default:
		return MXC_UART_FLOW_DIS;
	}
}

/**
 * @brief Get UART registers pointer from device ID
 */
static mxc_uart_regs_t *get_uart_regs(uint32_t device_id)
{
	switch (device_id) {
	case 0:
		return MXC_UART0;
	case 1:
		return MXC_UART1;
	case 2:
		return MXC_UART2;
	case 3:
		return MXC_UART3;
	default:
		return NULL;
	}
}

/**
 * @brief Configure UART pins
 */
static int configure_uart_pins(uint32_t device_id)
{
	mxc_gpio_cfg_t uart_pins;

	switch (device_id) {
	case 0:
		uart_pins = gpio_cfg_uart0;
		break;
	case 1:
		uart_pins = gpio_cfg_uart1;
		break;
	case 2:
		uart_pins = gpio_cfg_uart2;
		break;
	case 3:
		uart_pins = gpio_cfg_uart3;
		break;
	default:
		return -EINVAL;
	}

	uart_pins.vssel = MXC_GPIO_VSSEL_VDDIO;
	MXC_GPIO_Config(&uart_pins);

	return 0;
}

/**
 * @brief Receive data (async)
 */
static int maxim_capi_uart_receive_async(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	struct maxim_capi_uart_handle *maxim_handle;
	int ret;

	if (!handle || !buf || !len)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;

	/* Set up async request */
	maxim_handle->async_req.uart = maxim_handle->uart_regs;
	maxim_handle->async_req.callback = maxim_uart_callback;
	maxim_handle->async_req.txData = NULL;
	maxim_handle->async_req.txLen = 0;
	maxim_handle->async_req.rxData = buf;
	maxim_handle->async_req.rxLen = len;
	maxim_handle->async_req.txCnt = 0;
	maxim_handle->async_req.rxCnt = 0;

	capi_irq_enable(MXC_UART_GET_IRQ(maxim_handle->device_id));

	if (!is_callback[maxim_handle->device_id]){
		ret = MXC_UART_TransactionAsync(&maxim_handle->async_req);
		if (ret != E_SUCCESS)
			return (ret == E_BUSY) ? -EBUSY : -EIO;
	}

	return 0;
}

/**
 * @brief Async callback from MaximSDK
 */
void maxim_uart_callback(mxc_uart_req_t *req, int result)
{
	struct maxim_capi_uart_handle *maxim_handle;
	enum capi_uart_async_event event;

	/* Find our handle from the request */
	maxim_handle = (struct maxim_capi_uart_handle *)((char *)req -
		offsetof(struct maxim_capi_uart_handle, async_req));

	/* Convert result to CAPI event */
	if (result == E_SUCCESS) {
		if (req->txLen > 0)
			event = CAPI_UART_EVENT_TX_DONE;
		else
			event = CAPI_UART_EVENT_RX_DONE;
	} else {
		if (req->txLen > 0)
			event = CAPI_UART_EVENT_TX_ABORTED;
		else
			event = CAPI_UART_EVENT_RX_TIMEOUT;
	}

	switch (event){
	case CAPI_UART_EVENT_RX_DONE:
		for (int i = 0; i < req->rxLen; i++)
			lf256fifo_write(async_rx_fifo[MXC_UART_GET_IDX(req->uart)], req->rxData[i]);

		maxim_capi_uart_receive_async(&maxim_handle->capi_handle, &rx_char[MXC_UART_GET_IDX(req->uart)], 1);
		break;
	case CAPI_UART_EVENT_TX_DONE:
		break;
	default:
		break;
	}

	if (!maxim_handle->callback)
		return;

	maxim_handle->async_req.uart = NULL;

	is_callback[0] = true;
	maxim_handle->callback(event, maxim_handle->callback_arg, result);
	is_callback[0] = false;
}

/**
 * @brief UART ISR
 */
static void maxim_capi_uart_isr(void *arg)
{
	struct capi_uart_handle *dev = arg;
	struct maxim_capi_uart_handle *priv = dev->priv;
	mxc_uart_req_t *req = &priv->async_req;

	/* ISR handling is done by MaximSDK through the async callback mechanism */
	MXC_UART_AsyncHandler(MXC_UART_GET_UART(priv->device_id));

	// if (req->uart && ((req->rxCnt == 0 && req->rxLen != 0) || (req->txLen != 0)))
	if (req->uart && ((req->rxCnt == 0 && req->rxLen != 0) || (req->txLen != 0)))
		MXC_UART_TransactionAsync(req);
}

/**
 * @brief Initialize UART handle
 */
static int maxim_capi_uart_init(struct capi_uart_handle **handle,
                                const struct capi_uart_config *config)
{
	struct maxim_capi_uart_handle *maxim_handle;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	if (!*handle){
		maxim_handle = no_os_calloc(1, sizeof(*maxim_handle));
		if (!maxim_handle)
			return -ENOMEM;

		maxim_handle->capi_handle.init_allocated = true;
	} else {
		maxim_handle = (*handle)->priv;
		maxim_handle->capi_handle.init_allocated = false;
	}

	maxim_handle->device_id = config->identifier;
	maxim_handle->uart_regs = get_uart_regs(config->identifier);
	if (!maxim_handle->uart_regs) {
		ret = -EINVAL;
		goto error;
	}

	/* Configure GPIO pins */
	ret = configure_uart_pins(config->identifier);
	if (ret)
		goto error;

	/* Initialize UART with MaximSDK */
	ret = MXC_UART_Init(maxim_handle->uart_regs,
		config->line_config ? config->line_config->baudrate : 115200,
		config->clk_freq_hz ? MXC_UART_APB_CLK : MXC_UART_IBRO_CLK);
	if (ret != E_SUCCESS) {
		ret = -EIO;
		goto error;
	}

	/* Configure line parameters if provided */
	if (config->line_config) {
		ret = MXC_UART_SetDataSize(maxim_handle->uart_regs,
			capi_to_maxim_data_bits(config->line_config->size));
		if (ret != E_SUCCESS) {
			ret = -EINVAL;
			goto error_shutdown;
		}

		ret = MXC_UART_SetParity(maxim_handle->uart_regs,
			capi_to_maxim_parity(config->line_config->parity));
		if (ret != E_SUCCESS) {
			ret = -EINVAL;
			goto error_shutdown;
		}

		ret = MXC_UART_SetStopBits(maxim_handle->uart_regs,
			capi_to_maxim_stop_bits(config->line_config->stop_bits));
		if (ret != E_SUCCESS) {
			ret = -EINVAL;
			goto error_shutdown;
		}

		ret = MXC_UART_SetFlowCtrl(maxim_handle->uart_regs,
			capi_to_maxim_flow_control(config->line_config->flow_control), 1);
		if (ret != E_SUCCESS) {
			ret = -EINVAL;
			goto error_shutdown;
		}

		maxim_handle->line_config = *config->line_config;
	} else {
		/* Set defaults */
		maxim_handle->line_config.baudrate = 115200;
		maxim_handle->line_config.size = CAPI_UART_DATA_BITS_8;
		maxim_handle->line_config.parity = CAPI_UART_PARITY_NONE;
		maxim_handle->line_config.stop_bits = CAPI_UART_STOP_1_BIT;
		maxim_handle->line_config.flow_control = CAPI_UART_FLOW_CONTROL_NONE;
		maxim_handle->line_config.address_mode = CAPI_UART_ADDRESS_MODE_DISABLED;
		maxim_handle->line_config.device_address = 0;
		maxim_handle->line_config.sticky_parity = false;
		maxim_handle->line_config.loopback = false;
	}

	maxim_handle->fifo_enabled = true; /* FIFO is enabled by default */

	/* Set up CAPI handle */
	maxim_handle->capi_handle.ops = config->ops;
	maxim_handle->capi_handle.lock = NULL;
	maxim_handle->capi_handle.priv = NULL;

	/* Initialize async request structure */
	maxim_handle->async_req.uart = maxim_handle->uart_regs;
	maxim_handle->async_req.callback = maxim_uart_callback;

	*handle = &maxim_handle->capi_handle;
	(*handle)->priv = maxim_handle;

	lf256fifo_init(&async_rx_fifo[config->identifier]);
	capi_irq_connect(MXC_UART_GET_IRQ(config->identifier), maxim_capi_uart_isr, *handle);

	return 0;

error_shutdown:
	MXC_UART_Shutdown(maxim_handle->uart_regs);
error:
	no_os_free(maxim_handle);
	return ret;
}

/**
 * @brief Deinitialize UART handle
 */
static int maxim_capi_uart_deinit(struct capi_uart_handle *handle)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;

	capi_irq_disable(MXC_UART_GET_IRQ(maxim_handle->device_id));
	capi_irq_connect(MXC_UART_GET_IRQ(maxim_handle->device_id), NULL, NULL);
	MXC_UART_Shutdown(maxim_handle->uart_regs);

	if (handle->init_allocated)
		no_os_free(maxim_handle);

	return 0;
}

/**
 * @brief Get line configuration
 */
static int maxim_capi_uart_get_line_config(struct capi_uart_handle *handle,
                                           struct capi_uart_line_config *line_config)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle || !line_config)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	*line_config = maxim_handle->line_config;

	return 0;
}

/**
 * @brief Set line configuration
 */
static int maxim_capi_uart_set_line_config(struct capi_uart_handle *handle,
                                           struct capi_uart_line_config *line_config)
{
	struct maxim_capi_uart_handle *maxim_handle;
	int ret;

	if (!handle || !line_config)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;

	/* Set baud rate */
	ret = MXC_UART_SetFrequency(maxim_handle->uart_regs, line_config->baudrate, MXC_UART_APB_CLK);
	if (ret < 0)
		return -EINVAL;

	/* Set data size */
	ret = MXC_UART_SetDataSize(maxim_handle->uart_regs,
		capi_to_maxim_data_bits(line_config->size));
	if (ret != E_SUCCESS)
		return -EINVAL;

	/* Set parity */
	ret = MXC_UART_SetParity(maxim_handle->uart_regs,
		capi_to_maxim_parity(line_config->parity));
	if (ret != E_SUCCESS)
		return -EINVAL;

	/* Set stop bits */
	ret = MXC_UART_SetStopBits(maxim_handle->uart_regs,
		capi_to_maxim_stop_bits(line_config->stop_bits));
	if (ret != E_SUCCESS)
		return -EINVAL;

	/* Set flow control */
	ret = MXC_UART_SetFlowCtrl(maxim_handle->uart_regs,
		capi_to_maxim_flow_control(line_config->flow_control), 1);
	if (ret != E_SUCCESS)
		return -EINVAL;

	/* Update stored configuration */
	maxim_handle->line_config = *line_config;

	return 0;
}

/**
 * @brief Enable/disable FIFO
 */
static int maxim_capi_uart_enable_fifo(struct capi_uart_handle *handle, bool enable)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	maxim_handle->fifo_enabled = enable;

	/* FIFO control is handled internally by MaximSDK */
	return 0;
}

/**
 * @brief Flush TX FIFO
 */
static int maxim_capi_uart_flush_tx_fifo(struct capi_uart_handle *handle)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	MXC_UART_ClearTXFIFO(maxim_handle->uart_regs);

	return 0;
}

/**
 * @brief Flush RX FIFO
 */
static int maxim_capi_uart_flush_rx_fifo(struct capi_uart_handle *handle)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	MXC_UART_ClearRXFIFO(maxim_handle->uart_regs);

	return 0;
}

/**
 * @brief Get RX FIFO count
 */
static int maxim_capi_uart_get_rx_fifo_count(struct capi_uart_handle *handle, uint16_t *count)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle || !count)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	*count = MXC_UART_GetRXFIFOAvailable(maxim_handle->uart_regs);

	return 0;
}

/**
 * @brief Get TX FIFO count
 */
static int maxim_capi_uart_get_tx_fifo_count(struct capi_uart_handle *handle, uint16_t *count)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle || !count)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	*count = MXC_UART_FIFO_DEPTH - MXC_UART_GetTXFIFOAvailable(maxim_handle->uart_regs);

	return 0;
}

/**
 * @brief Transmit data (blocking)
 */
static int maxim_capi_uart_transmit(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	struct maxim_capi_uart_handle *maxim_handle;
	int32_t transferred = 0;
	int block_size;
	int ret;

	if (!handle || !buf || !len)
		return -EINVAL;

	maxim_handle = handle->priv;

	while (len) {
		block_size = no_os_min(MXC_UART_FIFO_DEPTH, len);
		while (!(MXC_UART_GetStatus(maxim_handle->uart_regs) &
			 MXC_F_UART_STATUS_TX_EM));
		ret = MXC_UART_Write(maxim_handle->uart_regs,
				     &buf[transferred],
				     &block_size);

		transferred += block_size;
		len -= block_size;

		if (ret != E_SUCCESS)
			return -EIO;
	}

	while (!(MXC_UART_GetStatus(maxim_handle->uart_regs) &
	       MXC_F_UART_STATUS_TX_EM));

	return 0;
}

/**
 * @brief Receive data (blocking)
 */
static int maxim_capi_uart_receive(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	struct maxim_capi_uart_handle *maxim_handle;
	uint32_t read = 0;
	int ret;

	if (!handle || !buf || !len)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;

	while(read < len){
		ret = lf256fifo_read(async_rx_fifo[maxim_handle->device_id], &buf[read]); 
		if (ret != -1)
			read++;
	}

	// ret = MXC_UART_Read(maxim_handle->uart_regs, buf, (int *)&len);
	// if (ret != E_SUCCESS)
	// 	return -EIO;

	return 0;
}

/**
 * @brief Register callback
 */
static int maxim_capi_uart_register_callback(struct capi_uart_handle *handle,
                                             capi_uart_callback const callback,
                                             void *const callback_arg)
{
	struct maxim_capi_uart_handle *maxim_handle;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	maxim_handle->callback = callback;
	maxim_handle->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Transmit data (async)
 */
static int maxim_capi_uart_transmit_async(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	struct maxim_capi_uart_handle *maxim_handle;
	int ret;

	if (!handle || !buf || !len)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;

	/* Set up async request */
	maxim_handle->async_req.uart = maxim_handle->uart_regs;
	maxim_handle->async_req.callback = maxim_uart_callback;
	maxim_handle->async_req.txData = buf;
	maxim_handle->async_req.txLen = len;
	maxim_handle->async_req.rxData = NULL;
	maxim_handle->async_req.rxLen = 0;
	maxim_handle->async_req.txCnt = 0;
	maxim_handle->async_req.rxCnt = 0;

	capi_irq_enable(MXC_UART_GET_IRQ(maxim_handle->device_id));

	if (!is_callback[maxim_handle->device_id]){
		ret = MXC_UART_TransactionAsync(&maxim_handle->async_req);
		if (ret != E_SUCCESS)
			return (ret == E_BUSY) ? -EBUSY : -EIO;
	}

	return 0;
}

/**
 * @brief Get interrupt reason (limited support)
 */
static int maxim_capi_uart_get_interrupt_reason(struct capi_uart_handle *handle,
                                                enum capi_uart_interrupt_reason *reason)
{
	if (!handle || !reason)
		return -EINVAL;

	*reason = CAPI_UART_INTR_RX_BUFFER_FULL;

	return -ENOSYS;
}

/**
 * @brief Get line status
 */
static int maxim_capi_uart_get_line_status(struct capi_uart_handle *handle, uint32_t *status_flags)
{
	struct maxim_capi_uart_handle *maxim_handle;
	uint32_t status;

	if (!handle || !status_flags)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_uart_handle *)handle;
	status = MXC_UART_GetStatus(maxim_handle->uart_regs);
	*status_flags = 0;

	/* Map Maxim status flags to CAPI flags */
	// if (status & MXC_F_UART_STATUS_RX_BREAK)
	// 	*status_flags |= CAPI_UART_LINE_STAT_BREAK_IND;
	// if (status & MXC_F_UART_STATUS_RX_FRAMING_ERR)
	// 	*status_flags |= CAPI_UART_LINE_STAT_FRAMING_ERROR;
	// if (status & MXC_F_UART_STATUS_RX_PARITY_ERR)
	// 	*status_flags |= CAPI_UART_LINE_STAT_PARITY_ERROR;
	// if (status & MXC_F_UART_STATUS_RX_OVERRUN)
	// 	*status_flags |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;

	return 0;
}

/**
 * @brief Transmit 9-bit data (not supported by MAX32655)
 */
static int maxim_capi_uart_transmit_9bit(struct capi_uart_handle *handle, uint16_t data, bool is_address)
{
	return -ENOSYS;
}

/**
 * @brief Receive 9-bit data (not supported by MAX32655)
 */
static int maxim_capi_uart_receive_9bit(struct capi_uart_handle *handle, uint16_t *data, bool *is_address)
{
	return -ENOSYS;
}

/**
 * @brief Set flow control state (limited support)
 */
static int maxim_capi_uart_set_flow_control_state(struct capi_uart_handle *handle,
                                                  bool rts_state, bool cts_state)
{
	return -ENOSYS;
}

/**
 * @brief Get flow control state (limited support)
 */
static int maxim_capi_uart_get_flow_control_state(struct capi_uart_handle *handle,
                                                  bool *rts_state, bool *cts_state)
{
	return -ENOSYS;
}

/**
 * @brief MAX32655 UART CAPI operations structure
 */
const struct capi_uart_ops maxim_uart_ops = {
	.init = maxim_capi_uart_init,
	.deinit = maxim_capi_uart_deinit,
	.get_line_config = maxim_capi_uart_get_line_config,
	.set_line_config = maxim_capi_uart_set_line_config,
	.enable_fifo = maxim_capi_uart_enable_fifo,
	.flush_tx_fifo = maxim_capi_uart_flush_tx_fifo,
	.flush_rx_fifo = maxim_capi_uart_flush_rx_fifo,
	.get_rx_fifo_count = maxim_capi_uart_get_rx_fifo_count,
	.get_tx_fifo_count = maxim_capi_uart_get_tx_fifo_count,
	.transmit = maxim_capi_uart_transmit,
	.receive = maxim_capi_uart_receive,
	.register_callback = maxim_capi_uart_register_callback,
	.transmit_async = maxim_capi_uart_transmit_async,
	.receive_async = maxim_capi_uart_receive_async,
	.get_interrupt_reason = maxim_capi_uart_get_interrupt_reason,
	.get_line_status = maxim_capi_uart_get_line_status,
	.transmit_9bit = maxim_capi_uart_transmit_9bit,
	.receive_9bit = maxim_capi_uart_receive_9bit,
	.set_flow_control_state = maxim_capi_uart_set_flow_control_state,
	.get_flow_control_state = maxim_capi_uart_get_flow_control_state,
	.isr = maxim_capi_uart_isr,
};