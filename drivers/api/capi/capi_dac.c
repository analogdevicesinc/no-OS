/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_dac.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the DAC driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the DAC functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_dac.h>

int capi_dac_init(struct capi_dac_handle **handle, struct capi_dac_config *config)
{
	if (config != NULL && config->dac_ops != NULL && config->dac_ops->init != NULL) {
		return config->dac_ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_dac_deinit(struct capi_dac_handle *handle)
{
	if (handle != NULL && handle->dac_ops != NULL && handle->dac_ops->deinit != NULL) {
		return handle->dac_ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_dac_power_on(struct capi_dac_handle *handle)
{
	if (handle != NULL && handle->dac_ops != NULL && handle->dac_ops->power_on != NULL) {
		return handle->dac_ops->power_on(handle);
	}

	return -EINVAL;
}

int capi_dac_power_off(struct capi_dac_handle *handle)
{
	if (handle != NULL && handle->dac_ops != NULL && handle->dac_ops->power_off != NULL) {
		return handle->dac_ops->power_off(handle);
	}

	return -EINVAL;
}

int capi_dac_set_vref_source(struct capi_dac_handle *handle, enum capi_dac_vref_src src)
{
	if (handle != NULL && handle->dac_ops != NULL && handle->dac_ops->set_vref_source != NULL) {
		return handle->dac_ops->set_vref_source(handle, src);
	}

	return -EINVAL;
}

int capi_dac_get_vref_source(struct capi_dac_handle *handle, enum capi_dac_vref_src *src)
{
	if (handle != NULL && handle->dac_ops != NULL && handle->dac_ops->get_vref_source != NULL) {
		return handle->dac_ops->get_vref_source(handle, src);
	}

	return -EINVAL;
}

int capi_dac_set_calibration(struct capi_dac_handle *handle, uint8_t *cal_data)
{
	if (handle != NULL && handle->dac_ops != NULL && handle->dac_ops->set_calibration != NULL) {
		return handle->dac_ops->set_calibration(handle, cal_data);
	}

	return -EINVAL;
}

int capi_dac_chan_init(struct capi_dac_chan_handle **handle,
		       const struct capi_dac_chan_config *config)
{
	if (config != NULL && config->dac_handle != NULL && config->dac_handle->dac_ops != NULL &&
	    config->dac_handle->dac_ops->chan_init != NULL) {
		return config->dac_handle->dac_ops->chan_init(handle, config);
	}

	return -EINVAL;
}

int capi_dac_chan_deinit(struct capi_dac_chan_handle *handle)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_deinit != NULL) {
		return handle->dac_handle->dac_ops->chan_deinit(handle);
	}

	return -EINVAL;
}

int capi_dac_chan_set_offset(struct capi_dac_chan_handle *handle, uint32_t offset)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_set_offset != NULL) {
		return handle->dac_handle->dac_ops->chan_set_offset(handle, offset);
	}

	return -EINVAL;
}

int capi_dac_chan_get_offset(struct capi_dac_chan_handle *handle, uint32_t *offset)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_get_offset != NULL) {
		return handle->dac_handle->dac_ops->chan_get_offset(handle, offset);
	}

	return -EINVAL;
}

int capi_dac_chan_set_attenuation(struct capi_dac_chan_handle *handle, uint32_t attenuation)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_set_attenuation != NULL) {
		return handle->dac_handle->dac_ops->chan_set_attenuation(handle, attenuation);
	}

	return -EINVAL;
}

int capi_dac_chan_get_attenuation(struct capi_dac_chan_handle *handle, uint32_t *attenuation)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_get_attenuation != NULL) {
		return handle->dac_handle->dac_ops->chan_get_attenuation(handle, attenuation);
	}

	return -EINVAL;
}

int capi_dac_chan_write(struct capi_dac_chan_handle *handle, void *data, int wr_cnt)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_write != NULL) {
		return handle->dac_handle->dac_ops->chan_write(handle, data, wr_cnt);
	}

	return -EINVAL;
}

int capi_dac_chan_stream_enable(struct capi_dac_chan_handle *handle)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_stream_enable != NULL) {
		return handle->dac_handle->dac_ops->chan_stream_enable(handle);
	}

	return -EINVAL;
}

int capi_dac_chan_stream_disable(struct capi_dac_chan_handle *handle)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->chan_stream_disable != NULL) {
		return handle->dac_handle->dac_ops->chan_stream_disable(handle);
	}

	return -EINVAL;
}

int capi_dac_attach_callback(struct capi_dac_chan_handle *handle, capi_dac_callback_t cb)
{
	if (handle != NULL && handle->dac_handle != NULL && handle->dac_handle->dac_ops != NULL &&
	    handle->dac_handle->dac_ops->capi_dac_attach_callback != NULL) {
		return handle->dac_handle->dac_ops->capi_dac_attach_callback(handle, cb);
	}

	return -EINVAL;
}