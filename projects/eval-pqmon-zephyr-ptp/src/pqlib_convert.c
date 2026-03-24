/*******************************************************************************
 *   @file   pqlib_convert.c
 *   @brief  Functions for converting data
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

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "pqlib_convert.h"
#include "pqlib_example.h"
#include "iio_pqm.h"
#include "pqm.h"
#include <string.h>
#include <inttypes.h>

ADI_AFE_ANGLE_TYPE
convert_angle_type(ADI_AFE_ANGLE_TYPE angleRaw)
{
	ADI_AFE_ANGLE_TYPE degree;
	degree = (ADI_AFE_ANGLE_TYPE)((float)angleRaw * 0.017578125f);

	return degree;
}

float convert_rms_type(ADI_AFE_RMS_TYPE mag, float scale)
{
	float rms;
	float sqrt2 = 1.414213f;
	rms = mag * scale / ((float)ADI_PQLIB_MAX_RMS_VALUE);
	rms = rms / sqrt2;

	return rms;
}

float convert_fract_type(ADI_PQLIB_FRACT_TYPE fractValue)
{
	return (float)fractValue / (float)(1 << ADI_PQLIB_FRACT_BITS);
}

float convert_fract_angle_type(ADI_PQLIB_FRACT_TYPE angle)
{
	return (float)angle * (2.0f * (float)M_PI / (float)0x7FFF);
}

float convert_pct_type(ADI_PQLIB_PCT_TYPE val)
{
	float pct = (float)val / 100.0f;
	if (pct >= 0 && pct <= 100)
		return pct;
	else
		return 0;
}

ADI_PQLIB_PCT_TYPE
convert_to_pct_type(float val)
{
	ADI_PQLIB_PCT_TYPE pctValue;
	pctValue = (ADI_PQLIB_PCT_TYPE)(val * 100);
	return pctValue;
}

ADI_PQLIB_FREQ_TYPE
convert_to_freq_type(float val)
{
	ADI_PQLIB_FREQ_TYPE freqValue;
	freqValue = (ADI_PQLIB_FREQ_TYPE)(val * 1000);
	return freqValue;
}

ADI_PQLIB_COEF_TYPE
convert_to_coef_fract_type(float val)
{
	ADI_PQLIB_COEF_TYPE corrCoef;
	corrCoef = (ADI_PQLIB_COEF_TYPE)(val * (1 << ADI_PQLIB_COEF_FRACT_BITS));
	return corrCoef;
}

uint32_t convert_to_time_ms(float timesec)
{
	uint32_t timems;
	timems = (uint32_t)(timesec * 1000);
	return timems;
}

float convert_power_type(int32_t power, float scale)
{
	float real_power;
	real_power = power * scale / ((float)ADI_PQLIB_POWER_MAX_VALUE);
	real_power = real_power / 2.0f;

	return real_power;
}

float convert_energy_type(int32_t energy_hi, float scale)
{
	return convert_power_type(energy_hi, scale) / ENERGY_ACCUMULATION_FACTOR;
}

float compute_power_factor(int32_t watthr, int32_t varhr)
{
	int64_t w = (int64_t)watthr;
	int64_t v = (int64_t)varhr;
	int64_t denom_sq = w * w + v * v;

	if (denom_sq == 0)
		return 0.0f;

	return (float)w / sqrtf((float)denom_sq);
}

int prepara_string(EVENT_TYPE event_type, int event_value, char *buf,
		   int event_count)
{
	int max_event_number = 0;
	uint32_t val_tmp_u32;
	int64_t val_tmp_d64;
	char buffTmp[24];
	float val_tmp_f;

	strcpy(buf, "");

	switch (event_type) {
	case EVENT_DIPS:
		max_event_number = ADI_PQLIB_CFG_NUM_DIPS;
		break;
	case EVENT_SWELL:
		max_event_number = ADI_PQLIB_CFG_NUM_SWELLS;
		break;
	case EVENT_RVC:
		max_event_number = ADI_PQLIB_CFG_NUM_RVC;
		break;
	case EVENT_INTRPS:
		max_event_number = ADI_PQLIB_CFG_NUM_INTRP;
		break;
	}

	for (int j = 0; j < MAX_EVENT_NUMBER; j++) {
		if (j < max_event_number && j < event_count) {
			switch (event_value) {
			case CHAN_EVENT_START_TIME:
				switch (event_type) {
				case EVENT_DIPS:
					val_tmp_d64 = accumulated_events.dips[j].startTime;
					break;
				case EVENT_SWELL:
					val_tmp_d64 = accumulated_events.swells[j].startTime;
					break;
				case EVENT_RVC:
					val_tmp_d64 = accumulated_events.rvc[j].startTime;
					break;
				case EVENT_INTRPS:
					val_tmp_d64 = accumulated_events.intrp[j].startTime;
					break;
				}
				sprintf(buffTmp, "%" PRId64 "", val_tmp_d64);
				break;
			case CHAN_EVENT_END_TIME:
				switch (event_type) {
				case EVENT_DIPS:
					val_tmp_d64 = accumulated_events.dips[j].endTime;
					break;
				case EVENT_SWELL:
					val_tmp_d64 = accumulated_events.swells[j].endTime;
					break;
				case EVENT_RVC:
					val_tmp_d64 = accumulated_events.rvc[j].endTime;
					break;
				case EVENT_INTRPS:
					val_tmp_d64 = accumulated_events.intrp[j].endTime;
					break;
				}
				sprintf(buffTmp, "%" PRId64 "", val_tmp_d64);
				break;
			case CHAN_EVENT_DURATION_IN_CYCL:
				switch (event_type) {
				case EVENT_DIPS:
					val_tmp_u32 = accumulated_events.dips[j].durationInCycles;
					break;
				case EVENT_SWELL:
					val_tmp_u32 = accumulated_events.swells[j].durationInCycles;
					break;
				case EVENT_RVC:
					val_tmp_u32 = accumulated_events.rvc[j].durationInCycles;
					break;
				case EVENT_INTRPS:
					val_tmp_u32 = accumulated_events.intrp[j].durationInCycles;
					break;
				}
				sprintf(buffTmp, "%" PRIu32 "", val_tmp_u32);
				break;
			case CHAN_EVENT_MIN_MAG:
				if (event_type == EVENT_DIPS) {
					val_tmp_f = accumulated_events.dips[j].minMag;
					val_tmp_f = convert_rms_type(val_tmp_f,
								     pqlibExample.exampleConfig.voltageScale);
				}
				sprintf(buffTmp, "%.5f", (double)val_tmp_f);
				break;
			case CHAN_EVENT_MAX_MAG:
				if (event_type == EVENT_SWELL) {
					val_tmp_f = accumulated_events.swells[j].maxMag;
					val_tmp_f = convert_rms_type(val_tmp_f,
								     pqlibExample.exampleConfig.voltageScale);
				}
				sprintf(buffTmp, "%.5f", (double)val_tmp_f);
				break;
			case CHAN_EVENT_DELTA_U_MAX:
				if (event_type == EVENT_RVC) {
					val_tmp_f = accumulated_events.rvc[j].deltaUmax;
					val_tmp_f = convert_rms_type(val_tmp_f,
								     pqlibExample.exampleConfig.voltageScale);
				}
				sprintf(buffTmp, "%.5f", (double)val_tmp_f);
				break;
			case CHAN_EVENT_DELTA_U_SS:
				if (event_type == EVENT_RVC) {
					val_tmp_f = accumulated_events.rvc[j].deltaUss;
					val_tmp_f = convert_rms_type(val_tmp_f,
								     pqlibExample.exampleConfig.voltageScale);
				}
				sprintf(buffTmp, "%.5f", (double)val_tmp_f);
				break;
			}
		} else {
			switch (event_value) {
			case CHAN_EVENT_START_TIME:
			case CHAN_EVENT_END_TIME:
				sprintf(buffTmp, "%" PRId64 "", (int64_t)0);
				break;
			case CHAN_EVENT_DURATION_IN_CYCL:
				sprintf(buffTmp, "%" PRIu32 "", (uint32_t)0);
				break;
			case CHAN_EVENT_MIN_MAG:
			case CHAN_EVENT_MAX_MAG:
			case CHAN_EVENT_DELTA_U_MAX:
			case CHAN_EVENT_DELTA_U_SS:
				sprintf(buffTmp, "%.5f", (double)0.0f);
				break;
			}
		}

		strcat(buf, buffTmp);
		if (j != (MAX_EVENT_NUMBER - 1))
			strcat(buf, " ");
	}
	return (strlen(buf));
}
