/***************************************************************************//**
 *   @file   ltm4686.h
 *   @brief  Header file of the LTM4686 driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef __LTM4686_H__
#define __LTM4686_H__

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

/* PMBus commands */
#define LTM4686_PAGE				0x00
#define LTM4686_OPERATION			0x01
#define LTM4686_ON_OFF_CONFIG			0x02
#define LTM4686_CLEAR_FAULTS			0x03
#define LTM4686_PAGE_PLUS_WRITE			0x05
#define LTM4686_PAGE_PLUS_READ			0x06

#define LTM4686_WRITE_PROTECT			0x10
#define LTM4686_STORE_USER_ALL			0x15
#define LTM4686_RESTORE_USER_ALL		0x16

#define LTM4686_CAPABILITY			0x19
#define LTM4686_SMBALERT_MASK			0x1B

#define LTM4686_VOUT_MODE			0x20
#define LTM4686_VOUT_COMMAND			0x21
#define LTM4686_VOUT_MAX			0x24
#define LTM4686_VOUT_MARGIN_HIGH		0x25
#define LTM4686_VOUT_MARGIN_LOW			0x26
#define LTM4686_VOUT_TRANSITION_RATE		0x27

#define LTM4686_FREQUENCY_SWITCH		0x33
#define LTM4686_VIN_ON				0x35
#define LTM4686_VIN_OFF				0x36
#define LTM4686_IOUT_CAL_GAIN			0x38

#define LTM4686_VOUT_OV_FAULT_LIMIT		0x40
#define LTM4686_VOUT_OV_FAULT_RESPONSE		0x41
#define LTM4686_VOUT_OV_WARN_LIMIT		0x42
#define LTM4686_VOUT_UV_WARN_LIMIT		0x43
#define LTM4686_VOUT_UV_FAULT_LIMIT		0x44
#define LTM4686_VOUT_UV_FAULT_RESPONSE		0x45
#define LTM4686_IOUT_OC_FAULT_LIMIT		0x46
#define LTM4686_IOUT_OC_FAULT_RESPONSE		0x47
#define LTM4686_IOUT_OC_WARN_LIMIT		0x4A

#define LTM4686_OT_FAULT_LIMIT			0x4F
#define LTM4686_OT_FAULT_RESPONSE		0x50
#define LTM4686_OT_WARN_LIMIT			0x51
#define LTM4686_UT_FAULT_LIMIT			0x53
#define LTM4686_UT_FAULT_RESPONSE		0x54

#define LTM4686_VIN_OV_FAULT_LIMIT		0x55
#define LTM4686_VIN_OV_FAULT_RESPONSE		0x56
#define LTM4686_VIN_UV_WARN_LIMIT		0x58
#define LTM4686_IIN_OC_WARN_LIMIT		0x5D

#define LTM4686_TON_DELAY			0x60
#define LTM4686_TON_RISE			0x61
#define LTM4686_TON_MAX_FAULT_LIMIT		0x62
#define LTM4686_TON_MAX_FAULT_RESPONSE		0x63
#define LTM4686_TOFF_DELAY			0x64
#define LTM4686_TOFF_FALL			0x65
#define LTM4686_TOFF_MAX_WARN_LIMIT		0x66

#define LTM4686_STATUS_BYTE			0x78
#define LTM4686_STATUS_WORD			0x79
#define LTM4686_STATUS_VOUT			0x7A
#define LTM4686_STATUS_IOUT			0x7B
#define LTM4686_STATUS_INPUT			0x7C
#define LTM4686_STATUS_TEMPERATURE		0x7D
#define LTM4686_STATUS_CML			0x7E
#define LTM4686_STATUS_MFR_SPECIFIC		0x80

#define LTM4686_READ_VIN			0x88
#define LTM4686_READ_IIN			0x89
#define LTM4686_READ_VOUT			0x8B
#define LTM4686_READ_IOUT			0x8C
#define LTM4686_READ_TEMPERATURE_1		0x8D
#define LTM4686_READ_TEMPERATURE_2		0x8E
#define LTM4686_READ_DUTY_CYCLE			0x94
#define LTM4686_READ_POUT			0x96

#define LTM4686_REVISION			0x98
#define LTM4686_MFR_ID				0x99
#define LTM4686_MFR_MODEL			0x9A
#define LTM4686_MFR_SERIAL			0x9E
#define LTM4686_MFR_VOUT_MAX			0xA5

#define LTM4686_MFR_USER_DATA_00		0xB0
#define LTM4686_MFR_USER_DATA_01		0xB1
#define LTM4686_MFR_USER_DATA_02		0xB2
#define LTM4686_MFR_USER_DATA_03		0xB3
#define LTM4686_MFR_USER_DATA_04		0xB4

#define LTM4686_MFR_INFO			0xB6
#define LTM4686_MFR_EE_UNLOCK			0xBD
#define LTM4686_MFR_EE_ERASE			0xBE
#define LTM4686_MFR_EE_DATA			0xBF

#define LTM4686_MFR_CHAN_CONFIG			0xD0
#define LTM4686_MFR_CONFIG_ALL			0xD1
#define LTM4686_MFR_GPIO_PROPAGATE		0xD2
#define LTM4686_MFR_PWM_MODE			0xD4
#define LTM4686_MFR_GPIO_RESPONSE		0xD5
#define LTM4686_MFR_OT_FAULT_RESPONSE		0xD6
#define LTM4686_MFR_IOUT_PEAK			0xD7
#define LTM4686_MFR_ADC_CONTROL			0xD8
#define LTM4686_MFR_ADC_TELEMETRY_STATUS	0xDA
#define LTM4686_MFR_RETRY_DELAY			0xDB
#define LTM4686_MFR_RESTART_DELAY		0xDC
#define LTM4686_MFR_VOUT_PEAK			0xDD
#define LTM4686_MFR_VIN_PEAK			0xDE
#define LTM4686_MFR_TEMPERATURE_1_PEAK		0xDF
#define LTM4686_MFR_CLEAR_PEAKS			0xE3
#define LTM4686_MFR_PADS			0xE5
#define LTM4686_MFR_ADDRESS			0xE6
#define LTM4686_MFR_SPECIAL_ID			0xE7
#define LTM4686_MFR_IIN_OFFSET			0xE9
#define LTM4686_MFR_FAULT_LOG_STORE		0xEA
#define LTM4686_MFR_FAULT_LOG_CLEAR		0xEC
#define LTM4686_MFR_READ_IIN			0xED
#define LTM4686_MFR_FAULT_LOG			0xEE
#define LTM4686_MFR_COMMON			0xEF
#define LTM4686_MFR_COMPARE_USER_ALL		0xF0
#define LTM4686_MFR_TEMPERATURE_2_PEAK		0xF4
#define LTM4686_MFR_PWM_CONFIG			0xF5
#define LTM4686_MFR_IOUT_CAL_GAIN_TC		0xF6
#define LTM4686_MFR_TEMP_1_GAIN			0xF8
#define LTM4686_MFR_TEMP_1_OFFSET		0xF9
#define LTM4686_MFR_RAIL_ADDRESS		0xFA
#define LTM4686_MFR_RESET			0xFD

/* LTM4673 manufacturer-specific commands */
#define LTM4673_MFR_FAULTB0_PROPAGATE		0xD2
#define LTM4673_MFR_FAULTB1_PROPAGATE		0xD3
#define LTM4673_MFR_PWRGD_ENABLE		0xD4
#define LTM4673_MFR_FAULTB0_RESPONSE		0xD5
#define LTM4673_MFR_FAULTB1_RESPONSE		0xD6
#define LTM4673_MFR_IOUT_MIN			0xD8
#define LTM4673_MFR_CONFIG2			0xD9
#define LTM4673_MFR_CONFIG3			0xDA
#define LTM4673_MFR_DAC				0xE0
#define LTM4673_MFR_PGD_ASSERTION_DELAY		0xE1
#define LTM4673_MFR_WATCHDOG_T_FIRST		0xE2
#define LTM4673_MFR_WATCHDOG_T			0xE3
#define LTM4673_MFR_PAGE_FF_MASK		0xE4
#define LTM4673_MFR_IIN_CAL_GAIN		0xE8
#define LTM4673_MFR_VOUT_DISCHARGE_THRESHOLD	0xE9
#define LTM4673_MFR_FAULT_LOG_RESTORE		0xEB
#define LTM4673_MFR_FAULT_LOG_STATUS		0xED
#define LTM4673_MFR_RETRY_COUNT			0xF7
#define LTM4673_MFR_IOUT_SENSE_VOLTAGE		0xFA
#define LTM4673_MFR_VOUT_MIN			0xFB
#define LTM4673_MFR_VIN_MIN			0xFC
#define LTM4673_MFR_TEMPERATURE_1_MIN		0xFD

/* PMBus-specific parameters */
#define LTM4686_CRC_POLYNOMIAL			0x7
#define LTM4686_VOUT_MODE_VAL_MSK		NO_OS_GENMASK(4,0)

/* LINEAR data format params */
#define LTM4686_LIN11_MANTISSA_MAX		1023L
#define LTM4686_LIN11_MANTISSA_MIN		511L
#define LTM4686_LIN11_EXPONENT_MAX		15
#define LTM4686_LIN11_EXPONENT_MIN		-15
#define LTM4686_LIN11_MANTISSA_MSK		NO_OS_GENMASK(10,0)
#define LTM4686_LIN11_EXPONENT_MSK		NO_OS_GENMASK(15,11)
#define LTM4686_LIN11_EXPONENT(x)		((int16_t)(x) >> 11)
#define LTM4686_LIN11_MANTISSA(x)		(((int16_t)((x & 0x7FF) << 5)) >> 5)
#define LTM4686_LIN16_EXPONENT			-12

/* LTM4686 channel numbers */
#define LTM4686_CHAN_0				0x0
#define LTM4686_CHAN_1				0x1
#define LTM4686_CHAN_2				0x2
#define LTM4686_CHAN_3				0x3
#define LTM4686_CHAN_ALL			0xFF

/* Status types masks */
#define LTM4686_STATUS_BYTE_TYPE_MSK		0x01
#define LTM4686_STATUS_VOUT_TYPE_MSK		0x02
#define LTM4686_STATUS_IOUT_TYPE_MSK		0x04
#define LTM4686_STATUS_INPUT_TYPE_MSK		0x08
#define LTM4686_STATUS_TEMP_TYPE_MSK		0x10
#define LTM4686_STATUS_CML_TYPE_MSK		0x20
#define LTM4686_STATUS_MFR_SPECIFIC_TYPE_MSK	0x40
#define LTM4686_STATUS_WORD_TYPE_MSK		0x80
#define LTM4686_STATUS_ALL_TYPE_MSK		0xFF

/* LTM4686 configurable bits and masks */
#define LTM4686_PWM_OP_MODE_BIT			NO_OS_BIT(0)
#define LTM4686_CONFIG_ALL_DIS_SYNC_BIT		NO_OS_BIT(4)
#define LTM4686_CONFIG_ALL_PEC_BIT		NO_OS_BIT(2)

/* LTM4686 ID values */
#define LTM4686_ID_MSK				0xFFF0
#define LTM4686_SPECIAL_ID_VALUE		0x4770
#define LTM4673_SPECIAL_ID_VALUE_REV_1		0x0230
#define LTM4673_SPECIAL_ID_VALUE_REV_2		0x4480

enum ltm4686_chip_id {
	ID_LTM4686,
	ID_LTM4686B,
	ID_LTM4673,
};

enum ltm4686_fault_pin_config {
	LTM4686_FAULT_PIN_INPUT,
	LTM4686_FAULT_PIN_OUTPUT,
};

enum ltm4686_operation_type {
	LTM4686_OPERATION_OFF = 0x00,
	LTM4686_OPERATION_ON = 0x80,
	LTM4686_OPERATION_MARGIN_LOW = 0x98,
	LTM4686_OPERATION_MARGIN_HIGH = 0xA8,
	LTM4686_OPERATION_SEQ_OFF = 0x40,
};

enum ltm4686_value_type {
	LTM4686_VIN = LTM4686_READ_VIN,
	LTM4686_IIN = LTM4686_READ_IIN,
	LTM4686_VOUT = LTM4686_READ_VOUT,
	LTM4686_IOUT = LTM4686_READ_IOUT,
	LTM4686_TEMP_TSNS = LTM4686_READ_TEMPERATURE_1,
	LTM4686_TEMP_IC = LTM4686_READ_TEMPERATURE_2,
	LTM4686_DUTY_CYCLE = LTM4686_READ_DUTY_CYCLE,
	LTM4686_POUT = LTM4686_READ_POUT,
	LTM4686_IOUT_PEAK = LTM4686_MFR_IOUT_PEAK,
	LTM4686_VOUT_PEAK = LTM4686_MFR_VOUT_PEAK,
	LTM4686_VIN_PEAK = LTM4686_MFR_VIN_PEAK,
	LTM4686_TEMP_PEAK = LTM4686_MFR_TEMPERATURE_1_PEAK,
};

enum ltm4686_status_type {
	LTM4686_STATUS_ALL_TYPE = LTM4686_STATUS_ALL_TYPE_MSK,
	LTM4686_STATUS_BYTE_TYPE = LTM4686_STATUS_BYTE_TYPE_MSK,
	LTM4686_STATUS_VOUT_TYPE = LTM4686_STATUS_VOUT_TYPE_MSK,
	LTM4686_STATUS_IOUT_TYPE = LTM4686_STATUS_IOUT_TYPE_MSK,
	LTM4686_STATUS_INPUT_TYPE = LTM4686_STATUS_INPUT_TYPE_MSK,
	LTM4686_STATUS_TEMP_TYPE = LTM4686_STATUS_TEMP_TYPE_MSK,
	LTM4686_STATUS_CML_TYPE = LTM4686_STATUS_CML_TYPE_MSK,
	LTM4686_STATUS_MFR_SPECIFIC_TYPE = LTM4686_STATUS_MFR_SPECIFIC_TYPE_MSK,
	LTM4686_STATUS_WORD_TYPE = LTM4686_STATUS_WORD_TYPE_MSK,
};

enum ltm4686_timing_type {
	LTM4686_TON_DELAY_TYPE = LTM4686_TON_DELAY,
	LTM4686_TON_RISE_TYPE = LTM4686_TON_RISE,
	LTM4686_TOFF_DELAY_TYPE = LTM4686_TOFF_DELAY,
	LTM4686_TOFF_FALL_TYPE = LTM4686_TOFF_FALL,
	LT7182_RETRY_DELAY = LTM4686_MFR_RETRY_DELAY,
	LTM4686_RESTART_DELAY = LTM4686_MFR_RESTART_DELAY,
};

enum ltm4686_freq {
	LTM4686_FREQ_EXT_OSC,
	LTM4686_FREQ_250_KHZ = 0xF3E8,
	LTM4686_FREQ_350_KHZ = 0xFABC,
	LTM4686_FREQ_425_KHZ = 0xFB52,
	LTM4686_FREQ_500_KHZ = 0xFBE8,
	LTM4686_FREQ_575_KHZ = 0x023F,
	LTM4686_FREQ_650_KHZ = 0x028A,
	LTM4686_FREQ_750_KHZ = 0x02EE,
	LTM4686_FREQ_1000_KHZ = 0x03E8,
};

enum ltm4686_nvm_cmd_type {
	LTM4686_STORE_USER = LTM4686_STORE_USER_ALL,
	LTM4686_RESTORE_USER = LTM4686_RESTORE_USER_ALL,
	LTM4686_COMPARE_USER = LTM4686_MFR_COMPARE_USER_ALL,
};

enum ltm4686_pwm_mode {
	LTM4686_PWM_FORCED_CONTINUOUS_MODE,
	LTM4686_PWM_PULSE_SKIP_MODE,
};

struct ltm4686_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *alert_desc;
	struct no_os_gpio_desc **pgood_descs;
	struct no_os_gpio_desc **run_descs;
	struct no_os_gpio_desc **fault_descs;

	enum ltm4686_chip_id id;
	int page;
	int lin16_exp;
	bool crc_en;
	uint8_t num_channels;
};

struct ltm4686_init_param {
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_gpio_init_param *alert_param;
	struct no_os_gpio_init_param **pgood_params;
	struct no_os_gpio_init_param **run_params;
	struct no_os_gpio_init_param **fault_params;

	enum ltm4686_chip_id id;
	bool external_clk_en;
	bool crc_en;
};

struct ltm4686_chip_info {
	uint8_t num_channels;
};

struct ltm4686_status {
	uint16_t word;
	uint8_t byte;
	uint8_t vout;
	uint8_t iout;
	uint8_t input;
	uint8_t temp;
	uint8_t cml;
	uint8_t mfr_specific;
};

/* Initialize the device structure */
int ltm4686_init(struct ltm4686_dev **device,
		 struct ltm4686_init_param *init_param);

/* Free or remove device instance */
int ltm4686_remove(struct ltm4686_dev *dev);

/* Set PMBus page and phase */
int ltm4686_set_page(struct ltm4686_dev *dev, int page);

/* Send a PMBus command to the device */
int ltm4686_send_byte(struct ltm4686_dev *dev, int page, uint8_t cmd);

/* Perform a PMBus read_byte operation */
int ltm4686_read_byte(struct ltm4686_dev *dev, int page,
		      uint8_t cmd, uint8_t *data);

/*  Perform a PMBus write_byte operation */
int ltm4686_write_byte(struct ltm4686_dev *dev, int page,
		       uint8_t cmd, uint8_t value);

/* Perform a PMBus read_word operation */
int ltm4686_read_word(struct ltm4686_dev *dev, int page,
		      uint8_t cmd, uint16_t *word);

/* Perform a PMBus write_word operation */
int ltm4686_write_word(struct ltm4686_dev *dev, int page,
		       uint8_t cmd, uint16_t word);

/* Perform a PMBus read_word operation then perform conversion*/
int ltm4686_read_word_data(struct ltm4686_dev *dev, int page,
			   uint8_t cmd, int *data);

/* Perform conversion then perform a PMBus write_word operation */
int ltm4686_write_word_data(struct ltm4686_dev *dev, int page,
			    uint8_t cmd, int data);

/* Read a block of bytes */
int ltm4686_read_block_data(struct ltm4686_dev *dev, int page, uint8_t cmd,
			    uint8_t *data, size_t nbytes);

/* Read specific value type */
int ltm4686_read_value(struct ltm4686_dev *dev,
		       uint8_t channel,
		       enum ltm4686_value_type value_type,
		       int *value);

/* Read status */
int ltm4686_read_status(struct ltm4686_dev *dev,
			uint8_t channel,
			enum ltm4686_status_type status_type,
			struct ltm4686_status *status);

/* Set VOUT parameters: VOUT_COMMAND and VOUT_MAX */
int ltm4686_vout_value(struct ltm4686_dev *dev, uint8_t channel,
		       int vout_command, int vout_max);

/* Set VOUT transition rate in microvolt per milliseconds */
int ltm4686_vout_tr(struct ltm4686_dev *dev, uint8_t channel, int tr);

/* Set VOUT margins */
int ltm4686_vout_margin(struct ltm4686_dev *dev, uint8_t channel,
			int margin_low, int margin_high);

/* Set VIN threshold when to start power conversion */
int ltm4686_set_vin(struct ltm4686_dev *dev, int vin_on, int vin_off);

/* Set timing values in microseconds */
int ltm4686_set_timing(struct ltm4686_dev *dev, uint8_t channel,
		       enum ltm4686_timing_type timing_type, int time);

/* Set switching frequency */
int ltm4686_switch_freq(struct ltm4686_dev *dev, enum ltm4686_freq freq);

/* Set PWM mode of a channel */
int ltm4686_pwm_mode(struct ltm4686_dev *dev, uint8_t channel,
		     enum ltm4686_pwm_mode pwm_mode);

/* Set operation */
int ltm4686_set_operation(struct ltm4686_dev *dev, uint8_t channel,
			  enum ltm4686_operation_type operation);

/* NVM/EEPROM user commands */
int ltm4686_nvm_cmd(struct ltm4686_dev *dev, enum ltm4686_nvm_cmd_type cmd);

/* Software reset */
int ltm4686_software_reset(struct ltm4686_dev *dev);

#endif /* __LTM4686_H__ */
