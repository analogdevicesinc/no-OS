/***************************************************************************//**
 *   @file   ltc4284.h
 *   @brief  Header file of LTC4284 Driver
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
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

#ifndef __LTC4284_H__
#define __LTC4284_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"

/******************************************************************************/
/************************** I2C Address Definitions ***************************/
/******************************************************************************/

/* I2C 7-bit device addresses (datasheet Table 2).
 * ADR1 / ADR0 straps: L = tie to VEE, H = tie to INTVCC, NC = no-connect (float).
 */
#define LTC4284_I2C_ADDR_0              0x10  /* ADR1=L,  ADR0=L  */
#define LTC4284_I2C_ADDR_1              0x11  /* ADR1=L,  ADR0=NC */
#define LTC4284_I2C_ADDR_2              0x12  /* ADR1=H,  ADR0=NC */
#define LTC4284_I2C_ADDR_3              0x13  /* ADR1=L,  ADR0=H  */
#define LTC4284_I2C_ADDR_4              0x14  /* ADR1=NC, ADR0=L  */
#define LTC4284_I2C_ADDR_5              0x15  /* ADR1=NC, ADR0=NC */
#define LTC4284_I2C_ADDR_6              0x16  /* ADR1=H,  ADR0=H  */
#define LTC4284_I2C_ADDR_7              0x17  /* ADR1=NC, ADR0=H  */

/* Global-broadcast addresses (ignore ADR straps). */
#define LTC4284_I2C_ADDR_MASS_WRITE     0x1F  /* Mass write (write-only) */
#define LTC4284_I2C_ADDR_ALERT_RESP     0x0C  /* Alert Response (read-only) */

/******************************************************************************/
/*************************** Register Definitions *****************************/
/******************************************************************************/

/* System Status and Control Registers */
#define LTC4284_REG_SYSTEM_STATUS       0x00  /* System status information (R) */
#define LTC4284_REG_ADC_STATUS          0x01  /* ADC conversion status (R) */
#define LTC4284_REG_INPUT_STATUS        0x02  /* GPIO input status (R) */
#define LTC4284_REG_FAULT_STATUS        0x03  /* Fault status information (R) */
#define LTC4284_REG_FAULT               0x04  /* System fault (R/W) */
#define LTC4284_REG_ADC_ALARM_LOG_1     0x05  /* ADC alarm log 1 (5-byte block start) */
#define LTC4284_REG_ADC_ALARM_LOG_2     0x06  /* ADC alarm log 2 */
#define LTC4284_REG_ADC_ALARM_LOG_3     0x07  /* ADC alarm log 3 */
#define LTC4284_REG_ADC_ALARM_LOG_4     0x08  /* ADC alarm log 4 */
#define LTC4284_REG_ADC_ALARM_LOG_5     0x09  /* ADC alarm log 5 */
#define LTC4284_REG_CONTROL_1           0x0A  /* Control register 1 (2-byte block start) */
#define LTC4284_REG_CONTROL_2           0x0B  /* Control register 2 */
#define LTC4284_REG_RESERVED_0C         0x0C  /* Reserved (always returns 0) */
#define LTC4284_REG_CONFIG_1            0x0D  /* Config register 1 */
#define LTC4284_REG_CONFIG_2            0x0E  /* Config register 2 */
#define LTC4284_REG_CONFIG_3            0x0F  /* Config register 3 */
#define LTC4284_REG_PGIO_CONFIG_1       0x10  /* GPIO config 1 (2-byte block start) */
#define LTC4284_REG_PGIO_CONFIG_2       0x11  /* GPIO config 2 */
#define LTC4284_REG_ADIO_CONFIG         0x12  /* ADC I/O config (R/W) */
#define LTC4284_REG_ADC_SELECT_1	0x13  /* Auxiliary ADC input selection 1 (2-byte block start) */
#define LTC4284_REG_ADC_SELECT_2	0x14  /* Auxiliary ADC input selection 2 */
#define LTC4284_REG_FAULT_ALERT         0x15  /* Fault alert configuration (R/W) */
#define LTC4284_REG_ADC_ALERT_1		0x16  /* ADC alert configuration 1 (5-byte block start) */
#define LTC4284_REG_ADC_ALERT_2		0x17  /* ADC alert configuration 2 */
#define LTC4284_REG_ADC_ALERT_3		0x18  /* ADC alert configuration 3 */
#define LTC4284_REG_ADC_ALERT_4		0x19  /* ADC alert configuration 4 */
#define LTC4284_REG_ADC_ALERT_5		0x1A  /* ADC alert configuration 5 */

/* ADC Threshold Registers (Min/Max) */
#define LTC4284_REG_SENSE_MIN_TH        0x1B  /* SENSE min threshold */
#define LTC4284_REG_SENSE_MAX_TH        0x1C  /* SENSE max threshold */
#define LTC4284_REG_VPWR_MIN_TH         0x1D  /* VPWR min threshold */
#define LTC4284_REG_VPWR_MAX_TH         0x1E  /* VPWR max threshold */
#define LTC4284_REG_POWER_MIN_TH        0x1F  /* POWER min threshold */
#define LTC4284_REG_POWER_MAX_TH        0x20  /* POWER max threshold */
#define LTC4284_REG_ADIN1_MIN_TH	0x21  /* ADIN1 min threshold */
#define LTC4284_REG_ADIN1_MAX_TH	0x22  /* ADIN1 max threshold */
#define LTC4284_REG_ADIN2_MIN_TH	0x23  /* ADIN2 min threshold */
#define LTC4284_REG_ADIN2_MAX_TH	0x24  /* ADIN2 max threshold */
#define LTC4284_REG_ADIN3_MIN_TH	0x25  /* ADIN3 min threshold */
#define LTC4284_REG_ADIN3_MAX_TH	0x26  /* ADIN3 max threshold */
#define LTC4284_REG_ADIN4_MIN_TH	0x27  /* ADIN4 min threshold */
#define LTC4284_REG_ADIN4_MAX_TH	0x28  /* ADIN4 max threshold */
#define LTC4284_REG_ADIO1_MIN_TH	0x29  /* ADIO1 min threshold */
#define LTC4284_REG_ADIO1_MAX_TH	0x2A  /* ADIO1 max threshold */
#define LTC4284_REG_ADIO2_MIN_TH	0x2B  /* ADIO2 min threshold */
#define LTC4284_REG_ADIO2_MAX_TH	0x2C  /* ADIO2 max threshold */
#define LTC4284_REG_ADIO3_MIN_TH	0x2D  /* ADIO3 min threshold */
#define LTC4284_REG_ADIO3_MAX_TH	0x2E  /* ADIO3 max threshold */
#define LTC4284_REG_ADIO4_MIN_TH	0x2F  /* ADIO4 min threshold */
#define LTC4284_REG_ADIO4_MAX_TH	0x30  /* ADIO4 max threshold */
#define LTC4284_REG_DRNS_MIN_TH		0x31  /* DRNS min threshold */
#define LTC4284_REG_DRNS_MAX_TH		0x32  /* DRNS max threshold */
#define LTC4284_REG_DRAIN_MIN_TH	0x33  /* DRAIN min threshold */
#define LTC4284_REG_DRAIN_MAX_TH	0x34  /* DRAIN max threshold */
#define LTC4284_REG_SENSE1_MIN_TH	0x35  /* SENSE1 min threshold */
#define LTC4284_REG_SENSE1_MAX_TH	0x36  /* SENSE1 max threshold */
#define LTC4284_REG_SENSE2_MIN_TH	0x37  /* SENSE2 min threshold */
#define LTC4284_REG_SENSE2_MAX_TH	0x38  /* SENSE2 max threshold */
#define LTC4284_REG_ADIN12_MIN_TH	0x39  /* ADIN2-ADIN1 min threshold */
#define LTC4284_REG_ADIN12_MAX_TH	0x3A  /* ADIN2-ADIN1 max threshold */
#define LTC4284_REG_ADIN34_MIN_TH	0x3B  /* ADIN4-ADIN3 min threshold */
#define LTC4284_REG_ADIN34_MAX_TH	0x3C  /* ADIN4-ADIN3 max threshold */
#define LTC4284_REG_ADIO12_MIN_TH	0x3D  /* ADIO2-ADIO1 min threshold */
#define LTC4284_REG_ADIO12_MAX_TH	0x3E  /* ADIO2-ADIO1 max threshold */
#define LTC4284_REG_ADIO34_MIN_TH	0x3F  /* ADIO4-ADIO3 min threshold */
#define LTC4284_REG_ADIO34_MAX_TH	0x40  /* ADIO4-ADIO3 max threshold */

/* ADC Data Registers (16-bit, 2 bytes each) */
#define LTC4284_REG_SENSE               0x41  /* Current SENSE ADC value */
#define LTC4284_REG_SENSE_MIN           0x42  /* SENSE minimum (2 bytes) */
#define LTC4284_REG_SENSE_MAX           0x43  /* SENSE maximum (2 bytes) */
#define LTC4284_REG_VPWR                0x44  /* Most recent ADC output for VPWR */
#define LTC4284_REG_VPWR_MIN            0x45  /* VPWR Minimum ADC output */
#define LTC4284_REG_VPWR_MAX            0x46  /* VPWR Maximum ADC output */
#define LTC4284_REG_POWER               0x47  /* Power ADC value (2 bytes) */
#define LTC4284_REG_POWER_MIN           0x48  /* Power minimum (2 bytes) */
#define LTC4284_REG_POWER_MAX           0x49  /* Power maximum (2 bytes) */
#define LTC4284_REG_ADIN1               0x4A  /* ADIN1 ADC value (2 bytes) */
#define LTC4284_REG_ADIN1_MIN           0x4B  /* ADIN1 minimum (2 bytes) */
#define LTC4284_REG_ADIN1_MAX           0x4C  /* ADIN1 maximum (2 bytes) */
#define LTC4284_REG_ADIN2               0x4D  /* ADIN2 ADC value (2 bytes) */
#define LTC4284_REG_ADIN2_MIN           0x4E  /* ADIN2 minimum (2 bytes) */
#define LTC4284_REG_ADIN2_MAX           0x4F  /* ADIN2 maximum (2 bytes) */
#define LTC4284_REG_ADIN3               0x50  /* ADIN3 ADC value (2 bytes) */
#define LTC4284_REG_ADIN3_MIN           0x51  /* ADIN3 minimum (2 bytes) */
#define LTC4284_REG_ADIN3_MAX           0x52  /* ADIN3 maximum (2 bytes) */
#define LTC4284_REG_ADIN4               0x53  /* ADIN4 ADC value (2 bytes) */
#define LTC4284_REG_ADIN4_MIN           0x54  /* ADIN4 minimum (2 bytes) */
#define LTC4284_REG_ADIN4_MAX           0x55  /* ADIN4 maximum (2 bytes) */
#define LTC4284_REG_ADIO1               0x56  /* ADIO1 ADC value (2 bytes) */
#define LTC4284_REG_ADIO1_MIN           0x57  /* ADIO1 minimum (2 bytes) */
#define LTC4284_REG_ADIO1_MAX           0x58  /* ADIO1 maximum (2 bytes) */
#define LTC4284_REG_ADIO2               0x59  /* ADIO2 ADC value (2 bytes) */
#define LTC4284_REG_ADIO2_MIN           0x5A  /* ADIO2 minimum (2 bytes) */
#define LTC4284_REG_ADIO2_MAX           0x5B  /* ADIO2 maximum (2 bytes) */
#define LTC4284_REG_ADIO3               0x5C  /* ADIO3 ADC value (2 bytes) */
#define LTC4284_REG_ADIO3_MIN           0x5D  /* ADIO3 minimum (2 bytes) */
#define LTC4284_REG_ADIO3_MAX           0x5E  /* ADIO3 maximum (2 bytes) */
#define LTC4284_REG_ADIO4               0x5F  /* ADIO4 ADC value (2 bytes) */
#define LTC4284_REG_ADIO4_MIN           0x60  /* ADIO4 minimum (2 bytes) */
#define LTC4284_REG_ADIO4_MAX           0x61  /* ADIO4 maximum (2 bytes) */
#define LTC4284_REG_DRNS                0x62  /* DRNS voltage (2 bytes) */
#define LTC4284_REG_DRNS_MIN            0x63  /* DRNS minimum (2 bytes) */
#define LTC4284_REG_DRNS_MAX            0x64  /* DRNS maximum (2 bytes) */
#define LTC4284_REG_DRAIN               0x65  /* DRAIN voltage (2 bytes) */
#define LTC4284_REG_DRAIN_MIN           0x66  /* DRAIN minimum (2 bytes) */
#define LTC4284_REG_DRAIN_MAX           0x67  /* DRAIN maximum (2 bytes) */
#define LTC4284_REG_SENSE1              0x68  /* SENSE1 current (2 bytes) */
#define LTC4284_REG_SENSE1_MIN          0x69  /* SENSE1 minimum (2 bytes) */
#define LTC4284_REG_SENSE1_MAX          0x6A  /* SENSE1 maximum (2 bytes) */
#define LTC4284_REG_SENSE2              0x6B  /* SENSE2 current (2 bytes) */
#define LTC4284_REG_SENSE2_MIN          0x6C  /* SENSE2 minimum (2 bytes) */
#define LTC4284_REG_SENSE2_MAX          0x6D  /* SENSE2 maximum (2 bytes) */

/* Auxiliary Differential ADC Data Registers (2 bytes each) */
#define LTC4284_REG_ADIN12              0x6E  /* Most recent ADIN2 - ADIN1 */
#define LTC4284_REG_ADIN12_MIN          0x6F  /* ADIN2 - ADIN1 minimum */
#define LTC4284_REG_ADIN12_MAX          0x70  /* ADIN2 - ADIN1 maximum */
#define LTC4284_REG_ADIN34              0x71  /* Most recent ADIN4 - ADIN3 */
#define LTC4284_REG_ADIN34_MIN          0x72  /* ADIN4 - ADIN3 minimum */
#define LTC4284_REG_ADIN34_MAX          0x73  /* ADIN4 - ADIN3 maximum */
#define LTC4284_REG_ADIO12              0x74  /* Most recent ADIO2 - ADIO1 */
#define LTC4284_REG_ADIO12_MIN          0x75  /* ADIO2 - ADIO1 minimum */
#define LTC4284_REG_ADIO12_MAX          0x76  /* ADIO2 - ADIO1 maximum */
#define LTC4284_REG_ADIO34              0x77  /* Most recent ADIO4 - ADIO3 */
#define LTC4284_REG_ADIO34_MIN          0x78  /* ADIO4 - ADIO3 minimum */
#define LTC4284_REG_ADIO34_MAX          0x79  /* ADIO4 - ADIO3 maximum */

/* Energy Meter (48-bit accumulator, 6 bytes, MSB first) */
#define LTC4284_REG_ENERGY_1            0x7A  /* Energy byte 1 / MSB (6-byte block start) */
#define LTC4284_REG_ENERGY_2            0x7B  /* Energy byte 2 */
#define LTC4284_REG_ENERGY_3            0x7C  /* Energy byte 3 */
#define LTC4284_REG_ENERGY_4            0x7D  /* Energy byte 4 */
#define LTC4284_REG_ENERGY_5            0x7E  /* Energy byte 5 */
#define LTC4284_REG_ENERGY_6            0x7F  /* Energy byte 6 (LSB) */

/* Tick Counter (32-bit, 4 bytes, MSB first) */
#define LTC4284_REG_TICK_COUNTER_1      0x80  /* Tick counter byte 1 / MSB (4-byte block start) */
#define LTC4284_REG_TICK_COUNTER_2      0x81  /* Tick counter byte 2 */
#define LTC4284_REG_TICK_COUNTER_3      0x82  /* Tick counter byte 3 */
#define LTC4284_REG_TICK_COUNTER_4      0x83  /* Tick counter byte 4 (LSB) */

/* Meter and Snapshot Control */
#define LTC4284_REG_METER_CONTROL       0x84  /* Meter/tick counter control */
#define LTC4284_REG_ADC_SNAPSHOT        0x85  /* ADC snapshot control */

/* Fault Log Control */
#define LTC4284_REG_FAULT_LOG_CONTROL   0x90  /* Fault/ADC data EEPROM logging enable */

/* Reboot Control */
#define LTC4284_REG_REBOOT              0xA2  /* Reboot control and delay */

/* EEPROM Mirror Registers (defaults for operating registers on power-up) */
#define LTC4284_REG_EE_FAULT            0xA4  /* EEPROM copy of FAULT on fault event */
#define LTC4284_REG_EE_ADC_ALARM_LOG_1  0xA5  /* EE ADC alarm log 1 (5-byte block start) */
#define LTC4284_REG_EE_ADC_ALARM_LOG_2  0xA6  /* EE ADC alarm log 2 */
#define LTC4284_REG_EE_ADC_ALARM_LOG_3  0xA7  /* EE ADC alarm log 3 */
#define LTC4284_REG_EE_ADC_ALARM_LOG_4  0xA8  /* EE ADC alarm log 4 */
#define LTC4284_REG_EE_ADC_ALARM_LOG_5  0xA9  /* EE ADC alarm log 5 */
#define LTC4284_REG_EE_CONTROL_1        0xAA  /* EE CONTROL 1 (2-byte block start) */
#define LTC4284_REG_EE_CONTROL_2        0xAB
#define LTC4284_REG_RESERVED_AC         0xAC  /* Reads 0xFF while EEPROM busy, else 0 */
#define LTC4284_REG_EE_CONFIG_1         0xAD  /* EE CONFIG 1 (3-byte block start) */
#define LTC4284_REG_EE_CONFIG_2         0xAE
#define LTC4284_REG_EE_CONFIG_3         0xAF
#define LTC4284_REG_EE_PGIO_CONFIG_1    0xB0  /* EE PGIO_CONFIG 1 (2-byte block start) */
#define LTC4284_REG_EE_PGIO_CONFIG_2    0xB1  /* EE PGIO_CONFIG 2 */
#define LTC4284_REG_EE_ADIO_CONFIG      0xB2  /* Default ADIO_CONFIG */
#define LTC4284_REG_EE_ADC_SELECT_1     0xB3  /* EE ADC_SELECT 1 (2-byte block start) */
#define LTC4284_REG_EE_ADC_SELECT_2     0xB4  /* EE ADC_SELECT 2 */
#define LTC4284_REG_EE_FAULT_ALERT      0xB5  /* Default FAULT_ALERT */
#define LTC4284_REG_EE_ADC_ALERT_1      0xB6  /* EE ADC_ALERT 1 (5-byte block start) */
#define LTC4284_REG_EE_ADC_ALERT_2      0xB7  /* EE ADC_ALERT 2 */
#define LTC4284_REG_EE_ADC_ALERT_3      0xB8  /* EE ADC_ALERT 3 */
#define LTC4284_REG_EE_ADC_ALERT_4      0xB9  /* EE ADC_ALERT 4 */
#define LTC4284_REG_EE_ADC_ALERT_5      0xBA  /* EE ADC_ALERT 5 */

/* EEPROM defaults for ADC threshold registers */
#define LTC4284_REG_EE_SENSE_MIN_TH     0xBB
#define LTC4284_REG_EE_SENSE_MAX_TH     0xBC
#define LTC4284_REG_EE_VPWR_MIN_TH      0xBD
#define LTC4284_REG_EE_VPWR_MAX_TH      0xBE
#define LTC4284_REG_EE_POWER_MIN_TH     0xBF
#define LTC4284_REG_EE_POWER_MAX_TH     0xC0
#define LTC4284_REG_EE_ADIN1_MIN_TH     0xC1
#define LTC4284_REG_EE_ADIN1_MAX_TH     0xC2
#define LTC4284_REG_EE_ADIN2_MIN_TH     0xC3
#define LTC4284_REG_EE_ADIN2_MAX_TH     0xC4
#define LTC4284_REG_EE_ADIN3_MIN_TH     0xC5
#define LTC4284_REG_EE_ADIN3_MAX_TH     0xC6
#define LTC4284_REG_EE_ADIN4_MIN_TH     0xC7
#define LTC4284_REG_EE_ADIN4_MAX_TH     0xC8
#define LTC4284_REG_EE_ADIO1_MIN_TH     0xC9
#define LTC4284_REG_EE_ADIO1_MAX_TH     0xCA
#define LTC4284_REG_EE_ADIO2_MIN_TH     0xCB
#define LTC4284_REG_EE_ADIO2_MAX_TH     0xCC
#define LTC4284_REG_EE_ADIO3_MIN_TH     0xCD
#define LTC4284_REG_EE_ADIO3_MAX_TH     0xCE
#define LTC4284_REG_EE_ADIO4_MIN_TH     0xCF
#define LTC4284_REG_EE_ADIO4_MAX_TH     0xD0
#define LTC4284_REG_EE_DRNS_MIN_TH      0xD1
#define LTC4284_REG_EE_DRNS_MAX_TH      0xD2
#define LTC4284_REG_EE_DRAIN_MIN_TH     0xD3
#define LTC4284_REG_EE_DRAIN_MAX_TH     0xD4
#define LTC4284_REG_EE_SENSE1_MIN_TH    0xD5
#define LTC4284_REG_EE_SENSE1_MAX_TH    0xD6
#define LTC4284_REG_EE_SENSE2_MIN_TH    0xD7
#define LTC4284_REG_EE_SENSE2_MAX_TH    0xD8
#define LTC4284_REG_EE_ADIN12_MIN_TH    0xD9
#define LTC4284_REG_EE_ADIN12_MAX_TH    0xDA
#define LTC4284_REG_EE_ADIN34_MIN_TH    0xDB
#define LTC4284_REG_EE_ADIN34_MAX_TH    0xDC
#define LTC4284_REG_EE_ADIO12_MIN_TH    0xDD
#define LTC4284_REG_EE_ADIO12_MAX_TH    0xDE
#define LTC4284_REG_EE_ADIO34_MIN_TH    0xDF
#define LTC4284_REG_EE_ADIO34_MAX_TH    0xE0

/* EEPROM fault-time snapshots (MSB byte only, populated on fault) */
#define LTC4284_REG_EE_SENSE            0xE1
#define LTC4284_REG_EE_SENSE_MIN        0xE2
#define LTC4284_REG_EE_SENSE_MAX        0xE3
#define LTC4284_REG_EE_RTNS             0xE4
#define LTC4284_REG_EE_RTNS_MIN         0xE5
#define LTC4284_REG_EE_RTNS_MAX         0xE6

/* Miscellaneous EEPROM */
#define LTC4284_REG_POWER_PLAY_ID_1     0xE7  /* PowerPlay ID byte 1 / MSB (2-byte block start, default 0x1070) */
#define LTC4284_REG_POWER_PLAY_ID_2     0xE8  /* PowerPlay ID byte 2 (LSB) */
#define LTC4284_REG_EE_SCRATCH_1        0xE9  /* EE scratch byte 1 (7-byte block start) */
#define LTC4284_REG_EE_SCRATCH_2        0xEA  /* EE scratch byte 2 */
#define LTC4284_REG_EE_SCRATCH_3        0xEB  /* EE scratch byte 3 */
#define LTC4284_REG_EE_SCRATCH_4        0xEC  /* EE scratch byte 4 */
#define LTC4284_REG_EE_SCRATCH_5        0xED  /* EE scratch byte 5 */
#define LTC4284_REG_EE_SCRATCH_6        0xEE  /* EE scratch byte 6 */
#define LTC4284_REG_EE_SCRATCH_7        0xEF  /* EE scratch byte 7 */
#define LTC4284_REG_EE_FAULT_LOG_CONTROL 0xF0 /* EEPROM copy of FAULT_LOG_CONTROL */

/******************************************************************************/
/*********************** SYSTEM_STATUS Register (0x00) ************************/
/******************************************************************************/

#define LTC4284_SYSTEM_STATUS_FET_ON_STATUS     NO_OS_BIT(7)  /* GATE commanded on */
#define LTC4284_SYSTEM_STATUS_EN                NO_OS_BIT(6)  /* EN# pin state */
#define LTC4284_SYSTEM_STATUS_GATE2_HIGH        NO_OS_BIT(5)  /* GATE2 pin high */
#define LTC4284_SYSTEM_STATUS_GATE1_HIGH        NO_OS_BIT(4)  /* GATE1 pin high */
#define LTC4284_SYSTEM_STATUS_TMR_LOW           NO_OS_BIT(3)  /* TMR < 0.1V */
#define LTC4284_SYSTEM_STATUS_EEPROM_BUSY       NO_OS_BIT(2)  /* EEPROM writing */
#define LTC4284_SYSTEM_STATUS_PG_STATUS         NO_OS_BIT(1)  /* Power good */
#define LTC4284_SYSTEM_STATUS_MODE1             NO_OS_BIT(0)  /* Mode 1 enabled */

/******************************************************************************/
/************************ ADC_STATUS Register (0x01) **************************/
/******************************************************************************/
/* Datasheet Table 5.
 *  [7:4] AUX_ADC_CH  — which auxiliary ADC channel completed the latest
 *                      conversion (4-bit code; see AUX_ADC_CH decode below)
 *  [3]   ADC_IDLE    — 1 = ADC idle in snapshot mode
 *                      0 = ADC in continuous mode or busy in snapshot mode
 *  [2]   MODE2       — Parallel mode enabled
 *  [1]   MODE3       — High-stress staged-start mode enabled
 *  [0]   MODE4       — Low-stress staged-start mode enabled (DC2470A)
 *
 * Note: MODE1 (single-driver mode) status lives in SYSTEM_STATUS bit 0,
 * NOT here. Only one MODE bit is set at a time.
 */

#define LTC4284_ADC_STATUS_AUX_ADC_CH           NO_OS_GENMASK(7, 4)
#define LTC4284_ADC_STATUS_ADC_IDLE             NO_OS_BIT(3)
#define LTC4284_ADC_STATUS_MODE2                NO_OS_BIT(2)
#define LTC4284_ADC_STATUS_MODE3                NO_OS_BIT(1)
#define LTC4284_ADC_STATUS_MODE4                NO_OS_BIT(0)

/* AUX_ADC_CH decode — which channel the last conversion was on */
#define LTC4284_AUX_CH_ADIN1                    0x0
#define LTC4284_AUX_CH_ADIN2                    0x1
#define LTC4284_AUX_CH_ADIN3                    0x2
#define LTC4284_AUX_CH_ADIN4                    0x3
#define LTC4284_AUX_CH_ADIO1                    0x4
#define LTC4284_AUX_CH_ADIO2                    0x5
#define LTC4284_AUX_CH_ADIO3                    0x6
#define LTC4284_AUX_CH_ADIO4                    0x7
#define LTC4284_AUX_CH_DRNS                     0x8
#define LTC4284_AUX_CH_DRAIN                    0x9
#define LTC4284_AUX_CH_SENSE1_DIFF              0xA
#define LTC4284_AUX_CH_SENSE2_DIFF              0xB
#define LTC4284_AUX_CH_ADIN12_DIFF              0xC
#define LTC4284_AUX_CH_ADIN34_DIFF              0xD
#define LTC4284_AUX_CH_ADIO12_DIFF              0xE
#define LTC4284_AUX_CH_ADIO34_DIFF              0xF

/******************************************************************************/
/*********************** INPUT_STATUS Register (0x02) *************************/
/******************************************************************************/

#define LTC4284_INPUT_STATUS_PGIO1_INPUT        NO_OS_BIT(7)  /* PGIO1 pin state */
#define LTC4284_INPUT_STATUS_PGIO2_INPUT        NO_OS_BIT(6)  /* PGIO2 pin state */
#define LTC4284_INPUT_STATUS_PGIO3_INPUT        NO_OS_BIT(5)  /* PGIO3 pin state */
#define LTC4284_INPUT_STATUS_PGIO4_INPUT        NO_OS_BIT(4)  /* PGIO4 pin state */
#define LTC4284_INPUT_STATUS_ADIO1_INPUT        NO_OS_BIT(3)  /* ADIO1 pin state */
#define LTC4284_INPUT_STATUS_ADIO2_INPUT        NO_OS_BIT(2)  /* ADIO2 pin state */
#define LTC4284_INPUT_STATUS_ADIO3_INPUT        NO_OS_BIT(1)  /* ADIO3 pin state */
#define LTC4284_INPUT_STATUS_ADIO4_INPUT        NO_OS_BIT(0)  /* ADIO4 pin state */

/******************************************************************************/
/********************** FAULT_STATUS Register (0x03) **************************/
/******************************************************************************/
/* Datasheet Table 7 — live status bits (mirror the condition, not latched). */

#define LTC4284_FAULT_STATUS_EXT_FAULT          NO_OS_BIT(7)  /* PGIO4 EXT_FAULT pin state */
#define LTC4284_FAULT_STATUS_FET_SHORT          NO_OS_BIT(6)  /* FET currently shorted */
#define LTC4284_FAULT_STATUS_VOUT_LOW           NO_OS_BIT(5)  /* V_OUT < V_OUTTH */
#define LTC4284_FAULT_STATUS_PGI                NO_OS_BIT(4)  /* PGIO3 PGI pin state */
#define LTC4284_FAULT_STATUS_FET_BAD            NO_OS_BIT(3)  /* FET bad condition */
#define LTC4284_FAULT_STATUS_OC                 NO_OS_BIT(2)  /* Active current-limit engaged */
#define LTC4284_FAULT_STATUS_UV                 NO_OS_BIT(1)  /* Input undervoltage */
#define LTC4284_FAULT_STATUS_OV                 NO_OS_BIT(0)  /* Input overvoltage */

/******************************************************************************/
/************************** FAULT Register (0x04) *****************************/
/******************************************************************************/

#define LTC4284_FAULT_EXT_FAULT_BIT             NO_OS_BIT(7)
#define LTC4284_FAULT_FET_SHORT_FAULT_BIT       NO_OS_BIT(6)
#define LTC4284_FAULT_POWER_FAILED_BIT          NO_OS_BIT(5)
#define LTC4284_FAULT_PGI_FAULT_BIT             NO_OS_BIT(4)
#define LTC4284_FAULT_FET_BAD_FAULT_BIT         NO_OS_BIT(3)
#define LTC4284_FAULT_OC_FAULT_BIT         	NO_OS_BIT(2)
#define LTC4284_FAULT_UV_FAULT_BIT              NO_OS_BIT(1)
#define LTC4284_FAULT_OV_FAULT_BIT             	NO_OS_BIT(0)

/* Shorter fault bit aliases for convenience */
#define LTC4284_FAULT_OV                        LTC4284_FAULT_OV_FAULT_BIT
#define LTC4284_FAULT_UV                        LTC4284_FAULT_UV_FAULT_BIT
#define LTC4284_FAULT_OC                        LTC4284_FAULT_OC_FAULT_BIT
#define LTC4284_FAULT_POWER_BAD                 LTC4284_FAULT_POWER_FAILED_BIT
#define LTC4284_FAULT_FET_BAD                   LTC4284_FAULT_FET_BAD_FAULT_BIT
#define LTC4284_FAULT_FET_SHORT                 LTC4284_FAULT_FET_SHORT_FAULT_BIT
#define LTC4284_FAULT_EXT                       LTC4284_FAULT_EXT_FAULT_BIT
#define LTC4284_FAULT_PGI                       LTC4284_FAULT_PGI_FAULT_BIT
/* Note: no on-die temperature sensor; there is no LTC4284_FAULT_OT alias. */

/******************************************************************************/
/********************* CONTROL_1 Register (0x0A) ******************************/
/******************************************************************************/
/* Datasheet Table 10. Default = 0xDB. */

#define LTC4284_CONTROL_1_ON                    NO_OS_BIT(7)  /* 1 = FET on */
#define LTC4284_CONTROL_1_DVDT                  NO_OS_BIT(6)  /* dV/dt inrush control at startup */
#define LTC4284_CONTROL_1_THERM_TMR             NO_OS_BIT(5)  /* 1 = TMR 2µA pull-down off */
#define LTC4284_CONTROL_1_FET_BAD_TURN_OFF      NO_OS_BIT(4)  /* 1 = turn off MOSFET on FET_BAD_FAULT */
#define LTC4284_CONTROL_1_PWRGD_RESET_CNTRL     NO_OS_BIT(3)  /* 0 = PG reset by MOSFET off, 1 = by V_OUT low */
#define LTC4284_CONTROL_1_PGIO2_ACLB            NO_OS_BIT(2)  /* PGIO2 as active-current-limit indicator */
#define LTC4284_CONTROL_1_MASS_WRITE_ENABLE     NO_OS_BIT(1)  /* Enable mass writes to all LTC4284s on bus */
#define LTC4284_CONTROL_1_PAGE_READ_WRITE_ENABLE NO_OS_BIT(0) /* Enable I2C page read/write protocols */

/******************************************************************************/
/********************* CONTROL_2 Register (0x0B) ******************************/
/******************************************************************************/
/* Datasheet Table 10. Default = 0x03 (UV_RETRY=1, OV_RETRY=1, others=0). */

#define LTC4284_CONTROL_2_EXT_FAULT_RETRY       NO_OS_BIT(7)         /* 1 = unlimited retry, 0 = latch-off */
#define LTC4284_CONTROL_2_PGI_RETRY             NO_OS_BIT(6)         /* 1 = unlimited retry, 0 = latch-off */
#define LTC4284_CONTROL_2_FET_BAD_RETRY         NO_OS_GENMASK(5, 4)  /* 2-bit retry code (see enum below) */
#define LTC4284_CONTROL_2_OC_RETRY              NO_OS_GENMASK(3, 2)  /* 2-bit retry code */
#define LTC4284_CONTROL_2_UV_RETRY              NO_OS_BIT(1)         /* 1 = unlimited retry, 0 = latch-off */
#define LTC4284_CONTROL_2_OV_RETRY              NO_OS_BIT(0)         /* 1 = unlimited retry, 0 = latch-off */

/* 2-bit retry-count codes for the FET_BAD_RETRY / OC_RETRY fields */
#define LTC4284_RETRY_LATCH_OFF                 0x0  /* 0 retries, latch off */
#define LTC4284_RETRY_1                         0x1  /* 1 retry */
#define LTC4284_RETRY_7                         0x2  /* 7 retries */
#define LTC4284_RETRY_UNLIMITED                 0x3  /* unlimited retries */

/******************************************************************************/
/********************** CONFIG_1 Register (0x0D) ******************************/
/******************************************************************************/
/* Datasheet Table 11. Default = 0x0C (ILIM=0000, FB=11 → 10%).
 * DC2470A ships EEPROM-programmed as 0x34 (ILIM=0011 → 18 mV, FB=01 → 50 %).
 */

#define LTC4284_CONFIG_1_ILIM                   NO_OS_GENMASK(7, 4)  /* V_ILIM 4-bit code (15-30 mV) */
#define LTC4284_CONFIG_1_FB                     NO_OS_GENMASK(3, 2)  /* Foldback factor (2-bit) */
#define LTC4284_CONFIG_1_FB_DIS                 NO_OS_BIT(1)         /* 1 = disable foldback after startup */
#define LTC4284_CONFIG_1_LPFB                   NO_OS_BIT(0)         /* 1 = enable load power foldback */

/* Foldback factor codes for CONFIG_1[3:2] (percent of V_ILIM at V_OUT = 0) */
#define LTC4284_FB_100                          0x0  /* Foldback disabled */
#define LTC4284_FB_50                           0x1  /* 50 % — DC2470A default */
#define LTC4284_FB_20                           0x2  /* 20 % */
#define LTC4284_FB_10                           0x3  /* 10 % — chip power-on default */

/******************************************************************************/
/********************** CONFIG_2 Register (0x0E) ******************************/
/******************************************************************************/
/* Datasheet Table 11. Default = 0xC0 (VDTH=11, others=0). */

#define LTC4284_CONFIG_2_VDTH                   NO_OS_GENMASK(7, 6)  /* DRAIN threshold for FET-bad timer */
#define LTC4284_CONFIG_2_FTBD_DL                NO_OS_GENMASK(5, 4)  /* FET-bad filter timer delay */
#define LTC4284_CONFIG_2_COOLING_DL             NO_OS_GENMASK(3, 1)  /* Cooling delay after fault (3 bits!) */
#define LTC4284_CONFIG_2_PORB                   NO_OS_BIT(0)         /* Power-on reset flag (writable to 1 to clear) */

/* NOTE: The 8-bit vs 16-bit ADC selection is NOT in CONFIG_2. It lives in
 * PGIO_CONFIG_2 (0x11) bits [2:0]. See datasheet Table 12.
 */

/******************************************************************************/
/********************** CONFIG_3 Register (0x0F) ******************************/
/******************************************************************************/
/* Datasheet Table 11. Default = 0x00. */

#define LTC4284_CONFIG_3_EXTFLT_TURN_OFF        NO_OS_BIT(7)  /* 1 = turn off MOSFET on external fault */
#define LTC4284_CONFIG_3_VPWR_SELECT            NO_OS_BIT(6)  /* 0 = RTNS (input power), 1 = DRNS (MOSFET power) */
#define LTC4284_CONFIG_3_FAST_I2C_EN            NO_OS_BIT(5)  /* 1 = 400 kHz I²C, 0 = 100 kHz */
#define LTC4284_CONFIG_3_BC                     NO_OS_GENMASK(4, 3) /* Single-wire broadcast bit rate */
#define LTC4284_CONFIG_3_TICK_OVERFLOW_ALERT    NO_OS_BIT(2)  /* Alert on tick counter overflow */
#define LTC4284_CONFIG_3_METER_OVERFLOW_ALERT   NO_OS_BIT(1)  /* Alert on energy meter overflow */
#define LTC4284_CONFIG_3_INTEGRATE_I            NO_OS_BIT(0)  /* 1 = integrate current (charge), 0 = integrate power (energy) */

/******************************************************************************/
/******************* PGIO_CONFIG_2 Register (0x11) ****************************/
/******************************************************************************/
/* Datasheet Table 12. Default = 0x04 (ADC=100 → 12-bit). Bits 7:4 are
 * PGIO1..PGIO4 general-purpose output data. Bit 3 is ADC_CONV_ALERT.
 * Bits [2:0] configure ADC resolution and conversion rate.
 */

#define LTC4284_PGIO_CONFIG_2_PGIO4_OUT         NO_OS_BIT(7)
#define LTC4284_PGIO_CONFIG_2_PGIO3_OUT         NO_OS_BIT(6)
#define LTC4284_PGIO_CONFIG_2_PGIO2_OUT         NO_OS_BIT(5)
#define LTC4284_PGIO_CONFIG_2_PGIO1_OUT         NO_OS_BIT(4)
#define LTC4284_PGIO_CONFIG_2_ADC_CONV_ALERT    NO_OS_BIT(3)
#define LTC4284_PGIO_CONFIG_2_ADC               NO_OS_GENMASK(2, 0)

/* ADC resolution / conversion-rate codes for PGIO_CONFIG_2[2:0] */
#define LTC4284_ADC_MODE_8BIT                   0x0  /* 8-bit @ 996 Hz */
#define LTC4284_ADC_MODE_10BIT                  0x2  /* 10-bit @ 125 Hz */
#define LTC4284_ADC_MODE_12BIT                  0x4  /* 12-bit @ 15.6 Hz — default */
#define LTC4284_ADC_MODE_14BIT                  0x6  /* 14-bit @ 3.91 Hz */
#define LTC4284_ADC_MODE_16BIT                  0x1  /* 16-bit @ 0.977 Hz (any xx1 works) */

/******************************************************************************/
/******************** METER_CONTROL Register (0x84) ***************************/
/******************************************************************************/
/* Datasheet Table 19. Default = 0x00.
 * Bits [1:0] are reserved (read-only 0).
 * EE_LOCK reflects whether the on-die EEPROM was factory-locked.
 */

#define LTC4284_METER_CONTROL_METER_RESET       NO_OS_BIT(7)  /* 1 = reset energy meter + tick counter */
#define LTC4284_METER_CONTROL_METER_HALT        NO_OS_BIT(6)  /* 1 = halt meter accumulation */
#define LTC4284_METER_CONTROL_TICK_OVERFLOW     NO_OS_BIT(5)  /* Tick counter overflowed (write to clear) */
#define LTC4284_METER_CONTROL_METER_OVERFLOW    NO_OS_BIT(4)  /* Energy meter overflowed (write to clear) */
#define LTC4284_METER_CONTROL_ALERT_GENERATED   NO_OS_BIT(3)  /* Latched ALERT# assertion (write to clear) */
#define LTC4284_METER_CONTROL_EE_LOCK           NO_OS_BIT(2)  /* Read-only: factory EEPROM lock status */

/******************************************************************************/
/******************** FAULT_LOG_CONTROL Register (0x90) ***********************/
/******************************************************************************/
/* Datasheet Table 21. Default = 0x00.
 * FAULT_LOG_ENABLE arms fault-log capture into EEPROM; the START/DONE bits are
 * chip-set and can only be cleared over I2C.
 */

#define LTC4284_FAULT_LOG_ENABLE                NO_OS_BIT(7)  /* Arm fault-log capture */
#define LTC4284_FAULT_LOG_UNLOCK                NO_OS_BIT(6)  /* Allow clearing START/DONE */
#define LTC4284_FAULT_LOG_START                 NO_OS_BIT(5)  /* Fault log capture in progress */
#define LTC4284_FAULT_LOG_DONE                  NO_OS_BIT(4)  /* Fault log capture complete */
#define LTC4284_FAULT_LOG_ALERT                 NO_OS_BIT(3)  /* Alert on fault-log completion */

/******************************************************************************/
/******************** ADC Conversion Constants ********************************/
/******************************************************************************/

/* Full-scale values (datasheet p.7 / p.51 — universal across all ADC
 * resolutions because register data is left-justified to 16 bits).
 */
#define LTC4284_ADC_SINGLE_ENDED_FS_UV          2048000 /* 2.048 V, in µV */
#define LTC4284_ADC_DIFFERENTIAL_FS_UV          32768   /* 32.768 mV, in µV */
#define LTC4284_ADC_LEVELS                      65536   /* 2^16, left-justified */

/* Programmable V_ILIM range (datasheet Table 11 — CONFIG_1 ILIM field). */
#define LTC4284_VILIM_MIN_MV                    15
#define LTC4284_VILIM_MAX_MV                    30

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct ltc4284_dev
 * @brief LTC4284 device descriptor
 */
struct ltc4284_dev {
	struct no_os_i2c_desc *i2c_desc;    /* I2C communication descriptor */
	uint8_t i2c_addr;                    /* Device I2C address */
	uint32_t rsense_uohm;                /* Current sense resistor (µΩ) */
	uint16_t vpwr_divider;               /* External divider ratio from bus to RTNS pin */
	uint16_t drain_divider;              /* External divider ratio from bus to DRAIN pin */
	uint8_t vsense_mv;                   /* V_ILIM threshold at init (mV, 15..30) */
	struct no_os_gpio_desc *alert_gpio;  /* ALERT# pin descriptor (optional) */
};

/**
 * @struct ltc4284_init_param
 * @brief LTC4284 initialization parameters
 */
struct ltc4284_init_param {
	struct no_os_i2c_init_param *i2c_init;     /* I2C initialization parameters */
	uint32_t rsense_uohm;                       /* Sense resistor value (µΩ) */
	uint16_t vpwr_divider;                      /* Bus → RTNS pin divider ratio */
	uint16_t drain_divider;                     /* Bus → DRAIN pin divider ratio */
	uint8_t vsense_mv;                          /* V_ILIM at init (15..30 mV, integer) */
	struct no_os_gpio_init_param *alert_gpio;   /* ALERT# pin init (optional) */
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Device initialization and removal */
int ltc4284_init(struct ltc4284_dev **device,
		 const struct ltc4284_init_param *init_param);
int ltc4284_remove(struct ltc4284_dev *dev);

/* Register access functions */
int ltc4284_read_byte(struct ltc4284_dev *dev, uint8_t reg, uint8_t *val);
int ltc4284_write_byte(struct ltc4284_dev *dev, uint8_t reg, uint8_t val);
int ltc4284_read_word(struct ltc4284_dev *dev, uint8_t reg, uint16_t *val);
int ltc4284_write_word(struct ltc4284_dev *dev, uint8_t reg, uint16_t val);
int ltc4284_update_bits(struct ltc4284_dev *dev, uint8_t reg,
			uint8_t mask, uint8_t val);

/* Monitoring functions */
int ltc4284_read_vin(struct ltc4284_dev *dev, uint32_t *vin_mv);
int ltc4284_read_iin(struct ltc4284_dev *dev, uint32_t *iin_ma);
int ltc4284_read_vds(struct ltc4284_dev *dev, uint32_t *vds_mv);
int ltc4284_read_vout(struct ltc4284_dev *dev, uint32_t *vout_mv);
int ltc4284_read_power(struct ltc4284_dev *dev, uint32_t *power_mw);
int ltc4284_read_energy(struct ltc4284_dev *dev, uint64_t *energy_code);

/* Control functions */
int ltc4284_enable_fet(struct ltc4284_dev *dev, bool enable);
int ltc4284_clear_faults(struct ltc4284_dev *dev);
int ltc4284_set_ilim_mv(struct ltc4284_dev *dev, uint8_t mv);
int ltc4284_set_foldback(struct ltc4284_dev *dev, uint8_t code);
int ltc4284_set_oc_retry(struct ltc4284_dev *dev, uint8_t code);

/* Status and fault management */
int ltc4284_read_status(struct ltc4284_dev *dev, uint8_t *status);
int ltc4284_get_fault(struct ltc4284_dev *dev, uint8_t *faults);

/* EEPROM functions */
int ltc4284_arm_fault_log(struct ltc4284_dev *dev);
int ltc4284_restore_from_eeprom(struct ltc4284_dev *dev);

#endif /* __LTC4284_H__ */
