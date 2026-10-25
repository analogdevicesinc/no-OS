/*******************************************************************************
 *   @file   maxim_capi_dma.c
 *   @brief  Implementation of the MAX32650/MAX32690 CAPI DMA functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include "capi_alloc.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_dma_priv.h"
#include "mxc_device.h"
#include "mxc_errors.h"
#include "nvic_table.h"

#define MAX_DMA_STATUS_FLAGS	(MXC_F_DMA_REVA_STATUS_CTZ_IF |		\
				 MXC_F_DMA_REVA_STATUS_RLD_IF |		\
				 MXC_F_DMA_REVA_STATUS_BUS_ERR |	\
				 MXC_F_DMA_REVA_STATUS_TO_IF)
#define MAX_DMA_STATUS_ERRORS	(MXC_F_DMA_REVA_STATUS_BUS_ERR |	\
				 MXC_F_DMA_REVA_STATUS_TO_IF)
#define MAX_DMA_CTRL_IE		(MXC_F_DMA_REVA_CTRL_CTZ_IE |		\
				 MXC_F_DMA_REVA_CTRL_DIS_IE)

static struct capi_dma_handle *dma_handle_singleton;
static struct max_capi_dma_ch_priv ch_pool[MXC_DMA_CHANNELS];
static void (*ch_prev_vector[MXC_DMA_CHANNELS])(void);

static inline mxc_dma_reva_regs_t *max_dma_regs(void)
{
	return (mxc_dma_reva_regs_t *)MXC_DMA;
}

static inline mxc_dma_reva_ch_regs_t *max_dma_ch_regs(int hw_ch)
{
	return (mxc_dma_reva_ch_regs_t *)((uintptr_t)MXC_DMA +
					  offsetof(mxc_dma_reva_regs_t, ch) +
					  hw_ch * sizeof(mxc_dma_reva_ch_regs_t));
}

/**
 * @brief Hardware channel of a DMA channel IRQ.
 *
 * DMA0..DMA3 and DMA4..DMA15 are two contiguous IRQ ranges.
 */
static inline int max_dma_irq_to_ch(uint32_t irq)
{
	if (irq >= (uint32_t)DMA4_IRQn)
		return irq - DMA4_IRQn + 4;

	return irq - DMA0_IRQn;
}

/**
 * @brief Channel interrupt handler.
 * @param ch_priv - Channel private data.
 */
static void max_dma_ch_isr(struct max_capi_dma_ch_priv *ch_priv)
{
	mxc_dma_reva_ch_regs_t *regs = ch_priv->regs;
	struct capi_dma_chan *chan = ch_priv->capi_chan;
	uint32_t status = regs->status;

	regs->status = status & MAX_DMA_STATUS_FLAGS;

	if (status & MAX_DMA_STATUS_ERRORS) {
		regs->ctrl &= ~(MXC_F_DMA_REVA_CTRL_EN | MAX_DMA_CTRL_IE);
		ch_priv->completed = true;
		if (chan->error_cb)
			chan->error_cb(status, chan->error_ctx);
		return;
	}

	if (status & MXC_F_DMA_REVA_STATUS_CTZ_IF) {
		ch_priv->completed = true;
		if (chan->xfer_complete_cb)
			chan->xfer_complete_cb(0, chan->xfer_complete_ctx);
	}
}

static void max_dma_irq_handler(void)
{
	uint32_t irq = (__get_IPSR() & 0x1FFU) - 16U;
	struct max_capi_dma_ch_priv *ch_priv = &ch_pool[max_dma_irq_to_ch(irq)];
	struct max_capi_dma_priv *dma_priv;

	if (!ch_priv->capi_chan)
		return;

	max_dma_ch_isr(ch_priv);

	dma_priv = ch_priv->capi_chan->handle->priv;
	if (dma_priv->irq_notify)
		dma_priv->irq_notify(dma_priv->irq_notify_ctx);
}

/**
 * @brief Initialize the DMA controller.
 * @param handle - Pointer to store the DMA handle.
 * @param config - DMA controller configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_init(struct capi_dma_handle **handle,
			     const struct capi_dma_config *config)
{
	const struct max_capi_dma_extra *extra;
	struct capi_dma_handle *dma_handle;
	struct max_capi_dma_priv *dma_priv;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	if (config->num_chans > MXC_DMA_CHANNELS)
		return -EINVAL;

	if (dma_handle_singleton) {
		*handle = dma_handle_singleton;
		return 0;
	}

	/* The no-OS DMA driver may have initialized the controller already */
	ret = MXC_DMA_Init();
	if (ret != E_NO_ERROR && ret != E_BAD_STATE)
		return -EIO;

	if (!*handle) {
		dma_handle = capi_calloc(1, sizeof(*dma_handle));
		if (!dma_handle)
			return -ENOMEM;

		dma_priv = capi_calloc(1, sizeof(*dma_priv));
		if (!dma_priv) {
			capi_free(dma_handle);
			return -ENOMEM;
		}

		dma_handle->priv = dma_priv;
		dma_handle->init_allocated = true;
	} else {
		dma_handle = *handle;
		if (!dma_handle->priv)
			return -EINVAL;

		dma_priv = dma_handle->priv;
		memset(dma_priv, 0, sizeof(*dma_priv));
		dma_handle->init_allocated = false;
	}

	extra = config->extra;
	dma_priv->num_chans = config->num_chans ? config->num_chans :
			      MXC_DMA_CHANNELS;
	dma_priv->use_irq = !(extra && extra->no_irq);
	dma_priv->irq_priority = extra ? extra->irq_priority : 0;
	dma_priv->irq_notify = extra ? extra->irq_notify : NULL;
	dma_priv->irq_notify_ctx = extra ? extra->irq_notify_ctx : NULL;
	dma_handle->ops = config->ops;

	dma_handle_singleton = dma_handle;
	*handle = dma_handle;

	return 0;
}

static int max_capi_dma_deinit_chan(struct capi_dma_chan *chan);

/**
 * @brief Release every channel and the DMA handle.
 * @param handle - DMA handle.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_deinit(struct capi_dma_handle *handle)
{
	struct max_capi_dma_priv *dma_priv;
	uint32_t i;

	if (!handle || !handle->priv)
		return -EINVAL;

	dma_priv = handle->priv;
	for (i = 0; i < dma_priv->num_chans; i++)
		if (dma_priv->chans[i])
			max_capi_dma_deinit_chan(dma_priv->chans[i]->capi_chan);

	if (handle->init_allocated) {
		capi_free(dma_priv);
		capi_free(handle);
	}

	dma_handle_singleton = NULL;

	return 0;
}

/**
 * @brief Acquire a hardware channel and bind it to a CAPI channel ID.
 * @param handle - DMA handle.
 * @param chan_ptr - Pointer to store the channel.
 * @param id - CAPI channel ID, lower than the configured number of channels.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_init_chan(struct capi_dma_handle *handle,
				  struct capi_dma_chan **chan_ptr,
				  uint32_t id)
{
	struct max_capi_dma_ch_priv *ch_priv;
	struct max_capi_dma_priv *dma_priv;
	struct capi_dma_chan *chan;
	IRQn_Type irq;
	int hw_ch;

	if (!handle || !handle->priv || !chan_ptr)
		return -EINVAL;

	dma_priv = handle->priv;
	if (id >= dma_priv->num_chans)
		return -EINVAL;

	if (dma_priv->chans[id])
		return -EBUSY;

	hw_ch = MXC_DMA_AcquireChannel();
	if (hw_ch < 0)
		return -EBUSY;

	ch_priv = &ch_pool[hw_ch];
	memset(ch_priv, 0, sizeof(*ch_priv));
	ch_priv->hw_ch = hw_ch;
	ch_priv->regs = max_dma_ch_regs(hw_ch);
	ch_priv->completed = true;

	if (*chan_ptr) {
		chan = *chan_ptr;
		chan->owned_by_app = true;
	} else {
		chan = &ch_priv->chan_storage;
		chan->owned_by_app = false;
	}

	chan->handle = handle;
	chan->id = id;
	chan->extra = ch_priv;
	chan->xfer = NULL;
	chan->xfer_complete_cb = NULL;
	chan->error_cb = NULL;
	ch_priv->capi_chan = chan;

	irq = MXC_DMA_CH_GET_IRQ(hw_ch);
	chan->irq_num = irq;

	if (dma_priv->use_irq) {
		ch_prev_vector[hw_ch] = ((void (**)(void))SCB->VTOR)[irq + 16];
		MXC_NVIC_SetVector(irq, max_dma_irq_handler);
		NVIC_SetPriority(irq, dma_priv->irq_priority);
		NVIC_ClearPendingIRQ(irq);
		NVIC_EnableIRQ(irq);
		max_dma_regs()->inten |= (1U << hw_ch);
	}

	dma_priv->chans[id] = ch_priv;
	*chan_ptr = chan;

	return 0;
}

/**
 * @brief Stop a channel and give its hardware channel back.
 * @param chan - DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	struct max_capi_dma_ch_priv *ch_priv;
	struct max_capi_dma_priv *dma_priv;
	IRQn_Type irq;
	int hw_ch;

	if (!chan || !chan->handle || !chan->extra)
		return -EINVAL;

	ch_priv = chan->extra;
	dma_priv = chan->handle->priv;
	hw_ch = ch_priv->hw_ch;

	ch_priv->regs->ctrl = 0;
	ch_priv->regs->status = MAX_DMA_STATUS_FLAGS;

	if (dma_priv->use_irq) {
		irq = MXC_DMA_CH_GET_IRQ(hw_ch);
		max_dma_regs()->inten &= ~(1U << hw_ch);
		NVIC_DisableIRQ(irq);
		MXC_NVIC_SetVector(irq, ch_prev_vector[hw_ch]);
	}

	MXC_DMA_ReleaseChannel(hw_ch);

	if (chan->id < dma_priv->num_chans)
		dma_priv->chans[chan->id] = NULL;

	chan->extra = NULL;
	memset(ch_priv, 0, sizeof(*ch_priv));

	return 0;
}

/**
 * @brief Set up the next transfer of a channel.
 *
 * CTRL is only rebuilt when the transfer shape (direction, request, widths,
 * increments) changes; otherwise only the addresses and the length are kept
 * for xfer_start().
 *
 * @param chan - DMA channel.
 * @param xfer - Transfer descriptor, extra is a struct max_capi_dma_xfer_extra.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_config_xfer(struct capi_dma_chan *chan,
				    struct capi_dma_transfer *xfer)
{
	const struct max_capi_dma_xfer_extra *xfer_extra;
	struct max_capi_dma_ch_priv *ch_priv;
	mxc_dma_reqsel_t reqsel;
	uint32_t ctrl;

	if (!chan || !chan->extra || !xfer || !xfer->length)
		return -EINVAL;

	ch_priv = chan->extra;
	xfer_extra = xfer->extra;
	reqsel = xfer_extra ? xfer_extra->reqsel : MXC_DMA_REQUEST_MEMTOMEM;

	if (ch_priv->cfg_valid && xfer->xfer_type == ch_priv->cfg_type &&
	    xfer->src_inc == ch_priv->cfg_src_inc &&
	    xfer->dst_inc == ch_priv->cfg_dst_inc &&
	    xfer->src_size == ch_priv->cfg_src_size &&
	    xfer->dst_size == ch_priv->cfg_dst_size &&
	    reqsel == ch_priv->cfg_reqsel)
		goto set_addr;

	if (xfer->src_size > CAPI_DMA_XFER_SIZE_4_BYTES ||
	    xfer->dst_size > CAPI_DMA_XFER_SIZE_4_BYTES)
		return -EINVAL;

	if ((xfer->xfer_type == CAPI_DMA_MEM_TO_MEM) !=
	    (reqsel == MXC_DMA_REQUEST_MEMTOMEM))
		return -EINVAL;

	ctrl = (uint32_t)reqsel |
	       (xfer->src_size << MXC_F_DMA_REVA_CTRL_SRCWD_POS) |
	       (xfer->dst_size << MXC_F_DMA_REVA_CTRL_DSTWD_POS);
	if (xfer->src_inc != CAPI_DMA_NO_INCREMENT)
		ctrl |= MXC_F_DMA_REVA_CTRL_SRCINC;
	if (xfer->dst_inc != CAPI_DMA_NO_INCREMENT)
		ctrl |= MXC_F_DMA_REVA_CTRL_DSTINC;
	if (xfer->xfer_type == CAPI_DMA_MEM_TO_MEM)
		ctrl |= MXC_F_DMA_REVA_CTRL_BURST_SIZE;

	ch_priv->ctrl = ctrl;
	ch_priv->cfg_valid = true;
	ch_priv->cfg_type = xfer->xfer_type;
	ch_priv->cfg_src_inc = xfer->src_inc;
	ch_priv->cfg_dst_inc = xfer->dst_inc;
	ch_priv->cfg_src_size = xfer->src_size;
	ch_priv->cfg_dst_size = xfer->dst_size;
	ch_priv->cfg_reqsel = reqsel;

set_addr:
	ch_priv->src = (uint32_t)xfer->src;
	ch_priv->dst = (uint32_t)xfer->dst;
	ch_priv->cnt = xfer->length;
	chan->xfer = xfer;

	return 0;
}

/**
 * @brief Start the configured transfer.
 *
 * Interrupts are only armed for the events that have a callback.
 *
 * @param chan - DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	struct max_capi_dma_ch_priv *ch_priv;
	mxc_dma_reva_ch_regs_t *regs;
	uint32_t ie = 0;

	if (!chan || !chan->extra)
		return -EINVAL;

	ch_priv = chan->extra;
	if (!ch_priv->cfg_valid)
		return -EINVAL;

	if (chan->xfer_complete_cb)
		ie |= MXC_F_DMA_REVA_CTRL_CTZ_IE;
	if (chan->error_cb)
		ie |= MXC_F_DMA_REVA_CTRL_DIS_IE;

	regs = ch_priv->regs;
	ch_priv->completed = false;
	regs->ctrl = ch_priv->ctrl;
	regs->status = MAX_DMA_STATUS_FLAGS;
	regs->src = ch_priv->src;
	regs->dst = ch_priv->dst;
	regs->cnt = ch_priv->cnt;
	regs->ctrl = ch_priv->ctrl | ie | MXC_F_DMA_REVA_CTRL_EN;

	return 0;
}

/**
 * @brief Stop a channel.
 * @param chan - DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	struct max_capi_dma_ch_priv *ch_priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	ch_priv = chan->extra;
	ch_priv->regs->ctrl &= ~(MXC_F_DMA_REVA_CTRL_EN | MAX_DMA_CTRL_IE);
	while (ch_priv->regs->status & MXC_F_DMA_REVA_STATUS_STATUS)
		;
	ch_priv->regs->status = MAX_DMA_STATUS_FLAGS;
	ch_priv->completed = true;

	return 0;
}

/**
 * @brief Check whether the last transfer of a channel has ended.
 * @param chan - DMA channel.
 * @return true if the channel is idle.
 */
static bool max_capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	struct max_capi_dma_ch_priv *ch_priv;

	if (!chan || !chan->extra)
		return true;

	ch_priv = chan->extra;
	if (!ch_priv->completed &&
	    (ch_priv->regs->status & (MXC_F_DMA_REVA_STATUS_CTZ_IF |
				      MAX_DMA_STATUS_ERRORS)))
		ch_priv->completed = true;

	return ch_priv->completed;
}

/**
 * @brief Service every channel of the controller.
 * @param handle - DMA handle.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_isr(struct capi_dma_handle *handle)
{
	struct max_capi_dma_priv *dma_priv;
	uint32_t i;

	if (!handle || !handle->priv)
		return -EINVAL;

	dma_priv = handle->priv;
	for (i = 0; i < dma_priv->num_chans; i++)
		if (dma_priv->chans[i])
			max_dma_ch_isr(dma_priv->chans[i]);

	return 0;
}

/**
 * @brief Service one channel.
 * @param chan - DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_isr_chan(struct capi_dma_chan *chan)
{
	if (!chan || !chan->extra)
		return -EINVAL;

	max_dma_ch_isr(chan->extra);

	return 0;
}

static int max_capi_dma_register_complete_callback(struct capi_dma_chan *chan,
		capi_dma_xfer_complete_cb callback,
		void *xfer_complete_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->xfer_complete_cb = callback;
	chan->xfer_complete_ctx = xfer_complete_ctx;

	return 0;
}

static int max_capi_dma_register_error_callback(struct capi_dma_chan *chan,
		capi_dma_error_cb callback,
		void *error_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->error_cb = callback;
	chan->error_ctx = error_ctx;

	return 0;
}

const struct capi_dma_ops max_capi_dma_ops = {
	.init = max_capi_dma_init,
	.deinit = max_capi_dma_deinit,
	.init_chan = max_capi_dma_init_chan,
	.deinit_chan = max_capi_dma_deinit_chan,
	.config_xfer = max_capi_dma_config_xfer,
	.xfer_start = max_capi_dma_xfer_start,
	.xfer_abort = max_capi_dma_xfer_abort,
	.chan_is_completed = max_capi_dma_chan_is_completed,
	.isr = max_capi_dma_isr,
	.isr_chan = max_capi_dma_isr_chan,
	.register_complete_callback = max_capi_dma_register_complete_callback,
	.register_error_callback = max_capi_dma_register_error_callback,
};
