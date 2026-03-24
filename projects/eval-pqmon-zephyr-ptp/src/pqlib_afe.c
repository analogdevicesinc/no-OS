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
#include "ptp_clock_adin1110_hw.h"
#include <zephyr/net/ptp_time.h>

volatile struct net_ptp_time last_measurement_ptp_ts;
volatile struct net_ptp_time last_1012cycle_ptp_ts;

int get_afe_input(void)
{
	int status = 0;
	ADI_PQLIB_WAVEFORM *pWaveform = &(pqlibExample.inputWaveform);
	ADI_PQLIB_CYCLE_INPUT *pOneCycle = &(pqlibExample.inputCycle);
	ADI_PQLIB_1012_CYCLE_INPUT *p1012Cycle = &(pqlibExample.input1012Cycles);

	status = afe_read_status0((uint32_t *)&pOneCycle->STATUS0);
	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_RMSONERDY)) {
		/* Capture PTP timestamp at zero-crossing — ADE9430 just signaled
		 * 1-cycle RMS ready. TS_CAPT uses SPI2. */
		adin1110_ptp_capture_timestamp(
			(struct net_ptp_time *)&last_measurement_ptp_ts);

		/* Provide PTP time to PQLib via external timestamp.
		 * This ensures event timestamps come directly from the
		 * PTP-syntonized counter, enabling cross-board correlation. */
		pOneCycle->timestamp = (int64_t)last_measurement_ptp_ts.second * 1000LL +
				       last_measurement_ptp_ts.nanosecond / 1000000;

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

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_COH_PAGE_RDY)) {
		status = afe_read_waveform(
				 (uint16_t *) & (pqlibExample.inputWaveform.waveform),
				 ADI_PQLIB_WAVEFORM_BLOCK_SIZE * ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS);
		pWaveform->isDataProcessed = 0;
		pWaveform->sequenceNumber++;
		/* no-OS circular buffer write removed (IIO only) */
	}

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_RMS1012RDY)) {
		adin1110_ptp_capture_timestamp(
			(struct net_ptp_time *)&last_1012cycle_ptp_ts);

		status = afe_read_rms_1012((uint32_t *)&p1012Cycle->AIRMS1012, 7);
		if (status == 0) {
			p1012Cycle->isDataProcessed = 0;
			p1012Cycle->sequenceNumber++;
		}
	}

	return status;
}
