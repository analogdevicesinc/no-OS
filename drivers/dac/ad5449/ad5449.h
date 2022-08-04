/***************************************************************************//**
*   @file   AD5449.h
*   @brief  Header file of AD5449 Driver. This driver supporting the following
*              devices: AD5415, AD5443, AD5432, AD5426, AD5429, AD5439, AD5449
*
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum bool_t {
	false,
	true
};

/* Supported devices */
enum ad5449_type_t {
	ID_AD5415,
	ID_AD5426,
	ID_AD5429,
	ID_AD5432,
	ID_AD5439,
	ID_AD5443,
	ID_AD5449,
};

struct ad5449_chip_info {
	uint8_t num_channels;
	uint8_t resolution;
	enum bool_t has_ctrl;
};

struct ad5449_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_ldac;
	struct no_os_gpio_desc	*gpio_clr;
	/* Device Settings */
	enum ad5449_type_t		act_device;
	uint16_t		control_reg;
};

struct ad5449_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_ldac;
	struct no_os_gpio_init_param	gpio_clr;
	/* Device Settings */
	enum ad5449_type_t	act_device;
};

/* Control Bits */
#define AD5449_CTRL_NOP             0
#define AD5449_CTRL_LOADUPDATE(x)   (1 + 3 * (x))
#define AD5449_CTRL_READBACK(x)     (2 + 3 * (x))
#define AD5449_CTRL_LOAD(x)         (3 + 3 * (x))
#define AD5449_CTRL_UPDATEALL       7
#define AD5449_CTRL_LOADALL         8
#define AD5449_CTRL_DAISY_CHAIN     9
#define AD5449_CTRL_CLK_EDGE        10
#define AD5449_CTRL_CLR_ZERO        11
#define AD5449_CTRL_CLR_MID         12
#define AD5449_CTRL_REG             13

/* AD5449 channels */
#define AD5449_CH_A                 0
#define AD5449_CH_B                 1

/* Clear target scales */
#define AD5449_ZERO_SCALE            0
#define AD5449_MID_SCALE             1

/* Active clock edge */
#define AD5449_CLOCK_NEGEDGE         0
#define AD5449_CLOCK_POSEDGE         1

/* Daisy-Chain Control */
#define AD5449_DAISY_CHAIN_DIS       0
#define AD5449_DAISY_CHAIN_EN        1

/* AD5449_CTRL_REG definition */
#define AD5449_SDO_MASK          (3 << 10)
#define AD5449_DSY_MASK          (1 << 9)
#define AD5449_HCLR_MASK         (1 << 8)
#define AD5449_SCLK_MASK         (1 << 7)
#define AD5449_SDO_BIT           10
#define AD5449_DSY_BIT           9
#define AD5449_HCLR_BIT          8
#define AD5449_SCLK_BIT          7

/* AD5449 GPIO */
#define AD5449_LDAC_OUT             no_os_gpio_direction_output(dev->gpio_ldac,   \
			            NO_OS_GPIO_HIGH)
#define AD5449_LDAC_LOW             no_os_gpio_set_value(dev->gpio_ldac,          \
			            NO_OS_GPIO_LOW)
#define AD5449_LDAC_HIGH            no_os_gpio_set_value(dev->gpio_ldac,          \
			            NO_OS_GPIO_HIGH)

#define AD5449_CLR_OUT              no_os_gpio_direction_output(dev->gpio_clr,   \
			            NO_OS_GPIO_HIGH)
#define AD5449_CLR_LOW              no_os_gpio_set_value(dev->gpio_clr,          \
			            NO_OS_GPIO_LOW)
#define AD5449_CLR_HIGH             no_os_gpio_set_value(dev->gpio_clr,          \
			            NO_OS_GPIO_HIGH)

/* SDO Control Bits */
#define AD5449_SDO_FULL             0
#define AD5449_SDO_WEAK             1
#define AD5449_SDO_OPEN_DRAIN       2
#define AD5449_SDO_DISABLE          3

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize SPI and Initial Values for AD5449 Board. */
int8_t ad5449_init(struct ad5449_dev **device,
		   struct ad5449_init_param init_param);

/* Free the resources allocated by AD5449_Init(). */
int32_t ad5449_remove(struct ad5449_dev *dev);

/* Write to shift register via SPI. */
uint16_t ad5449_set_input_shift_reg(struct ad5449_dev *dev,
				    uint16_t command,
				    uint16_t data);

/* Load and updates the selected DAC with a given value. */
void ad5449_load_update_channel(struct ad5449_dev *dev,
				uint8_t channel,
				uint16_t dac_value);

/* Load selected DAC input register with a given value. */
void ad5449_load_channel(struct ad5449_dev *dev,
			 uint8_t channel,
			 uint16_t dac_value);

/* Read from the selected DAC register. */
uint16_t ad5449_readback_channel(struct ad5449_dev *dev,
				 uint8_t channel);

/* Update the DAC outputs (all channels). */
void ad5449_update_all(struct ad5449_dev *dev);

/* Load the DAC input registers. */
void ad5449_load_all(struct ad5449_dev *dev,
		     int16_t dac_value);

/* Set up the scale where to the output will be cleared on active CLR signal */
void ad5449_clear_scale_setup(struct ad5449_dev *dev,
			      int8_t type);

/* Enable/disable the Daisy-Chain mode */
void ad5449_daisy_chain_setup(struct ad5449_dev *dev,
			      int8_t value);

/* Control the SDO output driver strength */
void ad5449_sdocontrol(struct ad5449_dev *dev,
		       int8_t control_bits);

/* Set up the active clock edge of the SPI interface */
void ad5449_sclksetup(struct ad5449_dev *dev,
		      int8_t value);
