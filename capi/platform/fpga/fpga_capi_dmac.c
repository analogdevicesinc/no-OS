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
 *   - Software cyclic (IRQ mode, cyclic buffer longer than one burst): as
 *                   above, but on SOT after the last burst the source rewinds
 *                   to the buffer start; the callback runs once per pass.
 *   - Polled mode : one burst only; TRANSFER_DONE of the submitted id is
 *                   read by chan_is_completed / fpga_dmac_wait_completion.
 *
 * TRANSFER_DONE (one bit per transfer id) is used rather than counting EOT
 * interrupts, because the pending bits of back-to-back bursts merge into a
 * single interrupt.
 *
 * xfer_start and start_scattergather_xfer return once the transfer is
 * submitted (axi_dmac_transfer_start); fpga_dmac_wait_completion waits for it
 * (axi_dmac_transfer_wait_completion).
 *
 * abort disables the core and drops the remaining bursts. Buffers are flushed
 * before and invalidated after a transfer through the weak fpga_dmac_cache_*
 * hooks.
 *
 * See fpga_capi_dmac.h for configuration.
 */

#include "fpga_capi_dmac_priv.h"
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
__attribute__((weak)) void fpga_dmac_cache_flush(uintptr_t addr, size_t len)
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
__attribute__((weak)) void fpga_dmac_cache_invalidate(uintptr_t addr,
		size_t len)
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
static inline void fpga_dmac_wr(uint32_t base, uint32_t reg, uint32_t val)
{
	CAPI_PLATFORM_IO_WRITE32(base + reg, val);
}

/**
 * @brief Read a 32-bit core register.
 * @param base - Base address of the core's register block.
 * @param reg - Register byte offset.
 * @return Register value.
 */
static inline uint32_t fpga_dmac_rd(uint32_t base, uint32_t reg)
{
	return CAPI_PLATFORM_IO_READ32(base + reg);
}

/* Hardware ISR trampoline connected to the DMAC interrupt line. */
static void fpga_capi_dmac_hw_isr(void *ctx);
static int fpga_capi_dmac_xfer_abort(struct capi_dma_chan *chan);

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
/**
 * @brief Bytes a (possibly 2D) block touches on one memory side.
 * @param len - Bytes per row.
 * @param rows - Rows (0 or 1 => 1D).
 * @param stride - Bytes between row starts.
 * @return Span from the first byte to the end of the last row.
 */
static size_t fpga_dmac_span(size_t len, uint32_t rows, uint32_t stride)
{
	return rows > 1U ? (size_t)(rows - 1U) * stride + len : len;
}

static void fpga_dmac_submit_burst(struct fpga_dmac_chan_priv *cp)
{
	uint32_t burst_size;

	if (cp->remaining_size > cp->max_length)
		burst_size = cp->max_length;
	else
		burst_size = cp->remaining_size - 1;

	if (cp->direction != FPGA_DMAC_DIR_MEM_TO_DEV) {
		fpga_dmac_wr(cp->base, FPGA_DMAC_REG_DEST_ADDRESS, cp->next_dest_addr);
		fpga_dmac_wr(cp->base, FPGA_DMAC_REG_DEST_STRIDE, cp->dst_stride);
		cp->next_dest_addr += burst_size + 1;
	}
	if (cp->direction != FPGA_DMAC_DIR_DEV_TO_MEM) {
		fpga_dmac_wr(cp->base, FPGA_DMAC_REG_SRC_ADDRESS, cp->next_src_addr);
		fpga_dmac_wr(cp->base, FPGA_DMAC_REG_SRC_STRIDE, cp->src_stride);
		cp->next_src_addr += burst_size + 1;
	}

	cp->remaining_size -= burst_size + 1;

	/* TRANSFER_ID is the id this submit gets; its TRANSFER_DONE bit clears
	 * on submit and sets at that burst's end-of-transfer. */
	cp->last_id = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_TRANSFER_ID) &
		      FPGA_DMAC_TRANSFER_ID_MASK;

	/* Software cyclic: remember the id so its end-of-pass can be found. */
	if (cp->sw_cyclic && cp->pend_count < FPGA_DMAC_PEND_MAX) {
		uint8_t slot = (cp->pend_head + cp->pend_count) %
			       FPGA_DMAC_PEND_MAX;

		cp->pend_id[slot] = (uint8_t)cp->last_id;
		cp->pend_end[slot] = (cp->remaining_size == 0U);
		cp->pend_count++;
	}

	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_X_LENGTH, burst_size);
	/* 2D runs as one burst (config_xfer checked it fits): rows - 1. */
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_Y_LENGTH,
		     cp->rows > 1U ? cp->rows - 1U : 0U);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_TRANSFER_SUBMIT,
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
static bool fpga_dmac_last_done(const struct fpga_dmac_chan_priv *cp)
{
	if (cp->remaining_size)
		return false;

	/* The done bit of an id clears only when the DMAC accepts the request;
	 * until then it may still show the id's previous use. */
	if (fpga_dmac_rd(cp->base, FPGA_DMAC_REG_TRANSFER_SUBMIT) &
	    FPGA_DMAC_QUEUE_FULL)
		return false;

	return (fpga_dmac_rd(cp->base, FPGA_DMAC_REG_TRANSFER_DONE) &
		(1U << cp->last_id)) != 0U;
}

/**
 * @brief Mark the transfer done: make the destination visible to the CPU,
 *        set the completion flag and deliver the registered callback.
 * @param cp - Pointer to the channel private data.
 */
static void fpga_dmac_complete(struct fpga_dmac_chan_priv *cp)
{
	uint32_t i;

	/* Drop stale destination lines so the CPU reads the DMA'd data. */
	if (cp->sg_desc) {
		for (i = 0; i < cp->sg_num; i++)
			if (cp->sg_desc[i].dest_addr)
				fpga_dmac_cache_invalidate(
					(uintptr_t)cp->sg_desc[i].dest_addr,
					fpga_dmac_span(cp->sg_desc[i].x_len + 1U,
						       cp->sg_desc[i].y_len + 1U,
						       cp->sg_desc[i].dst_stride));
	} else {
		fpga_dmac_cache_invalidate((uintptr_t)cp->dst,
					   fpga_dmac_span(cp->length, cp->rows,
							   cp->dst_stride));
	}

	cp->completed = true;

	if (cp->capi_chan && cp->capi_chan->xfer_complete_cb)
		cp->capi_chan->xfer_complete_cb(0,
						cp->capi_chan->xfer_complete_ctx);
}

/**
 * @brief Retire finished software-cyclic bursts, oldest first.
 *
 * A burst is finished once its TRANSFER_DONE bit is set. The newest id may
 * still sit in the submit register (QUEUE_FULL), its done bit then shows the
 * id's previous use, so it is not looked at until accepted. Every retired
 * burst that ended a pass runs the complete callback (one per buffer pass,
 * like the hardware cyclic EOT).
 * @param cp - Pointer to the channel private data.
 */
static void fpga_dmac_sw_cyclic_retire(struct fpga_dmac_chan_priv *cp)
{
	uint32_t done;
	bool end;

	done = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_TRANSFER_DONE);

	while (cp->pend_count) {
		if (cp->pend_count == 1U &&
		    (fpga_dmac_rd(cp->base, FPGA_DMAC_REG_TRANSFER_SUBMIT) &
		     FPGA_DMAC_QUEUE_FULL))
			break;
		if (!(done & (1U << cp->pend_id[cp->pend_head])))
			break;

		end = cp->pend_end[cp->pend_head];
		cp->pend_head = (cp->pend_head + 1U) % FPGA_DMAC_PEND_MAX;
		cp->pend_count--;

		if (end && cp->capi_chan && cp->capi_chan->xfer_complete_cb)
			cp->capi_chan->xfer_complete_cb(0,
							cp->capi_chan->xfer_complete_ctx);
	}
}

/**
 * @brief Service the DMAC interrupt for one channel.
 *
 * One function for all three directions: read and acknowledge IRQ_PENDING;
 * on SOT submit the next burst while data remains; on EOT the transfer is
 * done once the last submitted burst reports done.
 * Software cyclic (axi_dmac_mem_to_dev_isr): on SOT with the whole buffer
 * submitted, rewind to the buffer start and keep submitting; on EOT run the
 * callback for every finished pass. Never completes until xfer_abort.
 * @param cp - Pointer to the channel private data.
 * @return true if an interrupt was pending, false otherwise.
 */
static bool fpga_dmac_service_chan(struct fpga_dmac_chan_priv *cp)
{
	uint32_t reg_val;

	if (!cp)
		return false;

	/* Get interrupt sources and clear interrupts. */
	reg_val = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_IRQ_PENDING);
	if (!reg_val)
		return false;
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_IRQ_PENDING, reg_val);

	if (reg_val & FPGA_DMAC_IRQ_SOT) {
		if (cp->sw_cyclic && !cp->remaining_size) {
			cp->remaining_size = (uint32_t)cp->length;
			cp->next_src_addr = (uint32_t)cp->src;
		}
		if (cp->remaining_size)
			fpga_dmac_submit_burst(cp);
	}

	if (cp->sw_cyclic) {
		if (reg_val & FPGA_DMAC_IRQ_EOT)
			fpga_dmac_sw_cyclic_retire(cp);
		return true;
	}

	/* Hardware cyclic raises no EOT (axi_dmac "Cyclic Transfers"): no
	 * per-pass callback; the transfer only ends on xfer_abort. */
	if (cp->cyclic)
		return true;

	if ((reg_val & FPGA_DMAC_IRQ_EOT) && !cp->completed &&
	    fpga_dmac_last_done(cp))
		fpga_dmac_complete(cp);

	return true;
}

/**
 * @brief Map a CAPI transfer direction onto the backend direction enum.
 * @param t - CAPI transfer type.
 * @return Backend direction, or FPGA_DMAC_DIR_INVALID if unsupported.
 */
static enum fpga_dmac_direction fpga_dmac_dir(enum capi_dma_xfer_type t)
{
	switch (t) {
	case CAPI_DMA_MEM_TO_MEM:
				return FPGA_DMAC_DIR_MEM_TO_MEM;
	case CAPI_DMA_MEM_TO_DEV:
		return FPGA_DMAC_DIR_MEM_TO_DEV;
	case CAPI_DMA_DEV_TO_MEM:
		return FPGA_DMAC_DIR_DEV_TO_MEM;
	default:
		return FPGA_DMAC_DIR_INVALID;
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
static int fpga_dmac_detect_caps(struct fpga_dmac_chan_priv *cp)
{
	uint32_t initial_reg_val, reg_val, intf_desc;
	uint32_t src_mem_mapped, dest_mem_mapped;

	/* Check if HW cyclic possible */
	initial_reg_val = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_FLAGS);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_FLAGS, FPGA_DMAC_DMA_CYCLIC);
	reg_val = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_FLAGS);
	cp->hw_cyclic = (reg_val == FPGA_DMAC_DMA_CYCLIC);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_FLAGS, initial_reg_val);

	/* 2D and scatter-gather: the registers exist only when built in
	 * (Linux dma-axi-dmac.c axi_dmac_detect_caps). */
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_Y_LENGTH, 1U);
	cp->hw_2d = (fpga_dmac_rd(cp->base, FPGA_DMAC_REG_Y_LENGTH) == 1U);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_Y_LENGTH, 0U);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_SG_ADDRESS, 0xFFFFFFFFU);
	cp->hw_sg = (fpga_dmac_rd(cp->base, FPGA_DMAC_REG_SG_ADDRESS) != 0U);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_SG_ADDRESS, 0U);

	/* Get maximum burst size and set value. */
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_X_LENGTH, 0xFFFFFFFFU);
	cp->max_length = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_X_LENGTH);

	/* Get transfer direction and set value. */
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_DEST_ADDRESS, 0xFFFFFFFFU);
	dest_mem_mapped = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_DEST_ADDRESS);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_SRC_ADDRESS, 0xFFFFFFFFU);
	src_mem_mapped = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_SRC_ADDRESS);

	intf_desc = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_INTF_DESC);
	cp->width_dst = 1U << FPGA_DMAC_DMA_BPB_DEST(intf_desc);
	cp->width_src = 1U << FPGA_DMAC_DMA_BPB_SRC(intf_desc);

	if (dest_mem_mapped && !src_mem_mapped)
		cp->direction = FPGA_DMAC_DIR_DEV_TO_MEM;
	else if (!dest_mem_mapped && src_mem_mapped)
		cp->direction = FPGA_DMAC_DIR_MEM_TO_DEV;
	else if (dest_mem_mapped && src_mem_mapped)
		cp->direction = FPGA_DMAC_DIR_MEM_TO_MEM;
	else
		return -ENODEV;

	return 0;
}

/**
 * @brief Validate the 2D part of a block against the detected core.
 * @param cp - Pointer to the channel private data.
 * @param dir - Transfer direction.
 * @param len - Bytes per row.
 * @param xc - Per-transfer config with rows > 1.
 * @return 0 if valid, -ENOTSUP (no 2D core, or cyclic), -EINVAL otherwise.
 */
static int fpga_dmac_check_2d(const struct fpga_dmac_chan_priv *cp,
			      enum fpga_dmac_direction dir, size_t len,
			      const struct fpga_dmac_xfer_config *xc)
{
	if (!cp->hw_2d || xc->cyclic)
		return -ENOTSUP;
	/* One hardware submit: X and Y each within the length register. */
	if (len - 1U > cp->max_length || xc->rows - 1U > cp->max_length)
		return -EINVAL;
	if (dir != FPGA_DMAC_DIR_DEV_TO_MEM &&
	    (xc->src_stride < len || xc->src_stride % cp->width_src ||
	     fpga_dmac_span(len, xc->rows, xc->src_stride) > UINT32_MAX))
		return -EINVAL;
	if (dir != FPGA_DMAC_DIR_MEM_TO_DEV &&
	    (xc->dst_stride < len || xc->dst_stride % cp->width_dst ||
	     fpga_dmac_span(len, xc->rows, xc->dst_stride) > UINT32_MAX))
		return -EINVAL;

	return 0;
}

/**
 * @brief Enable the engine in register (or descriptor) mode.
 *
 * CTRL is rewritten only on a mode change (0 -> ENABLE[|HWDESC]), so
 * back-to-back transfers in the same mode keep the engine running.
 * @param base - DMAC register base.
 * @param hwdesc - Fetch transfers from SG descriptors (CTRL.HWDESC).
 */
static void fpga_dmac_enable(uint32_t base, bool hwdesc)
{
	uint32_t want = FPGA_DMAC_CTRL_ENABLE |
			(hwdesc ? FPGA_DMAC_CTRL_HWDESC : 0U);

	if (fpga_dmac_rd(base, FPGA_DMAC_REG_CTRL) != want) {
		fpga_dmac_wr(base, FPGA_DMAC_REG_CTRL, FPGA_DMAC_CTRL_DISABLE);
		fpga_dmac_wr(base, FPGA_DMAC_REG_CTRL, want);
		fpga_dmac_wr(base, FPGA_DMAC_REG_IRQ_MASK, 0U);
	}
}

/**
 * @brief Re-arm the DMAC interrupt line before an IRQ-mode transfer.
 *
 * An IRQ controller deinit/init since capi_dma_init() drops the connection
 * and the line enable, and the completion would then never be taken.
 * @param handle - CAPI DMA handle (ISR context).
 * @param dma_priv - Controller private data.
 * @return 0 on success, capi_irq error otherwise.
 */
static int fpga_dmac_irq_rearm(struct capi_dma_handle *handle,
			       const struct fpga_dmac_priv *dma_priv)
{
	int ret;

	if (!dma_priv->use_irq)
		return 0;
	ret = capi_irq_connect(dma_priv->irq_id, fpga_capi_dmac_hw_isr, handle);
	if (!ret)
		ret = capi_irq_enable(dma_priv->irq_id);

	return ret;
}

/**
 * @brief Polled mode: finish the transfer once TRANSFER_DONE reports it.
 *
 * axi_dmac_is_transfer_ready: acks IRQ_PENDING, invalidates the destination
 * and delivers the callback. IRQ mode completes from the ISR; nothing to do.
 * @param cp - Channel private data.
 * @param dma_priv - Controller private data.
 * @return true if the transfer has completed.
 */
static bool fpga_dmac_poll(struct fpga_dmac_chan_priv *cp,
			   const struct fpga_dmac_priv *dma_priv)
{
	if (cp->completed || dma_priv->use_irq)
		return cp->completed;
	if (!fpga_dmac_last_done(cp))
		return false;

	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_IRQ_PENDING,
		     FPGA_DMAC_IRQ_SOT | FPGA_DMAC_IRQ_EOT);
	fpga_dmac_complete(cp);

	return true;
}

/* --- ops ------------------------------------------------------------------ */

/* The memory-mapped side(s) always increment; the DMAC has no fixed
 * address mode. The stream side has no address, so its mode is ignored. */
static int fpga_dmac_check_inc(enum fpga_dmac_direction dir,
			       enum capi_dma_addr_inc src_inc,
			       enum capi_dma_addr_inc dst_inc)
{
	if (dir != FPGA_DMAC_DIR_DEV_TO_MEM && src_inc == CAPI_DMA_NO_INCREMENT)
		return -ENOTSUP;
	if (dir != FPGA_DMAC_DIR_MEM_TO_DEV && dst_inc == CAPI_DMA_NO_INCREMENT)
		return -ENOTSUP;

	return 0;
}

/* Memory-mapped addresses must be aligned to that side's bus width. */
static int fpga_dmac_check_align(const struct fpga_dmac_chan_priv *cp,
				 enum fpga_dmac_direction dir,
				 uint64_t src, uint64_t dst)
{
	if ((dir != FPGA_DMAC_DIR_DEV_TO_MEM && src % cp->width_src) ||
	    (dir != FPGA_DMAC_DIR_MEM_TO_DEV && dst % cp->width_dst))
		return -EINVAL;

	return 0;
}

/* Cyclic: hardware when it fits one burst (axi_dmac.c conditions), else
 * software cyclic driven by the SOT interrupt. */
static int fpga_dmac_pick_cyclic(struct fpga_dmac_chan_priv *cp,
				 const struct fpga_dmac_priv *dma_priv,
				 enum fpga_dmac_direction dir, size_t length)
{
	if (dir != FPGA_DMAC_DIR_MEM_TO_DEV)
		return -ENOTSUP;
	if (cp->hw_cyclic && length - 1U <= cp->max_length)
		cp->cyclic = true;
	else if (dma_priv->use_irq)
		cp->sw_cyclic = true;
	else
		return -ENOTSUP;

	return 0;
}

/* Takes the caller's static handle or allocates one (init_allocated). */
static int fpga_dmac_handle_get(struct capi_dma_handle **handle,
				struct capi_dma_handle **h)
{
	struct fpga_dmac_priv *priv;

	if (*handle != NULL) {
		*h = *handle;
		if (!(*h)->priv)
			return -EINVAL;
		/* Re-init of a live handle is a caller bug, not a reset. */
		if ((*h)->ops)
			return -EBUSY;
		(*h)->init_allocated = false;
		return 0;
	}

	*h = capi_calloc(1, sizeof(**h));
	if (!*h)
		return -ENOMEM;
	priv = capi_calloc(1, sizeof(*priv));
	if (!priv) {
		capi_free(*h);
		return -ENOMEM;
	}
	(*h)->priv = priv;
	(*h)->init_allocated = true;

	return 0;
}

/*
 * Connects the DMAC interrupt. A requested IRQ that cannot be connected
 * fails init: callers that depend on non-blocking completion (SPI Engine
 * offload) must not silently get polled mode.
 */
static int fpga_dmac_irq_setup(struct capi_dma_handle *h,
			       struct fpga_dmac_priv *dma_priv,
			       const struct fpga_dmac_config *extra)
{
	int ret;

	dma_priv->use_irq = false;
	if (!extra || !extra->use_irq)
		return 0;

	dma_priv->irq_id = extra->irq_id;
	ret = capi_irq_connect(dma_priv->irq_id, fpga_capi_dmac_hw_isr, h);
	if (!ret)
		ret = capi_irq_enable(dma_priv->irq_id);
	if (!ret)
		dma_priv->use_irq = true;

	return ret;
}

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
static int fpga_capi_dmac_init(struct capi_dma_handle **handle,
			       const struct capi_dma_config *config)
{
	struct capi_dma_handle *dma_handle;
	struct fpga_dmac_priv *dma_priv;
	int ret;

	if (!handle || !config || config->num_chans == 0)
		return -EINVAL;

	ret = fpga_dmac_handle_get(handle, &dma_handle);
	if (ret)
		return ret;
	dma_priv = dma_handle->priv;

	dma_priv->chan_privs = capi_calloc(config->num_chans,
					   sizeof(struct fpga_dmac_chan_priv *));
	if (!dma_priv->chan_privs) {
		ret = -ENOMEM;
		goto error;
	}
	dma_priv->num_chans = config->num_chans;
	dma_priv->base = (uint32_t)config->id;
	dma_handle->ops = config->ops;

	ret = fpga_dmac_irq_setup(dma_handle, dma_priv, config->extra);
	if (ret) {
		capi_free(dma_priv->chan_privs);
		dma_priv->chan_privs = NULL;
		goto error;
	}
	*handle = dma_handle;

	return 0;

error:
	if (dma_handle->init_allocated) {
		capi_free(dma_priv);
		capi_free(dma_handle);
	}

	return ret;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note DMAC: disables the DMAC interrupt, halts (CTRL = 0) and frees every
 *       channel still open.
 * @param handle - Pointer to the CAPI handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dmac_deinit(struct capi_dma_handle *handle)
{
	struct fpga_dmac_priv *dma_priv;
	struct fpga_dmac_chan_priv *cp;
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
			fpga_dmac_wr(cp->base, FPGA_DMAC_REG_CTRL,
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
	} else {
		/* Static handle: later calls (and a second deinit) get -EINVAL. */
		handle->ops = NULL;
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
static int fpga_capi_dmac_init_chan(struct capi_dma_handle *handle,
				    struct capi_dma_chan **chan_ptr,
				    uint32_t id)
{
	struct capi_dma_chan *chan;
	struct fpga_dmac_chan_priv *chan_priv;
	struct fpga_dmac_priv *dma_priv;
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
	ret = fpga_dmac_detect_caps(chan_priv);
	if (ret) {
		capi_free(chan_priv);
		capi_free(chan);
		return ret;
	}

	/* Bring the engine up in a known-idle state: disabled, IRQs pending
	 * acknowledged. */
	fpga_dmac_wr(chan_priv->base, FPGA_DMAC_REG_CTRL, FPGA_DMAC_CTRL_DISABLE);
	fpga_dmac_wr(chan_priv->base, FPGA_DMAC_REG_IRQ_PENDING,
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
static int fpga_capi_dmac_deinit_chan(struct capi_dma_chan *chan)
{
	struct fpga_dmac_chan_priv *chan_priv;
	struct fpga_dmac_priv *dma_priv;

	if (!chan || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;
	dma_priv = chan->handle->priv;

	if (chan_priv) {
		/* Halt the engine before releasing the channel. */
		fpga_dmac_wr(chan_priv->base, FPGA_DMAC_REG_CTRL,
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
 *       Cyclic follows axi_dmac_transfer_start: hardware cyclic when the
 *       core has it and the buffer fits one burst, otherwise software cyclic
 *       (axi_dmac_mem_to_dev_isr), which needs the IRQ. Unlike axi_dmac.c,
 *       software cyclic does not require the core's CYCLIC option.
 *       Alignment is checked at xfer_start.
 * @param chan - Pointer to the CAPI DMA channel.
 * @param xfer - Pointer to the transfer description.
 * @return 0 on success, or:
 *         -EINVAL   bad arguments, zero length, address/length above 32 bit
 *         -ENOTSUP  direction other than the detected one, a memory side
 *                   with CAPI_DMA_NO_INCREMENT, cyclic requested on a
 *                   non-MEM_TO_DEV channel, or a cyclic transfer that needs
 *                   software cyclic (longer than one burst, or no hardware
 *                   cyclic) on a polled channel
 */
static int fpga_capi_dmac_config_xfer(struct capi_dma_chan *chan,
				      struct capi_dma_transfer *xfer)
{
	struct fpga_dmac_chan_priv *chan_priv;
	struct fpga_dmac_priv *dma_priv;
	const struct fpga_dmac_xfer_config *xc;
	enum fpga_dmac_direction dir;
	int ret;

	if (!chan || !xfer || !chan->extra || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;
	dma_priv = chan->handle->priv;
	if (!dma_priv)
		return -EINVAL;

	if (xfer->length == 0)
		return -EINVAL;

	/* The DMAC direction is fixed in fabric and detected at init. */
	dir = fpga_dmac_dir(xfer->xfer_type);
	if (dir == FPGA_DMAC_DIR_INVALID || dir != chan_priv->direction)
		return -ENOTSUP;

	if (xfer->length > UINT32_MAX || xfer->src > UINT32_MAX ||
	    xfer->dst > UINT32_MAX)
		return -EINVAL;

	ret = fpga_dmac_check_inc(dir, xfer->src_inc, xfer->dst_inc);
	if (ret)
		return ret;

	xc = xfer->extra;
	chan_priv->cyclic = false;
	chan_priv->sw_cyclic = false;
	if (xc && xc->cyclic) {
		ret = fpga_dmac_pick_cyclic(chan_priv, dma_priv, dir, xfer->length);
		if (ret)
			return ret;
	}

	/* 2D: one hardware submit with Y_LENGTH and the row strides. */
	chan_priv->rows = 0U;
	chan_priv->src_stride = 0U;
	chan_priv->dst_stride = 0U;
	chan_priv->sg_desc = NULL;
	chan_priv->sg_num = 0U;
	if (xc && xc->rows > 1U) {
		ret = fpga_dmac_check_2d(chan_priv, dir, xfer->length, xc);
		if (ret)
			return ret;
		chan_priv->rows = xc->rows;
		if (dir != FPGA_DMAC_DIR_DEV_TO_MEM)
			chan_priv->src_stride = xc->src_stride;
		if (dir != FPGA_DMAC_DIR_MEM_TO_DEV)
			chan_priv->dst_stride = xc->dst_stride;
	}

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
 *       bursts (and, software cyclic, rewinds and keeps going). Polled
 *       mode: single burst only. Returns once submitted, as
 *       axi_dmac_transfer_start; see fpga_dmac_wait_completion.
 * @param chan - Pointer to the CAPI DMA channel.
 * @return 0 on success, or:
 *         -EINVAL     misaligned address, or polled transfer longer than one
 *                     burst
 *         -EBUSY      DMAC transfer queue full
 */
static int fpga_capi_dmac_xfer_start(struct capi_dma_chan *chan)
{
	struct fpga_dmac_chan_priv *chan_priv;
	struct fpga_dmac_priv *dma_priv;
	uint32_t reg_val;
	int ret;

	if (!chan || !chan->extra || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;
	dma_priv = chan->handle->priv;
	if (!dma_priv || chan_priv->length == 0)
		return -EINVAL;

	if (chan_priv->direction == FPGA_DMAC_DIR_INVALID)
		return -ENOTSUP;
	ret = fpga_dmac_check_align(chan_priv, chan_priv->direction,
				    chan_priv->src, chan_priv->dst);
	if (ret)
		return ret;

	/* Without the SOT interrupt nothing resubmits; polled is one burst. */
	if (!dma_priv->use_irq && chan_priv->length - 1U > chan_priv->max_length)
		return -EINVAL;

	ret = fpga_dmac_irq_rearm(chan->handle, dma_priv);
	if (ret)
		return ret;

	/* Set current transfer parameters. */
	chan_priv->remaining_size = (uint32_t)chan_priv->length;
	chan_priv->next_src_addr = (uint32_t)chan_priv->src;
	chan_priv->next_dest_addr = (uint32_t)chan_priv->dst;
	chan_priv->completed = false;
	chan_priv->pend_head = 0U;
	chan_priv->pend_count = 0U;

	/* Make the source (and the destination, to drop dirty lines that would
	 * be written back over the DMA'd data) visible to the DMAC. */
	fpga_dmac_cache_flush((uintptr_t)chan_priv->src,
			      fpga_dmac_span(chan_priv->length, chan_priv->rows,
					     chan_priv->src_stride));
	fpga_dmac_cache_flush((uintptr_t)chan_priv->dst,
			      fpga_dmac_span(chan_priv->length, chan_priv->rows,
					     chan_priv->dst_stride));

	/* Clear the DMA_CYCLIC flag, then set it again for a cyclic transfer
	 * (checked in config_xfer: MEM_TO_DEV, hw_cyclic, one burst). */
	reg_val = fpga_dmac_rd(chan_priv->base, FPGA_DMAC_REG_FLAGS);
	reg_val &= ~FPGA_DMAC_DMA_CYCLIC;
	if (chan_priv->cyclic)
		reg_val |= FPGA_DMAC_DMA_CYCLIC;
	fpga_dmac_wr(chan_priv->base, FPGA_DMAC_REG_FLAGS, reg_val);

	/* Register mode (drops CTRL.HWDESC left by a scatter-gather run). */
	fpga_dmac_enable(chan_priv->base, false);

	if (fpga_dmac_rd(chan_priv->base, FPGA_DMAC_REG_TRANSFER_SUBMIT) &
	    FPGA_DMAC_QUEUE_FULL)
		return -EBUSY;

	fpga_dmac_submit_burst(chan_priv);

	return 0;
}

/**
 * @brief Build one hardware scatter-gather descriptor.
 * @note DMAC: fills sg_config->extra[index] (struct fpga_dmac_sg_desc) from
 *       the block, links it to the next array entry, marks the last entry
 *       LAST | IRQ (one completion per chain, as Linux dma-axi-dmac.c), and
 *       flushes the block's source/destination. Array mode only.
 * @param chan - Pointer to the CAPI DMA channel.
 * @param sg_config - Chain config; extra = fpga_dmac_sg_desc[num_blocks].
 * @param block - Block transfer; extra may be a 2D fpga_dmac_xfer_config.
 * @param index - Block index, 0 .. num_blocks - 1.
 * @return 0 on success, or:
 *         -ENOTSUP  no SG (or no 2D) in the core, list mode, wrong direction,
 *                   fixed address, cyclic
 *         -EINVAL   bad arguments, zero/oversized length, misaligned address
 *                   or descriptor array
 */
static int fpga_capi_dmac_config_sg(struct capi_dma_chan *chan,
				    const struct capi_dma_sg_config *sg_config,
				    struct capi_dma_transfer *block,
				    uint32_t index)
{
	const struct fpga_dmac_xfer_config *xc;
	struct fpga_dmac_chan_priv *cp;
	struct fpga_dmac_sg_desc *d;
	enum fpga_dmac_direction dir;
	uint32_t rows = 1U, ss = 0U, ds = 0U;
	int ret;

	if (!chan || !chan->extra || !sg_config || !block || !sg_config->extra ||
	    sg_config->num_blocks == 0U || index >= sg_config->num_blocks)
		return -EINVAL;

	cp = chan->extra;
	if (!cp->hw_sg || sg_config->mode != CAPI_DMA_SG_MODE_ARRAY)
		return -ENOTSUP;
	/* dmac_sg.v fetches descriptors in 64-bit beats. */
	if ((uintptr_t)sg_config->extra % 8U)
		return -EINVAL;

	dir = fpga_dmac_dir(sg_config->xfer_type);
	if (dir == FPGA_DMAC_DIR_INVALID || dir != cp->direction ||
	    fpga_dmac_dir(block->xfer_type) != dir)
		return -ENOTSUP;
	ret = fpga_dmac_check_inc(dir, block->src_inc, block->dst_inc);
	if (ret)
		return ret;

	if (block->length == 0U || block->length - 1U > cp->max_length ||
	    block->src > UINT32_MAX || block->dst > UINT32_MAX)
		return -EINVAL;
	ret = fpga_dmac_check_align(cp, dir, block->src, block->dst);
	if (ret)
		return ret;

	xc = block->extra;
	if (xc && xc->cyclic)
		return -ENOTSUP;
	if (xc && xc->rows > 1U) {
		ret = fpga_dmac_check_2d(cp, dir, block->length, xc);
		if (ret)
			return ret;
		rows = xc->rows;
		ss = xc->src_stride;
		ds = xc->dst_stride;
	}

	d = &((struct fpga_dmac_sg_desc *)sg_config->extra)[index];
	memset(d, 0, sizeof(*d));
	d->flags = (index == sg_config->num_blocks - 1U) ?
		   (FPGA_DMAC_SG_FLAG_LAST | FPGA_DMAC_SG_FLAG_IRQ) : 0U;
	d->next_sg_addr = (index + 1U < sg_config->num_blocks) ?
			  (uint64_t)(uintptr_t)(d + 1) : 0U;
	d->x_len = (uint32_t)block->length - 1U;
	d->y_len = rows - 1U;
	if (dir != FPGA_DMAC_DIR_DEV_TO_MEM) {
		d->src_addr = (uint64_t)block->src;
		d->src_stride = ss;
		fpga_dmac_cache_flush((uintptr_t)block->src,
				      fpga_dmac_span(block->length, rows, ss));
	}
	if (dir != FPGA_DMAC_DIR_MEM_TO_DEV) {
		d->dest_addr = (uint64_t)block->dst;
		d->dst_stride = ds;
		fpga_dmac_cache_flush((uintptr_t)block->dst,
				      fpga_dmac_span(block->length, rows, ds));
	}

	return 0;
}

/**
 * @brief Start a scatter-gather chain built by config_sg.
 * @note DMAC: stamps the transfer id into every descriptor, flushes the
 *       array, enables descriptor mode (CTRL.HWDESC), writes SG_ADDRESS and
 *       submits once and returns; the chain completes as one transfer
 *       (same completion rules as xfer_start).
 * @param chan - Pointer to the CAPI DMA channel.
 * @param sg_config - Chain config whose descriptors are all built.
 * @return 0 on success, or:
 *         -ENOTSUP    no SG in the core, list mode
 *         -EINVAL     bad arguments, last descriptor not built
 *         -EBUSY      DMAC transfer queue full
 */
static int fpga_capi_dmac_start_sg(struct capi_dma_chan *chan,
				   const struct capi_dma_sg_config *sg_config)
{
	struct fpga_dmac_chan_priv *cp;
	struct fpga_dmac_priv *dma_priv;
	struct fpga_dmac_sg_desc *d;
	uint32_t i, id;
	int ret;

	if (!chan || !chan->extra || !chan->handle || !sg_config ||
	    !sg_config->extra || sg_config->num_blocks == 0U)
		return -EINVAL;

	cp = chan->extra;
	dma_priv = chan->handle->priv;
	if (!dma_priv)
		return -EINVAL;
	if (!cp->hw_sg || sg_config->mode != CAPI_DMA_SG_MODE_ARRAY)
		return -ENOTSUP;

	d = sg_config->extra;
	if (!(d[sg_config->num_blocks - 1U].flags & FPGA_DMAC_SG_FLAG_LAST))
		return -EINVAL;

	ret = fpga_dmac_irq_rearm(chan->handle, dma_priv);
	if (ret)
		return ret;

	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_FLAGS, 0U);
	fpga_dmac_enable(cp->base, true);

	if (fpga_dmac_rd(cp->base, FPGA_DMAC_REG_TRANSFER_SUBMIT) &
	    FPGA_DMAC_QUEUE_FULL)
		return -EBUSY;

	/* The whole chain is one transfer: one id, one TRANSFER_DONE bit. */
	id = fpga_dmac_rd(cp->base, FPGA_DMAC_REG_TRANSFER_ID) &
	     FPGA_DMAC_TRANSFER_ID_MASK;
	for (i = 0; i < sg_config->num_blocks; i++)
		d[i].id = id;
	fpga_dmac_cache_flush((uintptr_t)d, sg_config->num_blocks * sizeof(*d));

	cp->cyclic = false;
	cp->sw_cyclic = false;
	cp->rows = 0U;
	cp->src_stride = 0U;
	cp->dst_stride = 0U;
	cp->sg_desc = d;
	cp->sg_num = sg_config->num_blocks;
	cp->length = 1U;
	cp->remaining_size = 0U;
	cp->pend_count = 0U;
	cp->last_id = id;
	cp->completed = false;

	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_SG_ADDRESS, (uint32_t)(uintptr_t)d);
	fpga_dmac_wr(cp->base, FPGA_DMAC_REG_TRANSFER_SUBMIT,
		     FPGA_DMAC_TRANSFER_SUBMIT);

	return 0;
}

/**
 * @brief Abort an ongoing DMA transfer.
 * @note DMAC: halts the engine (CTRL = 0), then clears IRQ_PENDING and
 *       the remaining bursts.
 * @param chan - Pointer to the CAPI DMA channel.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dmac_xfer_abort(struct capi_dma_chan *chan)
{
	struct fpga_dmac_chan_priv *chan_priv;
	uint32_t waited;

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;

	/* Halt the engine; also drop the pending status and the burst
	 * bookkeeping so the ISR does not resubmit. */
	fpga_dmac_wr(chan_priv->base, FPGA_DMAC_REG_CTRL, FPGA_DMAC_CTRL_DISABLE);
	/* A stream destination stays enabled until its pending beat is taken
	 * (dest_axi_stream.v), so the core sits in SHUTDOWN and ignores the
	 * next ENABLE while its consumer is idle. Give a still-running consumer
	 * (e.g. an SPI Engine offload fed by MM2S) time to drain it. */
	for (waited = 0U; waited < FPGA_DMAC_ABORT_DRAIN_US &&
	     (fpga_dmac_rd(chan_priv->base, FPGA_DMAC_REG_DBG_STATUS) &
	      FPGA_DMAC_DBG_STATE_MASK) == FPGA_DMAC_DBG_STATE_SHUTDOWN;
	     waited++)
		capi_wait_us(1);
	chan_priv->remaining_size = 0;
	chan_priv->cyclic = false;
	chan_priv->sw_cyclic = false;
	chan_priv->pend_count = 0U;
	fpga_dmac_wr(chan_priv->base, FPGA_DMAC_REG_IRQ_PENDING,
		     FPGA_DMAC_IRQ_SOT | FPGA_DMAC_IRQ_EOT);
	chan_priv->completed = true;

	return 0;
}

/**
 * @brief Report whether the channel's transfer has completed.
 * @note DMAC: IRQ mode returns the flag set by the EOT interrupt; polled
 *       mode reads TRANSFER_DONE and completes the transfer when it is set
 *       (axi_dmac_is_transfer_ready).
 * @param chan - Pointer to the CAPI DMA channel.
 * @return true if completed (or idle), false otherwise.
 */
static bool fpga_capi_dmac_chan_is_completed(const struct capi_dma_chan *chan)
{
	if (!chan || !chan->extra || !chan->handle || !chan->handle->priv)
		return true;

	return fpga_dmac_poll(chan->extra, chan->handle->priv);
}

/**
 * @brief Register the transfer-completion callback for a channel.
 * @note DMAC: delivered from the EOT interrupt path by xfer_start.
 * @param chan - Pointer to the CAPI DMA channel.
 * @param callback - Completion callback (NULL to unregister).
 * @param xfer_complete_ctx - User context passed to the callback.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dmac_register_complete_callback(
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
static void fpga_capi_dmac_hw_isr(void *ctx)
{
	struct capi_dma_handle *handle = ctx;
	struct fpga_dmac_priv *dma_priv;
	uint32_t i;

	if (!handle || !handle->priv)
		return;

	dma_priv = handle->priv;

	for (i = 0; i < dma_priv->num_chans; i++)
		(void)fpga_dmac_service_chan(dma_priv->chan_privs[i]);
}

/**
 * @brief DMA controller interrupt handler (for a user-managed vector).
 * @note DMAC: services SOT/EOT on every channel.
 * @param handle - Pointer to the CAPI DMA handle.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dmac_isr(struct capi_dma_handle *handle)
{
	if (!handle)
		return -EINVAL;

	fpga_capi_dmac_hw_isr(handle);
	return 0;
}

/**
 * @brief DMA channel interrupt handler (for a user-managed vector).
 * @note DMAC: services SOT/EOT on one channel.
 * @param chan - Pointer to the CAPI DMA channel.
 * @return 0 on success, negative errno on failure.
 */
static int fpga_capi_dmac_isr_chan(struct capi_dma_chan *chan)
{
	if (!chan || !chan->extra)
		return -EINVAL;

	(void)fpga_dmac_service_chan(chan->extra);
	return 0;
}

/* --- public helpers -------------------------------------------------------- */

/**
 * @brief Read the DMAC bus widths detected at init_chan.
 * @note DMAC: width_src / width_dst from INTF_DESC, as axi_dmac_init.
 * @param chan - Channel opened on fpga_capi_dmac_ops.
 * @param src_bytes - Output: source width in bytes (may be NULL).
 * @param dst_bytes - Output: destination width in bytes (may be NULL).
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_dmac_get_width(const struct capi_dma_chan *chan, uint32_t *src_bytes,
			uint32_t *dst_bytes)
{
	const struct fpga_dmac_chan_priv *cp;

	if (!chan || !chan->extra || !chan->handle ||
	    chan->handle->ops != &fpga_capi_dmac_ops)
		return -EINVAL;

	cp = chan->extra;
	if (src_bytes)
		*src_bytes = cp->width_src;
	if (dst_bytes)
		*dst_bytes = cp->width_dst;

	return 0;
}

/**
 * @brief Read the longest single burst the DMAC accepts.
 * @note DMAC: max_length (X_LENGTH all-ones read-back) + 1, as axi_dmac_init.
 * @param chan - Channel opened on fpga_capi_dmac_ops.
 * @param bytes - Output: maximum burst length in bytes.
 * @return 0 on success, -EINVAL on bad arguments.
 */
int fpga_dmac_get_max_burst(const struct capi_dma_chan *chan, uint32_t *bytes)
{
	const struct fpga_dmac_chan_priv *cp;

	if (!chan || !chan->extra || !chan->handle || !bytes ||
	    chan->handle->ops != &fpga_capi_dmac_ops)
		return -EINVAL;

	cp = chan->extra;
	*bytes = cp->max_length + 1U;

	return 0;
}

int fpga_dmac_wait_completion(struct capi_dma_chan *chan, uint32_t timeout_ms)
{
	struct fpga_dmac_chan_priv *cp;
	uint32_t ms, us;

	if (!chan || !chan->extra || !chan->handle || !chan->handle->priv ||
	    chan->handle->ops != &fpga_capi_dmac_ops)
		return -EINVAL;

	cp = chan->extra;
	if (!cp->completed && (cp->cyclic || cp->sw_cyclic))
		return -EINVAL;

	for (ms = 0U; !timeout_ms || ms < timeout_ms; ms++)
		for (us = 0U; us < 1000U; us++) {
			if (fpga_dmac_poll(cp, chan->handle->priv))
				return 0;
			capi_wait_us(1U);
		}

	return fpga_dmac_poll(cp, chan->handle->priv) ? 0 : -ETIMEDOUT;
}

int fpga_dmac_get_features(const struct capi_dma_chan *chan, uint32_t *mask)
{
	const struct fpga_dmac_chan_priv *cp;

	if (!chan || !chan->extra || !mask)
		return -EINVAL;

	cp = chan->extra;
	*mask = (cp->hw_cyclic ? FPGA_DMAC_FEAT_CYCLIC : 0U) |
		(cp->hw_2d ? FPGA_DMAC_FEAT_2D : 0U) |
		(cp->hw_sg ? FPGA_DMAC_FEAT_SG : 0U);

	return 0;
}

const struct capi_dma_ops fpga_capi_dmac_ops = {
	.init = fpga_capi_dmac_init,
	.deinit = fpga_capi_dmac_deinit,
	.init_chan = fpga_capi_dmac_init_chan,
	.deinit_chan = fpga_capi_dmac_deinit_chan,
	.config_xfer = fpga_capi_dmac_config_xfer,
	.xfer_start = fpga_capi_dmac_xfer_start,
	.xfer_abort = fpga_capi_dmac_xfer_abort,
	.chan_is_completed = fpga_capi_dmac_chan_is_completed,
	.isr = fpga_capi_dmac_isr,
	.isr_chan = fpga_capi_dmac_isr_chan,
	.register_complete_callback = fpga_capi_dmac_register_complete_callback,
	.config_scattergather_xfer = fpga_capi_dmac_config_sg,
	.start_scattergather_xfer = fpga_capi_dmac_start_sg,
};
