/*******************************************************************************
*   @file   lt7182s.h
*   @brief  Header file of the LT7182S Driver
*   @authors Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
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

#ifndef __LT7182S_H__
#define __LT7182S_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* PMBus commands */
#define LT7182S_PAGE				0x00
#define LT7182S_OPERATION			0x01
#define LT7182S_ON_OFF_CONFIG			0x02
#define LT7182S_CLEAR_FAULTS			0x03
#define LT7182S_PAGE_PLUS_WRITE			0x05
#define LT7182S_PAGE_PLUS_READ			0x06
#define LT7182S_ZONE_CONFIG			0x07
#define LT7182S_ZONE_ACTIVE			0x08

#define LT7182S_WRITE_PROTECT			0x10

#define LT7182S_STORE_USER_ALL			0x15
#define LT7182S_RESTORE_USER_ALL		0x16

#define LT7182S_CAPABILITY			0x19
#define LT7182S_QUERY				0x1A
#define LT7182S_SMBALERT_MASK			0x1B

#define LT7182S_VOUT_MODE			0x20
#define LT7182S_VOUT_COMMAND			0x21
#define LT7182S_VOUT_MAX			0x24
#define LT7182S_VOUT_MARGIN_HIGH		0x25
#define LT7182S_VOUT_MARGIN_LOW			0x26
#define LT7182S_VOUT_TRANSITION_RATE		0x27

#define LT7182S_FREQUENCY_SWITCH		0x33
#define LT7182S_VIN_ON				0x35
#define LT7182S_VIN_OFF				0x36

#define LT7182S_VOUT_OV_FAULT_LIMIT		0x40
#define LT7182S_VOUT_OV_FAULT_RESPONSE		0x41
#define LT7182S_VOUT_OV_WARN_LIMIT		0x42
#define LT7182S_VOUT_UV_WARN_LIMIT		0x43
#define LT7182S_VOUT_UV_FAULT_LIMIT		0x44
#define LT7182S_VOUT_UV_FAULT_RESPONSE		0x45
#define LT7182S_IOUT_OC_FAULT_RESPONSE		0x47
#define LT7182S_IOUT_OC_WARN_LIMIT		0x4A

#define LT7182S_OT_FAULT_LIMIT			0x4F
#define LT7182S_OT_FAULT_RESPONSE		0x50
#define LT7182S_OT_WARN_LIMIT			0x51

#define LT7182S_VIN_OV_FAULT_RESPONSE		0x56
#define LT7182S_VIN_UV_WARN_LIMIT		0x58
#define LT7182S_IIN_OC_WARN_LIMIT		0x5D

#define LT7182S_TON_DELAY			0x60
#define LT7182S_TON_RISE			0x61
#define LT7182S_TON_MAX_FAULT_LIMIT		0x62
#define LT7182S_TON_MAX_FAULT_RESPONSE		0x63
#define LT7182S_TOFF_DELAY			0x64
#define LT7182S_TOFF_FALL			0x65
#define LT7182S_TOFF_MAX_WARN_LIMIT		0x66

#define LT7182S_STATUS_BYTE			0x78
#define LT7182S_STATUS_WORD			0x79
#define LT7182S_STATUS_VOUT			0x7A
#define LT7182S_STATUS_IOUT			0x7B
#define LT7182S_STATUS_INPUT			0x7C
#define LT7182S_STATUS_TEMPERATURE		0x7D
#define LT7182S_STATUS_CML			0x7E
#define LT7182S_STATUS_MFR_SPECIFIC		0x80

#define LT7182S_READ_VIN			0x88
#define LT7182S_READ_IIN			0x89
#define LT7182S_READ_VOUT			0x8B
#define LT7182S_READ_IOUT			0x8C
#define LT7182S_READ_TEMPERATURE_1		0x8D
#define LT7182S_READ_FREQUENCY			0x95
#define LT7182S_READ_POUT			0x96

#define LT7182S_REVISION			0x98
#define LT7182S_MFR_ID				0x99
#define LT7182S_MFR_MODEL			0x9A
#define LT7182S_MFR_REVISION			0x9B
#define LT7182S_MFR_SERIAL			0x9E

#define LT7182S_IC_DEVICE_ID			0xAD
#define LT7182S_IC_DEVICE_REV			0xAE

#define LT7182S_MFR_USER_DATA_00		0xC9
#define LT7182S_MFR_USER_DATA_01		0xCA
#define LT7182S_MFR_USER_DATA_02		0xCB
#define LT7182S_MFR_USER_DATA_03		0xCC
#define LT7182S_MFR_READ_EXTVCC			0xCD
#define LT7182S_MFR_READ_ITH			0xCE
#define LT7182S_MFR_CHAN_CONFIG			0xD0
#define LT7182S_MFR_CONFIG_ALL			0xD1
#define LT7182S_MFR_FAULT_PROPAGATE		0xD2
#define LT7182S_MFR_PWM_MODE			0xD4
#define LT7182S_MFR_FAULT_RESPONSE		0xD5
#define LT7182S_MFR_IOUT_PEAK			0xD7
#define LT7182S_MFR_ADC_CONTROL			0xD8
#define LT7182S_MFR_RETRY_DELAY			0xDB
#define LT7182S_MFR_RESTART_DELAY		0xDC
#define LT7182S_MFR_VOUT_PEAK			0xDD
#define LT7182S_MFR_VIN_PEAK			0xDE
#define LT7182S_MFR_TEMPERATURE_1_PEAK		0xDF
#define LT7182S_MFR_CLEAR_PEAKS			0xE3
#define LT7182S_MFR_DISCHARGE_THRESHOLD		0xE4
#define LT7182S_MFR_PADS			0xE5
#define LT7182S_MFR_ADDRESS			0xE6
#define LT7182S_MFR_SPECIAL_ID			0xE7
#define LT7182S_MFR_FAULT_LOG_TIMESTAMP_MSBS	0xE8
#define LT7182S_MFR_FAULT_LOG_TIMESTAMP_LSBS	0xE9
#define LT7182S_MFR_FAULT_LOG_STORE		0xEA
#define LT7182S_MFR_FAULT_LOG_CLEAR		0xEC
#define LT7182S_MFR_FAULT_LOG			0xEE
#define LT7182S_MFR_COMMON			0xEF
#define LT7182S_MFR_COMPARE_USER_ALL		0xF0
#define LT7182S_MFR_CHANNEL_STATE		0xF1
#define LT7182S_MFR_PGOOD_DELAY			0xF2
#define LT7182S_MFR_NOT_PGOOD_DELAY		0xF3
#define LT7182S_MFR_PWM_PHASE			0xF5
#define LT7182S_MFR_SYNC_CONFIG			0xF6
#define LT7182S_MFR_PIN_CONFIG			0xF7
#define LT7182S_MFR_RAIL_ADDRESS		0xFA
#define LT7182S_MFR_DISABLE_OUTPUT		0xFB
#define LT7182S_MFR_EE_USER_WP			0xFC
#define LT7182S_MFR_RESET			0xFD

/* PMBus-specific parameters */
#define LT7182S_CRC_POLYNOMIAL			0x7
#define LT7182S_VOUT_MODE_VAL_MSK		NO_OS_GENMASK(4,0)

/* LINEAR data format params */
#define LT7182S_LIN11_MANTISSA_MAX		1023L
#define LT7182S_LIN11_MANTISSA_MIN		511L
#define LT7182S_LIN11_EXPONENT_MAX		15
#define LT7182S_LIN11_EXPONENT_MIN		-15
#define LT7182S_LIN11_MANTISSA_MSK		NO_OS_GENMASK(10,0)
#define LT7182S_LIN11_EXPONENT_MSK		NO_OS_GENMASK(15,11)
#define LT7182S_LIN11_EXPONENT(x)		(int16_t)(x) >> 11
#define LT7182S_LIN11_MANTISSA(x)		((int16_t)((x & 0x7FF) << 5)) >> 5
#define LT7182S_LIN16_EXPONENT			-12

/* IEEE754 data format params */
#define LT7182S_IEEE754_SIGN_BIT		NO_OS_BIT(15)
#define LT7182S_IEEE754_EXPONENT_MSK		NO_OS_GENMASK(14, 10)
#define LT7182S_IEEE754_MANTISSA_MSK		NO_OS_GENMASK(9, 0)
#define LT7182S_IEEE754_MAX_MANTISSA		0x7ff
#define LT7182S_IEEE754_MIN_MANTISSA		0x400

/* Device specific constants */
#define LT7182S_FREQ_MIN			400000
#define LT7182S_FREQ_MAX			4000000
#define LT7182S_VIN_ON_MIN			1400
#define LT7182S_VIN_ON_MAX			20000
#define LT7182S_VIN_OFF_MIN			1350
#define LT7182S_VIN_OFF_MAX			20000
#define LT7182S_PHASE_FULL_MILLI		360000

/* Status types masks */
#define LT7182S_STATUS_BYTE_TYPE_MSK		0x01
#define LT7182S_STATUS_VOUT_TYPE_MSK		0x02
#define LT7182S_STATUS_IOUT_TYPE_MSK		0x04
#define LT7182S_STATUS_INPUT_TYPE_MSK		0x08
#define LT7182S_STATUS_TEMP_TYPE_MSK		0x10
#define LT7182S_STATUS_CML_TYPE_MSK		0x20
#define LT7182S_STATUS_MFR_SPECIFIC_TYPE_MSK	0x40
#define LT7182S_STATUS_WORD_TYPE_MSK		0x80
#define LT7182S_STATUS_ALL_TYPE_MSK		0xFF

/* LT7182s configurable bits and masks */
#define LT7182S_PWM_OP_MODE_BIT			NO_OS_BIT(0)
#define LT7182S_SYNC_CLK_INPUT_BIT		NO_OS_BIT(1)
#define LT7182S_SYNC_CLK_OUTPUT_BIT		NO_OS_BIT(0)
#define LT7182S_ADC_CTRL_LOW_FREQ_BIT		NO_OS_BIT(1)
#define LT7182S_ADC_CTRL_DEBUG_BIT		NO_OS_BIT(0)
#define LT7182S_CONFIG_ALL_FORMAT_BIT		NO_OS_BIT(8)
#define LT7182S_CONFIG_ALL_PEC_BIT		NO_OS_BIT(2)
#define LT7182S_PADS_RUN0_BIT			NO_OS_BIT(4)
#define LT7182S_PADS_RUN1_BIT			NO_OS_BIT(5)

#define LT7182S_SPECIAL_ID_VALUE		0x1C1D

enum lt7182s_chip_id {
	ID_LT7182S,
};

enum lt7182s_output_channel {
	LT7182S_CHAN_0,
	LT7182S_CHAN_1,
	LT7182S_CHAN_ALL = 0xFF,
};

enum lt7182s_fault_pin_config {
	LT7182S_FAULT_PIN_INPUT,
	LT7182S_FAULT_PIN_OUTPUT,
};

enum lt7182s_data_format {
	LT7182S_DATA_FORMAT_LINEAR,
	LT7182S_DATA_FORMAT_IEEE754,
};

enum lt7182s_operation_type {
	LT7182S_OPERATION_OFF = 0x00,
	LT7182S_OPERATION_ON = 0x80,
	LT7182S_OPERATION_MARGIN_HIGH = 0x98,
	LT7182S_OPERATION_MARGIN_LOW = 0xA8,
	LT7182S_OPERATION_SEQ_OFF = 0x40,
};

enum lt7182s_value_type {
	LT7182S_VIN = LT7182S_READ_VIN,
	LT7182S_IIN = LT7182S_READ_IIN,
	LT7182S_VOUT = LT7182S_READ_VOUT,
	LT7182S_IOUT = LT7182S_READ_IOUT,
	LT7182S_TEMP = LT7182S_READ_TEMPERATURE_1,
	LT7182S_FREQUENCY = LT7182S_READ_FREQUENCY,
	LT7182S_POUT = LT7182S_READ_POUT,
	LT7182S_ITH = LT7182S_MFR_READ_ITH,
	LT7182S_EXTVCC = LT7182S_MFR_READ_EXTVCC,
	LT7182S_IOUT_PEAK = LT7182S_MFR_IOUT_PEAK,
	LT7182S_VOUT_PEAK = LT7182S_MFR_VOUT_PEAK,
	LT7182S_VIN_PEAK = LT7182S_MFR_VIN_PEAK,
	LT7182S_TEMP_PEAK = LT7182S_MFR_TEMPERATURE_1_PEAK,
};

enum lt7182s_limit_type {
	LT7182S_VOUT_OV_FAULT_LIMIT_TYPE = LT7182S_VOUT_OV_FAULT_LIMIT,
	LT7182S_VOUT_OV_WARN_LIMIT_TYPE = LT7182S_VOUT_OV_WARN_LIMIT,
	LT7182S_VOUT_UV_FAULT_LIMIT_TYPE = LT7182S_VOUT_UV_FAULT_LIMIT,
	LT7182S_VOUT_UV_WARN_LIMIT_TYPE = LT7182S_VOUT_UV_WARN_LIMIT,
	LT7182S_IIN_OC_WARN_LIMIT_TYPE = LT7182S_IIN_OC_WARN_LIMIT,
	LT7182S_IOUT_OC_WARN_LIMIT_TYPE = LT7182S_IOUT_OC_WARN_LIMIT,
	LT7182S_OT_FAULT_LIMIT_TYPE = LT7182S_OT_FAULT_LIMIT,
	LT7182S_OT_WARN_LIMIT_TYPE = LT7182S_OT_WARN_LIMIT,
	LT7182S_VIN_UV_WARN_LIMIT_TYPE = LT7182S_VIN_UV_WARN_LIMIT,
	LT7182S_TON_MAX_FAULT_LIMIT_TYPE = LT7182S_TON_MAX_FAULT_LIMIT,
	LT7182S_TOFF_MAX_WARN_LIMIT_TYPE = LT7182S_TOFF_MAX_WARN_LIMIT,
};

enum lt7182s_status_type {
	LT7182S_STATUS_BYTE_TYPE = LT7182S_STATUS_BYTE_TYPE_MSK,
	LT7182S_STATUS_VOUT_TYPE = LT7182S_STATUS_VOUT_TYPE_MSK,
	LT7182S_STATUS_IOUT_TYPE = LT7182S_STATUS_IOUT_TYPE_MSK,
	LT7182S_STATUS_INPUT_TYPE = LT7182S_STATUS_INPUT_TYPE_MSK,
	LT7182S_STATUS_TEMP_TYPE = LT7182S_STATUS_TEMP_TYPE_MSK,
	LT7182S_STATUS_CML_TYPE = LT7182S_STATUS_CML_TYPE_MSK,
	LT7182S_STATUS_MFR_SPECIFIC_TYPE = LT7182S_STATUS_MFR_SPECIFIC_TYPE_MSK,
	LT7182S_STATUS_WORD_TYPE = LT7182S_STATUS_WORD_TYPE_MSK,
	LT7182S_STATUS_ALL_TYPE = LT7182S_STATUS_ALL_TYPE_MSK,
};

enum lt7182s_timing_type {
	LT7182S_TON_DELAY_TYPE = LT7182S_TON_DELAY,
	LT7182S_TON_RISE_TYPE = LT7182S_TON_RISE,
	LT7182S_TOFF_DELAY_TYPE = LT7182S_TOFF_DELAY,
	LT7182S_TOFF_FALL_TYPE = LT7182S_TOFF_FALL,
	LT7182_RETRY_DELAY_TYPE = LT7182S_MFR_RETRY_DELAY,
	LT7182S_RESTART_DELAY_TYPE = LT7182S_MFR_RESTART_DELAY,
};

enum lt7182s_nvm_cmd_type {
	LT7182S_LOCK_USER,
	LT7182S_UNLOCK_USER,
	LT7182S_STORE_USER = LT7182S_STORE_USER_ALL,
	LT7182S_RESTORE_USER = LT7182S_RESTORE_USER_ALL,
	LT7182S_COMPARE_USER = LT7182S_MFR_COMPARE_USER_ALL,
};

enum lt7182s_pwm_mode {
	LT7182S_PWM_FORCED_CONTINUOUS_MODE,
	LT7182S_PWM_PULSE_SKIP_MODE,
};

struct lt7182s_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *pg0_desc;
	struct no_os_gpio_desc *pg1_desc;
	struct no_os_gpio_desc *run0_desc;
	struct no_os_gpio_desc *run1_desc;
	struct no_os_gpio_desc *alert_desc;
	struct no_os_gpio_desc *fault0_desc;
	struct no_os_gpio_desc *fault1_desc;
	struct no_os_pwm_desc *sync_desc;

	enum lt7182s_chip_id chip_id;
	enum lt7182s_data_format format;
	int page;
	int lin16_exp;
	bool crc_en;
};

struct lt7182s_init_param {
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_gpio_init_param *pg0_param;
	struct no_os_gpio_init_param *pg1_param;
	struct no_os_gpio_init_param *run0_param;
	struct no_os_gpio_init_param *run1_param;
	struct no_os_gpio_init_param *alert_param;
	struct no_os_gpio_init_param *fault0_param;
	struct no_os_gpio_init_param *fault1_param;
	struct no_os_pwm_init_param *sync_param;

	enum lt7182s_chip_id chip_id;
	enum lt7182s_data_format format;
	enum lt7182s_fault_pin_config fault0_cfg;
	enum lt7182s_fault_pin_config fault1_cfg;

	bool external_clk_en;
	bool crc_en;
};

struct lt7182s_chip_info {
	char *name;
};

struct lt7182s_status {
	uint16_t word;
	uint8_t byte;
	uint8_t vout;
	uint8_t iout;
	uint8_t input;
	uint8_t temp;
	uint8_t cml;
	uint8_t mfr_specific;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the device structure */
int lt7182s_init(struct lt7182s_dev **dev,
		 struct lt7182s_init_param *init_param);

/* Free or remove device instance */
int lt7182s_remove(struct lt7182s_dev *dev);

/* Set PMBus page and phase */
int lt7182s_set_page(struct lt7182s_dev *dev, int page);

/* Send a PMBus command to the device */
int lt7182s_send_byte(struct lt7182s_dev *dev, int page, uint8_t cmd);

/* Perform a PMBus read_byte operation */
int lt7182s_read_byte(struct lt7182s_dev *dev, int page,
		      uint8_t cmd, uint8_t *data);

/*  Perform a PMBus write_byte operation */
int lt7182s_write_byte(struct lt7182s_dev *dev, int page,
		       uint8_t cmd, uint8_t value);

/* Perform a PMBus read_word operation */
int lt7182s_read_word(struct lt7182s_dev *dev, int page,
		      uint8_t cmd, uint16_t *word);

/* Perform a PMBus write_word operation */
int lt7182s_write_word(struct lt7182s_dev *dev, int page,
		       uint8_t cmd, uint16_t word);

/* Perform a PMBus read_word operation then perform conversion*/
int lt7182s_read_word_data(struct lt7182s_dev *dev, int page,
			   uint8_t cmd, int *data);

/* Perform conversion then perform a PMBus write_word operation */
int lt7182s_write_word_data(struct lt7182s_dev *dev, int page,
			    uint8_t cmd, int data);

/* Read a block of bytes */
int lt7182s_read_block_data(struct lt7182s_dev *dev, int page,
			    uint8_t cmd, uint8_t *data, size_t nbytes);

/* Read specific value type */
int lt7182s_read_value(struct lt7182s_dev *dev, int channel,
		       enum lt7182s_value_type value_type,
		       int *value);

/* Read status */
int lt7182s_read_status(struct lt7182s_dev *dev, int channel,
			enum lt7182s_status_type status_type,
			struct lt7182s_status *status);

/* Set VOUT parameters: VOUT_COMMAND and VOUT_MAX */
int lt7182s_vout_value(struct lt7182s_dev *dev, int channel,
		       int vout_command, int vout_max);

/* Set VOUT transition rate */
int lt7182s_vout_tr(struct lt7182s_dev *dev, int channel, int tr);

/* Set VOUT margins */
int lt7182s_vout_margin(struct lt7182s_dev *dev, int channel,
			int margin_high, int margin_low);

/* Set VIN threshold when to start power conversion */
int lt7182s_set_vin(struct lt7182s_dev *dev, int channel,
		    int vin_on, int vin_off);

/* Set timing values */
int lt7182s_set_timing(struct lt7182s_dev *dev, int channel,
		       enum lt7182s_timing_type timing_type, int time);

/* Set switching frequency */
int lt7182s_switch_freq(struct lt7182s_dev *dev, int freq);

/* Set PWM mode of a channel */
int lt7182s_pwm_mode(struct lt7182s_dev *dev, int channel,
		     enum lt7182s_pwm_mode pwm_mode);

/* Set PWM phase offset of a channel */
int lt7182s_pwm_phase(struct lt7182s_dev *dev, int channel,
		      int phase);

/* Set fault/warning limit values */
int lt7182s_set_limit(struct lt7182s_dev *dev, int channel,
		      enum lt7182s_limit_type limit, int limit_val);

/* Set operation */
int lt7182s_set_operation(struct lt7182s_dev *dev, int channel,
			  enum lt7182s_operation_type operation);

/* Set channel state */
int lt7182s_set_channel_state(struct lt7182s_dev *dev, int channel,
			      bool state);

/* Set clock synchronization configs */
int lt7182s_sync_config(struct lt7182s_dev *dev,
			bool input_clk,
			bool output_clk);

/* Set ADC telemetry control */
int lt7182s_adc_control(struct lt7182s_dev *dev, bool low_freq_telemetry,
			bool debug_telemetry);

/* NVM/EEPROM user commands */
int lt7182s_nvm_cmd(struct lt7182s_dev *dev, enum lt7182s_nvm_cmd_type cmd);

/* Clear status faults */
int lt7182s_clear_faults(struct lt7182s_dev *dev);

/* Software reset */
int lt7182s_software_reset(struct lt7182s_dev *dev);

#endif /* __LT7182S_H__ */
