/****************************************************************************//**
 *   @file   max34451.h
 *   @brief  Header file of MAX34451 PMBus driver.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
 * *******************************************************************************
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
#ifndef __MAX34451_H__
#define __MAX34451_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_gpio.h"

/* PMBus Commands MAX34451ETNA4 */
#define MAX34451_PAGE           0x00
#define MAX34451_OPERATION      0x01
#define MAX34451_ON_OFF_CONFIG  0x02
#define MAX34451_CLEAR_FAULTS   0x03

#define MAX34451_WRITE_PROTECT      0x10
#define MAX34451_STORE_DEFAULT_ALL  0x11
#define MAX34451_RESTORE_DEFAULT_ALL    0x12

#define MAX34451_CAPABILITY     0x19
#define MAX34451_VOUT_MODE      0x20
#define MAX34451_VOUT_MARGIN_HIGH   0x25
#define MAX34451_VOUT_MARGIN_LOW    0x26

#define MAX34451_VOUT_SCALE_MONITOR     0x2A
#define MAX34451_IOUT_CAL_GAIN          0x38

#define MAX34451_VOUT_OV_FAULT_LIMIT    0x40
#define MAX34451_VOUT_OV_WARN_LIMIT   0x42
#define MAX34451_VOUT_UV_WARN_LIMIT     0x43
#define MAX34451_VOUT_UV_FAULT_LIMIT    0x44
/* IOUT OC WARN and FAULT limit addresses are swapped in MAX34451 */
/* MAX34455, ADPM devices use the correct addresses */
/* Automatically swapped when using Read Word Data command for IIO */
#define MAX34451_IOUT_OC_WARN_LIMIT    0x46
#define MAX34451_IOUT_OC_FAULT_LIMIT     0x4A

#define MAX34451_OT_FAULT_LIMIT     0x4F
#define MAX34451_OT_WARN_LIMIT      0x51

#define MAX34451_POWER_GOOD_ON      0x5E
#define MAX34451_POWER_GOOD_OFF     0x5F
#define MAX34451_TON_DELAY      0x60
#define MAX34451_TON_MAX_FAULT_LIMIT        0x62
#define MAX34451_TOFF_DELAY     0x64

#define MAX34451_STATUS_WORD        0x79
#define MAX34451_STATUS_VOUT        0x7A
#define MAX34451_STATUS_IOUT        0x7B
#define MAX34451_STATUS_TEMPERATURE     0x7D
#define MAX34451_STATUS_CML     0x7E
#define MAX34451_STATUS_MFR_SPECIFIC        0x80

#define MAX34451_READ_VOUT      0x8B
#define MAX34451_READ_IOUT      0x8C
#define MAX34451_READ_TEMPERATURE_1     0x8D
#define MAX34451_PMBUS_REVISION     0x98

#define MAX34451_MFR_ID     0x99
#define MAX34451_MFR_MODEL      0x9A
#define MAX34451_MFR_REVISION       0x9B
#define MAX34451_MFR_LOCATION       0x9C
#define MAX34451_MFR_DATE       0x9D
#define MAX34451_MFR_SERIAL     0x9E
#define MAX34451_MFR_MODE       0xD1
#define MAX34451_MFR_PSEN_CONFIG        0xD2
#define MAX34451_MFR_VOUT_PEAK      0xD4
#define MAX34451_MFR_IOUT_PEAK      0xD5
#define MAX34451_MFR_TEMPERATURE_PEAK       0xD6
#define MAX34451_MFR_VOUT_MIN       0xD7
#define MAX34451_MFR_NV_LOG_CONFIG       0xD8
#define MAX34451_MFR_FAULT_RESPONSE     0xD9
#define MAX34451_MFR_FAULT_RETRY        0xDA
#define MAX34451_MFR_NV_FAULT_LOG       0xDC
#define MAX34451_MFR_TIME_COUNT     0xDD
#define MAX34451_MFR_MARGIN_CONFIG      0xDF
#define MAX34451_MFR_FW_SERIAL      0xE0
#define MAX34451_MFR_IOUT_AVG       0xE2
#define MAX34451_MFR_CHANNEL_CONFIG 0xE4
#define MAX34451_MFR_TON_SEQ_MAX    0xE6
#define MAX34451_MFR_PWM_CONFIG     0xE7
#define MAX34451_MFR_SEQ_CONFIG     0xE8
#define MAX34451_MFR_STORE_ALL      0xEE
#define MAX34451_MFR_RESTORE_ALL    0xEF
#define MAX34451_MFR_TEMP_SENSOR_CONFIG 0xF0
#define MAX34451_MFR_STORE_SINGLE   0xFC
#define MAX34451_MFR_CRC        0xFE

/* MAX34451NA6+ and onwards manufacturer-specific commands */
#define MAX34451NA6_STATUS_INPUT     0x7C
#define MAX34451NA6_READ_VIN       0x88
#define MAX34451NA6_READ_IIN      0x89
#define MAX34451NA6_MFR_CONFIG_VERSION   0xEC
#define MAX34451NA6_MFR_WP_CONTROL    0xF2

/* MAX34451 CHANNELS */
/* MAX34451 and MAX34451NA6+ have the same channel numbers */
/* Power supply monitor with sequencing and margining (0:7 with PWM, 8:11 use DS4424) */
#define MAX34451_CHAN_0       0x00
#define MAX34451_CHAN_1       0x01
#define MAX34451_CHAN_2       0x02
#define MAX34451_CHAN_3       0x03
#define MAX34451_CHAN_4       0x04
#define MAX34451_CHAN_5       0x05
#define MAX34451_CHAN_6       0x06
#define MAX34451_CHAN_7       0x07
#define MAX34451_CHAN_8       0x08
#define MAX34451_CHAN_9       0x09
#define MAX34451_CHAN_10      0x0A
#define MAX34451_CHAN_11      0x0B
/* Monitor only channels */
#define MAX34451_CHAN_12      0x0C
#define MAX34451_CHAN_13      0x0D
#define MAX34451_CHAN_14      0x0E
#define MAX34451_CHAN_15      0x0F
/* Temperature Sensors, 16 internal sensor, 17..20 external DS75LV @ 0x90/0x92/0x94/0x96 */
#define MAX34451_CHAN_16      0x10
#define MAX34451_CHAN_17      0x11
#define MAX34451_CHAN_18      0x12
#define MAX34451_CHAN_19      0x13
#define MAX34451_CHAN_20      0x14
#define MAX34451_CHAN_ALL       0xFF

/* MAX34455 CHANNELS */
/* Sequencing and Margining (0..5 PWM, 6:7 DS4424) */
#define MAX34455_CHAN_0       0x00
#define MAX34455_CHAN_1       0x01
#define MAX34455_CHAN_2       0x02
#define MAX34455_CHAN_3       0x03
#define MAX34455_CHAN_4       0x04
#define MAX34455_CHAN_5       0x05
#define MAX34455_CHAN_6       0x06
#define MAX34455_CHAN_7       0x07
/* 8..11 monitor only */
#define MAX34455_CHAN_8       0x08
#define MAX34455_CHAN_9       0x09
#define MAX34455_CHAN_10      0x0A
#define MAX34455_CHAN_11      0x0B
/* Temperature sensors, 12 internal, 13..16 external DS75LV address 0x90/0x92/0x94/0x96 */
#define MAX34455_CHAN_12      0x0C
#define MAX34455_CHAN_13      0x0D
#define MAX34455_CHAN_14      0x0E
#define MAX34455_CHAN_15      0x0F
#define MAX34455_CHAN_16      0x10
#define MAX34455_CHAN_ALL       0xFF

/* ADPM12XXX CHANNELS */
/* 0, 1. and 15 internal use, do NOT change */
#define ADPM12XXX_CHAN_0       0x00
#define ADPM12XXX_CHAN_1       0x01
#define ADPM12XXX_CHAN_15      0x0F
/* Output voltage monitoring */
#define ADPM12XXX_CHAN_2       0x02
/* PGOOD output delay control */
#define ADPM12XXX_CHAN_3       0x03
/* Output current control */
#define ADPM12XXX_CHAN_4       0x04
/* Phase 1..4 current monitoring and control*/
#define ADPM12XXX_CHAN_5       0x05
#define ADPM12XXX_CHAN_6       0x06
/* Channels 7 and 8 not used by ADPM12250 */
#define ADPM12XXX_CHAN_7       0x07
#define ADPM12XXX_CHAN_8       0x08
/* Input voltage monitoring and control */
#define ADPM12XXX_CHAN_9       0x09
/* Input current monitoring and control */
#define ADPM12XXX_CHAN_10      0x0A
/* READ_IOUT (8Ch) only */
#define ADPM12XXX_CHAN_14      0x0E
/* PCB board temperature monitoring and control */
#define ADPM12XXX_CHAN_18      0x12
#define ADPM12XXX_CHAN_ALL       0xFF

/* OPERATION mask */
#define MAX34451_OPERATION_ON_MSK     NO_OS_BIT(7)
#define MAX34451_OPERATION_TOFF_DELAY_MSK     NO_OS_BIT(6)
#define MAX34451_OPERATION_MARGIN_MSK     NO_OS_GENMASK(5,2)
#define MAX34451_OPERATION_SEQ_MSK     NO_OS_GENMASK(1,0)

/* ON_OFF_CONFIG mask */
#define MAX34451_ON_OFF_CONFIG_AND_OR     NO_OS_BIT(5)
#define MAX34451_ON_OFF_CONFIG_SUPPLIES     NO_OS_BIT(4)
#define MAX34451_ON_OFF_CONFIG_OPERATION_ENABLE     NO_OS_BIT(3)
#define MAX34451_ON_OFF_CONFIG_CONTROL_ENABLE     NO_OS_BIT(2)
#define MAX34451_ON_OFF_CONFIG_CONTROL_POLARITY     NO_OS_BIT(1)
#define MAX34451_ON_OFF_CONFIG_CONTROL_TURN_OFF     NO_OS_BIT(0)

/* CAPABILITY mask */
#define MAX34451_CAPABILITY_PEC     NO_OS_BIT(7)
#define MAX34451_CAPABILITY_PMBUS_SPEED     NO_OS_GENMASK(6,5)
#define MAX34451_CAPABILITY_ALERT     NO_OS_BIT(4)

/* STATUS_WORD message */
#define MAX34451_STATUS_WORD_VOUT     NO_OS_BIT(15)
#define MAX34451_STATUS_WORD_IOUT     NO_OS_BIT(14)
#define MAX34451_STATUS_WORD_MFR     NO_OS_BIT(12)
#define MAX34451_STATUS_WORD_PGOOD     NO_OS_BIT(11)
#define MAX34451_STATUS_WORD_MARGIN     NO_OS_BIT(8)
#define MAX34451_STATUS_WORD_SYS_OFF     NO_OS_BIT(6)
#define MAX34451_STATUS_WORD_VOUT_OV     NO_OS_BIT(5)
#define MAX34451_STATUS_WORD_IOUT_OC     NO_OS_BIT(4)
#define MAX34451_STATUS_WORD_TEMPERATURE     NO_OS_BIT(2)
#define MAX34451_STATUS_WORD_CML     NO_OS_BIT(1)

/* STATUS_VOUT message */
#define MAX34451_STATUS_VOUT_OV_FAULT     NO_OS_BIT(7)
#define MAX34451_STATUS_VOUT_OV_WARN     NO_OS_BIT(6)
#define MAX34451_STATUS_VOUT_UV_WARN     NO_OS_BIT(5)
#define MAX34451_STATUS_VOUT_UV_FAULT     NO_OS_BIT(4)
#define MAX34451_STATUS_VOUT_TON_MAX_FAULT     NO_OS_BIT(2)

/* STATUS_IOUT message */
#define MAX34451_STATUS_IOUT_OC_FAULT     NO_OS_BIT(7)
#define MAX34451_STATUS_IOUT_OC_WARN     NO_OS_BIT(5)

/* STATUS_TEMPERATURE message */
#define MAX34451_STATUS_OT_FAULT     NO_OS_BIT(7)
#define MAX34451_STATUS_OT_WARN     NO_OS_BIT(6)

/* STATUS_CML */
#define MAX34451_STATUS_CML_COMM_FAULT     NO_OS_BIT(7)
#define MAX34451_STATUS_CML_DATA_FAULT     NO_OS_BIT(6)
#define MAX34451_STATUS_CML_BACKUP_FAULT     NO_OS_BIT(2)
#define MAX34451_STATUS_CML_MAIN_FAULT     NO_OS_BIT(1)
#define MAX34451_STATUS_CML_FAULT_LOG_FULL     NO_OS_BIT(0)

/* STATUS_MFR_SPECIFIC @ PAGES 0-11 */
#define MAX34451_STATUS_MFR_SPECIFIC_OFF     NO_OS_BIT(7)
#define MAX34451_STATUS_MFR_SPECIFIC_MARGIN_FAULT	 NO_OS_BIT(3)
#define MAX34451_STATUS_MFR_SPECIFIC_PGOOD     NO_OS_BIT(2)

/* STATUS_MFR_SPECIFIC @ PAGE 255 */
#define MAX34451_STATUS_MFR_SPECIFIC_LOCK     NO_OS_BIT(7)
#define MAX34451_STATUS_MFR_SPECIFIC_FAULT_INPUT     NO_OS_BIT(6)
#define MAX34451_STATUS_MFR_SPECIFIC_POR     NO_OS_BIT(5)
#define MAX34451_STATUS_MFR_SPECIFIC_WATCHDOG_INIT     NO_OS_BIT(4)
#define MAX34451_STATUS_MFR_SPECIFIC_CONTROL     NO_OS_BIT(3)

/* STATUS_INPUT */
#define MAX34451NA6_STATUS_INPUT_VIN_OV_FAULT     NO_OS_BIT(7)
#define MAX34451NA6_STATUS_INPUT_VIN_OV_WARN     NO_OS_BIT(6)
#define MAX34451NA6_STATUS_INPUT_IIN_UV_WARN     NO_OS_BIT(5)
#define MAX34451NA6_STATUS_INPUT_IIN_UV_FAULT     NO_OS_BIT(4)
#define MAX34451NA6_STATUS_INPUT_UNIT_OFF     NO_OS_BIT(3)
#define MAX34451NA6_STATUS_INPUT_IIN_OV_FAULT     NO_OS_BIT(2)
#define MAX34451NA6_STATUS_INPUT_IIN_OV_WARN     NO_OS_BIT(1)

/* MFR_NV_LOG_CONFIG */
#define MAX34451_MFR_NV_LOG_CFG_FORCE_FAULT_LOG	 NO_OS_BIT(15)
#define MAX34451_MFR_NV_LOG_CFG_CLR_FAULT_LOG	 NO_OS_BIT(14)
#define MAX34451_MFR_NV_LOG_CFG_LOG_T0_CFG	 NO_OS_BIT(10)
#define MAX34451_MFR_NV_LOG_CFG_LOG_OVERWRITE	 NO_OS_BIT(9)
#define MAX34451_MFR_NV_LOG_CFG_LOG_DEPTH	 NO_OS_GENMASK(8,7)
#define MAX34451_MFR_NV_LOG_CFG_NV_LOG_FAULT0	NO_OS_BIT(6)
#define MAX34451_MFR_NV_LOG_CFG_NV_LOG_FAULT1	NO_OS_BIT(5)
#define MAX34451_MFR_NV_LOG_CFG_NV_LOG_FAULT2	NO_OS_BIT(4)

/* MFR_FAULT_RESPONSE */
#define MAX34451_MFR_FAULT_RESP_FAULT2_RESP_ENABLE	NO_OS_BIT(26)
#define MAX34451_MFR_FAULT_RESP_FAULT1_RESP_ENABLE	NO_OS_BIT(25)
#define MAX34451_MFR_FAULT_RESP_FAULT0_RESP_ENABLE	NO_OS_BIT(24)
#define MAX34451_MFR_FAULT_RESP_FAULT2_ASSERT_ENABLE	NO_OS_BIT(18)
#define MAX34451_MFR_FAULT_RESP_FAULT1_ASSERT_ENABLE	NO_OS_BIT(17)
#define MAX34451_MFR_FAULT_RESP_FAULT0_ASSERT_ENABLE	NO_OS_BIT(16)
#define MAX34451_MFR_FAULT_RESP_NV_LOG	NO_OS_BIT(15)
#define MAX34451_MFR_FAULT_RESP_GLOBAL	NO_OS_BIT(14)
#define MAX34451_MFR_FAULT_RESP_FILTER NO_OS_GENMASK(13,12)
#define MAX34451_MFR_FAULT_RESP_ALARM_CONFIG	NO_OS_GENMASK(10,8)
#define MAX34451_MFR_FAULT_RESP_OT_FAULT_LIM_RESP	 NO_OS_GENMASK(7,6)
#define MAX34451_MFR_FAULT_RESP_TON_MAX_FAULT_LIM_RESP	 NO_OS_GENMASK(5,4)
#define MAX34451_MFR_FAULT_RESP_VOUT_UV_FAULT_LIM_RESP	 NO_OS_GENMASK(3,2)
#define MAX34451_MFR_FAULT_RESP_VOUT_OV_FAULT_LIM_RESP	 NO_OS_GENMASK(1,0)

/* MFR_MARGIN_CONFIG */
#define MAX34451_MFR_MARGIN_CONFIG_SLOPE	 NO_OS_BIT(15)
#define MAX34451_MFR_MARGIN_CONFIG_OPEN_LOOP	 NO_OS_BIT(14)
#define MAX34451_MFR_MARGIN_CONFIG_DC_DAC	 NO_OS_GENMASK(7,0)

/* MFR_MODE */
#define MAX34451_MFR_MODE_ALERT     NO_OS_BIT(13)
#define MAX34451_MFR_MODE_SOFT_RESET     NO_OS_BIT(11)
#define MAX34451_MFR_MODE_LOCK     NO_OS_BIT(10)
#define MAX34451_MFR_MODE_ADC_TIME     NO_OS_GENMASK(7,6)
#define MAX34451_MFR_MODE_ADC_AVERAGE     NO_OS_GENMASK(5,4)
#define MAX34451_MFR_MODE_IOUT_AVG     NO_OS_GENMASK(3,0)

/* Startup time is 90 ms, or 170 ms when MFR_STORE_SINGLE data has to be
 * loaded from flash. The driver cannot tell the two cases apart. */
#define MAX34451_STARTUP_DELAY_MS     170

/* MFR_PSEN_CONFIG */
#define MAX34451_MFR_PSEN_CONFIG_PG_ALARM_SELECT	 NO_OS_GENMASK(31,16)
#define MAX34451_MFR_PSEN_CONFIG_PP_OD     NO_OS_BIT(7)
#define MAX34451_MFR_PSEN_CONFIG_ACTIVE_HIGH     NO_OS_BIT(6)
#define MAX34451_MFR_PSEN_CONFIG_SELECT_MASK     NO_OS_GENMASK(2,0)
#define MAX34451_PSEN_SELECT_PSEN     0x0
#define MAX34451_PSEN_SELECT_FORCE_ON     0x1
#define MAX34451_PSEN_SELECT_FORCE_OFF     0x2
#define MAX34451_PSEN_SELECT_PG_GPI     0x3
#define MAX34451_PSEN_SELECT_ALARM     0x4
#define MAX34451_PSEN_SELECT_FAULT2     0x5
#define MAX34451_PSEN_SELECT_SEQ     0x6

/* Status type masks */
#define MAX34451_STATUS_VOUT_TYPE_MSK		0x01
#define MAX34451_STATUS_IOUT_TYPE_MSK		0x02
#define MAX34451_STATUS_TEMP_TYPE_MSK		0x04
#define MAX34451_STATUS_CML_TYPE_MSK		0x08
#define MAX34451_STATUS_MFR_SPECIFIC_TYPE_MSK	0x10
#define MAX34451_STATUS_WORD_TYPE_MSK		0x20
#define MAX34451NA6_STATUS_INPUT_TYPE_MSK	0x40

/* Maximum block length for PMBus block read/write operations */
#define MAX34451_MAX_BLOCK_LENGTH	8

enum max34451_chip_id {
	ID_MAX34451,    /* MAX34451ETNA4 and below */
	ID_MAX34451NA6, /* NA6+ and NA6+T */
	ID_MAX34455,
	ID_ADPM12160,
	ID_ADPM12200,
	ID_ADPM12250,
};

enum max34451_operation_type {
	MAX34451_OPERATION_OFF = 0x00,
	MAX34451_OPERATION_OFF_SEQ0 = 0x01,
	MAX34451_OPERATION_OFF_SEQ1 = 0x02,
	MAX34451_OPERATION_SEQ_OFF = 0x40,
	MAX34451_OPERATION_SEQ_OFF_SEQ0 = 0x41,
	MAX34451_OPERATION_SEQ_OFF_SEQ1 = 0x42,
	MAX34451_OPERATION_ON_MARGIN_OFF = 0x80,
	MAX34451_OPERATION_ON_MARGIN_OFF_SEQ0 = 0x81,
	MAX34451_OPERATION_ON_MARGIN_OFF_SEQ1 = 0x82,
	MAX34451_OPERATION_ON_MARGIN_LOW_IGNORE_FAULT = 0x94,
	MAX34451_OPERATION_ON_MARGIN_LOW_ACT_FAULT = 0x98,
	MAX34451_OPERATION_ON_MARGIN_HIGH_IGNORE_FAULT = 0xA4,
	MAX34451_OPERATION_ON_MARGIN_HIGH_ACT_FAULT = 0xA8,
};

enum max34451_value_type {
	MAX34451NA6_VAL_VIN = MAX34451NA6_READ_VIN,
	MAX34451NA6_VAL_IIN = MAX34451NA6_READ_IIN,
	MAX34451_VAL_VOUT = MAX34451_READ_VOUT,
	MAX34451_VAL_IOUT = MAX34451_READ_IOUT,
	MAX34451_VAL_TEMP = MAX34451_READ_TEMPERATURE_1,
	MAX34451_VAL_IOUT_PEAK = MAX34451_MFR_IOUT_PEAK,
	MAX34451_VAL_VOUT_PEAK = MAX34451_MFR_VOUT_PEAK,
	MAX34451_VAL_TEMP_PEAK = MAX34451_MFR_TEMPERATURE_PEAK,
	MAX34451_VAL_IOUT_AVG = MAX34451_MFR_IOUT_AVG,
};

enum max34451_status_type {
	MAX34451_STATUS_VOUT_TYPE = MAX34451_STATUS_VOUT_TYPE_MSK,
	MAX34451_STATUS_IOUT_TYPE = MAX34451_STATUS_IOUT_TYPE_MSK,
	MAX34451_STATUS_TEMP_TYPE = MAX34451_STATUS_TEMP_TYPE_MSK,
	MAX34451_STATUS_CML_TYPE = MAX34451_STATUS_CML_TYPE_MSK,
	MAX34451_STATUS_MFR_SPECIFIC_TYPE = MAX34451_STATUS_MFR_SPECIFIC_TYPE_MSK,
	MAX34451_STATUS_WORD_TYPE = MAX34451_STATUS_WORD_TYPE_MSK,
	MAX34451NA6_STATUS_INPUT_TYPE = MAX34451NA6_STATUS_INPUT_TYPE_MSK,
};

enum max34451_timing_type {
	MAX34451_TON_DELAY_TYPE = MAX34451_TON_DELAY,
	MAX34451_TOFF_DELAY_TYPE = MAX34451_TOFF_DELAY,
	MAX34451_RETRY_DELAY = MAX34451_MFR_FAULT_RETRY,
};

struct max34451_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *alert_desc;
	struct no_os_gpio_desc **pgood_descs;
	struct no_os_gpio_desc **run_descs;
	struct no_os_gpio_desc **fault_descs;

	enum max34451_chip_id id;
	int page;
	uint8_t num_channels;
};

struct max34451_init_param {
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_gpio_init_param *alert_param;
	struct no_os_gpio_init_param **pgood_param;
	struct no_os_gpio_init_param **run_param;
	struct no_os_gpio_init_param **fault_param;

	enum max34451_chip_id id;
};

struct max34451_chip_info {
	uint8_t num_channels;
};

struct max34451_status {
	uint16_t word;
	uint8_t vout;
	uint8_t iout;
	uint8_t temp;
	uint8_t cml;
	uint8_t mfr_specific;
	uint8_t input;
};

/* Initialize the device structure */
int max34451_init(struct max34451_dev **device,
		  struct max34451_init_param *init_param);

/* Free or remove device instance */
int max34451_remove(struct max34451_dev *dev);

/* Set PMBus page and phase */
int max34451_set_page(struct max34451_dev *dev, int page);

/* Send PMBus command to device */
int max34451_send_byte(struct max34451_dev *dev, int page, uint8_t cmd);

/* Perform a PMBus read_byte operation */
int max34451_read_byte(struct max34451_dev *dev, int page,
		       uint8_t cmd, uint8_t *data);

/* Perform a PMBus write_byte operation */
int max34451_write_byte(struct max34451_dev *dev, int page,
			uint8_t cmd, uint8_t value);

/* Perform a PMBus read_word operation */
int max34451_read_word(struct max34451_dev *dev, int page,
		       uint8_t cmd, uint16_t *word);

/* Perform a PMBus write_word operation */
int max34451_write_word(struct max34451_dev *dev, int page,
			uint8_t cmd, uint16_t word);

/* PMBus read word data with conversion to real values */
int max34451_read_word_data(struct max34451_dev *dev, int page,
			    uint8_t cmd, int *data);

/* PMBus write word data with conversion from real values */
int max34451_write_word_data(struct max34451_dev *dev, int page,
			     uint8_t cmd, int data);

/* Perform a PMBus read_block operation */
int max34451_read_block_data(struct max34451_dev *dev, int page, uint8_t cmd,
			     uint8_t *data, size_t nbytes);

/* Perform a PMBus write_block operation */
int max34451_write_block_data(struct max34451_dev *dev, int page, uint8_t cmd,
			      uint8_t *data, size_t nbytes);

/* Read specific value type */
int max34451_read_value(struct max34451_dev *dev,
			uint8_t channel,
			enum max34451_value_type value_type,
			int *value);

/* Read status */
int max34451_read_status(struct max34451_dev *dev,
			 uint8_t channel,
			 enum max34451_status_type status_type,
			 struct max34451_status *status);

/* Clear faults */
int max34451_clear_faults(struct max34451_dev *dev);

/* Set timing parameters (TON_DELAY, TOFF_DELAY, etc.) in ms */
int max34451_set_timing(struct max34451_dev *dev, uint8_t channel,
			enum max34451_timing_type timing_type, int timing);

/* Set VOUT margin */
int max34451_vout_margin(struct max34451_dev *dev, uint8_t channel,
			 int margin_low, int margin_high);

/* Set operation mode */
int max34451_set_operation(struct max34451_dev *dev, uint8_t channel,
			   enum max34451_operation_type operation);

/* Software reset */
int max34451_software_reset(struct max34451_dev *dev);

/* Update a specific bit or bit field in a PMBus command */
int max34451_update_bit(struct max34451_dev *dev, uint8_t channel,
			uint8_t cmd, uint32_t mask, uint32_t value);

/* Read a specific bit or bit field from a PMBus command */
int max34451_read_bit(struct max34451_dev *dev, uint8_t channel,
		      uint8_t cmd, uint32_t mask, uint32_t *value);

#endif /* __MAX34451_H__ */
