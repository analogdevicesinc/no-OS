/***************************************************************************//**
 *   @file   powrms_utils.c
 *   @brief  Utility functions for input handling and data conversion in EVAL-POWRMS project
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

#include "powrms_utils.h"
#include "powrms_gpios.h"
#include "no_os_eeprom.h"
#include "no_os_delay.h"
#include "iio_powrms.h"


uint8_t read_input()
{
	int button_state;

	button_state = buttons_read();
	if (button_state > 0) {
		return button_state;
	}
	return 0;
}

void powrms_float_to_str(float var, char* text_buf, uint8_t int_precision,
			 uint8_t float_precision, bool is_settings_variable)
{
	uint8_t i = 0;
	if (!text_buf)
		return;

	// Handle negative numbers
	if (!is_settings_variable && var < 0) {
		text_buf[i++] = '-';
		var = -var;
	}

	// Integer part
	int int_part = (int)var;

	for (uint8_t j = int_precision; j > 0; j--) {
		int divisor = 1;
		for (uint8_t k = 1; k < j; k++)
			divisor *= 10;
		text_buf[i++] = '0' + (int_part / divisor) % 10;
	}

	// Add decimal point
	if (!is_settings_variable)
		text_buf[i++] = '.';

	// Fractional part
	float frac = var - (float)int_part;
	for (uint8_t j = 1; j <= float_precision; j++) {
		frac *= 10;
		text_buf[i++] = '0' + ((int)frac % 10);
	}
	if (!is_settings_variable)
		text_buf[i] = '\0';
}

void powrms_int_to_str(int var, char* text_buf)
{
	uint8_t i = -1;
	if (!text_buf)
		return;

#if INTEGER_PRECISION == 4
	text_buf[++i] = '0' + (uint16_t)(var / 1000) % 10;
#endif
#if INTEGER_PRECISION == 3
	text_buf[++i] = '0' + (uint16_t)(var / 100) % 10;
#endif
#if INTEGER_PRECISION == 2
	text_buf[++i] = '0' + (uint16_t)(var / 10) % 10;
#endif
#if INTEGER_PRECISION == 1
	text_buf[++i] = '0' + (uint16_t)(var / 1) % 10;
	text_buf[++i] = '\0';
#endif
}

/**
 * @brief Initialize EEPROM and set first byte to 1 if it's 0
 *
 * This function reads the first byte of the EEPROM and if it's 0,
 * sets it to 1. If it's already 1, leaves it as is.
 * The EEPROM should already be initialized before calling this function.
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_eeprom_get_use_def_cal_data(int32_t *use_cal_data)
{
	uint8_t first_byte;
	int ret;

	if (!m24512_desc) {
		return -ENODEV;
	}

	ret = no_os_eeprom_read(m24512_desc, MEM_USE_DEF_CALIB_DATA_POZ, &first_byte,
				MEM_USE_DEF_CALIB_DATA_LEN); // First 2 bytes from EEPROM is used for identifying
	if (ret) {
		return ret;
	}

	*use_cal_data = (int32_t)first_byte > 0 ? 1 : 0;

	return 0;
}

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
int powrms_eeprom_set_use_def_cal_data(int32_t value)
{
	uint8_t tmp;
	int ret;

	if (!m24512_desc) {
		return -ENODEV;
	}

	tmp = (uint8_t)value;

	ret = no_os_eeprom_write(m24512_desc, MEM_USE_DEF_CALIB_DATA_POZ, &tmp,
				 MEM_USE_DEF_CALIB_DATA_LEN);
	if (ret) {
		return ret;
	}

	return 0;
}

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
int powrms_eeprom_get_temp_comp_data(int32_t *use_temp_comp)
{
	uint8_t temp_comp_byte;
	int ret;

	if (!m24512_desc) {
		return -ENODEV;
	}

	ret = no_os_eeprom_read(m24512_desc, MEM_TEMP_COMP_DATA_POZ, &temp_comp_byte,
				MEM_TEMP_COMP_DATA_LEN);
	if (ret) {
		return ret;
	}

	*use_temp_comp = (int32_t)temp_comp_byte;

	return 0;
}

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
int powrms_eeprom_set_temp_comp_data(int32_t value)
{
	uint8_t tmp;
	int ret;

	if (!m24512_desc) {
		return -ENODEV;
	}

	tmp = (uint8_t)value;

	ret = no_os_eeprom_write(m24512_desc, MEM_TEMP_COMP_DATA_POZ, &tmp,
				 MEM_TEMP_COMP_DATA_LEN);
	if (ret) {
		return ret;
	}

	return 0;
}

/**
 * @brief Write precision array to EEPROM starting from the second byte
 *
 * This function writes the precision array using single-byte writes to avoid
 * I2C bus blocking issues that can occur with large transfers. Each int32_t
 * value is broken down into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_precision_array(const int32_t *precision_array)
{
	int ret;
	uint16_t base_address =
		MEM_PRECISION_ARRAY_POZ; // Starting address for precision array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t

	if (!m24512_desc) {
		return -ENODEV;
	}

	if (!precision_array) {
		return -EINVAL; // Invalid parameter
	}

	// Write each int32_t value byte by byte
	// Array: 48 elements × 4 bytes each = 192 bytes total
	for (int i = 0; i < PRECISION_ARRAY_SIZE; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));
		uint32_t value = (uint32_t)precision_array[i];

		// Break down int32_t into bytes (little-endian format)
		byte_buffer[0] = (uint8_t)(value & 0xFF);
		byte_buffer[1] = (uint8_t)((value >> 8) & 0xFF);
		byte_buffer[2] = (uint8_t)((value >> 16) & 0xFF);
		byte_buffer[3] = (uint8_t)((value >> 24) & 0xFF);

		// Write 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_write(m24512_desc, current_address + byte_idx,
						 &byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Write failed
			}

			// Small delay between byte writes to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Optimized delay for M24512 precision array - typical write cycle is 2-3ms
		no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle
	}

	return 0; // Success
}

/**
 * @brief Read precision array from EEPROM starting from the second byte
 *
 * This function reads the precision array using single-byte reads to avoid
 * I2C bus blocking issues that can occur with large transfers. Each int32_t
 * value is read byte by byte and reconstructed in little-endian format.
 */
int powrms_eeprom_read_precision_array(int32_t *precision_array)
{
	int ret;
	uint16_t base_address =
		MEM_PRECISION_ARRAY_POZ; // Starting address for precision array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t

	if (!m24512_desc) {
		return -ENODEV; // EEPROM not initialized
	}

	if (!precision_array) {
		return -EINVAL; // Invalid parameter
	}

	// Read each int32_t value byte by byte
	// Array: 48 elements × 4 bytes each = 192 bytes total
	for (int i = 0; i < PRECISION_ARRAY_SIZE; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));

		// Read 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_read(m24512_desc, current_address + byte_idx,
						&byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Read failed
			}

			// Small delay between byte reads to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Reconstruct int32_t from bytes (little-endian format)
		precision_array[i] = (int32_t)((uint32_t)byte_buffer[0] |
					       ((uint32_t)byte_buffer[1] << 8) |
					       ((uint32_t)byte_buffer[2] << 16) |
					       ((uint32_t)byte_buffer[3] << 24));

		// Longer delay between int32_t reads to allow I2C bus recovery
		no_os_mdelay(2); // 2ms delay between each complete value
	}

	return 0; // Success
}

/**
 * @brief Write temperature correction array to EEPROM
 *
 * This function writes the temperature correction array using single-byte writes to avoid
 * I2C bus blocking issues that can occur with large transfers. Each int32_t
 * value is broken down into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_temp_corr_array(const int32_t *temp_corr_array)
{
	int ret;
	uint16_t base_address =
		MEM_TEMP_COMP_ARRAY_POZ; // Starting address for temperature correction array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t
	int array_size = TEMPERATURE_CORRECTION_COEFFS *
			 FREQUENCY_RANGE_NR; // 3*8 = 24 values

	if (!m24512_desc) {
		return -ENODEV;
	}

	if (!temp_corr_array) {
		return -EINVAL; // Invalid parameter
	}

	// Write each int32_t value byte by byte
	// Array: 24 elements × 4 bytes each = 96 bytes total
	for (int i = 0; i < array_size; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));

		// Convert int32_t to bytes (little-endian format)
		byte_buffer[0] = (uint8_t)(temp_corr_array[i] & 0xFF);
		byte_buffer[1] = (uint8_t)((temp_corr_array[i] >> 8) & 0xFF);
		byte_buffer[2] = (uint8_t)((temp_corr_array[i] >> 16) & 0xFF);
		byte_buffer[3] = (uint8_t)((temp_corr_array[i] >> 24) & 0xFF);

		// Write 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_write(m24512_desc, current_address + byte_idx,
						 &byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Write failed
			}

			// Small delay between byte writes to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Optimized delay for M24512 temperature array - typical write cycle is 2-3ms
		no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle
	}

	return 0; // Success
}

/**
 * @brief Read temperature correction array from EEPROM
 *
 * This function reads the temperature correction array using single-byte reads to avoid
 * I2C bus blocking issues that can occur with large transfers. Each int32_t
 * value is read byte by byte and reconstructed in little-endian format.
 */
int  powrms_eeprom_read_temp_corr_array(int32_t *temp_corr_array)
{
	int ret;
	uint16_t base_address =
		MEM_TEMP_COMP_ARRAY_POZ; // Starting address for temperature correction array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t
	int array_size = TEMPERATURE_CORRECTION_COEFFS *
			 FREQUENCY_RANGE_NR; // 3*8 = 24 values

	if (!m24512_desc) {
		return -ENODEV; // EEPROM not initialized
	}

	if (!temp_corr_array) {
		return -EINVAL; // Invalid parameter
	}

	// Read each int32_t value byte by byte
	// Array: 24 elements × 4 bytes each = 96 bytes total
	for (int i = 0; i < array_size; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));

		// Read 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_read(m24512_desc, current_address + byte_idx,
						&byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Read failed
			}

			// Small delay between byte reads to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Reconstruct int32_t from bytes (little-endian format)
		temp_corr_array[i] = (int32_t)((uint32_t)byte_buffer[0] |
					       ((uint32_t)byte_buffer[1] << 8) |
					       ((uint32_t)byte_buffer[2] << 16) |
					       ((uint32_t)byte_buffer[3] << 24));

		// Longer delay between int32_t reads to allow I2C bus recovery
		no_os_mdelay(2); // 2ms delay between each complete value
	}

	return 0; // Success
}

/**
 * @brief Write default precision array to EEPROM
 *
 * This function writes the precision array to the default precision array memory position
 * using single-byte writes to avoid I2C bus blocking issues that can occur with large transfers.
 * Each int32_t value is broken down into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_def_precision_array(const int32_t *precision_array)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_PRECISION_ARRAY_POZ; // Starting address for default precision array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t

	if (!m24512_desc) {
		return -ENODEV;
	}

	if (!precision_array) {
		return -EINVAL; // Invalid parameter
	}

	// Write each int32_t value byte by byte
	// Array: 48 elements × 4 bytes each = 192 bytes total
	for (int i = 0; i < PRECISION_ARRAY_SIZE; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));
		uint32_t value = (uint32_t)precision_array[i];

		// Break down int32_t into bytes (little-endian format)
		byte_buffer[0] = (uint8_t)(value & 0xFF);
		byte_buffer[1] = (uint8_t)((value >> 8) & 0xFF);
		byte_buffer[2] = (uint8_t)((value >> 16) & 0xFF);
		byte_buffer[3] = (uint8_t)((value >> 24) & 0xFF);

		// Write 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_write(m24512_desc, current_address + byte_idx,
						 &byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Write failed
			}

			// Small delay between byte writes to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Optimized delay for M24512 precision array - typical write cycle is 2-3ms
		no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle
	}

	return 0; // Success
}

/**
 * @brief Read default precision array from EEPROM
 *
 * This function reads the precision array from the default precision array memory position
 * using single-byte reads to avoid I2C bus blocking issues that can occur with large transfers.
 * Each int32_t value is read byte by byte and reconstructed in little-endian format.
 */
int powrms_eeprom_read_def_precision_array(int32_t *precision_array)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_PRECISION_ARRAY_POZ; // Starting address for default precision array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t

	if (!m24512_desc) {
		return -ENODEV; // EEPROM not initialized
	}

	if (!precision_array) {
		return -EINVAL; // Invalid parameter
	}

	// Read each int32_t value byte by byte
	// Array: 48 elements × 4 bytes each = 192 bytes total
	for (int i = 0; i < PRECISION_ARRAY_SIZE; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));

		// Read 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_read(m24512_desc, current_address + byte_idx,
						&byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Read failed
			}

			// Small delay between byte reads to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Reconstruct int32_t from bytes (little-endian format)
		precision_array[i] = (int32_t)((uint32_t)byte_buffer[0] |
					       ((uint32_t)byte_buffer[1] << 8) |
					       ((uint32_t)byte_buffer[2] << 16) |
					       ((uint32_t)byte_buffer[3] << 24));

		// Longer delay between int32_t reads to allow I2C bus recovery
		no_os_mdelay(2); // 2ms delay between each complete value
	}

	return 0; // Success
}

/**
 * @brief Write default temperature correction array to EEPROM
 *
 * This function writes the temperature correction array to the default temperature correction array memory position
 * using single-byte writes to avoid I2C bus blocking issues that can occur with large transfers.
 * Each int32_t value is broken down into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_def_temp_corr_array(const int32_t *temp_corr_array)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_TEMP_COMP_ARRAY_POZ; // Starting address for default temperature correction array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t
	int array_size = TEMPERATURE_CORRECTION_COEFFS *
			 FREQUENCY_RANGE_NR; // 3*8 = 24 values

	if (!m24512_desc) {
		return -ENODEV;
	}

	if (!temp_corr_array) {
		return -EINVAL; // Invalid parameter
	}

	// Write each int32_t value byte by byte
	// Array: 24 elements × 4 bytes each = 96 bytes total
	for (int i = 0; i < array_size; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));

		// Convert int32_t to bytes (little-endian format)
		byte_buffer[0] = (uint8_t)(temp_corr_array[i] & 0xFF);
		byte_buffer[1] = (uint8_t)((temp_corr_array[i] >> 8) & 0xFF);
		byte_buffer[2] = (uint8_t)((temp_corr_array[i] >> 16) & 0xFF);
		byte_buffer[3] = (uint8_t)((temp_corr_array[i] >> 24) & 0xFF);

		// Write 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_write(m24512_desc, current_address + byte_idx,
						 &byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Write failed
			}

			// Small delay between byte writes to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Optimized delay for M24512 temperature array - typical write cycle is 2-3ms
		no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle
	}

	return 0; // Success
}

/**
 * @brief Read default temperature correction array from EEPROM
 *
 * This function reads the temperature correction array from the default temperature correction array memory position
 * using single-byte reads to avoid I2C bus blocking issues that can occur with large transfers.
 * Each int32_t value is read byte by byte and reconstructed in little-endian format.
 */
int powrms_eeprom_read_def_temp_corr_array(int32_t *temp_corr_array)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_TEMP_COMP_ARRAY_POZ; // Starting address for default temperature correction array
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of each int32_t
	int array_size = TEMPERATURE_CORRECTION_COEFFS *
			 FREQUENCY_RANGE_NR; // 3*8 = 24 values

	if (!m24512_desc) {
		return -ENODEV; // EEPROM not initialized
	}

	if (!temp_corr_array) {
		return -EINVAL; // Invalid parameter
	}

	// Read each int32_t value byte by byte
	// Array: 24 elements × 4 bytes each = 96 bytes total
	for (int i = 0; i < array_size; i++) {
		uint16_t current_address = base_address + (i * sizeof(int32_t));

		// Read 4 bytes for current int32_t value
		for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
			ret = no_os_eeprom_read(m24512_desc, current_address + byte_idx,
						&byte_buffer[byte_idx], 1);
			if (ret) {
				return ret; // Read failed
			}

			// Small delay between byte reads to prevent I2C bus saturation
			no_os_udelay(50); // 50 microseconds delay
		}

		// Reconstruct int32_t from bytes (little-endian format)
		temp_corr_array[i] = (int32_t)((uint32_t)byte_buffer[0] |
					       ((uint32_t)byte_buffer[1] << 8) |
					       ((uint32_t)byte_buffer[2] << 16) |
					       ((uint32_t)byte_buffer[3] << 24));

		// Longer delay between int32_t reads to allow I2C bus recovery
		no_os_mdelay(2); // 2ms delay between each complete value
	}

	return 0; // Success
}

/**
 * @brief Write voltage temperature compensation value to EEPROM
 *
 * This function writes the voltage temperature compensation value using single-byte writes to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is broken down
 * into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_v_temp_comp_val(int32_t value)
{
	int ret;
	uint16_t base_address =
		MEM_V_TEMP_COMP_VAL_POZ; // Address for voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc) {
		return -ENODEV;
	}

	// Convert int32_t to bytes (little-endian format)
	byte_buffer[0] = (uint8_t)(value & 0xFF);
	byte_buffer[1] = (uint8_t)((value >> 8) & 0xFF);
	byte_buffer[2] = (uint8_t)((value >> 16) & 0xFF);
	byte_buffer[3] = (uint8_t)((value >> 24) & 0xFF);

	// Write 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_write(m24512_desc, base_address + byte_idx,
					 &byte_buffer[byte_idx], 1);
		if (ret) {
			return ret; // Write failed
		}

		// Small delay between byte writes to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Optimized delay for M24512 - typical write cycle is 2-3ms
	no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle

	return 0; // Success
}

/**
 * @brief Read voltage temperature compensation value from EEPROM
 *
 * This function reads the voltage temperature compensation value using single-byte reads to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is read byte by byte
 * and reconstructed in little-endian format.
 */
int powrms_eeprom_read_v_temp_comp_val(int32_t *value)
{
	int ret;
	uint16_t base_address =
		MEM_V_TEMP_COMP_VAL_POZ; // Address for voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc) {
		return -ENODEV; // EEPROM not initialized
	}

	if (!value) {
		return -EINVAL; // Invalid parameter
	}

	// Read 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_read(m24512_desc, base_address + byte_idx,
					&byte_buffer[byte_idx], 1);
		if (ret) {
			return ret; // Read failed
		}

		// Small delay between byte reads to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Reconstruct int32_t from bytes (little-endian format)
	*value = (int32_t)((uint32_t)byte_buffer[0] |
			   ((uint32_t)byte_buffer[1] << 8) |
			   ((uint32_t)byte_buffer[2] << 16) |
			   ((uint32_t)byte_buffer[3] << 24));

	return 0; // Success
}

/**
 * @brief Write default voltage temperature compensation value to EEPROM
 *
 * This function writes the default voltage temperature compensation value using single-byte writes to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is broken down
 * into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_def_v_temp_comp_val(int32_t value)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_V_TEMP_COMP_VAL_POZ; // Address for default voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc) {
		return -ENODEV;
	}

	// Convert int32_t to bytes (little-endian format)
	byte_buffer[0] = (uint8_t)(value & 0xFF);
	byte_buffer[1] = (uint8_t)((value >> 8) & 0xFF);
	byte_buffer[2] = (uint8_t)((value >> 16) & 0xFF);
	byte_buffer[3] = (uint8_t)((value >> 24) & 0xFF);

	// Write 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_write(m24512_desc, base_address + byte_idx,
					 &byte_buffer[byte_idx], 1);
		if (ret) {
			return ret; // Write failed
		}

		// Small delay between byte writes to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Optimized delay for M24512 - typical write cycle is 2-3ms
	no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle

	return 0; // Success
}

/**
 * @brief Read default voltage temperature compensation value from EEPROM
 *
 * This function reads the default voltage temperature compensation value using single-byte reads to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is read byte by byte
 * and reconstructed in little-endian format.
 */
int powrms_eeprom_read_def_v_temp_comp_val(int32_t *value)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_V_TEMP_COMP_VAL_POZ; // Address for default voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc) {
		return -ENODEV; // EEPROM not initialized
	}

	if (!value) {
		return -EINVAL; // Invalid parameter
	}

	// Read 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_read(m24512_desc, base_address + byte_idx,
					&byte_buffer[byte_idx], 1);
		if (ret) {
			return ret; // Read failed
		}

		// Small delay between byte reads to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Reconstruct int32_t from bytes (little-endian format)
	*value = (int32_t)((uint32_t)byte_buffer[0] |
			   ((uint32_t)byte_buffer[1] << 8) |
			   ((uint32_t)byte_buffer[2] << 16) |
			   ((uint32_t)byte_buffer[3] << 24));

	return 0; // Success
}

#ifdef MAXIM_PLATFORM
#include "wdt.h"
#include "nvic_table.h"
#include "mxc_sys.h"

// Static variable to track watchdog state

void WDT_IRQHandler(void)
{
	MXC_WDT_ClearIntFlag(MXC_WDT0);
	NVIC_SystemReset();
}


/**
 * @brief Initialize and configure the watchdog timer
 *
 * Configures the watchdog for a 5-second timeout period with interrupts enabled.
 * Uses windowed mode but with a very small lower window to effectively disable
 * the lower limit requirement.
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_watchdog_init(int msec)
{

	static mxc_wdt_cfg_t cfg;
	cfg.mode = MXC_WDT_COMPATIBILITY;

	MXC_WDT_Disable(MXC_WDT0);
	MXC_WDT_Init(MXC_WDT0, &cfg);

	MXC_WDT_ResetTimer(MXC_WDT0);

	MXC_WDT_SetClockSource(MXC_WDT0, MXC_WDT_NANO_CLK); //80kHz

	// Calculate the required period based on msec input
	// Clock frequency is 80kHz (80,000 Hz)
	// Period = msec / 1000 * 80000 = msec * 80
	uint32_t required_ticks = (uint32_t)msec * 80;

	// Find the closest power of 2 period that accommodates the required timeout
	mxc_wdt_period_t reset_period = MXC_WDT_PERIOD_2_23; // Default to shortest
	mxc_wdt_period_t int_period =
		MXC_WDT_PERIOD_2_22;   // Interrupt period slightly shorter

	// Calculate the appropriate period based on required ticks
	if (required_ticks <= (1 << 17)) {
		reset_period = MXC_WDT_PERIOD_2_17;
		int_period = MXC_WDT_PERIOD_2_16;
	} else if (required_ticks <= (1 << 18)) {
		reset_period = MXC_WDT_PERIOD_2_18;
		int_period = MXC_WDT_PERIOD_2_17;
	} else if (required_ticks <= (1 << 19)) {
		reset_period = MXC_WDT_PERIOD_2_19;
		int_period = MXC_WDT_PERIOD_2_18;
	} else if (required_ticks <= (1 << 20)) {
		reset_period = MXC_WDT_PERIOD_2_20;
		int_period = MXC_WDT_PERIOD_2_19;
	} else if (required_ticks <= (1 << 21)) {
		reset_period = MXC_WDT_PERIOD_2_21;
		int_period = MXC_WDT_PERIOD_2_20;
	} else if (required_ticks <= (1 << 22)) {
		reset_period = MXC_WDT_PERIOD_2_22;
		int_period = MXC_WDT_PERIOD_2_21;
	} else if (required_ticks <= (1 << 23)) {
		reset_period = MXC_WDT_PERIOD_2_23;
		int_period = MXC_WDT_PERIOD_2_22;
	} else {
		// Use maximum period if requested timeout is too large
		reset_period = MXC_WDT_PERIOD_2_23;
		int_period = MXC_WDT_PERIOD_2_22;
	}

	cfg.upperResetPeriod = reset_period;
	cfg.upperIntPeriod = int_period;
	// Lower periods are not used in compatibility mode
	cfg.lowerResetPeriod = 0;
	cfg.lowerIntPeriod = 0;

	MXC_WDT_SetResetPeriod(MXC_WDT0, &cfg);
	MXC_WDT_SetIntPeriod(MXC_WDT0, &cfg);

	MXC_WDT_EnableReset(MXC_WDT0);
	MXC_WDT_ClearIntFlag(MXC_WDT0);
	MXC_WDT_EnableInt(MXC_WDT0);
	NVIC_EnableIRQ(WDT_IRQn);

	MXC_WDT_ResetTimer(MXC_WDT0); //Feed the dog
	MXC_WDT_Enable(MXC_WDT0); //Re-enable WDT

	return 0;
}

/**
 * @brief Reset the watchdog timer (feed the dog)
 *
 * This function should be called periodically (more frequently than every 5 seconds)
 * to prevent the system from resetting.
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_watchdog_reset(void)
{
	// Feed the watchdog (reset the timer)
	MXC_WDT_ResetTimer(MXC_WDT0);
	return 0;
}

/**
 * @brief Disable the watchdog timer
 *
 * Completely disables the watchdog timer functionality.
 *
 * @return int 0 on success, negative error code on failure
 */
int powrms_watchdog_disable(void)
{
	// Disable the watchdog timer
	MXC_WDT_Disable(MXC_WDT0);

	return 0;
}

#endif /* MAXIM_PLATFORM */

void powrms_get_firmware_version(char* version_buf)
{
	if (!version_buf)
		return;

	version_buf[0] = '0' + FW_VERSION_MAJOR;
	version_buf[1] = '.';
	version_buf[2] = '0' + FW_VERSION_MINOR;
	version_buf[3] = '.';
	version_buf[4] = '0' + FW_VERSION_PATCH;
	version_buf[5] = '\0';
}

int powrms_init_memory_upon_boot()
{
	int ret;

	ret = no_os_eeprom_init(&m24512_desc, &eeprom_m24512_init_param);
	if (ret) {
		return ret;
	}

	// Initialize first byte of EEPROM (set to 1 if it was 0)
	int32_t use_def_calib_data = 1;

	ret = powrms_eeprom_get_use_def_cal_data(&use_def_calib_data);
	if (ret) {
		return ret;
	}

	no_os_udelay(50);

	if (use_def_calib_data) {
		// Use default calibration data from MEM_DEF_PRECISION_ARRAY_POZ
		no_os_udelay(50);
		ret = powrms_eeprom_read_def_precision_array(precision_values);
		if (ret) {
			return ret;
		}
		no_os_udelay(50);
		ret = powrms_eeprom_read_def_temp_corr_array(temperature_precision_values);
		if (ret) {
			return ret;
		}
		no_os_udelay(50);
		ret = powrms_eeprom_read_def_v_temp_comp_val(&temperature_compensation_value);
		if (ret) {
			return ret;
		}
	} else {
		// Use custom calibration data from MEM_PRECISION_ARRAY_POZ
		no_os_udelay(50);
		ret = powrms_eeprom_read_precision_array(precision_values);
		if (ret) {
			return ret;
		}
		no_os_udelay(50);
		ret = powrms_eeprom_read_temp_corr_array(temperature_precision_values);
		if (ret) {
			return ret;
		}
		no_os_udelay(50);
		ret = powrms_eeprom_read_v_temp_comp_val(&temperature_compensation_value);
		if (ret) {
			return ret;
		}
	}

	return 0;
}