/*******************************************************************************
 *   @file   pqlib_convert.h
 *   @brief  Heder file for converting data
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __PQLIB_CONVERT_H__
#define __PQLIB_CONVERT_H__

#include "adi_pqlib.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
/**
 * @brief Enumeration for possible event types for iio output preparation
 *
 */
typedef enum {
	EVENT_DIPS,
	EVENT_SWELL,
	EVENT_RVC,
	EVENT_INTRPS,
} EVENT_TYPE;

/**
 * @brief Converts ADI_AFE_RMS_TYPE to voltage/current magnitude
 * @param[in] mag - Magnitude code from the library
 * @param[in] scale - Magnitude of full range RMS
 * @return magnitude in volts or amperes
 */
float convert_rms_type(ADI_AFE_RMS_TYPE mag, float scale);

/**
 * @brief Converts ADI_AFE_FRACT_TYPR to angle
 * @param[in] angle - angle value
 * @return angle in radians
 */
float convert_fract_angle_type(ADI_PQLIB_FRACT_TYPE angle);

/**
 * @brief Converts ADI_PQLIB_FRACT_TYPE to float
 * @param[in] fractValue - Value for conversion
 * @return Equivalent float value
 */
float convert_fract_type(ADI_PQLIB_FRACT_TYPE fractValue);

/**
 * @brief Converts ADI_PQLIB_PCT_TYPE to percentage
 * @param[in] val - Value for conversion
 * @return Equivalent percentage value
 */
float convert_pct_type(ADI_PQLIB_PCT_TYPE val);

/**
 * @brief Converts val to correction coefficient.
 * @param[in] val -  Value for conversion
 * @return correction coefficient
 */
ADI_PQLIB_COEF_TYPE convert_to_coef_fract_type(float val);

/**
 * @brief Converts val to percentage.
 * @param[in] val -  Value for conversion
 * @return threshold percentage
 */
ADI_PQLIB_PCT_TYPE convert_to_pct_type(float val);

/**
 * @brief Converts val to freq in mhz.
 * @param[in] val -  Value for conversion
 * @return frequency
 */
ADI_PQLIB_FREQ_TYPE convert_to_freq_type(float val);

/**
 * @brief Converts time in sec to time in ms.
 * @param[in] timesec -  Value for conversion.
 * @return time in millisec
 */
uint32_t convert_to_time_ms(float timesec);

/**
 * @brief Prepare string output for specific event attribute types
 *
 * @param event_type Type of the event needed for output.
 * @param event_value Type of the event property, depends on the event.
 * @param buf IIO-buffer used for output preparation.
 * @return int Length of the prepared string
 */
int prepara_string(EVENT_TYPE event_type, int event_value, char *buf);

#endif /* __PQLIB_CONVERT_H__ */
