/***************************************************************************//**
 *   @file   adxl355_capi.h
 *   @brief  Header file of ADXL355 CAPI Driver.
 *   @author Claude Code (noreply@anthropic.com)
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
#ifndef __ADXL355_CAPI_H__
#define __ADXL355_CAPI_H__

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "capi/capi_spi.h"

/* SPI commands */
#define ADXL355_SPI_READ          0x01
#define ADXL355_SPI_WRITE         0x00
#define ADXL355_ADDR(x)			  ((x) & 0xFF)
#define GET_ADXL355_TRANSF_LEN(x) (((x) >>  8) & 0x0000FF)
#define SET_ADXL355_TRANSF_LEN(x) (((x) <<  8) & 0x00FF00)
#define GET_ADXL355_RESET_VAL(x)  (((x) >> 16) & 0x0000FF)
#define SET_ADXL355_RESET_VAL(x)  (((x) << 16) & 0xFF0000)

/* ADXL355 Register Map */
#define ADXL355_DEVID_AD     (ADXL355_ADDR(0x00) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0xAD))
#define ADXL355_DEVID_MST 	 (ADXL355_ADDR(0x01) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x1D))
#define ADXL355_PARTID       (ADXL355_ADDR(0x02) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0xED))
#define ADXL359_PARTID       (ADXL355_ADDR(0x02) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0xE9))
#define ADXL355_REVID 		 (ADXL355_ADDR(0x03) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x01))
#define ADXL355_STATUS       (ADXL355_ADDR(0x04) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_FIFO_ENTRIES (ADXL355_ADDR(0x05) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_TEMP         (ADXL355_ADDR(0x06) | SET_ADXL355_TRANSF_LEN(2))
#define ADXL355_XDATA        (ADXL355_ADDR(0x08) | SET_ADXL355_TRANSF_LEN(3))
#define ADXL355_YDATA        (ADXL355_ADDR(0x0B) | SET_ADXL355_TRANSF_LEN(3))
#define ADXL355_ZDATA        (ADXL355_ADDR(0x0E) | SET_ADXL355_TRANSF_LEN(3))
#define ADXL355_FIFO_DATA    (ADXL355_ADDR(0x11) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_OFFSET_X     (ADXL355_ADDR(0x1E) | SET_ADXL355_TRANSF_LEN(2))
#define ADXL355_OFFSET_Y     (ADXL355_ADDR(0x20) | SET_ADXL355_TRANSF_LEN(2))
#define ADXL355_OFFSET_Z     (ADXL355_ADDR(0x22) | SET_ADXL355_TRANSF_LEN(2))
#define ADXL355_ACT_EN       (ADXL355_ADDR(0x24) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_ACT_THRESH   (ADXL355_ADDR(0x25) | SET_ADXL355_TRANSF_LEN(2))
#define ADXL355_ACT_CNT      (ADXL355_ADDR(0x27) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x01))
#define ADXL355_FILTER       (ADXL355_ADDR(0x28) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_FIFO_SAMPLES (ADXL355_ADDR(0x29) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x60))
#define ADXL355_INT_MAP      (ADXL355_ADDR(0x2A) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_SYNC         (ADXL355_ADDR(0x2B) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_RANGE        (ADXL355_ADDR(0x2C) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x81))
#define ADXL355_POWER_CTL    (ADXL355_ADDR(0x2D) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x01))
#define ADXL355_SELF_TEST    (ADXL355_ADDR(0x2E) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))
#define ADXL355_RESET        (ADXL355_ADDR(0x2F) | SET_ADXL355_TRANSF_LEN(1) | SET_ADXL355_RESET_VAL(0x00))

#define ADXL355_SHADOW_REGISTER_BASE_ADDR (ADXL355_ADDR(0x50) | SET_ADXL355_TRANSF_LEN(5))
#define ADXL355_MAX_FIFO_SAMPLES_VAL  0x60
#define ADXL355_SELF_TEST_TRIGGER_VAL 0x03
#define ADXL355_RESET_CODE            0x52

/*
 * At +/- 2g with 20-bit resolution, scale is given in datasheet as
 * 3.9ug/LSB = 0.0000039 * 9.80665 = 0.00003824593 m/s^2.
 * For +/- 4g range a multiplier with value 2 is used.
 * For +/-8g range, a multiplier with value 4 is used.
 */
#define ADXL355_ACC_SCALE_FACTOR_MUL  (int64_t)     38245
#define ADXL355_ACC_SCALE_FACTOR_DIV  (int32_t)1000000000

/*
 * At +/- 10g with 20-bit resolution, scale is given in datasheet as
 * 19.5ug/LSB = 0.0000195 * 9.80665 = 0.00019122967 m/s^2.
 * For +/- 4g range a multiplier with value 2 is used.
 * For +/-8g range, a multiplier with value 4 is used.
 */
#define ADXL359_ACC_SCALE_FACTOR_MUL  (int64_t)    191229
#define ADXL359_ACC_SCALE_FACTOR_DIV  (int32_t)1000000000

/*
 * The datasheet defines an intercept of 1885 LSB at 25 degC
 * and a slope of -9.05 LSB/C. The following formula can be used to find the
 * temperature:
 * Temp = ((RAW - 1885)/(-9.05)) + 25 but this doesn't follow the format of
 * the IIO which is Temp = (RAW + OFFSET) * SCALE. Hence using some rearranging
 * we get the scale as -0.110497238 and offset as -2111.25
 */
/* OFFSET = ADXL355_TEMP_OFFSET / ADXL355_TEMP_OFFSET_DIV */
#define ADXL355_TEMP_OFFSET     -211125
#define ADXL355_TEMP_OFFSET_DIV     100

/* SCALE = ADXL355_TEMP_SCALE_FACTOR / ADXL355_TEMP_SCALE_FACTOR_DIV
   We want to return the temperature in millidegrees Celsius so instead of
   0.110497238, we are going to use 110.497238 value for scale
 */
#define ADXL355_TEMP_SCALE_FACTOR     -110497238
#define ADXL355_TEMP_SCALE_FACTOR_DIV    1000000

/*
 * The datasheet defines an intercept of 1852 LSB at 25 degC
 * and a slope of -9.05 LSB/C. The following formula can be used to find the
 * temperature:
 * Temp = ((RAW - 1852)/(-9.05)) + 25 but this doesn't follow the format of
 * the IIO which is Temp = (RAW + OFFSET) * SCALE. Hence using some rearranging
 * we get the scale as -0.110497238 and offset as -2078.25
 */
/* OFFSET = ADXL359_TEMP_OFFSET / ADXL359_TEMP_OFFSET_DIV */
#define ADXL359_TEMP_OFFSET     -207825
#define ADXL359_TEMP_OFFSET_DIV     100

/* SCALE = ADXL359_TEMP_SCALE_FACTOR / ADXL359_TEMP_SCALE_FACTOR_DIV
   We want to return the temperature in millidegrees Celsius so instead of
   0.110497238, we are going to use 110.497238 value for scale
 */
#define ADXL359_TEMP_SCALE_FACTOR     -110497238
#define ADXL359_TEMP_SCALE_FACTOR_DIV    1000000

#define ADXL355_NEG_ACC_MSK        NO_OS_GENMASK(31, 20)
#define ADXL355_RANGE_FIELD_MSK    NO_OS_GENMASK( 1,  0)
#define ADXL355_ODR_LPF_FIELD_MSK  NO_OS_GENMASK( 3,  0)
#define ADXL355_HPF_FIELD_MSK      NO_OS_GENMASK( 6,  4)
#define ADXL355_INT_POL_FIELD_MSK  NO_OS_BIT(6)

enum adxl355_capi_type {
	ID_ADXL355_CAPI,
	ID_ADXL357_CAPI,
	ID_ADXL359_CAPI,
};

enum adxl355_capi_op_mode {
	ADXL355_CAPI_MEAS_TEMP_ON_DRDY_ON = 0,
	ADXL355_CAPI_STDBY_TEMP_ON_DRDY_ON = 1,
	ADXL355_CAPI_MEAS_TEMP_OFF_DRDY_ON = 2,
	ADXL355_CAPI_STDBY_TEMP_OFF_DRDY_ON = 3,
	ADXL355_CAPI_MEAS_TEMP_ON_DRDY_OFF = 4,
	ADXL355_CAPI_STDBY_TEMP_ON_DRDY_OFF = 5,
	ADXL355_CAPI_MEAS_TEMP_OFF_DRDY_OFF = 6,
	ADXL355_CAPI_STDBY_TEMP_OFF_DRDY_OFF = 7
};

enum adxl355_capi_hpf_corner {
	ADXL355_CAPI_HPF_OFF,
	ADXL355_CAPI_HPF_24_7,
	ADXL355_CAPI_HPF_6_2084,
	ADXL355_CAPI_HPF_1_5545,
	ADXL355_CAPI_HPF_0_3862,
	ADXL355_CAPI_HPF_0_0954,
	ADXL355_CAPI_HPF_0_0238
};

enum adxl355_capi_odr_lpf {
	ADXL355_CAPI_ODR_4000HZ,
	ADXL355_CAPI_ODR_2000HZ,
	ADXL355_CAPI_ODR_1000HZ,
	ADXL355_CAPI_ODR_500HZ,
	ADXL355_CAPI_ODR_250HZ,
	ADXL355_CAPI_ODR_125HZ,
	ADXL355_CAPI_ODR_62_5HZ,
	ADXL355_CAPI_ODR_31_25HZ,
	ADXL355_CAPI_ODR_15_625HZ,
	ADXL355_CAPI_ODR_7_813HZ,
	ADXL355_CAPI_ODR_3_906HZ
};

enum adxl355_capi_range {
	ADXL355_CAPI_RANGE_2G = 1,
	ADXL359_CAPI_RANGE_10G = 1,
	ADXL355_CAPI_RANGE_4G = 2,
	ADXL359_CAPI_RANGE_20G = 2,
	ADXL355_CAPI_RANGE_8G = 3,
	ADXL359_CAPI_RANGE_40G = 3,
};

enum adxl355_capi_int_pol {
	ADXL355_CAPI_INT_ACTIVE_LOW = 0,
	ADXL355_CAPI_INT_ACTIVE_HIGH = 1
};

/**
 * @struct adxl355_capi_init_param
 * @brief Structure holding the parameters for ADXL355 CAPI device initialization.
 */
struct adxl355_capi_init_param {
	/** SPI controller handle */
	struct capi_spi_controller_handle *spi_controller;
	/** SPI device parameters */
	struct capi_spi_config comm_param;
	/** Device type: ADXL355 or 359 */
	enum adxl355_capi_type dev_type;
	struct capi_spi_device *spi_dev;
};

struct _adxl355_capi_int_mask {
	uint8_t RDY_EN1 : 1;
	uint8_t FULL_EN1 : 1;
	uint8_t OVR_EN1 : 1;
	uint8_t ACT_EN1 : 1;
	uint8_t RDY_EN2 : 1;
	uint8_t FULL_EN2 : 1;
	uint8_t OVR_EN2 : 1;
	uint8_t ACT_EN2 : 1;
};

union adxl355_capi_int_mask {
	struct _adxl355_capi_int_mask fields;
	uint8_t value;
};

struct _adxl355_capi_sts_reg_flags {
	uint8_t DATA_RDY : 1;
	uint8_t FIFO_FULL : 1;
	uint8_t FIFO_OVR : 1;
	uint8_t Activity : 1;
	uint8_t NVM_BUSY : 1;
	uint8_t reserved : 3;
};

union adxl355_capi_sts_reg_flags {
	struct _adxl355_capi_sts_reg_flags fields;
	uint8_t value;
};

struct _adxl355_capi_act_en_flags {
	uint8_t ACT_X    : 1;
	uint8_t ACT_Y    : 1;
	uint8_t ACT_Z    : 1;
	uint8_t reserved : 4;
};

union adxl355_capi_act_en_flags {
	struct _adxl355_capi_act_en_flags fields;
	uint8_t value;
};

struct adxl355_capi_frac_repr {
	int64_t integer;
	int32_t fractional;
} ;

/**
 * @struct adxl355_capi_dev
 * @brief ADXL355 CAPI Device structure.
 */
struct adxl355_capi_dev {
	/** Device type */
	enum adxl355_capi_type dev_type;
	/** SPI controller handle */
	struct capi_spi_controller_handle *spi_controller;
	/** SPI device handle */
	struct capi_spi_device *spi_device;
	/** Device operation mode */
	enum adxl355_capi_op_mode op_mode;
	/** Output data rate and low-pass filter setting */
	enum adxl355_capi_odr_lpf odr_lpf;
	/** High-pass filter corner frequency */
	enum adxl355_capi_hpf_corner hpf_corner;
	/** Measurement range */
	enum adxl355_capi_range range;
	/** X-axis offset */
	uint16_t x_offset;
	/** Y-axis offset */
	uint16_t y_offset;
	/** Z-axis offset */
	uint16_t z_offset;
	/** FIFO sample count */
	uint8_t fifo_samples;
	/** Activity enable flags */
	union adxl355_capi_act_en_flags act_en;
	/** Activity count threshold */
	uint8_t act_cnt;
	/** Activity threshold */
	uint16_t act_thr;
	/** Communication buffer for SPI transactions */
	uint8_t comm_buff[289];
};

/*! Init. the SPI peripheral and checks if the ADXL355 part is present. */
int adxl355_capi_init(struct adxl355_capi_dev **device,
		      struct adxl355_capi_init_param init_param);

/*! Free the resources allocated by adxl355_capi_init(). */
int adxl355_capi_remove(struct adxl355_capi_dev *dev);

/*! Places the device into the given operation mode. */
int adxl355_capi_set_op_mode(struct adxl355_capi_dev *dev, enum adxl355_capi_op_mode op_mode);

/*! Reads the device current operation mode. */
int adxl355_capi_get_op_mode(struct adxl355_capi_dev *dev,
			     enum adxl355_capi_op_mode *read_op_mode);

/*! Performs soft reset of the device. */
int adxl355_capi_soft_reset(struct adxl355_capi_dev *dev);

/*! Triggers the self-test feature. */
int adxl355_capi_set_self_test(struct adxl355_capi_dev *dev);

/*! Sets the measurement range register value. */
int adxl355_capi_set_range(struct adxl355_capi_dev *dev, enum adxl355_capi_range range_val);

/*! Writes the low-pass filter settings. */
int adxl355_capi_set_odr_lpf(struct adxl355_capi_dev *dev,
			     enum adxl355_capi_odr_lpf odr_lpf_val);

/*! Writes the high-pass filter settings. */
int adxl355_capi_set_hpf_corner(struct adxl355_capi_dev *dev,
				enum adxl355_capi_hpf_corner hpf_corner_val);

/*! Sets an offset value for each axis (Offset Calibration). */
int adxl355_capi_set_offset(struct adxl355_capi_dev *dev, uint16_t x_offset,
			    uint16_t y_offset, uint16_t z_offset);

/*! Reads the raw output data of each axis. */
int adxl355_capi_get_raw_xyz(struct adxl355_capi_dev *dev, uint32_t *raw_x,
			     uint32_t *raw_y, uint32_t *raw_z);

/*! Reads the raw output data of each axis and converts it to g. */
int adxl355_capi_get_xyz(struct adxl355_capi_dev *dev, struct adxl355_capi_frac_repr *x,
			 struct adxl355_capi_frac_repr *y, struct adxl355_capi_frac_repr *z);

/*! Reads the raw temperature data. */
int adxl355_capi_get_raw_temp(struct adxl355_capi_dev *dev, uint16_t *raw_temp);

/*! Reads the raw temperature data and converts it to millidegrees Celsius. */
int adxl355_capi_get_temp(struct adxl355_capi_dev *dev, struct adxl355_capi_frac_repr *temp);

/*! Reads the status register value. */
int adxl355_capi_get_sts_reg(struct adxl355_capi_dev *dev,
			     union adxl355_capi_sts_reg_flags *status_flags);

/*! Reads the number of FIFO entries register value. */
int adxl355_capi_get_nb_of_fifo_entries(struct adxl355_capi_dev *dev, uint8_t *reg_value);

/*! Sets the number of FIFO samples register value. */
int adxl355_capi_set_fifo_samples(struct adxl355_capi_dev *dev, uint8_t reg_value);

/*! Reads fifo data and returns the raw values. */
int adxl355_capi_get_raw_fifo_data(struct adxl355_capi_dev *dev, uint8_t *fifo_entries,
				   uint32_t *raw_x, uint32_t *raw_y, uint32_t *raw_z);

/*! Reads fifo data and returns the values converted in g. */
int adxl355_capi_get_fifo_data(struct adxl355_capi_dev *dev, uint8_t *fifo_entries,
			       struct adxl355_capi_frac_repr *x, struct adxl355_capi_frac_repr *y,
			       struct adxl355_capi_frac_repr *z);

/*! Configures the activity enable register. */
int adxl355_capi_conf_act_en(struct adxl355_capi_dev *dev,
			     union adxl355_capi_act_en_flags act_config);

/*! Configures the activity threshold registers. */
int adxl355_capi_conf_act_thr(struct adxl355_capi_dev *dev, uint16_t act_thr);

/*! Writes the activity count register value. */
int adxl355_capi_set_act_cnt_reg(struct adxl355_capi_dev *dev, uint8_t act_cnt);

/*! Configures the interrupt map for INT1 and INT2 pins. */
int adxl355_capi_config_int_pins(struct adxl355_capi_dev *dev,
				 union adxl355_capi_int_mask int_conf);

/*! Configures the interrupt polarity. */
int adxl355_capi_set_int_pol(struct adxl355_capi_dev *dev, enum adxl355_capi_int_pol int_pol);

#endif /* __ADXL355_CAPI_H__ */