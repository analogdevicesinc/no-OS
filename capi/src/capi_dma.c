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

int capi_dma_init(struct capi_dma_handle **handle,
		  const struct capi_dma_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_dma_deinit(struct capi_dma_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_dma_init_chan(struct capi_dma_handle *handle,
		       struct capi_dma_chan **chan_ptr, uint32_t id)
{
	if (!handle || !handle->ops || !handle->ops->init_chan) {
		return -EINVAL;
	}
	return handle->ops->init_chan(handle, chan_ptr, id);
}

int capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	if (!chan || !chan->handle || !chan->handle->ops
	    || !chan->handle->ops->deinit_chan) {
		return -EINVAL;
	}
	return chan->handle->ops->deinit_chan(chan);
}

int capi_dma_config_xfer(struct capi_dma_chan *chan,
			 struct capi_dma_transfer *xfer)
{
	if (!chan || !chan->handle || !chan->handle->ops
	    || !chan->handle->ops->config_xfer) {
		return -EINVAL;
	}
	return chan->handle->ops->config_xfer(chan, xfer);
}

int capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	if (!chan || !chan->handle || !chan->handle->ops
	    || !chan->handle->ops->xfer_start) {
		return -EINVAL;
	}
	return chan->handle->ops->xfer_start(chan);
}

int capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	if (!chan || !chan->handle || !chan->handle->ops
	    || !chan->handle->ops->xfer_abort) {
		return -EINVAL;
	}
	return chan->handle->ops->xfer_abort(chan);
}

bool capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	if (!chan || !chan->handle || !chan->handle->ops
	    || !chan->handle->ops->chan_is_completed) {
		return false;
	}
	return chan->handle->ops->chan_is_completed(chan);
}

int capi_dma_isr(struct capi_dma_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->isr) {
		return -EINVAL;
	}
	return handle->ops->isr(handle);
}

int capi_dma_isr_chan(struct capi_dma_chan *chan)
{
	if (!chan || !chan->handle || !chan->handle->ops
	    || !chan->handle->ops->isr_chan) {
		return -EINVAL;
	}
	return chan->handle->ops->isr_chan(chan);
}

int capi_dma_register_complete_callback(struct capi_dma_chan *chan,
					capi_dma_xfer_complete_cb callback, void *xfer_complete_ctx)
{
	if (!chan || !chan->handle || !chan->handle->ops ||
	    !chan->handle->ops->register_complete_callback) {
		return -EINVAL;
	}

	return chan->handle->ops->register_complete_callback(chan, callback,
			xfer_complete_ctx);
}

int capi_dma_register_error_callback(struct capi_dma_chan *chan,
				     capi_dma_error_cb callback,
				     void *error_ctx)
{
	if (!chan || !chan->handle || !chan->handle->ops ||
	    !chan->handle->ops->register_error_callback) {
		return -EINVAL;
	}

	return chan->handle->ops->register_error_callback(chan, callback, error_ctx);
}

int capi_dma_config_scattergather_xfer(struct capi_dma_chan *chan,
				       const struct capi_dma_sg_config *sg_config,
				       struct capi_dma_transfer *block, uint32_t index)
{
	if (!chan || !chan->handle || !chan->handle->ops ||
	    !chan->handle->ops->config_scattergather_xfer || !sg_config || !block) {
		return -EINVAL;
	}
	return chan->handle->ops->config_scattergather_xfer(chan, sg_config, block,
			index);
}

int capi_dma_start_scattergather_xfer(struct capi_dma_chan *chan,
				      const struct capi_dma_sg_config *sg_config)
{
	if (!chan || !chan->handle || !chan->handle->ops ||
	    !chan->handle->ops->start_scattergather_xfer) {
		return -EINVAL;
	}
	return chan->handle->ops->start_scattergather_xfer(chan, sg_config);
}
