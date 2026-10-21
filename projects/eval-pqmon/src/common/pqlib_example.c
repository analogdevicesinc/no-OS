/*******************************************************************************
 *   @file   pqlib_example.h
 *   @brief  Pqlib example functions for display
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "pqlib_example.h"
#include "pqlib_afe.h"
#include "afe_queue.h"
#include "max32650.h"
#include "iio_pqm.h"
#include "afe_calibration.h"
#include "flash_storage.h"
#ifdef UART_EXPORT_ENABLED
#include "uart_export.h"
#endif
#include <string.h>
#include "no_os_delay.h"
#include "no_os_timer.h"
#include "parameters.h"
#include <stdio.h>
#include <inttypes.h>

PQLIB_EXAMPLE pqlibExample;
extern volatile uint8_t newSyncTimeAvailable = 0;
extern volatile bool configChanged;

/* Zero-filled placeholder so IIO reads are safe before the first GetOutput */
static ADI_PQLIB_OUTPUT emptyOutput;

/* Persistent event record - PQLib clears its events after each GetOutput(),
 * so the latest valid events are kept here until newer ones arrive. */
ADI_PQLIB_EVENTS accumulated_events;

/* Lifetime per-type event totals (reset by power cycle or pqm_clear_events) */
uint32_t total_dip_count;
uint32_t total_swell_count;
uint32_t total_intrp_count;
uint32_t total_rvc_count;

/* Number of GetOutput results rejected because of invalid event counts */
uint32_t bogus_event_count;

/* Start time of the newest accumulated event per type, used for dedup */
#define NO_EVENT_TIME	((ADI_PQLIB_TIME)-1)
static ADI_PQLIB_TIME lastDipStart = NO_EVENT_TIME;
static ADI_PQLIB_TIME lastSwellStart = NO_EVENT_TIME;
static ADI_PQLIB_TIME lastIntrpStart = NO_EVENT_TIME;
static ADI_PQLIB_TIME lastRvcStart = NO_EVENT_TIME;

/**
 * @brief Check that PQLib event counts fit the configured event arrays.
 * PQLib occasionally reports garbage counts (e.g. intrpCount=194).
 * @param ev - PQLib events structure
 * @return true if all counts are valid
 */
static bool events_counts_valid(const ADI_PQLIB_EVENTS *ev)
{
	return (ev->dipCount <= ADI_PQLIB_CFG_NUM_DIPS &&
		ev->swellCount <= ADI_PQLIB_CFG_NUM_SWELLS &&
		ev->intrpCount <= ADI_PQLIB_CFG_NUM_INTRP &&
		ev->rvcCount <= ADI_PQLIB_CFG_NUM_RVC);
}

/* Common leading fields of ADI_PQLIB_DIP/SWELL/INTRP/RVC */
typedef struct {
	ADI_PQLIB_TIME startTime;
	ADI_PQLIB_TIME endTime;
	uint32_t durationInCycles;
} EVENT_TIMING;

#define EVENT_TIMING_LAYOUT_OK(type)					\
	(offsetof(type, endTime) == offsetof(EVENT_TIMING, endTime) &&	\
	 offsetof(type, durationInCycles) ==				\
	 offsetof(EVENT_TIMING, durationInCycles))

_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_DIP), "dip layout");
_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_SWELL), "swell layout");
_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_INTRP), "intrp layout");
_Static_assert(EVENT_TIMING_LAYOUT_OK(ADI_PQLIB_RVC), "rvc layout");

/* Max number of rejected event records printed on the debug console */
#define MAX_BOGUS_EVENT_PRINTS	5

#ifdef PQM_EVENT_DEBUG
/**
 * @brief Print a PQLib result code on the debug console when it changes.
 * @param api    - API name
 * @param res    - result code returned by the API
 * @param last   - last printed code for this API (updated)
 */
static void event_debug_result(const char *api, ADI_PQLIB_RESULT res,
			       ADI_PQLIB_RESULT *last)
{
	if (res == *last)
		return;
	*last = res;
	printf("[c%" PRIu32 "] %s -> %d\n\r", pqlibExample.processedCycles,
	       api, (int)res);
}

/* Longest duration [us] per second: afe input, OneCycle, Waveform, 1012 */
enum { DBG_T_AFE, DBG_T_ONECYC, DBG_T_WF, DBG_T_1012, DBG_T_NUM };
static uint32_t dbgMaxUs[DBG_T_NUM];

/*
 * Debug time base: TMR5 free running at PeripheralClock / 64 (937.5 kHz;
 * the timer prescaler only divides by powers of two). The DWT
 * cycle counter does not run on this MAX32650, and no_os_get_time() stops
 * SysTick on every call, which drifts the system tick when called often.
 * Reading the counter is a single register read, safe in the AFE ISR.
 */
#define PQM_DEBUG_TIMER_ID	5

static struct no_os_timer_desc *pqmDebugTimer;

void pqm_debug_timer_init(void)
{
	struct no_os_timer_init_param ip = {
		.id = PQM_DEBUG_TIMER_ID,
		.freq_hz = PeripheralClock / 64,
		.ticks_count = 0xFFFFFFFF,
		.platform_ops = TIMER_OPS,
	};

	if (pqmDebugTimer)
		return;
	if (no_os_timer_init(&pqmDebugTimer, &ip) ||
	    no_os_timer_start(pqmDebugTimer)) {
		printf("[dbg] debug timer init failed\n\r");
		pqmDebugTimer = NULL;
	}
}

uint32_t pqm_debug_cycles(void)
{
	uint32_t ticks = 0;

	if (pqmDebugTimer)
		no_os_timer_counter_get(pqmDebugTimer, &ticks);

	return ticks;
}

uint32_t pqm_debug_us(uint32_t cycles)
{
	return (uint32_t)((uint64_t)cycles * 64 * 1000000 / PeripheralClock);
}

/**
 * @brief One-time check that the debug timer runs.
 */
static void event_debug_selftest(void)
{
	static bool done;
	uint32_t c0, c1;

	if (done)
		return;
	done = true;

	c0 = pqm_debug_cycles();
	no_os_udelay(1000);
	c1 = pqm_debug_cycles();
	printf("[dbg] SystemCoreClock=%" PRIu32 " timer 1ms=%" PRIu32
	       " us\n\r", SystemCoreClock, c1 - c0);
}

static uint32_t event_debug_now_us(void)
{
	return pqm_debug_cycles();
}

static void event_debug_time(int id, uint32_t startCycles)
{
	uint32_t d = pqm_debug_us(pqm_debug_cycles() - startCycles);

	if (d > dbgMaxUs[id])
		dbgMaxUs[id] = d;
}

/**
 * @brief Print loop and AFE input rates once per second. At 50 Hz expect
 * rmsone/onecyc = 50, 1012 = 5 and wf = waveform pages per second.
 * max[] = longest afe/onecycle/waveform/1012 call in that second [us].
 * @param loops - number of process_and_prepare_output() calls (updated)
 */
static void event_debug_rate(uint32_t *loops)
{
	static uint32_t lastCycles, lastRmsOne, lastOneCyc, lastWf, last1012;
	uint32_t nowCycles;
	uint32_t windowMs;

	event_debug_selftest();
	nowCycles = pqm_debug_cycles();
	windowMs = pqm_debug_us(nowCycles - lastCycles) / 1000;

	(*loops)++;
	if (windowMs < 1000)
		return;

	printf("[rate %" PRIu32 "ms] loop=%" PRIu32 " rmsone=%" PRIu32
	       " onecyc=%" PRIu32 " wf=%" PRIu32 " 1012=%" PRIu32
	       " max[us] afe=%" PRIu32 " onecyc=%" PRIu32 " wf=%" PRIu32
	       " 1012=%" PRIu32 " isr=%" PRIu32 "ms qhw=%" PRIu32
	       " ovf=%" PRIu32 "/%" PRIu32 "/%" PRIu32 " kick=%" PRIu32
	       " err=%" PRIu32 "\n\r",
	       windowMs, *loops,
	       pqlibExample.inputCycle.sequenceNumber - lastRmsOne,
	       pqlibExample.processedCycles - lastOneCyc,
	       pqlibExample.inputWaveform.sequenceNumber - lastWf,
	       pqlibExample.input1012Cycles.sequenceNumber - last1012,
	       dbgMaxUs[DBG_T_AFE], dbgMaxUs[DBG_T_ONECYC],
	       dbgMaxUs[DBG_T_WF], dbgMaxUs[DBG_T_1012], afeIsrBusyUs / 1000,
	       afeQueueStats.orderHighWater, afeQueueStats.overflow[AFE_Q_CYCLE],
	       afeQueueStats.overflow[AFE_Q_PAGE],
	       afeQueueStats.overflow[AFE_Q_1012], afeQueueStats.kicks,
	       afeQueueStats.errors);
	afeIsrBusyUs = 0;
	afeQueueStats.orderHighWater = 0;
	memset(dbgMaxUs, 0, sizeof(dbgMaxUs));

	lastCycles = nowCycles;
	lastRmsOne = pqlibExample.inputCycle.sequenceNumber;
	lastOneCyc = pqlibExample.processedCycles;
	lastWf = pqlibExample.inputWaveform.sequenceNumber;
	last1012 = pqlibExample.input1012Cycles.sequenceNumber;
	*loops = 0;
}

/**
 * @brief Print the raw event part of a PQLib output on the debug console.
 * @param output - PQLib output structure
 */
static void event_debug_output(const ADI_PQLIB_OUTPUT *output)
{
	const ADI_PQLIB_EVENTS *ev = &output->events;

	if (!ev->dipCount && !ev->swellCount && !ev->intrpCount && !ev->rvcCount)
		return;

	printf("[c%" PRIu32 "] out: dip=%u swell=%u intrp=%u rvc=%u flag=0x%" PRIx32
	       "\n\r", pqlibExample.processedCycles, ev->dipCount,
	       ev->swellCount, ev->intrpCount, ev->rvcCount,
	       (uint32_t)output->params1012Cycles.eventFlag);
	for (int i = 0; i < ADI_PQLIB_CFG_NUM_INTRP; i++)
		printf("    intrp[%d] start=%" PRId64 " end=%" PRId64
		       " cyc=%" PRIu32 "\n\r", i, ev->intrp[i].startTime,
		       ev->intrp[i].endTime, ev->intrp[i].durationInCycles);
	for (int i = 0; i < ADI_PQLIB_CFG_NUM_DIPS; i++)
		printf("    dip[%d] start=%" PRId64 " end=%" PRId64
		       " cyc=%" PRIu32 "\n\r", i, ev->dips[i].startTime,
		       ev->dips[i].endTime, ev->dips[i].durationInCycles);
}
#endif

/**
 * @brief Check that an event record is plausible. PQLib sometimes returns
 * records with a valid count but garbage content (e.g. endTime < startTime).
 * The start-to-end span must match durationInCycles within 10% + 2 cycles.
 * @param t - event timing fields
 * @return true if the record is plausible
 */
static bool event_timing_valid(const EVENT_TIMING *t)
{
	uint32_t freq = pqlibExample.exampleConfig.nominalFrequency;
	int64_t span, expected, tolerance;

	if (!freq)
		freq = 50;

	if (t->startTime < 0 || t->endTime <= t->startTime ||
	    t->durationInCycles == 0)
		return false;

	span = t->endTime - t->startTime;
	expected = (int64_t)t->durationInCycles * 1000 / freq;
	tolerance = expected / 10 + 2 * 1000 / freq;

	return (span >= expected - tolerance) && (span <= expected + tolerance);
}

/**
 * @brief Validate the records of one event type and count the new ones.
 * Nothing is updated when any record is implausible, so a garbage
 * startTime can never block later events through the dedup check.
 * @param first     - first event structure of the array
 * @param count     - number of reported events
 * @param stride    - size of one event structure
 * @param name      - event type name for the debug console
 * @param lastStart - newest start time already accumulated (updated)
 * @return number of new events, or -1 if the records were rejected
 */
static int count_new_events(const void *first, uint8_t count, size_t stride,
			    const char *name, ADI_PQLIB_TIME *lastStart)
{
	const uint8_t *event = first;
	const EVENT_TIMING *t;
	ADI_PQLIB_TIME newest = *lastStart;
	int newEvents = 0;

	for (uint8_t i = 0; i < count; i++, event += stride) {
		t = (const EVENT_TIMING *)event;
		if (!event_timing_valid(t)) {
			bogus_event_count++;
			if (bogus_event_count <= MAX_BOGUS_EVENT_PRINTS)
				printf("Bogus %s[%u]: start=%" PRId64
				       " end=%" PRId64 " cycles=%" PRIu32
				       " (ignored)\n\r", name, i, t->startTime,
				       t->endTime, t->durationInCycles);
			return -1;
		}
	}

	event = first;
	for (uint8_t i = 0; i < count; i++, event += stride) {
		t = (const EVENT_TIMING *)event;
		if (t->startTime > *lastStart) {
			newEvents++;
			if (t->startTime > newest)
				newest = t->startTime;
		}
	}
	*lastStart = newest;

	return newEvents;
}

/**
 * @brief Forget the dedup start times. Must be called whenever the PQLib
 * time base is cleared, otherwise new events would look older than the
 * last accumulated ones and be dropped.
 */
static void reset_event_dedup(void)
{
	lastDipStart = NO_EVENT_TIME;
	lastSwellStart = NO_EVENT_TIME;
	lastIntrpStart = NO_EVENT_TIME;
	lastRvcStart = NO_EVENT_TIME;
}

/**
 * @brief Copy new events from the PQLib output into the persistent record.
 * The previous record is kept when the output holds no new events.
 * @param output - PQLib output structure
 */
static void accumulate_events_from_output(const ADI_PQLIB_OUTPUT *output)
{
	const ADI_PQLIB_EVENTS *ev;
	int newEvents;

	if (!output)
		return;

	ev = &output->events;
	if (!ev->dipCount && !ev->swellCount &&
	    !ev->intrpCount && !ev->rvcCount)
		return;

	/* Do not reset or reconfigure PQLib here, it corrupts the handle */
	if (!events_counts_valid(ev)) {
		bogus_event_count++;
		return;
	}

	newEvents = count_new_events(&ev->dips[0], ev->dipCount,
				     sizeof(ev->dips[0]), "dip",
				     &lastDipStart);
	if (newEvents > 0) {
		memcpy(accumulated_events.dips, ev->dips, sizeof(ev->dips));
		accumulated_events.dipCount = ev->dipCount;
		total_dip_count += newEvents;
	}

	newEvents = count_new_events(&ev->swells[0], ev->swellCount,
				     sizeof(ev->swells[0]), "swell",
				     &lastSwellStart);
	if (newEvents > 0) {
		memcpy(accumulated_events.swells, ev->swells,
		       sizeof(ev->swells));
		accumulated_events.swellCount = ev->swellCount;
		total_swell_count += newEvents;
	}

	newEvents = count_new_events(&ev->intrp[0], ev->intrpCount,
				     sizeof(ev->intrp[0]), "intrp",
				     &lastIntrpStart);
	if (newEvents > 0) {
		memcpy(accumulated_events.intrp, ev->intrp, sizeof(ev->intrp));
		accumulated_events.intrpCount = ev->intrpCount;
		total_intrp_count += newEvents;
	}

	newEvents = count_new_events(&ev->rvc[0], ev->rvcCount,
				     sizeof(ev->rvc[0]), "rvc",
				     &lastRvcStart);
	if (newEvents > 0) {
		memcpy(accumulated_events.rvc, ev->rvc, sizeof(ev->rvc));
		accumulated_events.rvcCount = ev->rvcCount;
		total_rvc_count += newEvents;
	}
}

void pqm_clear_events(void)
{
	memset(&accumulated_events, 0, sizeof(accumulated_events));
	total_dip_count = 0;
	total_swell_count = 0;
	total_intrp_count = 0;
	total_rvc_count = 0;
	bogus_event_count = 0;
	reset_event_dedup();
}

int pqm_measurement_init(void)
{
	int status = 0;

	pqlibExample.output = &emptyOutput;
#ifdef PQM_EVENT_DEBUG
	pqm_debug_timer_init();
#endif
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

	afe_input_stop();
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

	if (status == 0)
		afe_input_start();

	return status;
}

int pqm_one_cycle(void)
{
	/* Check for calibration request first */
	if (pqlibExample.calibrationRequested || calibration_is_active()) {
		if (pqlibExample.state != PQLIB_STATE_CALIBRATING) {
			pqlibExample.state = PQLIB_STATE_CALIBRATING;
			afe_input_stop();
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

static int process_one_input(void)
{
	int status = 0;
	ADI_PQLIB_RESULT pqlibStatus = ADI_PQLIB_RESULT_SUCCESS;
	ADI_PQLIB_CYCLE_INPUT *pCycleInput = &pqlibExample.inputCycle;
	ADI_PQLIB_1012_CYCLE_INPUT *p1012CyclesInput = &pqlibExample.input1012Cycles;
	ADI_PQLIB_WAVEFORM *pWaveform = &pqlibExample.inputWaveform;
	ADI_PQLIB_OUTPUT_STATUS outputStatus;
	ADI_PQLIB_HANDLE hDevice = pqlibExample.hDevice;
	bool done = false;
#ifdef PQM_EVENT_DEBUG
	static ADI_PQLIB_RESULT dbgLastOneCycle, dbgLastWaveform, dbg1012;
	static uint32_t dbgLoops;
	uint32_t dbgT0;

	event_debug_rate(&dbgLoops);
	dbgT0 = event_debug_now_us();
#endif

	status = get_afe_input();
#ifdef PQM_EVENT_DEBUG
	event_debug_time(DBG_T_AFE, dbgT0);
#endif

	while ((done == false) && (status == SYS_STATUS_SUCCESS)) {
		if (pCycleInput->isDataProcessed == false) {
#ifdef PQM_EVENT_DEBUG
			dbgT0 = event_debug_now_us();
#endif
			ADI_PQLIB_PROFBEG(ADI_PQLIB_PROFILE_ID_ONECYCLE);
			pqlibStatus =
				adi_pqlib_ProcessOneCycle(hDevice, &pqlibExample.inputCycle);
			ADI_PQLIB_PROFEND(ADI_PQLIB_PROFILE_ID_ONECYCLE);
#ifdef PQM_EVENT_DEBUG
			event_debug_time(DBG_T_ONECYC, dbgT0);
#endif
#ifdef PQM_EVENT_DEBUG
			event_debug_result("OneCycle", pqlibStatus, &dbgLastOneCycle);
#endif
			status = wait_for_zero_crossing(&pqlibExample, pqlibStatus);

			if (status != SYS_STATUS_SUCCESS) {
				break;
			}
			SyncLibTime(&pqlibExample, true);
			SyncToDip(&pqlibExample, pqlibStatus);
			pqlibExample.processedCycles++;

			/* No GetOutput here: calling it every cycle resets
			 * the PQLib dip state machine, DIP_ENDED never fires */
			status = process_pqlib_error(&pqlibExample, pqlibStatus);
			if (status != SYS_STATUS_SUCCESS) {
				break;
			}
		}

		if (pqlibExample.state == PQLIB_STATE_WAITING_FOR_TRIGGER) {
			status = SYS_STATUS_PQLIB_RUNNING;
			break;
		}

		if (pWaveform->isDataProcessed == false) {
#ifdef PQM_EVENT_DEBUG
			dbgT0 = event_debug_now_us();
#endif
			ADI_PQLIB_PROFBEG(ADI_PQLIB_PROFILE_ID_WAVEFORM);
			pqlibStatus =
				adi_pqlib_ProcessWaveform(hDevice, &pqlibExample.inputWaveform);
			ADI_PQLIB_PROFEND(ADI_PQLIB_PROFILE_ID_WAVEFORM);
#ifdef PQM_EVENT_DEBUG
			event_debug_time(DBG_T_WF, dbgT0);
#endif
#ifdef PQM_EVENT_DEBUG
			event_debug_result("Waveform", pqlibStatus, &dbgLastWaveform);
#endif
			status = process_pqlib_error(&pqlibExample, pqlibStatus);
			if (status != SYS_STATUS_SUCCESS) {
				break;
			}
		}

		if (p1012CyclesInput->isDataProcessed == 0 && processData) {
#ifdef PQM_EVENT_DEBUG
			dbgT0 = event_debug_now_us();
#endif
			ADI_PQLIB_PROFBEG(ADI_PQLIB_PROFILE_ID_1012CYCLES);
			pqlibStatus =
				adi_pqlib_Process1012Cycles(hDevice, &pqlibExample.input1012Cycles);
			ADI_PQLIB_PROFEND(ADI_PQLIB_PROFILE_ID_1012CYCLES);
#ifdef PQM_EVENT_DEBUG
			event_debug_time(DBG_T_1012, dbgT0);
#endif
#ifdef PQM_EVENT_DEBUG
			event_debug_result("1012", pqlibStatus, &dbg1012);
#endif
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
			accumulate_events_from_output(pqlibExample.output);
#ifdef PQM_EVENT_DEBUG
			event_debug_output(pqlibExample.output);
#endif
#ifdef UART_EXPORT_ENABLED
			uart_export_send();
#endif
		}

		status = SYS_STATUS_PQLIB_RUNNING;
		done = true;
	}

	return status;
}

/* Max queued AFE items handled per call, so IIO is served in between */
#define MAX_INPUTS_PER_CALL	8

int process_and_prepare_output()
{
	int status = 0;

	for (int i = 0; i < MAX_INPUTS_PER_CALL; i++) {
		status = process_one_input();
		if (!afe_input_pending())
			break;
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
				/* Clear the time base so the start-up voltage
				 * ramp does not leave a dip active forever */
				bool clearTime = true;
				pqlibExample.state = PQLIB_STATE_RUNNING;
				adi_pqlib_Reset(pqlibExample.hDevice, clearTime);
				reset_event_dedup();
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
	pConfig->calNominalCurrent = 10.0f;     /* 10 Arms for gain cal */
	pConfig->calNominalVoltage = 230.0f;    /* 230 Vrms for gain cal */
	pConfig->calOffsetCurrent = 1.0f;       /* 1.0 Arms for offset cal */
	pConfig->calOffsetVoltage = 10.0f;      /* 10 Vrms for offset cal */
	pConfig->currentPgaGain = 1.0f;         /* Default PGA gain */
	pConfig->voltagePgaGain = 1.0f;         /* Default PGA gain */
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
