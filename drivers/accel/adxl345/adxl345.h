/***************************************************************************//**
 *   @file   adxl345.h
 *   @brief  Header file of ADXL345 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#ifndef __ADXL345_H__
#define __ADXL345_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_i2c.h"
#include "no_os_spi.h"

/******************************************************************************/
/******************************** ADXL345 *************************************/
/******************************************************************************/

/* Options for communicating with the device. */
#define ADXL345_SPI_COMM       0
#define ADXL345_I2C_COMM       1

/* I2C address of the device */
#define ADXL345_ADDRESS		0x1D

/* SPI commands */
#define ADXL345_SPI_READ        (1 << 7)
#define ADXL345_SPI_WRITE       (0 << 7)
#define ADXL345_SPI_MB          (1 << 6)

/* ADXL345 Register Map */
#define	ADXL345_DEVID           0x00 // R   Device ID.
#define ADXL345_THRESH_TAP      0x1D // R/W Tap threshold.
#define ADXL345_OFSX            0x1E // R/W X-axis offset.
#define ADXL345_OFSY            0x1F // R/W Y-axis offset.
#define ADXL345_OFSZ            0x20 // R/W Z-axis offset.
#define ADXL345_DUR             0x21 // R/W Tap duration.
#define ADXL345_LATENT          0x22 // R/W Tap latency.
#define ADXL345_WINDOW          0x23 // R/W Tap window.
#define ADXL345_THRESH_ACT      0x24 // R/W Activity threshold.
#define ADXL345_THRESH_INACT    0x25 // R/W Inactivity threshold.
#define ADXL345_TIME_INACT      0x26 // R/W Inactivity time.
#define ADXL345_ACT_INACT_CTL   0x27 // R/W Axis enable control for activity
// and inactivity detection.
#define ADXL345_THRESH_FF       0x28 // R/W Free-fall threshold.
#define ADXL345_TIME_FF         0x29 // R/W Free-fall time.
#define ADXL345_TAP_AXES        0x2A // R/W Axis control for tap/double tap.
#define ADXL345_ACT_TAP_STATUS  0x2B // R   Source of tap/double tap.
#define ADXL345_BW_RATE         0x2C // R/W Data rate and power mode control.
#define ADXL345_POWER_CTL       0x2D // R/W Power saving features control.
#define ADXL345_INT_ENABLE      0x2E // R/W Interrupt enable control.
#define ADXL345_INT_MAP         0x2F // R/W Interrupt mapping control.
#define ADXL345_INT_SOURCE      0x30 // R   Source of interrupts.
#define ADXL345_DATA_FORMAT     0x31 // R/W Data format control.
#define ADXL345_DATAX0          0x32 // R   X-Axis Data 0.
#define ADXL345_DATAX1          0x33 // R   X-Axis Data 1.
#define ADXL345_DATAY0          0x34 // R   Y-Axis Data 0.
#define ADXL345_DATAY1          0x35 // R   Y-Axis Data 1.
#define ADXL345_DATAZ0          0x36 // R   Z-Axis Data 0.
#define ADXL345_DATAZ1          0x37 // R   Z-Axis Data 1.
#define ADXL345_FIFO_CTL        0x38 // R/W FIFO control.
#define ADXL345_FIFO_STATUS     0x39 // R   FIFO status.
#define ADXL345_TAP_SIGN        0x3A // R   Sign and source for single tap/double tap.
#define ADXL345_ORIENT_CONF     0x3B // R/W Orientation configuration.
#define ADXL345_ORIENT          0x3C // R   Orientation status.

/* ADXL345_ACT_INACT_CTL definition */
#define ADXL345_ACT_ACDC        (1 << 7)
#define ADXL345_ACT_X_EN        (1 << 6)
#define ADXL345_ACT_Y_EN        (1 << 5)
#define ADXL345_ACT_Z_EN        (1 << 4)
#define ADXL345_INACT_ACDC      (1 << 3)
#define ADXL345_INACT_X_EN      (1 << 2)
#define ADXL345_INACT_Y_EN      (1 << 1)
#define ADXL345_INACT_Z_EN      (1 << 0)

/* ADXL345_TAP_AXES definition */
#define ADXL345_SUPPRESS        (1 << 3)
#define ADXL345_TAP_X_EN        (1 << 2)
#define ADXL345_TAP_Y_EN        (1 << 1)
#define ADXL345_TAP_Z_EN        (1 << 0)

/* ADXL345_ACT_TAP_STATUS definition */
#define ADXL345_ACT_X_SRC       (1 << 6)
#define ADXL345_ACT_Y_SRC       (1 << 5)
#define ADXL345_ACT_Z_SRC       (1 << 4)
#define ADXL345_ASLEEP          (1 << 3)
#define ADXL345_TAP_X_SRC       (1 << 2)
#define ADXL345_TAP_Y_SRC       (1 << 1)
#define ADXL345_TAP_Z_SRC       (1 << 0)

/* ADXL345_BW_RATE definition */
#define ADXL345_LOW_POWER       (1 << 4)
#define ADXL345_RATE(x)         ((x) & 0xF)

/* ADXL345_POWER_CTL definition */
#define ADXL345_PCTL_LINK       (1 << 5)
#define ADXL345_PCTL_AUTO_SLEEP (1 << 4)
#define ADXL345_PCTL_MEASURE    (1 << 3)
#define ADXL345_PCTL_SLEEP      (1 << 2)
#define ADXL345_PCTL_WAKEUP(x)  ((x) & 0x3)

/* ADXL345_INT_ENABLE / ADXL345_INT_MAP / ADXL345_INT_SOURCE definition */
#define ADXL345_DATA_READY      (1 << 7)
#define ADXL345_SINGLE_TAP      (1 << 6)
#define ADXL345_DOUBLE_TAP      (1 << 5)
#define ADXL345_ACTIVITY        (1 << 4)
#define ADXL345_INACTIVITY      (1 << 3)
#define ADXL345_FREE_FALL       (1 << 2)
#define ADXL345_WATERMARK       (1 << 1)
#define ADXL345_OVERRUN         (1 << 0)
#define ADXL345_ORIENTATION     (1 << 0)

/* ADXL345_DATA_FORMAT definition */
#define ADXL345_SELF_TEST       (1 << 7)
#define ADXL345_SPI             (1 << 6)
#define ADXL345_INT_INVERT      (1 << 5)
#define ADXL345_FULL_RES        (1 << 3)
#define ADXL345_JUSTIFY         (1 << 2)
#define ADXL345_RANGE(x)        ((x) & 0x3)

/* ADXL345_RANGE(x) options */
#define ADXL345_RANGE_PM_2G     0
#define ADXL345_RANGE_PM_4G     1
#define ADXL345_RANGE_PM_8G     2
#define ADXL345_RANGE_PM_16G    3

/* ADXL345_FIFO_CTL definition */
#define ADXL345_FIFO_MODE(x)    (((x) & 0x3) << 6)
#define ADXL345_TRIGGER         (1 << 5)
#define ADXL345_SAMPLES(x)      ((x) & 0x1F)

/* ADXL345_FIFO_MODE(x) options */
#define ADXL345_FIFO_BYPASS     0
#define ADXL345_FIFO_FIFO       1
#define ADXL345_FIFO_STREAM     2
#define ADXL345_FIFO_TRIGGER    3

/* ADXL345_FIFO_STATUS definition */
#define ADXL345_FIFO_TRIG       (1 << 7)
#define ADXL345_ENTRIES(x)      ((x) & 0x3F)

/* ADXL345_ORIENT_CONF definition */
#define ADXL345_INT_ORIENT(x)   (((x) & 0x1) << 7)
#define ADXL345_DEAD_ZONE(x)    (((x) & 0x7) << 4)
#define ADXL345_INT_3D(x)       (((x) & 0x1) << 3)
#define ADXL345_DIVISOR(x)      ((x) & 7)

/* ADXL345 ID */
#define ADXL345_ID              0xE5
/* ADXL346 ID */
#define ADXL346_ID              0xE6

/* ADXL345 Full Resolution Scale Factor */
#define ADXL345_SCALE_FACTOR    0.0039

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum adxl345_type
 * @brief ADXL345 Supported devices.
 */
enum adxl345_type {
	ID_ADXL345,
	ID_ADXL346,
};

/**
 * @enum adxl345_dead_zone_angle
 * @brief ADXL346 Dead zone angle encoding.
 */
enum adxl345_dead_zone_angle {
	DEGREES_5_1,
	DEGREES_10_2,
	DEGREES_15_2,
	DEGREES_20_4,
	DEGREES_25_5,
	DEGREES_30_8,
	DEGREES_36_1,
	DEGREES_41_4,
};

/**
 * @enum adxl345_divisor_bandwidth
 * @brief ADXL346 Divisor bandwidth encoding.
 */
enum adxl345_divisor_bandwidth {
	ODR_DIV_9,
	ODR_DIV_22,
	ODR_DIV_50,
	ODR_DIV_100,
	ODR_DIV_200,
	ODR_DIV_400,
	ODR_DIV_800,
	ODR_DIV_1600
};

/**
 * @struct adxl345_dev
 * @brief ADXL345 Device structure.
 */
struct adxl345_dev {
	/** I2C Descriptor */
	struct no_os_i2c_desc	*i2c_desc;
	/** SPI Descriptor */
	struct no_os_spi_desc	*spi_desc;
	/** Device type ADXL345 or 346 */
	enum adxl345_type	dev_type;
	/** Device Communication type: ADXL345_SPI_COMM, ADXL345_I2C_COMM */
	uint8_t		communication_type;
	/** Measurement range */
	uint8_t		selected_range;
	/** Enable/Disable Full Resolution */
	uint8_t		full_resolution_set;
};

/**
 * @struct adxl345_init_param
 * @brief Structure holding the parameters for ADXL345 device initialization.
 */
struct adxl345_init_param {
	/** I2C Initialization structure. */
	struct no_os_i2c_init_param	i2c_init;
	/** SPI Initialization structure. */
	struct no_os_spi_init_param	spi_init;
	/** Device type ADXL345 or 346 */
	enum adxl345_type		dev_type;
	/** Device Communication type: ADXL345_SPI_COMM, ADXL345_I2C_COMM */
	uint8_t		communication_type;
	/** Measurement range */
	uint8_t		selected_range;
	/** Enable/Disable Full Resolution */
	uint8_t		full_resolution_set;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Reads the value of a register. */
uint8_t adxl345_get_register_value(struct adxl345_dev *dev,
				   uint8_t register_address);

/*! Writes data into a register. */
void adxl345_set_register_value(struct adxl345_dev *dev,
				uint8_t register_address,
				uint8_t register_value);

/*! Init. the comm. peripheral and checks if the ADXL345 part is present. */
int32_t adxl345_init(struct adxl345_dev **device,
		     struct adxl345_init_param init_param);

/*! Free the resources allocated by adxl345_init(). */
int32_t adxl345_remove(struct adxl345_dev *dev);

/*! Places the device into standby/measure mode. */
void adxl345_set_power_mode(struct adxl345_dev *dev,
			    uint8_t pwr_mode);

/*! Reads the raw output data of each axis. */
void adxl345_get_xyz(struct adxl345_dev *dev,
		     int16_t* x,
		     int16_t* y,
		     int16_t* z);

/*! Reads the raw output data of each axis and converts it to g. */
void adxl345_get_g_xyz(struct adxl345_dev *dev,
		       float* x,
		       float* y,
		       float* z);

/*! Enables/disables the tap detection. */
void adxl345_set_tap_detection(struct adxl345_dev *dev,
			       uint8_t tap_type,
			       uint8_t tap_axes,
			       uint8_t tap_dur,
			       uint8_t tap_latent,
			       uint8_t tap_window,
			       uint8_t tap_thresh,
			       uint8_t tap_int);

/*! Enables/disables the activity detection. */
void adxl345_set_activity_detection(struct adxl345_dev *dev,
				    uint8_t act_on_off,
				    uint8_t act_axes,
				    uint8_t act_ac_dc,
				    uint8_t act_thresh,
				    uint8_t act_int);

/*! Enables/disables the inactivity detection. */
void adxl345_set_inactivity_detection(struct adxl345_dev *dev,
				      uint8_t inact_on_off,
				      uint8_t inact_axes,
				      uint8_t inact_ac_dc,
				      uint8_t inact_thresh,
				      uint8_t inact_time,
				      uint8_t inact_int);

/*! Enables/disables the free-fall detection. */
void adxl345_set_free_fall_detection(struct adxl345_dev *dev,
				     uint8_t ff_on_off,
				     uint8_t ff_thresh,
				     uint8_t ff_time,
				     uint8_t ff_int);

/*! Enables/disables the orientation detection. */
void adxl345_set_orientation_detection(struct adxl345_dev *dev,
				       uint8_t orient_int,
				       uint8_t orient_on_off,
				       uint8_t int_3d,
				       enum adxl345_dead_zone_angle dead_zone,
				       enum adxl345_divisor_bandwidth divisor);

/*! Sets an offset value for each axis (Offset Calibration). */
void adxl345_set_offset(struct adxl345_dev *dev,
			uint8_t x_offset,
			uint8_t y_offset,
			uint8_t z_offset);

/*! Selects the measurement range. */
void adxl345_set_range_resolution(struct adxl345_dev *dev,
				  uint8_t g_range,
				  uint8_t full_res);

#endif	/* __ADXL345_H__ */
