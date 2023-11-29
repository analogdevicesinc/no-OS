/***************************************************************************//**
 *   @file   adp1050.h
 *   @brief  Header file for the ADP1050 Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#ifndef __ADP_1050_H__
#define __ADP_1050_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_pwm.h"
#include "no_os_util.h"
#include "no_os_units.h"

#define ADP1050_EXTENDED_COMMAND		0xFF
#define ADP1050_WRITE_BYTE_MAX_VAL		0xFF

#define ADP1050_LSB_MASK			NO_OS_GENMASK(7, 0)
#define ADP1050_MSB_MASK			NO_OS_GENMASK(15, 8)

#define ADP1050_ON_OFF_DEFAULT_CFG		0x00
#define ADP1050_ON_OFF_CTRL_HIGH_CFG		0x1E
#define ADP1050_ON_OFF_CTRL_LOW_CFG		0x1C

#define ADP1050_CTRL_PIN_ENABLE			NO_OS_BIT(4)

#define ADP1050_SOFTWARE_RESET_ON		0x01
#define ADP1050_SOFTWARE_RESET_OFF		0x00

#define ADP1050_SW_RES_DELAY_0MS		0x00
#define ADP1050_SW_RES_DELAY_500MS		0x01
#define ADP1050_SW_RES_DELAY_1S			0x02
#define ADP1050_SW_RES_DELAY_2S			0x03

#define ADP1050_SW_RES_NO_DELAY			0x00
#define ADP1050_SW_RES_TOFF_DELAY		0x04

#define ADP1050_TOFF_DELAY_0MS			0x0000
#define ADP1050_TOFF_DELAY_50MS			0x0032
#define ADP1050_TOFF_DELAY_250MS		0x00FA
#define ADP1050_TOFF_DELAY_1000MS		0x03E8

#define ADP1050_OPERATION_ON			0x80
#define ADP1050_OPERATION_OFF			0x00
#define ADP1050_OPERATION_SOFT_OFF		0x40

#define ADP1050_MARGIN_OFF			0x00
#define ADP1050_MARGIN_LOW			0x10
#define ADP1050_MARGIN_HIGH			0x20

#define ADP1050_MANT_MAX			0x7FF
#define ADP1050_EXP_MAX				0x1F
#define ADP1050_EXP_MASK			NO_OS_GENMASK(15, 11)
#define ADP1050_MANT_MASK			NO_OS_GENMASK(10, 0)

#define ADP1050_VOUT_EXP_MASK			NO_OS_GENMASK(4, 0)
#define ADP1050_VOUT_GO_COMMAND			0x06
#define ADP1050_VOUT_OFF			0x00

#define ADP1050_OUTA_FALLING_EDGE_NEGATIVE_MOD	0x03
#define ADP1050_OUTB_FALLING_EDGE_NEGATIVE_MOD	0x30
#define ADP1050_OUTA_ON				0x32
#define ADP1050_OUTB_ON				0x31
#define ADP1050_SR1_ON				0x23
#define ADP1050_SR2_ON				0x13
#define ADP1050_ALL_PWM_OFF			0x33
#define ADP1050_EDGE_MAX_VAL			0x0FFF
#define ADP1050_PULSE_MAX_VAL			0xFFFF
#define ADP1050_EDGE_MSB_MASK			NO_OS_GENMASK(7, 0)
#define ADP1050_RISING_EDGE_LSB_MASK		NO_OS_GENMASK(7, 4)
#define ADP1050_FALLING_EDGE_LSB_MASK		NO_OS_GENMASK(3, 0)

#define ADP1050_CS1_CLIMIT_COMP_IGNORE		0x80
#define ADP1050_CS1_LEADING_EDGE_MASK		NO_OS_GENMASK(6, 4)
#define ADP1050_CS1_DEBOUNCE_MASK		NO_OS_GENMASK(1, 0)

#define ADP1050_OUTA_OL_ENABLE			NO_OS_BIT(0)
#define ADP1050_OUTB_OL_ENABLE			NO_OS_BIT(1)
#define ADP1050_SR1_OL_ENABLE			NO_OS_BIT(4)
#define ADP1050_SR2_OL_ENABLE			NO_OS_BIT(5)

#define ADP1050_OUTB_SR2_RISING_MOD_MASK	NO_OS_GENMASK(7, 6)
#define ADP1050_OUTB_SR2_FALLING_MOD_MASK	NO_OS_GENMASK(5, 4)
#define ADP1050_OUTA_SR1_RISING_MOD_MASK	NO_OS_GENMASK(3, 2)
#define ADP1050_OUTA_SR1_FALLING_MOD_MASK	NO_OS_GENMASK(1, 0)

#define ADP1050_POSITIVE_MOD_SIGN		0x02
#define ADP1050_NEGATIVE_MOD_SIGN		0x03

#define ADP1050_OPEN_LOOP_ENABLE		NO_OS_GENMASK(5, 0)
#define ADP1050_OPEN_LOOP_DISABLE		0x00

#define ADP1050_OL_SS_1_CYCLE			0x84
#define ADP1050_OL_SS_4_CYCLES			0x8C
#define APD1050_OL_SS_16_CYCLES			0x94
#define ADP1050_OL_SS_64_CYCLES			0x9C

#define ADP1050_FREQ_SYNC_ON			0x48
#define ADP1050_FREQ_SYNC_OFF			0x49
#define ADP1050_MAX_PERIOD_INTEGER(x)		((1000 / (x)) * 1000)
#define ADP1050_MAX_PERIOD_FLOAT(x)		((1000 / (((x) - 0xF800) / 2)) * 1000)
#define ADP1050_FLOAT_FREQ_MASK			NO_OS_BIT(15)

#define ADP1050_CHECK_CHIP_PASS_MASK		NO_OS_BIT(7)
#define ADP1050_CHIP_DEFAULT_PASS		0xFFFF
#define ADP1050_CHECK_EEPROM_PASS_MASK		NO_OS_BIT(3)
#define ADP1050_EEPROM_DEFAULT_PASS		0xFF
#define ADP1050_TRIM_DEFAULT_PASS		0xFF

#define ADP1050_VOUT_GO				0x06

#define ADP1050_FAULT_RESPONSE_MASK		NO_OS_GENMASK(3, 2)
#define ADP1050_AFTER_FAULT_MASK		NO_OS_GENMASK(1, 0)

#define ADP1050_FEEDFORWARD_OFF			0x00
#define ADP1050_CL_FEEDFORWARD_DISABLE		NO_OS_BIT(3)
#define ADP1050_FEEDFORWARD_CL_ENABLE		NO_OS_GENMASK(3, 2)
#define ADP1050_FEEDFORWARD_OL_ENABLE		(NO_OS_BIT(6) | NO_OS_BIT(3))

#define ADP1050_PULSE_SKIPPING_ENABLE		NO_OS_GENMASK(6, 0)
#define ADP1050_PULSE_SKIPPING_DISABLE		NO_OS_GENMASK(5, 0)

#define ADP1050_CS1_TRIM_VAL			0xA00
#define ADP1050_CS1_MAX_INIT_VAL		0xFF
#define ADP1050_VF_TRIM_VAL			0x500
#define ADP1050_VF_MAX_INIT_VAL			0xFF
#define ADP1050_VS_OFFSET_MAX_INIT_VAL		0x7FFF
#define ADP1050_VS_MAX_INIT_VAL			0xFF
#define ADP1050_VS_INIT_MASK			NO_OS_GENMASK(14, 0)

/* PMBus Addresses */
#define ADP1050_PMBUS_10KOHM_ADDRESS		0x70
#define ADP1050_PMBUS_31KOHM_ADDRESS		0x71
#define ADP1050_PMBUS_51KOHM_ADDRESS		0x72
#define ADP1050_PMBUS_71KOHM_ADDRESS		0x73
#define ADP1050_PMBUS_90KOHM_ADDRESS		0x74
#define ADP1050_PMBUS_110KOHM_ADDRESS		0x75
#define ADP1050_PMBUS_130KOHM_ADDRESS		0x76
#define ADP1050_PMBUS_150KOHM_ADDRESS		0x77

/* TON Delay Values */
#define ADP1050_TON_DELAY_0MS			0x0000
#define ADP1050_TON_DELAY_10MS			0x000A
#define ADP1050_TON_DELAY_25MS			0x0019
#define ADP1050_TON_DELAY_50MS			0x0032
#define ADP1050_TON_DELAY_75MS			0x004B
#define ADP1050_TON_DELAY_100MS			0x0064
#define ADP1050_TON_DELAY_250MS			0x00FA
#define ADP1050_TON_DELAY_1000MS		0x03E8

/* TON Rise Values */
#define ADP1050_TON_RISE_50US			0xC00D
#define ADP1050_TON_RISE_200US			0xD00D
#define ADP1050_TON_RISE_1750US			0xF007
#define ADP1050_TON_RISE_10MS			0xF815
#define ADP1050_TON_RISE_21MS			0x0015
#define ADP1050_TON_RISE_40MS			0xF0A1
#define ADP1050_TON_RISE_60MS			0x003C
#define ADP1050_TON_RISE_100MS			0x0064

/* PMBus COMMAND SET */
#define ADP1050_OPERATION			0x01
#define ADP1050_ON_OFF_CONFIG			0x02
#define ADP1050_CLEAR_FAULTS			0x03
#define ADP1050_WRITE_PROTECT			0x10
#define ADP1050_RESTORE_DEFAULT_ALL		0x12
#define ADP1050_STORE_USER_ALL			0x15
#define ADP1050_RESTORE_USER_ALL		0x16
#define ADP1050_CAPABILITY			0x19
#define ADP1050_VOUT_MODE			0x20
#define ADP1050_VOUT_COMMAND			0x21
#define ADP1050_VOUT_TRIM			0x22
#define ADP1050_VOUT_CAL_OFFSET			0x23
#define ADP1050_VOUT_MAX			0x24
#define ADP1050_VOUT_MARGIN_HIGH		0x25
#define ADP1050_VOUT_MARGIN_LOW			0x26
#define ADP1050_VOUT_TRANSITION_RATE		0x27
#define ADP1050_VOUT_SCALE_LOOP			0x29
#define ADP1050_VOUT_SCALE_MONITOR		0x2A
#define ADP1050_FREQUENCY_SWITCH		0x33
#define ADP1050_VIN_ON				0x35
#define ADP1050_VIN_OFF				0x36
#define ADP1050_VOUT_OV_FAULT_LIMIT		0x40
#define ADP1050_VOUT_OV_FAULT_RESPONSE		0x41
#define ADP1050_VOUT_UV_FAULT_LIMIT		0x44
#define ADP1050_VOUT_UV_FAULT_RESPONSE		0x45
#define ADP1050_OT_FAULT_LIMIT			0x4F
#define ADP1050_OT_FAULT_RESPONSE		0x50
#define ADP1050_POWER_GOOD_ON			0x5E
#define ADP1050_POWER_GOOD_OFF			0x5F
#define ADP1050_TON_DELAY			0x60
#define ADP1050_TON_RISE			0x61
#define ADP1050_TOFF_DELAY			0x64
#define ADP1050_STATUS_BYTE			0x78
#define ADP1050_STATUS_WORD			0x79
#define ADP1050_STATUS_VOUT			0x7A
#define ADP1050_STATUS_INPUT			0x7C
#define ADP1050_STATUS_TEMPERATURE		0x7D
#define ADP1050_STATUS_CML			0x7E
#define ADP1050_READ_VIN			0x88
#define ADP1050_READ_IIN			0x89
#define ADP1050_READ_VOUT			0x8B
#define ADP1050_READ_TEMPERATURE		0x8D
#define ADP1050_READ_DUTY_CYCLE			0x94
#define ADP1050_READ_FREQUENCY			0x95
#define ADP1050_READ_PMBUS_REVISION		0x98
#define ADP1050_MFR_ID				0x99
#define ADP1050_MFR_MODEL			0x9A
#define ADP1050_MFR_REVISION			0x9B
#define ADP1050_IC_DEVICE_ID			0xAD
#define ADP1050_IC_DEVICE_REV			0xAE
#define ADP1050_EEPROM_DATA_00			0xB0
#define ADP1050_EEPROM_DATA_01			0xB1
#define ADP1050_EEPROM_DATA_02			0xB2
#define ADP1050_EEPROM_DATA_03			0xB3
#define ADP1050_EEPROM_DATA_04			0xB4
#define ADP1050_EEPROM_DATA_05			0xB5
#define ADP1050_EEPROM_DATA_06			0xB6
#define ADP1050_EEPROM_DATA_07			0xB7
#define ADP1050_EEPROM_DATA_08			0xB8
#define ADP1050_EEPROM_DATA_09			0xB9
#define ADP1050_EEPROM_DATA_10			0xBA
#define ADP1050_EEPROM_DATA_11			0xBB
#define ADP1050_EEPROM_DATA_12			0xBC
#define ADP1050_EEPROM_DATA_13			0xBD
#define ADP1050_EEPROM_DATA_14			0xBE
#define ADP1050_EEPROM_DATA_15			0xBF
#define ADP1050_EEPROM_CRC_CHKSUM		0xD1
#define ADP1050_EEPROM_NUM_RD_BYTES		0xD2
#define ADP1050_EEPROM_ADDR_OFFSET		0xD3
#define ADP1050_EEPROM_PAGE_ERASE		0xD4
#define ADP1050_EEPROM_PASSWORD			0xD5
#define ADP1050_TRIM_PASSWORD			0xD6
#define ADP1050_CHIP_PASSWORD			0xD7
#define ADP1050_VIN_SCALE_MONITOR		0xD8
#define ADP1050_IIN_SCALE_MONITOR		0xD9
#define ADP1050_EEPROM_INFO			0xF1
#define ADP1050_MFR_SPECIFIC_1			0xFA
#define ADP1050_MFR_SPECIFIC_2			0xFB

/* MANUFACTURER SPECIFIC EXTENDED COMMAND LIST */

/* Flag Configuration Registers */
#define ADP1050_IIN_OC_FAST_FAULT_RESPONSE	0xFE00
#define ADP1050_CS3_OC_FAULT_RESPONSE		0xFE01
#define ADP1050_VIN_UV_FAULT_RESPONSE		0xFE02
#define ADP1050_FLAGIN_RESPONSE			0xFE03
#define ADP1050_VDD_OV_RESPONSE			0xFE05

/* Soft Start Software Reset Setting Registers */
#define ADP1050_SOFTWARE_RESET_GO		0xFE06
#define ADP1050_SOFTWARE_RESET_SETTINGS		0xFE07
#define ADP1050_SR_SOFT_START_SETTINGS		0xFE08
#define ADP1050_SOFT_START_SETTING_OL		0xFE09

/* Blanking and PGOOD Setting Registers */
#define ADP1050_FLAG_BLANKING_DURING_SS		0xFE0B
#define ADP1050_VS_BAL_BLANK_AND_SS_DISABLE	0xFE0C
#define ADP1050_PGOOD_MASK_SETTINGS		0xFE0D
#define ADP1050_PGOOD_FLAG_DEBOUNCE		0xFE0E
#define ADP1050_DEBOUNCE_TIME_PGOOD		0xFE0F

/* Switching Frequency and Synchronization Setting Registers */
#define ADP1050_SYNCH_DELAY_TIME		0xFE11
#define ADP1050_SYNCH_GENERAL_SETTINGS		0xFE12
#define ADP1050_DUAL_END_TOPOLOGY_MODE		0xFE13

/* Current Sense and Limit Setting Registers */
#define ADP1050_CS1_GAIN_TRIM			0xFE14
#define ADP1050_CS3_OC_DEBOUNCE			0xFE19
#define ADP1050_IIN_OC_FAST_FAULT_LIMIT		0xFE1A
#define ADP1050_CS1_CBC_CURR_LIM_REF		0xFE1B
#define ADP1050_MATCHED_CBC_CURR_LIM_SETTINGS	0xFE1D
#define ADP1050_SR1_SR2_RESPONSE_CBC_CURR_LIM	0xFE1E
#define ADP1050_CS1_CBC_CURR_LIM_SETTINGS	0xFE1F

/* Voltage Sense and Limit Setting Registers */
#define ADP1050_VS_GAIN_TRIM			0xFE20
#define ADP1050_PREBIAS_START_UP_ENABLE		0xFE25
#define ADP1050_VOUT_OV_FAULT_FLAGE_DEBOUNCE	0xFE26
#define ADP1050_VF_GAIN_TRIM			0xFE28
#define ADP1050_VIN_ON_AND_VIN_OFF_DELAY	0xFE29

/* Temperature Sense and Protection Setting Registers */
#define ADP1050_RTD_GAIN_TRIM			0xFE2A
#define ADP1050_RTD_OFFSET_TRIM_MSB		0xFE2B
#define ADP1050_RTD_CURRENT_SOURCE_SETTINGS	0xFE2D
#define ADP1050_OT_HYSTERESIS_SETTINGS		0xFE2F

/* Digital Compensator and Modulation Setting Registers */
#define ADP1050_NORMAL_MODE_COMP_LOW_FREQ	0xFE30
#define ADP1050_NORMAL_MODE_COMP_ZERO		0xFE31
#define ADP1050_NORMAL_MODE_COMP_POLE		0xFE32
#define ADP1050_NORMAL_MODE_COMP_HIGH_FREQ	0xFE33
#define ADP1050_CS1_THRESHOLD_VS_BALANCE	0xFE38
#define ADP1050_NOMINAL_MOD_VAL_PREBIAS		0xFE39
#define ADP1050_SR_DRIVER_DELAY			0xFE3A
#define ADP1050_PWM_180_PHASE_SHIFT_SETTINGS	0xFE3B
#define ADP1050_MODULATION_LIMIT		0xFE3C
#define ADP1050_FEEDFORWARD_SS_FILTER_GAIN	0xFE3D

/* PWM Outputs Timing Registers */
#define ADP1050_OUTA_RISING_EDGE_TIMING		0xFE3E
#define ADP1050_OUTA_FALLING_EDGE_TIMING	0xFE3F
#define ADP1050_OUTA_RISING_FALLING_TIMING_LSB	0xFE40
#define ADP1050_OUTB_RISING_EDGE_TIMING		0xFE41
#define ADP1050_OUTB_FALLING_EDGE_TIMING	0xFE42
#define ADP1050_OUTB_RISING_FALLING_TIMING_LSB	0xFE43
#define ADP1050_SR1_RISING_EDGE_TIMING		0xFE4A
#define ADP1050_SR1_FALLING_EDGE_TIMING		0xFE4B
#define ADP1050_SR1_RISING_FALLING_TIMING_LSB	0xFE4C
#define ADP1050_SR2_RISING_EDGE_TIMING		0xFE4D
#define ADP1050_SR2_FALLING_EDGE_TIMING		0xFE4E
#define ADP1050_SR2_RISING_FALLING_TIMING_LSB	0xFE4F
#define ADP1050_OUTA_OUTB_MODULATION_SETTINGS	0xFE50
#define ADP1050_SR1_SR2_MODULATION_SETTINGS	0xFE52
#define ADP1050_PWM_OUTPUT_DISABLE		0xFE53

/* Volt-Second Balance Control Registers */
#define ADP1050_VS_BAL_CTRL_GENERAL_SETTINGS	0xFE54
#define ADP1050_VS_BAL_CTRL_OUTA_OUTB		0xFE55
#define ADP1050_VS_BAL_CTRL_SR1_SR2		0xFE57

/* Duty Cycle Reading Setting Registers */
#define ADP1050_DUTY_CYCLE_READING_SETTINGS	0xFE58
#define ADP1050_INPUT_VOLTAGE_COMP_MULT		0xFE59

/* Other Setting Registers */
#define ADP1050_GO_COMMANDS			0xFE61
#define ADP1050_CUSTOMIZED_REGISTERS		0xFE62
#define ADP1050_MOD_REF_MSB_OL_INV_FF		0xFE63
#define ADP1050_MOD_REF_LSB_OL_INV_FF		0xFE64
#define ADP1050_CURRENT_VALUE_UPDATE_RATE	0xFE65
#define ADP1050_OL_OPERATION_SETTINGS		0xFE67
#define ADP1050_PULSE_SKIPPING_MODE_THR		0xFE69
#define ADP1050_CS3_OC_FAULT_LIMIT		0xFE6A
#define ADP1050_MOD_THR_OVP_SELECTION		0xFE6B
#define ADP1050_MOD_FLAG_OVP_SELECTION		0xFE6C
#define ADP1050_OUTA_OUTB_ADJ_REF_SYNCH		0xFE6D
#define ADP1050_SR1_SR2_ADJ_REF_SYNCH		0xFE6F

/* Manufacturer Specific Fault Flag Registers */
#define ADP1050_FLAG_REGISTER1			0xFEA0
#define ADP1050_FLAG_REGISTER2			0xFEA1
#define ADP1050_FLAG_REGISTER3			0xFEA2
#define ADP1050_LATCHED_FLAG_REGISTER1		0xFEA3
#define ADP1050_LATCHED_FLAG_REGISTER2		0xFEA4
#define ADP1050_LATCHED_FLAG_REGISTER3		0xFEA5
#define ADP1050_FIRST_FLAG_ID			0xFEA6

/* Manufacturer Specific Value Reading Regisers */
#define ADP1050_CS1_VALUE			0xFEA7
#define ADP1050_CS3_VALUE			0xFEA9
#define ADP1050_VS_VALUE			0xFEAA
#define ADP1050_RTD_VALUE			0xFEAB
#define ADP1050_VF_VALUE			0xFEAC
#define ADP1050_DUTY_CYCLE_VALUE		0xFEAD
#define ADP1050_INPUT_POWER_VALUE		0xFEAE

enum adp1050_loop {
	ADP1050_CLOSE_LOOP,
	ADP1050_OPEN_LOOP
};

enum adp1050_channel {
	ADP1050_OUTA,
	ADP1050_OUTB,
	ADP1050_SR1,
	ADP1050_SR2,
	ADP1050_DISABLE_ALL
};

enum adp1050_mod {
	ADP1050_OUTA_SR1_FALLING_MOD = NO_OS_BIT(1),
	ADP1050_OUTA_SR1_RISING_MOD = NO_OS_BIT(3),
	ADP1050_OUTB_SR2_FALLING_MOD = NO_OS_BIT(5),
	ADP1050_OUTB_SR2_RISING_MOD = NO_OS_BIT(7),
};

enum adp1050_pass_type {
	ADP1050_CHIP_PASS,
	ADP1050_EEPROM_PASS,
	ADP1050_TRIM_PASS
};

enum adp1050_freq {
	ADP1050_49KHZ = 0x0031,
	ADP1050_59KHZ = 0x0038,
	ADP1050_60KHZ = 0x003C,
	ADP1050_65KHZ = 0x0041,
	ADP1050_71KHZ = 0x0047,
	ADP1050_78KHZ = 0x004E,
	ADP1050_87KHZ = 0x0057,
	ADP1050_104KHZ = 0x0068,
	ADP1050_120KHZ = 0x0078,
	ADP1050_130KHZ = 0x0082,
	ADP1050_136KHZ = 0x0088,
	ADP1050_142KHZ = 0x008E,
	ADP1050_149KHZ = 0x0095,
	ADP1050_184KHZ = 0x00B8,
	ADP1050_223KHZ = 0x00DF,
	ADP1050_250KHZ = 0x00FA,
	ADP1050_284KHZ = 0x011C,
	ADP1050_329KHZ = 0x0149,
	ADP1050_338KHZ = 0x0152,
	ADP1050_347KHZ = 0x015B,
	ADP1050_357KHZ = 0x0165,
	ADP1050_379KHZ = 0x017B,
	ADP1050_397KHZ = 0x018D,
	ADP1050_403KHZ = 0x0193,
	ADP1050_410KHZ = 0x019A,
	ADP1050_97_5KHZ = 0xF8C3,
	ADP1050_111_5KHZ = 0xF8DF,
	ADP1050_156_5KHZ = 0xF939,
	ADP1050_164_5KHZ = 0xF949,
	ADP1050_173_5KHZ = 0xF95B,
	ADP1050_195_5KHZ = 0xF987,
	ADP1050_201_5KHZ = 0xF993,
	ADP1050_208_5KHZ = 0xF9A1,
	ADP1050_215_5KHZ = 0xF9AF,
	ADP1050_231_5KHZ = 0xF9CF,
	ADP1050_240_5KHZ = 0xF9E1,
	ADP1050_260_5KHZ = 0xFA09,
	ADP1050_271_5KHZ = 0xFA1F,
	ADP1050_297_5KHZ = 0xFA53,
	ADP1050_312_5KHZ = 0xFA71,
	ADP1050_320_5KHZ = 0xFA81,
	ADP1050_367_5KHZ = 0xFADF,
	ADP1050_390_5KHZ = 0xFB0D,
};

enum adp1050_vout_tr {
	ADP1050_VOUT_TR_INFINITE = 0x7BFF,
	ADP1050_VOUT_TR_1562NV_US = 0x980D,
	ADP1050_VOUT_TR_3125NV_US = 0xA00D,
	ADP1050_VOUT_TR_6250NV_US = 0xA80D,
	ADP1050_VOUT_TR_12UV_US = 0xB00D,
	ADP1050_VOUT_TR_25UV_US = 0xB80D,
	ADP1050_VOUT_TR_50UV_US = 0xC00D,
	ADP1050_VOUT_TR_100UV_US = 0xC80D,
	ADP1050_VOUT_TR_200UV_US = 0xD00D,
};

enum adp1050_iin_oc_fast_fault {
	ADP1050_IIN_OC_FAST_FAULT_2,
	ADP1050_IIN_OC_FAST_FAULT_8 = 0x10,
	ADP1050_IIN_OC_FAST_FAULT_16 = 0x20,
	ADP1050_IIN_OC_FAST_FAULT_64 = 0x30,
	ADP1050_IIN_OC_FAST_FAULT_128 = 0x40,
	ADP1050_IIN_OC_FAST_FAULT_256 = 0x50,
	ADP1050_IIN_OC_FAST_FAULT_512 = 0x60,
	ADP1050_IIN_OC_FAST_FAULT_1024 = 0x70
};

enum adp1050_climit_ref {
	ADP1050_CLIMIT_REF_1200MV,
	ADP1050_CLIMIT_REF_250MV = 0x40
};

enum adp1050_cs1_leading_edge {
	ADP1050_LEADING_EDGE_0NS,
	ADP1050_LEADING_EDGE_40NS,
	ADP1050_LEADING_EDGE_80NS,
	ADP1050_LEADING_EDGE_120NS,
	ADP1050_LEADING_EDGE_200NS,
	ADP1050_LEADING_EDGE_400NS,
	ADP1050_LEADING_EDGE_600NS,
	ADP1050_LEADING_EDGE_800NS
};

enum adp1050_cs1_debounce {
	ADP1050_DEBOUNCE_0NS,
	ADP1050_DEBOUNCE_40NS,
	ADP1050_DEBOUNCE_80NS,
	ADP1050_DEBOUNCE_120NS
};

enum adp1050_flgi_fault_response {
	ADP1050_CONTINUE_NO_INTERRUPT,
	ADP1050_DISABLE_SR1_SR2,
	ADP1050_DISABLE_ALL_PWM,
};

enum adp1050_flgi_after_fault {
	ADP1050_REENABLE_PWM_SS,
	ADP1050_REENABLE_PWM,
	ADP1050_REENABLE_PWM_WITH_PSON,
};

enum adp1050_value_type {
	ADP1050_VIN = ADP1050_READ_VIN,
	ADP1050_IIN = ADP1050_READ_IIN,
	ADP1050_TEMP = ADP1050_READ_TEMPERATURE,
	ADP1050_DUTY_CYCLE = ADP1050_READ_DUTY_CYCLE,
	ADP1050_FREQUENCY = ADP1050_READ_FREQUENCY
};

enum adp1050_status_type {
	ADP1050_STATUS_VOUT_TYPE = ADP1050_STATUS_VOUT,
	ADP1050_STATUS_INPUT_TYPE = ADP1050_STATUS_INPUT,
	ADP1050_STATUS_WORD_TYPE = ADP1050_STATUS_WORD,
	ADP1050_STATUS_TEMPERATURE_TYPE = ADP1050_STATUS_TEMPERATURE,
	ADP1050_STATUS_CML_TYPE = ADP1050_STATUS_CML
};

enum adp1050_trim_type {
	ADP1050_CS1_IIN_TRIM,
	ADP1050_VS_VOUT_TRIM,
	ADP1050_VF_VIN_TRIM
};

/**
 * @brief Initialization parameter for the ADP1050 device.
*/
struct adp1050_init_param {
	struct no_os_i2c_init_param *i2c_param;
	struct no_os_gpio_init_param *pg_alt_param;
	struct no_os_pwm_init_param *syni_param;
	struct no_os_gpio_init_param *flgi_param;
	struct no_os_gpio_init_param *ctrl_param;
	uint8_t on_off_config;
	bool ext_syni;
};

/**
 * @brief Device descriptor for ADP1050.
*/
struct adp1050_desc {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *pg_alt_desc;
	struct no_os_pwm_desc *syni_desc;
	struct no_os_gpio_desc *flgi_desc;
	struct no_os_gpio_desc *ctrl_desc;
	enum adp1050_loop loop;
	enum adp1050_freq freq;
};

/** Send command to ADP1050 device. */
int adp1050_send_command(struct adp1050_desc *desc, uint16_t command);

/** Read command from ADP1050 device. */
int adp1050_read(struct adp1050_desc *desc, uint16_t command, uint8_t *data,
		 uint8_t bytes_number);

/** Write command to ADP1050 device. */
int adp1050_write(struct adp1050_desc *desc, uint16_t command, uint16_t data,
		  uint8_t bytes_number);

/** Read statuses from ADP1050 device. */
int adp1050_read_status(struct adp1050_desc *desc,
			enum adp1050_status_type status,
			uint16_t *status_val);

/** Read Voltage Sense from ADP1050 device. */
int adp1050_read_vsense(struct adp1050_desc *desc, uint16_t *vsense);

/** Read values for ADP1050 device. */
int adp1050_read_value(struct adp1050_desc *desc, uint16_t *mant, uint8_t *exp,
		       enum adp1050_value_type val_type);

/** Set ADP1050 VOUT COMMAND and VOUT MAX COMMAND. */
int adp1050_vout_value(struct adp1050_desc *desc, uint16_t vout_command,
		       uint16_t vout_max);

/** Set ADP1050 VOUT CAL OFFSET. */
int adp1050_vout_offset(struct adp1050_desc *desc, int16_t vout_offset);

/** Set ADP1050 VOUT TRANSITION RATE. */
int adp1050_vout_tr(struct adp1050_desc *desc, enum adp1050_vout_tr vout_tr);

/** Set ADP1050 VOUT SCALE MONITOR and VOUT SCALE LOOP. */
int adp1050_vout_scale(struct adp1050_desc *desc, int8_t exp, uint16_t mant);

/** Set ADP1050 VOUT MARGINS. */
int adp1050_vout_margin(struct adp1050_desc *desc, uint16_t margin_high,
			uint16_t margin_low);

/** Set ADP1050's duty cycle for requested channel. */
int adp1050_pwm_duty_cycle(struct adp1050_desc *desc, uint16_t pulse_width,
			   uint16_t pulse_start, enum adp1050_channel chan);

/** Set ADP1050's modulation for requested channel. */
int adp1050_pwm_modulation(struct adp1050_desc *desc, enum adp1050_mod mod,
			   enum adp1050_channel chan, bool sign);

/** Set ADP1050's PWM output channel and frequency. */
int adp1050_set_pwm(struct adp1050_desc *desc, enum adp1050_channel chan,
		    enum adp1050_freq freq);

/** Set VIN ON and OFF value. */
int adp1050_set_vin(struct adp1050_desc *desc, int16_t mantissa, int8_t exp,
		    bool state_on);

/** Set CS1 fast fault and current reference. */
int adp1050_set_climit(struct adp1050_desc *desc,
		       enum adp1050_iin_oc_fast_fault fast_fault,
		       enum adp1050_climit_ref climit_ref);

/** Set CS1 leading edge and debounce time. */
int adp1050_set_cs1_settings(struct adp1050_desc *desc,
			     enum adp1050_cs1_leading_edge leading_edge,
			     enum adp1050_cs1_debounce debounce,
			     bool ignore);

/** Set ADP1050 to operate in open loop. */
int adp1050_set_open_loop(struct adp1050_desc *desc, uint8_t rising_edge,
			  uint8_t falling_edge, enum adp1050_channel chan);

/** Set ADP1050 to operate in closed loop. */
int adp1050_set_close_loop(struct adp1050_desc *desc);

/** Set ADP1050's pulse skipping. */
int adp1050_pulse_skipping(struct adp1050_desc *desc, uint8_t pulse_threshold,
			   bool state_on);

/** Set ADP1050's feedforward. */
int adp1050_set_feedforward(struct adp1050_desc *desc, bool state_on);

/** Set ADP1050's frequency synchronization. */
int adp1050_freq_sync(struct adp1050_desc *desc, bool state_on);

/** Set ADP1050's FLGI pin response. */
int adp1050_set_flgi_response(struct adp1050_desc *desc,
			      enum adp1050_flgi_fault_response fault_response,
			      enum adp1050_flgi_after_fault after_fault);

/** Set ADP1050's filter value. */
int adp1050_filter(struct adp1050_desc *desc, uint8_t zero, uint8_t pole,
		   uint8_t lf, uint8_t hf);

/** Set ADP1050's trim. */
int adp1050_trim(struct adp1050_desc *desc, enum adp1050_trim_type trim_type,
		 uint16_t vout_command, uint16_t vout_scale, uint16_t trim_val);

/** Unlock password for ADP1050. */
int adp1050_unlock_pass(struct adp1050_desc *desc, uint16_t password,
			enum adp1050_pass_type pass_type);

/** Lock passwords for ADP1050. */
int adp1050_lock_pass(struct adp1050_desc *desc, uint16_t password,
		      enum adp1050_pass_type pass_type);

/** Change passwords for ADP1050. */
int adp1050_change_pass(struct adp1050_desc *desc, uint16_t old_pass,
			uint16_t new_pass, enum adp1050_pass_type pass_type);

/** Software Reset the ADP1050. */
int adp1050_software_reset(struct adp1050_desc *desc);

/** Initialize the ADP1050 device descriptor. */
int adp1050_init(struct adp1050_desc **desc,
		 struct adp1050_init_param *init_param);

/** Remove resources allocated by the init function. */
int adp1050_remove(struct adp1050_desc *desc);

#endif /** __ADP_1050_H__ */
