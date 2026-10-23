/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Implementation of the AXI DMAC CAPI DMA backend.
 *
 * The AXI DMAC has a single channel and moves a transfer as one or more
 * bursts. Its capabilities are fixed at synthesis, so init_chan reads them
 * from the core:
 *   - cyclic support : whether FLAGS.DMA_CYCLIC keeps a written 1 (the bit
 *                      only exists if the core was built with it)
 *   - maximum burst  : X_LENGTH (the length register) read back after
 *                      writing all ones; unimplemented upper bits read 0
 *   - direction      : which of DEST_ADDRESS / SRC_ADDRESS keep a written
 *                      value; only memory-mapped sides have an address
 *                      register (both: MEM_TO_MEM, source only: MEM_TO_DEV,
 *                      destination only: DEV_TO_MEM)
 *   - bus widths     : bytes per bus beat on the source and destination
 *                      side, from the interface description register
 *                      INTF_DESC
 *
 * Starting a transfer (xfer_start):
 *   1. Check the transfer matches the direction and the memory addresses are
 *      aligned to the bus widths.
 *   2. Enable the core (first time only) and unmask its interrupts.
 *   3. Program the first burst (addresses and length; X_LENGTH holds
 *      bytes - 1, Y_LENGTH is 0 for a 1D transfer) and write
 *      TRANSFER_SUBMIT. The core queues it and returns a transfer id.
 *
 * Completion:
 *   - IRQ mode    : the core raises SOT (start of transfer: the queued burst
 *                   was accepted, so the queue has room) and EOT (end of
 *                   transfer: a burst finished). On SOT the interrupt submits
 *                   the next burst while bytes remain. The transfer is done
 *                   once the last submitted transfer id is reported in
 *                   TRANSFER_DONE; the complete callback then runs from the
 *                   interrupt.
 *   - Polled mode : one burst only; TRANSFER_DONE is polled until the
 *                   submitted id is reported or timeout_ms expires.
 *
 * TRANSFER_DONE (one bit per transfer id) is used rather than counting EOT
 * interrupts, because the pending bits of back-to-back bursts merge into a
 * single interrupt.
 *
 * abort disables the core and drops the remaining bursts. Buffers are flushed
 * before and invalidated after a transfer through the weak fpga_dma_cache_*
 * hooks.
 *
 * See fpga_capi_dma.h for configuration.
 */

#include "fpga_capi_dma_priv.h"
#include "capi_alloc.h"
#include "capi_irq.h"
#include "capi_time.h"
#include <errno.h>
#include <string.h>
#include "capi_platform.h"

/**
 * @brief Flush the data cache for a buffer before the DMAC reads it.
 *        Weak no-op; the project overrides it (see header).
 * @param addr - Start address of the buffer.
 * @param len - Length of the buffer in bytes.
 */
__attribute__((weak)) void fpga_dma_cache_flush(uintptr_t addr, size_t len)
{
	(void)addr;
	(void)len;
}

/**
 * @brief Invalidate the data cache for a buffer the DMAC wrote.
 *        Weak no-op; the project overrides it (see header).
 * @param addr - Start address of the buffer.
 * @param len - Length of the buffer in bytes.
 */
__attribute__((weak)) void fpga_dma_cache_invalidate(uintptr_t addr, size_t len)
{
	(void)addr;
	(void)len;
}

/* --- register access ------------------------------------------------------ */

/**
 * @brief Write a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @param val - Value to write.
 */
static inline void fpga_dma_wr(uint32_t base, uint32_t reg, uint32_t val)
{
	CAPI_PLATFORM_IO_WRITE32(base + reg, val);
}

/**
 * @brief Read a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @return Register value.
 */
static inline uint32_t fpga_dma_rd(uint32_t base, uint32_t reg)
{
	return CAPI_PLATFORM_IO_READ32(base + reg);
}

/* Hardware ISR trampoline connected to the DMAC interrupt line. */
static void fpga_capi_dma_hw_isr(void *ctx);
static int fpga_capi_dma_xfer_abort(struct capi_dma_chan *chan);

/* --- transfer engine (axi_dmac.c) ----------------------------------------- */

/**
 * @brief Program and submit the next burst of the current transfer.
 *
 * Shared by xfer_start (first burst) and the SOT interrupt (later bursts):
 * burst = min(remaining - 1, max_length), program the address(es) of the
 * channel's direction, advance them by burst + 1, update remaining_size,
 * record the transfer id, then write X/Y_LENGTH and TRANSFER_SUBMIT.
 * @param cp - Pointer to the channel private data.
 */
static void fpga_dma_submit_burst(struct fpga_dma_chan_priv *cp)
{
	uint32_t burst_size;

	if (cp->remaining_size > cp->max_length)
		burst_size = cp->max_length;
	else
		burst_size = cp->remaining_size - 1;

	if (cp->direction != FPGA_DMA_DIR_MEM_TO_DEV) {
		fpga_dma_wr(cp->base, FPGA_DMAC_REG_DEST_ADDRESS, cp->next_dest_addr);
		fpga_dma_wr(cp->base, FPGA_DMAC_REG_DEST_STRIDE, 0x0);
		cp->next_dest_addr += burst_size + 1;
	}
	if (cp->direction != FPGA_DMA_DIR_DEV_TO_MEM) {
		fpga_dma_wr(cp->base, FPGA_DMAC_REG_SRC_ADDRESS, cp->next_src_addr);
		fpga_dma_wr(cp->base, FPGA_DMAC_REG_SRC_STRIDE, 0x0);
		cp->next_src_addr += burst_size + 1;
	}

	cp->remaining_size -= burst_size + 1;

	/* TRANSFER_ID is the id this submit gets; its TRANSFER_DONE bit clears
	 * on submit and sets at that burst's end-of-transfer. */
	cp->last_id = fpga_dma_rd(cp->base, FPGA_DMAC_REG_TRANSFER_ID) &
		      FPGA_DMAC_TRANSFER_ID_MASK;

	fpga_dma_wr(cp->base, FPGA_DMAC_REG_X_LENGTH, burst_size);
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_Y_LENGTH, 0x0);
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_TRANSFER_SUBMIT,
		    FPGA_DMAC_TRANSFER_SUBMIT);
}

/**
 * @brief Check whether the final burst of the transfer has finished.
 *
 * IRQ_PENDING bits coalesce: one EOT can stand for several bursts, and the EOT
 * of burst N can arrive together with the SOT of burst N+1. Completion is
 * therefore taken from the TRANSFER_DONE bit of the last submitted id, not
 * from the EOT bit itself.
 * @param cp - Pointer to the channel private data.
 * @return true once every burst was submitted and the last one is done.
 */
static bool fpga_dma_last_done(const struct fpga_dma_chan_priv *cp)
{
	if (cp->remaining_size)
		return false;

	/* The done bit of an id clears only when the DMAC accepts the request;
	 * until then it may still show the id's previous use. */
	if (fpga_dma_rd(cp->base, FPGA_DMAC_REG_TRANSFER_SUBMIT) &
	    FPGA_DMAC_QUEUE_FULL)
		return false;

	return (fpga_dma_rd(cp->base, FPGA_DMAC_REG_TRANSFER_DONE) &
		(1U << cp->last_id)) != 0U;
}

/**
 * @brief Mark the transfer done: make the destination visible to the CPU,
 *        set the completion flag and deliver the registered callback.
 * @param cp - Pointer to the channel private data.
 */
static void fpga_dma_complete(struct fpga_dma_chan_priv *cp)
{
	/* Drop stale destination lines so the CPU reads the DMA'd data. */
	fpga_dma_cache_invalidate((uintptr_t)cp->dst, cp->length);

	cp->completed = true;

	if (cp->capi_chan && cp->capi_chan->xfer_complete_cb)
		cp->capi_chan->xfer_complete_cb(0,
						cp->capi_chan->xfer_complete_ctx);
}

/**
 * @brief Service the DMAC interrupt for one channel.
 *
 * One function for all three directions: read and acknowledge IRQ_PENDING;
 * on SOT submit the next burst while data remains; on EOT the transfer is
 * done once the last submitted burst reports done.
 * @param cp - Pointer to the channel private data.
 * @return true if an interrupt was pending, false otherwise.
 */
static bool fpga_dma_service_chan(struct fpga_dma_chan_priv *cp)
{
	uint32_t reg_val;

	if (!cp)
		return false;

	/* Get interrupt sources and clear interrupts. */
	reg_val = fpga_dma_rd(cp->base, FPGA_DMAC_REG_IRQ_PENDING);
	if (!reg_val)
		return false;
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_IRQ_PENDING, reg_val);

	if ((reg_val & FPGA_DMAC_IRQ_SOT) && cp->remaining_size)
		fpga_dma_submit_burst(cp);

	if ((reg_val & FPGA_DMAC_IRQ_EOT) && !cp->completed &&
	    fpga_dma_last_done(cp))
		fpga_dma_complete(cp);

	return true;
}

/**
 * @brief Map a CAPI transfer direction onto the backend direction enum.
 * @param t - CAPI transfer type.
 * @return Backend direction, or FPGA_DMA_DIR_INVALID if unsupported.
 */
static enum fpga_dma_direction fpga_dma_dir(enum capi_dma_xfer_type t)
{
	switch (t) {
	case CAPI_DMA_MEM_TO_MEM:
				return FPGA_DMA_DIR_MEM_TO_MEM;
	case CAPI_DMA_MEM_TO_DEV:
		return FPGA_DMA_DIR_MEM_TO_DEV;
	case CAPI_DMA_DEV_TO_MEM:
		return FPGA_DMA_DIR_DEV_TO_MEM;
	default:
		return FPGA_DMA_DIR_INVALID;
	}
}

/**
 * @brief Detect the DMAC capabilities from its registers.
 *
 * Hardware cyclic from a FLAGS write/read-back (restored afterwards), maximum
 * burst from writing all-ones to X_LENGTH, direction from writing all-ones to
 * DEST/SRC_ADDRESS (a memory-mapped side reads it back, a stream side reads
 * 0), data-path widths from the INTF_DESC bytes-per-beat fields.
 * @param cp - Pointer to the channel private data to fill.
 * @return 0 on success, -ENODEV if neither side is memory-mapped.
 */
static int fpga_dma_detect_caps(struct fpga_dma_chan_priv *cp)
{
	uint32_t initial_reg_val, reg_val, intf_desc;
	uint32_t src_mem_mapped, dest_mem_mapped;

	/* Check if HW cyclic possible */
	initial_reg_val = fpga_dma_rd(cp->base, FPGA_DMAC_REG_FLAGS);
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_FLAGS, FPGA_DMAC_DMA_CYCLIC);
	reg_val = fpga_dma_rd(cp->base, FPGA_DMAC_REG_FLAGS);
	cp->hw_cyclic = (reg_val == FPGA_DMAC_DMA_CYCLIC);
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_FLAGS, initial_reg_val);

	/* Get maximum burst size and set value. */
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_X_LENGTH, 0xFFFFFFFFU);
	cp->max_length = fpga_dma_rd(cp->base, FPGA_DMAC_REG_X_LENGTH);

	/* Get transfer direction and set value. */
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_DEST_ADDRESS, 0xFFFFFFFFU);
	dest_mem_mapped = fpga_dma_rd(cp->base, FPGA_DMAC_REG_DEST_ADDRESS);
	fpga_dma_wr(cp->base, FPGA_DMAC_REG_SRC_ADDRESS, 0xFFFFFFFFU);
	src_mem_mapped = fpga_dma_rd(cp->base, FPGA_DMAC_REG_SRC_ADDRESS);

	intf_desc = fpga_dma_rd(cp->base, FPGA_DMAC_REG_INTF_DESC);
	cp->width_dst = 1U << FPGA_DMAC_DMA_BPB_DEST(intf_desc);
	cp->width_src = 1U << FPGA_DMAC_DMA_BPB_SRC(intf_desc);

	if (dest_mem_mapped && !src_mem_mapped)
		cp->direction = FPGA_DMA_DIR_DEV_TO_MEM;
	else if (!dest_mem_mapped && src_mem_mapped)
		cp->direction = FPGA_DMA_DIR_MEM_TO_DEV;
	else if (dest_mem_mapped && src_mem_mapped)
		cp->direction = FPGA_DMA_DIR_MEM_TO_MEM;
	else
		return -ENODEV;

	return 0;
}

/* --- ops ------------------------------------------------------------------ */

/**
 * @brief Initialize the CAPI backend instance.
 * @note DMAC: allocates the controller; connects and enables the DMAC
 *       interrupt through capi_irq when extra->use_irq is set.
 * @param handle - Pointer to the CAPI handle; allocated if *handle is NULL.
 * @param config - Pointer to the CAPI configuration.
 * @return 0 on success, or:
 *         -EINVAL  bad arguments
 *         -ENOMEM  allocation failed
 *         other    capi_irq_connect/capi_irq_enable error (use_irq requested)
 */
static int fpga_capi_dma_init(struct capi_dma_handle **handle,
			      const struct capi_dma_config *config)
{
	struct capi_dma_handle *dma_handle;
	struct fpga_dma_priv_handle *dma_priv;
	const struct fpga_dma_extra_config *extra;
	int ret;

	if (!handle || !config || config->num_chans == 0)
		return -EINVAL;

	if (*handle == NULL) {
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
		dma_handle->init_allocated = false;

		if (!dma_handle->priv)
			return -EINVAL;

		dma_priv = dma_handle->priv;
	}

	dma_priv->chan_privs = capi_calloc(config->num_chans,
					   sizeof(struct fpga_dma_chan_priv *));
	if (!dma_priv->chan_privs) {
		if (dma_handle->init_allocated) {
			capi_free(dma_priv);
			capi_free(dma_handle);
		}
		return -ENOMEM;
	}

	dma_priv->num_chans = config->num_chans;
	dma_priv->base = (uint32_t)config->id;

	extra = config->extra;
	dma_priv->timeout_ms = (extra && extra->timeout_ms) ?
			       extra->timeout_ms : FPGA_DMAC_DEFAULT_TIMEOUT_MS;

	dma_handle->ops = config->ops;

	/*
	 * Connect the DMAC interrupt. A requested IRQ that cannot be connected
	 * fails init: callers that depend on non-blocking completion (SPI Engine
	 * offload) must not silently get polled mode.
	 */
	dma_priv->use_irq = false;

	if (extra && extra->use_irq) {
		dma_priv->irq_id = extra->irq_id;
		ret = capi_irq_connect(dma_priv->irq_id, fpga_capi_dma_hw_isr,
				       dma_handle);
		if (!ret)
			ret = capi_irq_enable(dma_priv->irq_id);
		if (ret) {
			capi_free(dma_priv->chan_privs);
			dma_priv->chan_privs = NULL;
			if (dma_handle->init_allocated) {
				capi_free(dma_priv);
				capi_free(dma_handle);
			}
			return ret;
		}
		dma_priv->use_irq = true;
	}

	*handle = dma_handle;

	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note DMAC: disables the DMAC interrupt, halts (CTRL = 0) and frees every
 *       channel still open.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dma_deinit(struct capi_dma_handle *handle)
{
	struct fpga_dma_priv_handle *dma_priv;
	struct fpga_dma_chan_priv *cp;
	uint32_t i;

	if (!handle)
		return -EINVAL;

	dma_priv = handle->priv;

	if (dma_priv) {
		if (dma_priv->use_irq)
			(void)capi_irq_disable(dma_priv->irq_id);

		/* Channels still open: halt the engine and release them. */
		for (i = 0; dma_priv->chan_privs && i < dma_priv->num_chans; i++) {
			cp = dma_priv->chan_privs[i];
			if (!cp)
				continue;
			fpga_dma_wr(cp->base, FPGA_DMAC_REG_CTRL,
				    FPGA_DMAC_CTRL_DISABLE);
			if (cp->capi_chan && !cp->capi_chan->owned_by_app)
				capi_free(cp->capi_chan);
			capi_free(cp);
		}
		/* chan_privs is allocated by init in both handle modes. */
		capi_free(dma_priv->chan_privs);
		dma_priv->chan_privs = NULL;
		dma_priv->use_irq = false;
	}

	if (handle->init_allocated) {
		capi_free(dma_priv);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Initialize a DMA channel.
 * @note DMAC: detects the capabilities (cyclic, max burst, direction from the
 *       address write/read-back, widths from INTF_DESC), then CTRL = 0.
 * @param handle - Pointer to the CAPI DMA handle.
 * @param chan_ptr - Output: pointer to the new channel.
 * @param id - Channel index on the controller.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dma_init_chan(struct capi_dma_handle *handle,
				   struct capi_dma_chan **chan_ptr,
				   uint32_t id)
{
	struct capi_dma_chan *chan;
	struct fpga_dma_chan_priv *chan_priv;
	struct fpga_dma_priv_handle *dma_priv;
	int ret;

	if (!handle || !chan_ptr)
		return -EINVAL;

	dma_priv = handle->priv;
	if (!dma_priv)
		return -EINVAL;

	if (id >= dma_priv->num_chans)
		return -EINVAL;

	if (dma_priv->chan_privs[id])
		return -EBUSY;

	chan = capi_calloc(1, sizeof(*chan));
	if (!chan)
		return -ENOMEM;

	chan_priv = capi_calloc(1, sizeof(*chan_priv));
	if (!chan_priv) {
		capi_free(chan);
		return -ENOMEM;
	}

	chan_priv->base = dma_priv->base;
	ret = fpga_dma_detect_caps(chan_priv);
	if (ret) {
		capi_free(chan_priv);
		capi_free(chan);
		return ret;
	}

	/* Bring the engine up in a known-idle state: disabled, IRQs pending
	 * acknowledged. */
	fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_CTRL, FPGA_DMAC_CTRL_DISABLE);
	fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_IRQ_PENDING,
		    FPGA_DMAC_IRQ_SOT | FPGA_DMAC_IRQ_EOT);

	chan->owned_by_app = false;
	chan->handle = handle;
	chan->id = id;
	chan->extra = chan_priv;

	chan_priv->capi_chan = chan;
	chan_priv->completed = true;

	dma_priv->chan_privs[id] = chan_priv;
	*chan_ptr = chan;

	return 0;
}

/**
 * @brief Deinitialize a DMA channel.
 * @note DMAC: disables the channel (CTRL = 0).
 * @param chan - Pointer to the CAPI DMA channel.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	struct fpga_dma_chan_priv *chan_priv;
	struct fpga_dma_priv_handle *dma_priv;

	if (!chan || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;
	dma_priv = chan->handle->priv;

	if (chan_priv) {
		/* Halt the engine before releasing the channel. */
		fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_CTRL,
			    FPGA_DMAC_CTRL_DISABLE);

		if (dma_priv && dma_priv->chan_privs &&
		    chan->id < dma_priv->num_chans)
			dma_priv->chan_privs[chan->id] = NULL;

		capi_free(chan_priv);
	}

	if (!chan->owned_by_app)
		capi_free(chan);

	return 0;
}

/**
 * @brief Configure a DMA transfer.
 * @note DMAC: checks the direction against the detected one and the memory
 *       side increment mode, caches the transfer; no register access.
 *       Alignment is checked at xfer_start.
 * @param chan - Pointer to the CAPI DMA channel.
 * @param xfer - Pointer to the transfer description.
 * @return 0 on success, or:
 *         -EINVAL   bad arguments, zero length or address/length above 32 bit
 *         -ENOTSUP  direction other than the detected one, or a memory side
 *                   with CAPI_DMA_NO_INCREMENT
 */
static int fpga_capi_dma_config_xfer(struct capi_dma_chan *chan,
				     struct capi_dma_transfer *xfer)
{
	struct fpga_dma_chan_priv *chan_priv;
	enum fpga_dma_direction dir;

	if (!chan || !xfer || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;

	if (xfer->length == 0)
		return -EINVAL;

	/* The DMAC direction is fixed in fabric and detected at init. */
	dir = fpga_dma_dir(xfer->xfer_type);
	if (dir == FPGA_DMA_DIR_INVALID || dir != chan_priv->direction)
		return -ENOTSUP;

	if (xfer->length > UINT32_MAX || xfer->src > UINT32_MAX ||
	    xfer->dst > UINT32_MAX)
		return -EINVAL;

	/* The memory-mapped side(s) always increment; the DMAC has no fixed
	 * address mode. The stream side has no address, so its mode is ignored. */
	if (dir != FPGA_DMA_DIR_DEV_TO_MEM && xfer->src_inc == CAPI_DMA_NO_INCREMENT)
		return -ENOTSUP;
	if (dir != FPGA_DMA_DIR_MEM_TO_DEV && xfer->dst_inc == CAPI_DMA_NO_INCREMENT)
		return -ENOTSUP;

	chan_priv->src = xfer->src;
	chan_priv->dst = xfer->dst;
	chan_priv->length = xfer->length;
	chan_priv->completed = false;

	chan->xfer = xfer;

	return 0;
}

/**
 * @brief Start a DMA transfer.
 * @note DMAC: alignment checks, clears FLAGS.DMA_CYCLIC, enables the engine
 *       once (CTRL 0 -> ENABLE, IRQ_MASK 0), checks QUEUE_FULL, submits the
 *       first burst. IRQ mode: the SOT interrupt submits the remaining
 *       bursts; returns at once with a complete callback, otherwise waits for
 *       completion in 1 ms steps. Polled mode: single burst only, polls
 *       TRANSFER_DONE of the submitted id in 1 ms steps.
 *       A timeout halts the engine (CTRL = 0).
 * @param chan - Pointer to the CAPI DMA channel.
 * @return 0 on success, or:
 *         -EINVAL     misaligned address, or polled transfer longer than one
 *                     burst
 *         -EBUSY      DMAC transfer queue full
 *         -ETIMEDOUT  no completion within timeout_ms
 */
static int fpga_capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	struct fpga_dma_chan_priv *chan_priv;
	struct fpga_dma_priv_handle *dma_priv;
	uint32_t reg_val, timeout;

	if (!chan || !chan->extra || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;
	dma_priv = chan->handle->priv;
	if (!dma_priv || chan_priv->length == 0)
		return -EINVAL;

	switch (chan_priv->direction) {
	case FPGA_DMA_DIR_DEV_TO_MEM:
		if (chan_priv->dst % chan_priv->width_dst)
			return -EINVAL;
		break;
	case FPGA_DMA_DIR_MEM_TO_DEV:
		if (chan_priv->src % chan_priv->width_src)
			return -EINVAL;
		break;
	case FPGA_DMA_DIR_MEM_TO_MEM:
		if ((chan_priv->dst % chan_priv->width_dst) ||
		    (chan_priv->src % chan_priv->width_src))
			return -EINVAL;
		break;
	default:
		return -ENOTSUP;
	}

	/* Without the SOT interrupt nothing resubmits; polled is one burst. */
	if (!dma_priv->use_irq && chan_priv->length - 1U > chan_priv->max_length)
		return -EINVAL;

	/* Set current transfer parameters. */
	chan_priv->remaining_size = (uint32_t)chan_priv->length;
	chan_priv->next_src_addr = (uint32_t)chan_priv->src;
	chan_priv->next_dest_addr = (uint32_t)chan_priv->dst;
	chan_priv->completed = false;

	/* Make the source (and the destination, to drop dirty lines that would
	 * be written back over the DMA'd data) visible to the DMAC. */
	fpga_dma_cache_flush((uintptr_t)chan_priv->src, chan_priv->length);
	fpga_dma_cache_flush((uintptr_t)chan_priv->dst, chan_priv->length);

	/* Clear the DMA_CYCLIC flag for all transfers */
	reg_val = fpga_dma_rd(chan_priv->base, FPGA_DMAC_REG_FLAGS);
	fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_FLAGS,
		    reg_val & ~FPGA_DMAC_DMA_CYCLIC);

	/* Enable DMA if not already enabled. */
	reg_val = fpga_dma_rd(chan_priv->base, FPGA_DMAC_REG_CTRL);
	if (!(reg_val & FPGA_DMAC_CTRL_ENABLE)) {
		fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_CTRL,
			    FPGA_DMAC_CTRL_DISABLE);
		fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_CTRL,
			    FPGA_DMAC_CTRL_ENABLE);
		fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_IRQ_MASK, 0U);
	}

	if (fpga_dma_rd(chan_priv->base, FPGA_DMAC_REG_TRANSFER_SUBMIT) &
	    FPGA_DMAC_QUEUE_FULL)
		return -EBUSY;

	fpga_dma_submit_burst(chan_priv);

	if (dma_priv->use_irq) {
		if (chan->xfer_complete_cb)
			return 0;

		/* IRQ mode: wait for the ISR to mark the transfer done. */
		for (timeout = 0; !chan_priv->completed; timeout++) {
			if (timeout == dma_priv->timeout_ms * 1000U) {
				(void)fpga_capi_dma_xfer_abort(chan);
				return -ETIMEDOUT;
			}
			capi_wait_us(1);
		}

		return 0;
	}

	/* Polled mode: polled every
	 * 1 us rather than 1 ms so short transfers are not rounded up to a
	 * millisecond; done is taken
	 * from TRANSFER_DONE of the submitted id, then IRQ_PENDING is acked. */
	for (timeout = 0; !fpga_dma_last_done(chan_priv); timeout++) {
		if (timeout == dma_priv->timeout_ms * 1000U) {
			fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_CTRL,
				    FPGA_DMAC_CTRL_DISABLE);
			chan_priv->completed = true;
			return -ETIMEDOUT;
		}
		capi_wait_us(1);
	}
	fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_IRQ_PENDING,
		    FPGA_DMAC_IRQ_SOT | FPGA_DMAC_IRQ_EOT);

	fpga_dma_complete(chan_priv);

	return 0;
}

/**
 * @brief Abort an ongoing DMA transfer.
 * @note DMAC: halts the engine (CTRL = 0), then clears IRQ_PENDING and
 *       the remaining bursts.
 * @param chan - Pointer to the CAPI DMA channel.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	struct fpga_dma_chan_priv *chan_priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;

	/* Halt the engine; also drop the pending status and the burst
	 * bookkeeping so the ISR does not resubmit. */
	fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_CTRL, FPGA_DMAC_CTRL_DISABLE);
	chan_priv->remaining_size = 0;
	fpga_dma_wr(chan_priv->base, FPGA_DMAC_REG_IRQ_PENDING,
		    FPGA_DMAC_IRQ_SOT | FPGA_DMAC_IRQ_EOT);
	chan_priv->completed = true;

	return 0;
}

/**
 * @brief Report whether the channel's transfer has completed.
 * @note DMAC: returns the completion flag set by the EOT path.
 * @param chan - Pointer to the CAPI DMA channel.
 * @return true if completed (or idle), false otherwise.
 */
static bool fpga_capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	const struct fpga_dma_chan_priv *chan_priv;

	if (!chan || !chan->extra)
		return true;

	chan_priv = chan->extra;

	return chan_priv->completed;
}

/**
 * @brief Register the transfer-completion callback for a channel.
 * @note DMAC: delivered from the EOT interrupt path by xfer_start.
 * @param chan - Pointer to the CAPI DMA channel.
 * @param callback - Completion callback (NULL to unregister).
 * @param xfer_complete_ctx - User context passed to the callback.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dma_register_complete_callback(
	struct capi_dma_chan *chan, capi_dma_xfer_complete_cb callback,
	void *xfer_complete_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->xfer_complete_cb = callback;
	chan->xfer_complete_ctx = xfer_complete_ctx;

	return 0;
}

/**
 * @brief Hardware ISR trampoline connected to the DMAC interrupt line.
 * @param ctx - Callback context (the channel private data).
 */
static void fpga_capi_dma_hw_isr(void *ctx)
{
	struct capi_dma_handle *handle = ctx;
	struct fpga_dma_priv_handle *dma_priv;
	uint32_t i;

	if (!handle || !handle->priv)
		return;

	dma_priv = handle->priv;

	for (i = 0; i < dma_priv->num_chans; i++)
		(void)fpga_dma_service_chan(dma_priv->chan_privs[i]);
}

/**
 * @brief DMA controller interrupt handler (for a user-managed vector).
 * @note DMAC: services SOT/EOT on every channel.
 * @param handle - Pointer to the CAPI DMA handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dma_isr(struct capi_dma_handle *handle)
{
	if (!handle)
		return -EINVAL;

	fpga_capi_dma_hw_isr(handle);
	return 0;
}

/**
 * @brief DMA channel interrupt handler (for a user-managed vector).
 * @note DMAC: services SOT/EOT on one channel.
 * @param chan - Pointer to the CAPI DMA channel.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dma_isr_chan(struct capi_dma_chan *chan)
{
	if (!chan || !chan->extra)
		return -EINVAL;

	(void)fpga_dma_service_chan(chan->extra);
	return 0;
}

const struct capi_dma_ops fpga_capi_dma_ops = {
	.init = fpga_capi_dma_init,
	.deinit = fpga_capi_dma_deinit,
	.init_chan = fpga_capi_dma_init_chan,
	.deinit_chan = fpga_capi_dma_deinit_chan,
	.config_xfer = fpga_capi_dma_config_xfer,
	.xfer_start = fpga_capi_dma_xfer_start,
	.xfer_abort = fpga_capi_dma_xfer_abort,
	.chan_is_completed = fpga_capi_dma_chan_is_completed,
	.isr = fpga_capi_dma_isr,
	.isr_chan = fpga_capi_dma_isr_chan,
	.register_complete_callback = fpga_capi_dma_register_complete_callback,
};
