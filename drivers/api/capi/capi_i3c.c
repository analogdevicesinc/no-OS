/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_i3c.c
 * @brief Thin wrapper functions for accessing the I3C driver via its ops pointer structure.
 *        Does not implement thread safety. If needed, copy and enhance in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_i3c.h>

int capi_i3c_init(struct capi_i3c_controller_handle **handle, const struct capi_i3c_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_i3c_deinit(struct capi_i3c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_i3c_transmit(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->transmit) {
		return -EINVAL;
	}
	return device->controller->ops->transmit(device, transfer);
}

int capi_i3c_receive(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->receive) {
		return -EINVAL;
	}
	return device->controller->ops->receive(device, transfer);
}

int capi_i3c_transmit_async(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->transmit_async) {
		return -EINVAL;
	}
	return device->controller->ops->transmit_async(device, transfer);
}

int capi_i3c_receive_async(struct capi_i3c_device *device, struct capi_i3c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->receive_async) {
		return -EINVAL;
	}
	return device->controller->ops->receive_async(device, transfer);
}

int capi_i3c_register_callback(struct capi_i3c_controller_handle *handle,
			       capi_i3c_callback callback, void *callback_arg)
{
	if (!handle || !handle->ops || !handle->ops->register_callback) {
		return -EINVAL;
	}
	return handle->ops->register_callback(handle, callback, callback_arg);
}

int capi_i3c_do_daa(struct capi_i3c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->do_daa) {
		return -EINVAL;
	}
	return handle->ops->do_daa(handle);
}

int capi_i3c_do_daa_async(struct capi_i3c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->do_daa_async) {
		return -EINVAL;
	}
	return handle->ops->do_daa_async(handle);
}

int capi_i3c_add_static_device(struct capi_i3c_controller_handle *handle, uint8_t addr,
			       uint8_t *dev_id)
{
	if (!handle || !handle->ops || !handle->ops->add_static_device) {
		return -EINVAL;
	}
	return handle->ops->add_static_device(handle, addr, dev_id);
}

int capi_i3c_get_num_devices(struct capi_i3c_controller_handle *handle, uint8_t *num_devices)
{
	if (!handle || !handle->ops || !handle->ops->get_num_devices) {
		return -EINVAL;
	}
	return handle->ops->get_num_devices(handle, num_devices);
}

int capi_i3c_get_device_info(struct capi_i3c_controller_handle *handle, uint8_t dev_id,
			     struct capi_i3c_device_info *info)
{
	if (!handle || !handle->ops || !handle->ops->get_device_info) {
		return -EINVAL;
	}
	return handle->ops->get_device_info(handle, dev_id, info);
}

int capi_i3c_register_target(struct capi_i3c_controller_handle *handle, bool wait_for_daa)
{
	if (!handle || !handle->ops || !handle->ops->register_target) {
		return -EINVAL;
	}
	return handle->ops->register_target(handle, wait_for_daa);
}

int capi_i3c_ibi_enable(struct capi_i3c_controller_handle *handle,
			enum capi_i3c_ibi_response response)
{
	if (!handle || !handle->ops || !handle->ops->ibi_enable) {
		return -EINVAL;
	}
	return handle->ops->ibi_enable(handle, response);
}

int capi_i3c_ibi_disable(struct capi_i3c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->ibi_disable) {
		return -EINVAL;
	}
	return handle->ops->ibi_disable(handle);
}

int capi_i3c_target_raise_ibi(struct capi_i3c_controller_handle *handle, uint8_t ibi_data,
			      bool has_extra)
{
	if (!handle || !handle->ops || !handle->ops->target_raise_ibi) {
		return -EINVAL;
	}
	return handle->ops->target_raise_ibi(handle, ibi_data, has_extra);
}

int capi_i3c_ibi_get_payload(struct capi_i3c_controller_handle *handle, uint8_t *addr,
			     uint8_t *ibi_data)
{
	if (!handle || !handle->ops || !handle->ops->ibi_get_payload) {
		return -EINVAL;
	}
	return handle->ops->ibi_get_payload(handle, addr, ibi_data);
}

int capi_i3c_send_ccc(struct capi_i3c_controller_handle *handle, uint8_t ccc_id, uint16_t addr,
		      const uint8_t *buf, uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->send_ccc) {
		return -EINVAL;
	}
	return handle->ops->send_ccc(handle, ccc_id, addr, buf, len);
}

int capi_i3c_recv_ccc(struct capi_i3c_controller_handle *handle, uint8_t ccc_id, uint16_t addr,
		      uint8_t *buf, uint32_t len)
{
	if (!handle || !handle->ops || !handle->ops->recv_ccc) {
		return -EINVAL;
	}
	return handle->ops->recv_ccc(handle, ccc_id, addr, buf, len);
}

int capi_i3c_hot_join_enable(struct capi_i3c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->hot_join_enable) {
		return -EINVAL;
	}
	return handle->ops->hot_join_enable(handle);
}

int capi_i3c_hot_join_disable(struct capi_i3c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->hot_join_disable) {
		return -EINVAL;
	}
	return handle->ops->hot_join_disable(handle);
}

int capi_i3c_transfer_batch(struct capi_i3c_device *device, struct capi_i3c_msg *msgs,
			    uint32_t count)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->transfer_batch) {
		return -EINVAL;
	}
	return device->controller->ops->transfer_batch(device, msgs, count);
}

void capi_i3c_isr(void *handle)
{
	if (!handle) {
		return;
	}
	struct capi_i3c_controller_handle *ctrl_handle =
		(struct capi_i3c_controller_handle *)handle;
	if (!ctrl_handle->ops || !ctrl_handle->ops->isr) {
		return;
	}
	ctrl_handle->ops->isr(ctrl_handle);
}
