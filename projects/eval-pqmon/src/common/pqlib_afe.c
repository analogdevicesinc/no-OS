/*******************************************************************************
 *   @file   pqlib_afe.c
 *   @brief  Analog frontend data polling process
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

#include "pqlib_afe.h"
#include "afe_config.h"
#include "no_os_irq.h"
#include "no_os_gpio.h"
#include "no_os_circular_buffer.h"
#include "maxim_irq.h"
#include "common_data.h"
#include "parameters.h"
#include "waveform_dma.h"
#include "iio.h"
#ifdef PQM_CONN_USB
#include "usb_raw_stream.h"
#endif
#include <stdio.h>
#include <string.h>

uint32_t wf_buf[WF_DMA_SINC4_TOTAL_SAMPLES];

volatile bool waveform_streaming_active = false;
volatile bool waveform_start_pending = false;
volatile bool new_block_ready = false;
volatile uint32_t waveform_block_count = 0;

/* Ring buffer to decouple DMA ISR from main-loop USB drain */
#define WF_RING_DEPTH	32
static uint32_t wf_ring[WF_RING_DEPTH][WF_DMA_SINC4_TOTAL_SAMPLES]
__attribute__((aligned(4)));
static uint32_t wf_ring_blk[WF_RING_DEPTH];
static volatile uint32_t wf_ring_head = 0;
static volatile uint32_t wf_ring_tail = 0;
volatile uint32_t wf_ring_overflow = 0;

volatile uint32_t dbg_isr_count = 0;
volatile uint32_t dbg_dma_kick_count = 0;
volatile uint32_t dbg_dma_done_count = 0;
volatile uint32_t dbg_svc_call_count = 0;

struct no_os_irq_ctrl_desc *afe_irq_desc = NULL;
static struct no_os_gpio_desc *afe_irq_gpio = NULL;

static volatile uint16_t next_wf_addr = REG_WAVEFORM_FIRST_HALF_ADDRESS;

/**
 * @brief Callback from waveform DMA when a block is ready (half or full buffer).
 * @param data Pointer to the block of data (size = num_samples * 4 bytes).
 * @param num_samples Number of 32-bit samples in the block (half or full buffer).
 */
static void wf_block_ready(uint32_t *data, uint16_t num_samples)
{
	uint32_t next_head;

	dbg_dma_done_count++;
	waveform_block_count++;

	/* Store RAW data in ring (no byte-swap here — done at drain time) */
	next_head = (wf_ring_head + 1) % WF_RING_DEPTH;
	if (next_head == wf_ring_tail) {
		wf_ring_overflow++;
	} else {
		memcpy(wf_ring[wf_ring_head], data, num_samples * sizeof(uint32_t));
		wf_ring_blk[wf_ring_head] = waveform_block_count;
		wf_ring_head = next_head;
		new_block_ready = true;
	}
}

/**
 * @brief Drain ready waveform blocks to USB (if active) or just mark them consumed
 */
void waveform_drain_to_cb(void)
{
#ifdef PQM_CONN_USB
	static volatile bool draining = false;
	extern int configured;
	uint32_t newest;
	uint32_t last_us, max_us, count;
	int ret;

	if (usb_raw_stream_is_active()) {
		if (draining)
			return;
		draining = true;

		/* Detect USB disconnect: stop streaming so IIO resumes
		 * when host reconnects. */
		static bool was_configured = false;
		static bool primed = false;
		if (!configured) {
			wf_ring_tail = wf_ring_head;
			new_block_ready = false;
			was_configured = false;
			primed = false;
			pqm_desc_global->waveform_streaming_active = false;
			pqm_desc_global->oneshot_running = false;
			waveform_streaming_active = false;
			usb_raw_stream_stop();
			draining = false;
			return;
		}
		if (!was_configured) {
			was_configured = true;
			primed = false;
			usb_raw_stream_reset_sync();
		}

		/* Until the host has consumed at least one USB transfer,
		 * keep the ring flushed and attempt one send to prime the
		 * endpoint. Once tx_complete fires (host reading), switch
		 * to normal streaming from block 1. */
		usb_raw_stream_get_timing(&last_us, &max_us, &count);

		/* If ring overflowed, host fell behind — reset sync so
		 * streaming restarts cleanly when host catches up. */
		if (wf_ring_overflow > 0) {
			usb_raw_stream_reset_sync();
			count = 0;
		}
		if (count == 0) {
			if (wf_ring_tail != wf_ring_head) {
				/* Grab newest block, discard the rest */
				newest = (wf_ring_head + WF_RING_DEPTH - 1)
					 % WF_RING_DEPTH;
				no_os_memswap64((void *)wf_ring[newest],
						WF_DMA_SINC4_DATA_BYTES, 4);
				usb_raw_stream_send_block(wf_ring[newest], 0,
							  pqlibExample.inputCycle.sequenceNumber,
							  pqlibExample.input1012Cycles.sequenceNumber);
				wf_ring_tail = wf_ring_head;
			}
			waveform_block_count = 0;
			wf_ring_overflow = 0;
			new_block_ready = false;
			primed = false;
			draining = false;
			return;
		}

		if (!primed) {
			/* First tx_complete arrived — discard any stale ring entries
			 * that accumulated between priming and host reading. */
			wf_ring_tail = wf_ring_head;
			waveform_block_count = 0;
			new_block_ready = false;
			primed = true;
			draining = false;
			return;
		}

		while (wf_ring_tail != wf_ring_head) {
			/* Byte-swap in place (ring stores raw DMA data) */
			no_os_memswap64((void *)wf_ring[wf_ring_tail],
					WF_DMA_SINC4_DATA_BYTES, 4);
			ret = usb_raw_stream_send_block(
				      wf_ring[wf_ring_tail],
				      wf_ring_blk[wf_ring_tail],
				      pqlibExample.inputCycle.sequenceNumber,
				      pqlibExample.input1012Cycles.sequenceNumber);
			if (ret == 0) {
				wf_ring_tail = (wf_ring_tail + 1) % WF_RING_DEPTH;
				if (pqm_desc_global->oneshot_running) {
					if (pqm_desc_global->oneshot_blocks_remaining > 0)
						pqm_desc_global->oneshot_blocks_remaining--;
					if (pqm_desc_global->oneshot_blocks_remaining == 0) {
						pqm_desc_global->waveform_streaming_active = false;
						pqm_desc_global->oneshot_running = false;
						waveform_streaming_active = false;
						usb_raw_stream_stop();
						break;
					}
				}
			} else {
				break;
			}
		}
		if (wf_ring_tail == wf_ring_head)
			new_block_ready = false;

		draining = false;
		return;
	}
#endif

	if (waveform_dma_is_busy())
		return;

	if (!new_block_ready)
		return;

	if (!pqm_desc_global || !pqm_desc_global->waveform_streaming_active ||
	    !pqlibExample.no_os_cb_desc)
		return;

	if (pqm_desc_global->oneshot_running) {
		if (pqm_desc_global->oneshot_blocks_remaining > 0)
			pqm_desc_global->oneshot_blocks_remaining--;
		if (pqm_desc_global->oneshot_blocks_remaining == 0) {
			pqm_desc_global->waveform_streaming_active = false;
			pqm_desc_global->oneshot_running = false;
			waveform_streaming_active = false;
		}
	}
}

/**
 * @brief GPIO IRQ handler for AFE waveform ready signal (PAGE_FULL).
 */
static void waveform_irq_handler(void *ctx)
{
	uint16_t addr;

	dbg_isr_count++;

	/* Capture current address and flip immediately — ensures deferred
	 * always stores the correct (next) half-buffer address even if
	 * the DMA ISR hasn't run yet. */
	addr = next_wf_addr;
	next_wf_addr = (addr == REG_WAVEFORM_FIRST_HALF_ADDRESS)
		       ? REG_WAVEFORM_SECOND_HALF_ADDRESS
		       : REG_WAVEFORM_FIRST_HALF_ADDRESS;

	if (waveform_dma_is_busy()) {
		waveform_dma_set_deferred(addr);
		return;
	}

	waveform_dma_start(addr);
}

/**
 * @brief Service routine to drain waveform blocks to USB or mark them consumed.
 */
void service_waveform(void)
{
	dbg_svc_call_count++;
	waveform_drain_to_cb();
}

/**
 * @brief Initialize GPIO IRQ for AFE waveform ready signal and configure AFE for waveform capture.
 */
int waveform_irq_init(void)
{
	int status;
	struct no_os_irq_ctrl_desc *nvic_desc;
	uint32_t mask0;
	uint32_t clr;

	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	status = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (status)
		return status;

	status = no_os_irq_set_priority(nvic_desc, GPIO0_IRQn, 2);
	if (status)
		return status;

	status = no_os_irq_enable(nvic_desc, GPIO0_IRQn);
	if (status)
		return status;

	status = no_os_gpio_get(&afe_irq_gpio, &intr_gpio_ip);
	if (status)
		return status;

	status = no_os_gpio_direction_input(afe_irq_gpio);
	if (status)
		return status;

	status = no_os_irq_ctrl_init(&afe_irq_desc, &afe_callback_ctrl_ip);
	if (status)
		return status;

	afe0_callback_desc.callback = waveform_irq_handler;
	afe0_callback_desc.ctx = afe_irq_desc;

	status = no_os_irq_register_callback(afe_irq_desc, INTR_GPIO_PIN_NUM,
					     &afe0_callback_desc);
	if (status)
		return status;

	status = no_os_irq_trigger_level_set(afe_irq_desc, INTR_GPIO_PIN_NUM,
					     NO_OS_IRQ_EDGE_FALLING);
	if (status)
		return status;

	status = no_os_irq_set_priority(afe_irq_desc, INTR_GPIO_PIN_NUM, 2);
	if (status)
		return status;

	mask0 = BITM_MASK0_PAGE_FULL;
	status = afe_write_32bit_reg(REG_MASK0, &mask0);
	if (status)
		return status;

	status = waveform_dma_init(hSPI);
	if (status)
		return status;

	waveform_dma_set_callback(wf_block_ready);

#ifdef PQM_CONN_USB
	usb_raw_stream_set_pump_cb(waveform_drain_to_cb);
#endif

	status = config_wfb_sinc4();
	if (status)
		return status;

	/* Clear PAGE_FULL so /IRQ0 de-asserts — first edge triggers ISR */
	clr = BITM_STATUS0_PAGE_FULL;
	status = afe_write_32bit_reg(REG_STATUS0, &clr);

	next_wf_addr = REG_WAVEFORM_FIRST_HALF_ADDRESS;
	status = no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
	if (status)
		return status;

	return 0;
}

/**
 * @brief Poll AFE for new data when /IRQ0 asserted, read available measurements, and clear IRQ.
 *        Each SPI transaction is protected by a brief IRQ-disable window to ensure PAGE_FULL
 *        edges aren't missed between reads.
 * @return 0 if successful, or error code from AFE read/write functions.
 */
int get_afe_input()
{
	int status = 0;
	uint32_t clr;
	uint8_t pin_val;
	ADI_PQLIB_CYCLE_INPUT *pOneCycle = &(pqlibExample.inputCycle);
	ADI_PQLIB_1012_CYCLE_INPUT *p1012Cycle = &(pqlibExample.input1012Cycles);

	/* Each SPI transaction gets its own short IRQ-disable window so
	 * PAGE_FULL edges are serviced between reads (~100-200µs gaps). */

	no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
	if (waveform_dma_is_busy()) {
		no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		return 0;
	}
	status = afe_read_status0((uint32_t *)&pOneCycle->STATUS0);
	no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);

	if (status != 0)
		return status;

	if (pOneCycle->STATUS0 & BITM_STATUS0_RMSONERDY) {
		no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		if (waveform_dma_is_busy()) {
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			return 0;
		}
		status = afe_read_rms_one((uint32_t *)&pOneCycle->AVRMSONE, 3);
		no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);

		if (status == 0) {
			no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			if (waveform_dma_is_busy()) {
				no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
				return 0;
			}
			status = afe_read_angle((uint16_t *)&pOneCycle->ANGL_VA_VB,
						3, ANGLE_VOLTAGE);
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		}
		if (status == 0) {
			no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			if (waveform_dma_is_busy()) {
				no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
				return 0;
			}
			status = afe_read_angle((uint16_t *)&pOneCycle->ANGL_IA_IB,
						3, ANGLE_CURRENT);
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		}
		if (status == 0) {
			no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			if (waveform_dma_is_busy()) {
				no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
				return 0;
			}
			status = afe_read_period((uint32_t *)&pOneCycle->PERIOD, 1);
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		}
		if (status == 0) {
			no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			if (waveform_dma_is_busy()) {
				no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
				return 0;
			}
			status = afe_read_status_1((uint32_t *)&pOneCycle->STATUS1);
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		}

		if (status == 0) {
			no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			if (waveform_dma_is_busy()) {
				no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
				return 0;
			}
			clr = BITM_STATUS0_RMSONERDY;
			afe_write_32bit_reg(REG_STATUS0, &clr);
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			pOneCycle->isDataProcessed = 0;
			pOneCycle->sequenceNumber++;
		}
	}

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_EGYRDY)) {
		no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		if (waveform_dma_is_busy()) {
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			return 0;
		}
		status = afe_read_power_energy(&pqlibExample.powerEnergy);
		no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		if (status == 0) {
			no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			if (waveform_dma_is_busy()) {
				no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
				return 0;
			}
			clr = BITM_STATUS0_EGYRDY;
			afe_write_32bit_reg(REG_STATUS0, &clr);
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		}
	}

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_RMS1012RDY)) {
		no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		if (waveform_dma_is_busy()) {
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			return 0;
		}
		status = afe_read_rms_1012((uint32_t *)&p1012Cycle->AIRMS1012, 7);
		no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
		if (status == 0) {
			no_os_irq_disable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			if (waveform_dma_is_busy()) {
				no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
				return 0;
			}
			clr = BITM_STATUS0_RMS1012RDY;
			afe_write_32bit_reg(REG_STATUS0, &clr);
			no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
			p1012Cycle->isDataProcessed = 0;
			p1012Cycle->sequenceNumber++;
		}
	}

	/* Pin-level recovery: if /IRQ0 is still asserted (active low)
	 * a PAGE_FULL occurred during one of the brief disable windows. */
	no_os_gpio_get_value(afe_irq_gpio, &pin_val);
	if (pin_val == 0)
		waveform_irq_handler(NULL);

	return status;
}
