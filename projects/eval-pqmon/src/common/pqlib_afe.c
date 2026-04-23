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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#include "iio_pqm.h"
#include "no_os_irq.h"
#include "no_os_gpio.h"
#include "maxim_irq.h"
#include "common_data.h"
#include "parameters.h"

static uint16_t isr_wf_buf[ADI_PQLIB_WAVEFORM_BLOCK_SIZE
			    * ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS];

static volatile bool spi_busy = false;
static volatile bool waveform_pending = false;
static volatile bool isr_wf_ready = false;

static struct no_os_irq_ctrl_desc *afe_irq_desc = NULL;

static void waveform_cb_write_and_oneshot(uint8_t *data, uint16_t len)
{
	no_os_cb_write(pqlibExample.no_os_cb_desc, data, len);
	pqm_desc_global->waveform_block_count++;

	if (pqm_desc_global->waveform_capture_mode ==
	    WAVEFORM_CAPTURE_ONESHOT) {
		if (pqm_desc_global->oneshot_blocks_remaining > 0)
			pqm_desc_global->oneshot_blocks_remaining--;
		if (pqm_desc_global->oneshot_blocks_remaining == 0) {
			pqm_desc_global->waveform_streaming_active = false;
			pqm_desc_global->waveform_capture_mode =
				WAVEFORM_CAPTURE_DISABLED;
			pqm_desc_global->oneshot_running = false;
		}
	}
}

static void waveform_irq_handler(void *ctx)
{
	uint16_t numSamples = ADI_PQLIB_WAVEFORM_BLOCK_SIZE
			      * ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS;
	uint32_t clr;

	if (!pqlibExample.no_os_cb_desc || !pqm_desc_global ||
	    !pqm_desc_global->waveform_streaming_active)
		return;

	if (spi_busy) {
		waveform_pending = true;
		return;
	}

	if (afe_read_waveform(isr_wf_buf, numSamples) != 0)
		return;

	/* Stage for main loop to write to CB */
	isr_wf_ready = true;

	/* Clear COH_PAGE_RDY so IRQ0 de-asserts */
	clr = BITM_STATUS0_COH_PAGE_RDY;
	afe_write_32bit_reg(REG_STATUS0, &clr);
}

void service_waveform_isr(void)
{
	uint16_t numSamples = ADI_PQLIB_WAVEFORM_BLOCK_SIZE
			      * ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS;
	uint32_t clr;

	/* Flush staged ISR data to CB */
	if (isr_wf_ready) {
		if (pqlibExample.no_os_cb_desc && pqm_desc_global &&
		    pqm_desc_global->waveform_streaming_active) {
			waveform_cb_write_and_oneshot((uint8_t *)isr_wf_buf,
						     numSamples * sizeof(uint16_t));
		}
		isr_wf_ready = false;
	}

	/* Handle deferred read (ISR fired while SPI was busy).
	 * get_afe_input skips waveform SPI during streaming, so the
	 * block is still unread. Read it now (main loop, SPI free). */
	if (waveform_pending) {
		waveform_pending = false;

		if (afe_read_waveform(isr_wf_buf, numSamples) != 0)
			return;

		if (pqlibExample.no_os_cb_desc && pqm_desc_global &&
		    pqm_desc_global->waveform_streaming_active) {
			clr = BITM_STATUS0_COH_PAGE_RDY;
			waveform_cb_write_and_oneshot((uint8_t *)isr_wf_buf,
						     numSamples * sizeof(uint16_t));
			afe_write_32bit_reg(REG_STATUS0, &clr);
		}
	}
}

int waveform_irq_init(void)
{
	int status;
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_gpio_desc *irq_gpio;
	uint32_t mask0;

	/* Level 1: Enable GPIO port 0 interrupt in NVIC */
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	status = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (status)
		return status;

	status = no_os_irq_set_priority(nvic_desc, GPIO0_IRQn, 1);
	if (status)
		return status;

	status = no_os_irq_enable(nvic_desc, GPIO0_IRQn);
	if (status)
		return status;

	/* Level 2: Configure GPIO pin as input */
	status = no_os_gpio_get(&irq_gpio, &intr_gpio_ip);
	if (status)
		return status;

	status = no_os_gpio_direction_input(irq_gpio);
	if (status)
		return status;

	/* Level 2: GPIO IRQ controller - register callback on pin */
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

	status = no_os_irq_set_priority(afe_irq_desc, INTR_GPIO_PIN_NUM, 1);
	if (status)
		return status;

	status = no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
	if (status)
		return status;

	/* Set MASK0 to only COH_PAGE_RDY so IRQ0 reflects only that bit.
	 * get_afe_input polls STATUS0 directly - doesn't need MASK0. */
	mask0 = BITM_MASK0_COH_PAGE_RDY;
	status = afe_write_32bit_reg(REG_MASK0, &mask0);
	if (status)
		return status;

	return 0;
}

/**
 * @details Get AFE input.
 */
int get_afe_input()
{
	int status = 0;
	ADI_PQLIB_WAVEFORM *pWaveform = &(pqlibExample.inputWaveform);
	ADI_PQLIB_CYCLE_INPUT *pOneCycle = &(pqlibExample.inputCycle);
	ADI_PQLIB_1012_CYCLE_INPUT *p1012Cycle = &(pqlibExample.input1012Cycles);

	/* Flush any ISR-staged waveform data to CB before SPI section */
	service_waveform_isr();

	spi_busy = true;

	status = afe_read_status0((uint32_t *)&pOneCycle->STATUS0);
	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_RMSONERDY)) {
		status = afe_read_rms_one((uint32_t *)&pOneCycle->AVRMSONE, 3);
		if (status == 0) {
			status =
				afe_read_angle((uint16_t *)&pOneCycle->ANGL_VA_VB, 3, ANGLE_VOLTAGE);
		}
		if (status == 0) {
			status =
				afe_read_angle((uint16_t *)&pOneCycle->ANGL_IA_IB, 3, ANGLE_CURRENT);
		}
		if (status == 0) {
			status = afe_read_period((uint32_t *)&pOneCycle->PERIOD, 1);
		}
		if (status == 0) {
			status = afe_read_status_1((uint32_t *)&pOneCycle->STATUS1);
		}

		if (status == 0) {
			pOneCycle->isDataProcessed = 0;
			pOneCycle->sequenceNumber++;
		}
	}

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_EGYRDY)) {
		status = afe_read_power_energy(&pqlibExample.powerEnergy);
	}

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_COH_PAGE_RDY) &&
	    (!pqm_desc_global || !pqm_desc_global->waveform_streaming_active)) {
		/* Skip waveform read when ISR is the sole reader (streaming).
		 * Read normally for PQLib when not streaming. */
		status = afe_read_waveform(
			(uint16_t *)&(pqlibExample.inputWaveform.waveform),
			ADI_PQLIB_WAVEFORM_BLOCK_SIZE
			* ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS);
		pWaveform->isDataProcessed = 0;
		pWaveform->sequenceNumber++;

		if (pqlibExample.no_os_cb_desc && !processData) {
			no_os_cb_write(pqlibExample.no_os_cb_desc,
				(uint8_t *)&(pqlibExample.inputWaveform.waveform),
				ADI_PQLIB_WAVEFORM_BLOCK_SIZE
				* ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS
				* sizeof(uint16_t));
			if (pqm_desc_global)
				pqm_desc_global->waveform_block_count++;
		}
	}

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_RMS1012RDY)) {
		status = afe_read_rms_1012((uint32_t *)&p1012Cycle->AIRMS1012, 7);
		if (status == 0) {
			p1012Cycle->isDataProcessed = 0;
			p1012Cycle->sequenceNumber++;
		}
	}

	spi_busy = false;

	/* Service any ISR that fired during our SPI section */
	service_waveform_isr();

	return status;
}
