/*******************************************************************************
 *   @file   pqlib_example.c
 *   @brief  Pqlib example functions
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

#include "pqlib_example.h"
#include "afe_config.h"
#include "afe_calibration.h"
#include "afe_sync.h"
#include "flash_storage.h"
#include "pqlib_afe.h"
#include "iio_server.h"

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <inttypes.h>

LOG_MODULE_REGISTER(pqlib_example, LOG_LEVEL_INF);

extern volatile bool processData;

PQLIB_EXAMPLE pqlibExample;

/* --- Dedicated AFE processing thread ---
 * Runs pqm_one_cycle() in a tight cooperative loop so AFE polling
 * and PQLib processing are never delayed by IIO, PTP, or logging. */
#define AFE_THREAD_STACK_SIZE 8192
#define AFE_THREAD_PRIORITY   K_PRIO_COOP(2)

K_THREAD_STACK_DEFINE(afe_thread_stack, AFE_THREAD_STACK_SIZE);
static struct k_thread afe_thread_data;
volatile uint8_t newSyncTimeAvailable = 0;

/* PTP time for SyncLibTime - set by main.c from TS_CAPT */
volatile int64_t ptp_time_ms = 0;

volatile bool configChanged = false;

/*
 * Persistent event accumulator — PQLib clears event counts after each
 * GetOutput() call, so events only live for one processing cycle (~20ms).
 * We copy new events here so IIO clients can read them at any time.
 * Cleared explicitly via the "clear_events" IIO attribute.
 *
 * Events are only accumulated after PTP time has converged, because
 * PQLib timestamps before convergence use an unsynced clock and would
 * show large cross-board deltas.
 */
ADI_PQLIB_EVENTS accumulated_events;

/* Gate: don't accumulate events until PTP time is reliable */
static bool events_accumulation_enabled = false;

/* Diagnostic counters — visible via console logs */
static uint32_t event_ended_count = 0;  /* total *_ENDED seen */
static uint32_t event_accumulated_count = 0;  /* successful accumulations */

/* Lifetime per-type event totals (never reset except by power cycle) */
uint32_t total_dip_count;
uint32_t total_swell_count;
uint32_t total_intrp_count;
uint32_t total_rvc_count;

/* Dedup: track last accumulated event startTime per type.
 * PQLib event data persists in GetOutput until the next event
 * overwrites it. Compare startTimes to avoid re-accumulating
 * stale data from previous events. */
static int64_t last_accum_dip_start;
static int64_t last_accum_swell_start;
static int64_t last_accum_intrp_start;
static int64_t last_accum_rvc_start;

/* Fallback event capture: record STARTED timestamp so we can
 * synthesize event data if PQLib's 10/12 output is corrupted
 * (e.g. after the i194 bug). Uses full PTP nanosecond timestamps
 * from TS_CAPT for sub-μs cross-board correlation. */
static bool event_in_progress;
static ADI_PQLIB_RESULT event_started_status;
static int64_t event_started_ext_ts;       /* PQLib ms (for PQLib fields) */
static uint32_t event_started_ptp_sec;     /* PTP seconds at STARTED */
static uint32_t event_started_ptp_nsec;    /* PTP nanoseconds at STARTED */

void pqlib_enable_event_accumulation(void)
{
	if (!events_accumulation_enabled) {
		events_accumulation_enabled = true;
		/* Clear any events that sneaked in before PTP convergence */
		memset(&accumulated_events, 0, sizeof(accumulated_events));
		last_accum_dip_start = 0;
		last_accum_swell_start = 0;
		last_accum_intrp_start = 0;
		last_accum_rvc_start = 0;
		event_in_progress = false;
		LOG_DBG("Event accumulation enabled (PTP converged)");
	}
}

/*
 * Check if PQLib result code indicates a completed event.
 * Only capture events when *_ENDED — at that point PQLib guarantees
 * the event struct is fully populated (startTime, endTime, durationInCycles).
 * When *_STARTED, the event struct may contain partially-filled data
 * or garbage from PQLib's internal buffer reuse.
 */
static bool is_event_ended(ADI_PQLIB_RESULT status)
{
	return (status == ADI_PQLIB_RESULT_DIP_ENDED ||
		status == ADI_PQLIB_RESULT_SWELL_ENDED ||
		status == ADI_PQLIB_RESULT_INTRP_ENDED ||
		status == ADI_PQLIB_RESULT_RVC_ENDED);
}

static bool is_event_started(ADI_PQLIB_RESULT status)
{
	return (status == ADI_PQLIB_RESULT_DIP_STARTED ||
		status == ADI_PQLIB_RESULT_SWELL_STARTED ||
		status == ADI_PQLIB_RESULT_INTRP_STARTED ||
		status == ADI_PQLIB_RESULT_RVC_STARTED);
}

/* Validate that event counts don't exceed buffer limits.
 * PQLib occasionally (at the third event for example) reports garbage counts (e.g. intrpCount=194)
 * during STARTED events or 10/12-cycle outputs, which would cause
 * out-of-bounds reads when accessing the fixed-size arrays. */
static bool events_counts_valid(const ADI_PQLIB_EVENTS *ev)
{
	return (ev->dipCount <= ADI_PQLIB_CFG_NUM_DIPS &&
		ev->swellCount <= ADI_PQLIB_CFG_NUM_SWELLS &&
		ev->intrpCount <= ADI_PQLIB_CFG_NUM_INTRP &&
		ev->rvcCount <= ADI_PQLIB_CFG_NUM_RVC);
}

void pqlib_accumulate_events(const ADI_PQLIB_OUTPUT *output,
			     ADI_PQLIB_RESULT cycleStatus)
{
	if (!output)
		return;

	if (is_event_started(cycleStatus)) {
		LOG_INF("Event STARTED (status=%d, ptp=%u.%09u)",
			cycleStatus,
			(uint32_t)last_measurement_ptp_ts.second,
			last_measurement_ptp_ts.nanosecond);
		event_in_progress = true;
		event_started_status = cycleStatus;
		event_started_ext_ts = pqlibExample.inputCycle.timestamp;
		event_started_ptp_sec = (uint32_t)last_measurement_ptp_ts.second;
		event_started_ptp_nsec = last_measurement_ptp_ts.nanosecond;
	} else if (is_event_ended(cycleStatus)) {
		int64_t end_ts = pqlibExample.inputCycle.timestamp;

		uint32_t end_ptp_sec = (uint32_t)last_measurement_ptp_ts.second;
		uint32_t end_ptp_nsec = last_measurement_ptp_ts.nanosecond;

		LOG_INF("Event ENDED (status=%d, ptp=%u.%09u)",
			cycleStatus, end_ptp_sec, end_ptp_nsec);

		/* Synthesize event from ProcessOneCycle timestamps.
		 * PQLib's 10/12 output may never populate event data
		 * after the i194 corruption bug. This fallback ensures
		 * every STARTED/ENDED pair produces an accumulated event.
		 * If the 10/12 path also has fresh data, its dedup (>)
		 * will overwrite with PQLib's more detailed record. */
		if (event_in_progress && events_accumulation_enabled) {
			int64_t start_ts = event_started_ext_ts;
			uint32_t dur_ms = (uint32_t)(end_ts - start_ts);
			uint32_t dur_cycles = dur_ms * 50 / 1000;

			memset(&accumulated_events, 0,
			       sizeof(accumulated_events));

			if (cycleStatus == ADI_PQLIB_RESULT_INTRP_ENDED) {
				accumulated_events.intrpCount = 1;
				accumulated_events.intrp[0].startTime = start_ts;
				accumulated_events.intrp[0].endTime = end_ts;
				accumulated_events.intrp[0].durationInCycles =
					dur_cycles;
				last_accum_intrp_start = start_ts;
				total_intrp_count++;
			} else if (cycleStatus == ADI_PQLIB_RESULT_DIP_ENDED) {
				accumulated_events.dipCount = 1;
				accumulated_events.dips[0].startTime = start_ts;
				accumulated_events.dips[0].endTime = end_ts;
				accumulated_events.dips[0].durationInCycles =
					dur_cycles;
				last_accum_dip_start = start_ts;
				total_dip_count++;
			} else if (cycleStatus ==
				   ADI_PQLIB_RESULT_SWELL_ENDED) {
				accumulated_events.swellCount = 1;
				accumulated_events.swells[0].startTime = start_ts;
				accumulated_events.swells[0].endTime = end_ts;
				accumulated_events.swells[0].durationInCycles =
					dur_cycles;
				last_accum_swell_start = start_ts;
				total_swell_count++;
			} else if (cycleStatus == ADI_PQLIB_RESULT_RVC_ENDED) {
				accumulated_events.rvcCount = 1;
				accumulated_events.rvc[0].startTime = start_ts;
				accumulated_events.rvc[0].endTime = end_ts;
				accumulated_events.rvc[0].durationInCycles =
					dur_cycles;
				last_accum_rvc_start = start_ts;
				total_rvc_count++;
			}

			event_ended_count++;
			event_accumulated_count++;

			LOG_INF("Event fallback #%u: d%u s%u i%u r%u "
				"start=%u.%09u end=%u.%09u dur=%u",
				event_ended_count,
				accumulated_events.dipCount,
				accumulated_events.swellCount,
				accumulated_events.intrpCount,
				accumulated_events.rvcCount,
				event_started_ptp_sec,
				event_started_ptp_nsec,
				end_ptp_sec, end_ptp_nsec,
				dur_cycles);
		}

		event_in_progress = false;
	}
}

void pqlib_clear_accumulated_events(void)
{
	memset(&accumulated_events, 0, sizeof(accumulated_events));
	last_accum_dip_start = 0;
	last_accum_swell_start = 0;
	last_accum_intrp_start = 0;
	last_accum_rvc_start = 0;
	event_in_progress = false;
	LOG_INF("Accumulated events cleared (ended=%u accum=%u)",
		event_ended_count, event_accumulated_count);
}

void pqlib_log_event_stats(void)
{
	LOG_DBG("Event stats: ended=%u accum=%u | acc.d%u s%u i%u r%u",
		event_ended_count, event_accumulated_count,
		accumulated_events.dipCount, accumulated_events.swellCount,
		accumulated_events.intrpCount, accumulated_events.rvcCount);
}

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
	/* Check for AFE sync restart request (from 1PPS handler).
	 * Must run in AFE thread context to avoid SPI/PQLib contention. */
	if (afe_sync_check_restart()) {
		return 0;  /* Restart done, skip this cycle */
	}

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
	return process_and_prepare_output();
}

static void afe_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	LOG_DBG("AFE thread started (prio=%d)", AFE_THREAD_PRIORITY);

	while (1) {
		pqm_one_cycle();
		k_usleep(200);
	}
}

int pqm_afe_thread_start(void)
{
	k_thread_create(&afe_thread_data, afe_thread_stack,
			K_THREAD_STACK_SIZEOF(afe_thread_stack),
			afe_thread_fn, NULL, NULL, NULL,
			AFE_THREAD_PRIORITY, 0, K_NO_WAIT);
	k_thread_name_set(&afe_thread_data, "afe_pqlib");
	LOG_DBG("AFE processing thread created");
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
		config = 2 * (uint32_t)1144084;
		status = afe_write_32bit_reg(REG_VLEVEL, (uint32_t *)&config);
	}

	/* Energy accumulation time: 100 half-cycles at 50Hz, 120 at 60Hz = ~1s */
	if (status == 0) {
		config = (pExampleConfig->nominalFrequency == 50) ? 99 : 119;
		status = afe_write_16bit_reg(REG_EGY_TIME, (uint16_t *)&config);
	}

	/* Enable energy comparison for all 3 phases */
	if (status == 0) {
		config = 0x0007;
		status = afe_write_16bit_reg(REG_COMPMODE, (uint16_t *)&config);
	}

	/* Energy/power accumulation: EGY_POW_EN + zero-crossing + done */
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

int process_and_prepare_output(void)
{
	int status = 0;
	ADI_PQLIB_RESULT pqlibStatus = ADI_PQLIB_RESULT_SUCCESS;
	ADI_PQLIB_RESULT cycleStatus = ADI_PQLIB_RESULT_SUCCESS;
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

			/* Save ProcessOneCycle status — it tells us whether
			 * an event started or ended in this cycle.
			 * GetOutput below will overwrite pqlibStatus. */
			cycleStatus = pqlibStatus;

			/* Diagnostic: log non-SUCCESS ProcessOneCycle results.
			 * Event status codes (21-28) are logged always since they're
			 * rare. All other non-SUCCESS codes are throttled to avoid
			 * flooding the log buffer from the tight AFE thread loop. */
			if (pqlibStatus != ADI_PQLIB_RESULT_SUCCESS) {
				if (is_event_started(pqlibStatus) ||
				    is_event_ended(pqlibStatus)) {
					LOG_DBG("ProcessOneCycle: status=%d", pqlibStatus);
				} else {
					static uint32_t nonevt_count = 0;
					nonevt_count++;
					if ((nonevt_count % 100) == 1) {
						LOG_DBG("ProcessOneCycle: status=%d (count=%u)",
							pqlibStatus, nonevt_count);
					}
				}
			}

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

			/* Lock mutex only around GetOutput + event accumulation.
			 * This is the only section that writes to pqlibExample.output
			 * (which IIO reads). AFE SPI reads and PQLib processing
			 * above run WITHOUT the mutex to avoid blocking. */
			k_mutex_lock(&pqm_data_mutex, K_FOREVER);

			pqlibStatus = adi_pqlib_GetOutput(hDevice, &pqlibExample.output);

			pqlib_accumulate_events(pqlibExample.output, cycleStatus);

			k_mutex_unlock(&pqm_data_mutex);
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

			if (pqlibStatus != ADI_PQLIB_RESULT_SUCCESS) {
				LOG_DBG("ProcessWaveform: status=%d", pqlibStatus);
			}

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

			/* Diagnostic: log non-SUCCESS Process1012Cycles results.
			 * If PQLib reports INTRP_ENDED here instead of from
			 * ProcessOneCycle, this will reveal it. */
			if (pqlibStatus != ADI_PQLIB_RESULT_SUCCESS) {
				LOG_DBG("Process1012Cycles: status=%d", pqlibStatus);
			}

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
			/* Lock mutex around GetOutput + event accumulation */
			k_mutex_lock(&pqm_data_mutex, K_FOREVER);

			pqlibStatus = adi_pqlib_GetOutput(hDevice, &pqlibExample.output);

			/* The 10/12-cycle GetOutput is the RELIABLE event capture
			 * path. PQLib populates event structs only after
			 * Process1012Cycles, not after ProcessOneCycle.
			 * Dedup via startTime comparison prevents re-accumulating
			 * stale data that persists in GetOutput between events. */
			if (pqlibExample.output != NULL) {
				const ADI_PQLIB_EVENTS *ev2 = &pqlibExample.output->events;
				if (ev2->dipCount || ev2->swellCount ||
				    ev2->intrpCount || ev2->rvcCount) {
					if (!events_counts_valid(ev2)) {
						/* i194 bug: PQLib reports garbage
						 * counts. Do NOT call SetConfig
						 * or Reset — that corrupts the
						 * handle. Just skip; fallback
						 * synthesis handles events. */
						static uint32_t bogus_count;
						bogus_count++;
						if (bogus_count <= 3) {
							LOG_WRN("Bogus 10/12 event "
								"counts: d%u s%u i%u "
								"r%u — skipping (#%u)",
								ev2->dipCount,
								ev2->swellCount,
								ev2->intrpCount,
								ev2->rvcCount,
								bogus_count);
						}
					} else if (events_accumulation_enabled) {
						/* Dedup: require startTime strictly
						 * NEWER than last accumulated.
						 * Rejects both identical re-reads
						 * and stale data from older events
						 * (e.g. after i194 corruption). */
						bool is_new = false;
						if (ev2->intrpCount &&
						    ev2->intrp[0].startTime > last_accum_intrp_start)
							is_new = true;
						if (ev2->dipCount &&
						    ev2->dips[0].startTime > last_accum_dip_start)
							is_new = true;
						if (ev2->swellCount &&
						    ev2->swells[0].startTime > last_accum_swell_start)
							is_new = true;
						if (ev2->rvcCount &&
						    ev2->rvc[0].startTime > last_accum_rvc_start)
							is_new = true;

						if (is_new) {
							accumulated_events = *ev2;
							event_ended_count++;
							event_accumulated_count++;

							/* Lifetime per-type totals */
							total_dip_count += ev2->dipCount;
							total_swell_count += ev2->swellCount;
							total_intrp_count += ev2->intrpCount;
							total_rvc_count += ev2->rvcCount;

							/* Update dedup timestamps */
							if (ev2->intrpCount)
								last_accum_intrp_start = ev2->intrp[0].startTime;
							if (ev2->dipCount)
								last_accum_dip_start = ev2->dips[0].startTime;
							if (ev2->swellCount)
								last_accum_swell_start = ev2->swells[0].startTime;
							if (ev2->rvcCount)
								last_accum_rvc_start = ev2->rvc[0].startTime;

							LOG_DBG("10/12 event captured #%u: "
								"d%u s%u i%u r%u "
								"(outputStatus=0x%x)",
								event_ended_count,
								ev2->dipCount, ev2->swellCount,
								ev2->intrpCount, ev2->rvcCount,
								outputStatus);

							for (uint8_t i = 0; i < ev2->dipCount; i++) {
								LOG_DBG("  dip[%u] start=%" PRId64
									" end=%" PRId64 " dur=%u",
									i, ev2->dips[i].startTime,
									ev2->dips[i].endTime,
									ev2->dips[i].durationInCycles);
							}
							for (uint8_t i = 0; i < ev2->swellCount; i++) {
								LOG_DBG("  swell[%u] start=%" PRId64
									" end=%" PRId64 " dur=%u",
									i, ev2->swells[i].startTime,
									ev2->swells[i].endTime,
									ev2->swells[i].durationInCycles);
							}
							for (uint8_t i = 0; i < ev2->intrpCount; i++) {
								LOG_DBG("  intrp[%u] start=%" PRId64
									" end=%" PRId64 " dur=%u",
									i, ev2->intrp[i].startTime,
									ev2->intrp[i].endTime,
									ev2->intrp[i].durationInCycles);
							}
							for (uint8_t i = 0; i < ev2->rvcCount; i++) {
								LOG_DBG("  rvc[%u] start=%" PRId64
									" end=%" PRId64 " dur=%u",
									i, ev2->rvc[i].startTime,
									ev2->rvc[i].endTime,
									ev2->rvc[i].durationInCycles);
							}
						}
					}
				}
			}

			k_mutex_unlock(&pqm_data_mutex);

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
	pConfig->useExternalTimestamp = true;
	pConfig->vconsel = VCONSEL_4W_WYE;
	pConfig->calNominalCurrent = 10.0f;     /* 10 Arms for gain cal */
	pConfig->calNominalVoltage = 230.0f;    /* 230 Vrms for gain cal */
	pConfig->calOffsetCurrent = 1.0f;       /* 1.0 Arms for offset cal */
	pConfig->calOffsetVoltage = 10.0f;      /* 10 Vrms for offset cal */
	pConfig->currentPgaGain = 1.0f;
	pConfig->voltagePgaGain = 1.0f;
}

int SyncLibTime(PQLIB_EXAMPLE *pExample, bool checkRtcTime)
{
	ADI_PQLIB_RESULT pqlibStatus = ADI_PQLIB_RESULT_SUCCESS;
	ADI_PQLIB_HANDLE hDevice = pqlibExample.hDevice;
	const int16_t uncertainty = 1; /* ISR captures at exact 1PPS boundary */
	bool applyImmediately = true;
	int status = 0;
	int64_t systemTime;
	int64_t rtcTime = ptp_time_ms;

	if (newSyncTimeAvailable) {
		newSyncTimeAvailable = 0;
		if (checkRtcTime) {
			adi_pqlib_GetTime(hDevice, &systemTime);
			int64_t drift = rtcTime - systemTime;

			/* Log every 10th sync (~10s) to monitor PQLib internal time */
			static uint32_t sync_count;
			sync_count++;
			if ((sync_count % 10) == 1) {
				LOG_DBG("SyncLib #%u: ptp=%" PRId64
					" lib=%" PRId64 " drift=%" PRId64,
					sync_count, rtcTime, systemTime, drift);
			}

			if (drift < 0) {
				drift = -drift;
			}
			if (drift >= 1) { /* Correct if drift > 1ms */
				pqlibStatus =
					adi_pqlib_SetTime(hDevice, rtcTime, uncertainty, applyImmediately);
				LOG_DBG("SetTime(%" PRId64 "): status=%d",
					rtcTime, pqlibStatus);
			}
		} else {
			pqlibStatus =
				adi_pqlib_SetTime(hDevice, rtcTime, uncertainty, applyImmediately);
			LOG_DBG("SetTime initial(%" PRId64 "): status=%d",
				rtcTime, pqlibStatus);
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

