/*******************************************************************************
 *   @file   pqlib_afe.h
 *   @brief  Analog frontend data polling header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#ifndef __PQLIB_AFE_H__
#define __PQLIB_AFE_H__

#include "ade9430.h"
#include "adi_pqlib.h"
#include "afe_config.h"
#include "pqlib_example.h"
#include "status.h"
#include "waveform_dma.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#define ANGLE_VOLTAGE 0
#define ANGLE_CURRENT 1

extern uint32_t wf_buf[WF_DMA_SINC4_TOTAL_SAMPLES];
extern volatile bool waveform_streaming_active;
extern volatile bool waveform_start_pending;
extern volatile bool new_block_ready;
extern volatile uint32_t waveform_block_count;
extern volatile uint32_t dbg_isr_count;
extern volatile uint32_t dbg_dma_kick_count;
extern volatile uint32_t dbg_dma_done_count;
extern volatile uint32_t dbg_svc_call_count;

/**
 * @brief Callback from waveform DMA when a block is ready (half or full buffer).
 * @param data Pointer to the block of data (size = num_samples * 4 bytes).
 * @param num_samples Number of 32-bit samples in the block (half or full buffer).
 */
static void wf_block_ready(uint32_t *data, uint16_t num_samples);
/**
 * @brief Initialize the AFE and related resources
 */
int get_afe_input(void);
/**
 * @brief Start the AFE data acquisition and waveform streaming
 */
int waveform_irq_init(void);
/**
 * @brief Stop the AFE data acquisition and waveform streaming
 */
void service_waveform(void);
/**
 * @brief Drain any remaining data in the AFE FIFO and prepare for shutdown
 */
void waveform_drain_to_cb(void);

#endif /* __PQLIB_AFE_H__ */
