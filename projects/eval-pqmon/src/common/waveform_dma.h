/******************************************************************************
 *   @file   waveform_dma.h
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

#ifndef WAVEFORM_DMA_H_
#define WAVEFORM_DMA_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

/* SINC4: 128 samples/channel x 6 channels x 4 bytes/sample (no IN) */
#define WF_DMA_SINC4_SAMPLES_PER_CH	128
#define WF_DMA_NUM_CHANNELS		6
#define WF_DMA_SINC4_TOTAL_SAMPLES	(WF_DMA_SINC4_SAMPLES_PER_CH * WF_DMA_NUM_CHANNELS)
#define WF_DMA_SINC4_DATA_BYTES	(WF_DMA_SINC4_TOTAL_SAMPLES * 4)
#define WF_DMA_SPI_HDR_BYTES		2
#define WF_DMA_XFER_BYTES		(WF_DMA_SPI_HDR_BYTES + WF_DMA_SINC4_DATA_BYTES)

typedef void (*wf_block_ready_cb)(uint32_t *data, uint16_t num_samples);

/**
 * @brief Initialize waveform DMA module with given SPI descriptor. Allocates necessary resources.
 * @param spi Pointer to initialized SPI descriptor for ADE9430 communication
 * @return 0 on success, negative error code on failure
 */
int waveform_dma_init(struct no_os_spi_desc *spi);

/**
 * @brief Start asynchronous DMA transfer to read waveform data from ADE9430. Non-blocking.
 * @param waveform_addr Register address to read waveform data from
 * @return 0 on successful start, negative error code on failure
 */
int waveform_dma_start(uint16_t waveform_addr);

/**
 * @brief Check if waveform DMA transfer is currently in progress.
 * @return true if DMA transfer is active, false otherwise
 */
bool waveform_dma_is_busy(void);

/**
 * @brief Check if waveform DMA transfer has completed and data is ready to be processed.
 * @return true if DMA transfer is done and data is ready, false otherwise
 */
bool waveform_dma_is_done(void);

/**
 * @brief Process completed DMA transfer and copy data to provided destination buffer.
 * Caller must ensure buffer is large enough.
 * @param dst Pointer to buffer where waveform data should be copied
 * @return true if data was successfully processed and copied, false if DMA transfer is not done or an error occurred
 */
bool waveform_dma_process(uint32_t *dst);

/**
 * @brief Get a block of waveform data from the completed DMA transfer. Caller must ensure buffer is large enough.
 * @param dst Pointer to buffer where waveform data should be copied
 * @param num_samples Number of 32-bit samples to copy
 * @return Number of samples actually copied, or negative error code on failure
 */
int waveform_dma_get_data(uint32_t *dst, uint16_t num_samples);

/**
 * @brief Set a callback function to be called when a block of waveform data is ready after DMA transfer completion.
 * @param cb Callback function pointer, or NULL to clear the callback
 */
void waveform_dma_set_callback(wf_block_ready_cb cb);

/**
 * @brief Set the register address for the next DMA transfer to be started after the current one completes.
 * This allows chaining multiple DMA transfers without CPU intervention.
 * @param addr Register address to read waveform data from in the next DMA transfer
 */
void waveform_dma_set_deferred(uint16_t addr);

/**
 * @brief Abort the current DMA transfer if it is in progress. After aborting, the module will be in idle state.
 */
void waveform_dma_abort(void);

/**
 * @brief Deinitialize waveform DMA module and free any allocated resources.
 * Should be called when the module is no longer needed.
 */
void waveform_dma_remove(void);

#endif /* WAVEFORM_DMA_H_ */
