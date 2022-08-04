/***************************************************************************//**
 *   @file   ADXL367.h
 *   @brief  Header file of ADXL367 Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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

#ifndef __ADXL367_H__
#define __ADXL367_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_i2c.h"

/******************************************************************************/
/********************************* ADXL367 ************************************/
/******************************************************************************/

/* ADXL367 communication commands */
#define ADXL367_WRITE_REG               0x0A
#define ADXL367_READ_REG                0x0B
#define ADXL367_READ_FIFO               0x0D
#define ADXL367_I2C_READ		0x01
#define ADXL367_I2C_WRITE		0x00

/* Registers */
#define ADXL367_REG_DEVID_AD            0x00
#define ADXL367_REG_DEVID_MST           0x01
#define ADXL367_REG_PARTID              0x02
#define ADXL367_REG_REVID               0x03
#define ADXL367_REG_SERIAL_NUMBER_3	0x04
#define ADXL367_REG_SERIAL_NUMBER_2	0x05
#define ADXL367_REG_SERIAL_NUMBER_1	0x06
#define ADXL367_REG_SERIAL_NUMBER_0	0x07
#define ADXL367_REG_XDATA               0x08
#define ADXL367_REG_YDATA               0x09
#define ADXL367_REG_ZDATA               0x0A
#define ADXL367_REG_STATUS              0x0B
#define ADXL367_REG_FIFO_ENTRIES_L      0x0C
#define ADXL367_REG_FIFO_ENTRIES_H      0x0D
#define ADXL367_REG_XDATA_H             0x0E
#define ADXL367_REG_XDATA_L             0x0F
#define ADXL367_REG_YDATA_H             0x10
#define ADXL367_REG_YDATA_L             0x11
#define ADXL367_REG_ZDATA_H             0x12
#define ADXL367_REG_ZDATA_L             0x13
#define ADXL367_REG_TEMP_H              0x14
#define ADXL367_REG_TEMP_L              0x15
#define ADXL367_REG_EX_ADC_H		0x16
#define ADXL367_REG_EX_ADC_L		0x17
#define ADXL367_REG_I2C_FIFO_DATA	0x18
#define ADXL367_REG_SOFT_RESET          0x1F
#define ADXL367_REG_THRESH_ACT_H        0x20
#define ADXL367_REG_THRESH_ACT_L        0x21
#define ADXL367_REG_TIME_ACT            0x22
#define ADXL367_REG_THRESH_INACT_H      0x23
#define ADXL367_REG_THRESH_INACT_L      0x24
#define ADXL367_REG_TIME_INACT_H        0x25
#define ADXL367_REG_TIME_INACT_L        0x26
#define ADXL367_REG_ACT_INACT_CTL       0x27
#define ADXL367_REG_FIFO_CONTROL	0x28
#define ADXL367_REG_FIFO_SAMPLES        0x29
#define ADXL367_REG_INTMAP1_LWR		0x2A
#define ADXL367_REG_INTMAP2_LWR		0x2B
#define ADXL367_REG_FILTER_CTL          0x2C
#define ADXL367_REG_POWER_CTL           0x2D
#define ADXL367_REG_SELF_TEST           0x2E
#define ADXL367_REG_TAP_THRESH          0x2F
#define ADXL367_REG_TAP_DUR		0x30
#define ADXL367_REG_TAP_LATENT		0x31
#define ADXL367_REG_TAP_WINDOW		0x32
#define ADXL367_REG_X_OFFSET		0x33
#define ADXL367_REG_Y_OFFSET		0x34
#define ADXL367_REG_Z_OFFSET		0x35
#define ADXL367_REG_X_SENS		0x36
#define ADXL367_REG_Y_SENS		0x37
#define ADXL367_REG_Z_SENS		0x38
#define ADXL367_REG_TIMER_CTL           0x39
#define ADXL367_REG_INTMAP1_UPPER       0x3A
#define ADXL367_REG_INTMAP2_UPPER	0x3B
#define ADXL367_REG_ADC_CTL		0x3C
#define ADXL367_REG_TEMP_CTL		0x3D
#define ADXL367_REG_TEMP_ADC_OV_TH_H	0x3E
#define ADXL367_REG_TEMP_ADC_OV_TH_L    0x3F
#define ADXL367_REG_TEMP_ADC_UN_TH_H    0x40
#define ADXL367_REG_TEMP_ADC_UN_TH_L	0x41
#define ADXL367_REG_TEMP_ADC_TIMER      0x42
#define ADXL367_REG_AXIS_MASK		0x43
#define ADXL367_REG_STATUS_COPY         0x44
#define ADXL367_REG_STATUS_2            0x45

/* ADXL367_REG_STATUS definitions */
#define ADXL367_STATUS_ERR_USER_REGS    NO_OS_BIT(7)
#define ADXL367_STATUS_AWAKE            NO_OS_BIT(6)
#define ADXL367_STATUS_INACT            NO_OS_BIT(5)
#define ADXL367_STATUS_ACT              NO_OS_BIT(4)
#define ADXL367_STATUS_FIFO_OVERRUN     NO_OS_BIT(3)
#define ADXL367_STATUS_FIFO_WATERMARK   NO_OS_BIT(2)
#define ADXL367_STATUS_FIFO_RDY         NO_OS_BIT(1)
#define ADXL367_STATUS_DATA_RDY         NO_OS_BIT(0)

/* ADXL367_REG_THRESH_H mask */
#define ADXL367_THRESH_H		0x7F

/* ADXL367_REG_THRESH_L mask */
#define ADXL367_THRESH_L		0xFC

/* ADXL367_REG_ACT_INACT_CTL definitions */
#define ADXL367_ACT_INACT_CTL_LINKLOOP_MSK	NO_OS_GENMASK(5, 4)
#define ADXL367_ACT_INACT_CTL_INACT_EN_MSK	NO_OS_GENMASK(3, 2)
#define ADXL367_ACT_INACT_CTL_ACT_EN_MSK	NO_OS_GENMASK(1, 0)

/* ADXL367_ACT_INACT_CTL_INACT_EN(x) options */
#define ADXL367_NO_INACTIVITY_DETECTION_ENABLED		0x0
#define ADXL367_INACTIVITY_ENABLE			0x1
#define ADXL367_NO_INACTIVITY_DETECTION_ENABLED_2	0x2
#define ADXL367_REFERENCED_INACTIVITY_ENABLE		0x3

/* ADXL367_ACT_INACT_CTL_ACT_EN(x) options */
#define ADXL367_NO_ACTIVITY_DETECTION		0x0
#define ADXL367_ACTIVITY_ENABLE			0x1
#define ADXL367_NO_ACTIVITY_DETECTION_2		0x2
#define ADXL367_REFERENCED_ACTIVITY_ENABLE	0x3

/* ADXL367_REG_FIFO_CONTROL */
#define ADXL367_FIFO_CONTROL_FIFO_CHANNEL_MSK	NO_OS_GENMASK(6, 3)
#define ADXL367_FIFO_CONTROL_FIFO_SAMPLES	NO_OS_BIT(2)
#define ADXL367_FIFO_CONTROL_FIFO_MODE_MSK	NO_OS_GENMASK(1, 0)

/* ADXL367_FIFO_CONTROL_FIFO_CHANNEL(x) options */
#define ADXL367_ALL_AXIS		0x0
#define ADXL367_X_AXIS			0x1
#define ADXL367_Y_AXIS			0x2
#define ADXL367_X_AXIS_2		0x3
#define ADXL367_ALL_AXIS_TEMP		0x4
#define ADXL367_X_AXIS_TEMP		0x5
#define ADXL367_Y_AXIS_TEMP		0x6
#define ADXL367_Z_AXIS_TEMP		0x7
#define ADXL367_ALL_AXIS_EXT_ADC	0x8
#define ADXL367_X_AXIS_EXT_ADC		0x9
#define ADXL367_Y_AXIS_EXT_ADC		0xA
#define ADXL367_Z_AXIS_EXT_ADC		0xB

/* ADXL367_FIFO_CONTROL_FIFO_MODE(x) options */
#define ADXL367_FIFO_DISABLE            0
#define ADXL367_FIFO_OLDEST_SAVED       1
#define ADXL367_FIFO_STREAM             2
#define ADXL367_FIFO_TRIGGERED          3

/* ADXL367_REG_INTMAP1_LOWER */
#define ADXL367_INTMAP1_INT_LOW_INT1		NO_OS_BIT(7)
#define ADXL367_INTMAP1_AWAKE_INT1		NO_OS_BIT(6)
#define ADXL367_INTMAP1_INACT_INT1		NO_OS_BIT(5)
#define ADXL367_INTMAP1_ACT_INT1		NO_OS_BIT(4)
#define ADXL367_INTMAP1_FIFO_OVERRUN_INT1	NO_OS_BIT(3)
#define ADXL367_INTMAP1_FIFO_WATERMARK_INT1	NO_OS_BIT(2)
#define ADXL367_INTMAP1_FIFO_RDY_INT1		NO_OS_BIT(1)
#define ADXL367_INTMAP1_DATA_RDY_INT1		NO_OS_BIT(0)

/* ADXL367_REG_INTMAP2_LOWER definitions */
#define ADXL367_INTMAP2_INT_LOW_INT2		NO_OS_BIT(7)
#define ADXL367_INTMAP2_AWAKE_INT2		NO_OS_BIT(6)
#define ADXL367_INTMAP2_INACT_INT2		NO_OS_BIT(5)
#define ADXL367_INTMAP2_ACT_INT2		NO_OS_BIT(4)
#define ADXL367_INTMAP2_FIFO_OVERRUN_INT2	NO_OS_BIT(3)
#define ADXL367_INTMAP2_FIFO_WATERMARK_INT2	NO_OS_BIT(2)
#define ADXL367_INTMAP2_FIFO_RDY_INT2		NO_OS_BIT(1)
#define ADXL367_INTMAP2_DATA_RDY_INT2		NO_OS_BIT(0)

/* ADXL367_REG_FILTER_CTL definitions */
#define ADXL367_FILTER_CTL_RANGE_MSK	NO_OS_GENMASK(7, 6)
#define ADXL367_FILTER_I2C_HS		NO_OS_BIT(5)
#define ADXL367_FILTER_CTL_RES		NO_OS_BIT(4)
#define ADXL367_FILTER_CTL_EXT_SAMPLE   NO_OS_BIT(3)
#define ADXL367_FILTER_CTL_ODR_MSK		NO_OS_GENMASK(2, 0)

/* ADXL367_FILTER_CTL_RANGE(x) options */
#define ADXL367_RANGE_2G                0 /* +/-2 g */
#define ADXL367_RANGE_4G                1 /* +/-4 g */
#define ADXL367_RANGE_8G                2 /* +/-8 g */

/* ADXL367_REG_POWER_CTL definitions */
#define ADXL367_POWER_CTL_RES           NO_OS_BIT(7)
#define ADXL367_POWER_CTL_EXT_CLK       NO_OS_BIT(6)
#define ADXL367_POWER_CTL_LOW_NOISE_MSK	NO_OS_GENMASK(5, 4)
#define ADXL367_POWER_CTL_WAKEUP        NO_OS_BIT(3)
#define ADXL367_POWER_CTL_AUTOSLEEP     NO_OS_BIT(2)
#define ADXL367_POWER_CTL_MEASURE_MSK	NO_OS_GENMASK(1, 0)

/* ADXL367_POWER_CTL_NOISE(x) options */
#define ADXL367_NOISE_MODE_NORMAL       0
#define ADXL367_NOISE_MODE_LOW          1
#define ADXL367_NOISE_MODE_ULTRALOW     2

/* ADXL367_REG_SELF_TEST */
#define ADXL367_SELF_TEST_ST_FORCE      NO_OS_BIT(1)
#define ADXL367_SELF_TEST_ST            NO_OS_BIT(0)

/* XYZ_AXIS_OFFSET MASK */
#define ADXL367_XYZ_AXIS_OFFSET_MASK	0x1F

/* ADXL367_REG_INTMAPX_UPPER MASK */
#define ADXL367_INTMAPX_UPPER_MASK	0xDF

/* ADXL367_REG_ADC_CTL definitions. */
#define ADXL367_FIFO_8_12BIT_MSK		NO_OS_GENMASK(7,6)
#define ADXL367_ADC_INACT_EN			NO_OS_BIT(3)
#define ADXL367_ADC_ACT_EN			NO_OS_BIT(1)
#define ADXL367_ADC_EN				NO_OS_BIT(0)

/* ADXL367_REG_TEMP_CTL definitions. */
#define ADXL367_TEMP_INACT_EN			NO_OS_BIT(3)
#define ADXL367_TEMP_ACT_EN			NO_OS_BIT(1)
#define ADXL367_TEMP_EN				NO_OS_BIT(0)

/* ADXL367 device information */
#define ADXL367_DEVICE_AD               0xAD
#define ADXL367_DEVICE_MST              0x1D
#define ADXL367_PART_ID                 0xF7

/* ADXL367 Reset settings */
#define ADXL367_RESET_KEY               0x52

/* Channel ID for FIFO read */
#define ADXL367_FIFO_X_ID		0x00
#define ADXL367_FIFO_Y_ID		0x01
#define ADXL367_FIFO_Z_ID		0x02
#define ADXL367_FIFO_TEMP_ADC_ID	0x03

#define ADXL367_ABSOLUTE		0x00
#define ADXL367_REFERENCED 		0x01

/*
 * At +/- 2g with 14-bit resolution, scale is given in datasheet as
 * 250ug/LSB = 0.0002500 * 9.80665 = 0.0024516625 m/s^2.
 * For +/- 4g range a multiplier with value 2 is used.
 * For +/-8g range, a multiplier with value 4 is used.
 */
#define ADXL367_ACC_SCALE_FACTOR_MUL  245166ULL
#define ADXL367_ACC_SCALE_FACTOR_DIV  1000000000

/*
 * At 25C, raw value is equal to 165 LSB. Raw value varies with 54LSB/C.
 * Offset = 25 * ADXL367_TEMP_PER_C - ADXL367_TEMP_25C = 1185.
 * Temp = (RAW + OFFSET) * SCALE
 *  */
#define ADXL367_TEMP_OFFSET		1185
#define ADXL367_TEMP_25C		165
#define ADXL367_TEMP_SCALE		18518518
#define ADXL367_TEMP_SCALE_DIV		1000000000

/* Min change = 90mg. Sensitivity = 4LSB / mg */
#define ADXL367_SELF_TEST_MIN	90 * 100 / 25
/* Max change = 270mg. Sensitivity = 4LSB / mg */
#define ADXL367_SELF_TEST_MAX	270 * 100 / 25

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum adxl367_comm_type
 * @brief Enum for communication type.
 */
enum adxl367_comm_type {
	ADXL367_SPI_COMM,
	ADXL367_I2C_COMM
};

/**
 * @enum adxl367_op_mode
 * @brief Enum for operating mode.
 */
enum adxl367_op_mode {
	ADXL367_OP_STANDBY = 0,
	ADXL367_OP_MEASURE = 2
};

/**
 * @enum adxl367_range
 * @brief Enum for selecting range.
 */
enum adxl367_range {
	ADXL367_2G_RANGE,
	ADXL367_4G_RANGE,
	ADXL367_8G_RANGE,
};

/**
 * @enum adxl367_odr
 * @brief Enum for selecting output data rate.
 */
enum adxl367_odr {
	ADXL367_ODR_12P5HZ,
	ADXL367_ODR_25HZ,
	ADXL367_ODR_50HZ,
	ADXL367_ODR_100HZ,
	ADXL367_ODR_200HZ,
	ADXL367_ODR_400HZ,
};

/**
 * @enum adxl367_fifo_mode
 * @brief Enum for selecting FIFO operating mode.
 */
enum adxl367_fifo_mode {
	ADXL367_FIFO_DISABLED,
	ADXL367_OLDEST_SAVED,
	ADXL367_STREAM_MODE,
	ADXL367_TRIGGERED_MODE
};

/**
 * @enum adxl367_fifo_format
 * @brief Enum for selecting FIFO storage mode.
 */
enum adxl367_fifo_format {
	/* All axis. Default mode. */
	ADXL367_FIFO_FORMAT_XYZ,
	/* X axis. */
	ADXL367_FIFO_FORMAT_X,
	/* Y axis. */
	ADXL367_FIFO_FORMAT_Y,
	/* Z axis. */
	ADXL367_FIFO_FORMAT_Z,
	/* All axis + temperature. */
	ADXL367_FIFO_FORMAT_XYZT,
	/* X axis + temperature. */
	ADXL367_FIFO_FORMAT_XT,
	/* Y axis + temperature. */
	ADXL367_FIFO_FORMAT_YT,
	/* Z axis + temperature. */
	ADXL367_FIFO_FORMAT_ZT,
	/* All axis + ADC. */
	ADXL367_FIFO_FORMAT_XYZA,
	/* X axis + ADC. */
	ADXL367_FIFO_FORMAT_XA,
	/* Y axis + ADC. */
	ADXL367_FIFO_FORMAT_YA,
	/* Z axis + ADC. */
	ADXL367_FIFO_FORMAT_ZA
};

/**
 * @enum adxl367_fifo_read_mode
 * @brief Enum for selecting FIFO reading mode.
 */
enum adxl367_fifo_read_mode {
	/* Upper 12 bits plus channel ID.*/
	ADXL367_12B_CHID,
	/* Upper 8 bits, no channel ID. */
	ADXL367_8B,
	/* Upper 12 bits, no channel ID. */
	ADXL367_12B,
	/* 14 bits plus channel ID. Default mode. */
	ADXL367_14B_CHID
};

/**
 * @struct adxl367_int_map
 * @brief Contains bit options from INT registers.
 */
struct adxl367_int_map {
	uint8_t err_fuse 	: 1;
	uint8_t err_user_regs 	: 1;
	uint8_t kpalv_timer	: 1;
	uint8_t temp_adc_hi 	: 1;
	uint8_t temp_adc_low	: 1;
	uint8_t tap_two		: 1;
	uint8_t tap_one		: 1;
	uint8_t int_low 	: 1;
	uint8_t awake 		: 1;
	uint8_t inact		: 1;
	uint8_t act		: 1;
	uint8_t fifo_overrun 	: 1;
	uint8_t fifo_watermark	: 1;
	uint8_t fifo_ready	: 1;
	uint8_t data_ready	: 1;
};

/**
 * @struct adxl367_fractional_val
 * @brief Data format in which the values are converted.
 */
struct adxl367_fractional_val {
	int64_t integer;
	int32_t fractional;
};

/**
 * @struct adxl367_dev
 * @brief ADXL367 Device structure.
 */
struct adxl367_dev {
	/** Communication type - I2C or SPI. */
	enum adxl367_comm_type		comm_type;
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** I2C Descriptor */
	struct no_os_i2c_desc  		*i2c_desc;
	/** Depending on ASEL pin, can be 0x53 or 0x1D. Only for I2C Comm. */
	uint8_t i2c_slave_address;
	/** Measurement Range: */
	enum adxl367_range		range;
	enum adxl367_op_mode 		op_mode;
	enum adxl367_odr 		odr;
	enum adxl367_fifo_mode		fifo_mode;
	enum adxl367_fifo_format 	fifo_format;
	enum adxl367_fifo_read_mode 	fifo_read_mode;
	/** FIFO Buffer 513 * 2 + 1 cmd byte */
	uint8_t 			fifo_buffer[1027];
	uint16_t 			x_offset;
	uint16_t 			y_offset;
	uint16_t 			z_offset;
};

/**
 * @struct adxl367_init_param
 * @brief Structure holding the parameters for ADXL367 device initialization.
 */
struct adxl367_init_param {
	/** Communication type - I2C or SPI. */
	enum adxl367_comm_type 		comm_type;
	/** SPI Initialization structure. */
	struct no_os_spi_init_param	spi_init;
	/** I2C Initialization structure. */
	struct no_os_i2c_init_param    	i2c_init;
	/** Depending on ASEL pin, can be 0x53 or 0x1D. Only for I2C Comm. */
	uint8_t 			i2c_slave_address;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initializes the device. */
int adxl367_init(struct adxl367_dev **device,
		 struct adxl367_init_param init_param);

/* Free the resources allocated by adxl367_init(). */
int adxl367_remove(struct adxl367_dev *dev);

/* Performs device self-test. */
int adxl367_self_test(struct adxl367_dev *dev);

/* Writes data into a register. */
int adxl367_set_register_value(struct adxl367_dev *dev,
			       uint8_t register_value,
			       uint8_t register_address);

/* Performs a burst read of a specified number of registers. */
int adxl367_get_register_value(struct adxl367_dev *dev,
			       uint8_t *read_data,
			       uint8_t  register_address,
			       uint8_t  bytes_number);

/* Performs a masked write to a register. */
int adxl367_reg_write_msk(struct adxl367_dev *dev,
			  uint8_t reg_addr,
			  uint8_t data,
			  uint8_t mask);

/* Resets the device via comm. */
int adxl367_software_reset(struct adxl367_dev *dev);

/* Places the device into standby/measure mode. */
int adxl367_set_power_mode(struct adxl367_dev *dev,
			   enum adxl367_op_mode mode);

/* Selects the measurement range. */
int adxl367_set_range(struct adxl367_dev *dev,
		      enum adxl367_range range);

/* Selects the Output Data Rate of the device. */
int adxl367_set_output_rate(struct adxl367_dev *dev,
			    enum adxl367_odr odr);

/* Sets user defined offset for each axis. */
int adxl367_set_offset(struct adxl367_dev *dev, uint16_t x_offset,
		       uint16_t y_offset, uint16_t z_offset);

/* Reads the 3-axis raw data from the accelerometer. */
int adxl367_get_raw_xyz(struct adxl367_dev *dev,
			int16_t* x,
			int16_t* y,
			int16_t* z);

/* Reads the 3-axis raw data from the accelerometer and converts it to g. */
int adxl367_get_g_xyz(struct adxl367_dev *dev,
		      struct adxl367_fractional_val* x,
		      struct adxl367_fractional_val* y,
		      struct adxl367_fractional_val* z);

/* Enables temperature reading. */
int adxl367_temp_read_en(struct adxl367_dev *dev, bool enable);

/* Enables adc reading and disables temperature reading. */
int adxl367_adc_read_en(struct adxl367_dev *dev, bool enable);

/* Reads the raw temperature of the device. */
int adxl367_read_raw_temp(struct adxl367_dev *dev, int16_t *raw_temp);

/* Reads the temperature of the device. */
int adxl367_read_temperature(struct adxl367_dev *dev,
			     struct adxl367_fractional_val *temp);

/* Reads ADC data. */
int adxl367_read_adc(struct adxl367_dev *dev, int16_t *data);

/* Reads the number of FIFO entries register value. */
int adxl367_get_nb_of_fifo_entries(struct adxl367_dev *dev,
				   uint16_t *entr_nb);

/* Sets the number of FIFO samples register value. */
int adxl367_set_fifo_sample_sets_nb(struct adxl367_dev *dev,
				    uint16_t sets_nb);

/* Sets FIFO mode. */
int adxl367_set_fifo_mode(struct adxl367_dev *dev,
			  enum adxl367_fifo_mode mode);

/* Sets FIFO read mode. */
int adxl367_set_fifo_read_mode(struct adxl367_dev *dev,
			       enum adxl367_fifo_read_mode read_mode);

/* Sets FIFO data storage format. */
int adxl367_set_fifo_format(struct adxl367_dev *dev,
			    enum adxl367_fifo_format format);

/* Configures FIFO feature. */
int adxl367_fifo_setup(struct adxl367_dev *dev,
		       enum adxl367_fifo_mode mode,
		       enum adxl367_fifo_format format,
		       uint8_t sets_nb);

/* Reads raw values from FIFO. */
int adxl367_read_raw_fifo(struct adxl367_dev *dev, int16_t *x, int16_t *y,
			  int16_t *z, int16_t *temp_adc, uint16_t *entries);

/* Reads converted values from FIFO. */
int adxl367_read_converted_fifo(struct adxl367_dev *dev,
				struct adxl367_fractional_val *x, struct adxl367_fractional_val *y,
				struct adxl367_fractional_val *z, struct adxl367_fractional_val *temp_adc,
				uint16_t *entries);

/* Enables specified events to interrupt pin. */
int adxl367_int_map(struct adxl367_dev *dev, struct adxl367_int_map *map,
		    uint8_t pin);

/* Configures activity detection. */
int adxl367_setup_activity_detection(struct adxl367_dev *dev,
				     uint8_t  ref_or_abs,
				     uint16_t threshold,
				     uint8_t  time);

/* Configures inactivity detection. */
int adxl367_setup_inactivity_detection(struct adxl367_dev *dev,
				       uint8_t  ref_or_abs,
				       uint16_t threshold,
				       uint16_t  time);

#endif /* __ADXL367_H__ */
