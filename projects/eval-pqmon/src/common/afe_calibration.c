/*******************************************************************************
 *   @file   afe_calibration.c
 *   @brief  ADE9430 calibration implementation
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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

#include "afe_calibration.h"
#include "afe_config.h"
#include "ade9430.h"
#include "status.h"
#include "pqlib_example.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Global calibration context */
CALIBRATION_CONTEXT calibrationCtx;

/* Status1 ZX bit masks for each phase */
static const uint32_t zx_masks[] = {
	BITM_STATUS1_ZXVA,  /* Phase A */
	BITM_STATUS1_ZXVB,  /* Phase B */
	BITM_STATUS1_ZXVC   /* Phase C */
};

/* RMS register addresses for each phase */
static const uint16_t irms_regs[] = {
	REG_AIRMS,
	REG_BIRMS,
	REG_CIRMS
};

static const uint16_t vrms_regs[] = {
	REG_AVRMS,
	REG_BVRMS,
	REG_CVRMS
};

static const uint16_t ifrms_regs[] = {
	REG_AIFRMS,
	REG_BIFRMS,
	REG_CIFRMS
};

static const uint16_t vfrms_regs[] = {
	REG_AVFRMS,
	REG_BVFRMS,
	REG_CVFRMS
};

/* Gain register addresses for each phase */
static const uint16_t igain_regs[] = {
	REG_AIGAIN,
	REG_BIGAIN,
	REG_CIGAIN
};

static const uint16_t vgain_regs[] = {
	REG_AVGAIN,
	REG_BVGAIN,
	REG_CVGAIN
};

/* Offset register addresses for each phase */
static const uint16_t irmsos_regs[] = {
	REG_AIRMSOS,
	REG_BIRMSOS,
	REG_CIRMSOS
};

static const uint16_t vrmsos_regs[] = {
	REG_AVRMSOS,
	REG_BVRMSOS,
	REG_CVRMSOS
};

static const uint16_t ifrmsos_regs[] = {
	REG_AIFRMSOS,
	REG_BIFRMSOS,
	REG_CIFRMSOS
};

static const uint16_t vfrmsos_regs[] = {
	REG_AVFRMSOS,
	REG_BVFRMSOS,
	REG_CVFRMSOS
};

void calibration_init(void)
{
	memset(&calibrationCtx, 0, sizeof(CALIBRATION_CONTEXT));
	calibrationCtx.state = CALIBRATION_STATE_IDLE;
	calibrationCtx.result.done = false;

	/* Set default calibration input values (for initial testing) */
	calibrationCtx.input.nominal_current = 10.0f;   /* 10 Arms */
	calibrationCtx.input.nominal_voltage = 230.0f;  /* 230 Vrms */
	calibrationCtx.input.offset_current = 1.0f;     /* 1 Arms */
	calibrationCtx.input.offset_voltage = 10.0f;    /* 10 Vrms */
	calibrationCtx.input.current_pga_gain = 1.0f;
	calibrationCtx.input.voltage_pga_gain = 1.0f;
}

uint32_t calculate_expected_irms(const CALIBRATION_INPUT *input,
				 float rms_value)
{
	if (input->current_scale == 0.0f)
		return 0;

	float percentage = (rms_value * SQRT_2 * input->current_pga_gain) /
			   input->current_scale;
	return (uint32_t)(percentage * (float)ADI_PQLIB_MAX_RMS_VALUE_CAL);
}

uint32_t calculate_expected_vrms(const CALIBRATION_INPUT *input,
				 float rms_value)
{
	if (input->voltage_scale == 0.0f)
		return 0;

	float percentage = (rms_value * SQRT_2 * input->voltage_pga_gain) /
			   input->voltage_scale;
	return (uint32_t)(percentage * (float)ADI_PQLIB_MAX_RMS_VALUE_CAL);
}

int32_t calculate_gain(int32_t expected, int32_t measured)
{
	int64_t diff, gain;

	if (measured == 0)
		return 0;

	diff = (int64_t)expected - (int64_t)measured;
	gain = (diff * GAIN_SCALE_FACTOR) / (int64_t)measured;

	return (int32_t)gain;
}

int32_t calculate_rms_without_gain(int32_t measured, int32_t gain)
{
	int64_t rms, num, numi;

	if (measured == 0)
		return 0;

	num = (int64_t)measured * GAIN_SCALE_FACTOR;
	numi = (int64_t)gain + GAIN_SCALE_FACTOR;
	rms = num / numi;

	return (int32_t)rms;
}

int32_t calculate_rmsos(int32_t expected, int32_t measured)
{
	int64_t offset, expected_sq, measured_sq;

	expected_sq = (int64_t)expected * (int64_t)expected;
	measured_sq = (int64_t)measured * (int64_t)measured;
	offset = (expected_sq - measured_sq) / OFFSET_SCALE_FACTOR;

	/* Clamp to 24-bit signed register range */
	if (offset > OFFSET_REG_MAX)
		offset = OFFSET_REG_MAX;
	else if (offset < OFFSET_REG_MIN)
		offset = OFFSET_REG_MIN;

	return (int32_t)offset;
}

int32_t calculate_rms_without_rmsos(int32_t measured, int32_t xrmsos)
{
	int64_t rms, measured_sq, xrmsos_int;

	measured_sq = (int64_t)measured * (int64_t)measured;
	xrmsos_int = (int64_t)xrmsos * OFFSET_SCALE_FACTOR;
	if (xrmsos_int >= measured_sq)
		return 0;
	rms = sqrtl(measured_sq - xrmsos_int);

	/* Clamp to 24-bit signed register range */
	if (rms > OFFSET_REG_MAX)
		rms = OFFSET_REG_MAX;
	else if (rms < OFFSET_REG_MIN)
		rms = OFFSET_REG_MIN;

	return (int32_t)rms;
}

float calculate_error(int32_t measured, int32_t expected)
{
	if (expected == 0)
		return 0.0f;

	return ((float)measured - (float)expected) / (float)expected;
}

int calibration_start(CALIBRATION_TYPE type, CALIBRATION_PHASE phase)
{
	if (calibrationCtx.state != CALIBRATION_STATE_IDLE &&
	    calibrationCtx.state != CALIBRATION_STATE_DONE &&
	    calibrationCtx.state != CALIBRATION_STATE_ERROR)
		/* Calibration already in progress */
		return -EBUSY;


	/* Get current config values */
	calibrationCtx.input.current_scale = pqlibExample.exampleConfig.currentScale;
	calibrationCtx.input.voltage_scale = pqlibExample.exampleConfig.voltageScale;

	/* Determine target cycles based on frequency */
	if (pqlibExample.exampleConfig.nominalFrequency == 50) {
		calibrationCtx.target_cycles = CALIBRATION_CYCLES_50HZ;
	} else {
		calibrationCtx.target_cycles = CALIBRATION_CYCLES_60HZ;
	}

	calibrationCtx.type = type;
	calibrationCtx.phase = phase;
	calibrationCtx.cycle_count = 0;
	calibrationCtx.irms_sum = 0;
	calibrationCtx.vrms_sum = 0;
	calibrationCtx.ifrms_sum = 0;
	calibrationCtx.vfrms_sum = 0;
	calibrationCtx.result.done = false;

	calibrationCtx.state = CALIBRATION_STATE_INIT;

	return 0;
}

static int calibration_read_rms_values(void)
{
	int status;
	uint8_t phase = calibrationCtx.phase;
	CALIBRATION_TYPE type = calibrationCtx.type;
	uint32_t irms, vrms, ifrms, vfrms;

	/* Read total RMS values */
	status = afe_read_32bit_buff(irms_regs[phase], 1, &irms);
	if (status != 0)
		return status;

	status = afe_read_32bit_buff(vrms_regs[phase], 1, &vrms);
	if (status != 0)
		return status;

	/* Read fundamental RMS values (needed for offset calibration) */
	status = afe_read_32bit_buff(ifrms_regs[phase], 1, &ifrms);
	if (status != 0)
		return status;

	status = afe_read_32bit_buff(vfrms_regs[phase], 1, &vfrms);
	if (status != 0)
		return status;

	if (type == CALIBRATION_TYPE_GAIN) {
		/* For gain calibration, remove gain */
		calibrationCtx.irms_sum_no_gain += calculate_rms_without_gain(irms,
						   calibrationCtx.input.i_gain_init);
		calibrationCtx.irms_sum += irms;
		calibrationCtx.vrms_sum_no_gain += calculate_rms_without_gain(vrms,
						   calibrationCtx.input.v_gain_init);
		calibrationCtx.vrms_sum += vrms;
	} else {
		/* For offset calibration, remove RMS offset */
		calibrationCtx.irms_sum_no_off += calculate_rms_without_rmsos(irms,
						  calibrationCtx.input.i_rmsos_init);
		calibrationCtx.irms_sum += irms;
		calibrationCtx.vrms_sum_no_off += calculate_rms_without_rmsos(vrms,
						  calibrationCtx.input.v_rmsos_init);
		calibrationCtx.vrms_sum += vrms;
		calibrationCtx.ifrms_sum_no_off += calculate_rms_without_rmsos(ifrms,
						   calibrationCtx.input.if_rmsos_init);
		calibrationCtx.ifrms_sum += ifrms;
		calibrationCtx.vfrms_sum_no_off += calculate_rms_without_rmsos(vfrms,
						   calibrationCtx.input.vf_rmsos_init);
		calibrationCtx.vfrms_sum += vfrms;
	}
	calibrationCtx.cycle_count++;

	return 0;
}

static int calibration_check_zero_crossing(bool *zx_detected)
{
	uint32_t status1;
	int status;

	*zx_detected = false;

	/* Use phase-aware status read that clears the correct ZX bit */
	status = afe_read_status_1_clear_zx(&status1, calibrationCtx.phase);
	if (status != 0)
		return status;

	if (status1 & zx_masks[calibrationCtx.phase]) {
		*zx_detected = true;
	}

	return 0;
}

static void calibration_calculate_means(void)
{
	if (calibrationCtx.cycle_count == 0)
		return;

	calibrationCtx.irms_measured = (uint32_t)(calibrationCtx.irms_sum /
				       calibrationCtx.cycle_count);
	calibrationCtx.irms_measured_no_gain = (uint32_t)(
			calibrationCtx.irms_sum_no_gain /
			calibrationCtx.cycle_count);
	calibrationCtx.irms_measured_no_off = (uint32_t)(
			calibrationCtx.irms_sum_no_off /
			calibrationCtx.cycle_count);
	calibrationCtx.vrms_measured = (uint32_t)(calibrationCtx.vrms_sum /
				       calibrationCtx.cycle_count);
	calibrationCtx.vrms_measured_no_gain = (uint32_t)(
			calibrationCtx.vrms_sum_no_gain /
			calibrationCtx.cycle_count);
	calibrationCtx.vrms_measured_no_off = (uint32_t)(
			calibrationCtx.vrms_sum_no_off /
			calibrationCtx.cycle_count);
	calibrationCtx.ifrms_measured = (uint32_t)(calibrationCtx.ifrms_sum /
					calibrationCtx.cycle_count);
	calibrationCtx.ifrms_measured_no_off = (uint32_t)(
			calibrationCtx.ifrms_sum_no_off /
			calibrationCtx.cycle_count);
	calibrationCtx.vfrms_measured = (uint32_t)(calibrationCtx.vfrms_sum /
					calibrationCtx.cycle_count);
	calibrationCtx.vfrms_measured_no_off = (uint32_t)(
			calibrationCtx.vfrms_sum_no_off /
			calibrationCtx.cycle_count);
}

static int calibration_write_gain_registers(void)
{
	int status;
	uint8_t phase = calibrationCtx.phase;
	uint32_t igain_copy, vgain_copy;

	/* Calculate expected values for gain calibration */
	calibrationCtx.irms_expected = calculate_expected_irms(
					       &calibrationCtx.input,
					       calibrationCtx.input.nominal_current);
	calibrationCtx.vrms_expected = calculate_expected_vrms(
					       &calibrationCtx.input,
					       calibrationCtx.input.nominal_voltage);

	/* Calculate gain coefficients */
	calibrationCtx.result.i_gain = calculate_gain(
					       calibrationCtx.irms_expected,
					       calibrationCtx.irms_measured_no_gain);
	calibrationCtx.result.v_gain = calculate_gain(
					       calibrationCtx.vrms_expected,
					       calibrationCtx.vrms_measured_no_gain);

	igain_copy = (uint32_t)calibrationCtx.result.i_gain;
	vgain_copy = (uint32_t)calibrationCtx.result.v_gain;

	status = afe_write_32bit_reg(igain_regs[phase], &igain_copy);
	if (status != 0)
		return status;

	status = afe_write_32bit_reg(vgain_regs[phase], &vgain_copy);
	if (status != 0)
		return status;

	return 0;
}

static int calibration_write_offset_registers(void)
{
	int status;
	uint8_t phase = calibrationCtx.phase;
	uint32_t irmsos_copy, vrmsos_copy;
	uint32_t ifrmsos_copy, vfrmsos_copy;

	/* Calculate RMS offset coefficients */
	calibrationCtx.result.i_rmsos = calculate_rmsos(
						calibrationCtx.irms_expected,
						calibrationCtx.irms_measured_no_off);
	calibrationCtx.result.v_rmsos = calculate_rmsos(
						calibrationCtx.vrms_expected,
						calibrationCtx.vrms_measured_no_off);

	/* Calculate fundamental RMS offset coefficients */
	calibrationCtx.result.if_rmsos = calculate_rmsos(
			calibrationCtx.irms_expected,
			calibrationCtx.ifrms_measured_no_off);
	calibrationCtx.result.vf_rmsos = calculate_rmsos(
			calibrationCtx.vrms_expected,
			calibrationCtx.vfrms_measured_no_off);

	irmsos_copy = (uint32_t)calibrationCtx.result.i_rmsos;
	vrmsos_copy = (uint32_t)calibrationCtx.result.v_rmsos;
	ifrmsos_copy = (uint32_t)calibrationCtx.result.if_rmsos;
	vfrmsos_copy = (uint32_t)calibrationCtx.result.vf_rmsos;

	status = afe_write_32bit_reg(irmsos_regs[phase], &irmsos_copy);
	if (status != 0)
		return status;

	status = afe_write_32bit_reg(vrmsos_regs[phase], &vrmsos_copy);
	if (status != 0)
		return status;

	status = afe_write_32bit_reg(ifrmsos_regs[phase], &ifrmsos_copy);
	if (status != 0)
		return status;

	status = afe_write_32bit_reg(vfrmsos_regs[phase], &vfrmsos_copy);
	if (status != 0)
		return status;

	return 0;
}

static void calibration_calculate_error_after(void)
{
	float i_error_after, v_error_after;

	i_error_after = calculate_error(
				calibrationCtx.irms_measured,
				calibrationCtx.irms_expected);
	v_error_after = calculate_error(
				calibrationCtx.vrms_measured,
				calibrationCtx.vrms_expected);

	if (calibrationCtx.type == CALIBRATION_TYPE_GAIN) {
		calibrationCtx.result.gain_i_error_after = i_error_after;
		calibrationCtx.result.gain_v_error_after = v_error_after;
	} else {
		calibrationCtx.result.offset_i_error_after = i_error_after;
		calibrationCtx.result.offset_v_error_after = v_error_after;
	}
}

int calibration_process_cycle(void)
{
	int status = 0;
	bool zx_detected = false;
	uint8_t phase = calibrationCtx.phase;
	uint32_t igain_init_copy, vgain_init_copy;
	uint32_t irmsos_init_copy, vrmsos_init_copy;
	uint32_t ifrmsos_init_copy, vfrmsos_init_copy;

	switch (calibrationCtx.state) {
	case CALIBRATION_STATE_IDLE:
		/* Nothing to do */
		break;

	case CALIBRATION_STATE_INIT:
		calibrationCtx.cycle_count = 0;
		calibrationCtx.irms_sum = 0;
		calibrationCtx.irms_sum_no_gain = 0;
		calibrationCtx.irms_sum_no_off = 0;
		calibrationCtx.vrms_sum = 0;
		calibrationCtx.vrms_sum_no_gain = 0;
		calibrationCtx.vrms_sum_no_off = 0;
		calibrationCtx.ifrms_sum = 0;
		calibrationCtx.ifrms_sum_no_off = 0;
		calibrationCtx.vfrms_sum = 0;
		calibrationCtx.vfrms_sum_no_off = 0;
		calibrationCtx.state = CALIBRATION_STATE_COLLECTING;
		if (calibrationCtx.type == CALIBRATION_TYPE_GAIN) {
			status = afe_read_32bit_buff(igain_regs[phase],
						     1, &calibrationCtx.input.i_gain_init);
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}
			status = afe_read_32bit_buff(vgain_regs[phase],
						     1, &calibrationCtx.input.v_gain_init);
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}
		} else {
			status = afe_read_32bit_buff(irmsos_regs[phase],
						     1, &calibrationCtx.input.i_rmsos_init);
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}
			status = afe_read_32bit_buff(vrmsos_regs[phase],
						     1, &calibrationCtx.input.v_rmsos_init);
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}
			status = afe_read_32bit_buff(ifrmsos_regs[phase],
						     1, &calibrationCtx.input.if_rmsos_init);
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}
			status = afe_read_32bit_buff(vfrmsos_regs[phase],
						     1, &calibrationCtx.input.vf_rmsos_init);
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}
		}
		break;

	case CALIBRATION_STATE_COLLECTING:
		status = calibration_check_zero_crossing(&zx_detected);
		if (status != 0) {
			calibrationCtx.state = CALIBRATION_STATE_ERROR;
			break;
		}

		if (zx_detected) {
			status = calibration_read_rms_values();
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}

			if (calibrationCtx.cycle_count >= calibrationCtx.target_cycles) {
				calibrationCtx.state = CALIBRATION_STATE_CALCULATING;
			}
		}
		break;

	case CALIBRATION_STATE_CALCULATING:
		calibration_calculate_means();
		calibrationCtx.state = CALIBRATION_STATE_CALCULATE_ERROR_BEFORE;
		break;

	case CALIBRATION_STATE_CALCULATE_ERROR_BEFORE:
		/* Calculate expected values based on calibration type */
		if (calibrationCtx.type == CALIBRATION_TYPE_GAIN) {
			calibrationCtx.irms_expected = calculate_expected_irms(
							       &calibrationCtx.input,
							       calibrationCtx.input.nominal_current);
			calibrationCtx.vrms_expected = calculate_expected_vrms(
							       &calibrationCtx.input,
							       calibrationCtx.input.nominal_voltage);
			/* Calculate error before calibration */
			calibrationCtx.result.gain_i_error_before = calculate_error(
						calibrationCtx.irms_measured,
						calibrationCtx.irms_expected);
			calibrationCtx.result.gain_v_error_before = calculate_error(
						calibrationCtx.vrms_measured,
						calibrationCtx.vrms_expected);
		} else {
			calibrationCtx.irms_expected = calculate_expected_irms(
							       &calibrationCtx.input,
							       calibrationCtx.input.offset_current);
			calibrationCtx.vrms_expected = calculate_expected_vrms(
							       &calibrationCtx.input,
							       calibrationCtx.input.offset_voltage);
			/* Calculate error before calibration */
			calibrationCtx.result.offset_i_error_before = calculate_error(
						calibrationCtx.irms_measured,
						calibrationCtx.irms_expected);
			calibrationCtx.result.offset_v_error_before = calculate_error(
						calibrationCtx.vrms_measured,
						calibrationCtx.vrms_expected);
		}
		calibrationCtx.state = CALIBRATION_STATE_WRITING;
		break;

	case CALIBRATION_STATE_WRITING:
		if (calibrationCtx.type == CALIBRATION_TYPE_GAIN) {
			status = calibration_write_gain_registers();
		} else {
			status = calibration_write_offset_registers();
		}

		if (status != 0) {
			calibrationCtx.state = CALIBRATION_STATE_ERROR;
			break;
		}

		/* Reset for verification */
		calibrationCtx.cycle_count = 0;
		calibrationCtx.irms_sum = 0;
		calibrationCtx.irms_sum_no_gain = 0;
		calibrationCtx.irms_sum_no_off = 0;
		calibrationCtx.vrms_sum = 0;
		calibrationCtx.vrms_sum_no_gain = 0;
		calibrationCtx.vrms_sum_no_off = 0;
		calibrationCtx.ifrms_sum = 0;
		calibrationCtx.ifrms_sum_no_off = 0;
		calibrationCtx.vfrms_sum = 0;
		calibrationCtx.vfrms_sum_no_off = 0;
		calibrationCtx.state = CALIBRATION_STATE_VERIFY_COLLECTING;
		break;

	case CALIBRATION_STATE_VERIFY_COLLECTING:
		status = calibration_check_zero_crossing(&zx_detected);
		if (status != 0) {
			calibrationCtx.state = CALIBRATION_STATE_ERROR;
			break;
		}

		if (zx_detected) {
			status = calibration_read_rms_values();
			if (status != 0) {
				calibrationCtx.state = CALIBRATION_STATE_ERROR;
				break;
			}

			if (calibrationCtx.cycle_count >= calibrationCtx.target_cycles)
				calibrationCtx.state = CALIBRATION_STATE_VERIFY_CALCULATING;
		}
		break;

	case CALIBRATION_STATE_VERIFY_CALCULATING:
		calibration_calculate_means();
		calibration_calculate_error_after();
		/* Compare the initial errors with the current ones
		 * Keep initial values if initial errors smaller */
		if (calibrationCtx.type == CALIBRATION_TYPE_GAIN) {
			if (fabsf(calibrationCtx.result.gain_i_error_after) >= fabsf(
				    calibrationCtx.result.gain_i_error_before)) {
				igain_init_copy = calibrationCtx.input.i_gain_init;
				status = afe_write_32bit_reg(igain_regs[phase], &igain_init_copy);
				if (status != 0) {
					calibrationCtx.state = CALIBRATION_STATE_ERROR;
					break;
				}
				calibrationCtx.result.i_gain = calibrationCtx.input.i_gain_init;
				calibrationCtx.result.gain_i_error_after =
					calibrationCtx.result.gain_i_error_before;
			}
			if (fabsf(calibrationCtx.result.gain_v_error_after) >= fabsf(
				    calibrationCtx.result.gain_v_error_before)) {
				vgain_init_copy = calibrationCtx.input.v_gain_init;
				status = afe_write_32bit_reg(vgain_regs[phase], &vgain_init_copy);
				if (status != 0) {
					calibrationCtx.state = CALIBRATION_STATE_ERROR;
					break;
				}
				calibrationCtx.result.v_gain = calibrationCtx.input.v_gain_init;
				calibrationCtx.result.gain_v_error_after =
					calibrationCtx.result.gain_v_error_before;
			}
		} else {
			if (fabsf(calibrationCtx.result.offset_i_error_after) >= fabsf(
				    calibrationCtx.result.offset_i_error_before)) {
				irmsos_init_copy = calibrationCtx.input.i_rmsos_init;
				status = afe_write_32bit_reg(irmsos_regs[phase], &irmsos_init_copy);
				if (status != 0) {
					calibrationCtx.state = CALIBRATION_STATE_ERROR;
					break;
				}
				ifrmsos_init_copy = calibrationCtx.input.if_rmsos_init;
				status = afe_write_32bit_reg(ifrmsos_regs[phase], &ifrmsos_init_copy);
				if (status != 0) {
					calibrationCtx.state = CALIBRATION_STATE_ERROR;
					break;
				}
				calibrationCtx.result.i_rmsos = calibrationCtx.input.i_rmsos_init;
				calibrationCtx.result.if_rmsos = calibrationCtx.input.if_rmsos_init;
				calibrationCtx.result.offset_i_error_after =
					calibrationCtx.result.offset_i_error_before;
			}
			if (fabsf(calibrationCtx.result.offset_v_error_after) >= fabsf(
				    calibrationCtx.result.offset_v_error_before)) {
				vrmsos_init_copy = calibrationCtx.input.v_rmsos_init;
				status = afe_write_32bit_reg(vrmsos_regs[phase], &vrmsos_init_copy);
				if (status != 0) {
					calibrationCtx.state = CALIBRATION_STATE_ERROR;
					break;
				}
				vfrmsos_init_copy = calibrationCtx.input.vf_rmsos_init;
				status = afe_write_32bit_reg(vfrmsos_regs[phase], &vfrmsos_init_copy);
				if (status != 0) {
					calibrationCtx.state = CALIBRATION_STATE_ERROR;
					break;
				}
				calibrationCtx.result.v_rmsos = calibrationCtx.input.v_rmsos_init;
				calibrationCtx.result.vf_rmsos = calibrationCtx.input.vf_rmsos_init;
				calibrationCtx.result.offset_v_error_after =
					calibrationCtx.result.offset_v_error_before;
			}
		}
		calibrationCtx.result.done = true;
		calibrationCtx.state = CALIBRATION_STATE_DONE;
		break;

	case CALIBRATION_STATE_ERROR:
		calibrationCtx.result.done = false;
		break;
	}

	return status;
}

bool calibration_is_active(void)
{
	return (calibrationCtx.state != CALIBRATION_STATE_IDLE &&
		calibrationCtx.state != CALIBRATION_STATE_DONE &&
		calibrationCtx.state != CALIBRATION_STATE_ERROR);
}

bool calibration_is_done(void)
{
	return calibrationCtx.result.done;
}

void calibration_clear_done(void)
{
	calibrationCtx.result.done = false;
}

const char *calibration_get_status_string(void)
{
	switch (calibrationCtx.state) {
	case CALIBRATION_STATE_IDLE:
		return "idle";
	case CALIBRATION_STATE_DONE:
		return "done";
	case CALIBRATION_STATE_ERROR:
		return "error";
	default:
		return "in_progress";
	}
}
