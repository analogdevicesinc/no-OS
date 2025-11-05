/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_clk.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the CLK driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the CLK functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_clk.h>

int capi_clk_init(struct capi_clk_handle **handle, const struct capi_clk_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_clk_deinit(struct capi_clk_handle **handle)
{
	if (handle != NULL && *handle != NULL && (*handle)->ops != NULL &&
	    (*handle)->ops->deinit != NULL) {
		return (*handle)->ops->deinit(*handle);
	}

	return -EINVAL;
}

int capi_clk_enable(struct capi_clk_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable != NULL) {
		return handle->ops->enable(handle);
	}

	return -EINVAL;
}

int capi_clk_disable(struct capi_clk_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable != NULL) {
		return handle->ops->disable(handle);
	}

	return -EINVAL;
}

int capi_clk_get_rate(struct capi_clk_handle *handle, uint64_t *rate)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_rate != NULL) {
		return handle->ops->get_rate(handle, rate);
	}

	return -EINVAL;
}

int capi_clk_round_rate(struct capi_clk_handle *handle, uint64_t rate, uint64_t *rounded_rate)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->round_rate != NULL) {
		return handle->ops->round_rate(handle, rate, rounded_rate);
	}

	return -EINVAL;
}

int capi_clk_set_rate(struct capi_clk_handle *handle, uint64_t rate)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_rate != NULL) {
		return handle->ops->set_rate(handle, rate);
	}

	return -EINVAL;
}