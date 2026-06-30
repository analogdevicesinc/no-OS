/***************************************************************************//**
 *   @file   ltm4700.h
 *   @brief  Header file of the LTM4700 driver
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#ifndef __LTM4700_H__
#define __LTM4700_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"

/* Standard PMBus commands */
#define LTM4700_PAGE				0x00
#define LTM4700_OPERATION			0x01
#define LTM4700_ON_OFF_CONFIG			0x02
#define LTM4700_CLEAR_FAULTS			0x03
#define LTM4700_PAGE_PLUS_WRITE			0x05
#define LTM4700_PAGE_PLUS_READ			0x06

#define LTM4700_WRITE_PROTECT			0x10
#define LTM4700_STORE_USER_ALL			0x15
#define LTM4700_RESTORE_USER_ALL		0x16

#define LTM4700_CAPABILITY			0x19
#define LTM4700_SMBALERT_MASK			0x1B

#define LTM4700_VOUT_MODE			0x20
#define LTM4700_VOUT_COMMAND			0x21
#define LTM4700_VOUT_MAX			0x24
#define LTM4700_VOUT_MARGIN_HIGH		0x25
#define LTM4700_VOUT_MARGIN_LOW			0x26

#define LTM4700_VOUT_OV_FAULT_LIMIT		0x40
#define LTM4700_VOUT_OV_FAULT_RESPONSE		0x41
#define LTM4700_VOUT_OV_WARN_LIMIT		0x42
#define LTM4700_VOUT_UV_WARN_LIMIT		0x43
#define LTM4700_VOUT_UV_FAULT_LIMIT		0x44
#define LTM4700_VOUT_UV_FAULT_RESPONSE		0x45
#define LTM4700_IOUT_OC_FAULT_LIMIT		0x46
#define LTM4700_IOUT_OC_FAULT_RESPONSE		0x47
#define LTM4700_IOUT_OC_WARN_LIMIT		0x4A

#define LTM4700_TON_DELAY			0x60
#define LTM4700_TOFF_DELAY			0x64

#define LTM4700_STATUS_BYTE			0x78
#define LTM4700_STATUS_WORD			0x79
#define LTM4700_STATUS_VOUT			0x7A
#define LTM4700_STATUS_IOUT			0x7B
#define LTM4700_STATUS_INPUT			0x7C
#define LTM4700_STATUS_TEMPERATURE		0x7D
#define LTM4700_STATUS_CML			0x7E
#define LTM4700_STATUS_MFR_SPECIFIC		0x80

#define LTM4700_READ_VIN			0x88
#define LTM4700_READ_IIN			0x89
#define LTM4700_READ_VOUT			0x8B
#define LTM4700_READ_IOUT			0x8C
#define LTM4700_READ_TEMPERATURE_1		0x8D
#define LTM4700_READ_TEMPERATURE_2		0x8E
#define LTM4700_READ_FREQUENCY			0x95
#define LTM4700_READ_POUT			0x96
#define LTM4700_READ_PIN			0x97

#define LTM4700_MFR_ID				0x99
#define LTM4700_MFR_MODEL			0x9A
#define LTM4700_MFR_VOUT_MAX			0xA5
#define LTM4700_MFR_PIN_ACCURACY		0xAC

/* Manufacturer specific commands */
#define LTM4700_MFR_EE_UNLOCK			0xBD
#define LTM4700_MFR_EE_ERASE			0xBE
#define LTM4700_MFR_EE_DATA			0xBF

#define LTM4700_MFR_CHAN_CONFIG			0xD0
#define LTM4700_MFR_CONFIG_ALL			0xD1
#define LTM4700_MFR_FAULT_PROPAGATE		0xD2
#define LTM4700_MFR_PWM_COMP			0xD3
#define LTM4700_MFR_PWM_MODE			0xD4
#define LTM4700_MFR_FAULT_RESPONSE		0xD5
#define LTM4700_MFR_OT_FAULT_RESPONSE		0xD6
#define LTM4700_MFR_IOUT_PEAK			0xD7
#define LTM4700_MFR_ADC_CONTROL			0xD8
#define LTM4700_MFR_RETRY_DELAY			0xDB
#define LTM4700_MFR_RESTART_DELAY		0xDC
#define LTM4700_MFR_VOUT_PEAK			0xDD
#define LTM4700_MFR_VIN_PEAK			0xDE
#define LTM4700_MFR_TEMPERATURE_1_PEAK		0xDF
#define LTM4700_MFR_READ_IIN_PEAK		0xE1
#define LTM4700_MFR_CLEAR_PEAKS			0xE3
#define LTM4700_MFR_READ_ICHIP			0xE4
#define LTM4700_MFR_PADS			0xE5
#define LTM4700_MFR_ADDRESS			0xE6
#define LTM4700_MFR_SPECIAL_ID			0xE7
#define LTM4700_MFR_IIN_CAL_GAIN		0xE8
#define LTM4700_MFR_FAULT_LOG_STORE		0xEA
#define LTM4700_MFR_FAULT_LOG_CLEAR		0xEC
#define LTM4700_MFR_FAULT_LOG			0xEE
#define LTM4700_MFR_COMMON			0xEF
#define LTM4700_MFR_COMPARE_USER_ALL		0xF0
#define LTM4700_MFR_TEMPERATURE_2_PEAK		0xF4
#define LTM4700_MFR_PWM_CONFIG			0xF5
#define LTM4700_MFR_IOUT_CAL_GAIN_TC		0xF6
#define LTM4700_MFR_RVIN			0xF7
#define LTM4700_MFR_TEMP_1_GAIN			0xF8
#define LTM4700_MFR_TEMP_1_OFFSET		0xF9
#define LTM4700_MFR_RAIL_ADDRESS		0xFA
#define LTM4700_MFR_REAL_TIME			0xFB
#define LTM4700_MFR_RESET			0xFD

/* PMBus-specific parameters */
#define LTM4700_CRC_POLYNOMIAL			0x7
#define LTM4700_VOUT_MODE_VAL_MSK		NO_OS_GENMASK(4,0)

/* LINEAR data format params - VOUT uses L16 with exponent -12 */
#define LTM4700_LIN11_MANTISSA_MAX		1023L
#define LTM4700_LIN11_MANTISSA_MIN		(-1024L)
#define LTM4700_LIN11_EXPONENT_MAX		15
#define LTM4700_LIN11_EXPONENT_MIN		(-15)
#define LTM4700_LIN11_MANTISSA_MSK		NO_OS_GENMASK(10,0)
#define LTM4700_LIN11_EXPONENT_MSK		NO_OS_GENMASK(15,11)
#define LTM4700_LIN11_EXPONENT(x)		((int16_t)(x) >> 11)
#define LTM4700_LIN11_MANTISSA(x)		(((int16_t)((x & LTM4700_LIN11_MANTISSA_MSK) << 5)) >> 5)
#define LTM4700_LIN16_EXPONENT			(-12)

/* LTM4700 channel numbers */
#define LTM4700_CHAN_0				0x0
#define LTM4700_CHAN_1				0x1
#define LTM4700_CHAN_ALL			0xFF

/* Status types masks */
#define LTM4700_STATUS_BYTE_TYPE_MSK		NO_OS_BIT(0)
#define LTM4700_STATUS_VOUT_TYPE_MSK		NO_OS_BIT(1)
#define LTM4700_STATUS_IOUT_TYPE_MSK		NO_OS_BIT(2)
#define LTM4700_STATUS_INPUT_TYPE_MSK		NO_OS_BIT(3)
#define LTM4700_STATUS_TEMP_TYPE_MSK		NO_OS_BIT(4)
#define LTM4700_STATUS_CML_TYPE_MSK		NO_OS_BIT(5)
#define LTM4700_STATUS_MFR_SPECIFIC_TYPE_MSK	NO_OS_BIT(6)
#define LTM4700_STATUS_WORD_TYPE_MSK		NO_OS_BIT(7)
#define LTM4700_STATUS_ALL_TYPE_MSK		0xFF

/* LTM4700 ID values */
#define LTM4700_ID_MSK				0xFFF0
#define LTM4700_SPECIAL_ID_VALUE		0x4130
#define LTM4700_MFR_ID_VALUE			"ADI"
#define LTM4700_MFR_ID_SIZE			3
#define LTM4700_MFR_MODEL_VALUE			"LTM4700"
#define LTM4700_MFR_MODEL_SIZE			7

enum ltm4700_operation_type {
	LTM4700_OPERATION_OFF = 0x00,
	LTM4700_OPERATION_ON = 0x80,
	LTM4700_OPERATION_MARGIN_LOW = 0x98,
	LTM4700_OPERATION_MARGIN_HIGH = 0xA8,
	LTM4700_OPERATION_SEQ_OFF = 0x40,
};

enum ltm4700_value_type {
	LTM4700_VIN = LTM4700_READ_VIN,
	LTM4700_IIN = LTM4700_READ_IIN,
	LTM4700_VOUT = LTM4700_READ_VOUT,
	LTM4700_IOUT = LTM4700_READ_IOUT,
	LTM4700_TEMP_EXT = LTM4700_READ_TEMPERATURE_1,
	LTM4700_TEMP_IC = LTM4700_READ_TEMPERATURE_2,
	LTM4700_FREQ = LTM4700_READ_FREQUENCY,
	LTM4700_POUT = LTM4700_READ_POUT,
	LTM4700_PIN = LTM4700_READ_PIN,
	LTM4700_IOUT_PEAK = LTM4700_MFR_IOUT_PEAK,
	LTM4700_VOUT_PEAK = LTM4700_MFR_VOUT_PEAK,
	LTM4700_VIN_PEAK = LTM4700_MFR_VIN_PEAK,
	LTM4700_TEMP_PEAK = LTM4700_MFR_TEMPERATURE_1_PEAK,
};

enum ltm4700_status_type {
	LTM4700_STATUS_ALL_TYPE = LTM4700_STATUS_ALL_TYPE_MSK,
	LTM4700_STATUS_BYTE_TYPE = LTM4700_STATUS_BYTE_TYPE_MSK,
	LTM4700_STATUS_VOUT_TYPE = LTM4700_STATUS_VOUT_TYPE_MSK,
	LTM4700_STATUS_IOUT_TYPE = LTM4700_STATUS_IOUT_TYPE_MSK,
	LTM4700_STATUS_INPUT_TYPE = LTM4700_STATUS_INPUT_TYPE_MSK,
	LTM4700_STATUS_TEMP_TYPE = LTM4700_STATUS_TEMP_TYPE_MSK,
	LTM4700_STATUS_CML_TYPE = LTM4700_STATUS_CML_TYPE_MSK,
	LTM4700_STATUS_MFR_SPECIFIC_TYPE = LTM4700_STATUS_MFR_SPECIFIC_TYPE_MSK,
	LTM4700_STATUS_WORD_TYPE = LTM4700_STATUS_WORD_TYPE_MSK,
};

enum ltm4700_timing_type {
	LTM4700_TON_DELAY_TYPE = LTM4700_TON_DELAY,
	LTM4700_TOFF_DELAY_TYPE = LTM4700_TOFF_DELAY,
	LTM4700_RETRY_DELAY = LTM4700_MFR_RETRY_DELAY,
	LTM4700_RESTART_DELAY = LTM4700_MFR_RESTART_DELAY,
};

enum ltm4700_nvm_cmd_type {
	LTM4700_STORE_USER = LTM4700_STORE_USER_ALL,
	LTM4700_RESTORE_USER = LTM4700_RESTORE_USER_ALL,
	LTM4700_COMPARE_USER = LTM4700_MFR_COMPARE_USER_ALL,
};

struct ltm4700_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *alert_desc;
	struct no_os_gpio_desc **pgood_descs;
	struct no_os_gpio_desc **run_descs;
	struct no_os_gpio_desc **fault_descs;

	int page;
	int lin16_exp;
	bool crc_en;
	uint8_t num_channels;
};

struct ltm4700_init_param {
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_gpio_init_param *alert_param;
	struct no_os_gpio_init_param **pgood_params;
	struct no_os_gpio_init_param **run_params;
	struct no_os_gpio_init_param **fault_params;

	bool crc_en;
};

struct ltm4700_status {
	uint16_t word;
	uint8_t byte;
	uint8_t vout;
	uint8_t iout;
	uint8_t input;
	uint8_t temp;
	uint8_t cml;
	uint8_t mfr_specific;
};

/**
 * @brief Initialize the LTM4700 device structure
 * @param device - Pointer to the device structure pointer
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_init(struct ltm4700_dev **device,
		 struct ltm4700_init_param *init_param);

/**
 * @brief Free or remove device instance
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_remove(struct ltm4700_dev *dev);

/**
 * @brief Set PMBus page and phase
 * @param dev - Device structure
 * @param page - Page number (0 or 1)
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_set_page(struct ltm4700_dev *dev, int page);

/**
 * @brief Send a PMBus command to the device
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_send_byte(struct ltm4700_dev *dev, int page, uint8_t cmd);

/**
 * @brief Perform a PMBus read_byte operation
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @param data - Pointer to store read data
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_byte(struct ltm4700_dev *dev, int page,
		      uint8_t cmd, uint8_t *data);

/**
 * @brief Perform a PMBus write_byte operation
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @param value - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_write_byte(struct ltm4700_dev *dev, int page,
		       uint8_t cmd, uint8_t value);

/**
 * @brief Perform a PMBus read_word operation
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @param word - Pointer to store read word
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_word(struct ltm4700_dev *dev, int page,
		      uint8_t cmd, uint16_t *word);

/**
 * @brief Perform a PMBus write_word operation
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @param word - Word value to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_write_word(struct ltm4700_dev *dev, int page,
		       uint8_t cmd, uint16_t word);

/**
 * @brief Perform a PMBus read_word operation then perform conversion
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @param data - Pointer to store converted data
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_word_data(struct ltm4700_dev *dev, int page,
			   uint8_t cmd, int *data);

/**
 * @brief Perform conversion then perform a PMBus write_word operation
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @param data - Data value to convert and write
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_write_word_data(struct ltm4700_dev *dev, int page,
			    uint8_t cmd, int data);

/**
 * @brief Read a block of bytes from device
 * @param dev - Device structure
 * @param page - Page number
 * @param cmd - Command byte
 * @param data - Buffer to store read data
 * @param nbytes - Number of bytes to read
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_block_data(struct ltm4700_dev *dev, int page, uint8_t cmd,
			    uint8_t *data, size_t nbytes);

/**
 * @brief Read specific value type (voltage, current, power, temperature)
 * @param dev - Device structure
 * @param channel - Channel number (0 or 1)
 * @param value_type - Type of value to read
 * @param value - Pointer to store read value
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_value(struct ltm4700_dev *dev,
		       uint8_t channel,
		       enum ltm4700_value_type value_type,
		       int *value);

/**
 * @brief Read status information from device
 * @param dev - Device structure
 * @param channel - Channel number (0 or 1)
 * @param status_type - Type of status to read
 * @param status - Pointer to store status information
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_status(struct ltm4700_dev *dev,
			uint8_t channel,
			enum ltm4700_status_type status_type,
			struct ltm4700_status *status);

/**
 * @brief Set VOUT parameters: VOUT_COMMAND and VOUT_MAX
 * @param dev - Device structure
 * @param channel - Channel number (0 or 1)
 * @param vout_command - Output voltage command in microvolts
 * @param vout_max - Maximum output voltage in microvolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_vout_value(struct ltm4700_dev *dev, uint8_t channel,
		       int vout_command, int vout_max);

/**
 * @brief Set VOUT margins for overvoltage/undervoltage protection
 * @param dev - Device structure
 * @param channel - Channel number (0 or 1)
 * @param margin_low - Low margin voltage in microvolts
 * @param margin_high - High margin voltage in microvolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_vout_margin(struct ltm4700_dev *dev, uint8_t channel,
			int margin_low, int margin_high);

/**
 * @brief Set timing values in microseconds
 * @param dev - Device structure
 * @param channel - Channel number (0 or 1)
 * @param timing_type - Type of timing parameter to set
 * @param time - Time value in microseconds
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_set_timing(struct ltm4700_dev *dev, uint8_t channel,
		       enum ltm4700_timing_type timing_type, int time);

/**
 * @brief Set operation mode for specified channel
 * @param dev - Device structure
 * @param channel - Channel number (0 or 1)
 * @param operation - Operation type to set
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_set_operation(struct ltm4700_dev *dev, uint8_t channel,
			  enum ltm4700_operation_type operation);

/**
 * @brief Execute NVM/EEPROM user commands
 * @param dev - Device structure
 * @param cmd - NVM command type to execute
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_nvm_cmd(struct ltm4700_dev *dev, enum ltm4700_nvm_cmd_type cmd);

/**
 * @brief Perform software reset of the device
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_software_reset(struct ltm4700_dev *dev);

/**
 * @brief Clear all peak value tracking registers
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_clear_peaks(struct ltm4700_dev *dev);

/**
 * @brief Verify manufacturer ID
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int ltm4700_verify_manufacturer_id(struct ltm4700_dev *dev);

/**
 * @brief Verify manufacturer model
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int ltm4700_verify_manufacturer_model(struct ltm4700_dev *dev);

#endif /* __LTM4700_H__ */
