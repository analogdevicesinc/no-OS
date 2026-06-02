/******************************************************************************
 *   @file   usb_raw_stream.h
 *   @brief  Raw waveform streaming over USB bulk endpoint (bypasses CDC-ACM FIFO).
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

#ifndef USB_RAW_STREAM_H_
#define USB_RAW_STREAM_H_

#include <stdint.h>
#include <stdbool.h>

#include "waveform_dma.h"

#define USB_RAW_STREAM_SYNC_WORD	0xDEADBEEF
#define USB_RAW_STREAM_HEADER_BYTES	16
#define USB_RAW_STREAM_DATA_SAMPLES	WF_DMA_SINC4_TOTAL_SAMPLES
#define USB_RAW_STREAM_DATA_BYTES	(USB_RAW_STREAM_DATA_SAMPLES * 4)
#define USB_RAW_STREAM_FRAME_BYTES	(USB_RAW_STREAM_HEADER_BYTES + USB_RAW_STREAM_DATA_BYTES)

/* Bulk IN endpoint number — from USB descriptor (bEndpointAddress 0x83 & 0x7) */
#define USB_RAW_STREAM_IN_EP		3

typedef void (*usb_raw_stream_pump_cb)(void);

/**
 * @brief Initialize USB raw stream module and allocate resources.
 */
int usb_raw_stream_init(void);

/**
 * @brief Deinitialize USB raw stream module and free resources.
 */
int usb_raw_stream_start(void);

/**
 * @brief Stop USB raw stream and return to idle state.
 */
int usb_raw_stream_stop(void);

/**
 * @brief Check if USB raw stream is active (started and not stopped yet).
 */
bool usb_raw_stream_is_active(void);

/**
 * @brief Send a block of data as a USB frame with header. Caller must ensure data is valid until callback.
 * @param data Pointer to block of 32-bit samples (size = block_count * 4 bytes)
 * @param block_count Number of 32-bit samples in the block
 * @param seq_cycle Sequence cycle number (for header, can be used for tracking dropped frames)
 * @param seq_1012 Sequence number within cycle (for header, can be used for tracking dropped frames)
 * @return 0 if send started successfully, -1 if error (e.g. not configured or previous send still in progress)
 */
int usb_raw_stream_send_block(uint32_t *data, uint32_t block_count,
			      uint32_t seq_cycle, uint32_t seq_1012);

/**
 * @brief Get the total number of frames that were dropped
 * (send attempted while previous send still in progress).
 */
uint32_t usb_raw_stream_get_drop_count(void);

/**
 * @brief Get timing information about the last frame transmission
 * @param last_us Pointer to variable to receive time taken for last frame transmission (in microseconds) NOT USED
 * @param max_us Pointer to variable to receive maximum time taken for any frame transmission (in microseconds) NOT USED
 * @param count Pointer to variable to receive total number of frames transmitted since last reset
 */
void usb_raw_stream_get_timing(uint32_t *last_us, uint32_t *max_us,
			       uint32_t *count);

/**
 * @brief Set a callback function to be called after each frame transmission completes (for pumping next frame).
 */
void usb_raw_stream_set_pump_cb(usb_raw_stream_pump_cb cb);

/**
 * @brief Reset the drop count and timing information (e.g. at the start of a new streaming session).
 */
void usb_raw_stream_reset_sync(void);

#endif /* USB_RAW_STREAM_H_ */
