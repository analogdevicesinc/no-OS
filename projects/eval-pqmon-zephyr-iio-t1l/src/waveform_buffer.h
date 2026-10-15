/*******************************************************************************
 *   @file   waveform_buffer.h
 *   @brief  Waveform ring buffer for ADE9430 raw data export
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
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
 *******************************************************************************/

#ifndef __WAVEFORM_BUFFER_H__
#define __WAVEFORM_BUFFER_H__

#include <stdint.h>

/* 7 channels × 256 samples × 2 bytes = 3584 bytes per block */
#define WF_NUM_CHANNELS    7
#define WF_SAMPLES_PER_CH  256
#define WF_BLOCK_SAMPLES   (WF_NUM_CHANNELS * WF_SAMPLES_PER_CH)
#define WF_BLOCK_SIZE      (WF_BLOCK_SAMPLES * sizeof(int16_t))

/* Ring buffer depth — 16 blocks (~800ms at 20 blocks/s) absorbs UART
 * latency plus once-per-second stalls from the COOP(0) 1PPS/RTC-sync
 * thread (scheduler-locked SPI work) that would otherwise overflow a
 * shallow ring and drop waveform frames. 16 × 3604B ≈ 57.7 KB. */
#define WF_RING_DEPTH      16

/** PTP timestamp for a waveform block */
struct wf_timestamp {
	uint32_t sec;
	uint32_t nsec;
};

/**
 * @brief Initialize the waveform ring buffer
 *
 * Must be called before waveform_buffer_put(). If
 * CONFIG_WAVEFORM_UART_EXPORT is enabled, also spawns the
 * UART2 binary export thread.
 */
void waveform_buffer_init(void);

/**
 * @brief Store a waveform block into the ring buffer
 *
 * Called from the AFE thread after afe_read_waveform().
 * Fast path: memcpy + atomic index advance + sem give.
 * If the ring is full, the oldest unread block is overwritten.
 *
 * @param data  Pointer to int16_t[WF_BLOCK_SAMPLES] waveform data
 * @param seq   Waveform sequence number (from PQLib)
 * @param ts    PTP timestamp captured at COH_PAGE_RDY
 */
void waveform_buffer_put(const int16_t *data, uint32_t seq,
			 const struct wf_timestamp *ts);

/**
 * @brief Read the next waveform block from the ring buffer
 *
 * Blocks on a semaphore until data is available.
 *
 * @param out   Buffer to receive int16_t[WF_BLOCK_SAMPLES]
 * @param seq   Receives the block's sequence number
 * @param ts    Receives the block's PTP timestamp
 * @return 0 on success, negative on error
 */
int waveform_buffer_get(int16_t *out, uint32_t *seq,
			struct wf_timestamp *ts);

#endif /* __WAVEFORM_BUFFER_H__ */
