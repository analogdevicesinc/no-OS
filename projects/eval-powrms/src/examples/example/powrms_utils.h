/***************************************************************************//**
 *   @file   powrms_utils.h
 *   @brief  Utility functions header for input handling and data conversion in eval-powrms project
 *   @author Robert Budai (robert.budai@analog.com)
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

#ifndef POWRMS_UTILS_H
#define POWRMS_UTILS_H

#include <stdint.h>
#include "common_data.h"

// EEPROM MEMORY LAYOUT
/*
    0x0002: Calibration data usage flag (1 byte)
        - 0: Do not use calibration data
        - 1: Use calibration data
    0x0003: Calibration data usage for temperature compensation (1 byte)
        - 0: Do not use temperature compensation
        - 1: Use temperature compensation
    0x0004 - 0x00C3: Precision values array (112 int32_t values, 4 bytes each, total 448 bytes)
        - Stored in little-endian format
    0x00C4 - 0x0113: Temperature compensation coefficients array (24 int32_t values, 4 bytes each, total 96 bytes)
        - Stored in little-endian format
    0x0114 - 0x0117: Voltage temperature compensation value (int32_t, 4 bytes)
        - Stored in little-endian format
    0x0118 - 0x01D7: Default factory precision values array (112 int32_t values, 4 bytes each, total 448 bytes)
        - Stored in little-endian format
    0x01D8 - 0x0237: Default factory temperature compensation coefficients array (24 int32_t values, 4 bytes each, total 96 bytes)
        - Stored in little-endian format
    0x0238 - 0x023B: Default factory voltage temperature compensation value (int32_t, 4 bytes)
        - Stored in little-endian format
    0x023C - 0x03FB: Reverse precision values array (112 int32_t values, 4 bytes each, total 448 bytes)
        - Stored in little-endian format
    0x03FC - 0x05BB: Default factory reverse precision values array (112 int32_t values, 4 bytes each, total 448 bytes)
        - Stored in little-endian format
    0x05BC - 0x05E3: Polynomial calibration coefficients for 5000MHz (10 int32_t values, 4 bytes each, total 40 bytes)
        - Stored in little-endian format
        - Order: intercept, c_x, c_f, c_x2, c_xf, c_f2, c_x3, c_x2f, c_xf2, c_f3
    0x05E4 - 0x060B: Default factory polynomial calibration coefficients for 5000MHz (10 int32_t values, 4 bytes each, total 40 bytes)
        - Stored in little-endian format
*/

// Note: Polynomial coefficients changed to double (8 bytes each) for 20 decimal precision
// Updated memory layout:
//    0x05BC - 0x060B: Polynomial calibration coefficients for 5000MHz (10 double values, 8 bytes each, total 80 bytes)
//    0x060C - 0x065B: Default factory polynomial calibration coefficients for 5000MHz (10 double values, 8 bytes each, total 80 bytes)

#define MEM_USE_DEF_CALIB_DATA_POZ      0x0002
#define MEM_USE_DEF_CALIB_DATA_LEN      1

#define MEM_TEMP_COMP_DATA_POZ          (MEM_USE_DEF_CALIB_DATA_POZ + MEM_USE_DEF_CALIB_DATA_LEN)
#define MEM_TEMP_COMP_DATA_LEN          1

// User configurable parameters in EEPROM

#define MEM_PRECISION_ARRAY_POZ         (MEM_TEMP_COMP_DATA_POZ + MEM_TEMP_COMP_DATA_LEN)
#define MEM_PRECISION_ARRAY_SIZE        960

#define MEM_TEMP_COMP_ARRAY_POZ         (MEM_PRECISION_ARRAY_POZ + MEM_PRECISION_ARRAY_SIZE)
#define MEM_TEMP_COMP_ARRAY_SIZE        96

#define MEM_V_TEMP_COMP_VAL_POZ         (MEM_TEMP_COMP_ARRAY_POZ + MEM_TEMP_COMP_ARRAY_SIZE)
#define MEM_V_TEMP_COMP_VAL_SIZE        4

// Default values in EEPROM

#define MEM_DEF_PRECISION_ARRAY_POZ     (MEM_V_TEMP_COMP_VAL_POZ + MEM_V_TEMP_COMP_VAL_SIZE)
#define MEM_DEF_PRECISION_ARRAY_SIZE    960

#define MEM_DEF_TEMP_COMP_ARRAY_POZ     (MEM_DEF_PRECISION_ARRAY_POZ + MEM_DEF_PRECISION_ARRAY_SIZE)
#define MEM_DEF_TEMP_COMP_ARRAY_SIZE     96

#define MEM_DEF_V_TEMP_COMP_VAL_POZ     (MEM_DEF_TEMP_COMP_ARRAY_POZ + MEM_DEF_TEMP_COMP_ARRAY_SIZE)
#define MEM_DEF_V_TEMP_COMP_VAL_SIZE     4

// Reverse precision array values in EEPROM (user configurable)

#define MEM_PRECISION_ARRAY_REVERSE_POZ         (MEM_DEF_V_TEMP_COMP_VAL_POZ + MEM_DEF_V_TEMP_COMP_VAL_SIZE)
#define MEM_PRECISION_ARRAY_REVERSE_SIZE        960

// Default reverse precision array values in EEPROM (factory settings)

#define MEM_DEF_PRECISION_ARRAY_REVERSE_POZ     (MEM_PRECISION_ARRAY_REVERSE_POZ + MEM_PRECISION_ARRAY_REVERSE_SIZE)
#define MEM_DEF_PRECISION_ARRAY_REVERSE_SIZE    960

// Polynomial calibration coefficients for 5000MHz (user configurable)

#define MEM_POLY_5000MHZ_COEFFS_POZ             (MEM_DEF_PRECISION_ARRAY_REVERSE_POZ + MEM_DEF_PRECISION_ARRAY_REVERSE_SIZE)
#define MEM_POLY_5000MHZ_COEFFS_SIZE            80  // 10 coefficients * 8 bytes each (double)

// Default polynomial calibration coefficients for 5000MHz (factory settings)

#define MEM_DEF_POLY_5000MHZ_COEFFS_POZ         (MEM_POLY_5000MHZ_COEFFS_POZ + MEM_POLY_5000MHZ_COEFFS_SIZE)
#define MEM_DEF_POLY_5000MHZ_COEFFS_SIZE        80  // 10 coefficients * 8 bytes each (double)

// Polynomial calibration coefficients for 5000MHz reverse (user configurable)

#define MEM_POLY_5000MHZ_REVERSE_COEFFS_POZ     (MEM_DEF_POLY_5000MHZ_COEFFS_POZ + MEM_DEF_POLY_5000MHZ_COEFFS_SIZE)
#define MEM_POLY_5000MHZ_REVERSE_COEFFS_SIZE    80  // 10 coefficients * 8 bytes each (double)

// Default polynomial calibration coefficients for 5000MHz reverse (factory settings)

#define MEM_DEF_POLY_5000MHZ_REVERSE_COEFFS_POZ (MEM_POLY_5000MHZ_REVERSE_COEFFS_POZ + MEM_POLY_5000MHZ_REVERSE_COEFFS_SIZE)
#define MEM_DEF_POLY_5000MHZ_REVERSE_COEFFS_SIZE 80  // 10 coefficients * 8 bytes each (double)

/**
 * @brief Read and decode user input from UART
 *
 * @return uint8_t Input command code:
 *         - 0: No input received
 *         - 1: 'q' - Next/Navigate forward
 *         - 2: 'w' - Back/Increment (context dependent)
 *         - 3: 'e' - Enter/Confirm
 */
uint8_t read_input();

/**
 * @brief Convert float value to string representation
 *
 * @param var Float value to convert
 * @param text_buf Output buffer for string representation
 * @param int_digits Number of integer digits
 * @param float_digits Number of decimal digits
 * @param with_floating_comma If true, include decimal point in output
 */
void powrms_float_to_str(float var, char* text_buf, uint8_t int_digits,
			 uint8_t float_digits, bool with_floating_comma);

/**
 * @brief Convert integer value to string representation
 *
 * @param var Integer value to convert
 * @param text_buf Output buffer for string representation
 */
void powrms_int_to_str(int var, char* text_buf);


/**
 * @brief Configure and initialize the watchdog timer
 * @param msec Timeout period in milliseconds
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_watchdog_init(int msec);

/**
 * @brief Reset the watchdog timer to prevent system reset
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_watchdog_reset(void);

/**
 * @brief Disable the watchdog timer
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_watchdog_disable(void);

/**
 * @brief Initialize EEPROM and set first byte to 1 if it's 0
 *
 * This function reads the first byte of the EEPROM and if it's 0,
 * sets it to 1. If it's already 1, leaves it as is.
 * The EEPROM should already be initialized before calling this function.
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_get_use_def_cal_data(int32_t *use_cal_data);

/**
 * @brief Write a value to the first byte of the EEPROM
 *
 * This function writes the specified value to the first byte (address 0)
 * of the EEPROM. The EEPROM should already be initialized before calling
 * this function.
 *
 * @param value The value to write to the first byte
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_set_use_def_cal_data(int32_t value);

/**
 * @brief Read temperature compensation flag from EEPROM
 *
 * This function reads the temperature compensation flag from the EEPROM
 * at the designated memory position. The EEPROM should already be initialized
 * before calling this function.
 *
 * @param use_temp_comp Pointer to store the temperature compensation flag value
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_get_temp_comp_data(int32_t *use_temp_comp);

/**
 * @brief Write temperature compensation flag to EEPROM
 *
 * This function writes the specified temperature compensation flag to the EEPROM
 * at the designated memory position. The EEPROM should already be initialized
 * before calling this function.
 *
 * @param value The temperature compensation flag value to write (0 or 1)
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_set_temp_comp_data(int32_t value);

/**
 * @brief Write precision array to EEPROM starting from the second byte
 *
 * This function writes the entire precision_values array (48 int32_t values)
 * to the EEPROM starting from address 1 (second byte). The first byte is
 * reserved for calibration flags.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array of 48 int32_t precision values
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_precision_array(const int32_t *precision_array);

/**
 * @brief Read precision array from EEPROM starting from the second byte
 *
 * This function reads the entire precision_values array (48 int32_t values)
 * from the EEPROM starting from address 1 (second byte). The first byte is
 * reserved for calibration flags.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array where 48 int32_t precision values will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_precision_array(int32_t *precision_array);

/**
 * @brief Write reverse precision array to EEPROM
 *
 * This function writes the entire precision_values_reverse array (112 int32_t values)
 * to the EEPROM starting from the designated reverse precision array memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array of 112 int32_t reverse precision values
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_precision_array_reverse(const int32_t *precision_array);

/**
 * @brief Read reverse precision array from EEPROM
 *
 * This function reads the entire precision_values_reverse array (112 int32_t values)
 * from the EEPROM starting from the designated reverse precision array memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array where 112 int32_t reverse precision values will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_precision_array_reverse(int32_t *precision_array);

/**
 * @brief Write default precision array to EEPROM
 *
 * This function writes the entire precision_values array (48 int32_t values)
 * to the EEPROM starting from the default precision array memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array of 48 int32_t precision values
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_def_precision_array(const int32_t *precision_array);

/**
 * @brief Read default precision array from EEPROM
 *
 * This function reads the entire precision_values array (48 int32_t values)
 * from the EEPROM starting from the default precision array memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array where 48 int32_t precision values will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_def_precision_array(int32_t *precision_array);

/**
 * @brief Write default reverse precision array to EEPROM
 *
 * This function writes the entire precision_values_reverse array (112 int32_t values)
 * to the EEPROM starting from the default reverse precision array memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array of 112 int32_t reverse precision values
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_def_precision_array_reverse(const int32_t
		*precision_array);

/**
 * @brief Read default reverse precision array from EEPROM
 *
 * This function reads the entire precision_values_reverse array (112 int32_t values)
 * from the EEPROM starting from the default reverse precision array memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param precision_array Pointer to the array where 112 int32_t reverse precision values will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_def_precision_array_reverse(int32_t *precision_array);

/**
 * @brief Get firmware version string in format "major.minor.patch"
 *
 * @param version_buf Buffer to store the version string (minimum 16 bytes)
 */
void powrms_get_firmware_version(char* version_buf);

/**
 * @brief Write voltage temperature compensation value to EEPROM
 *
 * This function writes the voltage temperature compensation value (int32_t)
 * to the EEPROM at the designated memory position for user configurable data.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param value The voltage temperature compensation value to write
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_v_temp_comp_val(int32_t value);

/**
 * @brief Read voltage temperature compensation value from EEPROM
 *
 * This function reads the voltage temperature compensation value (int32_t)
 * from the EEPROM at the designated memory position for user configurable data.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param value Pointer to store the voltage temperature compensation value
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_v_temp_comp_val(int32_t *value);

/**
 * @brief Write default voltage temperature compensation value to EEPROM
 *
 * This function writes the default voltage temperature compensation value (int32_t)
 * to the EEPROM at the designated memory position for default data.
* The EEPROM should already be initialized before calling this function.
 *
 * @param value The default voltage temperature compensation value to write
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_def_v_temp_comp_val(int32_t value);

/**
 * @brief Read default voltage temperature compensation value from EEPROM
 *
 * This function reads the default voltage temperature compensation value (int32_t)
 * from the EEPROM at the designated memory position for default data.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param value Pointer to store the default voltage temperature compensation value
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_def_v_temp_comp_val(int32_t *value);

/**
 * @brief Write polynomial calibration coefficients for 5000MHz to EEPROM
 *
 * This function writes the 10 polynomial calibration coefficients (double array)
 * to the EEPROM at the designated memory position. The coefficients are:
 * intercept, c_x, c_f, c_x2, c_xf, c_f2, c_x3, c_x2f, c_xf2, c_f3
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array of 10 double polynomial coefficients
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_poly_5000MHz_coeffs(const double *coeffs);

/**
 * @brief Read polynomial calibration coefficients for 5000MHz from EEPROM
 *
 * This function reads the 10 polynomial calibration coefficients (double array)
 * from the EEPROM at the designated memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array where 10 double polynomial coefficients will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_poly_5000MHz_coeffs(double *coeffs);

/**
 * @brief Write default polynomial calibration coefficients for 5000MHz to EEPROM
 *
 * This function writes the default 10 polynomial calibration coefficients (double array)
 * to the EEPROM at the designated memory position for factory data.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array of 10 double default polynomial coefficients
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_def_poly_5000MHz_coeffs(const double *coeffs);

/**
 * @brief Read default polynomial calibration coefficients for 5000MHz from EEPROM
 *
 * This function reads the default 10 polynomial calibration coefficients (double array)
 * from the EEPROM at the designated memory position for factory data.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array where 10 double default polynomial coefficients will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_def_poly_5000MHz_coeffs(double *coeffs);

/**
 * @brief Write polynomial calibration coefficients for 5000MHz reverse to EEPROM
 *
 * This function writes the 10 polynomial calibration coefficients (double array)
 * for reverse power to the EEPROM at the designated memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array of 10 double polynomial coefficients
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_poly_5000MHz_reverse_coeffs(const double *coeffs);

/**
 * @brief Read polynomial calibration coefficients for 5000MHz reverse from EEPROM
 *
 * This function reads the 10 polynomial calibration coefficients (double array)
 * for reverse power from the EEPROM at the designated memory position.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array where 10 double polynomial coefficients will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_poly_5000MHz_reverse_coeffs(double *coeffs);

/**
 * @brief Write default polynomial calibration coefficients for 5000MHz reverse to EEPROM
 *
 * This function writes the default 10 polynomial calibration coefficients (double array)
 * for reverse power to the EEPROM at the designated memory position for factory data.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array of 10 double default polynomial coefficients
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_write_def_poly_5000MHz_reverse_coeffs(const double *coeffs);

/**
 * @brief Read default polynomial calibration coefficients for 5000MHz reverse from EEPROM
 *
 * This function reads the default 10 polynomial calibration coefficients (double array)
 * for reverse power from the EEPROM at the designated memory position for factory data.
 * The EEPROM should already be initialized before calling this function.
 *
 * @param coeffs Pointer to array where 10 double default polynomial coefficients will be stored
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_read_def_poly_5000MHz_reverse_coeffs(double *coeffs);


int powrms_init_memory_upon_boot();

#endif /* POWRMS_UTILS_H */