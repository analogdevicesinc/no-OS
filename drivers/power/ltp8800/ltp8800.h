/*******************************************************************************
*   @file   ltp8800.h
*   @brief  Header file of the LTP8800 Driver
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
#ifndef __LTP8800_H__
#define __LTP8800_H__

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
#define LTP8800_PAGE				0x00
#define LTP8800_OPERATION			0x01
#define LTP8800_WRITE_PROTECT			0x10
#define LTP8800_STORE_USER_ALL			0x15
#define LTP8800_RESTORE_USER_ALL		0x16
#define LTP8800_VOUT_MODE			0x20

#define LTP8800_VOUT_COMMAND			0x21
#define LTP8800_VOUT_SCALE_LOOP			0x29
#define LTP8800_VOUT_SCALE_MONITOR		0x2A

#define LTP8800_FREQUENCY_SWITCH		0x33
#define LTP8800_VIN_ON				0x35
#define LTP8800_VIN_OFF				0x36
#define LTP8800_INTERLEAVE			0x37

#define LTP8800_VIN_OV_FAULT_LIMIT		0x55
#define LTP8800_VIN_UV_FAULT_LIMIT		0x59
#define LTP8800_IIN_OC_FAULT_LIMIT		0x5B
#define LTP8800_POUT_OP_FAULT_LIMIT		0x68

#define LTP8800_STATUS_BYTE			0x78
#define LTP8800_STATUS_WORD			0x79
#define LTP8800_STATUS_VOUT			0x7A
#define LTP8800_STATUS_IOUT			0x7B
#define LTP8800_STATUS_INPUT			0x7C
#define LTP8800_STATUS_TEMPERATURE		0x7D
#define LTP8800_STATUS_CML			0x7E
#define LTP8800_STATUS_OTHER			0x7F
#define LTP8800_STATUS_MFR_SPECIFIC		0x80

#define LTP8800_READ_VIN			0x88
#define LTP8800_READ_IIN			0x89
#define LTP8800_READ_VOUT			0x8B
#define LTP8800_READ_IOUT			0x8C
#define LTP8800_READ_TEMPERATURE_2		0x8E
#define LTP8800_READ_TEMPERATURE_3		0x8F
#define LTP8800_READ_DUTY_CYCLE			0x94
#define LTP8800_READ_FREQUENCY			0x95
#define LTP8800_READ_POUT			0x96

#define LTP8800_REVISION			0x98
#define LTP8800_MFR_ID				0x99
#define LTP8800_MFR_MODEL			0x9A
#define LTP8800_MFR_REVISION			0x9B
#define LTP8800_MFR_SERIAL			0x9E
#define LTP8800_IC_DEVICE_ID			0xAD
#define LTP8800_IC_DEVICE_REV			0xAE

#define LTP8800_EEPROM_PASSWORD			0xD5

#define LTP8800_GO_CMD				0xFE00
#define LTP8800_NM_DIGFILT_LF_GAIN		0xFE01
#define LTP8800_NM_DIGFILT_ZERO			0xFE02
#define LTP8800_NM_DIGFILT_POLE			0xFE03
#define LTP8800_NM_DIGFILT_HF_GAIN		0xFE04
#define LTP8800_SYNC				0xFE55

/* PMBus-specific parameters */
#define LTP8800_CRC_POLYNOMIAL			0x7
#define LTP8800_VOUT_MODE_VAL_MSK		NO_OS_GENMASK(4,0)

/* LINEAR data format params */
#define LTP8800_LIN11_MANTISSA_MAX		1023L
#define LTP8800_LIN11_MANTISSA_MIN		511L
#define LTP8800_LIN11_EXPONENT_MAX		15
#define LTP8800_LIN11_EXPONENT_MIN		-15
#define LTP8800_LIN11_MANTISSA_MSK		NO_OS_GENMASK(10,0)
#define LTP8800_LIN11_EXPONENT_MSK		NO_OS_GENMASK(15,11)
#define LTP8800_LIN11_EXPONENT(x)		(int16_t)(x) >> 11
#define LTP8800_LIN11_MANTISSA(x)		((int16_t)((x & 0x7FF) << 5)) >> 5
#define LTP8800_LIN16_EXPONENT			-14

/* Extended commands constants */
#define LTP8800_EXTENDED_COMMAND_PREFIX		0xFE
#define LTP8800_EXTENDED_COMMAND_BEGIN		0xFE00
#define LTP8800_COMMAND_LSB_MSK			NO_OS_GENMASK(7, 0)
#define LTP8800_COMMAND_MSB_MSK			NO_OS_GENMASK(15, 8)

/* Status types masks */
#define LTP8800_STATUS_BYTE_TYPE_MSK		0x01
#define LTP8800_STATUS_VOUT_TYPE_MSK		0x02
#define LTP8800_STATUS_IOUT_TYPE_MSK		0x04
#define LTP8800_STATUS_INPUT_TYPE_MSK		0x08
#define LTP8800_STATUS_TEMP_TYPE_MSK		0x10
#define LTP8800_STATUS_CML_TYPE_MSK		0x20
#define LTP8800_STATUS_MFR_SPECIFIC_TYPE_MSK	0x40
#define LTP8800_STATUS_WORD_TYPE_MSK		0x80
#define LTP8800_STATUS_ALL_TYPE_MSK		0xFF

/* LTP8800 configurable bits and masks */
#define LTP8800_SYNC_ENABLE_BIT			NO_OS_BIT(6)
#define LTP8800_SYNC_LATCH_BIT			NO_OS_BIT(6)
#define LTP8800_WRITE_PROTECT_1_BIT		NO_OS_BIT(7)
#define LTP8800_WRITE_PROTECT_2_BIT		NO_OS_BIT(6)
#define LTP8800_WRITE_PROTECT_3_BIT		NO_OS_BIT(5)
#define LTP8800_INTERLEAVE_ORDER_MSK		NO_OS_GENMASK(3, 0)

/* LTP8800 device constants */
#define LTP8800_IC_DEVICE_ID_VALUE		{0x41, 0x55}
#define LTP8800_MAX_INTERLEAVE_ORDER		0xF
#define LTP8800_VOUT_COMMAND_MAX		1100
#define LTP8800_VOUT_COMMAND_MIN		500
#define LTP8800_VOUT_COMMAND_DEFAULT		0x3000
#define LTP8800_EEPROM_PASSWORD_VALUE		0xFF
#define LTP8800_EEPROM_LOCK_VALUE		0xAB

/* LTP8800 device settings */
#define LTP8800_STATE_ON			1
#define LTP8800_STATE_OFF			0

enum ltp8800_value_type {
	LTP8800_VIN = LTP8800_READ_VIN,
	LTP8800_IIN = LTP8800_READ_IIN,
	LTP8800_VOUT = LTP8800_READ_VOUT,
	LTP8800_IOUT = LTP8800_READ_IOUT,
	LTP8800_FORWARD_DIODE_TEMP = LTP8800_READ_TEMPERATURE_2,
	LTP8800_REVERSE_DIODE_TEMP = LTP8800_READ_TEMPERATURE_3,
	LTP8800_DUTY_CYCLE = LTP8800_READ_DUTY_CYCLE,
	LTP8800_FREQUENCY = LTP8800_READ_FREQUENCY,
	LTP8800_POUT = LTP8800_READ_POUT,
};

enum ltp8800_limit_type {
	LTP8800_VIN_OV_FAULT_LIMIT_TYPE = LTP8800_VIN_OV_FAULT_LIMIT,
	LTP8800_VIN_UV_FAULT_LIMIT_TYPE = LTP8800_VIN_UV_FAULT_LIMIT,
	LTP8800_IIN_OC_FAULT_LIMIT_TYPE = LTP8800_IIN_OC_FAULT_LIMIT,
	LTP8800_POUT_OP_FAULT_LIMIT_TYPE = LTP8800_POUT_OP_FAULT_LIMIT,
};

enum ltp8800_status_type {
	LTP8800_STATUS_BYTE_TYPE = LTP8800_STATUS_BYTE_TYPE_MSK,
	LTP8800_STATUS_VOUT_TYPE = LTP8800_STATUS_VOUT_TYPE_MSK,
	LTP8800_STATUS_IOUT_TYPE = LTP8800_STATUS_IOUT_TYPE_MSK,
	LTP8800_STATUS_INPUT_TYPE = LTP8800_STATUS_INPUT_TYPE_MSK,
	LTP8800_STATUS_TEMP_TYPE = LTP8800_STATUS_TEMP_TYPE_MSK,
	LTP8800_STATUS_CML_TYPE = LTP8800_STATUS_CML_TYPE_MSK,
	LTP8800_STATUS_MFR_SPECIFIC_TYPE = LTP8800_STATUS_MFR_SPECIFIC_TYPE_MSK,
	LTP8800_STATUS_WORD_TYPE = LTP8800_STATUS_WORD_TYPE_MSK,
	LTP8800_STATUS_ALL_TYPE = LTP8800_STATUS_ALL_TYPE_MSK,
};

enum ltp8800_vout_settings {
	LTP8800_VOUT_SETTING_ADI_FACTORY = 0xB2A6,
	LTP8800_VOUT_SETTING_UNITY = 0xBA00,
};

struct ltp8800_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *smbalert_desc;
	struct no_os_gpio_desc *ctrl_desc;
	struct no_os_pwm_desc *ext_clk_desc;

	int lin16_exp;
	bool write_protect_en;
	bool crc_en;
	uint8_t polyphase_order;
};

struct ltp8800_init_param {
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_gpio_init_param *smbalert_param;
	struct no_os_gpio_init_param *ctrl_param;
	struct no_os_pwm_init_param *ext_clk_param;

	bool write_protect_en;
	bool external_clk_en;
	bool sync_en;
	bool crc_en;
	uint8_t polyphase_order;
};

struct ltp8800_status {
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
int ltp8800_init(struct ltp8800_dev **device,
		 struct ltp8800_init_param *init_param);

/* Free or remove device instance */
int ltp8800_remove(struct ltp8800_dev *dev);

/* Send a PMBus command to the device */
int ltp8800_send_byte(struct ltp8800_dev *dev, uint16_t cmd);

/* Perform a PMBus read_byte operation */
int ltp8800_read_byte(struct ltp8800_dev *dev, uint16_t cmd, uint8_t *data);

/* Perform a PMBus write_byte operation */
int ltp8800_write_byte(struct ltp8800_dev *dev, uint16_t cmd, uint8_t value);

/* Perform a PMBus read_word operation */
int ltp8800_read_word(struct ltp8800_dev *dev, uint16_t cmd, uint16_t *word);

/* Perform a PMBus write_word operation */
int ltp8800_write_word(struct ltp8800_dev *dev, uint16_t cmd, uint16_t word);

/* Perform a PMBus read_word operation then perform conversion*/
int ltp8800_read_word_data(struct ltp8800_dev *dev, uint16_t cmd, int *data);

/* Perform conversion then perform a PMBus write_word operation */
int ltp8800_write_word_data(struct ltp8800_dev *dev, uint16_t cmd, int data);

/* Read a block of bytes */
int ltp8800_read_block_data(struct ltp8800_dev *dev, uint16_t cmd,
			    uint8_t *data, size_t nbytes);

/* Read specific value type */
int ltp8800_read_value(struct ltp8800_dev *dev,
		       enum ltp8800_value_type value_type,
		       int *value);

/* Read status */
int ltp8800_read_status(struct ltp8800_dev *dev,
			enum ltp8800_status_type status_type,
			struct ltp8800_status *status);

/* Set VOUT command */
int ltp8800_vout_value(struct ltp8800_dev *dev, int vout_command);

/* Set gain settings */
int ltp8800_vout_settings(struct ltp8800_dev *dev,
			  enum ltp8800_vout_settings settings);

/* Set VIN threshold when to start power conversion */
int ltp8800_set_vin(struct ltp8800_dev *dev, int vin_on, int vin_off);

/* Set fault/warning limit values */
int ltp8800_set_fault_limit(struct ltp8800_dev *dev,
			    enum ltp8800_limit_type limit,
			    int limit_val);

/* Enable/Disable multi-device synchronization */
int ltp8800_sync_config(struct ltp8800_dev *dev, bool enable);

/* Set PolyPhase order */
int ltp8800_interleave_order(struct ltp8800_dev *dev, uint8_t order);

/* Program loop compensation */
int ltp8800_loop_compensation(struct ltp8800_dev *dev,
			      uint8_t pole,
			      uint8_t zero,
			      uint8_t hf_gain,
			      uint8_t lf_gain);

/* Set device state */
int ltp8800_set_device_state(struct ltp8800_dev *dev, bool state);

/* Store user settings to EEPROM */
int ltp8800_store_user_settings(struct ltp8800_dev *dev);

/* Restore user settings from EEPROM */
int ltp8800_restore_user_settings(struct ltp8800_dev *dev);

#endif /* __LTP8800_H__ */
