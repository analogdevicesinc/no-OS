/***************************************************************************//**
 *   @file   adp1055.h
 *   @brief  Header file for the ADP1055 Driver
 *   @author Ivangil Mercano (Ivangil.mercano@analog.com)
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
#ifndef __ADP_1055_H__
#define __ADP_1055_H__


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_pwm.h"
#include "no_os_util.h"
#include "no_os_units.h"

/*****************************************************************************/
/************************** ADP1055 Constants ********************************/
/*****************************************************************************/
#define ADP1055_WR_FRAME_SIZE			4
#define ADP1055_RD_FRAME_SIZE			5
#define ADP1055_SC_FRAME_SIZE			3
#define ADP1055_LSB_MASK			NO_OS_GENMASK(7, 0)
#define ADP1055_MSB_MASK			NO_OS_GENMASK(15, 8)
#define ADP1055_EXTENDED_COMMAND		0xFF

#define ADP1055_OPERATION_ON			0x80
#define ADP1055_OPERATION_OFF			0x00
#define ADP1055_OPERATION_SOFT_OFF		0x40

#define ADP1055_EXP_MASK			NO_OS_GENMASK(15, 11)
#define ADP1055_MANT_MASK			NO_OS_GENMASK(10, 0)

#define ADP1055_VOUT_MODE_MASK			NO_OS_GENMASK(4, 0)

#define ADP1055_PWM_TIMING_MASK			NO_OS_GENMASK(15, 4)
#define ADP1055_PWM_MODEN_MASK			3
#define ADP1055_PWM_SIGN_MASK			2

#define ADP1055_OUTA_ON				0x3E
#define ADP1055_OUTB_ON 			0x3D
#define ADP1055_OUTC_ON				0x3B
#define ADP1055_OUTD_ON 			0x37
#define ADP1055_SR1_ON 				0x2F
#define ADP1055_SR2_ON 				0x1F
#define ADP1055_PWM_OFF				0x3F

#define ADP1055_ON_OFF_DEFAULT_CFG		0x00
#define ADP1055_DEFAULT_FREQ_MANT		-4
#define ADP1055_DEFAULT_FREQ_EXP		782
#define ADP1055_CTRL_PIN_ENABLE			NO_OS_BIT(4)

/** CMD GO */
#define ADP1055_VOLT_REF_GO            		0x01
#define ADP1055_PWM_GO                  	0x02
#define ADP1055_FREQ_GO                 	0x04
#define ADP1055_FILTER_GO               	0x08
#define ADP1055_VS_BAL_GO               	0x10
#define ADP1055_VFF_GO                  	0x20
#define ADP1055_SYNC_GO                 	0x40

# define ADP1055_ON_OFF_MASK			NO_OS_GENMASK(7, 6)

#define ADP1055_FREQ_PLL_MASK			NO_OS_BIT(6)
#define ADP1055_FREQ_JITT_MASK			NO_OS_BIT(1)
#define ADP1055_FREQ_RESO_MASK			NO_OS_BIT(0)
#define ADP1055_FREQ_JITT_VAL			0x02

/* PMBus COMMAND SET */
/*****************************************************************************/
/************************** adp1055 Registers ********************************/
/*****************************************************************************/
#define ADP1055_OPERATION			0x01
#define ADP1055_ON_OFF_CONFIG			0x02
#define ADP1055_CLEAR_FAULTS			0x03
#define ADP1055_WRITE_PROTECT			0x10
#define ADP1055_RESTORE_DEFAULT_ALL		0x12
#define ADP1055_STORE_USER_ALL			0x15
#define ADP1055_RESTORE_USER_ALL		0x16
#define ADP1055_CAPABILITY			0x19

#define ADP1055_SMBALERT_MASK			0x1B
#define ADP1055_VOUT_MODE			0x20
#define ADP1055_VOUT_COMMAND			0x21
#define ADP1055_VOUT_TRIM			0x22
#define ADP1055_VOUT_CAL_OFFSET			0x23
#define ADP1055_VOUT_MAX			0x24
#define ADP1055_VOUT_TRANSITION_RATE		0x27
#define ADP1055_VOUT_DROOP			0x28
#define ADP1055_VOUT_SCALE_LOOP			0x29
#define ADP1055_VOUT_SCALE_MONITOR		0x2A
#define ADP1055_FREQUENCY_SWITCH		0x33

/** VIN ON OFF */
#define ADP1055_VIN_ON				0x35
#define ADP1055_VIN_OFF				0x36

#define ADP1055_INTERLEAVE			0x37
#define ADP1055_IOUT_CAL_GAIN			0x38
#define ADP1055_IOUT_CAL_OFFSET			0x39

/** Fault, Reponse, Limit, and Warn */
#define ADP1055_VOUT_OV_FAULT_LIMIT		0x40
#define ADP1055_VOUT_OV_FAULT_RESPONSE		0x41
#define ADP1055_VOUT_OV_WARN_LIMIT		0x42
#define ADP1055_VOUT_UV_WARN_LIMIT		0x43
#define ADP1055_VOUT_UV_FAULT_LIMIT		0x44
#define ADP1055_VOUT_UV_FAULT_RESPONSE		0x45
#define ADP1055_IOUT_OC_FAULT_LIMIT		0x46
#define ADP1055_IOUT_OC_FAULT_RES		0x47
#define ADP1055_IOUT_OC_LV_FAULT_LIMIT		0x48
#define ADP1055_IOUT_OC_LV_FAULT_RESPONSE	0x49
#define ADP1055_IOUT_OC_WARN_LIMIT		0x4A
#define ADP1055_IOUT_UC_FAULT_LIMIT		0x4B
#define ADP1055_IOUT_UC_FAULT_RESPONSE		0x4C
#define ADP1055_OT_FAULT_LIMIT			0x4F
#define ADP1055_OT_FAULT_RESPONSE		0x50
#define ADP1055_OT_WARN_LIMIT			0x51
#define ADP1055_VIN_OV_FAULT_LIMIT		0x55
#define ADP1055_VIN_OV_FAULT_RESPONSE		0x56
#define ADP1055_VIN_UV_FAULT_LIMIT		0x59
#define ADP1055_VIN_UV_FAULT_RESPONSE		0x5A
#define ADP1055_IIN_OC_FAULT_LIMIT		0x5B
#define ADP1055_IIN_OC_FAULT_RESPOSNE		0x5C

#define ADP1055_POWER_GOOD_ON			0x5E
#define ADP1055_POWER_GOOD_OFF			0x5F
#define ADP1055_TON_DELAY			0x60
#define ADP1055_TON_RISE			0x61
#define ADP1055_TON_MAX_FAULT_LIMIT		0x62
#define	ADP1055_TON_MAX_FAULT_RESPOSNE		0x63
#define ADP1055_TOFF_DELAY			0x64
#define ADP1055_TOFF_FALL			0x65
#define ADP1055_TOFF_MAX_WARN_LIMIT		0x66
#define ADP1055_POUT_OP_FAULT_LIMIT		0x68
#define ADP1055_POUT_OP_FAULT_RESPONSE		0x69

/** STATUS */
#define ADP1055_STATUS_BYTE			0x78
#define ADP1055_STATUS_WORD			0x79
#define ADP1055_STATUS_VOUT			0x7A
#define ADP1055_STATUS_IOUT			0x7B
#define ADP1055_STATUS_INPUT			0x7C
#define ADP1055_STATUS_TEMPERATURE		0x7D
#define ADP1055_STATUS_CML			0x7E
#define ADP1055_STATUS_OTHER			0x7F
#define ADP1055_STATUS_MFR_SPECIFIC		0x80

/** READ */
#define ADP1055_READ_VIN			0x88
#define ADP1055_READ_IIN			0x89
#define ADP1055_READ_VOUT			0x8B
#define ADP1055_READ_IOUT			0x8C
#define ADP1055_READ_TEMPERATURE_2		0x8E
#define ADP1055_READ_TEMPERATURE_3		0x8F
#define ADP1055_READ_DUTY_CYCLE			0x94
#define ADP1055_READ_FREQUENCY			0x95
#define ADP1055_READ_POUT			0x96

#define ADP1055_READ_PMBUS_REVISION		0x98
#define ADP1055_MFR_ID				0x99
#define ADP1055_MFR_MODEL			0x9A
#define ADP1055_MFR_REVISION			0x9B
#define ADP1055_MFR_LOCATION			0x9C
#define ADP1055_MFR_DATE			0x9D
#define ADP1055_IC_DEVICE_ID			0xAD
#define ADP1055_IC_DEVICE_REV			0xAE
#define ADP1055_EEPROM_DATA_00			0xB0
#define ADP1055_EEPROM_DATA_01			0xB1
#define ADP1055_EEPROM_DATA_02			0xB2
#define ADP1055_EEPROM_DATA_03			0xB3
#define ADP1055_EEPROM_DATA_04			0xB4
#define ADP1055_EEPROM_DATA_05			0xB5
#define ADP1055_EEPROM_DATA_06			0xB6
#define ADP1055_EEPROM_DATA_07			0xB7
#define ADP1055_EEPROM_DATA_08			0xB8
#define ADP1055_EEPROM_DATA_09			0xB9
#define ADP1055_EEPROM_DATA_10			0xBA
#define ADP1055_EEPROM_DATA_11			0xBB
#define ADP1055_EEPROM_DATA_12			0xBC
#define ADP1055_EEPROM_DATA_13			0xBD
#define ADP1055_EEPROM_DATA_14			0xBE
#define ADP1055_EEPROM_DATA_15			0xBF
#define ADP1055_EEPROM_CRC_CHKSUM		0xD1
#define ADP1055_EEPROM_NUM_RD_BYTES		0xD2
#define ADP1055_EEPROM_ADDR_OFFSET		0xD3
#define ADP1055_EEPROM_PAGE_ERASE		0xD4
#define ADP1055_EEPROM_PASSWORD			0xD5
#define ADP1055_TRIM_PASSWORD			0xD6
#define ADP1055_CHIP_PASSWORD			0xD7
#define ADP1055_IIN_SCALE_MONITOR		0xD9
#define ADP1055_EEPROM_INFO			0xF1
#define ADP1055_READ_BLACKBOX_CURR		0xF2
#define ADP1055_READ_BLACKBOX_PREV		0xF3
#define ADP1055_CMD_MASK			0xF4
#define ADP1055_EXTCMD_MASK			0xF5
#define ADP1055_MFR_SPECIFIC_1			0xFA
#define ADP1055_MFR_SPECIFIC_2			0xFB

/* MANUFACTURER SPECIFIC EXTENDED COMMAND LIST */

/** Go Command */
#define ADP1055_GO_CMD				0xFE00

/** NM LLM and SS Digfilt */
#define ADP1055_NM_DIGFILT_LF_GAIN_SETTING 	0xFE01
#define ADP1055_NM_DIGFILT_ZERO_SETTING 	0xFE02
#define ADP1055_NM_DIGFILT_POLE_SETTING  	0xFE03
#define ADP1055_NM_DIGFILT_HF_GAIN_SETTING	0xFE04

#define ADP1055_LLM_DIGFILT_LF_GAIN_SETTING 	0xFE05
#define ADP1055_LLM_DIGFILT_ZERO_SETTING	0xFE06
#define ADP1055_LLM_DIGFILT_POLE_SETTING 	0xFE07
#define ADP1055_LLM_DIGFILT_HF_GAIN_SETTING	0xFE08

#define ADP1055_SS_DIGFILT_LF_GAIN_SETTING 	0xFE09
#define ADP1055_SS_DIGFILT_ZERO_SETTING 	0xFE0A
#define ADP1055_SS_DIGFILT_POLE_SETTING 	0xFE0B
#define ADP1055_SS_DIGFILT_HF_GAIN_SETTING 	0xFE0C

/** OUTA, OUTB, OUTC, OUTC, SR1, and SR2 Settings */
#define ADP1055_OUTA_REDGE_SETTING		0xFE0D
#define ADP1055_OUTA_FEDGE_SETTING 		0xFE0E
#define ADP1055_OUTB_REDGE_SETTING 		0xFE0F
#define ADP1055_OUTB_FEDGE_SETTING 		0xFE10
#define ADP1055_OUTC_REDGE_SETTING		0xFE11
#define ADP1055_OUTC_FEDGE_SETTING		0xFE12
#define ADP1055_OUTD_REDGE_SETTING 		0xFE13
#define ADP1055_OUTD_FEDGE_SETTING 		0xFE14
#define ADP1055_SR1_REDGE_SETTING 		0xFE15
#define ADP1055_SR1_FEDGE_SETTING		0xFE16
#define ADP1055_SR2_REDGE_SETTING 		0xFE17
#define ADP1055_SR2_FEDGE_SETTING 		0xFE18
#define ADP1055_SR1_REDGE_LLM_SETTING 		0xFE19
#define ADP1055_SR1_FEDGE_LLM_SETTING		0xFE1A
#define ADP1055_SR2_REDGE_LLM_SETTING 		0xFE1B
#define ADP1055_SR2_FEDGE_LLM_SETTING 		0xFE1C

/** ADT Setting */
#define ADP1055_ADT_CONFIG 			0xFE1D
#define ADP1055_ADT_THRESHOLD			0xFE1E

/** PWM Output Dead Time */
#define ADP1055_OUTA_DEAD_TIME 			0xFE1F
#define ADP1055_OUTB_DEAD_TIME			0xFE20
#define ADP1055_OUTC_DEAD_TIME			0xFE21
#define ADP1055_OUTD_DEAD_TIME			0xFE22
#define ADP1055_SR1_DEAD_TIME 			0xFE23
#define ADP1055_SR2_DEAD_TIME 			0xFE24

/** VSBAL Settings */
#define ADP1055_VSBAL_SETTING			0xFE25
#define ADP1055_VSBAL_OUTA_B 			0xFE26
#define ADP1055_VSBAL_OUTC_D			0xFE27
#define ADP1055_VSBAL_SR1_2			0xFE28

/** FFWD Setting */
#define ADP1055_FFWD_SETTING			0xFE29

/* ISHARE Setting */
#define ADP1055_ISHARE_SETTING			0xFE2A
#define ADP1055_ISHARE_BANDWIDTH 		0xFE2B

/** Fast Setting */
#define ADP1055_IIN_OC_FAST_SETTING		0xFE2C
#define ADP1055_IOUT_OC_FAST_SETTING 		0xFE2D
#define ADP1055_IOUT_UC_FAST_SETTING		0xFE2E
#define ADP1055_VOUT_OV_FAST_SETTING		0xFE2F

/* Debounce Settings */
#define ADP1055_DEBOUNCE_SETTING_1		0xFE30
#define ADP1055_DEBOUNCE_SETTING_2		0xFE31
#define ADP1055_DEBOUNCE_SETTING_3 		0xFE32
#define ADP1055_DEBOUNCE_SETTING_4 		0xFE33

/** Fault Response  */
#define ADP1055_VOUT_OV_FAST_FAULT_RESPONSE	0xFE34
#define ADP1055_IOUT_OC_FAST_FAULT_RESPONSE	0xFE35
#define ADP1055_IOUT_UC_FAST_FAULT_RESPONSE	0xFE36
#define ADP1055_IIN_OC_FAST_FAULT_RESPONSE 	0xFE37
#define ADP1055ISHARE_FAULT_RESPONSE 		0xFE38
#define ADP1055_GPIO1_FAULT_RESPONSE		0xFE39
#define ADP1055_GPIO2_FAULT_RESPONSE 		0xFE3A
#define ADP1055_GPIO3_FAULT_RESPONSE 		0xFE3B
#define ADP1055_GPIO4_FAULT_RESPONSE		0xFE3C

/** PWM Fault Mask */
#define ADP1055_PWM_FAULT_MASK			0xFE3D

/** Delay Time Unit */
#define ADP1055_DELAY_TIME_UNIT 		0xFE3E

/** WDT Setting */
#define ADP1055_WDT_SETTING			0xFE3F

/** GPIO Setting */
#define ADP1055_GPIO_SETTING			0xFE40
#define ADP1055GPIO1_2_KARNAUGH_MAP		0xFE41
#define ADP1055_GPIO3_4_KARNAUGH_MAP		0xFE42

/** PGOOD Fault Setting */
#define ADP1055_PGOOD_FAULT_DEB			0xFE43
#define ADP1055_PGOOD1_FAULT_SELECT		0xFE44
#define ADP1055_PGOOD2_FAULT_SELECT		0xFE45

/** Soft Start */
#define ADP1055_SOFT_START_BLANKING		0xFE46
#define ADP1055_SOFT_STOP_BLANKING		0xFE47

/** Blackbox Setting */
#define ADP1055_BLACKBOX_SETTING		0xFE48

/** PWM Disable Setting */
#define ADP1055_PWM_DISABLE_SETTING		0xFE49

/** Filter Transition */
#define ADP1055_FILTER_TRANSITION		0xFE4A

/** Deep Light Load Mode Setting */
#define ADP1055_DEEP_LLM_SETTING		0xFE4B
#define ADP1055_DEEP_LLM_DISABLE_SETTING	0xFE4C

/** OVP Fault Config */
#define ADP1055_OVP_FAULT_CONFIG		0xFE4D

/** CS1 and CS2 */
#define ADP1055_CS1_SETTING			0xFE4E
#define ADP1055_CS2_SETTING			0xFE4F

/** Pulse Skiping and Shutdown */
#define ADP1055_PULSE_SKIP_AND_SHUTDOWN		0xFE50

/** Soft Start Setting */
#define ADP1055_SOFT_START_SETTING		0xFE51

/** SR Delay */
#define ADP1055_SR_DELAY			0xFE52

/** Modulation Limit */
#define ADP1055_MODULATION_LIMIT		0xFE53

/** SYNC */
#define ADP1055_SYNC				0xFE55

/** Duty Balance Edgesel */
#define ADP1055_DUTY_BAL_EDGESEL		0xFE56

/** Double UPD Rate */
#define ADP1055_DOUBLE_UPD_RATE			0xFE57

/** Vin Scale Monitor */
#define ADP1055_VIN_SCALE_MONITOR		0xFE58

/* IIN Cal Gain */
#define ADP1055_IIN_CAL_GAIN			0xFE59

/** TSNS Setting */
#define ADP1055_TSNS_SETTING			0xFE5A

/* Auto go CMD */
#define ADP1055_AUTO_GO_CMD			0xFE5B

/** Diode Emulation */
#define ADP1055_DIODE_EMULATION			0xFE5C

/** CS2 Constant Current Mode */
#define ADP1055_CS2_CONST_CUR_MODE		0xFE5D

/** NL ERR Gain Factor */
#define ADP1055_NL_ERR_GAIN_FACTOR		0xFE5E

/** SR Setting */
#define ADP1055_SR_SETTING			0xFE5F

/** Temperature Polarity */
#define ADP1055_NOMINAL_TEMP_POLE		0xFE60
#define ADP1055_LOW_TEMP_POLE			0xFE61
#define ADP1055_LOW_TEMP_SETTING		0xFE62

/** GPIO3 Snubber */
#define ADP1055_GPIO3_4_SNUBBER_ON_TIME		0xFE63
#define ADP1055_GPIO3_4_SNUBBER_DELAY		0xFE64

/** VOUT Droop Setting */
#define ADP1055_VOUT_DROOP_SETTING		0xFE65

/** NL Burst Mode */
#define ADP1055_NL_BURST_MODE			0xFE66

/** HF ADC Config */
#define ADP1055_HF_ADC_CONFIG			0xFE67

/** VS Trim */
#define ADP1055_VS_TRIM				0xFE80

/** VFF Gain Trim */
#define ADP1055_VFF_GAIN_TRIM			0xFE81

/** CS1 Gain Trim */
#define ADP1055_CS1_GAIN_TRIM			0xFE82

/** TSNS Settings */
#define ADP1055_TSNS_EXTFWD_GAIN_TRIM		0xFE86
#define ADP1055_TSNS_EXTFWD_OFFSET_TRIM		0xFE87
#define ADP1055_TSNS_EXTREV_GAIN_TRIM		0xFE88
#define ADP1055_TSNS_EXTREV_OFFSET_TRIM		0xFE89

/** Faults */
#define ADP1055_FAULT_VOUT			0xFE8C
#define ADP1055_FAULT_IOUT			0xFE8D
#define ADP1055_FAULT_INPUT			0xFE8E
#define ADP1055_FAULT_TEMPERATURE		0xFE8F
#define ADP1055_FAULT_CML			0xFE90
#define ADP1055_FAULT_OTHER			0xFE91
#define ADP1055_FAULT_MFR_SPECIFIC		0xFE92
#define ADP1055_FAULT_UNKNOWN			0xFE93
#define ADP1055_STATUS_UNKNOWN			0xFE94
#define ADP1055_FIRST_FAULT_ID			0xFE95

/** Values */
#define ADP1055_VFF_VALUE			0xFE96
#define ADP1055_VS_VALUE			0xFE97
#define ADP1055_CS1_VALUE			0xFE98
#define ADP1055_CS2_VALUE			0xFE99
#define ADP1055_POUT_VALUE			0xFE9A
#define ADP1055_TSNS_EXTFWD_VALUE		0xFE9C
#define ADP1055_TSNS_EXTREV_VALUE		0xFE9D
#define ADP1055_MODULATION_VALUE		0xFE9F
#define ADP1055_ISHARE_VALUE			0xFEA0
#define ADP1055_ADD_ADC_VALUE			0xFEA3

enum adp1055_status_type {
	ADP1055_STATUS_VOUT_TYPE = ADP1055_STATUS_VOUT,
	ADP1055_STATUS_IOUT_TYPE = ADP1055_STATUS_IOUT,
	ADP1055_STATUS_INPUT_TYPE = ADP1055_STATUS_INPUT,
	ADP1055_STATUS_TEMP_TYPE = ADP1055_STATUS_TEMPERATURE,
	ADP1055_STATUS_CML_TYPE = ADP1055_STATUS_CML,
	ADP1055_STATUS_OTHER_TYPE = ADP1055_STATUS_OTHER,
	ADP1055_STATUS_WORD_TYPE = ADP1055_STATUS_WORD,
	ADP1055_STATUS_MFR_TYPE = ADP1055_STATUS_MFR_SPECIFIC,
};

enum adp1055_value_type {
	ADP1055_VALUE_VIN = ADP1055_READ_VIN,
	ADP1055_VALUE_IIN = ADP1055_READ_IIN,
	ADP1055_VALUE_IOUT = ADP1055_READ_IOUT,
	ADP1055_VALUE_TEMP2 = ADP1055_READ_TEMPERATURE_2,
	ADP1055_VALUE_TEMP3 = ADP1055_READ_TEMPERATURE_3,
	ADP1055_VALUE_DUTY_CYCLE = ADP1055_READ_DUTY_CYCLE,
	ADP1055_VALUE_FREQUENCY = ADP1055_READ_FREQUENCY,
	ADP1055_VALUE_POUT = ADP1055_READ_POUT,
};

enum adp1055_channel {
	ADP1055_OUTA,
	ADP1055_OUTB,
	ADP1055_OUTC,
	ADP1055_OUTD,
	ADP1055_SR1,
	ADP1055_SR2,
	ADP1055_DISABLE_ALL,
};

/**
 * @brief Initialization parameter for the ADP1055 device.
*/
struct adp1055_init_param {
	struct no_os_i2c_init_param *i2c_param;
	struct no_os_gpio_init_param *pg_alt_param;
	struct no_os_pwm_init_param *syni_param;
	struct no_os_gpio_init_param *flgi_param;
	struct no_os_gpio_init_param *ctrl_param;
	uint8_t on_off_config;
	bool ext_syni;
};

/**
 * @brief Device descriptor for ADP1055.
*/
struct adp1055_desc {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *pg_alt_desc;
	struct no_os_pwm_desc *syni_desc;
	struct no_os_gpio_desc *flgi_desc;
	struct no_os_gpio_desc *ctrl_desc;
	uint16_t freq_mant;
	uint8_t freq_exp;
};

/** Send command byte/word to ADP1055 */
int adp1055_send_command(struct adp1055_desc *desc, uint16_t command);

/** Write data to ADP1055 */
int adp1055_write(struct adp1055_desc *desc, uint16_t command, uint16_t data,
		  uint8_t byte_num);

/** Read data from ADP1055 */
int adp1055_read(struct adp1055_desc *desc, uint16_t command, uint8_t *data,
		 uint8_t bytes_number);

/** Initialize the ADP1055 device. */
int adp1055_init(struct adp1055_desc **desc,
		 struct adp1055_init_param *init_param);

/** Free the resources allocated by the adp1055_init() */
int adp1055_remove(struct adp1055_desc *desc);

/** Send statuses  */
int adp1055_read_status(struct adp1055_desc *desc,
			enum adp1055_status_type status,
			uint16_t *status_val);

/** Read VIN/IIN/VOUT/TEMP2/TEMP3/DUTY_CYCLE_FREQ/POUT raw value from the ADP1055 */
int adp1055_read_value(struct adp1055_desc *desc, enum adp1055_value_type value,
		       uint16_t *mant, uint8_t *exp);

/** Read Voltage Sense output raw value from the ADP1055 */
int adp1055_read_vsense(struct adp1055_desc *desc, uint16_t *vsense);

/** Set VOUT_COMMAND and VOUT_MAX values */
int adp1055_vout_value(struct adp1055_desc *desc, uint16_t vout_command,
		       uint16_t vout_max);

/** Set output voltage offset */
int adp1055_vout_offset(struct adp1055_desc *desc, int16_t vout_offset);

/** Set ADP1055 VOUT transition rate */
int adp1055_vout_tr(struct adp1055_desc *desc, uint8_t exp, uint16_t mant);

/** Set ADP1055 Vout Droop */
int adp1055_vout_droop(struct adp1055_desc *desc, uint8_t exp, uint16_t mant);

/** Set ADP1055 vout scale loop */
int adp1055_vout_scale_loop(struct adp1055_desc *desc, uint8_t exp,
			    uint16_t mant);

/** Set ADP1055 VOUT_SCALE_MONITOR */
int adp1055_vout_scale(struct adp1055_desc *desc, uint8_t exp, uint16_t mant);

/** Set ADP1055 VIN on/off raw value for input voltage limiting. */
int adp1055_set_vin(struct adp1055_desc *desc, int16_t mant, int8_t exp,
		    bool state_on);

/** Set ADP1055 IOUT calibration gain */
int adp1055_iout_cal_gain(struct adp1055_desc *desc, int16_t mant, int8_t exp);

/** Set ADP1055 IOUT calibration offset */
int adp1055_iout_cal_offset(struct adp1055_desc *desc, int16_t mant,
			    int8_t exp);

/** Set ADP1055 Normal Mode Digital Filter */
int adp1055_normal_mode_df(struct adp1055_desc *desc, uint8_t zero,
			   uint8_t pole,
			   uint8_t lf, uint8_t hf);

/** Set ADP1055 Light Load Mode Digital Filter */
int adp1055_lightload_mode_df(struct adp1055_desc *desc, uint8_t zero,
			      uint8_t pole,
			      uint8_t lf, uint8_t hf);

/** Set ADP1055 Single Shot Digital Filter */
int adp1055_singleshot_mode_df(struct adp1055_desc *desc, uint8_t zero,
			       uint8_t pole,
			       uint8_t lf, uint8_t hf);

/** PWM modulation configuration for the ADP1055 */
int adp1055_pwm_config(struct adp1055_desc * desc, uint16_t pulse_width,
		       uint16_t pulse_start, bool mod_en, bool mod_sign, enum adp1055_channel chan);

/** Set PWM channel and frequency */
int adp1055_set_pwm(struct adp1055_desc * desc, enum adp1055_channel chan,
		    int8_t exp, uint16_t mant);

/** Set Frequency Synchronization */
int adp1055_freq_sync(struct adp1055_desc * desc, bool pll, bool reso);

#endif /** __ADP_1050_H__ */
