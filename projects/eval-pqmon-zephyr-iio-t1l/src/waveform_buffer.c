/*******************************************************************************
 *   @file   waveform_buffer.c
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

#include "waveform_buffer.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(waveform_buf, LOG_LEVEL_INF);

struct wf_slot {
	int16_t  data[WF_BLOCK_SAMPLES];
	uint32_t seq;
	struct wf_timestamp ts;
};

static struct wf_slot ring[WF_RING_DEPTH];
static volatile uint32_t wr_idx;
static volatile uint32_t rd_idx;
static volatile uint32_t count;  /* number of unread slots */

static struct k_sem wf_sem;

void waveform_buffer_init(void)
{
	k_sem_init(&wf_sem, 0, WF_RING_DEPTH);
	wr_idx = 0;
	rd_idx = 0;
	count = 0;
	LOG_INF("Waveform ring buffer initialized (%u slots, %u bytes each)",
		WF_RING_DEPTH, (unsigned)WF_BLOCK_SIZE);
}

void waveform_buffer_put(const int16_t *data, uint32_t seq,
			 const struct wf_timestamp *ts)
{
	struct wf_slot *slot = &ring[wr_idx];

	memcpy(slot->data, data, WF_BLOCK_SIZE);
	slot->seq = seq;
	slot->ts = *ts;

	wr_idx = (wr_idx + 1) % WF_RING_DEPTH;

	if (count < WF_RING_DEPTH) {
		count++;
		k_sem_give(&wf_sem);
	} else {
		/* Overflow: advance read index past the overwritten slot.
		 * Consumer will see a sequence gap and know data was lost. */
		rd_idx = (rd_idx + 1) % WF_RING_DEPTH;
	}
}

int waveform_buffer_get(int16_t *out, uint32_t *seq,
			struct wf_timestamp *ts)
{
	int ret;

	ret = k_sem_take(&wf_sem, K_FOREVER);
	if (ret != 0) {
		return ret;
	}

	struct wf_slot *slot = &ring[rd_idx];

	memcpy(out, slot->data, WF_BLOCK_SIZE);
	*seq = slot->seq;
	*ts = slot->ts;

	rd_idx = (rd_idx + 1) % WF_RING_DEPTH;
	count--;

	return 0;
}
