/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_edu.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the EDU driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the EDU functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_edu.h>

int capi_edu_init(struct capi_edu_handle **handle, const struct capi_edu_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_edu_deinit(struct capi_edu_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_edu_configure_source(struct capi_edu_handle *handle,
			      const struct capi_edu_source_config *src_cfg)
{
	if (!handle || !handle->ops || !handle->ops->configure_source) {
		return -EINVAL;
	}
	return handle->ops->configure_source(handle, src_cfg);
}

int capi_edu_set_fault_config(struct capi_edu_handle *handle,
			      const struct capi_edu_fault_config *cfg)
{
	if (!handle || !handle->ops || !handle->ops->set_fault_config) {
		return -EINVAL;
	}
	return handle->ops->set_fault_config(handle, cfg);
}

int capi_edu_set_interrupt_config(struct capi_edu_handle *handle,
				  const struct capi_edu_interrupt_config *cfg)
{
	if (!handle || !handle->ops || !handle->ops->set_interrupt_config) {
		return -EINVAL;
	}
	return handle->ops->set_interrupt_config(handle, cfg);
}

int capi_edu_register_source_callback(struct capi_edu_handle *handle,
				      capi_edu_source_callback_t callback, void *user_data)
{
	if (!handle || !handle->ops || !handle->ops->register_source_callback) {
		return -EINVAL;
	}
	return handle->ops->register_source_callback(handle, callback, user_data);
}

int capi_edu_register_error_callback(struct capi_edu_handle *handle,
				     capi_edu_error_callback_t callback, void *user_data)
{
	if (!handle || !handle->ops || !handle->ops->register_error_callback) {
		return -EINVAL;
	}
	return handle->ops->register_error_callback(handle, callback, user_data);
}

int capi_edu_enable(struct capi_edu_handle *handle, bool enable)
{
	if (!handle || !handle->ops || !handle->ops->enable) {
		return -EINVAL;
	}
	return handle->ops->enable(handle, enable);
}

int capi_edu_reset(struct capi_edu_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->reset) {
		return -EINVAL;
	}
	return handle->ops->reset(handle);
}

int capi_edu_lock(struct capi_edu_handle *handle, bool lock)
{
	if (!handle || !handle->ops || !handle->ops->lock) {
		return -EINVAL;
	}
	return handle->ops->lock(handle, lock);
}

int capi_edu_raise(struct capi_edu_handle *handle, uint8_t sid)
{
	if (!handle || !handle->ops || !handle->ops->raise) {
		return -EINVAL;
	}
	return handle->ops->raise(handle, sid);
}

int capi_edu_get_status(struct capi_edu_handle *handle, enum capi_edu_status_type type,
			uint32_t *status)
{
	if (!handle || !handle->ops || !handle->ops->get_status) {
		return -EINVAL;
	}
	return handle->ops->get_status(handle, type, status);
}

int capi_edu_get_source_status(struct capi_edu_handle *handle, uint8_t sid, uint32_t *status)
{
	if (!handle || !handle->ops || !handle->ops->get_source_status) {
		return -EINVAL;
	}
	return handle->ops->get_source_status(handle, sid, status);
}

void capi_edu_event_isr(void *handle)
{
	if (!handle) {
		return;
	}
	struct capi_edu_handle *edu_handle = (struct capi_edu_handle *)handle;

	if (!edu_handle->ops || !edu_handle->ops->event_isr) {
		return;
	}
	edu_handle->ops->event_isr(handle);
}

void capi_edu_fault_isr(void *handle)
{
	if (!handle) {
		return;
	}
	struct capi_edu_handle *edu_handle = (struct capi_edu_handle *)handle;

	if (!edu_handle->ops || !edu_handle->ops->fault_isr) {
		return;
	}
	edu_handle->ops->fault_isr(handle);
}

void capi_edu_error_handler(void *handle)
{
	if (!handle) {
		return;
	}
	struct capi_edu_handle *edu_handle = (struct capi_edu_handle *)handle;

	if (!edu_handle->ops || !edu_handle->ops->error_handler) {
		return;
	}
	edu_handle->ops->error_handler(handle);
}
