/*******************************************************************************
 *   @file   pqlib_example.h
 *   @brief  Pqlib example functions for display
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

#include "pqlib_example.h"
#include "pqlib_afe.h"
#include "iio_pqm.h"
#include "afe_calibration.h"
#include "flash_storage.h"
#ifdef UART_EXPORT_ENABLED
#include "uart_export.h"
#endif

PQLIB_EXAMPLE pqlibExample;
extern volatile uint8_t newSyncTimeAvailable = 0;
extern volatile bool configChanged;

/**
 * @brief Set default configuration values for the example application
 */
int pqm_measurement_init(void)
{
	int status = 0;

	set_default_config(&pqlibExample.exampleConfig);

	pqlibExample.state = PQLIB_STATE_WAITING_FOR_START_CMD;
	pqlibExample.processedCycles = 0;
	pqlibExample.inputCycle.sequenceNumber = 0;
	pqlibExample.input1012Cycles.sequenceNumber = 0;
	pqlibExample.calibrationRequested = false;

	/* Initialize calibration context */
	calibration_init();

	/* Initialize flash storage for calibration persistence */
	flash_storage_init();

	status = open_pqlib(&pqlibExample);

	return status;
}

/**
 * @brief Start the measurement process by configuring the AFE and PQLIB,
 * then waiting for the first trigger (zero-crossing)
 * @param waitingForSync If true, will wait for a synchronization event before starting;
 * if false, will start immediately
 * @return 0 on success, or an error code on failure
 */
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

/**
 * @brief Main processing loop for the PQLIB example. This function should be called
 * repeatedly (e.g. in a main loop) to process incoming data, handle calibration,
 * and prepare output for display or export.
 * @return 0 on success, or an error code on failure
 */
int pqm_one_cycle(void)
{

	/* Check for calibration request first */
	if (pqlibExample.calibrationRequested || calibration_is_active()) {
		if (pqlibExample.state != PQLIB_STATE_CALIBRATING) {
			pqlibExample.state = PQLIB_STATE_CALIBRATING;
		}
		int cal_status = calibration_process_cycle();
		if (!calibration_is_active()) {
			/* Calibration complete, resume normal operation */
			pqlibExample.calibrationRequested = false;
			pqlibExample.state = PQLIB_STATE_WAITING_FOR_TRIGGER;
			pqm_start_measurement(false);
		}
		return cal_status;
	}

	if (configChanged) {
		pqm_start_measurement(false);
		configChanged = false;
		pqlibExample.state = PQLIB_STATE_WAITING_FOR_TRIGGER;
	}
	process_and_prepare_output();
	return 0;
}

/**
 * @brief Open a connection to PQLIB and allocate necessary resources
 * @param pExample Pointer to the example context structure
 * @return 0 on success, or an error code on failure
 */
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

/**
 * @brief Configure the AFE and PQLIB with the desired settings for measurement
 * @param pExample Pointer to the example context structure
 * @return 0 on success, or an error code on failure
 */
int config_measurement(PQLIB_EXAMPLE *pExample)
{
	int status = 0;
	uint32_t config = 0;

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

	/* Configure VLEVEL for headroom above nominal voltage */
	if (status == 0) {
		config = 3 * (uint32_t)1144084;
		status = afe_write_32bit_reg(REG_VLEVEL, (uint32_t *)&config);
	}

	/* Energy accumulation time: 100 half-cycles at 50Hz, 120 at 60Hz = ~1 second */
	if (status == 0) {
		config = (pExampleConfig->nominalFrequency == 50) ? 99 : 119;
		status = afe_write_16bit_reg(REG_EGY_TIME, (uint16_t *)&config);
	}

	/* Enable energy comparison for all 3 phases */
	if (status == 0) {
		config = 0x0007;
		status = afe_write_16bit_reg(REG_COMPMODE, (uint16_t *)&config);
	}

	/* Energy/power accumulation: EGY_POW_EN + zero-crossing + accumulation done */
	if (status == 0) {
		config = 0x0013;
		status = afe_write_16bit_reg(REG_EP_CFG, (uint16_t *)&config);
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

	/* Auto-load calibration from flash if available, but skip if calibration
	 * was just performed (to avoid overwriting newly calibrated values) */
	if (status == 0 && flash_storage_is_initialized() &&
	    flash_has_valid_calibration() && !calibration_is_done()) {
		flash_load_and_apply_calibration();
	}

	return status;
}

/**
 * @brief Process one cycle of data from PQLIB, handle any errors, and prepare output for display or export
 * @return 0 on success, or an error code on failure
 */
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
	static uint32_t last_afe_isr = 0;
	uint32_t isr = dbg_isr_count;

	waveform_drain_to_cb();
	if ((isr - last_afe_isr) >= 4 && !waveform_dma_is_busy()) {
		last_afe_isr = isr;
		status = get_afe_input();
	}

	if (pCycleInput->isDataProcessed == false) {
		pqlibStatus =
			adi_pqlib_ProcessOneCycle(hDevice, &pqlibExample.inputCycle);
		status = wait_for_zero_crossing(&pqlibExample, pqlibStatus);
		if (status != SYS_STATUS_SUCCESS)
			return status;

		SyncLibTime(&pqlibExample, true);
		SyncToDip(&pqlibExample, pqlibStatus);
		pqlibExample.processedCycles++;

		status = process_pqlib_error(&pqlibExample, pqlibStatus);
		if (status != SYS_STATUS_SUCCESS)
			return status;

		pqlibStatus = adi_pqlib_GetOutput(hDevice, &pqlibExample.output);
	}

	if (pqlibExample.state == PQLIB_STATE_WAITING_FOR_TRIGGER)
		return SYS_STATUS_PQLIB_RUNNING;

	if (p1012CyclesInput->isDataProcessed == 0 && processData) {
		waveform_drain_to_cb();
		pqlibStatus =
			adi_pqlib_Process1012Cycles(hDevice, &pqlibExample.input1012Cycles);
		waveform_drain_to_cb();
		status = process_pqlib_error(&pqlibExample, pqlibStatus);
		if (status != SYS_STATUS_SUCCESS)
			return status;
	}

	pqlibStatus = adi_pqlib_GetOutputStatus(hDevice, &outputStatus);
	status = process_pqlib_error(&pqlibExample, pqlibStatus);
	if (status != SYS_STATUS_SUCCESS)
		return status;

	if (outputStatus != 0u) {
		pqlibStatus = adi_pqlib_GetOutput(hDevice, &pqlibExample.output);
		status = process_pqlib_error(&pqlibExample, pqlibStatus);
		if (status != 0)
			return status;
#ifdef UART_EXPORT_ENABLED
		uart_export_send();
#endif
	}

	return status;
}

/**
 * @brief Copy correction coefficients from the example configuration to the PQLIB configuration structure,
 * converting to the appropriate fixed-point format
 * @param nominalFrequency The nominal frequency (50Hz or 60Hz) to determine which set of coefficients to use
 * @param pCorrCoef Pointer to the PQLIB correction coefficient structure to populate
 * @return 0 on success, or 1 if there was a mismatch in the number of voltage and current coefficients
 */
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

/**
 * @brief Populate the PQLIB event configuration based on the example configuration settings,
 * converting thresholds and hysteresis values to the appropriate fixed-point format
 * @param pExampleConfig Pointer to the example configuration structure containing user-defined settings
 */
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

/**
 * @brief Handle the logic for waiting for zero-crossing triggers before starting the measurement,
 * including handling timeouts and synchronization events
 * @param pExample Pointer to the example context structure
 * @param pqlibStatus The result code from the last PQLIB operation, used to determine if a timeout occurred
 * @return SYS_STATUS_SUCCESS if ready to start measurement, SYS_STATUS_PQLIB_RUNNING if still waiting for trigger,
 * or SYS_STATUS_NO_SIGNAL if a timeout occurred without detecting the required number of zero-crossings
 */
int wait_for_zero_crossing(PQLIB_EXAMPLE *pExample,
			   ADI_PQLIB_RESULT pqlibStatus)
{
	int status = SYS_STATUS_SUCCESS;
	const uint32_t timeOutcycles = 1000;
	const uint16_t maxSyncCycles = 100;
	bool clearTime = false;

	if (pqlibExample.state == PQLIB_STATE_WAITING_FOR_TRIGGER) {
		if (pqlibExample.exampleConfig.zeroCrossingCheck >
		    pqlibExample.zeroCrossingCount) {
			newSyncTimeAvailable = 0;
			if (pqlibStatus == ADI_PQLIB_RESULT_REFZX_TIMEOUT) {
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

			if (pqlibExample.exampleConfig.enableRTCSync == false) {
				newSyncTimeAvailable = 1;
			}
			if ((newSyncTimeAvailable) || (pqlibExample.syncCycles > maxSyncCycles)) {
				clearTime = false;
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

/**
 * @brief Process the result code from a PQLIB operation and determine if there was an error,
 * updating the example state accordingly
 * @param pExample Pointer to the example context structure
 * @param pqlibStatus The result code from the PQLIB operation to check for errors
 * @return SYS_STATUS_SUCCESS if no error, or an appropriate error code if a configuration error occurred
 */
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

/**
 * @brief Set default configuration values for the example application, including harmonic settings,
 * channel names, thresholds, and other parameters
 * @param pConfig Pointer to the example configuration structure to populate with default values
 */
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
	pConfig->calNominalCurrent = 10.0f;     /* 10 Arms for gain cal */
	pConfig->calNominalVoltage = 230.0f;    /* 230 Vrms for gain cal */
	pConfig->calOffsetCurrent = 1.0f;       /* 1.0 Arms for offset cal */
	pConfig->calOffsetVoltage = 10.0f;      /* 10 Vrms for offset cal */
	pConfig->currentPgaGain = 1.0f;         /* Default PGA gain */
	pConfig->voltagePgaGain = 1.0f;         /* Default PGA gain */
}

/**
 * @brief Synchronize the PQLIB internal time with an external RTC time if available,
 * or with the system time if RTC sync is not enabled
 * @param pExample Pointer to the example context structure
 * @param checkRtcTime If true, will compare the RTC time with the system time and only
 * update if they differ by more than 1 second;
 * if false, will update the PQLIB time with the RTC time without checking
 * @return 0 on success, or an error code on failure
 */
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

/**
 * @brief If the example is configured to wait for synchronization, and a synchronization event has occurred
 * (indicated by the PQLIB status), update the PQLIB time with the synchronization time from the example configuration
 * @param pExample Pointer to the example context structure
 * @param pqlibStatus The result code from the last PQLIB operation,
 * used to determine if a synchronization event occurred
 */
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
