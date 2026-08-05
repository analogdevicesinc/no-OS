/*******************************************************************************
 *   @file   afe_calibration.h
 *   @brief  ADE9430 calibration header file
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __AFE_CALIBRATION_H__
#define __AFE_CALIBRATION_H__

#include <stdint.h>
#include <stdbool.h>

/* Calibration constants */
#define ADI_PQLIB_MAX_RMS_VALUE_CAL     52702092
#define CALIBRATION_CYCLES_50HZ         150
#define CALIBRATION_CYCLES_60HZ         180
#define SQRT_2                          1.41421356f
#define GAIN_SCALE_FACTOR               (1LL << 27)  /* 2^27 */
#define OFFSET_SCALE_FACTOR             (1LL << 15)  /* 2^15 */

/* 24-bit signed register limits for offset registers */
#define OFFSET_REG_MAX                  ((1L << 23) - 1)   /* +8388607 */
#define OFFSET_REG_MIN                  (-(1L << 23))      /* -8388608 */

/**
 * @brief Calibration state machine states
 */
typedef enum {
	CALIBRATION_STATE_IDLE,
	CALIBRATION_STATE_INIT,
	CALIBRATION_STATE_COLLECTING,
	CALIBRATION_STATE_CALCULATING,
	CALIBRATION_STATE_CALCULATE_ERROR_BEFORE,
	CALIBRATION_STATE_WRITING,
	CALIBRATION_STATE_VERIFY_COLLECTING,
	CALIBRATION_STATE_VERIFY_CALCULATING,
	CALIBRATION_STATE_DONE,
	CALIBRATION_STATE_ERROR
} CALIBRATION_STATE;

/**
 * @brief Calibration type (gain or offset)
 */
typedef enum {
	CALIBRATION_TYPE_GAIN,
	CALIBRATION_TYPE_OFFSET
} CALIBRATION_TYPE;

/**
 * @brief Phase selection for calibration
 */
typedef enum {
	CALIBRATION_PHASE_A = 0,
	CALIBRATION_PHASE_B = 1,
	CALIBRATION_PHASE_C = 2
} CALIBRATION_PHASE;

/**
 * @brief Calibration input parameters (user-provided values)
 */
typedef struct {
	float nominal_current;      /* Arms - for gain calibration */
	float nominal_voltage;      /* Vrms - for gain calibration */
	float offset_current;       /* Arms - for offset calibration */
	float offset_voltage;       /* Vrms - for offset calibration */
	float current_pga_gain;     /* PGA gain for current (default 1) */
	float voltage_pga_gain;     /* PGA gain for voltage (default 1) */
	float current_scale;        /* Current scale factor */
	float voltage_scale;        /* Voltage scale factor */
	int32_t i_gain_init;             /* Initial current gain coefficient */
	int32_t v_gain_init;             /* Initial voltage gain coefficient */
	int32_t i_rmsos_init;            /* Initial current RMS offset */
	int32_t v_rmsos_init;            /* Initial voltage RMS offset */
	int32_t if_rmsos_init;           /* Initial current fundamental RMS offset */
	int32_t vf_rmsos_init;           /* Initial voltage fundamental RMS offset */
} CALIBRATION_INPUT;

/**
 * @brief Calibration results (computed values)
 */
typedef struct {
	int32_t i_gain;             /* Computed current gain coefficient */
	int32_t v_gain;             /* Computed voltage gain coefficient */
	int32_t i_rmsos;            /* Computed current RMS offset */
	int32_t v_rmsos;            /* Computed voltage RMS offset */
	int32_t if_rmsos;           /* Computed current fundamental RMS offset */
	int32_t vf_rmsos;           /* Computed voltage fundamental RMS offset */
	/* Gain calibration errors (persistent) */
	float gain_i_error_before;  /* Gain cal: current error before */
	float gain_v_error_before;  /* Gain cal: voltage error before */
	float gain_i_error_after;   /* Gain cal: current error after */
	float gain_v_error_after;   /* Gain cal: voltage error after */
	/* Offset calibration errors (persistent) */
	float offset_i_error_before; /* Offset cal: current error before */
	float offset_v_error_before; /* Offset cal: voltage error before */
	float offset_i_error_after;  /* Offset cal: current error after */
	float offset_v_error_after;  /* Offset cal: voltage error after */
	bool done;                  /* Calibration complete flag */
} CALIBRATION_RESULT;

/**
 * @brief Calibration state structure (runtime state)
 */
typedef struct {
	CALIBRATION_STATE state;
	CALIBRATION_TYPE type;
	CALIBRATION_PHASE phase;
	uint16_t cycle_count;
	uint16_t target_cycles;
	int64_t irms_sum;
	int64_t irms_sum_no_gain;
	int64_t irms_sum_no_off;
	int64_t vrms_sum;
	int64_t vrms_sum_no_gain;
	int64_t vrms_sum_no_off;
	int64_t ifrms_sum;
	int64_t ifrms_sum_no_off;
	int64_t vfrms_sum;
	int64_t vfrms_sum_no_off;
	uint32_t irms_expected;
	uint32_t vrms_expected;
	uint32_t irms_measured;
	uint32_t irms_measured_no_gain;
	uint32_t irms_measured_no_off;
	uint32_t vrms_measured;
	uint32_t vrms_measured_no_gain;
	uint32_t vrms_measured_no_off;
	uint32_t ifrms_measured;
	uint32_t ifrms_measured_no_off;
	uint32_t vfrms_measured;
	uint32_t vfrms_measured_no_off;
	CALIBRATION_INPUT input;
	CALIBRATION_RESULT result;
} CALIBRATION_CONTEXT;

/* Global calibration context */
extern CALIBRATION_CONTEXT calibrationCtx;

/**
 * @brief Initialize calibration context with default values
 */
void calibration_init(void);

/**
 * @brief Start calibration process
 * @param type - Calibration type (gain or offset)
 * @param phase - Phase to calibrate (A, B, or C)
 * @return 0 on success, error code otherwise
 */
int calibration_start(CALIBRATION_TYPE type, CALIBRATION_PHASE phase);

/**
 * @brief Process one calibration cycle (called from main loop)
 * @return 0 on success, error code otherwise
 */
int calibration_process_cycle(void);

/**
 * @brief Check if calibration is active
 * @return true if calibration is in progress
 */
bool calibration_is_active(void);

/**
 * @brief Check if calibration just completed
 * @return true if calibration was just done (result.done is true)
 */
bool calibration_is_done(void);

/**
 * @brief Clear calibration done flag
 */
void calibration_clear_done(void);

/**
 * @brief Get current calibration status string
 * @return Status string ("idle", "in_progress", "done", "error")
 */
const char *calibration_get_status_string(void);

/**
 * @brief Calculate expected RMS value for current
 * @param input - Calibration input parameters
 * @param rms_value - RMS value in Arms
 * @return Expected register value
 */
uint32_t calculate_expected_irms(const CALIBRATION_INPUT *input,
				 float rms_value);

/**
 * @brief Calculate expected RMS value for voltage
 * @param input - Calibration input parameters
 * @param rms_value - RMS value in Vrms
 * @return Expected register value
 */
uint32_t calculate_expected_vrms(const CALIBRATION_INPUT *input,
				 float rms_value);

/**
 * @brief Calculate gain coefficient
 * @param expected - Expected RMS value
 * @param measured - Measured RMS value
 * @return Gain coefficient (signed 32-bit)
 */
int32_t calculate_gain(int32_t expected, int32_t measured);

/**
 * @brief Calculate RMS without gain coefficient
 * @param measured - Measured RMS value
 * @param gain - Gain register value
 * @return rms value without gain (signed 32-bit)
 */
int32_t calculate_rms_without_gain(int32_t measured, int32_t gain);

/**
 * @brief Calculate RMS offset coefficient
 * @param expected - Expected RMS value
 * @param measured - Measured RMS value
 * @return Offset coefficient (signed 32-bit)
 */
int32_t calculate_rmsos(int32_t expected, int32_t measured);

/**
 * @brief Calculate RMS value without offset coefficient
 * @param measured - Measured RMS value
 * @param xrmsos - RMSOS register value
 * @return rms value without offset (signed 32-bit)
 */
int32_t calculate_rms_without_rmsos(int32_t measured, int32_t xrmsos);


/**
 * @brief Calculate error percentage
 * @param measured - Measured value
 * @param expected - Expected value
 * @return Error as percentage (e.g., 0.01 = 1%)
 */
float calculate_error(int32_t measured, int32_t expected);

#endif /* __AFE_CALIBRATION_H__ */
