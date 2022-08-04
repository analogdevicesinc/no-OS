/***************************************************************************//**
 *   @file   ADXL362.h
 *   @brief  Header file of ADXL362 Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
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

#ifndef __ADXL362_H__
#define __ADXL362_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************************* ADXL362 ************************************/
/******************************************************************************/

/* ADXL362 communication commands */
#define ADXL362_WRITE_REG               0x0A
#define ADXL362_READ_REG                0x0B
#define ADXL362_WRITE_FIFO              0x0D

/* Registers */
#define ADXL362_REG_DEVID_AD            0x00
#define ADXL362_REG_DEVID_MST           0x01
#define ADXL362_REG_PARTID              0x02
#define ADXL362_REG_REVID               0x03
#define ADXL362_REG_XDATA               0x08
#define ADXL362_REG_YDATA               0x09
#define ADXL362_REG_ZDATA               0x0A
#define ADXL362_REG_STATUS              0x0B
#define ADXL362_REG_FIFO_L              0x0C
#define ADXL362_REG_FIFO_H              0x0D
#define ADXL362_REG_XDATA_L             0x0E
#define ADXL362_REG_XDATA_H             0x0F
#define ADXL362_REG_YDATA_L             0x10
#define ADXL362_REG_YDATA_H             0x11
#define ADXL362_REG_ZDATA_L             0x12
#define ADXL362_REG_ZDATA_H             0x13
#define ADXL362_REG_TEMP_L              0x14
#define ADXL362_REG_TEMP_H              0x15
#define ADXL362_REG_SOFT_RESET          0x1F
#define ADXL362_REG_THRESH_ACT_L        0x20
#define ADXL362_REG_THRESH_ACT_H        0x21
#define ADXL362_REG_TIME_ACT            0x22
#define ADXL362_REG_THRESH_INACT_L      0x23
#define ADXL362_REG_THRESH_INACT_H      0x24
#define ADXL362_REG_TIME_INACT_L        0x25
#define ADXL362_REG_TIME_INACT_H        0x26
#define ADXL362_REG_ACT_INACT_CTL       0x27
#define ADXL362_REG_FIFO_CTL            0x28
#define ADXL362_REG_FIFO_SAMPLES        0x29
#define ADXL362_REG_INTMAP1             0x2A
#define ADXL362_REG_INTMAP2             0x2B
#define ADXL362_REG_FILTER_CTL          0x2C
#define ADXL362_REG_POWER_CTL           0x2D
#define ADXL362_REG_SELF_TEST           0x2E

/* ADXL362_REG_STATUS definitions */
#define ADXL362_STATUS_ERR_USER_REGS    (1 << 7)
#define ADXL362_STATUS_AWAKE            (1 << 6)
#define ADXL362_STATUS_INACT            (1 << 5)
#define ADXL362_STATUS_ACT              (1 << 4)
#define ADXL362_STATUS_FIFO_OVERRUN     (1 << 3)
#define ADXL362_STATUS_FIFO_WATERMARK   (1 << 2)
#define ADXL362_STATUS_FIFO_RDY         (1 << 1)
#define ADXL362_STATUS_DATA_RDY         (1 << 0)

/* ADXL362_REG_ACT_INACT_CTL definitions */
#define ADXL362_ACT_INACT_CTL_LINKLOOP(x)   (((x) & 0x3) << 4)
#define ADXL362_ACT_INACT_CTL_INACT_REF     (1 << 3)
#define ADXL362_ACT_INACT_CTL_INACT_EN      (1 << 2)
#define ADXL362_ACT_INACT_CTL_ACT_REF       (1 << 1)
#define ADXL362_ACT_INACT_CTL_ACT_EN        (1 << 0)

/* ADXL362_ACT_INACT_CTL_LINKLOOP(x) options */
#define ADXL362_MODE_DEFAULT            0
#define ADXL362_MODE_LINK               1
#define ADXL362_MODE_LOOP               3

/* ADXL362_REG_FIFO_CTL */
#define ADXL362_FIFO_CTL_AH             (1 << 3)
#define ADXL362_FIFO_CTL_FIFO_TEMP      (1 << 2)
#define ADXL362_FIFO_CTL_FIFO_MODE(x)   (((x) & 0x3) << 0)

/* ADXL362_FIFO_CTL_FIFO_MODE(x) options */
#define ADXL362_FIFO_DISABLE            0
#define ADXL362_FIFO_OLDEST_SAVED       1
#define ADXL362_FIFO_STREAM             2
#define ADXL362_FIFO_TRIGGERED          3

/* ADXL362_REG_INTMAP1 */
#define ADXL362_INTMAP1_INT_LOW         (1 << 7)
#define ADXL362_INTMAP1_AWAKE           (1 << 6)
#define ADXL362_INTMAP1_INACT           (1 << 5)
#define ADXL362_INTMAP1_ACT             (1 << 4)
#define ADXL362_INTMAP1_FIFO_OVERRUN    (1 << 3)
#define ADXL362_INTMAP1_FIFO_WATERMARK  (1 << 2)
#define ADXL362_INTMAP1_FIFO_READY      (1 << 1)
#define ADXL362_INTMAP1_DATA_READY      (1 << 0)

/* ADXL362_REG_INTMAP2 definitions */
#define ADXL362_INTMAP2_INT_LOW         (1 << 7)
#define ADXL362_INTMAP2_AWAKE           (1 << 6)
#define ADXL362_INTMAP2_INACT           (1 << 5)
#define ADXL362_INTMAP2_ACT             (1 << 4)
#define ADXL362_INTMAP2_FIFO_OVERRUN    (1 << 3)
#define ADXL362_INTMAP2_FIFO_WATERMARK  (1 << 2)
#define ADXL362_INTMAP2_FIFO_READY      (1 << 1)
#define ADXL362_INTMAP2_DATA_READY      (1 << 0)

/* ADXL362_REG_FILTER_CTL definitions */
#define ADXL362_FILTER_CTL_RANGE(x)     (((x) & 0x3) << 6)
#define ADXL362_FILTER_CTL_RES          (1 << 5)
#define ADXL362_FILTER_CTL_HALF_BW      (1 << 4)
#define ADXL362_FILTER_CTL_EXT_SAMPLE   (1 << 3)
#define ADXL362_FILTER_CTL_ODR(x)       (((x) & 0x7) << 0)

/* ADXL362_FILTER_CTL_RANGE(x) options */
#define ADXL362_RANGE_2G                0 /* +/-2 g */
#define ADXL362_RANGE_4G                1 /* +/-4 g */
#define ADXL362_RANGE_8G                2 /* +/-8 g */

/* ADXL362_FILTER_CTL_ODR(x) options */
#define ADXL362_ODR_12_5_HZ             0 /* 12.5 Hz */
#define ADXL362_ODR_25_HZ               1 /* 25 Hz */
#define ADXL362_ODR_50_HZ               2 /* 50 Hz */
#define ADXL362_ODR_100_HZ              3 /* 100 Hz */
#define ADXL362_ODR_200_HZ              4 /* 200 Hz */
#define ADXL362_ODR_400_HZ              5 /* 400 Hz */

/* ADXL362_REG_POWER_CTL definitions */
#define ADXL362_POWER_CTL_RES           (1 << 7)
#define ADXL362_POWER_CTL_EXT_CLK       (1 << 6)
#define ADXL362_POWER_CTL_LOW_NOISE(x)  (((x) & 0x3) << 4)
#define ADXL362_POWER_CTL_WAKEUP        (1 << 3)
#define ADXL362_POWER_CTL_AUTOSLEEP     (1 << 2)
#define ADXL362_POWER_CTL_MEASURE(x)    (((x) & 0x3) << 0)

/* ADXL362_POWER_CTL_LOW_NOISE(x) options */
#define ADXL362_NOISE_MODE_NORMAL       0
#define ADXL362_NOISE_MODE_LOW          1
#define ADXL362_NOISE_MODE_ULTRALOW     2

/* ADXL362_POWER_CTL_MEASURE(x) options */
#define ADXL362_MEASURE_STANDBY         0
#define ADXL362_MEASURE_ON              2

/* ADXL362_REG_SELF_TEST */
#define ADXL362_SELF_TEST_ST            (1 << 0)

/* ADXL362 device information */
#define ADXL362_DEVICE_AD               0xAD
#define ADXL362_DEVICE_MST              0x1D
#define ADXL362_PART_ID                 0xF2

/* ADXL362 Reset settings */
#define ADXL362_RESET_KEY               0x52

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct adxl362_dev
 * @brief ADXL362 Device structure.
 */
struct adxl362_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc	*spi_desc;
	/** Measurement Range: */
	uint8_t		selected_range;
};

/**
 * @struct adxl362_init_param
 * @brief Structure holding the parameters for ADXL362 device initialization.
 */
struct adxl362_init_param {
	/** SPI Initialization structure. */
	struct no_os_spi_init_param	spi_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the device. */
int32_t adxl362_init(struct adxl362_dev **device,
		     struct adxl362_init_param init_param);

/*! Free the resources allocated by adxl362_init(). */
int32_t adxl362_remove(struct adxl362_dev *dev);

/*! Writes data into a register. */
void adxl362_set_register_value(struct adxl362_dev *dev,
				uint16_t register_value,
				uint8_t  register_address,
				uint8_t  bytes_number);

/*! Performs a burst read of a specified number of registers. */
void adxl362_get_register_value(struct adxl362_dev *dev,
				uint8_t *read_data,
				uint8_t  register_address,
				uint8_t  bytes_number);

/*! Reads multiple bytes from the device's FIFO buffer. */
void adxl362_get_fifo_value(struct adxl362_dev *dev,
			    uint8_t *buffer,
			    uint16_t bytes_number);

/*! Resets the device via SPI communication bus. */
void adxl362_software_reset(struct adxl362_dev *dev);

/*! Places the device into standby/measure mode. */
void adxl362_set_power_mode(struct adxl362_dev *dev,
			    uint8_t pwr_mode);

/*! Selects the measurement range. */
void adxl362_set_range(struct adxl362_dev *dev,
		       uint8_t g_range);

/*! Selects the Output Data Rate of the device. */
void adxl362_set_output_rate(struct adxl362_dev *dev,
			     uint8_t out_rate);

/*! Reads the 3-axis raw data from the accelerometer. */
void adxl362_get_xyz(struct adxl362_dev *dev,
		     int16_t *x,
		     int16_t *y,
		     int16_t *z);

/*! Reads the 3-axis raw data from the accelerometer and converts it to g. */
void adxl362_get_g_xyz(struct adxl362_dev *dev,
		       float* x,
		       float* y,
		       float* z);

/*! Reads the temperature of the device. */
float adxl362_read_temperature(struct adxl362_dev *dev);

/*! Configures the FIFO feature. */
void adxl362_fifo_setup(struct adxl362_dev *dev,
			uint8_t  mode,
			uint16_t water_mark_lvl,
			uint8_t  en_temp_read);

/*! Configures activity detection. */
void adxl362_setup_activity_detection(struct adxl362_dev *dev,
				      uint8_t  ref_or_abs,
				      uint16_t threshold,
				      uint8_t  time);

/*! Configures inactivity detection. */
void adxl362_setup_inactivity_detection(struct adxl362_dev *dev,
					uint8_t  ref_or_abs,
					uint16_t threshold,
					uint16_t time);

#endif /* __ADXL362_H__ */
