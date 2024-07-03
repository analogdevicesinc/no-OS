/*******************************************************************************
 *   @file   pqlib_example.h
 *   @brief  Pqlib example functions for display
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "pqlib_example.h"
#include "iio_pqm.h"

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/

PQLIB_EXAMPLE pqlibExample;
extern volatile uint8_t newSyncTimeAvailable;
extern volatile bool configChanged;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

int pqm_measurement_init(void)
{
	int status = 0;

	set_default_config(&pqlibExample.exampleConfig);

	pqlibExample.state = PQLIB_STATE_WAITING_FOR_START_CMD;
	pqlibExample.processedCycles = 0;
	pqlibExample.inputCycle.sequenceNumber = 0;
	pqlibExample.input1012Cycles.sequenceNumber = 0;
	status = open_pqlib(&pqlibExample);

	return status;
}

int pqm_start_measurement(bool waitingForSync)
{
	int status = 0;
	PQLIB_EXAMPLE *pExample = &pqlibExample;
	uint32_t afeVersion = 0;
	const uint32_t supportedAfeVersionRC3 = 0x809B461;
	const uint32_t supportedAfeVersionRC4 = 0x954871C;

	pqlibExample.processedCycles = 0;
	pqlibExample.zeroCrossingCount = 0;
	pqlibExample.timeOutCount = 0;
	pqlibExample.input1012Cycles.isDataProcessed = 1u;
	pqlibExample.inputCycle.isDataProcessed = 1u;
	pqlibExample.inputWaveform.isDataProcessed = true;
	pqlibExample.inputCycle.sequenceNumber = 0;
	pqlibExample.inputWaveform.sequenceNumber = 0;
	pqlibExample.input1012Cycles.sequenceNumber = 0;
	pqlibExample.state = PQLIB_STATE_WAITING_FOR_START_CMD;
	pqlibExample.readyToDisplay = false;
	pqlibExample.waitingForSync = waitingForSync;
	pqlibExample.syncCycles = 0;

	status = afe_read_version(&afeVersion);

	if (status == 0) {
		if ((afeVersion == supportedAfeVersionRC3) ||
		    (afeVersion == supportedAfeVersionRC4)) {
			status = config_measurement(pExample);
			pqlibExample.processedCycles = 0;

			if (status == ADI_PQLIB_RESULT_SUCCESS) {
				status = afe_start();
				if (status == 0) {
					afe_wait_settling(pqlibExample.exampleConfig.nominalFrequency);
					newSyncTimeAvailable = 0;
				}
			} else {
				status = SYS_STATUS_PQLIB_ERROR;
			}

			if (status == 0) {
				pqlibExample.state = PQLIB_STATE_WAITING_FOR_TRIGGER;
			}
		}
	}

	return status;
}

int pqm_one_cycle(void)
{
	if (configChanged) {
		printf("Recallibrating\n\r");
		pqm_start_measurement(false);
		configChanged = false;
		pqlibExample.state = PQLIB_STATE_WAITING_FOR_TRIGGER;
	}
	process_and_prepare_output();
	return 0;
}

int open_pqlib(PQLIB_EXAMPLE *pExample)
{
	int status;
	ADI_PQLIB_RESULT pqlibStatus;
	void *pStateMemory = &pqlibExample.pqlibStateMemory[0];
	uint32_t stateMemSize = sizeof(pqlibExample.pqlibStateMemory);
	void *pTempMemory = &pqlibExample.pqlibTempMemory[0];
	uint32_t tempMemSize = sizeof(pqlibExample.pqlibTempMemory);

	pqlibStatus = adi_pqlib_Open(&pqlibExample.hDevice, pStateMemory,
				     stateMemSize, pTempMemory, tempMemSize, NULL);

	status = process_pqlib_error(pExample, pqlibStatus);

	return status;
}

int config_measurement(PQLIB_EXAMPLE *pExample)
{
	int status = 0;

	ADI_PQLIB_RESULT pqlibStatus = ADI_PQLIB_RESULT_SUCCESS;
	ADI_PQLIB_CONFIG *pLibConfig = &pqlibExample.config;
	EXAMPLE_CONFIG *pExampleConfig = &pqlibExample.exampleConfig;
	ADI_PQLIB_PHASE_MAP *pChannelMap = &pExampleConfig->phaseMap;

	float nominalVoltageScale = pExampleConfig->nominalVoltage;
	pqlibStatus = adi_pqlib_Reset(pqlibExample.hDevice, true);
	if (pqlibStatus == 0) {
		status = afe_set_acc_mode(pqlibExample.exampleConfig.nominalFrequency,
					  pqlibExample.exampleConfig.vconsel,
					  pqlibExample.exampleConfig.enableIconsel);
	}

	if (status == 0) {
		status = afe_set_ref_channel(pExampleConfig->refChannel);
	}

	if (status == 0) {
		pLibConfig->refChannel = pExampleConfig->refChannel;
		pLibConfig->nominalVoltage =
			(ADI_AFE_RMS_TYPE)(ADI_PQLIB_MAX_RMS_VALUE * nominalVoltageScale);
		if (pExampleConfig->nominalFrequency == 50) {
			pLibConfig->nominalFrequency = ADI_PQLIB_NOMINAL_FREQUENCY_50HZ;
		} else {
			pLibConfig->nominalFrequency = ADI_PQLIB_NOMINAL_FREQUENCY_60HZ;
		}

		pLibConfig->flickerModel = pExampleConfig->flickerModel;
		pqlibExample.config.pPhaseMap = pChannelMap;
		populate_event_config(pExampleConfig);

		pLibConfig->msvConfig.threshold =
			convert_to_pct_type(pExampleConfig->msvThreshold);
		pLibConfig->msvConfig.carrierFrequency =
			convert_to_freq_type(pExampleConfig->msvCarrierFrequency);
		pLibConfig->msvConfig.recordingLength =
			convert_to_time_ms(pExampleConfig->msvRecordingLength);

		if (pExampleConfig->correction == 0) {
			pLibConfig->disableCorrection = true;
		}

		pLibConfig->useExternalTimestamp = pExampleConfig->useExternalTimestamp;

		pqlibStatus =
			adi_pqlib_SetConfig(pqlibExample.hDevice, &pqlibExample.config);

		if ((pqlibStatus == 0) && (pExampleConfig->correction != 0)) {
			status = cpy_correction_coeffs(pLibConfig->nominalFrequency,
						       &pExampleConfig->correctionData);
			if (status == 0) {
				pqlibStatus = adi_pqlib_SetCorrectionCoefficients(
						      pqlibExample.hDevice, &pExampleConfig->correctionData,
						      pLibConfig->nominalFrequency);
			}
		}

		status = process_pqlib_error(pExample, pqlibStatus);
	}

	return status;
}

int process_and_prepare_output()
{
	int status = 0;
	ADI_PQLIB_RESULT pqlibStatus = ADI_PQLIB_RESULT_SUCCESS;
	ADI_PQLIB_CYCLE_INPUT *pCycleInput = &pqlibExample.inputCycle;
	ADI_PQLIB_1012_CYCLE_INPUT *p1012CyclesInput = &pqlibExample.input1012Cycles;
	ADI_PQLIB_WAVEFORM *pWaveform = &pqlibExample.inputWaveform;
	ADI_PQLIB_OUTPUT_STATUS outputStatus;
	ADI_PQLIB_HANDLE hDevice = pqlibExample.hDevice;
	bool done = false;

	status = get_afe_input();

	while ((done == false) && (status == SYS_STATUS_SUCCESS)) {
		if (pCycleInput->isDataProcessed == false) {
			ADI_PQLIB_PROFBEG(ADI_PQLIB_PROFILE_ID_ONECYCLE);
			pqlibStatus =
				adi_pqlib_ProcessOneCycle(hDevice, &pqlibExample.inputCycle);
			ADI_PQLIB_PROFEND(ADI_PQLIB_PROFILE_ID_ONECYCLE);
			status = wait_for_zero_crossing(&pqlibExample, pqlibStatus);

			if (status != SYS_STATUS_SUCCESS) {
				break;
			}
			SyncLibTime(&pqlibExample, true);
			SyncToDip(&pqlibExample, pqlibStatus);
			pqlibExample.processedCycles++;

			status = process_pqlib_error(&pqlibExample, pqlibStatus);
			if (status != SYS_STATUS_SUCCESS) {
				break;
			}

			pqlibStatus = adi_pqlib_GetOutput(hDevice, &pqlibExample.output);
		}

		if (pqlibExample.state == PQLIB_STATE_WAITING_FOR_TRIGGER) {
			status = SYS_STATUS_PQLIB_RUNNING;
			break;
		}

		if (pWaveform->isDataProcessed == false) {
			ADI_PQLIB_PROFBEG(ADI_PQLIB_PROFILE_ID_WAVEFORM);
			pqlibStatus =
				adi_pqlib_ProcessWaveform(hDevice, &pqlibExample.inputWaveform);
			ADI_PQLIB_PROFEND(ADI_PQLIB_PROFILE_ID_WAVEFORM);
			status = process_pqlib_error(&pqlibExample, pqlibStatus);
			if (status != SYS_STATUS_SUCCESS) {
				break;
			}
		}

		if (p1012CyclesInput->isDataProcessed == 0 && processData) {
			ADI_PQLIB_PROFBEG(ADI_PQLIB_PROFILE_ID_1012CYCLES);
			pqlibStatus =
				adi_pqlib_Process1012Cycles(hDevice, &pqlibExample.input1012Cycles);
			ADI_PQLIB_PROFEND(ADI_PQLIB_PROFILE_ID_1012CYCLES);
			status = process_pqlib_error(&pqlibExample, pqlibStatus);
			if (status != SYS_STATUS_SUCCESS) {
				break;
			}
		}
		pqlibStatus = adi_pqlib_GetOutputStatus(hDevice, &outputStatus);

		status = process_pqlib_error(&pqlibExample, pqlibStatus);
		if (status != SYS_STATUS_SUCCESS) {
			break;
		}

		if (outputStatus != 0u) {
			pqlibStatus = adi_pqlib_GetOutput(hDevice, &pqlibExample.output);
			status = process_pqlib_error(&pqlibExample, pqlibStatus);
			if (status != 0) {
				break;
			}
		}

		status = SYS_STATUS_PQLIB_RUNNING;
		done = true;
	}

	return status;
}

int cpy_correction_coeffs(ADI_PQLIB_NOMINAL_FREQUENCY nominalFrequency,
			  ADI_PQLIB_CORR_COEF *pCorrCoef)
{
	int status = 0u;
	uint32_t i = 0;
	EXAMPLE_CONFIG *pConfig = &pqlibExample.exampleConfig;
	uint32_t numVCoeffs;
	uint32_t numICoeffs;

	if (nominalFrequency == ADI_PQLIB_NOMINAL_FREQUENCY_50HZ) {
		numICoeffs = pConfig->numCorrCoefCurrent50Hz;
		numVCoeffs = pConfig->numCorrCoefVoltage50Hz;
		if (pConfig->numCorrCoefCurrent50Hz) {
			for (i = 0; i < pConfig->numCorrCoefCurrent50Hz; i++) {
				pConfig->currentCorrCoef[i] =
					convert_to_coef_fract_type(pConfig->inputCurrentCorrCoef50Hz[i]);
			}
			pCorrCoef->numCoef = pConfig->numCorrCoefCurrent50Hz;
			pCorrCoef->startOrder =
				PQLIB_MAX_HARMONICS + 1 - pConfig->numCorrCoefCurrent50Hz;
			pCorrCoef->pCurrentCoef = &pConfig->currentCorrCoef[0];
		}
		if (pConfig->numCorrCoefVoltage50Hz) {
			for (i = 0; i < pConfig->numCorrCoefVoltage50Hz; i++) {
				pConfig->voltageCorrCoef[i] =
					convert_to_coef_fract_type(pConfig->inputVoltageCorrCoef50Hz[i]);
			}
			pCorrCoef->numCoef = pConfig->numCorrCoefVoltage50Hz;
			pCorrCoef->startOrder =
				PQLIB_MAX_HARMONICS + 1 - pConfig->numCorrCoefVoltage50Hz;
			pCorrCoef->pVoltageCoef = &pConfig->voltageCorrCoef[0];
		}
	} else {
		numICoeffs = pConfig->numCorrCoefCurrent60Hz;
		numVCoeffs = pConfig->numCorrCoefVoltage60Hz;
		if (pConfig->numCorrCoefCurrent60Hz) {
			for (i = 0; i < pConfig->numCorrCoefCurrent60Hz; i++) {
				pConfig->currentCorrCoef[i] =
					convert_to_coef_fract_type(pConfig->inputCurrentCorrCoef60Hz[i]);
			}
			pCorrCoef->numCoef = pConfig->numCorrCoefCurrent60Hz;
			pCorrCoef->startOrder =
				PQLIB_MAX_HARMONICS + 1 - pConfig->numCorrCoefCurrent60Hz;
			pCorrCoef->pCurrentCoef = &pConfig->currentCorrCoef[0];
		}

		if (pConfig->numCorrCoefVoltage60Hz) {
			for (i = 0; i < pConfig->numCorrCoefVoltage60Hz; i++) {
				pConfig->voltageCorrCoef[i] =
					convert_to_coef_fract_type(pConfig->inputVoltageCorrCoef60Hz[i]);
			}
			pCorrCoef->numCoef = pConfig->numCorrCoefVoltage60Hz;
			pCorrCoef->startOrder =
				PQLIB_MAX_HARMONICS + 1 - pConfig->numCorrCoefVoltage60Hz;
			pCorrCoef->pVoltageCoef = &pConfig->voltageCorrCoef[0];
		}
	}
	if (numVCoeffs != numICoeffs) {
		if (numVCoeffs == 0) {
			for (i = 0; i < numICoeffs; i++) {
				pConfig->voltageCorrCoef[i] = pConfig->currentCorrCoef[i];
			}
		} else if (numICoeffs == 0) {
			for (i = 0; i < numVCoeffs; i++) {
				pConfig->currentCorrCoef[i] = pConfig->voltageCorrCoef[i];
			}
		} else {
			if (numICoeffs < numVCoeffs) {
				pCorrCoef->numCoef = numICoeffs;
			} else {
				pCorrCoef->numCoef = numVCoeffs;
			}
		}
	} else if (numVCoeffs == 0) {
		status = 1;
	}
	return status;
}

void populate_event_config(EXAMPLE_CONFIG *pExampleConfig)
{
	ADI_PQLIB_CONFIG *pLibConfig = &pqlibExample.config;
	ADI_PQLIB_PCT_TYPE dipLowThreshold, dipHighThreshold, dipHysteresis;
	ADI_PQLIB_PCT_TYPE swellLowThreshold, swellHighThreshold, swellHysteresis;
	ADI_PQLIB_PCT_TYPE intrpLowThreshold, intrpHighThreshold, intrpHysteresis;
	ADI_PQLIB_PCT_TYPE rvcLowThreshold, rvcHighThreshold, rvcHysteresis;

	rvcHighThreshold = convert_to_pct_type(pExampleConfig->rvcThreshold);
	rvcHysteresis = convert_to_pct_type(pExampleConfig->rvcHysteresis);
	rvcLowThreshold = rvcHighThreshold - rvcHysteresis;

	pLibConfig->rvcThreshold.high = rvcHighThreshold;
	pLibConfig->rvcThreshold.low = rvcLowThreshold;
	dipLowThreshold = convert_to_pct_type(pExampleConfig->dipThreshold);
	dipHysteresis = convert_to_pct_type(pExampleConfig->dipHysteresis);
	dipHighThreshold = dipHysteresis + dipLowThreshold;

	swellHighThreshold = convert_to_pct_type(pExampleConfig->swellThreshold);
	swellHysteresis = convert_to_pct_type(pExampleConfig->swellHysteresis);
	swellLowThreshold = swellHighThreshold - swellHysteresis;

	intrpLowThreshold = convert_to_pct_type(pExampleConfig->intrpThreshold);
	intrpHysteresis = convert_to_pct_type(pExampleConfig->intrpHysteresis);
	intrpHighThreshold = intrpHysteresis + intrpLowThreshold;

	pLibConfig->dipThreshold.high = dipHighThreshold;
	pLibConfig->dipThreshold.low = dipLowThreshold;

	pLibConfig->swellThreshold.high = swellHighThreshold;
	pLibConfig->swellThreshold.low = swellLowThreshold;

	pLibConfig->intrpThreshold.high = intrpHighThreshold;
	pLibConfig->intrpThreshold.low = intrpLowThreshold;
}

int wait_for_zero_crossing(PQLIB_EXAMPLE *pExample,
			   ADI_PQLIB_RESULT pqlibStatus)
{
	int status = SYS_STATUS_SUCCESS;

	if (pqlibExample.state == PQLIB_STATE_WAITING_FOR_TRIGGER) {
		if (pqlibExample.exampleConfig.zeroCrossingCheck >
		    pqlibExample.zeroCrossingCount) {
			newSyncTimeAvailable = 0;
			if (pqlibStatus == ADI_PQLIB_RESULT_REFZX_TIMEOUT) {
				const uint32_t timeOutcycles = 1000;
				pqlibExample.zeroCrossingCount = 0;
				pqlibExample.timeOutCount++;

				if (pqlibExample.timeOutCount > timeOutcycles) {
					pqlibExample.exampleConfig.cyclesToRun = 0;
					pqlibExample.state = PQLIB_STATE_WAITING_FOR_START_CMD;
					status = SYS_STATUS_NO_SIGNAL;
				} else {
					status = SYS_STATUS_PQLIB_RUNNING;
				}
			} else {
				pqlibExample.zeroCrossingCount++;
				status = SYS_STATUS_PQLIB_RUNNING;
			}
		} else {
			const uint16_t maxSyncCycles = 100;
			if (pqlibExample.exampleConfig.enableRTCSync == false) {
				newSyncTimeAvailable = 1;
			}
			if ((newSyncTimeAvailable) || (pqlibExample.syncCycles > maxSyncCycles)) {
				bool clearTime = false;
				pqlibExample.state = PQLIB_STATE_RUNNING;
				adi_pqlib_Reset(pqlibExample.hDevice, clearTime);
				pqlibExample.processedCycles = 0;
				SyncLibTime(pExample, false);
			} else {
				status = SYS_STATUS_PQLIB_RUNNING;
			}
			pqlibExample.syncCycles++;
		}

		pqlibExample.input1012Cycles.isDataProcessed = 1;
		pqlibExample.inputWaveform.isDataProcessed = 1;
	}

	return status;
}

int process_pqlib_error(PQLIB_EXAMPLE *pExample,
			ADI_PQLIB_RESULT pqlibStatus)
{
	ADI_PQLIB_ERROR_TYPE errorType;
	int status = SYS_STATUS_SUCCESS;

	if (pqlibStatus != ADI_PQLIB_RESULT_SUCCESS) {
		adi_pqlib_GetErrorType(pqlibStatus, &errorType);
		if (errorType == ADI_PQLIB_ERROR_TYPE_CONFIG) {
			status = SYS_STATUS_PQLIB_CONFIG_ERROR;
			pqlibExample.exampleConfig.cyclesToRun = 0;
		}
	}

	return status;
}

void set_default_config(EXAMPLE_CONFIG *pConfig)
{
	int32_t i;
	static char *channelNames[] = {"VA", "VB", "VC", "IA", "IB", "IC", "IN"};

	for (i = 2; i < (PQLIB_MAX_HARMONICS + 1); i++) {
		pConfig->harmonics[i - 2] = i;
	}
	for (i = 1; i < (PQLIB_MAX_INTER_HARMONICS + 1); i++) {
		pConfig->interHarmonics[i - 1] = i;
	}
	pConfig->maxHarmonics = ADI_PQLIB_CFG_NUM_V_HARMONICS;
	pConfig->maxInterHarmonics = ADI_PQLIB_CFG_NUM_V_INTER_HARMONICS;
	for (i = 0; i < PQLIB_MAX_CHANNELS; i++) {
		pConfig->pEnabledChannels[i] = channelNames[i];
	}

	pConfig->zeroCrossingCheck = 0;
	pConfig->voltageScale = 1001;
	pConfig->currentScale = 294;
	pConfig->cyclesToRun = 0;
	pConfig->nominalFrequency = 50;
	pConfig->nominalVoltage = 0.3249;
	pConfig->dipHysteresis = 2;
	pConfig->dipThreshold = 90;
	pConfig->swellHysteresis = 2;
	pConfig->swellThreshold = 110;
	pConfig->intrpHysteresis = 2;
	pConfig->intrpThreshold = 5;
	pConfig->rvcThreshold = 6;
	pConfig->rvcHysteresis = 3;
	pConfig->correctionData.numCoef = 39;
	pConfig->correctionData.startOrder = 2;
	pConfig->correction = 1;
	pConfig->numCorrCoefCurrent50Hz = 0;
	pConfig->numCorrCoefVoltage50Hz = 0;
	pConfig->numCorrCoefCurrent60Hz = 0;
	pConfig->numCorrCoefVoltage60Hz = 0;
	pConfig->msvThreshold = 0.3;
	pConfig->msvCarrierFrequency = 330;
	pConfig->msvRecordingLength = 120;
	pConfig->phaseMap.phaseVA = ADI_PQLIB_PHASE_0;
	pConfig->phaseMap.phaseVB = ADI_PQLIB_PHASE_1;
	pConfig->phaseMap.phaseVC = ADI_PQLIB_PHASE_2;
	pConfig->phaseMap.phaseIA = ADI_PQLIB_PHASE_0;
	pConfig->phaseMap.phaseIB = ADI_PQLIB_PHASE_1;
	pConfig->phaseMap.phaseIC = ADI_PQLIB_PHASE_2;
	pConfig->phaseMap.phaseIN = ADI_PQLIB_PHASE_IN;
	pConfig->enableRTCSync = true;
	pConfig->enableIconsel = false;
	pConfig->useExternalTimestamp = false;
	pConfig->vconsel = VCONSEL_4W_WYE;
}

int SyncLibTime(PQLIB_EXAMPLE *pExample, bool checkRtcTime)
{
	ADI_PQLIB_RESULT pqlibStatus = ADI_PQLIB_RESULT_SUCCESS;
	ADI_PQLIB_HANDLE hDevice = pqlibExample.hDevice;
	const int16_t uncertainty = 20;
	bool applyImmediately = true;
	int status = 0;
	int64_t systemTime;
	int64_t rtcTime = 0;

	if (newSyncTimeAvailable) {
		newSyncTimeAvailable = 0;
		if (checkRtcTime) {
			adi_pqlib_GetTime(hDevice, &systemTime);
			if ((rtcTime - systemTime) >= 1000) {
				pqlibStatus =
					adi_pqlib_SetTime(hDevice, rtcTime, uncertainty, applyImmediately);
			}
		} else {
			pqlibStatus =
				adi_pqlib_SetTime(hDevice, rtcTime, uncertainty, applyImmediately);
		}
		status = process_pqlib_error(&pqlibExample, pqlibStatus);
	}

	return status;
}

void SyncToDip(PQLIB_EXAMPLE *pExample, ADI_PQLIB_RESULT pqlibStatus)
{
	bool applyImmediately = true;
	uint64_t systemTime;
	const int16_t uncertainty = 0;

	if (pqlibExample.waitingForSync == true) {
		if ((pqlibStatus == ADI_PQLIB_RESULT_DIP_STARTED) ||
		    ((pqlibStatus == ADI_PQLIB_RESULT_INTRP_STARTED))) {

			systemTime = pqlibExample.exampleConfig.syncTime;
			pqlibExample.waitingForSync = false;
			adi_pqlib_SetTime(pqlibExample.hDevice, systemTime, uncertainty,
					  applyImmediately);
		}
	}
}
