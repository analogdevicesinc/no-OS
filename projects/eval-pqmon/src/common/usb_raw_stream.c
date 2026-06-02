/******************************************************************************
 *   @file   usb_raw_stream.c
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

#include "usb_raw_stream.h"
#include "usb.h"
#include "cdc_acm.h"
#include "mxc_sys.h"
#include "usbhs_regs.h"
#include <string.h>
#include <stdio.h>

#include "mxc_device.h"

extern int configured;

static volatile bool streaming_active = false;
static volatile bool tx_busy = false;
static volatile bool first_send = false;
static volatile uint32_t drop_count = 0;
static volatile uint32_t tx_count = 0;

static uint8_t frame_buf_a[USB_RAW_STREAM_FRAME_BYTES] __attribute__((aligned(
			4)));
static uint8_t frame_buf_b[USB_RAW_STREAM_FRAME_BYTES] __attribute__((aligned(
			4)));
static uint8_t *active_buf = frame_buf_a;
static uint8_t *pending_buf = frame_buf_b;

static MXC_USB_Req_t stream_req;

static usb_raw_stream_pump_cb pump_cb = NULL;

/**
 * @brief Callback from USB driver when a frame transmission completes (successfully or with error)
 */
static void tx_complete_callback(void *cbdata)
{
	tx_count++;
	tx_busy = false;

	if (pump_cb && streaming_active)
		pump_cb();
}
/**
 * @brief Set the callback function to be called after each frame transmission completes (successfully or with error)
 */
void usb_raw_stream_set_pump_cb(usb_raw_stream_pump_cb cb)
{
	pump_cb = cb;
}

/**
 * @brief Initialize the USB raw stream module (must be called before any other functions in this module)
 * @return 0 on success, negative error code on failure
 */
int usb_raw_stream_init(void)
{
	streaming_active = false;
	tx_busy = false;
	drop_count = 0;

	memset(&stream_req, 0, sizeof(stream_req));
	stream_req.callback = tx_complete_callback;
	stream_req.cbdata = &stream_req;
	stream_req.type = MAXUSB_TYPE_TRANS;

	return 0;
}

/**
 * @brief Start the USB raw stream (must be called after usb_raw_stream_init and before sending any data)
 * @return 0 on success, negative error code on failure
 */
int usb_raw_stream_start(void)
{
	if (streaming_active)
		return 0;

	drop_count = 0;
	tx_count = 0;
	tx_busy = false;
	first_send = true;

	streaming_active = true;

	return 0;
}

/**
 * @brief Reset the USB raw stream timing and state (e.g. after a synchronization event)
 */
void usb_raw_stream_reset_sync(void)
{
	tx_count = 0;
	tx_busy = false;
	first_send = true;
}

/**
 * @brief Stop the USB raw stream (can be called to halt streaming, e.g. on error or shutdown)
 * @return 0 on success, negative error code on failure
 */
int usb_raw_stream_stop(void)
{
	streaming_active = false;

	if (tx_busy) {
		MXC_USB_RemoveRequest(&stream_req);
		tx_busy = false;
	}

	return 0;
}

/**
 * @brief Check if the USB raw stream is currently active (started and not stopped)
 * @return true if streaming is active, false otherwise
 */
bool usb_raw_stream_is_active(void)
{
	return streaming_active;
}

/**
 * @brief Get the number of frames that were dropped due to the previous frame still being in transmission
 * @return Number of dropped frames
 */
uint32_t usb_raw_stream_get_drop_count(void)
{
	return drop_count;
}

/**
 * @brief Get timing information about the USB raw stream transmissions (for monitoring and debugging)
 * @param last_us Pointer to variable to receive the time taken for the last frame transmission
 * in microseconds(NOT USED CURRENTLY, ALWAYS 0)
 * @param max_us Pointer to variable to receive the maximum time taken for any frame transmission
 * in microseconds(NOT USED CURRENTLY, ALWAYS 0)
 * @param count Pointer to variable to receive the total number of frames transmitted since the last reset
 */
void usb_raw_stream_get_timing(uint32_t *last_us, uint32_t *max_us,
			       uint32_t *count)
{
	*last_us = 0;
	*max_us = 0;
	*count = tx_count;
}

/**
 * @brief Send a block of waveform data over the USB raw stream (non-blocking, returns immediately)
 * @param data Pointer to the block of data to send (size should be block_count * 4 bytes)
 * @param block_count Number of 32-bit blocks in the data
 * @param seq_cycle Sequence cycle number for synchronization (e.g. AFE sequence cycle)
 * @param seq_1012 Sequence ID for 1012-cycle parameters (for correlation with parameter exports)
 * @return 0 on successful initiation of transmission, negative error code on failure
 */
int usb_raw_stream_send_block(uint32_t *data, uint32_t block_count,
			      uint32_t seq_cycle, uint32_t seq_1012)
{
	uint32_t *hdr;
	MXC_USB_Req_t *stale;
	uint8_t *tmp;
	int ret;

	if (!streaming_active)
		return -1;

	if (!configured)
		return -1;

	if (first_send) {
		first_send = false;
		stale = MXC_USB_GetRequest(USB_RAW_STREAM_IN_EP);
		if (stale && stale != &stream_req)
			MXC_USB_RemoveRequest(stale);
		uint8_t saved_idx = MXC_USBHS->index;
		MXC_USBHS->index = USB_RAW_STREAM_IN_EP;
		MXC_USBHS->incsru &= ~MXC_F_USBHS_INCSRU_DPKTBUFDIS;
		MXC_USBHS->index = saved_idx;
	}

	if (tx_busy) {
		drop_count++;
		return -2;
	}

	tmp = active_buf;
	active_buf = pending_buf;
	pending_buf = tmp;

	hdr = (uint32_t *)active_buf;
	hdr[0] = USB_RAW_STREAM_SYNC_WORD;
	hdr[1] = block_count;
	hdr[2] = seq_cycle;
	hdr[3] = seq_1012;
	memcpy(active_buf + USB_RAW_STREAM_HEADER_BYTES, data,
	       USB_RAW_STREAM_DATA_BYTES);

	stream_req.ep = USB_RAW_STREAM_IN_EP;
	stream_req.data = active_buf;
	stream_req.reqlen = USB_RAW_STREAM_FRAME_BYTES;
	stream_req.actlen = 0;
	stream_req.error_code = 0;

	tx_busy = true;
	ret = MXC_USB_WriteEndpoint(&stream_req);
	if (ret) {
		tx_busy = false;
		drop_count++;
		return ret;
	}

	return 0;
}
