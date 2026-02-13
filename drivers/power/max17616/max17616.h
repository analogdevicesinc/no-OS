/***************************************************************************//**
*   @file   max17616.h
*   @brief  Header file of the MAX17616 Driver
*   @author Carlos Jones (carlosjr.jones@analog.com)
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

#ifndef __MAX17616_H__
#define __MAX17616_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

#define MAX17616_DS_0B			(0 << 8) /* Send-only command */
#define MAX17616_DS_1B			(1 << 8) /* Byte */
#define MAX17616_DS_2B			(2 << 8) /* Word */
#define MAX17616_DS_3B			(3 << 8) /* Block of size 3*/
#define MAX17616_DS_5B			(5 << 8) /* Block of size 5*/
#define MAX17616_DS_9B			(9 << 8) /* Block of size 9*/
#define MAX17616_DS_10B			(10 << 8) /* Block of size 10*/
#define MAX17616_DF_NONE		(0 << 12) /* None */
#define MAX17616_DF_LINEAR11		(1 << 12) /* Linear 11 */
#define MAX17616_DF_LINEAR16		(2 << 12) /* Linear 16 */
#define MAX17616_DF_DIRECT		(3 << 12) /* Direct */
#define MAX17616_DF_VID			(4 << 12) /* VID */
#define MAX17616_DF_IEEE754		(5 << 12) /* IEEE754 */

#define MAX17616_CMD(x)			((uint8_t)(x & 0xFF))
#define MAX17616_DATA_SIZE(x)		no_os_field_get(NO_OS_GENMASK(11,8), x)
#define MAX17616_DATA_FORMAT(x)		no_os_field_get(NO_OS_GENMASK(15,12), x)

#define MAX17616_OPERATION		(MAX17616_DS_1B | 0x01)
#define MAX17616_CLEAR_FAULTS		(MAX17616_DS_0B | 0x03)
#define MAX17616_CAPABILITY		(MAX17616_DS_1B | 0x19)
#define MAX17616_SMBALERT_MASK		(MAX17616_DS_1B | 0x1B)
#define MAX17616_VOUT_UV_FAULT_LIMIT	(MAX17616_DS_1B | 0x44)
#define MAX17616_STATUS_BYTE		(MAX17616_DS_1B | 0x78)
#define MAX17616_STATUS_WORD		(MAX17616_DS_2B | 0x79)
#define MAX17616_STATUS_VOUT		(MAX17616_DS_1B | 0x7A)
#define MAX17616_STATUS_IOUT		(MAX17616_DS_1B | 0x7B)
#define MAX17616_STATUS_INPUT		(MAX17616_DS_1B | 0x7C)
#define MAX17616_STATUS_TEMPERATURE	(MAX17616_DS_1B | 0x7D)
#define MAX17616_STATUS_CML		(MAX17616_DS_1B | 0x7E)
#define MAX17616_STATUS_MFR_SPECIFIC	(MAX17616_DS_1B | 0x80)
#define MAX17616_READ_VIN		(MAX17616_DF_DIRECT | MAX17616_DS_2B | 0x88)
#define MAX17616_READ_VOUT		(MAX17616_DF_DIRECT | MAX17616_DS_2B | 0x8B)
#define MAX17616_READ_IOUT		(MAX17616_DF_DIRECT | MAX17616_DS_2B | 0x8C)
#define MAX17616_READ_TEMPERATURE_1	(MAX17616_DF_DIRECT | MAX17616_DS_2B | 0x8D)
#define MAX17616_PMBUS_REVISION		(MAX17616_DS_2B | 0x98)

#define MAX17616_MFR_ID			(MAX17616_DS_5B | 0x99)
#define MAX17616_MFR_MODEL		(MAX17616_DS_10B | 0x9A)
#define MAX17616_MFR_REVISION		(MAX17616_DS_3B | 0x9B)
#define MAX17616_SET_CLMODE		(MAX17616_DS_1B | 0xC4)
#define MAX17616_SET_ISTART_RATIO	(MAX17616_DS_1B | 0xC5)
#define MAX17616_SET_TSTOC		(MAX17616_DS_1B | 0xC6)
#define MAX17616_SET_ISTLIM		(MAX17616_DS_1B | 0xC7)
#define MAX17616_IC_DEVICE_ID		(MAX17616_DS_9B | 0xAD)
#define MAX17616_IC_DEVICE_REV		(MAX17616_DS_3B | 0xAE)

/* Status types masks */
#define MAX17616_STATUS_BYTE_TYPE_MSK		0x01
#define MAX17616_STATUS_VOUT_TYPE_MSK		0x02
#define MAX17616_STATUS_IOUT_TYPE_MSK		0x04
#define MAX17616_STATUS_INPUT_TYPE_MSK		0x08
#define MAX17616_STATUS_TEMP_TYPE_MSK		0x10
#define MAX17616_STATUS_CML_TYPE_MSK		0x20
#define MAX17616_STATUS_MFR_SPECIFIC_TYPE_MSK	0x40
#define MAX17616_STATUS_WORD_TYPE_MSK		0x80
#define MAX17616_STATUS_ALL_TYPE_MSK		0xFF

/* Maximum PMBus block transfer size per PMBus spec */
#define MAX17616_MAX_BLOCK_SIZE		255

/**
 * Direct exponent scale factor for voltage/current/temperature readings
 */
#define MAX17616_DIRECT_EXPONENT_SCALE	10	/* 10^(-R) for R=-1 */
#define MAX17616_MILLIUNIT_SCALE	1000	/* mV, mA, m°C conversion */

#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

enum max17616_chip_id {
	ID_MAX17616,
	ID_MAX17616A,
	ID_MAX17616_CHIP_COUNT
};

/* Current limit mode options */
enum max17616_current_limit_mode {
	MAX17616_CLMODE_LATCH_OFF = 0x00,	/* bit[7:6] 00b - latch-off mode */
	MAX17616_CLMODE_CONTINUOUS = 0x40,	/* bit[7:6] 01b - continuous mode */
	MAX17616_CLMODE_AUTO_RETRY = 0x80	/* bit[7:6] 10b - auto-retry mode */
};

/* Current start ratio options - fractions of hardware-configured current limit
 * Note: The actual current limit is set by external resistor on SETI pin */
enum max17616_istart_ratio {
	MAX17616_ISTART_FULL,			/* 000b = Current Limit (1/1) */
	MAX17616_ISTART_HALF,			/* 001b = Current Limit / 2 */
	MAX17616_ISTART_QUARTER,		/* 010b = Current Limit / 4 */
	MAX17616_ISTART_EIGHTH,			/* 011b = Current Limit / 8 */
	MAX17616_ISTART_SIXTEENTH		/* 100b = Current Limit / 16 */
};

/* Overcurrent timeout duration options */
enum max17616_overcurrent_timeout {
	MAX17616_TIMEOUT_400US,			/* 00b = 400 microseconds */
	MAX17616_TIMEOUT_1MS,			/* 01b = 1 millisecond */
	MAX17616_TIMEOUT_4MS,			/* 10b = 4 milliseconds */
	MAX17616_TIMEOUT_24MS			/* 11b = 24 milliseconds */
};

/* Overcurrent limit ratio options - multiples of hardware current limit */
enum max17616_overcurrent_limit {
	MAX17616_OC_LIMIT_1_25,			/* 00b = 1.25:1 ratio */
	MAX17616_OC_LIMIT_1_50,			/* 01b = 1.50:1 ratio */
	MAX17616_OC_LIMIT_1_75,			/* 10b = 1.75:1 ratio */
	MAX17616_OC_LIMIT_2_00			/* 11b = 2.00:1 ratio */
};

/* Nominal voltage selection options for VOUT UV fault limit (bits 4:2) */
enum max17616_nominal_voltage {
	MAX17616_NOMINAL_5V,			/* 000b = 5V */
	MAX17616_NOMINAL_9V,			/* 001b = 9V */
	MAX17616_NOMINAL_12V,			/* 010b = 12V */
	MAX17616_NOMINAL_24V,			/* 011b = 24V */
	MAX17616_NOMINAL_36V,			/* 100b = 36V */
	MAX17616_NOMINAL_48V,			/* 101b = 48V */
	MAX17616_NOMINAL_60V,			/* 110b = 60V */
	MAX17616_NOMINAL_72V			/* 111b = 72V */
};

/* PGOOD rising threshold options for VOUT UV fault limit (bits 1:0) */
enum max17616_pgood_threshold {
	MAX17616_PGOOD_MINUS_10_PERCENT = 0x00,	/* 00b = -10% */
	MAX17616_PGOOD_MINUS_20_PERCENT = 0x01,	/* 01b = -20% */
	MAX17616_PGOOD_MINUS_30_PERCENT = 0x02	/* 10b = -30% */
};

/* Bit field macros for current limit mode */
#define MAX17616_CLMODE_MASK			NO_OS_GENMASK(7, 6)
#define MAX17616_CLMODE_LATCH_OFF_BITS		0x00
#define MAX17616_CLMODE_CONTINUOUS_BITS		0x01
#define MAX17616_CLMODE_AUTO_RETRY_BITS		0x02

/* Bit field macros for ISTART ratio */
#define MAX17616_ISTART_MASK			NO_OS_GENMASK(3, 0)
#define MAX17616_ISTART_FULL_BITS		0x00
#define MAX17616_ISTART_HALF_BITS		0x01
#define MAX17616_ISTART_QUARTER_BITS		0x02
#define MAX17616_ISTART_EIGHTH_BITS		0x03
#define MAX17616_ISTART_SIXTEENTH_BITS		0x04

/* Bit field macros for overcurrent timeout */
#define MAX17616_TIMEOUT_MASK			NO_OS_GENMASK(1, 0)
#define MAX17616_TIMEOUT_400US_BITS		0x00
#define MAX17616_TIMEOUT_1MS_BITS		0x01
#define MAX17616_TIMEOUT_4MS_BITS		0x02
#define MAX17616_TIMEOUT_24MS_BITS		0x03

/* Bit field macros for overcurrent limit */
#define MAX17616_OC_LIMIT_MASK			NO_OS_GENMASK(1, 0)
#define MAX17616_OC_LIMIT_1_25_BITS		0x00
#define MAX17616_OC_LIMIT_1_50_BITS		0x01
#define MAX17616_OC_LIMIT_1_75_BITS		0x02
#define MAX17616_OC_LIMIT_2_00_BITS		0x03

/* Bit field macros for nominal voltage configuration */
#define MAX17616_NOMINAL_VOLTAGE_MASK		NO_OS_GENMASK(4, 2)
#define MAX17616_NOMINAL_5V_BITS		0x00
#define MAX17616_NOMINAL_9V_BITS		0x01
#define MAX17616_NOMINAL_12V_BITS		0x02
#define MAX17616_NOMINAL_24V_BITS		0x03
#define MAX17616_NOMINAL_36V_BITS		0x04
#define MAX17616_NOMINAL_48V_BITS		0x05
#define MAX17616_NOMINAL_60V_BITS		0x06
#define MAX17616_NOMINAL_72V_BITS		0x07

/* Bit field macros for PGOOD threshold configuration */
#define MAX17616_PGOOD_MASK			NO_OS_GENMASK(1, 0)
#define MAX17616_PGOOD_MINUS_10_PERCENT_BITS	0x00
#define MAX17616_PGOOD_MINUS_20_PERCENT_BITS	0x01
#define MAX17616_PGOOD_MINUS_30_PERCENT_BITS	0x02

/* Value types for high-level read/write functions */
enum max17616_value_type {
	MAX17616_VIN,		/* Input voltage in volts */
	MAX17616_VOUT,		/* Output voltage in volts */
	MAX17616_IOUT,		/* Output current in amps */
	MAX17616_TEMP,		/* Temperature in degrees Celsius */
	MAX17616_POWER,		/* Power in watts */
};

/* Status structure */
struct max17616_status {
	uint16_t word;
	uint8_t byte;
	uint8_t vout;
	uint8_t iout;
	uint8_t input;
	uint8_t temperature;
	uint8_t cml;
	uint8_t other;
	uint8_t mfr_specific;
};

struct max17616_chip_specific_info {
	char *ic_dev_id;
	uint8_t ic_dev_id_size;
};

struct max17616_chip_info {
	char *mfr_id;
	uint8_t mfr_id_size;
	char *mfr_rev;
	uint8_t mfr_rev_size;
	uint8_t pmbus_rev;
	struct max17616_chip_specific_info specific_info[ID_MAX17616_CHIP_COUNT];
};

struct max17616_dev {
	struct no_os_i2c_desc *i2c_desc;
	enum max17616_chip_id chip_id;
	struct max17616_chip_info *chip_info;
};

struct max17616_init_param {
	struct no_os_i2c_init_param *i2c_init;
	enum max17616_chip_id chip_id;
	struct max17616_chip_info chip_info;
};

enum max17616_status_byte {
	MAX17616_STATUS_BIT_GENERAL,		/* Not supported */
	MAX17616_STATUS_BIT_CML,		/* Comms, memory, or logic Fault */
	MAX17616_STATUS_BIT_TEMPERATURE,	/* Temperature Fault or Warning */
	MAX17616_STATUS_BIT_VIN_UV,		/* Input Under Voltage Fault */
	MAX17616_STATUS_BIT_IOUT_OC,		/* Output Over Current Fault */
	MAX17616_STATUS_BIT_VOUT_OV,		/* An output overvoltage fault occurred */
	MAX17616_STATUS_BIT_OFF,		/* Max17616 is not enabled */
	MAX17616_STATUS_BIT_BUSY		/* Not supported */
};

enum max17616_status_word_byte_high {
	MAX17616_STATUS_BIT_STARTUP,		/* Not supported */
	MAX17616_STATUS_BIT_OTHER,		/* Not supported */
	MAX17616_STATUS_BIT_FANS,		/* Not supported */
	MAX17616_STATUS_BIT_POWER_GOOD,		/* Not supported */
	MAX17616_STATUS_BIT_MFR,		/* Manufacturer specific Fault */
	MAX17616_STATUS_BIT_INPUT,		/* Input V, I, or P Fault */
	MAX17616_STATUS_BIT_IOUT_POUT,		/* Output current or power Fault */
	MAX17616_STATUS_BIT_VOUT		/* Output voltage Fault */
};

enum max17616_general_bit_fault_byte {
	MAX17616_GENERAL_FAULT_NS_BIT0,		/* Not supported */
	MAX17616_GENERAL_FAULT_NS_BIT1,		/* Not supported */
	MAX17616_GENERAL_FAULT_NS_BIT2,		/* Not supported */
	MAX17616_GENERAL_FAULT_NS_BIT3,		/* Not supported */
	MAX17616_GENERAL_FAULT_NS_BIT4,		/* Not supported */
	MAX17616_GENERAL_FAULT_NS_BIT5,		/* Not supported */
	MAX17616_GENERAL_FAULT_NS_BIT6,		/* Not supported */
	MAX17616_GENERAL_FAULT_NS_BIT7		/* Not supported */
};

enum max17616_cml_bit_fault_byte {
	MAX17616_CML_FAULT_NS_BIT0,		/* Not supported */
	MAX17616_CML_FAULT_OTHER,		/* Communications other error */
	MAX17616_CML_FAULT_NS_BIT2,		/* Not supported */
	MAX17616_CML_FAULT_NS_BIT3,		/* Not supported */
	MAX17616_CML_FAULT_MEM_ERROR,		/* Memory Error detected */
	MAX17616_CML_FAULT_PEC_FAIL,		/* Packet error checking failed */
	MAX17616_CML_FAULT_DATA,		/* Invalid or unsupported data received */
	MAX17616_CML_FAULT_CMD			/* Invalid or unsupported command received */
};

enum max17616_temperature_bit_fault_byte {
	MAX17616_TEMPERATURE_FAULT_NS_BIT0,		/* Not supported */
	MAX17616_TEMPERATURE_FAULT_NS_BIT1,		/* Not supported */
	MAX17616_TEMPERATURE_FAULT_NS_BIT2,		/* Not supported */
	MAX17616_TEMPERATURE_FAULT_NS_BIT3,		/* Not supported */
	MAX17616_TEMPERATURE_FAULT_UT_FAULT,		/* Not supported */
	MAX17616_TEMPERATURE_FAULT_UT_WARNING,		/* Not supported */
	MAX17616_TEMPERATURE_FAULT_OT_WARNING,		/* Not supported */
	MAX17616_TEMPERATURE_FAULT_OT_FAULT		/* An overtemperature fault occurred */
};

enum max17616_input_fault_bit {
	MAX17616_INPUT_FAULT_PIN_OP_WARNING,		/* Not supported */
	MAX17616_INPUT_FAULT_IIN_OC_WARNING,		/* Not supported */
	MAX17616_INPUT_FAULT_IIN_OC_FAULT,		/* Not supported */
	MAX17616_INPUT_FAULT_UNIT_OFF,			/* Not supported */
	MAX17616_INPUT_FAULT_VIN_UV_FAULT,		/* An input undervoltage fault occurred */
	MAX17616_INPUT_FAULT_VIN_UV_WARNING,		/* Not supported */
	MAX17616_INPUT_FAULT_VIN_OV_WARNING,		/* Not supported */
	MAX17616_INPUT_FAULT_VIN_OV_FAULT		/* An input overvoltage fault occurred */
};

enum max17616_iout_fault_bit {
	MAX17616_IOUT_FAULT_NS_BIT0,			/* An output overcurrent fault occurred */
	MAX17616_IOUT_FAULT_NS_BIT1,			/* Not supported */
	MAX17616_IOUT_FAULT_NS_BIT2,			/* Not supported */
	MAX17616_IOUT_FAULT_NS_BIT3,			/* Not supported */
	MAX17616_IOUT_FAULT_NS_BIT4,			/* Not supported */
	MAX17616_IOUT_FAULT_OUT_OC_WARNING,		/* Not supported */
	MAX17616_IOUT_FAULT_OUT_OC_UV_LO,		/* An output overcurrent and low voltage fault occurred */
	MAX17616_IOUT_FAULT_OUT_OC_FAULT		/* An output overcurrent fault occurred */
};

enum max17616_vout_fault_bit {
	MAX17616_VOUT_FAULT_VOUT_TRACKING_ERROR,	/* Not supported */
	MAX17616_VOUT_FAULT_TOFF_MAX_WARNING,		/* Not supported */
	MAX17616_VOUT_FAULT_TON_MAX_FAULT,		/* Not supported */
	MAX17616_VOUT_FAULT_VOUT_MAX_WARNING,		/* Not supported */
	MAX17616_VOUT_FAULT_VOUT_UV_FAULT,		/* Not supported */
	MAX17616_VOUT_FAULT_VOUT_UV_WARNING,		/* Output has gone below the output undervoltage threshold */
	MAX17616_VOUT_FAULT_VOUT_OV_WARNING,		/* Output regulation event has occurred */
	MAX17616_VOUT_FAULT_VOUT_OV_FAULT		/* Not supported */
};

enum max17616_mfr_specific_fault_bit {
	MAX17616_MFR_FAULT_NS_BIT0,			/* Not supported */
	MAX17616_MFR_FAULT_NS_BIT1,			/* Not supported */
	MAX17616_MFR_FAULT_REVERSE_CURRENT,		/* A reverse current fault occurred */
	MAX17616_MFR_FAULT_OUTPUT_SHORT_CIRCUIT,	/* An output short circuit fault occurred */
	MAX17616_MFR_FAULT_NS_BIT4,			/* Not supported */
	MAX17616_MFR_FAULT_SOFT_START,			/* Soft start failed */
	MAX17616_MFR_FAULT_IMON_PIN,			/* IMON pin fault */
	MAX17616_MFR_FAULT_SETI_PIN			/* SETI pin fault */
};

/* Fault Management Module Definitions */

/* Fault group identifiers matching STATUS_WORD bits */
enum max17616_fault_group {
	MAX17616_FAULT_GRP_NONE = 0x00,
	MAX17616_FAULT_GRP_CML = 0x01,
	MAX17616_FAULT_GRP_TEMPERATURE = 0x02,
	MAX17616_FAULT_GRP_VIN_UV = 0x04,
	MAX17616_FAULT_GRP_IOUT_OC = 0x08,
	MAX17616_FAULT_GRP_VOUT_OV = 0x10,
	MAX17616_FAULT_GRP_INPUT = 0x20,
	MAX17616_FAULT_GRP_IOUT_POUT = 0x40,
	MAX17616_FAULT_GRP_VOUT = 0x80,
	MAX17616_FAULT_GRP_MFR_SPECIFIC = 0x100,
	MAX17616_FAULT_GRP_ALL = 0xFFFF
};

/* Individual fault information structure */
struct max17616_fault_info {
	uint16_t group;
	uint8_t bit;
	const char *description;
	bool is_supported;
};

/**
 * @struct max17616_telemetry
 * @brief Telemetry data structure with milliunits
 */
struct max17616_telemetry {
	int32_t vin_mv;		/* Input voltage in millivolts */
	int32_t vout_mv;	/* Output voltage in millivolts */
	int32_t iout_ma;	/* Output current in milliamps */
	int32_t pin_mw;		/* Input power in milliwatts */
	int32_t pout_mw;	/* Output power in milliwatts */
	int32_t temp1_mc;	/* Temperature in millidegrees Celsius */
	uint8_t valid_mask;	/* Bitmask indicating which values are valid */
};

/* Initialize the device structure */
int max17616_init(struct max17616_dev **dev,
		  struct max17616_init_param *init_param);

/* Free or remove device instance */
int max17616_remove(struct max17616_dev *dev);

/* Clear status registers */
int max17616_clear_faults(struct max17616_dev *dev);

/*  Perform a PMBus write_byte operation */
int max17616_write_byte(struct max17616_dev *dev, uint8_t cmd, uint8_t value);

/* Read a block of bytes */
int max17616_read_block_data(struct max17616_dev *dev, uint8_t cmd,
			     uint8_t *data, size_t nbytes);

/* Perform a PMBus read_word operation */
int max17616_read_word(struct max17616_dev *dev, uint8_t cmd, uint16_t *word);

/* Send a PMBus command to the device */
int max17616_send_byte(struct max17616_dev *dev, uint8_t cmd);

/* Read a single byte from the device */
int max17616_read_byte(struct max17616_dev *dev, uint8_t cmd, uint8_t *data);

/* Fault Management Module Functions */
int max17616_read_status(struct max17616_dev *dev,
			 struct max17616_status *status);

/* Get human-readable description for a specific fault */
const char *max17616_get_fault_description(uint16_t group, uint8_t bit);

/* High-level value reading functions */
int max17616_read_value(struct max17616_dev *dev,
			enum max17616_value_type value_type,
			int32_t *value_milliunit);

/* Enhanced operation control functions */
int max17616_set_operation_state(struct max17616_dev *dev, bool enable);
int max17616_get_operation_state(struct max17616_dev *dev, bool *enabled);

/* Manufacturer-specific command functions */
int max17616_set_current_limit_mode(struct max17616_dev *dev,
				    enum max17616_current_limit_mode clmode);
int max17616_get_current_limit_mode(struct max17616_dev *dev,
				    enum max17616_current_limit_mode *clmode);
int max17616_set_istart_ratio(struct max17616_dev *dev,
			      enum max17616_istart_ratio ratio);
int max17616_get_istart_ratio(struct max17616_dev *dev,
			      enum max17616_istart_ratio *istart_ratio);
int max17616_set_overcurrent_timeout(struct max17616_dev *dev,
				     enum max17616_overcurrent_timeout timeout);
int max17616_get_overcurrent_timeout(struct max17616_dev *dev,
				     enum max17616_overcurrent_timeout *timeout);
int max17616_set_overcurrent_limit(struct max17616_dev *dev,
				   enum max17616_overcurrent_limit limit);
int max17616_get_overcurrent_limit(struct max17616_dev *dev,
				   enum max17616_overcurrent_limit *istlim);

/* VOUT UV fault limit configuration functions */
int max17616_set_vout_uv_fault_limit_config(struct max17616_dev *dev,
		enum max17616_nominal_voltage voltage,
		enum max17616_pgood_threshold threshold);
int max17616_get_vout_uv_fault_limit_config(struct max17616_dev *dev,
		enum max17616_nominal_voltage *voltage,
		enum max17616_pgood_threshold *threshold);

/* PMBus capability function */
int max17616_read_capability(struct max17616_dev *dev, uint8_t *capability);

/* Comprehensive telemetry functions */
int max17616_read_telemetry_all(struct max17616_dev *dev,
				struct max17616_telemetry *telemetry);

/* Read STATUS_BYTE register */
int max17616_read_status_byte(struct max17616_dev *dev, uint8_t *status_byte);

/* Read individual status registers */
int max17616_read_status_vout(struct max17616_dev *dev, uint8_t *status_vout);
int max17616_read_status_iout(struct max17616_dev *dev, uint8_t *status_iout);
int max17616_read_status_input(struct max17616_dev *dev, uint8_t *status_input);
int max17616_read_status_temperature(struct max17616_dev *dev,
				     uint8_t *status_temperature);
int max17616_read_status_cml(struct max17616_dev *dev, uint8_t *status_cml);
int max17616_read_status_mfr_specific(struct max17616_dev *dev,
				      uint8_t *status_mfr);

/* Device identification and verification functions */
int max17616_verify_manufacturer_id(struct max17616_dev *dev);
int max17616_identify_chip_variant(struct max17616_dev *dev);
int max17616_verify_pmbus_revision(struct max17616_dev *dev);

#endif /* __MAX17616_H__ */
