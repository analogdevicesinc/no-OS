/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx XUartPs UART driver (Zynq-7000 / ZynqMP PS UART)
 *
 * SDT BSP only. IRQ is explicit via capi_uart_xilinx_config.
 * capi_irq_init() must be called before async ops will work.
 */

#include <capi_uart.h>
#include <xilinx_capi_uart_priv.h>
#include <capi_alloc.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#ifndef CAPI_UART_XILINX_PS_NO_IRQ
#include "capi_irq.h"
#endif
#include "xinterrupt_wrap.h"
#include "xuartps.h"

/* Common UART reset/default line coding used when the caller does not provide one. */
#define CAPI_UART_PS_DEFAULT_BAUDRATE	115200U

static int capi_uart_ps_init(struct capi_uart_handle **handle,
			     const struct capi_uart_config *config);
static int capi_uart_ps_deinit(struct capi_uart_handle *handle);
static int capi_uart_ps_get_line_config(struct capi_uart_handle *handle,
					struct capi_uart_line_config *line_config);
static int capi_uart_ps_set_line_config(struct capi_uart_handle *handle,
					struct capi_uart_line_config *line_config);
static int capi_uart_ps_enable_fifo(struct capi_uart_handle *handle,
				    bool enable);
static int capi_uart_ps_flush_tx_fifo(struct capi_uart_handle *handle);
static int capi_uart_ps_flush_rx_fifo(struct capi_uart_handle *handle);
static int capi_uart_ps_get_rx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count);
static int capi_uart_ps_get_tx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count);
static int capi_uart_ps_transmit(struct capi_uart_handle *handle, uint8_t *buf,
				 uint32_t len);
static int capi_uart_ps_receive(struct capi_uart_handle *handle, uint8_t *buf,
				uint32_t len);
static int capi_uart_ps_register_callback(struct capi_uart_handle *handle,
		capi_uart_callback const callback,
		void *const callback_arg);
static int capi_uart_ps_transmit_async(struct capi_uart_handle *handle,
				       uint8_t *buf, uint32_t len);
static int capi_uart_ps_receive_async(struct capi_uart_handle *handle,
				      uint8_t *buf, uint32_t len);
static int capi_uart_ps_get_interrupt_reason(struct capi_uart_handle *handle,
		enum capi_uart_interrupt_reason *reason);
static int capi_uart_ps_get_line_status(struct capi_uart_handle *handle,
					uint32_t *status_flags);
static void capi_uart_ps_isr(void *handle);
static void ps_tx_done(struct capi_uart_xilinx_handle *xh, u32 event_data);
static void ps_event_handler(void *callback_ref, u32 event, u32 event_data);

struct capi_uart_ops capi_uart_xilinx_ps_ops = {
	.init = capi_uart_ps_init,
	.deinit = capi_uart_ps_deinit,
	.get_line_config = capi_uart_ps_get_line_config,
	.set_line_config = capi_uart_ps_set_line_config,
	.enable_fifo = capi_uart_ps_enable_fifo,
	.flush_tx_fifo = capi_uart_ps_flush_tx_fifo,
	.flush_rx_fifo = capi_uart_ps_flush_rx_fifo,
	.get_rx_fifo_count = capi_uart_ps_get_rx_fifo_count,
	.get_tx_fifo_count = capi_uart_ps_get_tx_fifo_count,
	.transmit = capi_uart_ps_transmit,
	.receive = capi_uart_ps_receive,
	.register_callback = capi_uart_ps_register_callback,
	.transmit_async = capi_uart_ps_transmit_async,
	.receive_async = capi_uart_ps_receive_async,
	.get_interrupt_reason = capi_uart_ps_get_interrupt_reason,
	.get_line_status = capi_uart_ps_get_line_status,
	.transmit_9bit = NULL,
	.receive_9bit = NULL,
	.set_flow_control_state = NULL,
	.get_flow_control_state = NULL,
	.isr = capi_uart_ps_isr,
};

static XUartPs *inst(struct capi_uart_xilinx_handle *xh)
{
	return (XUartPs *)xh->instance;
}

static bool ps_xfer_busy(struct capi_uart_xilinx_handle *xh)
{
	return inst(xh)->SendBuffer.RemainingBytes > 0 ||
	       inst(xh)->ReceiveBuffer.RemainingBytes > 0;
}

static void xilinx_uart_free_allocated_handle(struct capi_uart_handle **handle)
{
	if (handle == NULL || *handle == NULL)
		return;

	capi_free((*handle)->priv);
	capi_free(*handle);
	*handle = NULL;
}

/**
 * @brief Initialize the CAPI backend instance.
 * @note PS: XUartPs_LookupConfig()/XUartPs_CfgInitialize().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_init(struct capi_uart_handle **handle,
			     const struct capi_uart_config *config)
{
	if (handle == NULL || config == NULL)
		return -EINVAL;
	/* XUartPs async support is IRQ-only. */
	if (config->dma_handle != NULL)
		return -ENOTSUP;

	bool alloc = (*handle == NULL);
	struct capi_uart_handle *h = *handle;
	struct capi_uart_xilinx_handle *xh;

	if (alloc) {
		h = capi_malloc(sizeof(*h));
		if (h == NULL)
			return -ENOMEM;
		memset(h, 0, sizeof(*h));

		xh = capi_malloc(sizeof(*xh));
		if (xh == NULL) {
			capi_free(h);
			return -ENOMEM;
		}
		h->priv = xh;
		*handle = h;
	} else {
		xh = h->priv;
		if (xh == NULL)
			return -EINVAL;
	}

	memset(xh, 0, sizeof(*xh));
	h->init_allocated = alloc;
	h->ops = config->ops ? config->ops : &capi_uart_xilinx_ps_ops;
	h->priv = xh;

	xh->line_config.baudrate = CAPI_UART_PS_DEFAULT_BAUDRATE;
	xh->line_config.size = CAPI_UART_DATA_BITS_8;
	xh->line_config.parity = CAPI_UART_PARITY_NONE;
	xh->line_config.stop_bits = CAPI_UART_STOP_1_BIT;
	xh->line_config.flow_control = CAPI_UART_FLOW_CONTROL_NONE;
	if (config->line_config != NULL)
		memcpy(&xh->line_config, config->line_config, sizeof(xh->line_config));

	XUartPs_Config *cfg = XUartPs_LookupConfig((UINTPTR)config->identifier);
	if (cfg == NULL) {
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -ENODEV;
	}

	XUartPs *ps = capi_malloc(sizeof(XUartPs));
	if (ps == NULL) {
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -ENOMEM;
	}
	memset(ps, 0, sizeof(XUartPs));

	if (XUartPs_CfgInitialize(ps, cfg, cfg->BaseAddress) != XST_SUCCESS) {
		capi_free(ps);
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -EIO;
	}

	xh->instance = ps;

	capi_uart_ps_set_line_config(h, &xh->line_config);
	XUartPs_SetHandler(ps, ps_event_handler, xh);
	XUartPs_SetRecvTimeout(ps, 8);

	const struct capi_uart_xilinx_config *xcfg =
		(const struct capi_uart_xilinx_config *)config->extra;
#ifndef CAPI_UART_XILINX_PS_NO_IRQ
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, capi_uart_ps_isr, h);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
			if (ret == 0) {
				xh->irq_id = irq_id;
				xh->use_irq = true;
			}
		}
	}
#else
	(void)xcfg;
#endif

	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note PS: XUartPs_DisableUart()/XUartPs_SetInterruptMask(0).
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_deinit(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (xh == NULL)
		return -EINVAL;

	if (inst(xh)->SendBuffer.RemainingBytes > 0
	    || inst(xh)->ReceiveBuffer.RemainingBytes > 0)
		return -EBUSY;

#ifndef CAPI_UART_XILINX_PS_NO_IRQ
	if (xh->use_irq) {
		capi_irq_disable(xh->irq_id);
		xh->use_irq = false;
	}
#endif

	XUartPs_DisableUart(inst(xh));
	XUartPs_SetOperMode(inst(xh), 0);
	XUartPs_SetInterruptMask(inst(xh), 0);

	capi_free(xh->instance);
	xh->instance = NULL;

	if (handle->init_allocated) {
		capi_free(xh);
		capi_free(handle);
	} else {
		handle->ops = NULL;
	}

	return 0;
}

/**
 * @brief Get the UART line configuration.
 * @note PS: XUartPs_GetDataFormat()/XUartPs_ReadReg().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_get_line_config(struct capi_uart_handle *handle,
					struct capi_uart_line_config *line_config)
{
	if (handle == NULL || line_config == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	XUartPsFormat fmt;
	XUartPs_GetDataFormat(inst(xh), &fmt);

	line_config->baudrate = fmt.BaudRate;

	switch (fmt.DataBits) {
	case XUARTPS_FORMAT_6_BITS:
		line_config->size = CAPI_UART_DATA_BITS_6;
		break;
	case XUARTPS_FORMAT_7_BITS:
		line_config->size = CAPI_UART_DATA_BITS_7;
		break;
	default:
		line_config->size = CAPI_UART_DATA_BITS_8;
		break;
	}

	switch (fmt.Parity) {
	case XUARTPS_FORMAT_ODD_PARITY:
		line_config->parity = CAPI_UART_PARITY_ODD;
		break;
	case XUARTPS_FORMAT_EVEN_PARITY:
		line_config->parity = CAPI_UART_PARITY_EVEN;
		break;
	default:
		line_config->parity = CAPI_UART_PARITY_NONE;
		break;
	}

	line_config->stop_bits = (fmt.StopBits == XUARTPS_FORMAT_2_STOP_BIT) ?
				 CAPI_UART_STOP_2_BIT
				 : CAPI_UART_STOP_1_BIT;

	u32 mode = XUartPs_ReadReg(inst(xh)->Config.BaseAddress, XUARTPS_MR_OFFSET);
	line_config->loopback = (mode & XUARTPS_MR_CHMODE_L_LOOP) ? true : false;

	/* XUartPs has no hardware registers for these fields; return safe defaults. */
	line_config->flow_control = CAPI_UART_FLOW_CONTROL_NONE;
	line_config->address_mode = CAPI_UART_ADDRESS_MODE_DISABLED;
	line_config->device_address = 0;
	line_config->sticky_parity = false;

	return 0;
}

/**
 * @brief Set the UART line configuration.
 * @note PS: XUartPs_SetBaudRate()/XUartPs_SetDataFormat().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_set_line_config(struct capi_uart_handle *handle,
					struct capi_uart_line_config *line_config)
{
	if (handle == NULL || line_config == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;

	/* Reject line-config fields not supported by XUartPs. */
	if (line_config->flow_control != CAPI_UART_FLOW_CONTROL_NONE)
		return -ENOTSUP;
	if (line_config->address_mode != CAPI_UART_ADDRESS_MODE_DISABLED)
		return -ENOTSUP;
	if (line_config->device_address != 0)
		return -ENOTSUP;
	if (line_config->sticky_parity)
		return -ENOTSUP;
	if (line_config->parity == CAPI_UART_PARITY_MARK ||
	    line_config->parity == CAPI_UART_PARITY_SPACE)
		return -ENOTSUP;

	XUartPsFormat fmt;
	fmt.BaudRate = line_config->baudrate;

	switch (line_config->size) {
	case CAPI_UART_DATA_BITS_6:
		fmt.DataBits = XUARTPS_FORMAT_6_BITS;
		break;
	case CAPI_UART_DATA_BITS_7:
		fmt.DataBits = XUARTPS_FORMAT_7_BITS;
		break;
	default:
		fmt.DataBits = XUARTPS_FORMAT_8_BITS;
		break;
	}

	switch (line_config->parity) {
	case CAPI_UART_PARITY_ODD:
		fmt.Parity = XUARTPS_FORMAT_ODD_PARITY;
		break;
	case CAPI_UART_PARITY_EVEN:
		fmt.Parity = XUARTPS_FORMAT_EVEN_PARITY;
		break;
	default:
		fmt.Parity = XUARTPS_FORMAT_NO_PARITY;
		break;
	}

	fmt.StopBits = (line_config->stop_bits == CAPI_UART_STOP_2_BIT) ?
		       XUARTPS_FORMAT_2_STOP_BIT
		       : XUARTPS_FORMAT_1_STOP_BIT;

	if (XUartPs_SetBaudRate(inst(xh), fmt.BaudRate) != XST_SUCCESS)
		return -EINVAL;

	if (XUartPs_SetDataFormat(inst(xh), &fmt) != XST_SUCCESS)
		return -EINVAL;

	u8 oper = line_config->loopback ? XUARTPS_OPER_MODE_LOCAL_LOOP :
		  XUARTPS_OPER_MODE_NORMAL;
	XUartPs_SetOperMode(inst(xh), oper);

	memcpy(&xh->line_config, line_config, sizeof(*line_config));
	return 0;
}

/**
 * @brief Enable or disable UART FIFO behavior.
 * @note PS: FIFOs are always on.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_enable_fifo(struct capi_uart_handle *handle,
				    bool enable)
{
	(void)handle;
	(void)enable;
	return 0; /* XUartPs always uses FIFO */
}

/**
 * @brief Flush the UART TX FIFO.
 * @note PS: XUartPs_IsTransmitEmpty().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_flush_tx_fifo(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	while (!XUartPs_IsTransmitEmpty(inst(xh)))
		;
	return 0;
}

/**
 * @brief Flush the UART RX FIFO.
 * @note PS: XUartPs_IsReceiveData()/XUartPs_RecvByte().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_flush_rx_fifo(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->Config.BaseAddress;
	while (XUartPs_IsReceiveData(base))
		(void)XUartPs_RecvByte(base);
	return 0;
}

/**
 * @brief Get the UART RX FIFO occupancy.
 * @note PS: XUartPs_IsReceiveData().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_get_rx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	if (handle == NULL || count == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	*count = XUartPs_IsReceiveData(inst(xh)->Config.BaseAddress) ? 1 : 0;
	return 0;
}

/**
 * @brief Get the UART TX FIFO occupancy.
 * @note PS: XUartPs_IsTransmitEmpty().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_get_tx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	if (handle == NULL || count == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	*count = XUartPs_IsTransmitEmpty(inst(xh)) ? 0 : 1;
	return 0;
}

/**
 * @brief Run a synchronous transmit operation.
 * @note PS: XUartPs_Send()/XUartPs_IsSending().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_transmit(struct capi_uart_handle *handle, uint8_t *buf,
				 uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (ps_xfer_busy(xh))
		return -EBUSY;

	uint32_t sent = XUartPs_Send(inst(xh), buf, len);
	while (XUartPs_IsSending(inst(xh)))
		;
	return (sent == len) ? 0 : -EIO;
}

/**
 * @brief Run a synchronous receive operation.
 * @note PS: XUartPs_Recv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_receive(struct capi_uart_handle *handle, uint8_t *buf,
				uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (ps_xfer_busy(xh))
		return -EBUSY;

	uint32_t total = 0;
	while (total < len)
		total += XUartPs_Recv(inst(xh), buf + total, len - total);
	return 0;
}

/**
 * @brief Register the CAPI asynchronous callback.
 * @note PS: Stores callback for XUartPs events.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_register_callback(struct capi_uart_handle *handle,
		capi_uart_callback const callback,
		void *const callback_arg)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
#ifndef CAPI_UART_XILINX_PS_NO_IRQ
	if (xh->use_irq)
		capi_irq_disable(xh->irq_id);
#endif
	xh->callback = callback;
	xh->callback_arg = callback_arg;
#ifndef CAPI_UART_XILINX_PS_NO_IRQ
	if (xh->use_irq)
		capi_irq_enable(xh->irq_id);
#endif
	return 0;
}

/**
 * @brief Start an asynchronous transmit operation.
 * @note PS: XUartPs_Send()/XUartPs_SetInterruptMask().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_transmit_async(struct capi_uart_handle *handle,
				       uint8_t *buf, uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (inst(xh)->SendBuffer.RemainingBytes > 0)
		return -EBUSY;

	/* Clear stale TX status before starting this transfer. */
	XUartPs_WriteReg(inst(xh)->Config.BaseAddress, XUARTPS_ISR_OFFSET,
			 XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_TXFULL);

	/* Send first; XUartPs_Send disables TX interrupts internally. Arm
	 * TXEMPTY only if more bytes remain in the software buffer. */
	XUartPs_Send(inst(xh), buf, len);
	if (inst(xh)->SendBuffer.RemainingBytes == 0) {
		ps_tx_done(xh, inst(xh)->SendBuffer.RequestedBytes);
		return 0;
	}

	u32 mask = XUartPs_GetInterruptMask(inst(xh));
	XUartPs_SetInterruptMask(inst(xh), mask | XUARTPS_IXR_TXEMPTY);

	return 0;
}

/**
 * @brief Start an asynchronous receive operation.
 * @note PS: XUartPs_SetInterruptMask()/XUartPs_Recv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_receive_async(struct capi_uart_handle *handle,
				      uint8_t *buf, uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (inst(xh)->ReceiveBuffer.RemainingBytes > 0)
		return -EBUSY;

	/* OR in RX bits; preserve any active TX mask bits from transmit_async. */
	u32 mask = XUartPs_GetInterruptMask(inst(xh));
	XUartPs_SetInterruptMask(inst(xh),
				 mask | XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXFULL |
				 XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY |
				 XUARTPS_IXR_FRAMING | XUARTPS_IXR_OVER);
	XUartPs_Recv(inst(xh), buf, len);
	return 0;
}

/**
 * @brief Get the last UART interrupt reason.
 * @note PS: Returns cached ISR reason.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_get_interrupt_reason(struct capi_uart_handle *handle,
		enum capi_uart_interrupt_reason *reason)
{
	if (handle == NULL || reason == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	*reason = xh->last_interrupt_reason;
	return 0;
}

/**
 * @brief Get UART line status flags.
 * @note PS: XUartPs_ReadReg(ISR/RXBS).
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ps_get_line_status(struct capi_uart_handle *handle,
					uint32_t *status_flags)
{
	if (handle == NULL || status_flags == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->Config.BaseAddress;
	*status_flags = 0;

	u32 isr = XUartPs_ReadReg(base, XUARTPS_ISR_OFFSET);
	u32 rxbs = XUartPs_ReadReg(base, XUARTPS_RXBS_OFFSET);

	if ((isr & XUARTPS_IXR_RBRK) ||
	    (rxbs & (XUARTPS_RXBS_BYTE0_BRKE | XUARTPS_RXBS_BYTE1_BRKE |
		     XUARTPS_RXBS_BYTE2_BRKE |
		     XUARTPS_RXBS_BYTE3_BRKE)))
		*status_flags |= CAPI_UART_LINE_STAT_BREAK_IND;

	if ((isr & XUARTPS_IXR_FRAMING) ||
	    (rxbs & (XUARTPS_RXBS_BYTE0_FRME | XUARTPS_RXBS_BYTE1_FRME |
		     XUARTPS_RXBS_BYTE2_FRME |
		     XUARTPS_RXBS_BYTE3_FRME)))
		*status_flags |= CAPI_UART_LINE_STAT_FRAMING_ERROR;

	if ((isr & XUARTPS_IXR_PARITY) ||
	    (rxbs & (XUARTPS_RXBS_BYTE0_PARE | XUARTPS_RXBS_BYTE1_PARE |
		     XUARTPS_RXBS_BYTE2_PARE |
		     XUARTPS_RXBS_BYTE3_PARE)))
		*status_flags |= CAPI_UART_LINE_STAT_PARITY_ERROR;

	if (isr & XUARTPS_IXR_OVER)
		*status_flags |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;

	return 0;
}

/**
 * @brief Dispatch the backend interrupt handler.
 * @note PS: XUartPs_InterruptHandler().
 *
 */
static void capi_uart_ps_isr(void *handle)
{
	if (handle == NULL)
		return;

	struct capi_uart_xilinx_handle *xh = ((struct capi_uart_handle *)handle)->priv;
	XUartPs_InterruptHandler(inst(xh));
}

static void ps_tx_done(struct capi_uart_xilinx_handle *xh, u32 event_data)
{
	if (xh == NULL || xh->instance == NULL)
		return;

	if (inst(xh)->SendBuffer.RequestedBytes == 0 && event_data == 0)
		return;
	if (event_data == 0)
		event_data = inst(xh)->SendBuffer.RequestedBytes;

	xh->last_interrupt_reason = CAPI_UART_INTR_TX_BUFFER_EMPTY;
	XUartPs_SetInterruptMask(inst(xh), XUartPs_GetInterruptMask(inst(xh)) &
				 ~(XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_TXFULL));
	inst(xh)->SendBuffer.RequestedBytes = 0;
	if (xh->callback)
		xh->callback(CAPI_UART_EVENT_TX_DONE, xh->callback_arg, event_data);
}

static void ps_event_handler(void *callback_ref, u32 event, u32 event_data)
{
	struct capi_uart_xilinx_handle *xh = (struct capi_uart_xilinx_handle *)
					     callback_ref;

	if (xh == NULL || xh->instance == NULL)
		return;

	u32 rx_remaining = inst(xh)->ReceiveBuffer.RemainingBytes;

	switch (event) {
	case XUARTPS_EVENT_SENT_DATA: /* 3 */
		ps_tx_done(xh, event_data);
		break;

	case XUARTPS_EVENT_RECV_DATA: /* 1 */
		xh->last_interrupt_reason = CAPI_UART_INTR_RX_BUFFER_FULL;
		if (rx_remaining == 0) {
			XUartPs_SetInterruptMask(inst(xh),
						 XUartPs_GetInterruptMask(inst(xh)) &
						 ~(XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXFULL |
						   XUARTPS_IXR_TOUT));
			if (xh->callback)
				xh->callback(CAPI_UART_EVENT_RX_DONE, xh->callback_arg, event_data);
		}
		break;

	case XUARTPS_EVENT_RECV_TOUT: /* 2 */
		xh->last_interrupt_reason = CAPI_UART_INTR_RX_BUFFER_FULL;
		/* Timeout means no more data arriving; always disable and signal. */
		XUartPs_SetInterruptMask(
			inst(xh),
			XUartPs_GetInterruptMask(inst(xh)) &
			~(XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXFULL | XUARTPS_IXR_TOUT));
		if (xh->callback)
			xh->callback(rx_remaining == 0 ? CAPI_UART_EVENT_RX_DONE
				     : CAPI_UART_EVENT_RX_TIMEOUT,
				     xh->callback_arg, event_data);
		break;

	case XUARTPS_EVENT_RECV_ERROR:      /* 4 */
	case XUARTPS_EVENT_PARE_FRAME_BRKE: /* 6 */
	case XUARTPS_EVENT_RECV_ORERR:      /* 7 */
		xh->last_interrupt_reason = CAPI_UART_INTR_RX_LINE_STATUS;
		if (xh->callback)
			xh->callback(CAPI_UART_EVENT_INTERRUPT, xh->callback_arg, event_data);
		break;

	default:
		xh->last_interrupt_reason = CAPI_UART_INTR_MODEM_STATUS;
		if (xh->callback)
			xh->callback(CAPI_UART_EVENT_INTERRUPT, xh->callback_arg, event_data);
		break;
	}
}
