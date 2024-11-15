/*******************************************************************************
 *   @file   pqlib_afe.c
 *   @brief  Analog frontend data polling process
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "pqlib_afe.h"
#include "iio_pqm.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @details Get AFE input.
 */
int get_afe_input()
{
	int status = 0;
	ADI_PQLIB_WAVEFORM *pWaveform = &(pqlibExample.inputWaveform);
	ADI_PQLIB_CYCLE_INPUT *pOneCycle = &(pqlibExample.inputCycle);
	ADI_PQLIB_1012_CYCLE_INPUT *p1012Cycle = &(pqlibExample.input1012Cycles);

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

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_COH_PAGE_RDY)) {
		status = afe_read_waveform(
				 (uint16_t *)&(pqlibExample.inputWaveform.waveform),
				 ADI_PQLIB_WAVEFORM_BLOCK_SIZE * ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS);
		pWaveform->isDataProcessed = 0;
		pWaveform->sequenceNumber++;
		if (pqlibExample.no_os_cb_desc && !processData) {
			no_os_cb_write (pqlibExample.no_os_cb_desc,
					(uint8_t *)&(pqlibExample.inputWaveform.waveform),
					ADI_PQLIB_WAVEFORM_BLOCK_SIZE
					* ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS
					* sizeof (uint16_t));
		}
	}

	if ((status == 0) && (pOneCycle->STATUS0 & BITM_STATUS0_RMS1012RDY)) {
		status = afe_read_rms_1012((uint32_t *)&p1012Cycle->AIRMS1012, 7);
		if (status == 0) {
			p1012Cycle->isDataProcessed = 0;
			p1012Cycle->sequenceNumber++;
		}
	}

	return status;
}
