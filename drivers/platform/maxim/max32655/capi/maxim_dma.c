/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_dma.c
 * @brief MAX32655 DMA CAPI implementation
 * @author Claude Code (noreply@anthropic.com)
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "maxim_dma.h"
#include "maxim_irq.h"
#include "capi_dma.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "dma.h"
#include "mxc_sys.h"
#include "mxc_errors.h"

extern struct capi_isr_table isr_table[200];

void DMA0_IRQHandler()
{
	if (isr_table[DMA0_IRQn].isr)
		isr_table[DMA0_IRQn].isr(isr_table[DMA0_IRQn].arg);
}

void DMA1_IRQHandler()
{
	if (isr_table[DMA1_IRQn].isr)
		isr_table[DMA1_IRQn].isr(isr_table[DMA1_IRQn].arg);
}

void DMA2_IRQHandler()
{
	if (isr_table[DMA2_IRQn].isr)
		isr_table[DMA2_IRQn].isr(isr_table[DMA2_IRQn].arg);
}

void DMA3_IRQHandler()
{
	if (isr_table[DMA3_IRQn].isr)
		isr_table[DMA3_IRQn].isr(isr_table[DMA3_IRQn].arg);
}

/**
 * @brief Convert CAPI DMA transfer type to Maxim DMA request
 * @param xfer_type CAPI transfer type
 * @return Maxim DMA request value
 */
static uint32_t capi_to_maxim_xfer_type(enum capi_dma_xfer_type xfer_type)
{
	switch (xfer_type) {
	case CAPI_DMA_MEM_TO_MEM:
		return MXC_V_DMA_CTRL_REQUEST_MEMTOMEM;
	case CAPI_DMA_MEM_TO_DEV:
	case CAPI_DMA_DEV_TO_MEM:
		/* These would need specific peripheral request IDs */
		return MXC_V_DMA_CTRL_REQUEST_MEMTOMEM;
	default:
		return MXC_V_DMA_CTRL_REQUEST_MEMTOMEM;
	}
}

/**
 * @brief DMA interrupt handler
 * @param handle DMA controller handle
 * @return 0 on success, negative error code otherwise
 */
static void maxim_capi_dma_isr(void *arg)
{
	struct maxim_capi_dma_handle *priv;
	struct maxim_capi_dma_chan_handle *chan_priv;
	struct capi_dma_handle *handle = arg;
	uint32_t intr_status;
	uint32_t i;

	if (!handle || !handle->extra)
		return;

	priv = (struct maxim_capi_dma_handle *)handle->extra;
	intr_status = priv->dma_regs->intfl;

	/* Check each channel for completion */
	for (i = 0; i < priv->num_channels; i++) {
		if (intr_status & NO_OS_BIT(i)) {
			chan_priv = &priv->channels[i];
			if (chan_priv->transfer_active && chan_priv->xfer_complete_cb) {
				chan_priv->transfer_active = false;
				chan_priv->xfer_complete_cb(chan_priv->current_xfer,
							    chan_priv->callback_ctx);
			}
		}
	}
}

/**
 * @brief DMA channel interrupt handler
 * @param chan DMA channel handle
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_isr_chan(void *arg)
{
	struct maxim_capi_dma_chan_handle *priv;
	struct capi_dma_chan *chan = arg;

	if (!chan || !chan->extra)
		return -EINVAL;

	priv = (struct maxim_capi_dma_chan_handle *)chan->extra;

	/* Clear the DMA interrupt flag */
	MXC_DMA->ch[chan->id].status |= NO_OS_BIT(2);

	if (priv->transfer_active && priv->xfer_complete_cb) {
		priv->transfer_active = false;
		priv->xfer_complete_cb(priv->current_xfer, priv->callback_ctx);
	}

	return 0;
}

/**
 * @brief Initialize DMA controller
 * @param handle Double pointer to DMA handle (allocated if *handle is NULL)
 * @param config DMA configuration
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_init(struct capi_dma_handle **handle,
			       const struct capi_dma_config *config)
{
	struct capi_dma_handle *dma_handle;
	struct maxim_capi_dma_handle *priv;
	uint32_t i;

	if (!handle || !config)
		return -EINVAL;

	if (config->num_chans > MAX_DMA_CHANNELS)
		return -EINVAL;

	/* Handle dynamic vs static allocation */
	if (!*handle) {
		/* Dynamic allocation */
		dma_handle = no_os_calloc(1, sizeof(struct capi_dma_handle));
		if (!dma_handle)
			return -ENOMEM;

		priv = no_os_calloc(1, sizeof(struct maxim_capi_dma_handle));
		if (!priv) {
			no_os_free(dma_handle);
			return -ENOMEM;
		}

		dma_handle->init_allocated = true;
		dma_handle->extra = priv;
	} else {
		/* Static allocation */
		dma_handle = *handle;
		priv = (struct maxim_capi_dma_handle *)dma_handle->extra;
		if (!priv)
			return -EINVAL;

		/* Clear the private structure */
		memset(priv, 0, sizeof(struct maxim_capi_dma_handle));
		dma_handle->init_allocated = false;
	}

	/* Allocate channel handles */
	priv->channels = no_os_calloc(config->num_chans,
				      sizeof(struct maxim_capi_dma_chan_handle));
	if (!priv->channels) {
		if (dma_handle->init_allocated) {
			no_os_free(priv);
			no_os_free(dma_handle);
		}
		return -ENOMEM;
	}

	/* Set up the handle */
	dma_handle->ops = &maxim_capi_dma_ops;

	/* Initialize private data */
	priv->controller_id = config->id;
	priv->num_channels = config->num_chans;
	priv->dma_regs = MXC_DMA;

	/* Enable DMA clock if not already enabled */
	if (!MXC_SYS_IsClockEnabled(MXC_SYS_PERIPH_CLOCK_DMA)) {
		MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_DMA);
		MXC_SYS_Reset_Periph(MAX_DMA_GCR_RST_POS);
	}

	/* Enable DMA controller - enable all channel interrupts */
	priv->dma_regs->inten = 0xFFFF;

	/* Initialize channel data */
	for (i = 0; i < config->num_chans; i++) {
		priv->channels[i].channel_id = i;
		priv->channels[i].dma_regs = priv->dma_regs;
		priv->channels[i].ch_regs = &priv->dma_regs->ch[i];
		priv->channels[i].transfer_active = false;
		priv->channels[i].irq_num = maxim_dma_get_irq(priv->controller_id, i);
	}

	priv->initialized = true;
	*handle = dma_handle;

	return 0;
}

/**
 * @brief Deinitialize DMA controller
 * @param handle DMA handle to deinitialize
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_deinit(struct capi_dma_handle *handle)
{
	struct maxim_capi_dma_handle *priv;

	if (!handle || !handle->extra)
		return -EINVAL;

	priv = (struct maxim_capi_dma_handle *)handle->extra;

	/* Disable DMA controller */
	if (priv->dma_regs)
		priv->dma_regs->inten = 0;

	/* Free channel handles */
	if (priv->channels)
		no_os_free(priv->channels);

	/* Free dynamically allocated memory */
	if (handle->init_allocated) {
		no_os_free(priv);
		no_os_free(handle);
	}

	return 0;
}

/**
 * @brief Initialize DMA channel
 * @param handle DMA controller handle
 * @param chan_ptr Double pointer to DMA channel (allocated if *chan_ptr is NULL)
 * @param id Channel ID
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_init_chan(struct capi_dma_handle *handle,
				    struct capi_dma_chan **chan_ptr,
				    uint32_t id)
{
	struct maxim_capi_dma_handle *dma_priv;
	struct capi_dma_chan *chan;
	struct maxim_capi_dma_chan_handle *chan_priv;

	if (!handle || !handle->extra || !chan_ptr)
		return -EINVAL;

	dma_priv = (struct maxim_capi_dma_handle *)handle->extra;

	if (id >= dma_priv->num_channels)
		return -EINVAL;

	/* Handle dynamic vs static allocation */
	if (!*chan_ptr) {
		/* Dynamic allocation */
		chan = no_os_calloc(1, sizeof(struct capi_dma_chan));
		if (!chan)
			return -ENOMEM;

		chan_priv = no_os_calloc(1, sizeof(struct maxim_capi_dma_chan_handle));
		if (!chan_priv) {
			no_os_free(chan);
			return -ENOMEM;
		}

		chan->owned_by_app = false;
		chan->extra = chan_priv;
	} else {
		/* Static allocation */
		chan = *chan_ptr;
		chan_priv = (struct maxim_capi_dma_chan_handle *)chan->extra;
		if (!chan_priv)
			return -EINVAL;

		/* Clear the private structure */
		memset(chan_priv, 0, sizeof(struct maxim_capi_dma_chan_handle));
		chan->owned_by_app = true;
	}

	/* Set up channel handle */
	chan->handle = handle;
	chan->id = id;
	chan->irq_num = maxim_dma_get_irq(dma_priv->controller_id, id);

	/* Initialize channel private data */
	*chan_priv = dma_priv->channels[id];

	*chan_ptr = chan;

	capi_irq_connect(MXC_DMA_CH_GET_IRQ(id), maxim_capi_dma_isr_chan, *chan_ptr);
	capi_irq_enable(MXC_DMA_CH_GET_IRQ(id));

	return 0;
}

/**
 * @brief Deinitialize DMA channel
 * @param chan DMA channel to deinitialize
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	struct maxim_capi_dma_chan_handle *priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	priv = (struct maxim_capi_dma_chan_handle *)chan->extra;

	/* Abort any ongoing transfer */
	if (priv->transfer_active && priv->ch_regs) {
		priv->ch_regs->ctrl = 0;  /* Disable channel */
		priv->transfer_active = false;
	}

	/* Free dynamically allocated memory */
	if (!chan->owned_by_app) {
		no_os_free(priv);
		no_os_free(chan);
	}

	capi_irq_enable(MXC_DMA_CH_GET_IRQ(chan->id));

	return 0;
}

/**
 * @brief Configure DMA transfer
 * @param chan DMA channel handle
 * @param xfer DMA transfer configuration
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_config_xfer(struct capi_dma_chan *chan,
				      struct capi_dma_transfer *xfer)
{
	struct maxim_capi_dma_chan_handle *priv;
	uint32_t cfg_reg = 0;
	uint32_t request_type;

	if (!chan || !chan->extra || !xfer)
		return -EINVAL;

	priv = (struct maxim_capi_dma_chan_handle *)chan->extra;

	if (priv->transfer_active)
		return -EBUSY;

	/* Store transfer configuration */
	priv->current_xfer = xfer;
	priv->xfer_complete_cb = xfer->xfer_complete_cb;
	priv->callback_ctx = xfer->user_data;

	/* Configure DMA channel registers */
	priv->ch_regs->src = (uint32_t)xfer->src;
	priv->ch_regs->dst = (uint32_t)xfer->dst;
	priv->ch_regs->cnt = xfer->length;

	/* Configure control register */
	request_type = capi_to_maxim_xfer_type(xfer->xfer_type);
	cfg_reg |= no_os_field_prep(MAX_DMA_REQSEL_MASK, request_type);

	/* Configure address increment */
	if (xfer->src_inc == CAPI_DMA_BYTE_INCREMENT)
		cfg_reg |= MAX_DMA_SRC_INC;

	if (xfer->dst_inc == CAPI_DMA_BYTE_INCREMENT)
		cfg_reg |= MAX_DMA_DST_INC;

	/* Enable interrupt on completion */
	cfg_reg |= MAX_DMA_IRQ_CTZ_EN;

	/* Enable channel */
	cfg_reg |= MAX_DMA_ENABLE;

	priv->ch_regs->ctrl = cfg_reg;

	/* Store callback information */
	chan->xfer_complete_cb = xfer->xfer_complete_cb;
	chan->xfer = xfer;

	return 0;
}

/**
 * @brief Start DMA transfer
 * @param chan DMA channel handle
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	struct maxim_capi_dma_chan_handle *priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	priv = (struct maxim_capi_dma_chan_handle *)chan->extra;

	if (!priv->current_xfer)
		return -EINVAL;

	priv->transfer_active = true;

	/* Start the transfer by enabling the channel (already configured) */
	/* The channel should already be configured and enabled from config_xfer */

	return 0;
}

/**
 * @brief Abort DMA transfer
 * @param chan DMA channel handle
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	struct maxim_capi_dma_chan_handle *priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	priv = (struct maxim_capi_dma_chan_handle *)chan->extra;

	if (!priv->transfer_active)
		return -EINVAL;

	/* Disable the channel to abort transfer */
	priv->ch_regs->ctrl = 0;
	priv->transfer_active = false;

	return 0;
}

/**
 * @brief Check if DMA channel transfer is completed
 * @param chan DMA channel handle
 * @return true if completed, false otherwise
 */
static bool maxim_capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	struct maxim_capi_dma_chan_handle *priv;

	if (!chan || !chan->extra)
		return true;  /* Assume completed if invalid */

	priv = (struct maxim_capi_dma_chan_handle *)chan->extra;

	return !priv->transfer_active;
}

/**
 * @brief MAX32655 DMA CAPI operations structure
 */
const struct capi_dma_ops maxim_capi_dma_ops = {
	.init = maxim_capi_dma_init,
	.deinit = maxim_capi_dma_deinit,
	.init_chan = maxim_capi_dma_init_chan,
	.deinit_chan = maxim_capi_dma_deinit_chan,
	.config_xfer = maxim_capi_dma_config_xfer,
	.xfer_start = maxim_capi_dma_xfer_start,
	.xfer_abort = maxim_capi_dma_xfer_abort,
	.chan_is_completed = maxim_capi_dma_chan_is_completed,
	.isr = maxim_capi_dma_isr,
	.isr_chan = maxim_capi_dma_isr_chan,
};