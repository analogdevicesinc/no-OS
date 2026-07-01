/*
 * Copyright (c) 2024-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_trng.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the TRNG driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the TRNG functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_trng.h>

int capi_trng_init(struct capi_trng_handle **handle,
		   const struct capi_trng_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_trng_deinit(struct capi_trng_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_trng_generate_u32(struct capi_trng_handle *handle, uint32_t *value)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->generate_u32 != NULL) {
		return handle->ops->generate_u32(handle, value);
	}

	return -EINVAL;
}

int capi_trng_fill_buffer(struct capi_trng_handle *handle, uint8_t *buffer,
			  uint32_t length)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->fill_buffer != NULL) {
		return handle->ops->fill_buffer(handle, buffer, length);
	}

	return -EINVAL;
}

int capi_trng_fill_buffer_async(struct capi_trng_handle *handle,
				uint8_t *buffer, uint32_t length)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->fill_buffer_async != NULL) {
		return handle->ops->fill_buffer_async(handle, buffer, length);
	}

	return -EINVAL;
}

int capi_trng_register_callback(struct capi_trng_handle *handle,
				capi_trng_callback_t callback, void *callback_arg)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->register_callback != NULL) {
		return handle->ops->register_callback(handle, callback, callback_arg);
	}

	return -EINVAL;
}

int capi_trng_health_test(struct capi_trng_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->health_test != NULL) {
		return handle->ops->health_test(handle);
	}

	return -EINVAL;
}

void capi_trng_isr(void *handle)
{
	if (!handle)
		return;

	struct capi_trng_handle *trng_handle = (struct capi_trng_handle *)handle;

	if (!trng_handle->ops && !trng_handle->ops->isr)
		return;

	trng_handle->ops->isr(handle);
}
