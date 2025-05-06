/***************************************************************************//**
 *   @file   iio_powrms.h
 *   @brief  Header file for IIO POWRMS example
 *   @author robert.budai@analog.com
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef __IIO_PWRMS_H__
#define __IIO_PWRMS_H__

#include "iio_app.h"
#include "no_os_uart.h"
#include "no_os_circular_buffer.h"
#include "powrms_data_processing.h"

#define INTC_DEVICE_ID                  1
#define IIO_BUFF_TYPE                   int32_t
#define TOTAL_PWM_CHANNELS              0  // No scannable channels (all voltage channels are now non-scannable)
#define POWRMS_CIRC_BUFF_SIZE           512
#define MAX_SIZE_BASE_ADDR              (TOTAL_PWM_CHANNELS > 0 ? TOTAL_PWM_CHANNELS * POWRMS_CIRC_BUFF_SIZE : 1)
#define BUFFER_VAL_PRECISSION           1000.0

// Buffer sizes for IIO attribute arrays
#define FREQUENCY_RANGES_BUFFER_SIZE    256   // For 8 frequency ranges

// IIO channel indices
#define IIO_CH_V_IN_CORRECTED           0
#define IIO_CH_V_OUT_CORRECTED          1
#define IIO_CH_TEMPERATURE              2
#define IIO_CH_V_IN_RAW                 3
#define IIO_CH_V_OUT_RAW                4
#define IIO_CH_P_FORWARD                5
#define IIO_CH_P_REVERSE                6
#define IIO_CH_PRECISION_ARRAY          7
#define IIO_CH_POLY_CALIB_5000MHZ       8
#define IIO_CH_POLY_CALIB_5000MHZ_REVERSE 9

// IIO attribute private data indices
#define IIO_ATTR_SCALE                  0
#define IIO_ATTR_OFFSET                 1
#define IIO_ATTR_PRECISION_FACTOR       2
#define IIO_ATTR_ARRAY_SIZE             3

// Number of channels
#define IIO_POWRMS_NUM_CHANNELS         10

// Scan type configuration
#define IIO_SCAN_REALBITS               32
#define IIO_SCAN_STORAGEBITS            32
#define IIO_SCAN_SHIFT                  0

// Voltage scaling constants
#define VOLTAGE_TO_MILLIVOLT_SCALE      1000
#define ADC_OFFSET_VALUE                0.0
#define FLOAT_PRECISION_DIGITS          3

// Frequency range processing constants
#define FREQ_RANGE_VALUES_PER_RANGE     28    // 28 precision values per frequency range
#define TEMP_COEFFS_PER_RANGE           3    // 3 temperature coefficients per frequency range
#define MAX_FREQ_RANGE_INDEX            7    // 0-7 for 8 frequency ranges
#define MIN_FREQ_RANGE_INDEX            0

// Buffer sizes for parsing calibration data
#define CALIB_VALUES_BUFFER_SIZE        256  // Buffer for 28 float values
#define TEMP_VALUES_BUFFER_SIZE         150  // Buffer for 3 float values

// String parsing constants
#define MIN_REMAINING_BUFFER_SIZE       20   // Minimum buffer space for value formatting
#define MIN_COMMA_BUFFER_SIZE           1    // Space needed for comma separator
#define COMMA_SEPARATOR_SIZE            1    // Size of comma separator

// Error return constants
#define IIO_ERROR_INVALID_INPUT         -1
#define IIO_SUCCESS                     0

// Boolean conversion constants
#define CALIBRATION_ENABLED             1
#define CALIBRATION_DISABLED            0
#define DEV_MODE_TRIGGER_VALUE          1


extern struct iio_device powrms_iio_descriptor;
extern IIO_BUFF_TYPE iio_data_buffer_loc[MAX_SIZE_BASE_ADDR];
extern struct no_os_circular_buffer *powrms_vin0_buffer;
extern struct no_os_circular_buffer *powrms_vin1_buffer;
extern struct no_os_circular_buffer *powrms_p_fwd_buffer;
extern struct no_os_circular_buffer *powrms_p_rev_buffer;


int powrms_buffers_init(void);
void powrms_buffers_cleanup(void);
int powrms_push_vin0(float value);
int powrms_push_vin1(float value);
int powrms_push_p_fwd(float value);
int powrms_push_p_rev(float value);
int powrms_get_latest_vin0(float *value);
int powrms_get_latest_vin1(float *value);
int powrms_get_latest_p_fwd(float *value);
int powrms_get_latest_p_rev(float *value);

// Precision array functions (float interface - with conversion)
int powrms_set_precision_value(int index, float value);
float powrms_get_precision_value(int index);
int powrms_set_precision_array(const float *values, int count);
int powrms_get_precision_array(float *values, int max_count);

// Precision array functions (integer interface - no conversion, for MCU internal use)
int powrms_set_precision_value_raw(int index, int32_t raw_value);
int32_t powrms_get_precision_value_raw(int index);
int powrms_set_precision_array_raw(const int32_t *raw_values, int count);
int powrms_get_precision_array_raw(int32_t *raw_values, int max_count);

// Reverse precision array functions (float interface - with conversion)
int powrms_set_precision_value_reverse(int index, float value);
float powrms_get_precision_value_reverse(int index);
int powrms_set_precision_array_reverse(const float *values, int count);
int powrms_get_precision_array_reverse(float *values, int max_count);

// Reverse precision array functions (integer interface - no conversion, for MCU internal use)
int powrms_set_precision_value_raw_reverse(int index, int32_t raw_value);
int32_t powrms_get_precision_value_raw_reverse(int index);
int powrms_set_precision_array_raw_reverse(const int32_t *raw_values,
		int count);
int powrms_get_precision_array_raw_reverse(int32_t *raw_values, int max_count);

// Frequency ranges functions
int powrms_set_frequency_range(int index, uint32_t frequency_MHz);
uint32_t powrms_get_frequency_range(int index);
int powrms_set_frequency_ranges(const uint32_t *frequencies, int count);
int powrms_get_frequency_ranges(uint32_t *frequencies, int max_count);

#endif