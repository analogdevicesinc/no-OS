/***************************************************************************//**
 *   @file   adxl366.h
 *   @brief  Header file of ADXL366 Driver.
 *   @author Marco Ramirez (marco.ramirez@analog.com)
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

#ifndef __ADXL366_H__
#define __ADXL366_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

/* ADXL366 SPI commands */
#define ADXL366_WRITE_REG               0x0A
#define ADXL366_READ_REG                0x0B
#define ADXL366_READ_FIFO               0x0D

/* ADXL366 Registers */
#define ADXL366_REG_DEVID_AD            0x00  /* Analog Devices ID (0xAD) */
#define ADXL366_REG_DEVID_MST           0x01  /* Analog Devices MEMS ID (0x1D) */
#define ADXL366_REG_PARTID              0x02  /* Device ID (0xF7 for ADXL366) */
#define ADXL366_REG_REVID               0x03  /* Silicon Revision ID */
#define ADXL366_REG_XDATA               0x08  /* X-axis data (8-bit) */
#define ADXL366_REG_YDATA               0x09  /* Y-axis data (8-bit) */
#define ADXL366_REG_ZDATA               0x0A  /* Z-axis data (8-bit) */
#define ADXL366_REG_STATUS              0x0B  /* Status register */
#define ADXL366_REG_FIFO_ENTRIES_L      0x0C  /* FIFO entries low byte */
#define ADXL366_REG_FIFO_ENTRIES_H      0x0D  /* FIFO entries high byte */
#define ADXL366_REG_XDATA_H             0x0E  /* X-axis data high byte */
#define ADXL366_REG_XDATA_L             0x0F  /* X-axis data low byte */
#define ADXL366_REG_YDATA_H             0x10  /* Y-axis data high byte */
#define ADXL366_REG_YDATA_L             0x11  /* Y-axis data low byte */
#define ADXL366_REG_ZDATA_H             0x12  /* Z-axis data high byte */
#define ADXL366_REG_ZDATA_L             0x13  /* Z-axis data low byte */
#define ADXL366_REG_TEMP_H              0x14  /* Temperature high byte */
#define ADXL366_REG_TEMP_L              0x15  /* Temperature low byte */
#define ADXL366_REG_SOFT_RESET          0x1F  /* Soft reset */
#define ADXL366_REG_THRESH_ACT_H        0x20  /* Activity threshold high */
#define ADXL366_REG_THRESH_ACT_L        0x21  /* Activity threshold low */
#define ADXL366_REG_TIME_ACT            0x22  /* Activity time */
#define ADXL366_REG_THRESH_INACT_H      0x23  /* Inactivity threshold high */
#define ADXL366_REG_THRESH_INACT_L      0x24  /* Inactivity threshold low */
#define ADXL366_REG_TIME_INACT_H        0x25  /* Inactivity time high */
#define ADXL366_REG_TIME_INACT_L        0x26  /* Inactivity time low */
#define ADXL366_REG_ACT_INACT_CTL       0x27  /* Activity/Inactivity control */
#define ADXL366_REG_FIFO_CONTROL        0x28  /* FIFO control */
#define ADXL366_REG_FIFO_SAMPLES        0x29  /* FIFO samples */
#define ADXL366_REG_INTMAP1_LWR         0x2A  /* Interrupt map 1 lower */
#define ADXL366_REG_INTMAP2_LWR         0x2B  /* Interrupt map 2 lower */
#define ADXL366_REG_FILTER_CTL          0x2C  /* Filter control */
#define ADXL366_REG_POWER_CTL           0x2D  /* Power control */

/* ADXL366_REG_STATUS definitions */
#define ADXL366_STATUS_ERR_USER_REGS    NO_OS_BIT(7)
#define ADXL366_STATUS_AWAKE            NO_OS_BIT(6)
#define ADXL366_STATUS_INACT            NO_OS_BIT(5)
#define ADXL366_STATUS_ACT              NO_OS_BIT(4)
#define ADXL366_STATUS_FIFO_OVERRUN     NO_OS_BIT(3)
#define ADXL366_STATUS_FIFO_WATERMARK   NO_OS_BIT(2)
#define ADXL366_STATUS_FIFO_RDY         NO_OS_BIT(1)
#define ADXL366_STATUS_DATA_RDY         NO_OS_BIT(0)

/* ADXL366_REG_FILTER_CTL definitions */
#define ADXL366_FILTER_CTL_RANGE_MSK    NO_OS_GENMASK(7, 6)
#define ADXL366_FILTER_CTL_RANGE_2G     0x00
#define ADXL366_FILTER_CTL_RANGE_4G     0x01
#define ADXL366_FILTER_CTL_RANGE_8G     0x02
#define ADXL366_FILTER_CTL_ODR_MSK      NO_OS_GENMASK(2, 0)
#define ADXL366_FILTER_CTL_ODR_12_5HZ   0x00
#define ADXL366_FILTER_CTL_ODR_25HZ     0x01
#define ADXL366_FILTER_CTL_ODR_50HZ     0x02
#define ADXL366_FILTER_CTL_ODR_100HZ    0x03
#define ADXL366_FILTER_CTL_ODR_200HZ    0x04
#define ADXL366_FILTER_CTL_ODR_400HZ    0x05

/* ADXL366_REG_POWER_CTL definitions */
#define ADXL366_POWER_CTL_MODE_MSK      0x03
#define ADXL366_POWER_CTL_STANDBY       0x00
#define ADXL366_POWER_CTL_MEASURE       0x02
#define ADXL366_POWER_CTL_NOISE_MODE    NO_OS_BIT(4)
#define ADXL366_POWER_CTL_EXT_CLK       NO_OS_BIT(6)

/* ADXL366_REG_FIFO_CONTROL definitions */
#define ADXL366_FIFO_CONTROL_MODE_MSK   NO_OS_GENMASK(1, 0)
#define ADXL366_FIFO_CONTROL_DISABLED   0x00
#define ADXL366_FIFO_CONTROL_OLDEST_SAVED 0x01
#define ADXL366_FIFO_CONTROL_STREAM     0x02
#define ADXL366_FIFO_CONTROL_TRIGGERED  0x03

/* ADXL366_REG_ACT_INACT_CTL definitions */
#define ADXL366_ACT_INACT_CTL_LINKLOOP_MSK  NO_OS_GENMASK(5, 4)
#define ADXL366_ACT_INACT_CTL_INACT_EN      NO_OS_BIT(2)
#define ADXL366_ACT_INACT_CTL_ACT_EN        NO_OS_BIT(0)

/* ADXL366_REG_INTMAP1_LWR and INTMAP2_LWR definitions */
#define ADXL366_INTMAP_DATA_RDY         NO_OS_BIT(0)
#define ADXL366_INTMAP_FIFO_RDY         NO_OS_BIT(1)
#define ADXL366_INTMAP_FIFO_WATERMARK   NO_OS_BIT(2)
#define ADXL366_INTMAP_FIFO_OVERRUN     NO_OS_BIT(3)
#define ADXL366_INTMAP_ACT              NO_OS_BIT(4)
#define ADXL366_INTMAP_INACT            NO_OS_BIT(5)
#define ADXL366_INTMAP_AWAKE            NO_OS_BIT(6)

/* Range enumerations */
enum adxl366_range {
	ADXL366_RANGE_2G = 0,
	ADXL366_RANGE_4G = 1,
	ADXL366_RANGE_8G = 2
};

/* Output Data Rate enumerations */
enum adxl366_odr {
	ADXL366_ODR_12_5HZ = 0,
	ADXL366_ODR_25HZ = 1,
	ADXL366_ODR_50HZ = 2,
	ADXL366_ODR_100HZ = 3,
	ADXL366_ODR_200HZ = 4,
	ADXL366_ODR_400HZ = 5
};

/* FIFO mode enumerations */
enum adxl366_fifo_mode {
	ADXL366_FIFO_DISABLED = 0,
	ADXL366_FIFO_OLDEST_SAVED = 1,
	ADXL366_FIFO_STREAM = 2,
	ADXL366_FIFO_TRIGGERED = 3
};

/* ADXL366 Expected ID values */
#define ADXL366_DEVID_AD_VAL            0xAD
#define ADXL366_DEVID_MST_VAL           0x1D
#define ADXL366_PARTID_VAL              0xF7  /* ADXL366 Part ID per datasheet */

/* Soft reset key */
#define ADXL366_RESET_KEY               0x52

/**
 * @struct adxl366_dev
 * @brief ADXL366 Device structure.
 */
struct adxl366_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
	/** Device ID read from chip */
	uint8_t dev_id;
};

/**
 * @struct adxl366_init_param
 * @brief Structure holding the parameters for ADXL366 device initialization.
 */
struct adxl366_init_param {
	/** SPI initialization parameters */
	struct no_os_spi_init_param spi_init;
};

/* Initialize the device */
int adxl366_init(struct adxl366_dev **device,
		 struct adxl366_init_param init_param);

/* Free resources allocated by adxl366_init() */
int adxl366_remove(struct adxl366_dev *dev);

/* SPI register write */
int adxl366_reg_write(struct adxl366_dev *dev, uint8_t reg_addr,
		      uint8_t reg_data);

/* SPI register read */
int adxl366_reg_read(struct adxl366_dev *dev, uint8_t reg_addr,
		     uint8_t *reg_data);

/* Read multiple registers */
int adxl366_reg_read_multiple(struct adxl366_dev *dev, uint8_t reg_addr,
			       uint8_t *reg_data, uint16_t count);

/* Soft reset */
int adxl366_software_reset(struct adxl366_dev *dev);

/* Set power mode */
int adxl366_set_power_mode(struct adxl366_dev *dev, uint8_t mode);

/* Get power mode */
int adxl366_get_power_mode(struct adxl366_dev *dev, uint8_t *mode);

/* Read accelerometer data */
int adxl366_read_accel(struct adxl366_dev *dev, int16_t *x, int16_t *y,
		       int16_t *z);

/* Set measurement range */
int adxl366_set_range(struct adxl366_dev *dev, enum adxl366_range range);

/* Get measurement range */
int adxl366_get_range(struct adxl366_dev *dev, enum adxl366_range *range);

/* Set output data rate */
int adxl366_set_odr(struct adxl366_dev *dev, enum adxl366_odr odr);

/* Get output data rate */
int adxl366_get_odr(struct adxl366_dev *dev, enum adxl366_odr *odr);

/* Read temperature */
int adxl366_read_temperature(struct adxl366_dev *dev, float *temp_c);

/* Configure FIFO mode */
int adxl366_set_fifo_mode(struct adxl366_dev *dev, enum adxl366_fifo_mode mode,
			  uint16_t watermark);

/* Read FIFO entries count */
int adxl366_get_fifo_entries(struct adxl366_dev *dev, uint16_t *entries);

/* Read FIFO data */
int adxl366_read_fifo(struct adxl366_dev *dev, int16_t *x, int16_t *y,
		      int16_t *z, uint16_t *entries);

/* Set activity threshold */
int adxl366_set_activity_threshold(struct adxl366_dev *dev, uint16_t threshold);

/* Set inactivity threshold */
int adxl366_set_inactivity_threshold(struct adxl366_dev *dev,
				     uint16_t threshold);

/* Set activity time */
int adxl366_set_activity_time(struct adxl366_dev *dev, uint8_t time);

/* Set inactivity time */
int adxl366_set_inactivity_time(struct adxl366_dev *dev, uint16_t time);

/* Enable/disable activity detection */
int adxl366_set_activity_detection(struct adxl366_dev *dev, bool enable);

/* Enable/disable inactivity detection */
int adxl366_set_inactivity_detection(struct adxl366_dev *dev, bool enable);

/* Configure interrupt mapping */
int adxl366_set_interrupt_map(struct adxl366_dev *dev, uint8_t int_pin,
			       uint8_t int_mask);

/* Read status register */
int adxl366_get_status(struct adxl366_dev *dev, uint8_t *status);

#endif /* __ADXL366_H__ */
