/***************************************************************************//**
 *   @file   adt7420.h
 *   @brief  Header file of ADT7420 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012, 2019, 2021(c) Analog Devices, Inc.
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
#ifndef __ADT7420_H__
#define __ADT7420_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include <stdbool.h>

/******************************************************************************/
/************************** ADT7420 Definitions *******************************/
/******************************************************************************/

/* ADT7420 address */
#define ADT7420_A0_PIN(x)		(((x) & 0x1) << 0) // I2C Serial Bus Address Selection Pin
#define ADT7420_A1_PIN(x)		(((x) & 0x1) << 1) // I2C Serial Bus Address Selection Pin
#define ADT7420_ADDRESS(x,y)		(0x48 + ADT7420_A1_PIN(x) + ADT7420_A0_PIN(y))
#define ADT7320_L16				NO_OS_BIT(8)	//indicator that register is 16-bit
#define ADT7320_ADDR_MSK		NO_OS_GENMASK(7,0)

/* ADT7320  (SPI) registers */
#define ADT7320_REG_STATUS		0x00 // Status
#define ADT7320_REG_CONFIG		0x01 // Configuration
#define ADT7320_REG_TEMP    	(ADT7320_L16 | 0x02) // Temperature value
#define ADT7320_REG_ID			0x03 // ID
#define ADT7320_REG_T_CRIT		(ADT7320_L16 | 0x04) // Temperature CRIT setpoint (147'C)
#define ADT7320_REG_HIST		0x05 // Temperature HYST setpoint (5'C)
#define ADT7320_REG_T_HIGH    		(ADT7320_L16 | 0x06) // Temperature HIGH setpoint (64'C)
#define ADT7320_REG_T_LOW    		(ADT7320_L16 | 0x07) // Temperature LOW setpoint (10'C)

/* ADT7320 SPI command byte */
#define ADT7320_WRITE_MASK_CMD		0b00111000 // SPI write command
#define ADT7320_READ_CMD		0b01000000 // SPI read command

/* ADT7420 (I2C) registers */
#define ADT7420_REG_TEMP_MSB		(ADT7320_L16 |0x00) // Temperature value MSB
#define ADT7420_REG_TEMP_LSB		0x01 // Temperature value LSB
#define ADT7420_REG_STATUS		0x02 // Status
#define ADT7420_REG_CONFIG		0x03 // Configuration
#define ADT7420_REG_T_HIGH_MSB		(ADT7320_L16 |0x04 )// Temperature HIGH setpoint MSB
#define ADT7420_REG_T_HIGH_LSB		0x05 // Temperature HIGH setpoint LSB
#define ADT7420_REG_T_LOW_MSB		(ADT7320_L16 |0x06) // Temperature LOW setpoint MSB
#define ADT7420_REG_T_LOW_LSB		0x07 // Temperature LOW setpoint LSB
#define ADT7420_REG_T_CRIT_MSB		(ADT7320_L16 |0x08) // Temperature CRIT setpoint MSB
#define ADT7420_REG_T_CRIT_LSB		0x09 // Temperature CRIT setpoint LSB
#define ADT7420_REG_HIST		0x0A // Temperature HYST setpoint
#define ADT7420_REG_ID			0x0B // ID
#define ADT7420_REG_RESET		0x2F // Software reset

/* ADT7420_REG_STATUS definition */
#define ADT7420_STATUS_T_LOW		NO_OS_BIT(4)
#define ADT7420_STATUS_T_HIGH		NO_OS_BIT(5)
#define ADT7420_STATUS_T_CRIT		NO_OS_BIT(6)
#define ADT7420_STATUS_RDY		NO_OS_BIT(7)

/* ADT7420_REG_CONFIG definition */
#define ADT7420_CONFIG_OP_MODE(x)	((x) << 5) & (NO_OS_GENMASK(6,5))
#define ADT7420_CONFIG_RESOLUTION	NO_OS_BIT(7)

/* ADT7420 temperature conversion definitions */
#define ADT7420_16BIT_NEG			NO_OS_BIT(16)
#define ADT7420_16BIT_SIGN			0x8000
#define ADT7420_16BIT_DIV			128
#define ADT7420_13BIT_NEG			NO_OS_BIT(13)
#define ADT7420_13BIT_SIGN			0x1000
#define ADT7420_13BIT_DIV			16

/* ADT7420_CONFIG_OP_MODE(x) options */
#define ADT7420_OP_MODE_CONT_CONV	0
#define ADT7420_OP_MODE_ONE_SHOT	1
#define ADT7420_OP_MODE_1_SPS		2
#define ADT7420_OP_MODE_SHUTDOWN	3

/* ADT7420 default ID */
#define ADT7xxx_ID			0xC

#define ADT7420_RESET_DELAY 		1


/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum dev_interface {
	SPI,
	I2C,
};

struct adt7420_chip_info {
	uint8_t			resolution;
	enum dev_interface  communication;
};

enum adt7420_type {
	ID_ADT7410,
	ID_ADT7420,
	ID_ADT7422,
	ID_ADT7310,
	ID_ADT7311,
	ID_ADT7312,
	ID_ADT7320
};

struct adt7420_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* Device Settings */
	enum adt7420_type active_device;
	/* Device Settings */
	uint8_t		resolution_setting;

};

struct adt7420_init_param {
	union interface_type {
		/* I2C */
		struct no_os_i2c_init_param	i2c_init;
		/* SPI */
		struct no_os_spi_init_param	spi_init;
	} interface_init;
	/* Device Settings */
	uint8_t		resolution_setting;
	/* Device Settings */
	enum adt7420_type	active_device;
};

extern const struct adt7420_chip_info chip_info[];

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Reads the value of a register */
int adt7420_reg_read(struct adt7420_dev *dev,
		     uint16_t register_address, uint16_t *data);

/* Read-modify-write operation*/
int adt7420_reg_update_bits(struct adt7420_dev *dev,
			    uint16_t register_address, uint8_t mask, uint8_t value);

/* Write to SPI register */
int adt7420_spi_reg_write(struct adt7420_dev *dev, uint16_t register_address,
			  uint32_t data);

/* Write to I2C register */
int adt7420_i2c_reg_write(struct adt7420_dev *dev, uint16_t register_address,
			  uint32_t data);

/*Sets register value*/
int adt7420_reg_write(struct adt7420_dev *dev,
		      uint16_t register_address,
		      uint32_t data);

/*! Initializes the comm. peripheral and checks if the device is present. */
int32_t adt7420_init(struct adt7420_dev **device,
		     struct adt7420_init_param init_param);

/* Free the resources allocated by adt7420_init(). */
int32_t adt7420_remove(struct adt7420_dev *dev);

/*! Sets the operational mode for ADT7420. */
int adt7420_set_operation_mode(struct adt7420_dev *dev,
			       uint8_t mode);

/*! Sets the resolution for ADT7420. */
int adt7420_set_resolution(struct adt7420_dev *dev,
			   uint8_t resolution);

/*! Resets the SPI or I2C inteface for the ADT7420/ADT7320*/
int32_t adt7420_reset(struct adt7420_dev *dev);

/*! Reads the temperature data and converts it to Celsius degrees. */
float adt7420_get_temperature(struct adt7420_dev *dev);

/*! Read the register value for I2C interface devices */
int adt7420_i2c_reg_read(struct adt7420_dev *dev,
			 uint16_t register_address, uint16_t *data);

/*! Read the register value for SPI interface devices */
int adt7420_spi_reg_read(struct adt7420_dev *dev,
			 uint16_t register_address, uint16_t *data);

/*! Check if the interface of the selected device is SPI */
bool adt7420_is_spi(struct adt7420_dev *dev);

#endif	/* __ADT7420_H__ */
