/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_eth_mac.c
 * @brief Thin wrapper functions that dispatch Ethernet MAC driver calls
 *        through the ops pointer structure.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_eth_mac.h>

int capi_eth_mac_init(struct capi_eth_mac_handle **handle,
		      const struct capi_eth_mac_init_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_eth_mac_deinit(struct capi_eth_mac_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_eth_mac_power_control(struct capi_eth_mac_handle *handle,
			       enum capi_eth_mac_power_state state)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->power_control != NULL) {
		return handle->ops->power_control(handle, state);
	}

	return -EINVAL;
}

int capi_eth_mac_get_capabilities(struct capi_eth_mac_handle *handle,
				  struct capi_eth_mac_capabilities *capabilities)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->get_capabilities != NULL) {
		return handle->ops->get_capabilities(handle, capabilities);
	}

	return -EINVAL;
}

int capi_eth_mac_get_address(struct capi_eth_mac_handle *handle,
			     capi_eth_mac_addr addr)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_address != NULL) {
		return handle->ops->get_address(handle, addr);
	}

	return -EINVAL;
}

int capi_eth_mac_set_address(struct capi_eth_mac_handle *handle,
			     const capi_eth_mac_addr addr)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_address != NULL) {
		return handle->ops->set_address(handle, addr);
	}

	return -EINVAL;
}

int capi_eth_mac_set_address_filter(struct capi_eth_mac_handle *handle,
				    const capi_eth_mac_addr *addr_list, uint32_t num_addr)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->set_address_filter != NULL) {
		return handle->ops->set_address_filter(handle, addr_list, num_addr);
	}

	return -EINVAL;
}

int capi_eth_mac_send_frame(struct capi_eth_mac_handle *handle,
			    const uint8_t *frame, uint32_t len,
			    const struct capi_eth_mac_tx_frame_config *config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->send_frame != NULL) {
		return handle->ops->send_frame(handle, frame, len, config);
	}

	return -EINVAL;
}

int capi_eth_mac_get_rx_frame_size(struct capi_eth_mac_handle *handle,
				   uint32_t *size)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->get_rx_frame_size != NULL) {
		return handle->ops->get_rx_frame_size(handle, size);
	}

	return -EINVAL;
}

int capi_eth_mac_read_frame(struct capi_eth_mac_handle *handle, uint8_t *frame,
			    uint32_t len,
			    struct capi_eth_mac_time *timestamp)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->read_frame != NULL) {
		return handle->ops->read_frame(handle, frame, len, timestamp);
	}

	return -EINVAL;
}

int capi_eth_mac_get_stats(struct capi_eth_mac_handle *handle,
			   struct capi_eth_mac_stats *stats)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_stats != NULL) {
		return handle->ops->get_stats(handle, stats);
	}

	return -EINVAL;
}

int capi_eth_mac_get_tx_frame_time(struct capi_eth_mac_handle *handle,
				   struct capi_eth_mac_time *time)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->get_tx_frame_time != NULL) {
		return handle->ops->get_tx_frame_time(handle, time);
	}

	return -EINVAL;
}

int capi_eth_mac_configure(struct capi_eth_mac_handle *handle,
			   const struct capi_eth_mac_config *config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->configure != NULL) {
		return handle->ops->configure(handle, config);
	}

	return -EINVAL;
}

int capi_eth_mac_set_tx_enabled(struct capi_eth_mac_handle *handle, bool enable)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->set_tx_enabled != NULL) {
		return handle->ops->set_tx_enabled(handle, enable);
	}

	return -EINVAL;
}

int capi_eth_mac_set_rx_enabled(struct capi_eth_mac_handle *handle, bool enable)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->set_rx_enabled != NULL) {
		return handle->ops->set_rx_enabled(handle, enable);
	}

	return -EINVAL;
}

int capi_eth_mac_flush_rx_buffer(struct capi_eth_mac_handle *handle)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->flush_rx_buffer != NULL) {
		return handle->ops->flush_rx_buffer(handle);
	}

	return -EINVAL;
}

int capi_eth_mac_flush_tx_buffer(struct capi_eth_mac_handle *handle)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->flush_tx_buffer != NULL) {
		return handle->ops->flush_tx_buffer(handle);
	}

	return -EINVAL;
}

int capi_eth_mac_sleep(struct capi_eth_mac_handle *handle, bool enable)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->sleep != NULL) {
		return handle->ops->sleep(handle, enable);
	}

	return -EINVAL;
}

int capi_eth_mac_set_vlan_filter(struct capi_eth_mac_handle *handle,
				 const struct capi_eth_vlan_filter *filter)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->set_vlan_filter != NULL) {
		return handle->ops->set_vlan_filter(handle, filter);
	}

	return -EINVAL;
}

int capi_eth_mac_ptp_get_time(struct capi_eth_mac_handle *handle,
			      struct capi_eth_mac_time *time)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->ptp_get_time != NULL) {
		return handle->ops->ptp_get_time(handle, time);
	}

	return -EINVAL;
}

int capi_eth_mac_ptp_set_time(struct capi_eth_mac_handle *handle,
			      const struct capi_eth_mac_time *time)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->ptp_set_time != NULL) {
		return handle->ops->ptp_set_time(handle, time);
	}

	return -EINVAL;
}

int capi_eth_mac_ptp_inc_time(struct capi_eth_mac_handle *handle,
			      const struct capi_eth_mac_time *time)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->ptp_inc_time != NULL) {
		return handle->ops->ptp_inc_time(handle, time);
	}

	return -EINVAL;
}

int capi_eth_mac_ptp_dec_time(struct capi_eth_mac_handle *handle,
			      const struct capi_eth_mac_time *time)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->ptp_dec_time != NULL) {
		return handle->ops->ptp_dec_time(handle, time);
	}

	return -EINVAL;
}

int capi_eth_mac_ptp_set_alarm(struct capi_eth_mac_handle *handle,
			       const struct capi_eth_mac_time *time)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->ptp_set_alarm != NULL) {
		return handle->ops->ptp_set_alarm(handle, time);
	}

	return -EINVAL;
}

int capi_eth_mac_ptp_adjust_freq(struct capi_eth_mac_handle *handle,
				 int32_t correction_q31)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->ptp_adjust_freq != NULL) {
		return handle->ops->ptp_adjust_freq(handle, correction_q31);
	}

	return -EINVAL;
}

int capi_eth_mac_register_callback(struct capi_eth_mac_handle *handle,
				   capi_eth_mac_event_callback const callback,
				   void *const callback_arg)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->register_callback != NULL) {
		return handle->ops->register_callback(handle, callback, callback_arg);
	}

	return -EINVAL;
}

void capi_eth_mac_isr(void *handle)
{
	struct capi_eth_mac_handle *mac_handle = (struct capi_eth_mac_handle *)handle;

	if (mac_handle != NULL && mac_handle->ops != NULL
	    && mac_handle->ops->isr != NULL) {
		mac_handle->ops->isr(handle);
	}
}