/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx XUartNs550 UART driver (AXI UART 16550)
 *
 * Full 16550 UART: runtime baud, data bits, parity, stop bits, loopback.
 * OUT2 must be asserted for interrupts to propagate to the GIC.
 * Level-triggered: IRQ must be enabled per-operation and disabled after
 * completion to prevent interrupt storms.
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
#include "xuartns550.h"
#include "xuartns550_l.h"

/* Common UART reset/default line coding used when the caller does not provide one. */
#define CAPI_UART_NS550_DEFAULT_BAUDRATE	115200U

/* embeddedsw has no named "FIFO disabled" value for the 16550 FCR register. */
#define CAPI_UART_NS550_FCR_DISABLE		0x00U

static int capi_uart_ns550_init(struct capi_uart_handle **handle,
				const struct capi_uart_config *config);
static int capi_uart_ns550_deinit(struct capi_uart_handle *handle);
static int capi_uart_ns550_get_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config);
static int capi_uart_ns550_set_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config);
static int capi_uart_ns550_enable_fifo(struct capi_uart_handle *handle,
				       bool enable);
static int capi_uart_ns550_flush_tx_fifo(struct capi_uart_handle *handle);
static int capi_uart_ns550_flush_rx_fifo(struct capi_uart_handle *handle);
static int capi_uart_ns550_get_rx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count);
static int capi_uart_ns550_get_tx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count);
static int capi_uart_ns550_transmit(struct capi_uart_handle *handle,
				    uint8_t *buf, uint32_t len);
static int capi_uart_ns550_receive(struct capi_uart_handle *handle,
				   uint8_t *buf, uint32_t len);
static int capi_uart_ns550_register_callback(struct capi_uart_handle *handle,
		capi_uart_callback const callback,
		void *const callback_arg);
static int capi_uart_ns550_transmit_async(struct capi_uart_handle *handle,
		uint8_t *buf,
		uint32_t len);
static int capi_uart_ns550_receive_async(struct capi_uart_handle *handle,
		uint8_t *buf,
		uint32_t len);
static int capi_uart_ns550_get_interrupt_reason(struct capi_uart_handle *handle,
		enum capi_uart_interrupt_reason *reason);
static int capi_uart_ns550_get_line_status(struct capi_uart_handle *handle,
		uint32_t *status_flags);
static void capi_uart_ns550_isr(void *handle);
static void ns550_event_handler(void *callback_ref, u32 event,
				unsigned int event_data);

struct capi_uart_ops capi_uart_xilinx_pl_ns550_ops = {
	.init = capi_uart_ns550_init,
	.deinit = capi_uart_ns550_deinit,
	.get_line_config = capi_uart_ns550_get_line_config,
	.set_line_config = capi_uart_ns550_set_line_config,
	.enable_fifo = capi_uart_ns550_enable_fifo,
	.flush_tx_fifo = capi_uart_ns550_flush_tx_fifo,
	.flush_rx_fifo = capi_uart_ns550_flush_rx_fifo,
	.get_rx_fifo_count = capi_uart_ns550_get_rx_fifo_count,
	.get_tx_fifo_count = capi_uart_ns550_get_tx_fifo_count,
	.transmit = capi_uart_ns550_transmit,
	.receive = capi_uart_ns550_receive,
	.register_callback = capi_uart_ns550_register_callback,
	.transmit_async = capi_uart_ns550_transmit_async,
	.receive_async = capi_uart_ns550_receive_async,
	.get_interrupt_reason = capi_uart_ns550_get_interrupt_reason,
	.get_line_status = capi_uart_ns550_get_line_status,
	.transmit_9bit = NULL,
	.receive_9bit = NULL,
	.set_flow_control_state = NULL,
	.get_flow_control_state = NULL,
	.isr = capi_uart_ns550_isr,
};

static XUartNs550 *inst(struct capi_uart_xilinx_handle *xh)
{
	return (XUartNs550 *)xh->instance;
}

static bool ns550_xfer_busy(struct capi_uart_xilinx_handle *xh)
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
 * @note PL: XUartNs550_LookupConfig()/XUartNs550_CfgInitialize().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_init(struct capi_uart_handle **handle,
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
	h->ops = config->ops ? config->ops : &capi_uart_xilinx_pl_ns550_ops;
	h->priv = xh;

	xh->line_config.baudrate = CAPI_UART_NS550_DEFAULT_BAUDRATE;
	xh->line_config.size = CAPI_UART_DATA_BITS_8;
	xh->line_config.parity = CAPI_UART_PARITY_NONE;
	xh->line_config.stop_bits = CAPI_UART_STOP_1_BIT;
	xh->line_config.flow_control = CAPI_UART_FLOW_CONTROL_NONE;
	if (config->line_config != NULL)
		memcpy(&xh->line_config, config->line_config, sizeof(xh->line_config));

	XUartNs550_Config *cfg = XUartNs550_LookupConfig((UINTPTR)config->identifier);
	if (cfg == NULL) {
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -ENODEV;
	}

	XUartNs550 *ns = capi_malloc(sizeof(XUartNs550));
	if (ns == NULL) {
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -ENOMEM;
	}
	memset(ns, 0, sizeof(XUartNs550));

	if (XUartNs550_CfgInitialize(ns, cfg, cfg->BaseAddress) != XST_SUCCESS) {
		capi_free(ns);
		if (alloc)
			xilinx_uart_free_allocated_handle(handle);
		return -EIO;
	}

	xh->instance = ns;

	if (config->clk_freq_hz != 0) {
		ns->InputClockHz = config->clk_freq_hz;
	} else {
	}

	u16 options = XUN_OPTION_FIFOS_ENABLE | XUN_OPTION_RESET_TX_FIFO |
		      XUN_OPTION_RESET_RX_FIFO | XUN_OPTION_ASSERT_OUT2;
	XUartNs550_SetOptions(ns, options);

	capi_uart_ns550_set_line_config(h, &xh->line_config);

	XUartNs550_SetHandler(ns, ns550_event_handler, xh);

	const struct capi_uart_xilinx_config *xcfg =
		(const struct capi_uart_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, capi_uart_ns550_isr, h);
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
 * @note PL: Clears IER/FCR/MCR.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_deinit(struct capi_uart_handle *handle)
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

	UINTPTR base = inst(xh)->BaseAddress;
	XUartNs550_WriteReg(base, XUN_IER_OFFSET, 0);
	XUartNs550_WriteReg(base, XUN_FCR_OFFSET,
			    XUN_FIFO_TX_RESET | XUN_FIFO_RX_RESET);
	XUartNs550_WriteReg(base, XUN_MCR_OFFSET,
			    XUartNs550_ReadReg(base, XUN_MCR_OFFSET) & ~XUN_MCR_LOOP);

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
 * @note PL: Reads LCR/MCR.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_get_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	if (handle == NULL || line_config == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->BaseAddress;

	u8 lcr = XUartNs550_GetLineControlReg(base);

	/* Data bits: LCR[1:0] */
	u8 db = lcr & XUN_LCR_LENGTH_MASK;
	line_config->size = (db == 0)   ? CAPI_UART_DATA_BITS_5
			    : (db == 1) ? CAPI_UART_DATA_BITS_6
			    : (db == 2) ? CAPI_UART_DATA_BITS_7
			    : CAPI_UART_DATA_BITS_8;

	line_config->stop_bits =
		(lcr & XUN_LCR_2_STOP_BITS) ? CAPI_UART_STOP_2_BIT : CAPI_UART_STOP_1_BIT;

	/* MARK/SPACE parity needs cached config. */
	line_config->parity = xh->line_config.parity;
	line_config->sticky_parity = xh->line_config.sticky_parity;

	/* Baud: XUartNs550_SetBaud does not update BaudRate; use cached. */
	line_config->baudrate = xh->line_config.baudrate;

	u8 mcr = XUartNs550_ReadReg(base, XUN_MCR_OFFSET);
	line_config->loopback = (mcr & XUN_MCR_LOOP) ? true : false;

	line_config->flow_control = CAPI_UART_FLOW_CONTROL_NONE;
	line_config->address_mode = CAPI_UART_ADDRESS_MODE_DISABLED;
	line_config->device_address = 0;

	return 0;
}

/**
 * @brief Set the UART line configuration.
 * @note PL: XUartNs550_SetBaud()/SetLineControlReg().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_set_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	if (handle == NULL || line_config == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->BaseAddress;

	XUartNs550_SetBaud(base, inst(xh)->InputClockHz, line_config->baudrate);
	/* SetBaud does not update the BaudRate field; track it manually. */
	inst(xh)->BaudRate = line_config->baudrate;

	u8 lcr = 0;

	switch (line_config->size) {
	case CAPI_UART_DATA_BITS_5:
		break;
	case CAPI_UART_DATA_BITS_6:
		lcr |= XUN_LCR_6_DATA_BITS;
		break;
	case CAPI_UART_DATA_BITS_7:
		lcr |= XUN_LCR_7_DATA_BITS;
		break;
	default:
		lcr |= XUN_LCR_8_DATA_BITS;
		break;
	}

	if (line_config->stop_bits == CAPI_UART_STOP_2_BIT)
		lcr |= XUN_LCR_2_STOP_BITS;

	if (line_config->parity != CAPI_UART_PARITY_NONE) {
		lcr |= XUN_LCR_ENABLE_PARITY;
		switch (line_config->parity) {
		case CAPI_UART_PARITY_ODD:
			if (line_config->sticky_parity)
				lcr |= XUN_LCR_STICK_PARITY;
			break;
		case CAPI_UART_PARITY_EVEN:
			lcr |= XUN_LCR_EVEN_PARITY;
			if (line_config->sticky_parity)
				lcr |= XUN_LCR_STICK_PARITY;
			break;
		case CAPI_UART_PARITY_MARK:
			lcr |= XUN_LCR_STICK_PARITY;
			break;
		case CAPI_UART_PARITY_SPACE:
			lcr |= XUN_LCR_EVEN_PARITY | XUN_LCR_STICK_PARITY;
			break;
		default:
			break;
		}
	}

	XUartNs550_SetLineControlReg(base, lcr);

	u8 mcr = XUartNs550_ReadReg(base, XUN_MCR_OFFSET);
	if (line_config->loopback)
		mcr |= XUN_MCR_LOOP;
	else
		mcr &= ~XUN_MCR_LOOP;
	XUartNs550_WriteReg(base, XUN_MCR_OFFSET, mcr);

	memcpy(&xh->line_config, line_config, sizeof(*line_config));
	return 0;
}

/**
 * @brief Enable or disable UART FIFO behavior.
 * @note PL: Writes XUN_FCR_OFFSET.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_enable_fifo(struct capi_uart_handle *handle,
				       bool enable)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->BaseAddress;

	if (enable)
		XUartNs550_WriteReg(base, XUN_FCR_OFFSET,
				    XUN_FIFO_ENABLE | XUN_FIFO_TX_RESET | XUN_FIFO_RX_RESET |
				    XUN_FIFO_TRIGGER_08);
	else
		XUartNs550_WriteReg(base, XUN_FCR_OFFSET,
				    CAPI_UART_NS550_FCR_DISABLE);

	return 0;
}

/**
 * @brief Flush the UART TX FIFO.
 * @note PL: Resets TX FIFO.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_flush_tx_fifo(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->BaseAddress;
	XUartNs550_WriteReg(base, XUN_FCR_OFFSET,
			    XUN_FIFO_ENABLE | XUN_FIFO_TX_RESET | XUN_FIFO_TRIGGER_08);
	return 0;
}

/**
 * @brief Flush the UART RX FIFO.
 * @note PL: Resets RX FIFO.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_flush_rx_fifo(struct capi_uart_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	UINTPTR base = inst(xh)->BaseAddress;
	XUartNs550_WriteReg(base, XUN_FCR_OFFSET,
			    XUN_FIFO_ENABLE | XUN_FIFO_RX_RESET | XUN_FIFO_TRIGGER_08);
	return 0;
}

/**
 * @brief Get the UART RX FIFO occupancy.
 * @note PL: Reads XUN_LSR_OFFSET.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_get_rx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	if (handle == NULL || count == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	u8 lsr = XUartNs550_ReadReg(inst(xh)->BaseAddress, XUN_LSR_OFFSET);
	*count = (lsr & XUN_LSR_DATA_READY) ? 1 : 0;
	return 0;
}

/**
 * @brief Get the UART TX FIFO occupancy.
 * @note PL: Reads XUN_LSR_OFFSET.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_get_tx_fifo_count(struct capi_uart_handle *handle,
		uint16_t *count)
{
	if (handle == NULL || count == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	u8 lsr = XUartNs550_ReadReg(inst(xh)->BaseAddress, XUN_LSR_OFFSET);
	*count = (lsr & XUN_LSR_TX_BUFFER_EMPTY) ? 0 : 1;
	return 0;
}

/**
 * @brief Run a synchronous transmit operation.
 * @note PL: XUartNs550_Send().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_transmit(struct capi_uart_handle *handle,
				    uint8_t *buf, uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (ns550_xfer_busy(xh))
		return -EBUSY;

	uint32_t total = 0;
	while (total < len) {
		uint32_t sent = XUartNs550_Send(inst(xh), buf + total, len - total);
		total += sent;
		if (total < len) {
			/* Wait for TX FIFO to drain before sending more */
			while (!(XUartNs550_ReadReg(inst(xh)->BaseAddress, XUN_LSR_OFFSET) &
				 XUN_LSR_TX_BUFFER_EMPTY))
				;
		}
	}
	while (!(XUartNs550_ReadReg(inst(xh)->BaseAddress, XUN_LSR_OFFSET) &
		 XUN_LSR_TX_BUFFER_EMPTY))
		;
	return 0;
}

/**
 * @brief Run a synchronous receive operation.
 * @note PL: XUartNs550_Recv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_receive(struct capi_uart_handle *handle,
				   uint8_t *buf, uint32_t len)
{
	if (handle == NULL || buf == NULL || len == 0)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	if (ns550_xfer_busy(xh))
		return -EBUSY;

	uint32_t total = 0;
	while (total < len)
		total += XUartNs550_Recv(inst(xh), buf + total, len - total);
	return 0;
}

/**
 * @brief Register the CAPI asynchronous callback.
 * @note PL: Stores callback for events.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_register_callback(struct capi_uart_handle *handle,
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
 * @note PL: XUartNs550_Send() then enables TX IRQ.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_transmit_async(struct capi_uart_handle *handle,
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

	XUartNs550_Send(inst(xh), buf, len);
	/* Enable TX empty interrupt only after Send to prevent an early ISR. */
	u8 ier = XUartNs550_ReadReg(inst(xh)->BaseAddress, XUN_IER_OFFSET);
	XUartNs550_WriteReg(inst(xh)->BaseAddress, XUN_IER_OFFSET,
			    ier | XUN_IER_TX_EMPTY);
	return 0;
}

/**
 * @brief Start an asynchronous receive operation.
 * @note PL: Enables RX IRQ then XUartNs550_Recv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_receive_async(struct capi_uart_handle *handle,
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

	/* Enable RX interrupt before Recv */
	u8 ier = XUartNs550_ReadReg(inst(xh)->BaseAddress, XUN_IER_OFFSET);
	XUartNs550_WriteReg(inst(xh)->BaseAddress, XUN_IER_OFFSET,
			    ier | XUN_IER_RX_DATA);
	XUartNs550_Recv(inst(xh), buf, len);
	return 0;
}

/**
 * @brief Get the last UART interrupt reason.
 * @note PL: Returns cached ISR reason.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_get_interrupt_reason(struct capi_uart_handle *handle,
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
 * @note PL: Reads XUN_LSR_OFFSET.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_uart_ns550_get_line_status(struct capi_uart_handle *handle,
		uint32_t *status_flags)
{
	if (handle == NULL || status_flags == NULL)
		return -EINVAL;

	struct capi_uart_xilinx_handle *xh = handle->priv;
	u32 lsr = XUartNs550_ReadReg(inst(xh)->BaseAddress, XUN_LSR_OFFSET);
	*status_flags = 0;

	if (lsr & XUN_LSR_BREAK_INT)
		*status_flags |= CAPI_UART_LINE_STAT_BREAK_IND;

	if (lsr & XUN_LSR_FRAMING_ERROR)
		*status_flags |= CAPI_UART_LINE_STAT_FRAMING_ERROR;

	if (lsr & XUN_LSR_PARITY_ERROR)
		*status_flags |= CAPI_UART_LINE_STAT_PARITY_ERROR;

	if (lsr & XUN_LSR_OVERRUN_ERROR)
		*status_flags |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;

	return 0;
}

/**
 * @brief Dispatch the backend interrupt handler.
 * @note PL: XUartNs550_InterruptHandler().
 *
 */
static void capi_uart_ns550_isr(void *handle)
{
	if (handle == NULL)
		return;

	struct capi_uart_xilinx_handle *xh = ((struct capi_uart_handle *)handle)->priv;
	XUartNs550_InterruptHandler(inst(xh));
}

static void ns550_event_handler(void *callback_ref, u32 event,
				unsigned int event_data)
{
	struct capi_uart_xilinx_handle *xh = (struct capi_uart_xilinx_handle *)
					     callback_ref;

	if (xh == NULL || xh->instance == NULL)
		return;

	UINTPTR base = inst(xh)->BaseAddress;
	u8 ier;

	switch (event) {
	case XUN_EVENT_SENT_DATA:
		xh->last_interrupt_reason = CAPI_UART_INTR_TX_BUFFER_EMPTY;
		ier = XUartNs550_ReadReg(base, XUN_IER_OFFSET);
		XUartNs550_WriteReg(base, XUN_IER_OFFSET, ier & ~XUN_IER_TX_EMPTY);
		if (xh->callback)
			xh->callback(CAPI_UART_EVENT_TX_DONE, xh->callback_arg, event_data);
		break;

	case XUN_EVENT_RECV_DATA:
		xh->last_interrupt_reason = CAPI_UART_INTR_RX_BUFFER_FULL;
		if (inst(xh)->ReceiveBuffer.RemainingBytes > 0)
			return; /* Partial; keep interrupt enabled. */
		ier = XUartNs550_ReadReg(base, XUN_IER_OFFSET);
		XUartNs550_WriteReg(base, XUN_IER_OFFSET, ier & ~XUN_IER_RX_DATA);
		if (xh->callback)
			xh->callback(CAPI_UART_EVENT_RX_DONE, xh->callback_arg, event_data);
		break;

	case XUN_EVENT_RECV_TIMEOUT:
		xh->last_interrupt_reason = CAPI_UART_INTR_RX_BUFFER_FULL;
		if (inst(xh)->ReceiveBuffer.RemainingBytes == 0) {
			ier = XUartNs550_ReadReg(base, XUN_IER_OFFSET);
			XUartNs550_WriteReg(base, XUN_IER_OFFSET, ier & ~XUN_IER_RX_DATA);
			if (xh->callback)
				xh->callback(CAPI_UART_EVENT_RX_DONE, xh->callback_arg, event_data);
		} else {
			if (xh->callback)
				xh->callback(CAPI_UART_EVENT_RX_TIMEOUT, xh->callback_arg,
					     event_data);
		}
		break;

	case XUN_EVENT_RECV_ERROR:
		xh->last_interrupt_reason = CAPI_UART_INTR_RX_LINE_STATUS;
		if (xh->callback)
			xh->callback(CAPI_UART_EVENT_INTERRUPT, xh->callback_arg, event_data);
		break;

	case XUN_EVENT_MODEM:
		xh->last_interrupt_reason = CAPI_UART_INTR_MODEM_STATUS;
		if (xh->callback)
			xh->callback(CAPI_UART_EVENT_INTERRUPT, xh->callback_arg, event_data);
		break;

	default:
		if (xh->callback)
			xh->callback(CAPI_UART_EVENT_INTERRUPT, xh->callback_arg, event_data);
		break;
	}
}
