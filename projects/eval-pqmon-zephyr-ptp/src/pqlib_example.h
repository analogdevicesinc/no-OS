/*******************************************************************************
 *   @file   pqlib_example.h
 *   @brief  Header file for pqlib display example functions
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

#ifndef __PQLIB_EXAMPLE_H__
#define __PQLIB_EXAMPLE_H__

#include "ade9430.h"
#include "adi_pqlib.h"
#include "adi_pqlib_memory.h"
#include "adi_pqlib_profile.h"
#include "pqlib_convert.h"
#include "afe_calibration.h"
#include "status.h"
#include <stddef.h>
#include <stdbool.h>

#define POWER_ENERGY_NUM_PHASES 3
#define PQLIB_MAX_CHANNELS 7
#define ADI_PQLIB_NUM_WAVEFORM_BLOCKS 4
#define PQLIB_MAX_HARMONICS 50
#define PQLIB_MAX_INTER_HARMONICS 50
#define SIZE_OF_INPUT_RTC 19

/**
 * @brief PQLIB Example states
 */
typedef enum {
	PQLIB_STATE_WAITING_FOR_START_CMD,
	PQLIB_STATE_WAITING_FOR_TRIGGER,
	PQLIB_STATE_RUNNING,
	PQLIB_STATE_CALIBRATING
} PQLIB_STATE;

/**
 * @brief VCONSEL H/W config selection
 */
typedef enum {
	VCONSEL_4W_WYE,
	VCONSEL_3W_DELTA_VB_VA_NEGVC,
	VCONSEL_4W_WYE_VB_NEGVA_NEGVC,
	VCONSEL_4W_DELTA_VB_NEGVA,
	VCONSEL_4W_DELTA_VA_VB_VC,
} VCONSEL_CONFIG;

/**
 * @brief Power and energy data read from ADE9430
 */
typedef struct power_energy_data {
	int32_t activePower[POWER_ENERGY_NUM_PHASES];
	int32_t activeEnergyHi[POWER_ENERGY_NUM_PHASES];
	int32_t reactiveEnergyHi[POWER_ENERGY_NUM_PHASES];
	int32_t fundActivePower[POWER_ENERGY_NUM_PHASES];
	int32_t fundActiveEnergyHi[POWER_ENERGY_NUM_PHASES];
	int32_t fundReactiveEnergyHi[POWER_ENERGY_NUM_PHASES];
} POWER_ENERGY_DATA;

/**
 * Structure for pqlib example configurations.
 */
typedef struct {
	bool enableRTCSync;
	bool enableIconsel;
	bool useExternalTimestamp;
	uint8_t harmonics[PQLIB_MAX_HARMONICS];
	uint8_t interHarmonics[PQLIB_MAX_INTER_HARMONICS];
	uint8_t maxHarmonics;
	uint8_t maxInterHarmonics;
	uint8_t rtcBuff[SIZE_OF_INPUT_RTC];
	uint16_t currentCorrCoef[PQLIB_MAX_HARMONICS];
	uint16_t voltageCorrCoef[PQLIB_MAX_HARMONICS];
	uint16_t numCorrCoefCurrent50Hz;
	uint16_t numCorrCoefVoltage50Hz;
	uint16_t numCorrCoefCurrent60Hz;
	uint16_t numCorrCoefVoltage60Hz;
	uint32_t cyclesToRun;
	uint32_t nominalFrequency;
	uint32_t correction;
	uint32_t zeroCrossingCheck;
	uint64_t syncTime;
	float dipThreshold;
	float dipHysteresis;
	float swellThreshold;
	float swellHysteresis;
	float intrpThreshold;
	float intrpHysteresis;
	float rvcThreshold;
	float rvcHysteresis;
	float msvThreshold;
	float msvCarrierFrequency;
	float msvRecordingLength;
	float voltageScale;
	float currentScale;
	float nominalVoltage;
	float inputCurrentCorrCoef50Hz[PQLIB_MAX_HARMONICS];
	float inputVoltageCorrCoef50Hz[PQLIB_MAX_HARMONICS];
	float inputCurrentCorrCoef60Hz[PQLIB_MAX_HARMONICS];
	float inputVoltageCorrCoef60Hz[PQLIB_MAX_HARMONICS];
	char *pEnabledChannels[PQLIB_MAX_CHANNELS];
	ADI_PQLIB_REF_CHANNEL refChannel;
	ADI_PQLIB_CORR_COEF correctionData;
	ADI_PQLIB_FLICKER_MODEL flickerModel;
	ADI_PQLIB_PHASE_MAP phaseMap;
	VCONSEL_CONFIG vconsel;
	CALIBRATION_TYPE calibrationType; /* Gain or Offset calibration */
	float calNominalCurrent;        /* Arms for gain calibration */
	float calNominalVoltage;        /* Vrms for gain calibration */
	float calOffsetCurrent;         /* Arms for offset calibration */
	float calOffsetVoltage;         /* Vrms for offset calibration */
	float currentPgaGain;           /* PGA gain for current (default 1) */
	float voltagePgaGain;           /* PGA gain for voltage (default 1) */
} EXAMPLE_CONFIG;

typedef struct {
	bool readyToDisplay;
	bool waitingForSync;
	bool calibration;
	bool calibrationRequested;
	float calExpectedRms;
	char *pChannel;
	uint16_t syncCycles;
	uint32_t processedCycles;
	uint32_t zeroCrossingCount;
	uint32_t timeOutCount;
	uint32_t pqlibStateMemory[ADI_PQLIB_STATE_MEM_NUM_BYTES / 4];
	uint32_t pqlibTempMemory[ADI_PQLIB_TEMP_MEM_NUM_BYTES / 4];
	ADI_PQLIB_HANDLE hDevice;
	ADI_PQLIB_CONFIG config;
	ADI_PQLIB_CYCLE_INPUT inputCycle;
	ADI_PQLIB_1012_CYCLE_INPUT input1012Cycles;
	ADI_PQLIB_WAVEFORM inputWaveform;
	EXAMPLE_CONFIG exampleConfig;
	ADI_PQLIB_OUTPUT *output;
	ADI_PQLIB_PHASE_MAP channelMap;
	PQLIB_STATE state;
	POWER_ENERGY_DATA powerEnergy;
} PQLIB_EXAMPLE;

int pqm_one_cycle(void);
int process_pqlib_error(PQLIB_EXAMPLE *pExample,
			ADI_PQLIB_RESULT pqlibStatus);
int open_pqlib(PQLIB_EXAMPLE *pExample);
int SyncLibTime(PQLIB_EXAMPLE *pExample, bool checkRtcTime);
int config_measurement(PQLIB_EXAMPLE *pExample);
void set_default_config(EXAMPLE_CONFIG *pConfig);
void populate_event_config(EXAMPLE_CONFIG *pExampleConfig);
int cpy_correction_coeffs(ADI_PQLIB_NOMINAL_FREQUENCY nominalFrequency,
			  ADI_PQLIB_CORR_COEF *pCorrCoef);
int wait_for_zero_crossing(PQLIB_EXAMPLE *pExample,
			   ADI_PQLIB_RESULT pqlibStatus);
void SyncToDip(PQLIB_EXAMPLE *pExample, ADI_PQLIB_RESULT pqlibStatus);

extern PQLIB_EXAMPLE pqlibExample;
int pqm_measurement_init(void);

/* Persistent event accumulator — survives PQLib GetOutput() clears */
extern ADI_PQLIB_EVENTS accumulated_events;

/* Lifetime per-type event totals (never reset except by power cycle) */
extern uint32_t total_dip_count;
extern uint32_t total_swell_count;
extern uint32_t total_intrp_count;
extern uint32_t total_rvc_count;
void pqlib_accumulate_events(const ADI_PQLIB_OUTPUT *output,
			     ADI_PQLIB_RESULT cycleStatus);
void pqlib_clear_accumulated_events(void);
void pqlib_enable_event_accumulation(void);
void pqlib_log_event_stats(void);
int pqm_start_measurement(bool waitingForSync);
int process_and_prepare_output(void);
int pqm_afe_thread_start(void);

#endif /* __PQLIB_EXAMPLE_H__ */
