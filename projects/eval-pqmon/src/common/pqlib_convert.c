/*******************************************************************************
 *   @file   pqlib_convert.c
 *   @brief  Functions for converting data
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "pqlib_convert.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
