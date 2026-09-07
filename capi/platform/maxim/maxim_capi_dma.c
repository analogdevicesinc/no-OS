/***************************************************************************//**
 *   @file   maxim_capi_dma.c
 *   @brief  Implementation of DMA functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * Maxim DMA backend for CAPI -- one file for every MSDK part (see
 * maxim_capi_dma.h for the two controller families and the MXC_DMA1_S probe).
 * On the dual-controller secure part (MAX32657) it drives MXC_DMA1_S; on
 * single-controller parts (MAX32655/MAX32690/most) it drives the sole MXC_DMA.
 * The MSDK call-shape difference is absorbed by the _max_dma_* shims below, so
 * the transfer logic never branches on the part.
 *
 * Interrupt routing is delegated to the shared IRQ backend, not to
 * MXC_DMA_SetCallback(). maxim_capi_irq.c already ships weak per-channel DMA
 * vectors (DMA1_CHn on the dual part, DMAn on the single-controller parts) that
 * funnel into its _dma_handler(), which first invokes the callback registered in
 * irq_table[<channel IRQn>] and only then runs MXC_DMA_Handler(). Registering
 * here through capi_irq_connect() populates that same table entry, so the
 * per-channel dispatcher below runs exactly once; MXC_DMA_Handler() then finds
 * no MSDK-registered callback and merely clears the flags. Adding a
 * SetCallback() handler on top would fire the completion twice.
 *
 * A CAPI channel id is a logical slot (UART asks for id 0 for both its RX and
 * TX legs, one at a time); the physical MSDK channel is acquired at
 * init_chan() and released at deinit_chan().
 */

#include <errno.h>
#include <stdlib.h>
#include "maxim_capi_dma_priv.h"
#include "capi_alloc.h"
#include "capi_irq.h"

/* Per-channel interrupt dispatcher, registered via capi_irq_connect(). */
/*
 * DMA channel-status flags come in three generated spellings across the MSDK
 * matrix (STATUS_*_IF, ST_*_ST, STAT_*_ST) with no relation to the driver
 * revision -- ME11 uses STAT_ while ME10/ME14/ES17 use ST_ and the newer
 * parts use STATUS_. Probe each bit rather than tabulate the parts.
 */
#if   defined(MXC_F_DMA_STATUS_CTZ_IF)
#define MAX_DMA_ST_CTZ		MXC_F_DMA_STATUS_CTZ_IF
#elif defined(MXC_F_DMA_ST_CTZ_ST)
#define MAX_DMA_ST_CTZ		MXC_F_DMA_ST_CTZ_ST
#elif defined(MXC_F_DMA_STAT_CTZ_ST)
#define MAX_DMA_ST_CTZ		MXC_F_DMA_STAT_CTZ_ST
#else
#error "No known DMA count-to-zero status flag in this target's dma_regs.h"
#endif

#if   defined(MXC_F_DMA_STATUS_BUS_ERR)
#define MAX_DMA_ST_BUS_ERR	MXC_F_DMA_STATUS_BUS_ERR
#elif defined(MXC_F_DMA_ST_BUS_ERR)
#define MAX_DMA_ST_BUS_ERR	MXC_F_DMA_ST_BUS_ERR
#elif defined(MXC_F_DMA_STAT_BUS_ERR)
#define MAX_DMA_ST_BUS_ERR	MXC_F_DMA_STAT_BUS_ERR
#else
#error "No known DMA bus-error status flag in this target's dma_regs.h"
#endif

#if   defined(MXC_F_DMA_STATUS_TO_IF)
#define MAX_DMA_ST_TIMEOUT	MXC_F_DMA_STATUS_TO_IF
#elif defined(MXC_F_DMA_ST_TO_ST)
#define MAX_DMA_ST_TIMEOUT	MXC_F_DMA_ST_TO_ST
#elif defined(MXC_F_DMA_STAT_TO_ST)
#define MAX_DMA_ST_TIMEOUT	MXC_F_DMA_STAT_TO_ST
#else
#error "No known DMA timeout status flag in this target's dma_regs.h"
#endif

static void max_capi_dma_ch_dispatch(void *arg);

/*
 * MSDK DMA call-shape shims. Each shim accepts the controller instance for a
 * uniform call site and drops it where the MSDK API has none, so the code below
 * never re-tests the part. See maxim_capi_dma.h for the three traits and why
 * "takes an instance" and "is secure" cannot be one flag. MAX_CAPI_DMA_REGS is
 * declared in maxim_capi_dma.h, which maxim_capi_irq.c shares.
 */
/*
 * On the dual-controller part the driven instance is the SECURE one (DMA1_S),
 * which the MSDK only clock-gates on in a secure build; a non-secure build would
 * compile a silently dead transfer path, so fail loudly. Single-controller parts
 * have no secure/non-secure DMA split and are unaffected by this guard.
 */
#if MAX_CAPI_DMA_SECURE_DUAL && \
	(!defined(CONFIG_TRUSTED_EXECUTION_SECURE) || (CONFIG_TRUSTED_EXECUTION_SECURE == 0))
#error "maxim_capi_dma drives the MAX32657 secure DMA (DMA1_S), reachable only "\
	"from the TrustZone-secure world; build with CONFIG_TRUSTED_EXECUTION_SECURE."
#endif

static inline int _max_dma_init(mxc_dma_regs_t *dma)
{
#if MAX_CAPI_DMA_INST_ARG
	return MXC_DMA_Init(dma);
#else
	(void)dma;
	return MXC_DMA_Init();
#endif
}

static inline void _max_dma_deinit(mxc_dma_regs_t *dma)
{
#if MAX_CAPI_DMA_INST_ARG
	MXC_DMA_DeInit(dma);
#else
	(void)dma;
	MXC_DMA_DeInit();
#endif
}

static inline int _max_dma_acquire(mxc_dma_regs_t *dma)
{
#if MAX_CAPI_DMA_INST_ARG
	return MXC_DMA_AcquireChannel(dma);
#else
	(void)dma;
	return MXC_DMA_AcquireChannel();
#endif
}

static inline int _max_dma_enable_int(mxc_dma_regs_t *dma, int ch)
{
#if MAX_CAPI_DMA_INT_INST_ARG
	return MXC_DMA_EnableInt(dma, ch);
#else
	(void)dma;
	return MXC_DMA_EnableInt(ch);
#endif
}

static inline void _max_dma_handler(mxc_dma_regs_t *dma)
{
#if MAX_CAPI_DMA_INST_ARG
	MXC_DMA_Handler(dma);
#else
	(void)dma;
	MXC_DMA_Handler();
#endif
}

/**
 * @brief Resolve an acquired MSDK channel to its NVIC interrupt number.
 * @param msdk_ch - MSDK channel index (0..MXC_DMA_CHANNELS-1).
 * @param out_irq - Receives the per-channel IRQn (DMA1_CHn on the dual-secure
 *                  part, DMAn on single-controller parts).
 * @return 0 on success, negative error code otherwise.
 *
 * MXC_DMA_CH_GET_IRQ maps a channel to its vector; its arity differs by family
 * (it takes the controller on the dual part), so the shim branch supplies it.
 */
static int max_capi_dma_ch_to_irq(int msdk_ch, uint32_t *out_irq)
{
	if (msdk_ch < 0 || msdk_ch >= MXC_DMA_CHANNELS)
		return -EINVAL;

#if MAX_CAPI_DMA_INT_INST_ARG
	*out_irq = (uint32_t)MXC_DMA_CH_GET_IRQ(MXC_DMA1_S, msdk_ch);
#else
	*out_irq = (uint32_t)MXC_DMA_CH_GET_IRQ(msdk_ch);
#endif

	return 0;
}

/**
 * @brief Map a CAPI transfer size to an MSDK DMA bus width.
 * @param size - CAPI per-transfer size.
 * @return The MSDK width; widths above a word fall back to byte.
 */
static mxc_dma_width_t max_capi_dma_width(enum capi_dma_xfer_size size)
{
	switch (size) {
	case CAPI_DMA_XFER_SIZE_2_BYTES:
		return MXC_DMA_WIDTH_HALFWORD;
	case CAPI_DMA_XFER_SIZE_4_BYTES:
		return MXC_DMA_WIDTH_WORD;
	default:
		return MXC_DMA_WIDTH_BYTE;
	}
}

/**
 * @brief Initialize the MAX32657 secure DMA controller.
 * @param handle - Pointer to the DMA handle pointer. If *handle is NULL, memory
 *                 is allocated by the driver.
 * @param config - Pointer to the DMA configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_init(struct capi_dma_handle **handle,
			     const struct capi_dma_config *config)
{
	struct capi_dma_handle *dma_handle;
	struct max_capi_dma_priv_handle *priv;
	int ret;

	if (!handle || !config || !config->num_chans)
		return -EINVAL;

	/*
	 * MXC_DMA_Init() clock-gates the controller and clears its channels. It
	 * reports E_BAD_STATE if the instance is already up; treat that as success
	 * so a second consumer (e.g. UART) can share the controller.
	 */
	ret = _max_dma_init(MAX_CAPI_DMA_REGS);
	if (ret != E_NO_ERROR && ret != E_BAD_STATE)
		return -EIO;

	if (*handle == NULL) {
		dma_handle = capi_calloc(1, sizeof(*dma_handle));
		if (!dma_handle)
			return -ENOMEM;

		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(dma_handle);
			return -ENOMEM;
		}

		dma_handle->priv = priv;
		dma_handle->init_allocated = true;
	} else {
		dma_handle = *handle;
		dma_handle->init_allocated = false;

		if (!dma_handle->priv)
			return -EINVAL;

		priv = dma_handle->priv;
	}

	priv->chan_privs = capi_calloc(config->num_chans,
				       sizeof(*priv->chan_privs));
	if (!priv->chan_privs) {
		if (dma_handle->init_allocated) {
			capi_free(priv);
			capi_free(dma_handle);
		}
		return -ENOMEM;
	}

	priv->dma = MAX_CAPI_DMA_REGS;
	priv->num_chans = config->num_chans;
	dma_handle->ops = config->ops;

	*handle = dma_handle;
	return 0;
}

/**
 * @brief Deinitialize the DMA controller.
 * @param handle - Pointer to the DMA handle.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_deinit(struct capi_dma_handle *handle)
{
	struct max_capi_dma_priv_handle *priv;
	uint32_t i;

	if (!handle)
		return -EINVAL;

	priv = handle->priv;
	if (priv) {
		if (priv->chan_privs) {
			for (i = 0; i < priv->num_chans; i++) {
				if (!priv->chan_privs[i])
					continue;
				if (priv->chan_privs[i]->msdk_ch >= 0)
					MXC_DMA_ReleaseChannel(
						priv->chan_privs[i]->msdk_ch);
				if (handle->init_allocated)
					capi_free(priv->chan_privs[i]);
			}
			if (handle->init_allocated)
				capi_free(priv->chan_privs);
		}

		_max_dma_deinit(priv->dma);
	}

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Initialize a DMA channel: acquire hardware and wire its interrupt.
 * @param handle - Pointer to the DMA handle.
 * @param chan_ptr - Pointer to the channel pointer. If *chan_ptr is NULL,
 *                   memory is allocated by the driver.
 * @param id - Logical channel identifier.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_init_chan(struct capi_dma_handle *handle,
				  struct capi_dma_chan **chan_ptr,
				  uint32_t id)
{
	struct max_capi_dma_priv_handle *priv;
	struct max_capi_dma_chan_priv *chan_priv;
	struct capi_dma_chan *chan;
	uint32_t irq;
	int msdk_ch;
	int ret;

	if (!handle || !chan_ptr)
		return -EINVAL;

	priv = handle->priv;
	if (!priv || !priv->chan_privs)
		return -EINVAL;

	if (id >= priv->num_chans)
		return -EINVAL;

	if (priv->chan_privs[id])
		return -EBUSY;

	msdk_ch = _max_dma_acquire(priv->dma);
	if (msdk_ch < 0)
		return -EBUSY;

	ret = max_capi_dma_ch_to_irq(msdk_ch, &irq);
	if (ret) {
		MXC_DMA_ReleaseChannel(msdk_ch);
		return ret;
	}

	if (*chan_ptr == NULL) {
		chan = capi_calloc(1, sizeof(*chan));
		if (!chan) {
			MXC_DMA_ReleaseChannel(msdk_ch);
			return -ENOMEM;
		}
		chan->owned_by_app = false;
	} else {
		chan = *chan_ptr;
		chan->owned_by_app = true;
	}

	chan_priv = capi_calloc(1, sizeof(*chan_priv));
	if (!chan_priv) {
		if (!chan->owned_by_app)
			capi_free(chan);
		MXC_DMA_ReleaseChannel(msdk_ch);
		return -ENOMEM;
	}

	chan_priv->msdk_ch = msdk_ch;
	chan_priv->irq_num = irq;
	chan_priv->completed = true;
	chan_priv->capi_chan = chan;

	chan->handle = handle;
	chan->id = id;
	chan->irq_num = irq;
	chan->extra = chan_priv;

	/*
	 * Route the channel-completion vector to our dispatcher. This lands in
	 * irq_table[irq], which maxim_capi_irq.c's _dma_handler() invokes before
	 * MXC_DMA_Handler(); see the file header for why we avoid SetCallback().
	 */
	ret = capi_irq_connect(irq, max_capi_dma_ch_dispatch, chan);
	if (ret) {
		capi_free(chan_priv);
		if (!chan->owned_by_app)
			capi_free(chan);
		MXC_DMA_ReleaseChannel(msdk_ch);
		return ret;
	}

	priv->chan_privs[id] = chan_priv;
	*chan_ptr = chan;

	return 0;
}

/**
 * @brief Deinitialize a DMA channel and release its hardware.
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	struct max_capi_dma_priv_handle *priv;
	struct max_capi_dma_chan_priv *chan_priv;

	if (!chan || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;

	if (chan_priv) {
		capi_irq_disable(chan_priv->irq_num);
		if (chan_priv->msdk_ch >= 0) {
			MXC_DMA_Stop(chan_priv->msdk_ch);
			MXC_DMA_ReleaseChannel(chan_priv->msdk_ch);
		}

		priv = chan->handle->priv;
		if (priv && priv->chan_privs && chan->id < priv->num_chans)
			priv->chan_privs[chan->id] = NULL;

		capi_free(chan_priv);
		chan->extra = NULL;
	}

	if (!chan->owned_by_app)
		capi_free(chan);

	return 0;
}

/**
 * @brief Configure a DMA transfer on a channel.
 * @param chan - Pointer to the DMA channel.
 * @param xfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_config_xfer(struct capi_dma_chan *chan,
				    struct capi_dma_transfer *xfer)
{
	struct max_capi_dma_chan_priv *chan_priv;
	struct max_capi_dma_xfer_extra *extra;
	mxc_dma_config_t cfg;
	mxc_dma_srcdst_t srcdst;
	int ret;

	if (!chan || !chan->extra || !xfer)
		return -EINVAL;

	chan_priv = chan->extra;
	extra = xfer->extra;

	srcdst.ch = chan_priv->msdk_ch;
	srcdst.source = (void *)xfer->src;
	srcdst.dest = (void *)xfer->dst;
	srcdst.len = (int)xfer->length;

	cfg.ch = chan_priv->msdk_ch;
	cfg.reqsel = extra ? extra->reqsel : MXC_DMA_REQUEST_MEMTOMEM;
	cfg.srcwd = max_capi_dma_width(xfer->src_size);
	cfg.dstwd = max_capi_dma_width(xfer->dst_size);
	cfg.srcinc_en = (xfer->src_inc != CAPI_DMA_NO_INCREMENT);
	cfg.dstinc_en = (xfer->dst_inc != CAPI_DMA_NO_INCREMENT);

	ret = MXC_DMA_ConfigChannel(cfg, srcdst);
	if (ret != E_NO_ERROR)
		return -EIO;

	/* Interrupt on count-to-zero, i.e. transfer complete. */
	ret = MXC_DMA_SetChannelInterruptEn(chan_priv->msdk_ch, false, true);
	if (ret != E_NO_ERROR)
		return -EIO;

	chan->xfer = xfer;
	chan_priv->completed = false;

	return 0;
}

/**
 * @brief Start a configured DMA transfer.
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	struct max_capi_dma_chan_priv *chan_priv;
	struct max_capi_dma_priv_handle *priv;
	int ret;

	if (!chan || !chan->extra || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;
	priv = chan->handle->priv;
	if (!priv)
		return -EINVAL;

	chan_priv->completed = false;

	/* Enable the channel at the controller and NVIC before starting. */
	ret = _max_dma_enable_int(priv->dma, chan_priv->msdk_ch);
	if (ret != E_NO_ERROR)
		return -EIO;

	ret = capi_irq_enable(chan_priv->irq_num);
	if (ret)
		return ret;

	ret = MXC_DMA_Start(chan_priv->msdk_ch);
	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Abort an ongoing DMA transfer.
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	struct max_capi_dma_chan_priv *chan_priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;

	MXC_DMA_Stop(chan_priv->msdk_ch);
	capi_irq_disable(chan_priv->irq_num);
	chan_priv->completed = true;

	return 0;
}

/**
 * @brief Report whether a channel's transfer has completed.
 * @param chan - Pointer to the DMA channel.
 * @return true if completed (or the channel is invalid), false otherwise.
 */
static bool max_capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	struct max_capi_dma_chan_priv *chan_priv;

	if (!chan || !chan->extra)
		return true;

	chan_priv = chan->extra;
	return chan_priv->completed;
}

/**
 * @brief Per-channel interrupt dispatcher.
 * @param arg - The capi_dma_chan the interrupt belongs to.
 *
 * Runs from maxim_capi_irq.c's _dma_handler() ahead of MXC_DMA_Handler(), so the
 * status flags are still latched here; we read but do not clear them (the MSDK
 * handler that follows clears them). A count-to-zero flag is a completion; a
 * bus-error or timeout flag routes to the error callback.
 */
static void max_capi_dma_ch_dispatch(void *arg)
{
	struct capi_dma_chan *chan = arg;
	struct max_capi_dma_chan_priv *chan_priv;
	int flags;

	if (!chan || !chan->extra)
		return;

	chan_priv = chan->extra;
	flags = MXC_DMA_ChannelGetFlags(chan_priv->msdk_ch);

	chan_priv->completed = true;

	if (flags & (MAX_DMA_ST_BUS_ERR | MAX_DMA_ST_TIMEOUT)) {
		if (chan->error_cb)
			chan->error_cb((uint32_t)flags, chan->error_ctx);
		return;
	}

	if (flags & MAX_DMA_ST_CTZ) {
		if (chan->xfer_complete_cb)
			chan->xfer_complete_cb(0, chan->xfer_complete_ctx);
	}
}

/**
 * @brief Controller-level interrupt handler for caller-managed vectors.
 * @param handle - Pointer to the DMA handle.
 * @return 0 on success, negative error code otherwise.
 *
 * Provided for completeness; the shared IRQ backend already services the
 * DMA1_CHn vectors, so this is only needed by callers that own the vectors.
 */
static int max_capi_dma_isr(struct capi_dma_handle *handle)
{
	struct max_capi_dma_priv_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	_max_dma_handler(priv->dma);

	return 0;
}

/**
 * @brief Channel-level interrupt handler for caller-managed vectors.
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_isr_chan(struct capi_dma_chan *chan)
{
	if (!chan || !chan->extra)
		return -EINVAL;

	max_capi_dma_ch_dispatch(chan);

	return 0;
}

/**
 * @brief Register a transfer-completion callback for a channel.
 * @param chan - Pointer to the DMA channel.
 * @param callback - Completion callback.
 * @param xfer_complete_ctx - Context passed to the callback.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_register_complete_callback(struct capi_dma_chan *chan,
		capi_dma_xfer_complete_cb callback, void *xfer_complete_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->xfer_complete_cb = callback;
	chan->xfer_complete_ctx = xfer_complete_ctx;

	return 0;
}

/**
 * @brief Register an error callback for a channel.
 * @param chan - Pointer to the DMA channel.
 * @param callback - Error callback.
 * @param error_ctx - Context passed to the callback.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_dma_register_error_callback(struct capi_dma_chan *chan,
		capi_dma_error_cb callback, void *error_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->error_cb = callback;
	chan->error_ctx = error_ctx;

	return 0;
}

/**
 * @brief Maxim platform specific DMA operations for CAPI.
 */
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
