/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_fftacc.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the FFTACC driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the FFTACC functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_fftacc.h>

int capi_fftacc_init(struct capi_fftacc_handle **handle, const struct capi_fftacc_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_fftacc_deinit(const struct capi_fftacc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_fftacc_get_status(const struct capi_fftacc_handle *handle, uint32_t *stat)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_status != NULL) {
		return handle->ops->get_status(handle, stat);
	}

	return -EINVAL;
}

int capi_fftacc_chan_init(const struct capi_fftacc_handle *handle, uint8_t idx,
			  const struct capi_fftacc_chan_config *config,
			  const struct capi_fftacc_chan_wola_config *wola_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_init != NULL) {
		return handle->ops->chan_init(handle, idx, config, wola_config);
	}

	return -EINVAL;
}

int capi_fftacc_sw_chan_start(const struct capi_fftacc_handle *handle, uint8_t idx)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->sw_chan_start != NULL) {
		return handle->ops->sw_chan_start(handle, idx);
	}

	return -EINVAL;
}

int capi_fftacc_chan_start(const struct capi_fftacc_handle *handle, uint8_t idx)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_start != NULL) {
		return handle->ops->chan_start(handle, idx);
	}

	return -EINVAL;
}

int capi_fftacc_get_chan_op_done_stat(const struct capi_fftacc_handle *handle, uint8_t idx,
				      bool *op_done)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_chan_op_done_stat != NULL) {
		return handle->ops->get_chan_op_done_stat(handle, idx, op_done);
	}

	return -EINVAL;
}

int capi_fftacc_clear_chan_op_done_stat(struct capi_fftacc_handle *handle, uint8_t idx)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->clear_chan_op_done_stat != NULL) {
		return handle->ops->clear_chan_op_done_stat(handle, idx);
	}

	return -EINVAL;
}

int capi_fftacc_chan_deinit(struct capi_fftacc_handle *handle, uint8_t idx)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_deinit != NULL) {
		return handle->ops->chan_deinit(handle, idx);
	}

	return -EINVAL;
}