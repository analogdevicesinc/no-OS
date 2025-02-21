/*******************************************************************************
*   @file   lt7170.h
*   @brief  Header file of the LT7170 Driver
*   @authors Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __LT7170_H__
#define __LT7170_H__

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

/* PMBus commands */
#define LT7170_PAGE				0x00
#define LT7170_OPERATION			0x01
#define LT7170_ON_OFF_CONFIG			0x02
#define LT7170_CLEAR_FAULTS			0x03
#define LT7170_PAGE_PLUS_WRITE			0x05
#define LT7170_PAGE_PLUS_READ			0x06
#define LT7170_ZONE_CONFIG			0x07
#define LT7170_ZONE_ACTIVE			0x08

#define LT7170_WRITE_PROTECT			0x10

#define LT7170_STORE_USER_ALL			0x15
#define LT7170_RESTORE_USER_ALL			0x16

#define LT7170_CAPABILITY			0x19
#define LT7170_QUERY				0x1A
#define LT7170_SMBALERT_MASK			0x1B

#define LT7170_VOUT_MODE			0x20
#define LT7170_VOUT_COMMAND			0x21
#define LT7170_VOUT_MAX				0x24
#define LT7170_VOUT_MARGIN_HIGH			0x25
#define LT7170_VOUT_MARGIN_LOW			0x26
#define LT7170_VOUT_TRANSITION_RATE		0x27

#define LT7170_FREQUENCY_SWITCH			0x33
#define LT7170_VIN_ON				0x35
#define LT7170_VIN_OFF				0x36

#define LT7170_IOUT_CAL_OFFSET			0x39
#define LT7170_VOUT_OV_FAULT_LIMIT		0x40
#define LT7170_VOUT_OV_FAULT_RESPONSE		0x41
#define LT7170_VOUT_OV_WARN_LIMIT		0x42
#define LT7170_VOUT_UV_WARN_LIMIT		0x43
#define LT7170_VOUT_UV_FAULT_LIMIT		0x44
#define LT7170_VOUT_UV_FAULT_RESPONSE		0x45
#define LT7170_IOUT_OC_FAULT_RESPONSE		0x47
#define LT7170_IOUT_OC_WARN_LIMIT		0x4A

#define LT7170_OT_FAULT_LIMIT			0x4F
#define LT7170_OT_FAULT_RESPONSE		0x50
#define LT7170_OT_WARN_LIMIT			0x51

#define LT7170_VIN_OV_FAULT_RESPONSE		0x56
#define LT7170_VIN_UV_WARN_LIMIT		0x58

#define LT7170_TON_DELAY			0x60
#define LT7170_TON_RISE				0x61
#define LT7170_TON_MAX_FAULT_LIMIT		0x62
#define LT7170_TON_MAX_FAULT_RESPONSE		0x63
#define LT7170_TOFF_DELAY			0x64
#define LT7170_TOFF_FALL			0x65
#define LT7170_TOFF_MAX_WARN_LIMIT		0x66

#define LT7170_STATUS_BYTE			0x78
#define LT7170_STATUS_WORD			0x79
#define LT7170_STATUS_VOUT			0x7A
#define LT7170_STATUS_IOUT			0x7B
#define LT7170_STATUS_INPUT			0x7C
#define LT7170_STATUS_TEMPERATURE		0x7D
#define LT7170_STATUS_CML			0x7E
#define LT7170_STATUS_MFR_SPECIFIC		0x80

#define LT7170_READ_VIN				0x88
#define LT7170_READ_VOUT			0x8B
#define LT7170_READ_IOUT			0x8C
#define LT7170_READ_TEMPERATURE_1		0x8D
#define LT7170_READ_FREQUENCY			0x95

#define LT7170_REVISION				0x98
#define LT7170_MFR_ID				0x99
#define LT7170_MFR_SERIAL			0x9E

#define LT7170_IC_DEVICE_ID			0xAD
#define LT7170_IC_DEVICE_REV			0xAE

#define LT7170_MFR_NVM_UNLOCK			0xBD
#define LT7170_MFR_NVM_USER_WRITES_REMAINING	0xBE
#define LT7170_MFR_NVM_DATA			0xBF
#define LT7170_MFR_USER_DATA_00			0xC9
#define LT7170_MFR_USER_DATA_01			0xCA
#define LT7170_MFR_READ_EXTVCC			0xCD
#define LT7170_MFR_READ_ITH			0xCE
#define LT7170_MFR_CHAN_CONFIG			0xD0
#define LT7170_MFR_CONFIG_ALL			0xD1
#define LT7170_MFR_FAULT_PROPAGATE		0xD2
#define LT7170_MFR_READ_ASEL			0xD3
#define LT7170_MFR_PWM_MODE			0xD4
#define LT7170_MFR_FAULT_RESPONSE		0xD5
#define LT7170_MFR_IOUT_PEAK			0xD7
#define LT7170_MFR_ADC_CONTROL			0xD8
#define LT7170_MFR_RETRY_DELAY			0xDB
#define LT7170_MFR_VOUT_PEAK			0xDD
#define LT7170_MFR_VIN_PEAK			0xDE
#define LT7170_MFR_TEMPERATURE_1_PEAK		0xDF
#define LT7170_MFR_READ_PWM_CFG			0xE0
#define LT7170_MFR_READ_VOUT_CFG		0xE1
#define LT7170_MFR_CLEAR_PEAKS			0xE3
#define LT7170_MFR_DISCHARGE_THRESHOLD		0xE4
#define LT7170_MFR_PADS				0xE5
#define LT7170_MFR_I2C_ADDRESS			0xE6
#define LT7170_MFR_SPECIAL_ID			0xE7
#define LT7170_MFR_COMMON			0xEF
#define LT7170_MFR_COMPARE_USER_ALL		0xF0
#define LT7170_MFR_CHANNEL_STATE		0xF1
#define LT7170_MFR_PGOOD_DELAY			0xF2
#define LT7170_MFR_NOT_PGOOD_DELAY		0xF3
#define LT7170_MFR_PWM_PHASE			0xF5
#define LT7170_MFR_SYNC_CONFIG			0xF6
#define LT7170_MFR_PIN_CONFIG_STATUS		0xF7
#define LT7170_MFR_RAIL_ADDRESS			0xFA
#define LT7170_MFR_DISABLE_OUTPUT		0xFB
#define LT7170_MFR_NVM_USER_WP			0xFC
#define LT7170_MFR_RESET			0xFD

/* PMBus-specific parameters */
#define LT7170_CRC_POLYNOMIAL			0x7
#define LT7170_VOUT_MODE_VAL_MSK		NO_OS_GENMASK(4,0)

/* IEEE754 data format params */
#define LT7170_IEEE754_SIGN_BIT			NO_OS_BIT(15)
#define LT7170_IEEE754_EXPONENT_MSK		NO_OS_GENMASK(14, 10)
#define LT7170_IEEE754_MANTISSA_MSK		NO_OS_GENMASK(9, 0)
#define LT7170_IEEE754_MAX_MANTISSA		0x7ff
#define LT7170_IEEE754_MIN_MANTISSA		0x400

/* Device specific constants */
#define LT7170_FREQ_MIN				400000
#define LT7170_FREQ_MAX				4000000
#define LT7170_VIN_ON_MIN			1400
#define LT7170_VIN_ON_MAX			16000
#define LT7170_VIN_OFF_MIN			1350
#define LT7170_VIN_OFF_MAX			16000
#define LT7170_PHASE_FULL_MILLI			360000

/* Status types masks */
#define LT7170_STATUS_BYTE_TYPE_MSK		0x01
#define LT7170_STATUS_VOUT_TYPE_MSK		0x02
#define LT7170_STATUS_IOUT_TYPE_MSK		0x04
#define LT7170_STATUS_INPUT_TYPE_MSK		0x08
#define LT7170_STATUS_TEMP_TYPE_MSK		0x10
#define LT7170_STATUS_CML_TYPE_MSK		0x20
#define LT7170_STATUS_MFR_SPECIFIC_TYPE_MSK	0x40
#define LT7170_STATUS_WORD_TYPE_MSK		0x80
#define LT7170_STATUS_ALL_TYPE_MSK		0xFF

/* LT7170 configurable bits and masks */
#define LT7170_PWM_OP_MODE_BIT			NO_OS_BIT(0)
#define LT7170_SYNC_CLK_INPUT_BIT		NO_OS_BIT(1)
#define LT7170_SYNC_CLK_OUTPUT_BIT		NO_OS_BIT(0)
#define LT7170_ADC_CTRL_LOW_FREQ_BIT		NO_OS_BIT(1)
#define LT7170_ADC_CTRL_DEBUG_BIT		NO_OS_BIT(0)
#define LT7170_CONFIG_ALL_PEC_BIT		NO_OS_BIT(2)
#define LT7170_PADS_RUN_BIT			NO_OS_BIT(2)

#define LT7170_SPECIAL_ID_VALUE			0x1C1D

enum lt7170_chip_id {
	ID_LT7170,
	ID_LT7170_1,
	ID_LT7171,
	ID_LT7171_1,
};

enum lt7170_fault_pin_config {
	LT7170_FAULT_PIN_INPUT,
	LT7170_FAULT_PIN_OUTPUT,
};

enum lt7170_operation_type {
	LT7170_OPERATION_OFF = 0x00,
	LT7170_OPERATION_ON = 0x80,
	LT7170_OPERATION_MARGIN_HIGH = 0x98,
	LT7170_OPERATION_MARGIN_LOW = 0xA8,
	LT7170_OPERATION_SEQ_OFF = 0x40,
};

enum lt7170_value_type {
	LT7170_VIN = LT7170_READ_VIN,
	LT7170_VOUT = LT7170_READ_VOUT,
	LT7170_IOUT = LT7170_READ_IOUT,
	LT7170_TEMP = LT7170_READ_TEMPERATURE_1,
	LT7170_FREQUENCY = LT7170_READ_FREQUENCY,
	LT7170_ITH = LT7170_MFR_READ_ITH,
	LT7170_EXTVCC = LT7170_MFR_READ_EXTVCC,
	LT7170_IOUT_PEAK = LT7170_MFR_IOUT_PEAK,
	LT7170_VOUT_PEAK = LT7170_MFR_VOUT_PEAK,
	LT7170_VIN_PEAK = LT7170_MFR_VIN_PEAK,
	LT7170_TEMP_PEAK = LT7170_MFR_TEMPERATURE_1_PEAK,
};

enum lt7170_limit_type {
	LT7170_VOUT_OV_FAULT_LIMIT_TYPE = LT7170_VOUT_OV_FAULT_LIMIT,
	LT7170_VOUT_OV_WARN_LIMIT_TYPE = LT7170_VOUT_OV_WARN_LIMIT,
	LT7170_VOUT_UV_FAULT_LIMIT_TYPE = LT7170_VOUT_UV_FAULT_LIMIT,
	LT7170_VOUT_UV_WARN_LIMIT_TYPE = LT7170_VOUT_UV_WARN_LIMIT,
	LT7170_IOUT_OC_WARN_LIMIT_TYPE = LT7170_IOUT_OC_WARN_LIMIT,
	LT7170_OT_FAULT_LIMIT_TYPE = LT7170_OT_FAULT_LIMIT,
	LT7170_OT_WARN_LIMIT_TYPE = LT7170_OT_WARN_LIMIT,
	LT7170_VIN_UV_WARN_LIMIT_TYPE = LT7170_VIN_UV_WARN_LIMIT,
	LT7170_TON_MAX_FAULT_LIMIT_TYPE = LT7170_TON_MAX_FAULT_LIMIT,
	LT7170_TOFF_MAX_WARN_LIMIT_TYPE = LT7170_TOFF_MAX_WARN_LIMIT,
};

enum lt7170_status_type {
	LT7170_STATUS_BYTE_TYPE = LT7170_STATUS_BYTE_TYPE_MSK,
	LT7170_STATUS_VOUT_TYPE = LT7170_STATUS_VOUT_TYPE_MSK,
	LT7170_STATUS_IOUT_TYPE = LT7170_STATUS_IOUT_TYPE_MSK,
	LT7170_STATUS_INPUT_TYPE = LT7170_STATUS_INPUT_TYPE_MSK,
	LT7170_STATUS_TEMP_TYPE = LT7170_STATUS_TEMP_TYPE_MSK,
	LT7170_STATUS_CML_TYPE = LT7170_STATUS_CML_TYPE_MSK,
	LT7170_STATUS_MFR_SPECIFIC_TYPE = LT7170_STATUS_MFR_SPECIFIC_TYPE_MSK,
	LT7170_STATUS_WORD_TYPE = LT7170_STATUS_WORD_TYPE_MSK,
	LT7170_STATUS_ALL_TYPE = LT7170_STATUS_ALL_TYPE_MSK,
};

enum lt7170_timing_type {
	LT7170_TON_DELAY_TYPE = LT7170_TON_DELAY,
	LT7170_TON_RISE_TYPE = LT7170_TON_RISE,
	LT7170_TOFF_DELAY_TYPE = LT7170_TOFF_DELAY,
	LT7170_TOFF_FALL_TYPE = LT7170_TOFF_FALL,
	LT7170_RETRY_DELAY = LT7170_MFR_RETRY_DELAY,
};

enum lt7170_nvm_cmd_type {
	LT7170_LOCK_USER,
	LT7170_UNLOCK_USER,
	LT7170_STORE_USER = LT7170_STORE_USER_ALL,
	LT7170_RESTORE_USER = LT7170_RESTORE_USER_ALL,
	LT7170_COMPARE_USER = LT7170_MFR_COMPARE_USER_ALL,
};

enum lt7170_pwm_mode {
	LT7170_PWM_FORCED_CONTINUOUS_MODE,
	LT7170_PWM_PULSE_SKIP_MODE,
};

struct lt7170_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *pg_desc;
	struct no_os_gpio_desc *run_desc;
	struct no_os_gpio_desc *alert_desc;
	struct no_os_gpio_desc *fault_desc;
	struct no_os_pwm_desc *sync_desc;

	enum lt7170_chip_id chip_id;
	bool crc_en;
};

struct lt7170_init_param {
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_gpio_init_param *pg_param;
	struct no_os_gpio_init_param *run_param;
	struct no_os_gpio_init_param *alert_param;
	struct no_os_gpio_init_param *fault_param;
	struct no_os_pwm_init_param *sync_param;

	enum lt7170_chip_id chip_id;
	enum lt7170_fault_pin_config fault_cfg;

	bool external_clk_en;
	bool crc_en;
};

struct lt7170_chip_info {
	char *name;
	uint8_t name_size;
};

struct lt7170_status {
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
int lt7170_init(struct lt7170_dev **dev,
		struct lt7170_init_param *init_param);

/* Free or remove device instance */
int lt7170_remove(struct lt7170_dev *dev);

/* Send a PMBus command to the device */
int lt7170_send_byte(struct lt7170_dev *dev, uint8_t cmd);

/* Perform a PMBus read_byte operation */
int lt7170_read_byte(struct lt7170_dev *dev,
		     uint8_t cmd, uint8_t *data);

/*  Perform a PMBus write_byte operation */
int lt7170_write_byte(struct lt7170_dev *dev,
		      uint8_t cmd, uint8_t value);

/* Perform a PMBus read_word operation */
int lt7170_read_word(struct lt7170_dev *dev,
		     uint8_t cmd, uint16_t *word);

/* Perform a PMBus write_word operation */
int lt7170_write_word(struct lt7170_dev *dev,
		      uint8_t cmd, uint16_t word);

/* Perform a PMBus read_word operation then perform conversion*/
int lt7170_read_word_data(struct lt7170_dev *dev,
			  uint8_t cmd, int *data);

/* Perform conversion then perform a PMBus write_word operation */
int lt7170_write_word_data(struct lt7170_dev *dev,
			   uint8_t cmd, int data);

/* Read a block of bytes */
int lt7170_read_block_data(struct lt7170_dev *dev,
			   uint8_t cmd, uint8_t *data, size_t nbytes);

/* Read specific value type */
int lt7170_read_value(struct lt7170_dev *dev,
		      enum lt7170_value_type value_type,
		      int *value);

/* Read status */
int lt7170_read_status(struct lt7170_dev *dev,
		       enum lt7170_status_type status_type,
		       struct lt7170_status *status);

/* Set VOUT parameters: VOUT_COMMAND and VOUT_MAX */
int lt7170_vout_value(struct lt7170_dev *dev,
		      int vout_command, int vout_max);

/* Set VOUT transition rate */
int lt7170_vout_tr(struct lt7170_dev *dev, int tr);

/* Set VOUT margins */
int lt7170_vout_margin(struct lt7170_dev *dev,
		       int margin_high, int margin_low);

/* Set VIN threshold when to start power conversion */
int lt7170_set_vin(struct lt7170_dev *dev,
		   int vin_on, int vin_off);

/* Set timing values */
int lt7170_set_timing(struct lt7170_dev *dev,
		      enum lt7170_timing_type timing_type, int time);

/* Set switching frequency */
int lt7170_switch_freq(struct lt7170_dev *dev, int freq);

/* Set PWM mode of a channel */
int lt7170_pwm_mode(struct lt7170_dev *dev,
		    enum lt7170_pwm_mode pwm_mode);

/* Set PWM phase offset of a channel */
int lt7170_pwm_phase(struct lt7170_dev *dev,
		     int phase);

/* Set fault/warning limit values */
int lt7170_set_limit(struct lt7170_dev *dev,
		     enum lt7170_limit_type limit, int limit_val);

/* Set operation */
int lt7170_set_operation(struct lt7170_dev *dev,
			 enum lt7170_operation_type operation);

/* Set channel state */
int lt7170_set_channel_state(struct lt7170_dev *dev,
			     bool state);

/* Set clock synchronization configs */
int lt7170_sync_config(struct lt7170_dev *dev,
		       bool input_clk,
		       bool output_clk);

/* Set ADC telemetry control */
int lt7170_adc_control(struct lt7170_dev *dev, bool low_freq_telemetry,
		       bool debug_telemetry);

/* NVM/EEPROM user commands */
int lt7170_nvm_cmd(struct lt7170_dev *dev, enum lt7170_nvm_cmd_type cmd);

/* Clear status registers */
int lt7170_clear_faults(struct lt7170_dev *dev);

/* Software reset */
int lt7170_software_reset(struct lt7170_dev *dev);

#endif /* __LT7170_H__ */
