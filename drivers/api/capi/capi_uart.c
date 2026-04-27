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

int capi_uart_init(struct capi_uart_handle **handle, const struct capi_uart_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_uart_deinit(struct capi_uart_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_uart_get_line_config(struct capi_uart_handle *handle,
			      struct capi_uart_line_config *line_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_line_config != NULL) {
		return handle->ops->get_line_config(handle, line_config);
	}

	return -EINVAL;
}

int capi_uart_set_line_config(struct capi_uart_handle *handle,
			      struct capi_uart_line_config *line_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_line_config != NULL) {
		return handle->ops->set_line_config(handle, line_config);
	}

	return -EINVAL;
}

int capi_uart_enable_fifo(struct capi_uart_handle *handle, bool enable)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_fifo != NULL) {
		return handle->ops->enable_fifo(handle, enable);
	}

	return -EINVAL;
}

int capi_uart_flush_tx_fifo(struct capi_uart_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->flush_tx_fifo != NULL) {
		return handle->ops->flush_tx_fifo(handle);
	}

	return -EINVAL;
}

int capi_uart_flush_rx_fifo(struct capi_uart_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->flush_rx_fifo != NULL) {
		return handle->ops->flush_rx_fifo(handle);
	}

	return -EINVAL;
}

int capi_uart_get_rx_fifo_count(struct capi_uart_handle *handle, uint16_t *count)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_rx_fifo_count != NULL) {
		return handle->ops->get_rx_fifo_count(handle, count);
	}

	return -EINVAL;
}

int capi_uart_get_tx_fifo_count(struct capi_uart_handle *handle, uint16_t *count)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_tx_fifo_count != NULL) {
		return handle->ops->get_tx_fifo_count(handle, count);
	}

	return -EINVAL;
}

int capi_uart_transmit(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->transmit != NULL) {
		return handle->ops->transmit(handle, buf, len);
	}

	return -EINVAL;
}

int capi_uart_receive(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->receive != NULL) {
		return handle->ops->receive(handle, buf, len);
	}

	return -EINVAL;
}

int capi_uart_register_callback(struct capi_uart_handle *handle, capi_uart_callback const callback,
				void *const callback_arg)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->register_callback != NULL) {
		return handle->ops->register_callback(handle, callback, callback_arg);
	}

	return -EINVAL;
}

int capi_uart_transmit_async(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->transmit_async != NULL) {
		return handle->ops->transmit_async(handle, buf, len);
	}

	return -EINVAL;
}

int capi_uart_receive_async(struct capi_uart_handle *handle, uint8_t *buf, uint32_t len)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->receive_async != NULL) {
		return handle->ops->receive_async(handle, buf, len);
	}

	return -EINVAL;
}

int capi_uart_get_interrupt_reason(struct capi_uart_handle *handle,
				   enum capi_uart_interrupt_reason *reason)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_interrupt_reason != NULL) {
		return handle->ops->get_interrupt_reason(handle, reason);
	}

	return -EINVAL;
}

int capi_uart_get_line_status(struct capi_uart_handle *handle, uint32_t *status_flags)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_line_status != NULL) {
		return handle->ops->get_line_status(handle, status_flags);
	}

	return -EINVAL;
}

void capi_uart_isr(void *handle)
{
	struct capi_uart_handle *ch = (struct capi_uart_handle *)handle;

	if (handle != NULL && ch->ops != NULL && ch->ops->isr != NULL) {
		ch->ops->isr(handle);
	}
}
