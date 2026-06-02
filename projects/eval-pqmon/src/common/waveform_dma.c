/******************************************************************************
 *   @file   waveform_dma.c
 *   @brief  Async DMA SPI for ADE9430 SINC4 waveform reads.
 *   @author Radu Etz (radu.etz@analog.com)
 ******************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "waveform_dma.h"
#include "no_os_dma.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_irq.h"
#include "maxim_dma.h"
#include "maxim_spi.h"
#include "spi.h"
#include <string.h>
#include <stdio.h>

static uint8_t dma_cmd_buf[2][WF_DMA_XFER_BYTES] __attribute__((aligned(4)));
static volatile uint8_t dma_buf_idx;

static struct no_os_spi_desc *spi_desc;
static mxc_spi_regs_t *spi_regs;

static volatile struct max_dma_ch_regs *tx_ch_regs;
static volatile struct max_dma_ch_regs *rx_ch_regs;
static uint32_t tx_ch_id;
static uint32_t rx_ch_id;
static uint32_t tx_reqsel;
static uint32_t rx_reqsel;

static struct no_os_irq_ctrl_desc *dma_irq_ctrl;

static volatile bool xfer_busy;
static volatile bool xfer_done;
static volatile uint8_t completed_buf_idx;

static wf_block_ready_cb block_ready_cb;

static bool initialized;

/* Deferred start support — set by caller, acted on by DMA ISR */
static volatile bool deferred_pending;
static volatile uint16_t deferred_addr;

/**
 * @brief DMA ISR for TX channel CTZ (end of transfer) interrupt.
 * Clears flags, ends SPI transaction, and notifies caller.
 */
static void wf_dma_tx_isr(void *ctx)
{
	(void)ctx;

	/* Clear TX channel CTZ interrupt flag first to prevent re-fire */
	tx_ch_regs->st |= NO_OS_BIT(2);

	/* Wait for RX DMA to finish (RX trails TX by a few clocks) */
	while (rx_ch_regs->st & MAX_DMA_ENABLED)
		;

	/* Clear RX channel status flags */
	rx_ch_regs->st |= NO_OS_BIT(2);

	/* Wait for SPI to finish clocking */
	while (spi_regs->stat & 1)
		;

	/* End the SPI transaction */
	spi_regs->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
	spi_regs->dma &= ~(MXC_F_SPI_DMA_TX_FIFO_EN |
			   MXC_F_SPI_DMA_RX_FIFO_EN |
			   MXC_F_SPI_DMA_TX_DMA_EN |
			   MXC_F_SPI_DMA_RX_DMA_EN);

	/* Save completed buffer index, flip for next DMA */
	completed_buf_idx = dma_buf_idx;
	dma_buf_idx ^= 1;
	xfer_busy = false;
	xfer_done = true;

	/* Notify caller — passes raw (non-swapped) data pointer.*/
	if (block_ready_cb)
		block_ready_cb(
			(uint32_t *)&dma_cmd_buf[completed_buf_idx][WF_DMA_SPI_HDR_BYTES],
			WF_DMA_SINC4_TOTAL_SAMPLES);

	/* If a PAGE_FULL edge arrived while DMA was running, chain immediately */
	if (deferred_pending) {
		deferred_pending = false;
		waveform_dma_start(deferred_addr);
	}
}

/**
 * @brief Initialize the waveform DMA system with the given SPI descriptor.
 * Acquires DMA channels, sets up ISR, and prepares for transfers.
 * @param spi Pointer to the SPI descriptor for the target SPI peripheral.
 * @return 0 on success, negative error code on failure.
 */
int waveform_dma_init(struct no_os_spi_desc *spi)
{
	struct max_spi_state *spi_state;
	struct no_os_dma_desc *dma_desc;
	struct no_os_dma_ch *tx_ch, *rx_ch;
	int ret;

	if (!spi)
		return -EINVAL;

	spi_desc = spi;
	spi_state = (struct max_spi_state *)spi->extra;
	spi_regs = MXC_SPI_GET_SPI(spi->device_id);

	if (!spi_state->dma)
		return -ENODEV;

	dma_desc = spi_state->dma;

	/* Acquire two DMA channels (uses mutex — safe in init context) */
	ret = no_os_dma_acquire_channel(dma_desc, &tx_ch);
	if (ret)
		return ret;

	ret = no_os_dma_acquire_channel(dma_desc, &rx_ch);
	if (ret) {
		no_os_dma_release_channel(dma_desc, tx_ch);
		return ret;
	}

	/* Cache raw register pointers for ISR-safe direct access */
	tx_ch_id = tx_ch->id;
	rx_ch_id = rx_ch->id;
	tx_ch_regs = (volatile struct max_dma_ch_regs *)tx_ch->extra;
	rx_ch_regs = (volatile struct max_dma_ch_regs *)rx_ch->extra;

	/* Compute DMA request select values for SPI TX/RX */
	tx_reqsel = spi_state->dma_req_tx;
	rx_reqsel = spi_state->dma_req_rx;

	/* Save IRQ controller for enable/disable */
	dma_irq_ctrl = dma_desc->irq_ctrl;

	/* Register our raw ISR callback for the TX channel CTZ interrupt */
	static struct no_os_callback_desc tx_cb_desc = {0};
	tx_cb_desc.callback = wf_dma_tx_isr;
	tx_cb_desc.ctx = NULL;
	tx_cb_desc.event = NO_OS_EVT_DMA_TX_COMPLETE;
	tx_cb_desc.peripheral = NO_OS_DMA_IRQ;
	tx_cb_desc.handle = NULL;

	ret = no_os_irq_register_callback(dma_irq_ctrl, tx_ch->irq_num,
					  &tx_cb_desc);
	if (ret) {
		no_os_dma_release_channel(dma_desc, rx_ch);
		no_os_dma_release_channel(dma_desc, tx_ch);
		return ret;
	}

	/* Set DMA IRQ to highest priority so it can preempt GPIO ISR */
	no_os_irq_set_priority(dma_irq_ctrl, tx_ch->irq_num, 0);

	/* Enable the DMA IRQ for our TX channel */
	ret = no_os_irq_enable(dma_irq_ctrl, tx_ch->irq_num);
	if (ret) {
		no_os_dma_release_channel(dma_desc, rx_ch);
		no_os_dma_release_channel(dma_desc, tx_ch);
		return ret;
	}

	/* Pre-configure static parts of channel cfg registers */
	tx_ch_regs->cfg = MAX_DMA_SRC_INC |
			  no_os_field_prep(MAX_DMA_REQSEL_MASK, tx_reqsel) |
			  MAX_DMA_IRQ_CTZ_EN;

	rx_ch_regs->cfg = MAX_DMA_DST_INC |
			  no_os_field_prep(MAX_DMA_REQSEL_MASK, rx_reqsel);

	memset(dma_cmd_buf, 0, sizeof(dma_cmd_buf));

	dma_buf_idx = 0;
	completed_buf_idx = 0;
	deferred_pending = false;
	xfer_busy = false;
	xfer_done = false;
	initialized = true;

	return 0;
}

/**
 * @brief Perform a fast raw SPI PIO write to clear the PAGE_FULL bit in STATUS0.
 * This is needed before starting a DMA transfer to ensure we get a clean PAGE_FULL edge.
 */
static void raw_spi_clear_page_full(void)
{
	/* 6-byte PIO write: REG_STATUS0(0x0402) = 0x00020000 (PAGE_FULL bit).*/
	static const uint8_t cmd[6] = {0x40, 0x20, 0x00, 0x02, 0x00, 0x00};
	int i;

	spi_regs->dma |= MXC_F_SPI_DMA_TX_FIFO_CLEAR | MXC_F_SPI_DMA_RX_FIFO_CLEAR;
	spi_regs->int_fl |= MXC_F_SPI_INT_FL_M_DONE;

	spi_regs->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_CTRL;
	spi_regs->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_SEL;
	spi_regs->ctrl0 |= no_os_field_prep(MXC_F_SPI_CTRL0_SS_SEL,
					    NO_OS_BIT(spi_desc->chip_select));

	spi_regs->ctrl1 = 6;
	spi_regs->ctrl1 |= no_os_field_prep(MXC_F_SPI_CTRL1_RX_NUM_CHAR, 6);

	spi_regs->dma |= MXC_F_SPI_DMA_TX_FIFO_EN;

	for (i = 0; i < 6; i++)
		spi_regs->fifo8[0] = cmd[i];

	spi_regs->ctrl0 |= MXC_F_SPI_CTRL0_START;

	while (!(spi_regs->int_fl & MXC_F_SPI_INT_FL_M_DONE))
		;

	spi_regs->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
	spi_regs->dma &= ~MXC_F_SPI_DMA_TX_FIFO_EN;
	spi_regs->int_fl |= MXC_F_SPI_INT_FL_M_DONE;
}

/**
 * @brief Start a DMA transfer to read waveform data from the ADE9430 at the given address.
 * Builds the SPI command, configures DMA channels, and starts the SPI transaction.
 * @param waveform_addr address of the waveform register to read
 * @return 0 on success, negative error code on failure (e.g. if a transfer is already in progress).
 */
int waveform_dma_start(uint16_t waveform_addr)
{
	uint32_t xfer_len = WF_DMA_XFER_BYTES;
	uint8_t *buf = dma_cmd_buf[dma_buf_idx];

	if (!initialized)
		return -ENODEV;

	if (xfer_busy)
		return -EBUSY;

	/* Clear STATUS0.PAGE_FULL via fast raw PIO (~6µs).
	 * De-asserts /IRQ0 so next PAGE_FULL produces a fresh edge. */
	raw_spi_clear_page_full();

	xfer_busy = true;
	xfer_done = false;

	/* Build SPI command header */
	buf[0] = (waveform_addr >> 4);
	buf[1] = ((waveform_addr & 0x0F) << 4) + 8;
	memset(&buf[WF_DMA_SPI_HDR_BYTES], 0, WF_DMA_SINC4_DATA_BYTES);

	/* Clear and prepare SPI FIFOs */
	spi_regs->dma |= MXC_F_SPI_DMA_RX_FIFO_CLEAR | MXC_F_SPI_DMA_TX_FIFO_CLEAR;
	spi_regs->int_fl |= MXC_F_SPI_INT_FL_M_DONE;

	/* Configure chip select */
	spi_regs->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_CTRL;
	spi_regs->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_SEL;
	spi_regs->ctrl0 |= no_os_field_prep(MXC_F_SPI_CTRL0_SS_SEL,
					    NO_OS_BIT(spi_desc->chip_select));

	/* Set transfer length */
	spi_regs->ctrl1 = xfer_len;
	spi_regs->ctrl1 |= no_os_field_prep(MXC_F_SPI_CTRL1_RX_NUM_CHAR,
					    xfer_len);

	/* Enable SPI FIFOs and DMA mode */
	spi_regs->dma |= MXC_F_SPI_DMA_TX_FIFO_EN;
	spi_regs->dma |= MXC_F_SPI_DMA_RX_FIFO_EN;
	spi_regs->dma |= MXC_F_SPI_DMA_RX_DMA_EN | MXC_F_SPI_DMA_TX_DMA_EN;

	/* Program DMA channels */

	/* RX channel: SPI FIFO → buf */
	rx_ch_regs->src = rx_reqsel;
	rx_ch_regs->dst = (uint32_t)buf;
	rx_ch_regs->cnt = xfer_len;

	/* TX channel: buf → SPI FIFO */
	tx_ch_regs->src = (uint32_t)buf;
	tx_ch_regs->dst = tx_reqsel;
	tx_ch_regs->cnt = xfer_len;

	/* Start SPI transaction */
	spi_regs->ctrl0 |= MXC_F_SPI_CTRL0_START;

	/* Re-arm full cfg and enable DMA channels (RX first so it's ready to receive) */
	rx_ch_regs->cfg = MAX_DMA_DST_INC |
			  no_os_field_prep(MAX_DMA_REQSEL_MASK, rx_reqsel) |
			  MAX_DMA_ENABLE;

	tx_ch_regs->cfg = MAX_DMA_SRC_INC |
			  no_os_field_prep(MAX_DMA_REQSEL_MASK, tx_reqsel) |
			  MAX_DMA_IRQ_CTZ_EN |
			  MAX_DMA_ENABLE;

	return 0;
}

/**
 * @brief Set the callback function to be called when a DMA block is ready.
 * The callback receives a pointer to the raw data block and the number of samples.
 * @param cb Function pointer to the callback to set.
 */
void waveform_dma_set_callback(wf_block_ready_cb cb)
{
	block_ready_cb = cb;
}

/**
 * @brief Check if a DMA transfer is currently in progress.
 * @return true if a transfer is in progress, false otherwise.
 */
bool waveform_dma_is_busy(void)
{
	return xfer_busy;
}

/**
 * @brief Check if a DMA transfer has completed and data is ready to be processed.
 * @return true if data is ready, false otherwise.
 */
bool waveform_dma_is_done(void)
{
	return xfer_done;
}

/**
 * @brief Get the data from the completed DMA transfer into the provided destination buffer.
 * @param dst Pointer to the destination buffer where the data should be copied (must be large enough for num_samples).
 * @param num_samples Number of 32-bit samples to copy from the DMA buffer to the destination buffer.
 * @return 0 on success, negative error code if data is not ready or if parameters are invalid.
 */
int waveform_dma_get_data(uint32_t *dst, uint16_t num_samples)
{
	if (!xfer_done)
		return -EBUSY;

	memcpy((uint8_t *)dst, &dma_cmd_buf[completed_buf_idx][WF_DMA_SPI_HDR_BYTES],
	       num_samples * sizeof(uint32_t));
	no_os_memswap64((void *)dst, num_samples * sizeof(uint32_t), 4);

	xfer_done = false;

	return 0;
}

/**
 * @brief Convenience function to check if DMA data is ready and get it in one call.
 * @param dst Pointer to the destination buffer where the data should be copied (must be large enough for num_samples).
 * @return true if data was ready and copied, false if data was not ready.
 */
bool waveform_dma_process(uint32_t *dst)
{
	if (!xfer_done)
		return false;

	memcpy((uint8_t *)dst,
	       &dma_cmd_buf[completed_buf_idx][WF_DMA_SPI_HDR_BYTES],
	       WF_DMA_SINC4_DATA_BYTES);
	no_os_memswap64((void *)dst, WF_DMA_SINC4_DATA_BYTES, 4);

	xfer_done = false;
	return true;
}

/**
 * @brief Set a deferred DMA start request that will be triggered immediately after the current transfer completes.
 * This is used to chain DMA transfers without gaps when a new PAGE_FULL edge arrives during an ongoing transfer.
 * @param addr The waveform register address for the next transfer to start after the current one completes.
 */
void waveform_dma_set_deferred(uint16_t addr)
{
	deferred_addr = addr;
	deferred_pending = true;
}

/**
 * @brief Abort the current DMA transfer if it is in progress. Disables DMA channels and SPI transaction immediately.
 */
void waveform_dma_abort(void)
{
	if (!xfer_busy)
		return;

	/* Disable DMA channels directly */
	tx_ch_regs->cfg &= ~MAX_DMA_ENABLE;
	rx_ch_regs->cfg &= ~MAX_DMA_ENABLE;

	spi_regs->ctrl0 &= ~MXC_F_SPI_CTRL0_START;
	spi_regs->dma &= ~(MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN);

	xfer_busy = false;
	xfer_done = false;
}

/**
 * @brief Clean up the waveform DMA system. Aborts any ongoing transfer, disables DMA channels, and resets state.
 */
void waveform_dma_remove(void)
{
	if (!initialized)
		return;

	if (xfer_busy)
		waveform_dma_abort();

	/* Disable DMA channels */
	tx_ch_regs->cfg &= ~MAX_DMA_ENABLE;
	rx_ch_regs->cfg &= ~MAX_DMA_ENABLE;

	xfer_busy = false;
	xfer_done = false;
	initialized = false;
}
