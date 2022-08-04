/***************************************************************************//**
*   @file   ad5686.h
*   @brief  Header file of AD5686 Driver. This driver supporting the following
*              devices: AD5684R, AD5685R, AD5686R, AD5694R, AD5695R, AD5696R,
*
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
* Copyright 2013, 2020(c) Analog Devices, Inc.
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
*
*******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"

/* Control Bits */
#define AD5686_CTRL_NOP          0
#define AD5686_CTRL_WRITE        1
#define AD5686_CTRL_UPDATE       2
#define AD5686_CTRL_WRITEUPDATE  3
#define AD5686_CTRL_PWR          4
#define AD5686_CTRL_LDAC_MASK    5
#define AD5686_CTRL_SWRESET      6
#define AD5686_CTRL_IREF_REG     7
#define AD5686_CTRL_DCEN         8
#define AD5686_CTRL_RB_REG       9

#define AD5683_CMD_WR_CTRL_REG   4
#define AD5683_CTRL_RB_REG       5

/* Power-down operation modes masks */
#define AD5686_PWRM_NORMAL       0
#define AD5686_PWRM_1K           1
#define AD5686_PWRM_100K         2
#define AD5686_PWRM_THREESTATE   3

#define AD5686_PWRM_MASK         3

/* Enable/disable defines */
#define AD5686_INTREF_EN        1
#define AD5686_INTREF_DIS       0
#define AD5686_DC_EN            1
#define AD5686_DC_DIS           0
#define AD5686_RB_EN            1
#define AD5686_RB_DIS           0

#define MAX_RESOLUTION  16     // Maximum resolution of the supported devices

#define PKT_LENGTH               3      // SPI packet length in byte

#define ADDR_MASK                0xFF   // Mask for Address bits
#define CMD_OFFSET               4      // Offset for Command

#define AD5686_CMD_MASK          0xFF
#define AD5686_MSB_MASK          0xFF00 // Most significant byte of the data word
#define AD5686_MSB_OFFSET        8
#define AD5686_LSB_MASK          0x00FF // Least significant byte of the data word
#define AD5686_LSB_OFFSET        0

#define AD5683_MIDB_OFFSET       4	   // Offset for middle bits
#define AD5683_MIDB_MASK         0xFF
#define AD5683_MSB_OFFSET        12
#define AD5683_MSB_MASK          0xF
#define AD5683_CMD_MASK          0xF
#define AD5683_LSB_MASK          0xF
#define AD5683_LSB_OFFSET        4

#define AD5683_REG_MAP           0
#define AD5686_REG_MAP           1

/********************** AD5683 Write Control Register Bits ********************/

#define AD5683_CTRL_DCEN(x)      (((((x) & 0x1) << 0) << 10) & 0xFC00)
#define AD5683_CTRL_GM(x)        (((((x) & 0x1) << 1) << 10) & 0xFC00)
#define AD5683_CTRL_INT_REF(x)   (((((x) & 0x1) << 2) << 10) & 0xFC00)
#define AD5683_CTRL_PWRM(x)      (((((x) & 0x3) << 3) << 10) & 0xFC00)
#define AD5683_SW_RESET          ((((0x1) << 5) << 10) & 0xFC00)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Supported devices */
enum ad5686_type {
	ID_AD5671R,
	ID_AD5672R,
	ID_AD5673R,
	ID_AD5674,
	ID_AD5674R,
	ID_AD5675R,
	ID_AD5676,
	ID_AD5676R,
	ID_AD5677R,
	ID_AD5679,
	ID_AD5679R,
	ID_AD5686,
	ID_AD5684R,
	ID_AD5685R,
	ID_AD5686R,
	ID_AD5687,
	ID_AD5687R,
	ID_AD5689,
	ID_AD5689R,
	ID_AD5697R,
	ID_AD5694,
	ID_AD5694R,
	ID_AD5695R,
	ID_AD5696,
	ID_AD5696R,
	ID_AD5681R,
	ID_AD5682R,
	ID_AD5683R,
	ID_AD5683,
	ID_AD5691R,
	ID_AD5692R,
	ID_AD5693R,
	ID_AD5693
};

enum comm_type {
	SPI,
	I2C,
};

enum ad5686_dac_channels {
	AD5686_CH_0 = 0,
	AD5686_CH_1,
	AD5686_CH_2,
	AD5686_CH_3,
	AD5686_CH_4,
	AD5686_CH_5,
	AD5686_CH_6,
	AD5686_CH_7,
	AD5686_CH_8,
	AD5686_CH_9,
	AD5686_CH_10,
	AD5686_CH_11,
	AD5686_CH_12,
	AD5686_CH_13,
	AD5686_CH_14,
	AD5686_CH_15,
};

struct ad5686_chip_info {
	uint8_t		resolution;
	uint8_t		register_map;
	enum comm_type	communication;
	const uint32_t *channel_addr;
};

struct ad5686_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	struct no_os_gpio_desc	*gpio_ldac;
	struct no_os_gpio_desc	*gpio_gain;
	/* Device Settings */
	enum ad5686_type	act_device;
	uint32_t power_down_mask;
	uint32_t ldac_mask;
};

struct ad5686_init_param {
	/* I2C */
	struct no_os_i2c_init_param	i2c_init;
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_reset;
	struct no_os_gpio_init_param	gpio_ldac;
	struct no_os_gpio_init_param	gpio_gain;
	/* Device Settings */
	enum ad5686_type	act_device;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize SPI and Initial Values for AD5686 Board. */
int32_t ad5686_init(struct ad5686_dev **device,
		    struct ad5686_init_param init_param);

/* Free the resources allocated by ad5686_init(). */
int32_t ad5686_remove(struct ad5686_dev *dev);

/* Write to input register */
uint16_t ad5686_set_shift_reg(struct ad5686_dev *dev,
			      uint8_t command,
			      uint8_t address,
			      uint16_t data);

/* Write to Input Register n (dependent on LDAC) */
void ad5686_write_register(struct ad5686_dev *dev,
			   enum ad5686_dac_channels channel,
			   uint16_t data);

/* Update DAC Register n with contents of Input Register n */
void ad5686_update_register(struct ad5686_dev *dev,
			    enum ad5686_dac_channels channel);

/* Write to and update DAC channel n */
void ad5686_write_update_register(struct ad5686_dev *dev,
				  enum ad5686_dac_channels channel,
				  uint16_t data);

/* Read back Input Register n */
uint16_t ad5686_read_back_register(struct ad5686_dev *dev,
				   enum ad5686_dac_channels channel);

/* Power down / power up DAC */
void ad5686_power_mode(struct ad5686_dev *dev,
		       enum ad5686_dac_channels channel,
		       uint8_t mode);

/* Set up LDAC mask register */
void ad5686_ldac_mask(struct ad5686_dev *dev,
		      enum ad5686_dac_channels channel,
		      uint8_t enable);

/* Software reset (power-on reset) */
void ad5686_software_reset(struct ad5686_dev *dev);

/* Write to Internal reference setup register */
void ad5686_internal_reference(struct ad5686_dev *dev,
			       uint8_t value);

/* Set up DCEN register (daisy-chain enable) */
void ad5686_daisy_chain_en(struct ad5686_dev *dev,
			   uint8_t value);

/* Set up readback register (readback enable) */
void ad5686_read_back_en(struct ad5686_dev *dev,
			 uint8_t value);

/* Set Gain mode */
int32_t ad5686_gain_mode(struct ad5686_dev *dev, uint8_t value);
