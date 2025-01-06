/*******************************************************************************
*   @file   lt3074.h
*   @brief  Header file of the LT3074 Driver
*   @authors Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
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
#ifndef __LT3074_H__
#define __LT3074_H__

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

#define LT3074_SEND_BYTE			0 /* Send-only command */
#define LT3074_BYTE				1 /* Byte */
#define LT3074_WORD				2 /* Word */

#define LT3074_R0B				(LT3074_SEND_BYTE << 8)
#define LT3074_R1B				(LT3074_BYTE << 8)
#define LT3074_R2B				(LT3074_WORD << 8)
#define LT3074_R6B				(6 << 8) /* Block of size 6*/

#define LT3074_SIZE_MSK				NO_OS_GENMASK(15,8)
#define LT3074_CMD(x)				((uint8_t)(x & 0xFF))
#define LT3074_ADDR_SIZE(x)			no_os_field_get(LT3074_SIZE_MSK, x)

/* PMBus registers */
#define LT3074_PAGE				(LT3074_R1B | 0x00)
#define LT3074_OPERATION			(LT3074_R1B | 0x01)
#define LT3074_ON_OFF_CONFIG			(LT3074_R1B | 0x02)
#define LT3074_CLEAR_FAULTS			(LT3074_R0B | 0x03)
#define LT3074_WRITE_PROTECT			(LT3074_R1B | 0x10)
#define LT3074_CAPABILITY			(LT3074_R1B | 0x19)
#define LT3074_VOUT_MODE			(LT3074_R1B | 0x20)
#define LT3074_VOUT_OV_WARN_LIMIT		(LT3074_R2B | 0x42)
#define LT3074_VOUT_UV_WARN_LIMIT		(LT3074_R2B | 0x43)
#define LT3074_IOUT_OC_FAULT_LIMIT		(LT3074_R2B | 0x46)
#define LT3074_IOUT_OC_FAULT_RESPONSE		(LT3074_R1B | 0x47)
#define LT3074_OT_WARN_LIMIT			(LT3074_R2B | 0x51)
#define LT3074_VIN_OV_WARN_LIMIT		(LT3074_R2B | 0x57)
#define LT3074_VIN_UV_WARN_LIMIT		(LT3074_R2B | 0x58)
#define LT3074_STATUS_BYTE			(LT3074_R1B | 0x78)
#define LT3074_STATUS_WORD			(LT3074_R2B | 0x79)
#define LT3074_STATUS_VOUT			(LT3074_R1B | 0x7A)
#define LT3074_STATUS_IOUT			(LT3074_R1B | 0x7B)
#define LT3074_STATUS_INPUT			(LT3074_R1B | 0x7C)
#define LT3074_STATUS_TEMPERATURE		(LT3074_R1B | 0x7D)
#define LT3074_STATUS_CML			(LT3074_R1B | 0x7E)
#define LT3074_STATUS_MFR_SPECIFIC		(LT3074_R1B | 0x80)
#define LT3074_READ_VIN				(LT3074_R2B | 0x88)
#define LT3074_READ_VOUT			(LT3074_R2B | 0x8B)
#define LT3074_READ_IOUT			(LT3074_R2B | 0x8C)
#define LT3074_READ_TEMPERATURE_1		(LT3074_R2B | 0x8D)
#define LT3074_REVISION				(LT3074_R1B | 0x98)
#define LT3074_IC_DEVICE_ID			(LT3074_R6B | 0xAD)
#define LT3074_IC_DEVICE_REV			(LT3074_R2B | 0xAE)

/* Manufacturer registers */
#define LT3074_MFR_MARGIN			(LT3074_R1B | 0xC4)
#define LT3074_MFR_READ_VBIAS			(LT3074_R2B | 0xC6)
#define LT3074_MFR_BIAS_OV_WARN_LIMIT		(LT3074_R2B | 0xC7)
#define LT3074_MFR_BIAS_UV_WARN_LIMIT		(LT3074_R2B | 0xC8)
#define LT3074_MFR_IOUT_MIN_WARN_LIMIT		(LT3074_R2B | 0xC9)
#define LT3074_MFR_SPECIAL_ID			(LT3074_R2B | 0xE7)
#define LT3074_MFR_DEFAULT_CONFIG		(LT3074_R1B | 0xF5)
#define LT3074_MFR_RAIL_ADDRESS			(LT3074_R1B | 0xFA)
#define LT3074_MFR_RESET			(LT3074_R0B | 0xFD)

/* PMBus-specific parameters */
#define LT3074_CRC_POLYNOMIAL			0x7
#define LT3074_MARGIN_HIGH_MSK			0xF0
#define LT3074_MARGIN_LOW_MSK			0x0F
#define LT3074_OPERATION_ACCESS_MSK		(NO_OS_GENMASK(5,4) | NO_OS_BIT(7))
#define LT3074_SPECIAL_ID_VALUE			0x1C1D

/* Status types masks */
#define LT3074_STATUS_BYTE_TYPE_MSK		0x01
#define LT3074_STATUS_VOUT_TYPE_MSK		0x02
#define LT3074_STATUS_IOUT_TYPE_MSK		0x04
#define LT3074_STATUS_INPUT_TYPE_MSK		0x08
#define LT3074_STATUS_TEMP_TYPE_MSK		0x10
#define LT3074_STATUS_CML_TYPE_MSK		0x20
#define LT3074_STATUS_MFR_SPECIFIC_TYPE_MSK	0x40
#define LT3074_STATUS_WORD_TYPE_MSK		0x80
#define LT3074_STATUS_ALL_TYPE_MSK		0xFF

enum lt3074_operation_type {
	LT3074_OPERATION_OFF,
	LT3074_OPERATION_ON = 0x80,
	LT3074_OPERATION_MARGIN_HIGH = 0xA0,
	LT3074_OPERATION_MARGIN_LOW = 0x90,
};

enum lt3074_value_type {
	LT3074_VIN = LT3074_READ_VIN,
	LT3074_VOUT = LT3074_READ_VOUT,
	LT3074_IOUT = LT3074_READ_IOUT,
	LT3074_TEMP = LT3074_READ_TEMPERATURE_1,
	LT3074_VBIAS = LT3074_MFR_READ_VBIAS,
};

enum lt3074_limit_type {
	LT3074_VOUT_OV_WARN_LIMIT_TYPE = LT3074_VOUT_OV_WARN_LIMIT,
	LT3074_VOUT_UV_WARN_LIMIT_TYPE = LT3074_VOUT_UV_WARN_LIMIT,
	LT3074_IOUT_OC_FAULT_LIMIT_TYPE = LT3074_IOUT_OC_FAULT_LIMIT,
	LT3074_OT_WARN_LIMIT_TYPE = LT3074_OT_WARN_LIMIT,
	LT3074_VIN_OV_WARN_LIMIT_TYPE = LT3074_VIN_OV_WARN_LIMIT,
	LT3074_VIN_UV_WARN_LIMIT_TYPE = LT3074_VIN_UV_WARN_LIMIT,
	LT3074_VBIAS_OV_WARN_LIMIT_TYPE = LT3074_MFR_BIAS_OV_WARN_LIMIT,
	LT3074_VBIAS_UV_WARN_LIMIT_TYPE = LT3074_MFR_BIAS_UV_WARN_LIMIT,
	LT3074_IOUT_MIN_WARN_LIMIT_TYPE	= LT3074_MFR_IOUT_MIN_WARN_LIMIT,
};

enum lt3074_status_type {
	LT3074_STATUS_BYTE_TYPE = LT3074_STATUS_BYTE_TYPE_MSK,
	LT3074_STATUS_VOUT_TYPE = LT3074_STATUS_VOUT_TYPE_MSK,
	LT3074_STATUS_IOUT_TYPE = LT3074_STATUS_IOUT_TYPE_MSK,
	LT3074_STATUS_INPUT_TYPE = LT3074_STATUS_INPUT_TYPE_MSK,
	LT3074_STATUS_TEMP_TYPE = LT3074_STATUS_TEMP_TYPE_MSK,
	LT3074_STATUS_CML_TYPE = LT3074_STATUS_CML_TYPE_MSK,
	LT3074_STATUS_MFR_SPECIFIC_TYPE = LT3074_STATUS_MFR_SPECIFIC_TYPE_MSK,
	LT3074_STATUS_WORD_TYPE = LT3074_STATUS_WORD_TYPE_MSK,
	LT3074_STATUS_ALL_TYPE = LT3074_STATUS_ALL_TYPE_MSK,
};

enum lt3074_margin {
	LT3074_MARGIN_PERCENTAGE_0,
	LT3074_MARGIN_PERCENTAGE_1,
	LT3074_MARGIN_PERCENTAGE_3,
	LT3074_MARGIN_PERCENTAGE_5,
	LT3074_MARGIN_PERCENTAGE_10,
	LT3074_MARGIN_PERCENTAGE_15,
	LT3074_MARGIN_PERCENTAGE_20,
	LT3074_MARGIN_PERCENTAGE_25,
	LT3074_MARGIN_PERCENTAGE_30,
};

struct lt3074_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *pg_desc;
	struct no_os_gpio_desc *en_desc;
	struct no_os_gpio_desc *alert_desc;

	int lin16_exp;
	bool crc_en;
};

struct lt3074_init_param {
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_gpio_init_param *pg_param;
	struct no_os_gpio_init_param *en_param;
	struct no_os_gpio_init_param *alert_param;

	bool crc_en;
};

struct lt3074_status {
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
int lt3074_init(struct lt3074_dev **dev,
		struct lt3074_init_param *init_param);

/* Free or remove device instance */
int lt3074_remove(struct lt3074_dev *dev);

/* Send a PMBus command to the device */
int lt3074_send_byte(struct lt3074_dev *dev, uint32_t cmd);

/* Perform a PMBus read_byte operation */
int lt3074_read_byte(struct lt3074_dev *dev, uint32_t cmd, uint8_t *data);

/*  Perform a PMBus write_byte operation */
int lt3074_write_byte(struct lt3074_dev *dev, uint32_t cmd, uint8_t value);

/* Perform a PMBus read_word operation */
int lt3074_read_word(struct lt3074_dev *dev, uint32_t cmd, uint16_t *word);

/* Perform a PMBus write_word operation */
int lt3074_write_word(struct lt3074_dev *dev, uint32_t cmd, uint16_t word);

/* Perform a PMBus read_word operation then perform conversion*/
int lt3074_read_word_data(struct lt3074_dev *dev, uint32_t cmd, int *data);

/* Perform conversion then perform a PMBus write_word operation */
int lt3074_write_word_data(struct lt3074_dev *dev, uint32_t cmd, int data);

/* Read a block of bytes */
int lt3074_read_block_data(struct lt3074_dev *dev, uint32_t cmd,
			   uint8_t *data, size_t nbytes);

/* Register read */
int lt3074_reg_read(struct lt3074_dev *dev, uint32_t reg, uint32_t *data);

/* Register write */
int lt3074_reg_write(struct lt3074_dev *dev, uint32_t reg, uint32_t val);

/* Read specific value type */
int lt3074_read_value(struct lt3074_dev *dev,
		      enum lt3074_value_type value_type,
		      int *value);

/* Read status */
int lt3074_read_status(struct lt3074_dev *dev,
		       enum lt3074_status_type status_type,
		       struct lt3074_status *status);

/* Set VOUT margins */
int lt3074_vout_margin(struct lt3074_dev *dev, enum lt3074_margin margin_high,
		       enum lt3074_margin margin_low);

/* Set fault/warning limit values */
int lt3074_set_limit(struct lt3074_dev *dev, enum lt3074_limit_type limit,
		     int limit_val);

/* Set operation */
int lt3074_set_operation(struct lt3074_dev *dev,
			 enum lt3074_operation_type operation);

/* Clear status registers */
int lt3074_clear_faults(struct lt3074_dev *dev);

/* Turn output ON/OFF */
int lt3074_set_enable_pin(struct lt3074_dev *dev, bool state);

/* Software reset */
int lt3074_software_reset(struct lt3074_dev *dev);

#endif /* __LT3074_H__ */
