/***************************************************************************//**
 *   @file   adxl313.h
 *   @brief  Header file of ADXL313 Driver.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef __ADXL313_H__
#define __ADXL313_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* SPI commands */
#define ADXL313_SPI_READ					(0x80)
#define ADXL313_SPI_WRITE					(0x00)
#define ADXL313_MULTIBIT					NO_OS_BIT(6)

/* ADXL313 register map */
#define ADXL313_REG_DEVID_AD					0x00
#define ADXL313_REG_DEVID1_AD					0x01
#define ADXL313_REG_PARTID					0x02
#define ADXL313_REG_REVID					0x03
#define ADXL313_REG_XID						0x04
#define ADXL313_REG_SOFT_RESET					0x18
#define ADXL313_REG_OFS_AXIS(index)				(0x1E + (index))
#define ADXL313_REG_THRESH_ACT					0x24
#define ADXL313_REG_THRESH_INACT				0x25
#define ADXL313_REG_TIME_INACT					0x26
#define ADXL313_REG_ACT_INACT_CTL				0x27
#define ADXL313_REG_BW_RATE					0x2C
#define ADXL313_REG_POWER_CTL					0x2D
#define ADXL313_REG_INT_EN					0x2E
#define ADXL313_REG_INT_MAP					0x2F
#define ADXL313_REG_INT_SRC					0x30
#define ADXL313_REG_DATA_FORMAT					0x31
#define ADXL313_REG_DATA_AXIS(index)				(0x32 + ((index) * 2))
#define ADXL313_REG_FIFO_CTL					0x38
#define ADXL313_REG_FIFO_STATUS					0x39

/* ADXL313_REG_DEVID_AD value */
#define ADXL313_DEVID						0xAD
/* ADXL313_REG_DEVID1_AD value */
#define ADXL313_DEVID1						0x1D
/* ADXL313_REG_PARTID value, 0xCB, 313 in octal */
#define ADXL313_PARTID						0xCB
/* ADXL314_DEVID_AD value, same as ADXL312 */
#define ADXL314_DEVID						0xE5

/* ADXL313 reset key */
#define ADXL313_RESET_KEY					0x52

/* Maximum number of FIFO samples */
#define ADXL313_MAX_FIFO_SAMPLES_VAL  				0x60

/* Device bandwidth settings, ADXL313_REG_BW_RATE definitions */
#define ADXL313_RATE_MSK					NO_OS_GENMASK(3, 0)
#define ADXL313_LOW_POWER_OP					NO_OS_BIT(4)

/* ADXL313_REG_POWER_CTL definitions */
#define ADXL313_POWER_CTL_I2C_DISABLE				NO_OS_BIT(6)
#define ADXL313_POWER_CTL_LINK					NO_OS_BIT(5)
#define ADXL313_POWER_CTL_AUTO_SLEEP				NO_OS_BIT(4)
#define ADXL313_POWER_CTL_MEASURE				NO_OS_BIT(3)
#define ADXL313_POWER_CTL_SLEEP					NO_OS_BIT(2)
#define ADXL313_POWER_CTL_WAKEUP(x)				((x) & 0x3)

/* ADXL313_REG_INT_EN definitions */
#define ADXL313_REG_INT_EN_DATA_READY				NO_OS_BIT(7)
#define ADXL313_REG_INT_EN_ACTIVITY				NO_OS_BIT(4)
#define ADXL313_REG_INT_EN_INACTIVITY				NO_OS_BIT(3)
#define ADXL313_REG_INT_EN_WATERMARK				NO_OS_BIT(1)
#define ADXL313_REG_INT_EN_OVERRUN				NO_OS_BIT(0)

/* ADXL313_REG_INT_MAP definitions */
#define ADXL313_REG_INT_MAP_DR_INT2				NO_OS_BIT(7)
#define ADXL313_REG_INT_MAP_ACT_INT2				NO_OS_BIT(4)
#define ADXL313_REG_INT_MAP_INACT_INT2				NO_OS_BIT(3)
#define ADXL313_REG_INT_MAP_WM_INT2				NO_OS_BIT(1)
#define ADXL313_REG_INT_MAP_OVER_INT2				NO_OS_BIT(0)

/* ADXL313_REG_INT_SRC definitions */
#define ADXL313_REG_INT_SRC_DATA_READY				NO_OS_BIT(7)
#define ADXL313_REG_INT_SRC_ACTIVITY				NO_OS_BIT(4)
#define ADXL313_REG_INT_SRC_INACTIVITY				NO_OS_BIT(3)
#define ADXL313_REG_INT_SRC_WATERMARK				NO_OS_BIT(1)
#define ADXL313_REG_INT_SRC_OVERRUN				NO_OS_BIT(0)

/* ADXL313_REG_DATA_FORMAT definitions */
#define ADXL313_REG_DATA_FORMAT_SELF_TEST			NO_OS_BIT(7)
#define ADXL313_REG_DATA_FORMAT_SPI_3WIRE			NO_OS_BIT(6)
#define ADXL313_REG_DATA_FORMAT_INT_INV				NO_OS_BIT(5)
#define ADXL313_REG_DATA_FORMAT_FULL_RES			NO_OS_BIT(3)
#define ADXL313_REG_DATA_FORMAT_JUSTIFY_BIT			NO_OS_BIT(2)
#define ADXL313_REG_DATA_FORMAT_RANGE				NO_OS_GENMASK(1, 0)

/* ADXL313_REG_FIFO_CTL definitions */
#define ADXL313_REG_FIFO_CTL_MODE_MSK				NO_OS_GENMASK(7, 6)
#define ADXL313_REG_FIFO_CTL_TRIGGER_BIT			NO_OS_BIT(5)
#define ADXL313_REG_FIFO_CTL_SAMPLES_MSK			NO_OS_GENMASK(4, 0)

/* ADXL313_REG_FIFO_STATUS definitions */
#define ADXL313_REG_FIFO_FIFO_TRIGGER_BIT			NO_OS_BIT(7)
#define ADXL313_REG_FIFO_STS_ENTRIES_MSK			NO_OS_GENMASK(5, 0)

/*
 * ADXL312
 * At all g ranges with full bit resolution, sensitivity is given in datasheet as
 * 2.9 mg/LSB = 0.0029000 * 9.80665 = 0.0284392850 m/s^2.
 */
#define ADXL312_ACC_SCALE_FACTOR_MUL_FULL_RES			((int64_t)28439285ULL)

/*
 * ADXL313
 * At all g ranges with full bit resolution, sensitivity is given in datasheet as
 * 1024 LSB/g = 0.0009765625 * 9.80665 = 0.00957680664 m/s^2.
 *
 * For +/- 2g range, a multiplier with value 2 is used.
 * For +/- 2g range, a multiplier with value 4 is used.
 * For +/- 4g range, a multiplier with value 8 is used.
 *
 */
#define ADXL313_ACC_SCALE_FACTOR_MUL_FULL_RES			((int64_t)957680664ULL)
/* Divider used for dividing of multiplier.
 * Multiplication with 100 was used for preserving precision. */
#define ADXL313_ACC_SCALE_FACTOR_MUL_DIVIDER			((int32_t)100)

/*
 * ADXL314
 * At +/-200g with 13-bit resolution, scale factor is given in datasheet as
 * 48.83 mg/LSB = 0.0488300 * 9.80665 = 0.4788587195 m/s^2.
 */
#define ADXL314_ACC_SCALE_FACTOR_MUL				((int64_t)478858719ULL)
#define ADXL313_ACC_SCALE_FACTOR_DIV				((int32_t)1000000000)

/* ADXL312
 * The scale factor of the offset adjustment, in g / 1000 000 (ug) */
#define ADXL312_OFFSET_SCALE_FACTOR				11600

/* ADXL313
 * The scale factor of the offset adjustment, in g / 1000 000 (ug) */
#define ADXL313_OFFSET_SCALE_FACTOR				3900

/* ADXL314
 * The scale factor of the offset adjustment, in g / 1000 000 (ug) */
#define ADXL314_OFFSET_SCALE_FACTOR				195000

/* ADXL312
 * The scale factor of the activity/inactivity thresholds, in g / 1000 000 (ug) */
#define ADXL312_THRESH_SCALE_FACTOR				46400

/* ADXL313
 * The scale factor of the activity/inactivity thresholds, in g / 1000 000 (ug) */
#define ADXL313_THRESH_SCALE_FACTOR				15625

/* ADXL314
 * The scale factor of the activity/inactivity thresholds, in g / 1000 000 (ug) */
#define ADXL314_THRESH_SCALE_FACTOR				784000

/* Offset added to the ODR enum for obtaining corresponding register setting */
#define ADXL313_ODR_OFFSET_VAL					6

/* Number of registers read for all axes */
#define ADXL313_REGS_PER_ENTRY					6

/* Max number of FIFO entries */
#define ADXL313_MAX_FIFO_ENTRIES				32

/* Number of Self-test samples */
#define ADXL313_SELF_TEST_SAMPLES				10

/* Self-test multiplication factor */
#define ADXL312_SELF_TEST_MULT					2900
#define ADXL313_SELF_TEST_MULT					976
#define ADXL314_SELF_TEST_MULT					48830
#define ADXL313_SELF_TEST_DIV					1000

/* Self-test minimum deviation for ADXL312, mg */
#define ADXL312_SELF_TEST_MIN_DEVIATION				300
/* Self-test maximum deviation for ADXL312, mg */
#define ADXL312_SELF_TEST_MAX_DEVIATION				3400

/* Self-test minimum deviation for ADXL313, mg */
#define ADXL313_SELF_TEST_MIN_DEVIATION				300
/* Self-test maximum deviation for ADXL313, mg */
#define ADXL313_SELF_TEST_MAX_DEVIATION				3700

/* Self-test minimum deviation for ADXL314, mg */
#define ADXL314_SELF_TEST_MIN_DEVIATION				100
/* Self-test maximum deviation for ADXL314, mg */
#define ADXL314_SELF_TEST_MAX_DEVIATION				8000

/* Factor used for distinguishing between ADXL312 and ADXL313 ranges */
#define ADXL313_RANGE_FACTOR					4

/* Activity/inactivity detection defines */
/* Enable axis for participating in act/inact detection */
#define ADXL313_X_EN        					NO_OS_BIT(1)
#define ADXL313_Y_EN        					NO_OS_BIT(2)
#define ADXL313_Z_EN        					NO_OS_BIT(3)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum adxl313_type
 * @brief Enum for device type.
 */
enum adxl313_type {
	ID_ADXL312 = 0,
	ID_ADXL313 = 1,
	ID_ADXL314 = 2
};

/**
 * @enum adxl313_comm_type
 * @brief Enum for communication type.
 */
enum adxl313_comm_type {
	ADXL313_SPI_COMM = 0,
	ADXL313_I2C_COMM = 1
};

/**
 * @enum adxl313_op_mode
 * @brief Enum for operating mode.
 */
enum adxl313_op_mode {
	ADXL313_STDBY = 0,
	ADXL313_MEAS = 1
};

/**
 * @enum adxl313_lp_mode
 * @brief Enum for low power operating mode.
 */
enum adxl313_lp_mode {
	ADXL313_LP_MODE_NONE = 0,
	ADXL313_LP_MODE_ON = 1,
	ADXL313_LP_MODE_AUTOSLEEP = 2
};

/**
 * @enum adxl313_wake_up_f
 * @brief Enum for frequency of wake-up events during sleep.
 */
enum adxl313_wake_up_f {
	ADXL313_WU_8_HZ = 0,
	ADXL313_WU_4_HZ = 1,
	ADXL313_WU_2_HZ = 2,
	ADXL313_WU_1_HZ = 3
};

/**
 * @enum adxl313_range
 * @brief Enum for selecting range.
 */
enum adxl313_range {
	ADXL313_0_5G_RANGE = 0,
	ADXL313_1G_RANGE = 1,
	ADXL313_2G_RANGE = 2,
	ADXL313_4G_RANGE = 3,
	ADXL313_1_5G_RANGE = 4,
	ADXL313_3G_RANGE = 5,
	ADXL313_6G_RANGE = 6,
	ADXL313_12G_RANGE = 7,
	ADXL313_200G_RANGE = 8
};

/**
 * @enum adxl313_resolution
 * @brief Enum for specifying resolution.
 */
enum adxl313_resolution {
	ADXL313_10_BIT_RES = 0,
	ADXL313_11_BIT_RES = 1,
	ADXL313_12_BIT_RES = 2,
	ADXL313_13_BIT_RES = 3
};

/**
 * @enum adxl313_odr
 * @brief Enum for selecting output data rate.
 */
enum adxl313_odr {
	ADXL313_ODR_6_25HZ = 0,
	ADXL313_ODR_12_5HZ = 1,
	ADXL313_ODR_25HZ = 2,
	ADXL313_ODR_50HZ = 3,
	ADXL313_ODR_100HZ = 4,
	ADXL313_ODR_200HZ = 5,
	ADXL313_ODR_400HZ = 6,
	ADXL313_ODR_800HZ = 7,
	ADXL313_ODR_1600HZ = 8,
	ADXL313_ODR_3200HZ = 9
};

/**
 * @enum adxl313_axis
 * @brief Enum for accelerometer axis.
 */
enum adxl313_axis {
	ADXL313_X_AXIS = 0,
	ADXL313_Y_AXIS = 1,
	ADXL313_Z_AXIS = 2
};

/**
 * @enum adxl313_fifo_mode
 * @brief Enum for selecting FIFO operating mode.
 */
enum adxl313_fifo_mode {
	ADXL313_BYPAS_MODE = 0,
	ADXL313_FIFO_MODE = 1,
	ADXL313_STREAM_MODE = 2,
	ADXL313_TRIGGERED_MODE = 3
};

/**
 * @enum adxl313_int_pol
 * @brief Enum for selecting interrupt polarity.
 */
enum adxl313_int_pol {
	ADXL313_INT_ACTIVE_HIGH = 0,
	ADXL313_INT_ACTIVE_LOW = 1
};

/**
 * @enum bit_action
 * @brief Enum for selecting Set/Reset bit action.
 */
enum bit_action {
	DISABLE_E = 0,
	ENABLE_E = 1
};

/**
 * @struct adxl313_comm_init_param
 * @brief Structure holding the parameters for ADXL313
 *        communication initialization.
 */
union adxl313_comm_init_param {
	/** I2C Initialization structure. */
	struct no_os_i2c_init_param i2c_init;
	/** SPI Initialization structure. */
	struct no_os_spi_init_param spi_init;
} ;

/**
 * @struct adxl313_frac_repr
 * @brief Data format in which the acceleration values are converted.
 */
struct adxl313_frac_repr {
	int64_t integer;
	int32_t fractional;
};

/**
 * @struct adxl313_init_param
 * @brief Structure holding the parameters for ADXL313 device initialization.
 */
struct adxl313_init_param {
	/** Device Communication initialization structure: either SPI or I2C */
	union adxl313_comm_init_param comm_init;
	/** Device Communication type: ADXL355_SPI_COMM, ADXL355_I2C_COMM */
	enum adxl313_comm_type comm_type;
	/** Device type: ADXL312, 313, or 314 */
	enum adxl313_type dev_type;
};

/**
 * @struct _adxl313_int_reg_flags
 * @brief Structure holding the flags for ADXL313 interrupt sources.
 */
struct _adxl313_int_reg_flags {
	uint8_t OVERRUN    : 1;
	uint8_t WATERMARK  : 1;
	uint8_t reserved   : 1;
	uint8_t INACTIVITY : 1;
	uint8_t ACTIVITY   : 1;
	uint8_t reserved1  : 2;
	uint8_t DATA_READY : 1;
};

/**
 * @union adxl313_int_en_reg_flags
 * @brief Union holding the flags for ADXL313 interrupt enable.
 */
union adxl313_int_en_reg_flags {
	struct _adxl313_int_reg_flags fields;
	uint8_t value;
};

/**
 * @union adxl313_int_map_reg_flags
 * @brief Union holding the flags for ADXL313 interrupt mapping.
 */
union adxl313_int_map_reg_flags {
	struct _adxl313_int_reg_flags fields;
	uint8_t value;
};

/**
 * @union adxl313_int_src_reg_flags
 * @brief Union holding the flags for ADXL313 interrupt source.
 */
union adxl313_int_src_reg_flags {
	struct _adxl313_int_reg_flags fields;
	uint8_t value;
};

/**
 * @struct _adxl313_act_inact_ctl_flags
 * @brief Structure holding the flags corresponding to
 * 				ACT AC/DC and INACT AC/DC Bits.
 */
struct _adxl313_act_inact_ctl_flags {
	uint8_t INACT_Z_EN     : 1;
	uint8_t INACT_Y_EN     : 1;
	uint8_t INACT_X_EN     : 1;
	uint8_t INACT_AC_EN    : 1;
	uint8_t ACT_Z_EN       : 1;
	uint8_t ACT_Y_EN       : 1;
	uint8_t ACT_X_EN       : 1;
	uint8_t ACT_AC_EN      : 1;
};

/**
 * @union adxl313_act_inact_ctl_flags
 * @brief Union holding the flags corresponding to
 * 				ACT AC/DC and INACT AC/DC Bits.
 */
union adxl313_act_inact_ctl_flags {
	struct _adxl313_act_inact_ctl_flags fields;
	uint8_t value;
};

/**
 * @union adxl313_comm_desc
 * @brief Union holding the communication parameters.
 */
union adxl313_comm_desc {
	/** I2C Descriptor */
	struct no_os_i2c_desc *i2c_desc;
	/** SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
};

/**
 * @struct adxl313_dev
 * @brief ADXL313 Device structure.
 */
struct adxl313_dev {
	/** Device communication descriptor */
	union adxl313_comm_desc com_desc;
	/** Device Communication type: ADXL313_SPI_COMM, ADXL313_I2C_COMM */
	enum adxl313_comm_type comm_type;
	/** Device type */
	enum adxl313_type dev_type;
	/** ADXL313 measurement range */
	enum adxl313_range range;
	/** ADXL313 resolution */
	enum adxl313_resolution resolution;
	/** ADXL313 operation mode, STDBY or MEASURE */
	enum adxl313_op_mode op_mode;
	/** ADXL313 LOW POOWER operation mode (NONE, ON, or AUTOSLEEP) */
	enum adxl313_lp_mode lp_mode;
	/** ADXL313 output data rate */
	enum adxl313_odr odr;
	/** User-set offset adjustments  on the X axis in twos complement
	 * format with a scale factor depending on device */
	uint8_t x_offset_raw;
	/** User-set offset adjustments  on the Y axis in twos complement
	 * format with a scale factor depending on device */
	uint8_t y_offset_raw;
	/** User-set offset adjustments  on the Z axis in twos complement
	 * format with a scale factor depending on device */
	uint8_t z_offset_raw;
	/** Scale factor multiplier for data conversion to m/s^2 */
	int64_t scale_factor_mult;
	/** ADXL313 FIFO mode selection */
	enum adxl313_fifo_mode fifo_mode;
	/** Number of FISO samples before FIFO event signalling */
	uint8_t fifo_samples;
	/** Threshold value for detecting activity, raw data */
	uint8_t act_thr;
	/** Threshold value for detecting inactivity, raw data */
	uint8_t inact_thr;
	/** Amount of time for inactivity to be declared */
	uint8_t time_inact;
	/** The contents of the ACT_INACT_CTL register */
	union adxl313_act_inact_ctl_flags act_inact_ctl;
	/** Buffer used for communication with ADXL313 */
	uint8_t comm_buff[24];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Init. the comm. peripheral and check if the ADXL313 is present. */
int adxl313_init(struct adxl313_dev **device,
		 struct adxl313_init_param init_param);

/*! Free the resources allocated by adxl313_init(). */
int adxl313_remove(struct adxl313_dev *dev);

/*! Reset the device via communication. */
int adxl313_software_reset(struct adxl313_dev *dev);

/*! Place the device into the given operation mode. */
int adxl313_set_op_mode(struct adxl313_dev *dev,
			enum adxl313_op_mode op_mode);

/*! Get the current operation mode of the device. */
int adxl313_get_op_mode(struct adxl313_dev *dev,
			enum adxl313_op_mode *op_mode);

/*! Set offset for each axis. */
int adxl313_set_offset(struct adxl313_dev *dev,
		       int32_t offset_ug,
		       enum adxl313_axis axis);

/*! Get offset for each axis. */
int adxl313_get_offset(struct adxl313_dev *dev,
		       int32_t *offset_ug,
		       enum adxl313_axis axis);

/*! Set raw offset for each axis. */
int adxl313_set_raw_offset(struct adxl313_dev *dev,
			   int32_t offset_raw,
			   enum adxl313_axis axis);

/*! Get raw offset for each axis. */
int adxl313_get_raw_offset(struct adxl313_dev *dev,
			   int32_t *offset_raw,
			   enum adxl313_axis axis);

/*! Read the 3-axis raw data from the accelerometer. */
int adxl313_get_raw_xyz(struct adxl313_dev *dev,
			int16_t *x_raw,
			int16_t *y_raw,
			int16_t *z_raw);

/*! Read the raw output data of each axis and convert it to m/s^2. */
int adxl313_get_xyz(struct adxl313_dev *dev,
		    struct adxl313_frac_repr *x_m_s2,
		    struct adxl313_frac_repr *y_m_s2,
		    struct adxl313_frac_repr *z_m_s2);

/*! Read the number of FIFO entries register value. */
int adxl313_get_no_of_fifo_entries(struct adxl313_dev *dev,
				   uint8_t *entries_no);

/*! Set the number of FIFO samples register value. */
int adxl313_set_fifo_samples(struct adxl313_dev *dev,
			     uint8_t samples_no);

/*! Set FIFO mode. */
int adxl313_set_fifo_mode(struct adxl313_dev *dev,
			  enum adxl313_fifo_mode mode);

/*! Read FIFO data and return the raw values. */
int adxl313_get_raw_fifo_data(struct adxl313_dev *dev, uint8_t *entries,
			      int16_t *x_raw, int16_t *y_raw, int16_t *z_raw);

/*! Read converted values from FIFO. */
int adxl313_get_fifo_data(struct adxl313_dev *dev,
			  uint8_t *entries,
			  struct adxl313_frac_repr *x,
			  struct adxl313_frac_repr *y,
			  struct adxl313_frac_repr *z);

/*! Configure the activity threshold register. */
int adxl313_conf_act_thr(struct adxl313_dev *dev, uint32_t act_thr_ug);

/*! Get the value saved in the activity threshold register. */
int adxl313_get_act_thr(struct adxl313_dev *dev, uint32_t *act_thr_ug);

/*! Configure the inactivity threshold register. */
int adxl313_conf_inact_thr(struct adxl313_dev *dev, uint32_t inact_thr_ug);

/*! Get the value in the inactivity threshold register. */
int adxl313_get_inact_thr(struct adxl313_dev *dev, uint32_t *inact_thr_ug);

/*! Configure the time inactivity threshold register. */
int adxl313_conf_time_inact(struct adxl313_dev *dev, uint8_t time_inact_s);

/*! Get the value of the time inactivity threshold register. */
int adxl313_get_time_inact(struct adxl313_dev *dev, uint8_t *time_inact_s);

/*! Configure the ACT AC/DC and INACT AC/DC bits. */
int adxl313_conf_act_inact_ctl(struct adxl313_dev *dev,
			       union adxl313_act_inact_ctl_flags config);

/*! Get the ACT AC/DC and INACT AC/DC bits. */
int adxl313_get_conf_act_inact_ctl(struct adxl313_dev *dev,
				   union adxl313_act_inact_ctl_flags *config);

/*! Enable activity detection. */
int adxl313_set_activity_detection(struct adxl313_dev *dev,
				   uint8_t act_axes,
				   uint8_t act_ac_dc,
				   uint32_t act_thresh_ug,
				   uint8_t int_pin);

/*! Enable inactivity detection. */
int adxl313_set_inactivity_detection(struct adxl313_dev *dev,
				     uint8_t inact_axes,
				     uint8_t inact_ac_dc,
				     uint32_t inact_thresh_ug,
				     uint8_t inact_time,
				     uint8_t int_pin);

/*! Set output data rate (ODR). */
int adxl313_set_odr(struct adxl313_dev *dev, enum adxl313_odr odr);

/*! Get output data rate (ODR). */
int adxl313_get_odr(struct adxl313_dev *dev, enum adxl313_odr *odr);

/*! Disable I2C. */
int adxl313_disable_i2c(struct adxl313_dev *dev);

/*! Enable I2C. */
int adxl313_enable_i2c(struct adxl313_dev *dev);

/*! Activate/Deactivate LOW_POWER. */
int adxl313_set_low_power_mode(struct adxl313_dev *dev,
			       enum bit_action enable);

/*! Activate/Deactivate AUTOSLEEP. */
int adxl313_set_autosleep_mode(struct adxl313_dev *dev, enum bit_action enable,
			       uint8_t inact_thr, uint8_t time_inact_s);

/*! Enable Link Mode for activity inactivity functions. */
int adxl313_link_mode_enable(struct adxl313_dev *dev);

/*! Disable Link Mode for activity inactivity functions. */
int adxl313_link_mode_disable(struct adxl313_dev *dev);

/*! Put device in Sleep mode. */
int adxl313_sleep(struct adxl313_dev *dev, enum adxl313_wake_up_f wake_up_f_hz);

/*! Exit Sleep mode. */
int adxl313_exit_sleep(struct adxl313_dev *dev);

/*! Configure interrupt enable control. */
int adxl313_conf_int_enable(struct adxl313_dev *dev,
			    union adxl313_int_en_reg_flags en_ctl);

/*! Configure interrupt enable control for activity. */
int adxl313_activity_int_enable(struct adxl313_dev *dev);

/*! Configure interrupt disable control for activity. */
int adxl313_activity_int_disable(struct adxl313_dev *dev);

/*! Configure interrupt enable control for inactivity. */
int adxl313_inactivity_int_enable(struct adxl313_dev *dev);

/*! Configure interrupt disable control for inactivity. */
int adxl313_inactivity_int_disable(struct adxl313_dev *dev);

/*! Configure interrupt mapping control. */
int adxl313_conf_int_map(struct adxl313_dev *dev,
			 union adxl313_int_map_reg_flags int_map);

/*! Set pin for DATA_READY interrupt. */
int adxl313_data_ready_int_map(struct adxl313_dev *dev, uint8_t int_pin);

/*! Set pin for activity interrupt. */
int adxl313_activity_int_map(struct adxl313_dev *dev, uint8_t int_pin);

/*! Set pin for inactivity interrupt. */
int adxl313_inactivity_int_map(struct adxl313_dev *dev, uint8_t int_pin);

/*! Set pin for watermark interrupt. */
int adxl313_watermark_int_map(struct adxl313_dev *dev, uint8_t int_pin);

/*! Set pin for overrun interrupt. */
int adxl313_overrun_int_map(struct adxl313_dev *dev, uint8_t int_pin);

/*! Read the INT_SOURCE register value. */
int adxl313_get_int_source_reg(struct adxl313_dev *dev,
			       union adxl313_int_src_reg_flags *int_status_flags);

/*! Configure the interrupt polarity. */
int adxl313_set_int_pol(struct adxl313_dev *dev, enum adxl313_int_pol int_pol);

/*! Enable full resolution for the ADXL312/ADXL313 device. */
int adxl313_enable_full_res(struct adxl313_dev *dev);

/*! Disable full resolution for the ADXL312/ADXL313 device. */
int adxl313_disable_full_res(struct adxl313_dev *dev);

/*! Get the ADXL312/ADXL313 FULL_RES setting. */
int adxl313_get_full_res_setting(struct adxl313_dev *dev, uint8_t *full_res);

/*! Set the ADXL312/ADXL313 device range. */
int adxl313_set_range(struct adxl313_dev *dev, enum adxl313_range range);

/*! Get the ADXL312/ADXL313 device range. */
int adxl313_get_range(struct adxl313_dev *dev, enum adxl313_range *range);

/* Perform device self-test. */
int adxl313_self_test(struct adxl313_dev *dev);

#endif /* __ADXL313_H__ */
