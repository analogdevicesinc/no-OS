/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_uart.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the UART driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the UART functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_uart.h>

int capi_uart_init(struct capi_uart_handle **handle,
		   const struct capi_uart_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_uart_deinit(struct capi_uart_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_uart_get_line_config(struct capi_uart_handle *handle,
			      struct capi_uart_line_config *line_config)
{
	if (!handle || !handle->ops || !handle->ops->get_line_config) {
		return -EINVAL;
	}
	return handle->ops->get_line_config(handle, line_config);
}

int capi_uart_set_line_config(struct capi_uart_handle *handle,
			      struct capi_uart_line_config *line_config)
{
	if (!handle || !handle->ops || !handle->ops->set_line_config) {
		return -EINVAL;
	}
	return handle->ops->set_line_config(handle, line_config);
}

int capi_uart_enable_fifo(struct capi_uart_handle *handle, bool enable)
{
	if (!handle || !handle->ops || !handle->ops->enable_fifo) {
		return -EINVAL;
	}
	return handle->ops->enable_fifo(handle, enable);
}

int capi_uart_flush_tx_fifo(struct capi_uart_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->flush_tx_fifo) {
		return -EINVAL;
	}
	return handle->ops->flush_tx_fifo(handle);
}

int capi_uart_flush_rx_fifo(struct capi_uart_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->flush_rx_fifo) {
		return -EINVAL;
	}
	return handle->ops->flush_rx_fifo(handle);
}

int capi_uart_get_rx_fifo_count(struct capi_uart_handle *handle,
				uint16_t *count)
{
	if (!handle || !handle->ops || !handle->ops->get_rx_fifo_count) {
		return -EINVAL;
	}
	return handle->ops->get_rx_fifo_count(handle, count);
}

int capi_uart_get_tx_fifo_count(struct capi_uart_handle *handle,
				uint16_t *count)
{
	if (!handle || !handle->ops || !handle->ops->get_tx_fifo_count) {
		return -EINVAL;
	}
	return handle->ops->get_tx_fifo_count(handle, count);
}

int capi_uart_transmit(struct capi_uart_handle *handle, uint8_t *buf,
		       uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->transmit) {
		return -EINVAL;
	}
	return handle->ops->transmit(handle, buf, len);
}

int capi_uart_receive(struct capi_uart_handle *handle, uint8_t *buf,
		      uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->receive) {
		return -EINVAL;
	}
	return handle->ops->receive(handle, buf, len);
}

int capi_uart_register_callback(struct capi_uart_handle *handle,
				capi_uart_callback const callback,
				void *const callback_arg)
{
	if (!handle || !handle->ops || !handle->ops->register_callback) {
		return -EINVAL;
	}
	return handle->ops->register_callback(handle, callback, callback_arg);
}

int capi_uart_transmit_async(struct capi_uart_handle *handle, uint8_t *buf,
			     uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->transmit_async) {
		return -EINVAL;
	}
	return handle->ops->transmit_async(handle, buf, len);
}

int capi_uart_receive_async(struct capi_uart_handle *handle, uint8_t *buf,
			    uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->receive_async) {
		return -EINVAL;
	}
	return handle->ops->receive_async(handle, buf, len);
}

int capi_uart_get_interrupt_reason(struct capi_uart_handle *handle,
				   enum capi_uart_interrupt_reason *reason)
{
	if (!handle || !handle->ops || !handle->ops->get_interrupt_reason) {
		return -EINVAL;
	}
	return handle->ops->get_interrupt_reason(handle, reason);
}

int capi_uart_get_line_status(struct capi_uart_handle *handle,
			      uint32_t *status_flags)
{
	if (!handle || !handle->ops || !handle->ops->get_line_status) {
		return -EINVAL;
	}
	return handle->ops->get_line_status(handle, status_flags);
}

int capi_uart_set_flow_control_state(struct capi_uart_handle *handle,
				     bool rts_state,
				     bool cts_state)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->set_flow_control_state != NULL) {
		return handle->ops->set_flow_control_state(handle, rts_state, cts_state);
	}

	return -EINVAL;
}

int capi_uart_get_flow_control_state(struct capi_uart_handle *handle,
				     bool *rts_state,
				     bool *cts_state)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->get_flow_control_state != NULL) {
		return handle->ops->get_flow_control_state(handle, rts_state, cts_state);
	}

	return -EINVAL;
}

void capi_uart_isr(void *handle)
{
	struct capi_uart_handle *ch = (struct capi_uart_handle *)handle;

	if (!handle || !ch->ops || !ch->ops->isr) {
		return;
	}
	ch->ops->isr(handle);
}

uint32_t capi_uart_read_byte(struct capi_uart_handle *handle, uint8_t *byte)
{
	if (!handle || !handle->ops || !handle->ops->read_byte) {
		return 0U;
	}
	return handle->ops->read_byte(handle, byte);
}

uint32_t capi_uart_write_byte(struct capi_uart_handle *handle, uint8_t byte)
{
	if (!handle || !handle->ops || !handle->ops->write_byte) {
		return 0U;
	}
	return handle->ops->write_byte(handle, byte);
}

int capi_uart_set_irq_tx(struct capi_uart_handle *handle, bool enable)
{
	if (!handle || !handle->ops || !handle->ops->set_irq_tx) {
		return -EINVAL;
	}
	return handle->ops->set_irq_tx(handle, enable);
}

int capi_uart_irq_tx_ready(struct capi_uart_handle *handle, bool *ready)
{
	if (!handle || !handle->ops || !handle->ops->irq_tx_ready) {
		return -EINVAL;
	}
	return handle->ops->irq_tx_ready(handle, ready);
}

int capi_uart_irq_tx_complete(struct capi_uart_handle *handle, bool *complete)
{
	if (!handle || !handle->ops || !handle->ops->irq_tx_complete) {
		return -EINVAL;
	}
	return handle->ops->irq_tx_complete(handle, complete);
}

int capi_uart_set_irq_rx(struct capi_uart_handle *handle, bool enable)
{
	if (!handle || !handle->ops || !handle->ops->set_irq_rx) {
		return -EINVAL;
	}
	return handle->ops->set_irq_rx(handle, enable);
}

int capi_uart_irq_rx_ready(struct capi_uart_handle *handle, bool *ready)
{
	if (!handle || !handle->ops || !handle->ops->irq_rx_ready) {
		return -EINVAL;
	}
	return handle->ops->irq_rx_ready(handle, ready);
}

int capi_uart_set_irq_err(struct capi_uart_handle *handle, bool enable)
{
	if (!handle || !handle->ops || !handle->ops->set_irq_err) {
		return -EINVAL;
	}
	return handle->ops->set_irq_err(handle, enable);
}

int capi_uart_is_irq_pending(struct capi_uart_handle *handle, bool *pending)
{
	if (!handle || !handle->ops || !handle->ops->is_irq_pending) {
		return -EINVAL;
	}
	return handle->ops->is_irq_pending(handle, pending);
}
