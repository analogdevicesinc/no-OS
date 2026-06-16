/***************************************************************************//**
 *   @file   ltc4284.h
 *   @brief  Header file of LTC4284 Driver
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
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

/* I2C 7-bit address range (configurable via ADR0/ADR1 pins) */
#define LTC4284_I2C_ADDR_0              0x28
#define LTC4284_I2C_ADDR_1              0x29
#define LTC4284_I2C_ADDR_2              0x2A  /* Default address */
#define LTC4284_I2C_ADDR_3              0x2B

/******************************************************************************/
/*************************** Register Definitions *****************************/
/******************************************************************************/

/* System Status and Control Registers */
#define LTC4284_REG_SYSTEM_STATUS       0x00  /* System status information (R) */
#define LTC4284_REG_ADC_STATUS          0x01  /* ADC conversion status (R) */
#define LTC4284_REG_INPUT_STATUS        0x02  /* GPIO input status (R) */
#define LTC4284_REG_FAULT_STATUS        0x03  /* Fault status information (R) */
#define LTC4284_REG_FAULT               0x04  /* System fault (R/W) */
#define LTC4284_REG_ADC_ALARM_LOG       0x05  /* ADC alarm log start (R/W, 5 bytes) */
#define LTC4284_REG_ADC_ALARM_LOG_1     0x05  /* ADC alarm log 1 */
#define LTC4284_REG_ADC_ALARM_LOG_2     0x06  /* ADC alarm log 2 */
#define LTC4284_REG_ADC_ALARM_LOG_3     0x07  /* ADC alarm log 3 */
#define LTC4284_REG_ADC_ALARM_LOG_4     0x08  /* ADC alarm log 4 */
#define LTC4284_REG_ADC_ALARM_LOG_5     0x09  /* ADC alarm log 5 */
#define LTC4284_REG_CONTROL             0x0A  /* Control registers start (R/W, 2 bytes) */
#define LTC4284_REG_CONTROL_1           0x0A  /* Control register 1 */
#define LTC4284_REG_CONTROL_2           0x0B  /* Control register 2 */
#define LTC4284_REG_RESERVED_0C         0x0C  /* Reserved (always returns 0) */
#define LTC4284_REG_CONFIG              0x0D  /* Config registers start (R/W, 3 bytes) */
#define LTC4284_REG_CONFIG_1            0x0D  /* Config register 1 */
#define LTC4284_REG_CONFIG_2            0x0E  /* Config register 2 */
#define LTC4284_REG_CONFIG_3            0x0F  /* Config register 3 */
#define LTC4284_REG_PGIO_CONFIG         0x10  /* GPIO config start (R/W, 2 bytes) */
#define LTC4284_REG_PGIO_CONFIG_1       0x10  /* GPIO config 1 */
#define LTC4284_REG_PGIO_CONFIG_2       0x11  /* GPIO config 2 */
#define LTC4284_REG_ADIO_CONFIG         0x12  /* ADC I/O config (R/W) */
#define LTC4284_REG_FAULT_ALERT         0x15  /* Fault alert configuration (R/W) */

/* ADC Threshold Registers (Min/Max) */
#define LTC4284_REG_SENSE_MIN_TH        0x1E  /* SENSE min threshold */
#define LTC4284_REG_SENSE_MAX_TH        0x1F  /* SENSE max threshold */
#define LTC4284_REG_VPWR_MIN_TH         0x20  /* VPWR min threshold */
#define LTC4284_REG_VPWR_MAX_TH         0x21  /* VPWR max threshold */
#define LTC4284_REG_POWER_MIN_TH        0x22  /* POWER min threshold (2 bytes) */
#define LTC4284_REG_POWER_MAX_TH        0x23  /* POWER max threshold (2 bytes) */
#define LTC4284_REG_ENERGY_TH           0x24  /* ENERGY threshold (3 bytes) */
#define LTC4284_REG_VPWR_MIN            0x27  /* VPWR minimum */
#define LTC4284_REG_VPWR_MAX            0x28  /* VPWR maximum */
#define LTC4284_REG_POWER_MIN           0x29  /* POWER minimum (2 bytes) */
#define LTC4284_REG_POWER_MAX           0x2A  /* POWER maximum (2 bytes) */
#define LTC4284_REG_ENERGY              0x2B  /* ENERGY accumulator (3 bytes) */

/* Additional ADC Channel Thresholds */
#define LTC4284_REG_DRNS_MIN_TH         0x32  /* DRNS min threshold */
#define LTC4284_REG_DRNS_MAX_TH         0x33  /* DRNS max threshold */
#define LTC4284_REG_DRAIN_MIN_TH        0x34  /* DRAIN min threshold */
#define LTC4284_REG_DRAIN_MAX_TH        0x35  /* DRAIN max threshold */
#define LTC4284_REG_SENSE1_MIN_TH       0x36  /* SENSE1 min threshold */
#define LTC4284_REG_SENSE1_MAX_TH       0x37  /* SENSE1 max threshold */
#define LTC4284_REG_SENSE2_MIN_TH       0x38  /* SENSE2 min threshold */
#define LTC4284_REG_SENSE2_MAX_TH       0x39  /* SENSE2 max threshold */
#define LTC4284_REG_ADIN12_MIN_TH       0x3A  /* ADIN2-ADIN1 min threshold */
#define LTC4284_REG_ADIN12_MAX_TH       0x3B  /* ADIN2-ADIN1 max threshold */
#define LTC4284_REG_ADIN34_MIN_TH       0x3C  /* ADIN4-ADIN3 min threshold */
#define LTC4284_REG_ADIN34_MAX_TH       0x3D  /* ADIN4-ADIN3 max threshold */

/* ADC Data Registers (16-bit, 2 bytes each) */
#define LTC4284_REG_SENSE               0x40  /* Current SENSE ADC value */
#define LTC4284_REG_VPWR                0x41  /* VPWR voltage */
#define LTC4284_REG_POWER               0x42  /* Power (2 bytes) */
#define LTC4284_REG_ADIO1               0x55  /* ADIO1 ADC value (2 bytes) */
#define LTC4284_REG_ADIO1_MIN           0x56  /* ADIO1 minimum (2 bytes) */
#define LTC4284_REG_ADIO1_MAX           0x57  /* ADIO1 maximum (2 bytes) */
#define LTC4284_REG_ADIO2               0x58  /* ADIO2 ADC value (2 bytes) */
#define LTC4284_REG_ADIO2_MIN           0x59  /* ADIO2 minimum (2 bytes) */
#define LTC4284_REG_ADIO2_MAX           0x5A  /* ADIO2 maximum (2 bytes) */
#define LTC4284_REG_ADIO3               0x5B  /* ADIO3 ADC value (2 bytes) */
#define LTC4284_REG_ADIO3_MIN           0x5C  /* ADIO3 minimum (2 bytes) */
#define LTC4284_REG_ADIO3_MAX           0x5D  /* ADIO3 maximum (2 bytes) */
#define LTC4284_REG_ADIO4               0x5E  /* ADIO4 ADC value (2 bytes) */
#define LTC4284_REG_ADIO4_MIN           0x5F  /* ADIO4 minimum (2 bytes) */
#define LTC4284_REG_ADIO4_MAX           0x60  /* ADIO4 maximum (2 bytes) */
#define LTC4284_REG_DRNS                0x61  /* DRNS voltage (2 bytes) */
#define LTC4284_REG_DRNS_MIN            0x62  /* DRNS minimum (2 bytes) */
#define LTC4284_REG_DRNS_MAX            0x63  /* DRNS maximum (2 bytes) */
#define LTC4284_REG_DRAIN               0x64  /* DRAIN voltage (2 bytes) */
#define LTC4284_REG_DRAIN_MIN           0x65  /* DRAIN minimum (2 bytes) */
#define LTC4284_REG_DRAIN_MAX           0x66  /* DRAIN maximum (2 bytes) */
#define LTC4284_REG_SENSE1              0x67  /* SENSE1 current (2 bytes) */
#define LTC4284_REG_SENSE1_MIN          0x68  /* SENSE1 minimum (2 bytes) */
#define LTC4284_REG_SENSE1_MAX          0x69  /* SENSE1 maximum (2 bytes) */
#define LTC4284_REG_SENSE2              0x6A  /* SENSE2 current (2 bytes) */
#define LTC4284_REG_SENSE2_MIN          0x6B  /* SENSE2 minimum (2 bytes) */
#define LTC4284_REG_SENSE2_MAX          0x6C  /* SENSE2 maximum (2 bytes) */

/* Snapshot and EEPROM Control */
#define LTC4284_REG_SNAPSHOT            0x84  /* Snapshot control */
#define LTC4284_REG_SNAPSHOT_1          0x84  /* Snapshot control 1 */
#define LTC4284_REG_SNAPSHOT_2          0x85  /* Snapshot control 2 */
#define LTC4284_REG_REBOOT              0xA2  /* Reboot control and status */

/* EEPROM Mirror Registers */
#define LTC4284_REG_EE_FAULT            0xA4  /* EEPROM fault record */
#define LTC4284_REG_EE_ADC_ALARM_LOG    0xA5  /* EEPROM alarm log (5 bytes) */
#define LTC4284_REG_EE_CONTROL          0xAA  /* EEPROM control (2 bytes) */
#define LTC4284_REG_EE_CONTROL_1        0xAA  /* EEPROM control 1 */
#define LTC4284_REG_EE_CONTROL_2        0xAB  /* EEPROM control 2 */
#define LTC4284_REG_RESERVED_AC         0xAC  /* EEPROM busy status */
#define LTC4284_REG_EE_CONFIG           0xAD  /* EEPROM config (3 bytes) */
#define LTC4284_REG_EE_CONFIG_1         0xAD  /* EEPROM config 1 */
#define LTC4284_REG_EE_CONFIG_2         0xAE  /* EEPROM config 2 */
#define LTC4284_REG_EE_CONFIG_3         0xAF  /* EEPROM config 3 */
#define LTC4284_REG_EE_PGIO_CONFIG      0xB0  /* EEPROM GPIO config (2 bytes) */
#define LTC4284_REG_EE_ADIO_CONFIG      0xB2  /* EEPROM ADC I/O config */

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

#define LTC4284_ADC_STATUS_ADC_READY            NO_OS_BIT(7)  /* ADC conversion done */
#define LTC4284_ADC_STATUS_ALERT                NO_OS_BIT(6)  /* Alert condition */
#define LTC4284_ADC_STATUS_ENERGY_OVERFLOW      NO_OS_BIT(5)  /* Energy overflow */

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

#define LTC4284_FAULT_STATUS_EXT_FAULT          NO_OS_BIT(7)  /* External fault */
#define LTC4284_FAULT_STATUS_POWER_FAULT        NO_OS_BIT(6)  /* Power failed */
#define LTC4284_FAULT_STATUS_FET_SHORT          NO_OS_BIT(5)  /* FET short */
#define LTC4284_FAULT_STATUS_UV_STATUS          NO_OS_BIT(4)  /* Undervoltage */
#define LTC4284_FAULT_STATUS_OV_STATUS          NO_OS_BIT(3)  /* Overvoltage */
#define LTC4284_FAULT_STATUS_FET_BAD_STATUS     NO_OS_BIT(2)  /* FET bad */
#define LTC4284_FAULT_STATUS_OC_STATUS          NO_OS_BIT(1)  /* Overcurrent */
#define LTC4284_FAULT_STATUS_PGI_FAULT          NO_OS_BIT(0)  /* Power good input fault */

/******************************************************************************/
/************************** FAULT Register (0x04) *****************************/
/******************************************************************************/

#define LTC4284_FAULT_EXT_FAULT_BIT             NO_OS_BIT(7)  /* External fault bit */
#define LTC4284_FAULT_POWER_FAULT_BIT           NO_OS_BIT(6)  /* Power failed bit */
#define LTC4284_FAULT_FET_SHORT_BIT             NO_OS_BIT(5)  /* FET short bit */
#define LTC4284_FAULT_UV_FAULT_BIT              NO_OS_BIT(4)  /* Undervoltage bit */
#define LTC4284_FAULT_OV_FAULT_BIT              NO_OS_BIT(3)  /* Overvoltage bit */
#define LTC4284_FAULT_FET_BAD_FAULT_BIT         NO_OS_BIT(2)  /* FET bad bit */
#define LTC4284_FAULT_OC_FAULT_BIT              NO_OS_BIT(1)  /* Overcurrent bit */
#define LTC4284_FAULT_PGI_FAULT_BIT             NO_OS_BIT(0)  /* PGI fault bit */

/* Shorter fault bit aliases for convenience */
#define LTC4284_FAULT_OV                        LTC4284_FAULT_OV_FAULT_BIT
#define LTC4284_FAULT_UV                        LTC4284_FAULT_UV_FAULT_BIT
#define LTC4284_FAULT_OC                        LTC4284_FAULT_OC_FAULT_BIT
#define LTC4284_FAULT_POWER_BAD                 LTC4284_FAULT_POWER_FAULT_BIT
#define LTC4284_FAULT_FET                       LTC4284_FAULT_FET_BAD_FAULT_BIT
#define LTC4284_FAULT_OT                        LTC4284_FAULT_EXT_FAULT_BIT

/******************************************************************************/
/********************* CONTROL_1 Register (0x0A) ******************************/
/******************************************************************************/

#define LTC4284_CONTROL_1_ON                    NO_OS_BIT(7)  /* Turn on FET */
#define LTC4284_CONTROL_1_DVDT                  NO_OS_BIT(6)  /* dV/dt mode */
#define LTC4284_CONTROL_1_GATE2_ON              NO_OS_BIT(5)  /* Gate2 enable */
#define LTC4284_CONTROL_1_MASS_WRITE_ENABLE     NO_OS_BIT(4)  /* Mass write enable */
#define LTC4284_CONTROL_1_PWR_ALARM_MODE        NO_OS_BIT(3)  /* Power alarm mode */
#define LTC4284_CONTROL_1_PG_RESET_CNTRL        NO_OS_BIT(1)  /* PG reset control */
#define LTC4284_CONTROL_1_FET_SHORT_PRESENT     NO_OS_BIT(0)  /* FET short present */

/******************************************************************************/
/********************* CONTROL_2 Register (0x0B) ******************************/
/******************************************************************************/

#define LTC4284_CONTROL_2_OC_RETRY              NO_OS_GENMASK(7, 6)  /* OC retry */
#define LTC4284_CONTROL_2_UV_RETRY              NO_OS_GENMASK(5, 4)  /* UV retry */
#define LTC4284_CONTROL_2_OV_RETRY              NO_OS_GENMASK(3, 2)  /* OV retry */
#define LTC4284_CONTROL_2_FET_BAD_RETRY         NO_OS_GENMASK(1, 0)  /* FET bad retry */

/******************************************************************************/
/********************** CONFIG_1 Register (0x0D) ******************************/
/******************************************************************************/

#define LTC4284_CONFIG_1_ILIM                   NO_OS_GENMASK(7, 4)  /* Current limit */
#define LTC4284_CONFIG_1_FB                     NO_OS_GENMASK(3, 0)  /* Foldback */

/******************************************************************************/
/********************** CONFIG_2 Register (0x0E) ******************************/
/******************************************************************************/

#define LTC4284_CONFIG_2_COOLING_DL             NO_OS_GENMASK(7, 6)  /* Cooling delay */
#define LTC4284_CONFIG_2_FTBD_DL                NO_OS_GENMASK(5, 4)  /* FET bad delay */
#define LTC4284_CONFIG_2_REBOOT_DL              NO_OS_GENMASK(3, 2)  /* Reboot delay */
#define LTC4284_CONFIG_2_ADC_16BIT              NO_OS_BIT(1)  /* 16-bit ADC mode */
#define LTC4284_CONFIG_2_METER_POWER            NO_OS_BIT(0)  /* Meter power */

/******************************************************************************/
/********************** CONFIG_3 Register (0x0F) ******************************/
/******************************************************************************/

#define LTC4284_CONFIG_3_EXTFLT_TURN_OFF        NO_OS_BIT(7)  /* Ext fault turn off */
#define LTC4284_CONFIG_3_PGI_TURN_OFF           NO_OS_BIT(6)  /* PGI turn off */

/******************************************************************************/
/******************** ADC Conversion Constants ********************************/
/******************************************************************************/

/* ADC reference and LSB values (from datasheet specifications) */
#define LTC4284_ADC_VREF_MV                     2048    /* 2.048V reference */
#define LTC4284_ADC_8BIT_LEVELS                 256     /* 8-bit resolution */
#define LTC4284_ADC_16BIT_LEVELS                65536   /* 16-bit resolution */

/* Voltage scaling (depends on resistor dividers - to be calculated per application) */
#define LTC4284_VPWR_LSB_UV                     32000   /* VPWR LSB in microvolts */
#define LTC4284_DRAIN_LSB_UV                    32000   /* DRAIN LSB in microvolts */

/* Current sense voltage LSB (programmable 15mV to 30mV) */
#define LTC4284_SENSE_MIN_MV                    15      /* Minimum sense voltage */
#define LTC4284_SENSE_MAX_MV                    30      /* Maximum sense voltage */

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
	uint16_t rsense_mohm;                /* Current sense resistor (mΩ) */
	uint8_t vsense_mv;                   /* Sense voltage threshold (mV) */
	struct no_os_gpio_desc *alert_gpio;  /* ALERT# pin descriptor (optional) */
};

/**
 * @struct ltc4284_init_param
 * @brief LTC4284 initialization parameters
 */
struct ltc4284_init_param {
	struct no_os_i2c_init_param *i2c_init;     /* I2C initialization parameters */
	uint8_t i2c_addr;                           /* Device I2C address */
	uint16_t rsense_mohm;                       /* Sense resistor value (mΩ) */
	uint8_t vsense_mv;                          /* Sense voltage (15, 20, 25, 30 mV) */
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
int ltc4284_read_vin(struct ltc4284_dev *dev, uint16_t *vin_mv);
int ltc4284_read_iin(struct ltc4284_dev *dev, uint16_t *iin_ma);
int ltc4284_read_vout(struct ltc4284_dev *dev, uint16_t *vout_mv);
int ltc4284_read_power(struct ltc4284_dev *dev, uint32_t *power_mw);
int ltc4284_read_energy(struct ltc4284_dev *dev, uint64_t *energy_mj);
int ltc4284_read_temperature(struct ltc4284_dev *dev, int16_t *temp_mc);

/* Control functions */
int ltc4284_set_current_limit(struct ltc4284_dev *dev, uint8_t ilim_code);
int ltc4284_enable_fet(struct ltc4284_dev *dev, bool enable);
int ltc4284_clear_faults(struct ltc4284_dev *dev);

/* Status and fault management */
int ltc4284_read_status(struct ltc4284_dev *dev, uint8_t *status);
int ltc4284_get_fault(struct ltc4284_dev *dev, uint8_t *faults);

/* EEPROM functions */
int ltc4284_store_config(struct ltc4284_dev *dev);
int ltc4284_restore_config(struct ltc4284_dev *dev);

#endif /* __LTC4284_H__ */
