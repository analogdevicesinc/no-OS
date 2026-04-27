/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_dma.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the DMA driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the DMA functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_dma.h>

int capi_dma_init(struct capi_dma_handle **handle, const struct capi_dma_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_dma_deinit(struct capi_dma_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_dma_init_chan(struct capi_dma_handle *handle, struct capi_dma_chan **chan_ptr, uint32_t id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->init_chan != NULL) {
		return handle->ops->init_chan(handle, chan_ptr, id);
	}

	return -EINVAL;
}

int capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	if (chan != NULL && chan->handle != NULL && chan->handle->ops != NULL &&
	    chan->handle->ops->deinit_chan != NULL) {
		return chan->handle->ops->deinit_chan(chan);
	}

	return -EINVAL;
}

int capi_dma_config_xfer(struct capi_dma_chan *chan, struct capi_dma_transfer *xfer)
{
	if (chan != NULL && chan->handle != NULL && chan->handle->ops != NULL &&
	    chan->handle->ops->config_xfer != NULL) {
		return chan->handle->ops->config_xfer(chan, xfer);
	}

	return -EINVAL;
}

int capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	if (chan != NULL && chan->handle != NULL && chan->handle->ops != NULL &&
	    chan->handle->ops->xfer_start != NULL) {
		return chan->handle->ops->xfer_start(chan);
	}

	return -EINVAL;
}

int capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	if (chan != NULL && chan->handle != NULL && chan->handle->ops != NULL &&
	    chan->handle->ops->xfer_abort != NULL) {
		return chan->handle->ops->xfer_abort(chan);
	}

	return -EINVAL;
}

bool capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	if (chan != NULL && chan->handle != NULL && chan->handle->ops != NULL &&
	    chan->handle->ops->chan_is_completed != NULL) {
		return chan->handle->ops->chan_is_completed(chan);
	}

	return false;
}

void capi_dma_isr(struct capi_dma_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->isr != NULL) {
		handle->ops->isr(handle);
	}
}

void capi_dma_isr_chan(struct capi_dma_chan *chan)
{
	if (chan != NULL && chan->handle != NULL && chan->handle->ops != NULL &&
	    chan->handle->ops->isr_chan != NULL) {
		chan->handle->ops->isr_chan(chan);
	}
}