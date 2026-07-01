/*******************************************************************************
 *   @file   maxim_capi_dma.c
 *   @brief  Implementation of DMA functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <stdint.h>
#include <errno.h>
#include "dma.h"
#include "spc.h"
#include "max32657.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_irq.h"
#include "capi_irq.h"

static struct capi_dma_handle *dma;

/** Forward declaration *******************************************************/

static void max_capi_dma_isr_callback(void *ctx);

/** Helper functions **********************************************************/

/**
 * @brief Convert CAPI DMA transfer size to msdk DMA data transfer width
 */
static int _max_capi_dma_get_width(enum capi_dma_xfer_size size,
				   mxc_dma_width_t *width)
{
	switch (size) {
	case CAPI_DMA_XFER_SIZE_1_BYTE:
		*width = MXC_DMA_WIDTH_BYTE;
		break;
	case CAPI_DMA_XFER_SIZE_2_BYTES:
		*width = MXC_DMA_WIDTH_HALFWORD;
		break;
	case CAPI_DMA_XFER_SIZE_4_BYTES:
		*width = MXC_DMA_WIDTH_WORD;
		break;
	case CAPI_DMA_XFER_SIZE_8_BYTES:
	case CAPI_DMA_XFER_SIZE_16_BYTES:
	case CAPI_DMA_XFER_SIZE_32_BYTES:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	return 0;
}

/** DMA functions implementation **********************************************/

/**
 * @brief Initialize the DMA
 * @param handle The DMA handle
 * @param config The config struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_init(struct capi_dma_handle **handle,
		      const struct capi_dma_config *config)
{
	int ret;
	struct capi_dma_handle *dma_handle;
	struct max_capi_dma_priv *dma_priv;
	struct capi_irq_config irq_config;
	uint8_t num_channels = MXC_DMA_CHANNELS;

	if (!handle || !config)
		return -EINVAL;

	if (config->num_chans > MXC_DMA_CHANNELS)
		return -EINVAL;

	if (dma) {
		*handle = dma;
		return 0;
	}

	if (config->num_chans > 0)
		num_channels = config->num_chans;

	if (*handle == NULL) {
		dma_handle = capi_calloc(1, sizeof(*dma_handle));
		if (!dma_handle)
			return -ENOMEM;
		dma_handle->init_allocated = true;
	} else {
		dma_handle = *handle;
		dma_handle->init_allocated = false;
	}

	dma_priv = capi_calloc(1, sizeof(*dma_priv));
	if (!dma_priv) {
		ret = -ENOMEM;
		goto free_handle;
	}
	dma_priv->channels = capi_calloc(num_channels,
					 sizeof(struct capi_dma_chan *));
	if (!dma_priv->channels) {
		ret = -ENOMEM;
		goto free_priv;
	}

	ret = MXC_DMA_Init(MXC_DMA1_S);
	if (ret != E_NO_ERROR) {
		ret = -EIO;
		goto free_channels;
	}

	dma_priv->id = config->id;
	dma_handle->ops = config->ops;
	dma_handle->priv = dma_priv;

	irq_config = (struct capi_irq_config) {
		.irq_ctrl_id = 0,
	};
	ret = capi_irq_init(&irq_config);
	if (ret)
		goto deinit_dma;

	dma = dma_handle;
	*handle = dma_handle;

	return 0;
deinit_dma:
	MXC_DMA_DeInit(MXC_DMA1_S);
	MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_DMA1);
free_channels:
	capi_free(dma_priv->channels);
free_priv:
	capi_free(dma_priv);
free_handle:
	if (dma_handle->init_allocated)
		capi_free(dma_handle);

	dma = NULL;

	return ret;
}

/**
 * @brief Deinitialize the DMA
 * @param handle The DMA handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_deinit(struct capi_dma_handle *handle)
{
	struct max_capi_dma_priv *dma_priv;
	uint8_t i;
	IRQn_Type irq;

	if (!handle || !handle->priv)
		return -EINVAL;

	dma_priv = handle->priv;

	MXC_DMA_DeInit(MXC_DMA1_S);
	MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_DMA1);

	MXC_DMA1_S->inten &= ~((1 << MXC_DMA_CHANNELS) - 1);

	for (i = 0; i < MXC_DMA_CHANNELS; i++) {
		irq = MXC_DMA_CH_GET_IRQ(MXC_DMA1_S, i);
		capi_irq_disable(irq);
		capi_irq_connect(irq, NULL, NULL);
	}

	capi_free(dma_priv->channels);
	capi_free(dma_priv);
	if (handle->init_allocated)
		capi_free(handle);

	dma = NULL;

	return 0;
}

/**
 * @brief Initialize a DMA channel
 * @param handle The DMA handle
 * @param chan_prt Pointer to a channel
 * @param id ID of the DMA channel
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_init_chan(struct capi_dma_handle *handle,
			   struct capi_dma_chan **chan_ptr, uint32_t id)
{
	int hw_id, ret;
	struct max_capi_dma_priv *dma_priv;
	struct max_capi_dma_ch_priv *ch_priv;
	struct capi_dma_chan *chan;

	if (!handle || !handle->priv || !chan_ptr)
		return -EINVAL;

	dma_priv = handle->priv;

	if (id > dma_priv->num_channels)
		return -EINVAL;

	if (dma_priv->channels[id] != NULL) {
		/* Channel already initialized */
		*chan_ptr = dma_priv->channels[id];
		return 0;
	}

	if (*chan_ptr == NULL) {
		chan = capi_calloc(1, sizeof(*chan));
		if (!chan)
			return -ENOMEM;
		chan->owned_by_app = false;
	} else {
		chan = *chan_ptr;
		chan->owned_by_app = true;
	}

	hw_id = MXC_DMA_AcquireChannel(MXC_DMA1_S);
	if (hw_id < 0) {
		ret = -EBUSY;
		goto free_channel;
	}
	ch_priv = capi_calloc(1, sizeof(*ch_priv));
	if (!ch_priv) {
		MXC_DMA_ReleaseChannel(hw_id);
		ret = -ENOMEM;
		goto free_channel;
	}
	ch_priv->hw_channel_id = hw_id; /* MSDK-assigned */
	chan->handle = handle;
	chan->id = id; /* User-assigned */
	chan->irq_num = MXC_DMA_CH_GET_IRQ(MXC_DMA1_S, hw_id);
	chan->extra = ch_priv;

	ret = capi_irq_connect(chan->irq_num,
			       max_capi_dma_isr_callback, chan);
	if (ret)
		goto free_channel_priv;

	ret = capi_irq_enable(chan->irq_num);
	if (ret)
		goto disconnect_irq;

	dma_priv->channels[id] = chan;
	*chan_ptr = chan;

	return 0;

disconnect_irq:
	capi_irq_connect(chan->irq_num, NULL, NULL);
free_channel_priv:
	capi_free(ch_priv);
	MXC_DMA_ReleaseChannel(hw_id);
free_channel:
	if (!chan->owned_by_app)
		capi_free(chan);
	return ret;
}

/**
 * @brief Deinitialize a DMA channel
 * @param chan The DMA channel
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	int ret = 0;
	struct max_capi_dma_priv *dma_priv;
	struct max_capi_dma_ch_priv *ch_priv;

	if (!chan || !chan->handle || !chan->handle->priv)
		return -EINVAL;

	dma_priv = chan->handle->priv;
	ch_priv = chan->extra;

	/* Disconnect from CAPI IRQ subsystem if it was used */
	capi_irq_disable(chan->irq_num);
	capi_irq_connect(chan->irq_num, NULL, NULL);

	ret = MXC_DMA_ReleaseChannel(ch_priv->hw_channel_id);

	dma_priv->channels[chan->id] = NULL;

	capi_free(ch_priv);
	if (!chan->owned_by_app)
		capi_free(chan);

	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Configure a DMA channel transfer
 * @param chan The DMA channel
 * @param xfer The transfer configuration struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_config_xfer(struct capi_dma_chan *chan,
			     struct capi_dma_transfer *xfer)
{
	int ret;
	struct max_capi_dma_ch_priv *ch_priv;
	struct max_capi_dma_xfer_extra *xfer_extra;
	mxc_dma_config_t dma_config;
	mxc_dma_reqsel_t reqsel = MXC_DMA_REQUEST_MEMTOMEM;
	mxc_dma_srcdst_t srcdst;
	mxc_dma_width_t srcwd, dstwd;

	if (!chan || !xfer || !xfer->extra)
		return -EINVAL;

	xfer_extra = xfer->extra;
	ch_priv = chan->extra;

	/* Determine request select based on transfer type */
	if (xfer->xfer_type != CAPI_DMA_MEM_TO_MEM) {
		reqsel = (mxc_dma_reqsel_t)xfer_extra->reqsel;
		/* Validate that reqsel direction matches transfer type */
		if (xfer->xfer_type == CAPI_DMA_MEM_TO_DEV) {
			/* MEM_TO_DEV should use TX request selects */
			if (reqsel == MXC_DMA_REQUEST_SPIRX ||
			    reqsel == MXC_DMA_REQUEST_UARTRX ||
			    reqsel == MXC_DMA_REQUEST_I3CRX_CONT ||
			    reqsel == MXC_DMA_REQUEST_I3CRX_TARG ||
			    reqsel == MXC_DMA_REQUEST_AESRX)
				return -EINVAL;
		} else if (xfer->xfer_type == CAPI_DMA_DEV_TO_MEM) {
			/* DEV_TO_MEM should use RX request selects */
			if (reqsel == MXC_DMA_REQUEST_SPITX ||
			    reqsel == MXC_DMA_REQUEST_UARTTX ||
			    reqsel == MXC_DMA_REQUEST_I3CTX_CONT ||
			    reqsel == MXC_DMA_REQUEST_I3CTX_TARG ||
			    reqsel == MXC_DMA_REQUEST_CRCTX ||
			    reqsel == MXC_DMA_REQUEST_AESTX)
				return -EINVAL;
		}
	}

	ret = _max_capi_dma_get_width(xfer->src_size, &srcwd);
	if (ret)
		return ret;
	ret = _max_capi_dma_get_width(xfer->dst_size, &dstwd);
	if (ret)
		return ret;

	dma_config = (mxc_dma_config_t) {
		.ch = ch_priv->hw_channel_id,
		.reqsel = reqsel,
		.srcwd = srcwd,
		.dstwd = dstwd,
		.srcinc_en = (xfer->src_inc != CAPI_DMA_NO_INCREMENT),
		.dstinc_en = (xfer->dst_inc != CAPI_DMA_NO_INCREMENT),
	};

	srcdst = (mxc_dma_srcdst_t) {
		.ch = ch_priv->hw_channel_id,
		.source = (void *)xfer->src,
		.dest = (void *)xfer->dst,
		.len = xfer->length,
	};

	ret = MXC_DMA_ConfigChannel(dma_config, srcdst);
	if (ret != E_NO_ERROR)
		return -EIO;

	/*  MXC_DMA_ConfigChannel explicitly sets the CTRL register for a
	    channel. We have to call MXC_DMA_ChannelEnableInt again to add the
	    MXC_F_DMA_CTRL_CTZ_IE flag. */
	MXC_DMA_EnableInt(MXC_DMA1_S, ch_priv->hw_channel_id);
	MXC_DMA_ChannelEnableInt(ch_priv->hw_channel_id, MXC_F_DMA_CTRL_CTZ_IE);

	chan->xfer = xfer;

	return 0;
}

/**
 * @brief Start a DMA transfer on a channel
 * @param chan The DMA channel
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	int ret;
	struct max_capi_dma_ch_priv *ch_priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	ch_priv = chan->extra;

	ret = MXC_DMA_Start(ch_priv->hw_channel_id);
	if (ret)
		return -EIO;

	return 0;
}

/**
 * @brief Stop a DMA transfer on a channel
 * @param chan The DMA channel
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	int ret;
	struct max_capi_dma_ch_priv *ch_priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	ch_priv = chan->extra;

	ret = MXC_DMA_Stop(ch_priv->hw_channel_id);
	if (ret)
		return -EIO;

	return 0;
}

/**
 * @brief Check if a DMA channel has completed its transfer
 * @param chan The DMA channel
 * @return true if the transfer is complete, false otherwise
 */
bool max_capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	struct max_capi_dma_ch_priv *ch_priv;
	int flags;

	if (!chan || !chan->extra)
		return false;

	ch_priv = chan->extra;
	flags = MXC_DMA_ChannelGetFlags(ch_priv->hw_channel_id);
	if (flags < 0)
		return false;

	/* Channel is complete when STATUS bit (bit 0) is clear (channel idle)
	   and CTZ_IF bit (bit 2) is set */
	return ((flags & MXC_F_DMA_STATUS_CTZ_IF) &&
		!(flags & MXC_F_DMA_STATUS_STATUS));
}

/**
 * @brief DMA controller interrupt handler
 * @param handle The DMA handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_isr(struct capi_dma_handle *handle)
{
	if (!handle)
		return -EINVAL;

	/* Call MSDK DMA interrupt handler for the controller */
	MXC_DMA_Handler(MXC_DMA1_S);

	return 0;
}

/**
 * @brief DMA channel interrupt handler
 * @param chan The DMA channel
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_isr_chan(struct capi_dma_chan *chan)
{
	struct max_capi_dma_ch_priv *ch_priv;
	int flags;

	if (!chan || !chan->extra)
		return -EINVAL;

	ch_priv = chan->extra;

	/* Get the interrupt flags for this channel */
	flags = MXC_DMA_ChannelGetFlags(ch_priv->hw_channel_id);
	if (flags < 0)
		return flags;

	/* Clear the interrupt flags */
	MXC_DMA_ChannelClearFlags(ch_priv->hw_channel_id, flags);

	/* Check if the transfer completed (CTZ interrupt or bus error) */
	if (flags & MXC_F_DMA_STATUS_CTZ_IF) {
		if (chan->xfer_complete_cb)
			chan->xfer_complete_cb(0, chan->xfer_complete_ctx);
	} else if (flags & MXC_F_DMA_STATUS_BUS_ERR) {
		if (chan->error_cb)
			chan->error_cb(0, chan->error_ctx);
	}

	return 0;
}

/**
 * @brief Register callback function for DMA channel interrupts
 * @param chan The DMA channel
 * @param callback The callback function to be called when an interrupt occurs
 * @param xfer_complete_ctx Context data to be passed to the function
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_register_complete_callback(struct capi_dma_chan *chan,
		capi_dma_xfer_complete_cb callback,
		void *xfer_complete_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->xfer_complete_cb = callback;
	chan->xfer_complete_ctx = xfer_complete_ctx;

	return 0;
}

/**
 * @brief Register callback function for DMA error events
 * @param chan The DMA channel
 * @param callback The callback function to be called when an error occurs
 * @param error_ctx Context data to be passed to the function
 * @return 0 on success, negative error code otherwise
 */
int max_capi_dma_register_error_callback(struct capi_dma_chan *chan,
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

/**
 * @brief IRQ callback wrapper that bridges CAPI IRQ to CAPI DMA
 * @param ctx - Context pointer (capi_dma_chan)
 */
static void max_capi_dma_isr_callback(void *ctx)
{
	struct capi_dma_chan *chan = (struct capi_dma_chan *)ctx;

	if (chan)
		max_capi_dma_isr_chan(chan);
}
