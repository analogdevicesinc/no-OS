/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx XUartLite UART driver (AXI UartLite)
 *
 * Fixed 8N1. Baud rate, word length, parity and stop bits are set at
 * synthesis time and cannot be changed at runtime. set_line_config() is
 * accepted but only the loopback field has any effect.
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
#include "capi_irq.h"
#include "xinterrupt_wrap.h"
#include "xuartlite.h"
#include "xuartlite_l.h"

/*
 * UartLite baud rate is fixed in hardware at synthesis time. Cache the common
 * Vivado default for get_line_config() because embeddedsw cannot read it back.
 */
#define CAPI_UART_LITE_DEFAULT_BAUDRATE	9600U

static int capi_uart_pl_lite_init(struct capi_uart_handle **handle,
				  const struct capi_uart_config *config);
static int capi_uart_pl_lite_deinit(struct capi_uart_handle *handle);
static int capi_uart_pl_lite_get_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config);
static int capi_uart_pl_lite_set_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config);
static int capi_uart_pl_lite_enable_fifo(struct capi_uart_handle *handle,
		bool enable);
static int capi_uart_pl_lite_flush_tx_fifo(struct capi_uart_handle *handle);
static int capi_uart_pl_lite_flush_rx_fifo(struct capi_uart_handle *handle);
static int capi_uart_pl_lite_get_rx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count);
static int capi_uart_pl_lite_get_tx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count);
static int capi_uart_pl_lite_transmit(struct capi_uart_handle *handle,
				      uint8_t *buf, uint32_t len);
static int capi_uart_pl_lite_receive(struct capi_uart_handle *handle,
				     uint8_t *buf, uint32_t len);
static int capi_uart_pl_lite_register_callback(struct capi_uart_handle *handle,
		capi_uart_callback const callback,
		void *const callback_arg);
static int capi_uart_pl_lite_transmit_async(struct capi_uart_handle *handle,
		uint8_t *buf,
		uint32_t len);
static int capi_uart_pl_lite_receive_async(struct capi_uart_handle *handle,
		uint8_t *buf,
		uint32_t len);
static int capi_uart_pl_lite_get_interrupt_reason(struct capi_uart_handle
		*handle,
		enum capi_uart_interrupt_reason *reason);
static int capi_uart_pl_lite_get_line_status(struct capi_uart_handle *handle,
		uint32_t *status_flags);
static void capi_uart_pl_lite_isr(void *handle);
static void lite_tx_done(struct capi_uart_xilinx_handle *xh,
			 unsigned int event_data);
static void lite_send_handler(void *callback_ref, unsigned int event_data);
static void lite_recv_handler(void *callback_ref, unsigned int event_data);

struct capi_uart_ops capi_uart_xilinx_pl_lite_ops = {
	.init = capi_uart_pl_lite_init,
	.deinit = capi_uart_pl_lite_deinit,
	.get_line_config = capi_uart_pl_lite_get_line_config,
	.set_line_config = capi_uart_pl_lite_set_line_config,
	.enable_fifo = capi_uart_pl_lite_enable_fifo,
	.flush_tx_fifo = capi_uart_pl_lite_flush_tx_fifo,
	.flush_rx_fifo = capi_uart_pl_lite_flush_rx_fifo,
	.get_rx_fifo_count = capi_uart_pl_lite_get_rx_fifo_count,
	.get_tx_fifo_count = capi_uart_pl_lite_get_tx_fifo_count,
	.transmit = capi_uart_pl_lite_transmit,
	.receive = capi_uart_pl_lite_receive,
	.register_callback = capi_uart_pl_lite_register_callback,
	.transmit_async = capi_uart_pl_lite_transmit_async,
	.receive_async = capi_uart_pl_lite_receive_async,
	.get_interrupt_reason = capi_uart_pl_lite_get_interrupt_reason,
	.get_line_status = capi_uart_pl_lite_get_line_status,
	.transmit_9bit = NULL,
	.receive_9bit = NULL,
	.set_flow_control_state = NULL,
	.get_flow_control_state = NULL,
	.isr = capi_uart_pl_lite_isr,
};

static XUartLite *inst(struct capi_uart_xilinx_handle *xh)
{
	return (XUartLite *)xh->instance;
}

static bool lite_xfer_busy(struct capi_uart_xilinx_handle *xh)
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
 * @note PL: XUartLite_Initialize().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_init(struct capi_uart_handle **handle,
				  const struct capi_uart_config *config)
{
	if (handle == NULL || config == NULL)
		return -EINVAL;

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
	h->ops = config->ops ? config->ops : &capi_uart_xilinx_pl_lite_ops;
	h->priv = xh;

	/* Fixed 8N1; cache the known fields because hardware cannot report them. */
	xh->line_config.baudrate = CAPI_UART_LITE_DEFAULT_BAUDRATE;
	xh->line_config.size = CAPI_UART_DATA_BITS_8;
	xh->line_config.parity = CAPI_UART_PARITY_NONE;
	xh->line_config.stop_bits = CAPI_UART_STOP_1_BIT;
	xh->line_config.flow_control = CAPI_UART_FLOW_CONTROL_NONE;

	XUartLite *ul = capi_malloc(sizeof(XUartLite));
	if (ul == NULL) {
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -ENOMEM;
	}
	memset(ul, 0, sizeof(XUartLite));

	if (XUartLite_Initialize(ul, (UINTPTR)config->identifier) != XST_SUCCESS) {
		capi_free(ul);
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -EIO;
	}

	xh->instance = ul;

	XUartLite_ResetFifos(ul);
	XUartLite_SetSendHandler(ul, lite_send_handler, xh);
	XUartLite_SetRecvHandler(ul, lite_recv_handler, xh);

	const struct capi_uart_xilinx_config *xcfg =
		(const struct capi_uart_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;

		int ret = capi_irq_connect(irq_id, capi_uart_pl_lite_isr, h);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
			if (ret == 0) {
				xh->irq_id = irq_id;
				xh->use_irq = true;
			}
		}
	}

	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note PL: XUartLite_DisableInterrupt()/XUartLite_ResetFifos().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_deinit(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (xh == NULL)
		return -EINVAL;

	if (inst(xh)->SendBuffer.RemainingBytes > 0
	    || inst(xh)->ReceiveBuffer.RemainingBytes > 0)
		return -EBUSY;

	if (xh->use_irq) {
		capi_irq_disable(xh->irq_id);
		xh->use_irq = false;
	}

	XUartLite_DisableInterrupt(inst(xh));
	XUartLite_ResetFifos(inst(xh));

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
 * @note PL: Returns cached fixed 8N1.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_get_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	if (handle == NULL || line_config == NULL)
		return -EINVAL;

	/* Fixed at synthesis; return cached. */
	struct capi_uart_xilinx_handle *xh = handle->priv;
	memcpy(line_config, &xh->line_config, sizeof(*line_config));
	return 0;
}

/**
 * @brief Set the UART line configuration.
 * @note PL: Fixed 8N1; returns -ENOTSUP.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_set_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	if (handle == NULL || line_config == NULL)
		return -EINVAL;

	/* XUartLite line format is fixed in the IP. */
	(void)handle;
	(void)line_config;
	return -ENOTSUP;
}

/**
 * @brief Enable or disable UART FIFO behavior.
 * @note PL: FIFOs are always on.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_enable_fifo(struct capi_uart_handle *handle,
		bool enable)
{
	(void)handle;
	(void)enable;
	return 0; /* XUartLite always uses FIFOs */
}

/**
 * @brief Flush the UART TX FIFO.
 * @note PL: XUartLite_ResetFifos().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_flush_tx_fifo(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	XUartLite_ResetFifos(inst(xh));
	return 0;
}

/**
 * @brief Flush the UART RX FIFO.
 * @note PL: XUartLite_ResetFifos().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_flush_rx_fifo(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	XUartLite_ResetFifos(inst(xh));
	return 0;
}

/**
 * @brief Get the UART RX FIFO occupancy.
 * @note PL: Reads XUL_STATUS_REG_OFFSET.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_get_rx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	if (handle == NULL || count == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->RegBaseAddress;
	u32 sr = XUartLite_ReadReg(base, XUL_STATUS_REG_OFFSET);
	*count = (sr & XUL_SR_RX_FIFO_VALID_DATA) ? 1 : 0;
	return 0;
}

/**
 * @brief Get the UART TX FIFO occupancy.
 * @note PL: Reads XUL_STATUS_REG_OFFSET.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_get_tx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	if (handle == NULL || count == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->RegBaseAddress;
	u32 sr = XUartLite_ReadReg(base, XUL_STATUS_REG_OFFSET);
	*count = (sr & XUL_SR_TX_FIFO_EMPTY) ? 0 : 1;
	return 0;
}

/**
 * @brief Run a synchronous transmit operation.
 * @note PL: XUartLite_Send().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_transmit(struct capi_uart_handle *handle,
				      uint8_t *buf, uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (lite_xfer_busy(xh))
		return -EBUSY;

	uint32_t total = 0;
	while (total < len) {
		uint32_t sent = XUartLite_Send(inst(xh), buf + total, len - total);
		total += sent;
		/* Spin until FIFO has space if we couldn't send all */
		if (total < len) {
			UINTPTR base = inst(xh)->RegBaseAddress;
			while (!(XUartLite_ReadReg(base, XUL_STATUS_REG_OFFSET) &
				 XUL_SR_TX_FIFO_EMPTY))
				;
		}
	}
	/* Wait until TX FIFO drains */
	UINTPTR base = inst(xh)->RegBaseAddress;
	while (!(XUartLite_ReadReg(base, XUL_STATUS_REG_OFFSET) & XUL_SR_TX_FIFO_EMPTY))
		;
	return 0;
}

/**
 * @brief Run a synchronous receive operation.
 * @note PL: XUartLite_Recv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_receive(struct capi_uart_handle *handle,
				     uint8_t *buf, uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (lite_xfer_busy(xh))
		return -EBUSY;

	uint32_t total = 0;
	while (total < len)
		total += XUartLite_Recv(inst(xh), buf + total, len - total);
	return 0;
}

/**
 * @brief Register the CAPI asynchronous callback.
 * @note PL: Stores callback for handlers.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_register_callback(struct capi_uart_handle *handle,
		capi_uart_callback const callback,
		void *const callback_arg)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	xh->callback = callback;
	xh->callback_arg = callback_arg;
	return 0;
}

/**
 * @brief Start an asynchronous transmit operation.
 * @note PL: XUartLite_Send()+XUartLite_EnableInterrupt().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_transmit_async(struct capi_uart_handle *handle,
		uint8_t *buf,
		uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (inst(xh)->SendBuffer.RemainingBytes > 0)
		return -EBUSY;

	XUartLite_DisableInterrupt(inst(xh));
	XUartLite_Send(inst(xh), buf, len);
	if (inst(xh)->SendBuffer.RemainingBytes == 0) {
		lite_tx_done(xh, inst(xh)->SendBuffer.RequestedBytes);
		return 0;
	}

	XUartLite_EnableInterrupt(inst(xh));
	return 0;
}

/**
 * @brief Start an asynchronous receive operation.
 * @note PL: XUartLite_EnableInterrupt()+XUartLite_Recv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_receive_async(struct capi_uart_handle *handle,
		uint8_t *buf,
		uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (inst(xh)->ReceiveBuffer.RemainingBytes > 0)
		return -EBUSY;

	XUartLite_EnableInterrupt(inst(xh));
	XUartLite_Recv(inst(xh), buf, len);
	return 0;
}

/**
 * @brief Get the last UART interrupt reason.
 * @note PL: Returns cached ISR reason.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_get_interrupt_reason(struct capi_uart_handle
		*handle,
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
 * @note PL: XUartLite_GetStatusReg().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_pl_lite_get_line_status(struct capi_uart_handle *handle,
		uint32_t *status_flags)
{
	if (handle == NULL || status_flags == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->RegBaseAddress;
	u32 sr = XUartLite_GetStatusReg(base);
	*status_flags = 0;

#ifdef XUL_SR_FRAMING_ERROR
	if (sr & XUL_SR_FRAMING_ERROR)
		*status_flags |= CAPI_UART_LINE_STAT_FRAMING_ERROR;
#endif
#ifdef XUL_SR_PARITY_ERROR
	if (sr & XUL_SR_PARITY_ERROR)
		*status_flags |= CAPI_UART_LINE_STAT_PARITY_ERROR;
#endif
#ifdef XUL_SR_OVERRUN_ERROR
	if (sr & XUL_SR_OVERRUN_ERROR)
		*status_flags |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;
#endif

	return 0;
}

/**
 * @brief Dispatch the backend interrupt handler.
 * @note PL: XUartLite_InterruptHandler().
 *
 */
static void capi_uart_pl_lite_isr(void *handle)
{
	if (handle == NULL)
		return;

	struct capi_uart_xilinx_handle *xh = ((struct capi_uart_handle *)handle)->priv;
	XUartLite_InterruptHandler(inst(xh));
}

static void lite_tx_done(struct capi_uart_xilinx_handle *xh,
			 unsigned int event_data)
{
	if (xh == NULL || xh->instance == NULL)
		return;

	if (inst(xh)->SendBuffer.RequestedBytes == 0 && event_data == 0)
		return;
	if (event_data == 0)
		event_data = inst(xh)->SendBuffer.RequestedBytes;

	xh->last_interrupt_reason = CAPI_UART_INTR_TX_BUFFER_EMPTY;
	inst(xh)->SendBuffer.RequestedBytes = 0;
	XUartLite_DisableInterrupt(inst(xh));
	if (xh->callback)
		xh->callback(CAPI_UART_EVENT_TX_DONE, xh->callback_arg, event_data);
}

static void lite_send_handler(void *callback_ref, unsigned int event_data)
{
	struct capi_uart_xilinx_handle *xh = (struct capi_uart_xilinx_handle *)
					     callback_ref;

	if (xh == NULL || xh->instance == NULL)
		return;

	if (inst(xh)->SendBuffer.RemainingBytes > 0)
		return; /* Partial; wait for next TX empty interrupt. */

	lite_tx_done(xh, event_data);
}

static void lite_recv_handler(void *callback_ref, unsigned int event_data)
{
	struct capi_uart_xilinx_handle *xh = (struct capi_uart_xilinx_handle *)
					     callback_ref;

	if (xh == NULL || xh->instance == NULL)
		return;

	xh->last_interrupt_reason = CAPI_UART_INTR_RX_BUFFER_FULL;

	/* No receive_async pending; drain RX FIFO to clear the interrupt source,
	 * but do NOT disable interrupts (TX async may still be in progress). */
	if (inst(xh)->ReceiveBuffer.RequestedBytes == 0) {
		UINTPTR base = inst(xh)->RegBaseAddress;
		while (XUartLite_ReadReg(base,
					 XUL_STATUS_REG_OFFSET) & XUL_SR_RX_FIFO_VALID_DATA)
			(void)XUartLite_ReadReg(base, XUL_RX_FIFO_OFFSET);
		return;
	}

	if (inst(xh)->ReceiveBuffer.RemainingBytes > 0)
		return; /* Partial; wait for more data. */

	XUartLite_DisableInterrupt(inst(xh));

	if (xh->callback)
		xh->callback(CAPI_UART_EVENT_RX_DONE, xh->callback_arg, event_data);
}
