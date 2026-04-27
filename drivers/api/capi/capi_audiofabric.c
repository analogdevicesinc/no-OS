/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_audiofabric.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the AudioFabric driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the AudioFabric functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_audiofabric.h>

int capi_audiofabric_init(struct capi_audiofabric_handle **handle,
			  const struct capi_audiofabric_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config->identifier);
	}

	return -EINVAL;
}

int capi_audiofabric_deinit(const struct capi_audiofabric_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_audiofabric_set_route(const struct capi_audiofabric_handle *handle,
			       const struct capi_audiofabric_src_chan_config *src_config,
			       const struct capi_audiofabric_sink_chan_config *sink_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_route != NULL) {
		return handle->ops->set_route(handle, src_config, sink_config);
	}

	return -EINVAL;
}