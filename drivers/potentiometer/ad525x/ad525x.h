/**************************************************************************//**
*   @file   ad525x.h
*   @brief  Header file of AD525X Driver for Microblaze processor. This driver
*           supporting the following devices : AD5232, AD5235, AD5252, AD5251,
*           AD5254, AD5253, ADN2850
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
#ifndef __AD525X_H__
#define __AD525X_H__

/*****************************************************************************/
/****************************** Include Files ********************************/
/*****************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"

/*****************************************************************************/
/*  Device specific MACROs                                                   */
/*****************************************************************************/
/* GPIOs */
#define AD525X_RESET_OUT                    no_os_gpio_direction_output(dev->gpio_reset,    \
			                    NO_OS_GPIO_HIGH)
#define AD525X_RESET_LOW                    no_os_gpio_set_value(dev->gpio_reset,           \
			                    NO_OS_GPIO_LOW)
#define AD525X_RESET_HIGH                   no_os_gpio_set_value(dev->gpio_reset,           \
			                    NO_OS_GPIO_HIGH)
#define AD525X_SHUTDOWN_OUT                 no_os_gpio_direction_output(dev->gpio_shutdown, \
			                    NO_OS_GPIO_HIGH)
#define AD525X_SHUTDOWN_LOW                 no_os_gpio_set_value(dev->gpio_shutdown,        \
			                    NO_OS_GPIO_LOW)
#define AD525X_SHUTDOWN_HIGH                no_os_gpio_set_value(dev->gpio_shutdown,        \
			                    NO_OS_GPIO_HIGH)
#define AD525X_READY_IN                     no_os_gpio_direction_input(dev->gpio_ready)
#define AD525X_READY_LOW                    no_os_gpio_set_value(dev->gpio_ready,           \
			                    NO_OS_GPIO_LOW)
#define AD525X_READY_HIGH                   no_os_gpio_set_value(dev->gpio_ready,           \
			                    NO_OS_GPIO_HIGH)
#define AD525X_WP_BF_OUT                    no_os_gpio_direction_output(dev->gpio_wpbf,     \
			                    NO_OS_GPIO_HIGH)
#define AD525X_WP_BF_LOW                    no_os_gpio_set_value(dev->gpio_wpbf,            \
			                    NO_OS_GPIO_LOW)
#define AD525X_WP_BF_HIGH                   no_os_gpio_set_value(dev->gpio_wpbf,            \
			                    NO_OS_GPIO_HIGH)

/* Data word masks */
#define AD525X_MEM_ADDR_MASK                0xF
#define AD525X_RDAC_ADDR_MASK_1BIT          0x1
#define AD525X_RDAC_ADDR_MASK_3BIT          0x7
#define AD525X_DATA8_MASK                   0xFF
#define AD525X_DATA10_MASK                  0x3FF

/*********************** Command definitions *********************************/
#define AD525X_CMD_NOP                      0x0
#define AD525X_CMD_MEM2RDAC                 0x1
#define AD525X_CMD_RDAC2MEM                 0x2
#define AD525X_CMD_DECRDAC_6DB              0x3
#define AD525X_CMD_DECALLRDAC_6DB           0x4
#define AD525X_CMD_DECRDAC_ONE              0x5
#define AD525X_CMD_DECALLRDAC_ONE           0x6
#define AD525X_CMD_RESET                    0x7
#define AD525X_CMD_INCRDAC_6DB              0x8
#define AD525X_CMD_INCALLRDAC_6DB           0x9
#define AD525X_CMD_INCRDAC_ONE              0xA
#define AD525X_CMD_INCALLRDAC_ONE           0xB
/*****************************************************************************/

/**************************** SPI specific macros ****************************/
#define AD525X_CMD_SPI_OFFSET               0x4
#define AD525X_CMD_MASK                     0xF
/* SPI Read/Write commands */
#define AD525X_CMD_SPI_SREG2MEM             0x3
#define AD525X_CMD_SPI_MEM2SREG             0x9 // Result in the next frame
#define AD525X_CMD_SPI_RDAC2SREG            0xA // Result in the next frame
#define AD525X_CMD_SPI_SREG2RDAC            0xB

/*************************** I2C specific macros *****************************/
/* I2C device address */
#define AD525X_I2C_HARD_ADDR                0x2C
/* Package pin-programmable address bits */
#define AD525X_I2C_PIN_ADDR_MASK            0x03
/* Mask for I2C Control Bits */
#define AD525X_I2C_CNTR_MASK                0xBF
/* Access Types */
#define AD525X_I2C_CMD_OR_REG               0x80
#define AD525X_I2C_EE_OR_RDAC               0x20
/* Address mask for EEMEM addresses, one bit wider than at devices with SPI */
#define AD525X_I2C_MEM_ADDR_MASK            0x1F
/* The offset of the Command in the I2C word */
#define AD525X_CMD_I2C_OFFSET               0x3

/*************************** Reading tolerance addresses *********************/
/* Note: The valid tolerance addresses varies on different devices. Check the data sheet for further information */
#define AD525x_RDAC_TOLERANCE               0x0F
#define AD525X_RDAC0_SIGN_TOL               0x18
#define AD525X_RDAC0_DECIMAL_TOL            0x19
#define AD525X_RDAC1_SIGN_TOL               0x1A
#define AD525X_RDAC1_DECIMAL_TOL            0x1B
#define AD525X_RDAC2_SIGN_TOL               0x1C
#define AD525X_RDAC2_DECIMAL_TOL            0x1D
#define AD525X_RDAC3_SIGN_TOL               0x1E
#define AD525X_RDAC4_DECIMAL_TOL            0x1F

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Custom boolean type */
enum bool_t {
	false,
	true
};

/* Supported devices */
enum ad525_x_type_t {
	ID_AD5232,
	ID_AD5235,
	ID_ADN2850,
	ID_AD5252,
	ID_AD5251,
	ID_AD5254,
	ID_AD5253,
};

/* Communication types */
enum comm_type_t {
	SPI,
	I2C
};

struct ad525x_chip_info {
	uint8_t num_channels;
	enum comm_type_t comm_type;
	uint16_t num_position;
};

struct ad525x_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	struct no_os_gpio_desc	*gpio_shutdown;
	struct no_os_gpio_desc	*gpio_ready;
	struct no_os_gpio_desc	*gpio_wpbf;
	/* Device Settings */
	enum ad525_x_type_t	this_device;
};

struct ad525x_init_param {
	/* I2C */
	struct no_os_i2c_init_param	i2c_init;
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_reset;
	struct no_os_gpio_init_param	gpio_shutdown;
	struct no_os_gpio_init_param	gpio_ready;
	struct no_os_gpio_init_param	gpio_wpbf;
	/* Device Settings */
	enum ad525_x_type_t	this_device;
};

/*****************************************************************************/
/*  Functions Prototypes                                                     */
/*****************************************************************************/
/* Initialize the communication with the device */
int8_t ad525x_init(struct ad525x_dev **device,
		   struct ad525x_init_param init_param);

/* Free the resources allocated by ad525x_init(). */
int32_t ad525x_remove(struct ad525x_dev *dev);

/* Read data from the EEMEM */
uint16_t ad525x_read_mem(struct ad525x_dev *dev,
			 uint8_t address);

/* Write data to EEMEM */
void ad525x_write_mem(struct ad525x_dev *dev,
		      uint8_t address,
		      uint16_t data);

/* Read the value of the RDAC register */
uint16_t ad525x_read_rdac(struct ad525x_dev *dev,
			  uint8_t address);

/* Write the value of the RDAC register */
void ad525x_write_rdac(struct ad525x_dev *dev,
		       uint8_t address,
		       uint16_t data);

/* Write quick commands to the device */
void ad525x_write_command(struct ad525x_dev *dev,
			  uint8_t command,
			  uint8_t address);

#endif // __AD525X_H__
