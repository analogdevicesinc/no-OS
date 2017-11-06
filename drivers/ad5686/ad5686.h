/***************************************************************************//**
*   @file   ad5686.h
*   @brief  Header file of AD5686 Driver. This driver supporting the following
*              devices: AD5684R, AD5685R, AD5686R, AD5694R, AD5695R, AD5696R,
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
*
*******************************************************************************/

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

/* AD5686 channels */
#define AD5686_CH_A              1
#define AD5686_CH_B              2
#define AD5686_CH_C              4
#define AD5686_CH_D              8
#define AD5686_CH_ALL            0xF

/* Power-down operation modes masks */
#define AD5686_PWRM_NORMAL       0
#define AD5686_PWRM_1K           1
#define AD5686_PWRM_100K         2
#define AD5686_PWRM_THREESTATE   3

#define AD5686_PWRM_MASK         3

/* Power-down operation modes offset for each channel */
#define AD5686_PWRM_CHA_OFFSET   0
#define AD5686_PWRM_CHB_OFFSET   2
#define AD5686_PWRM_CHC_OFFSET   4
#define AD5686_PWRM_CHD_OFFSET   6

/* Enable/disable defines */
#define AD5686_INTREF_EN        1
#define AD5686_INTREF_DIS       0
#define AD5686_DC_EN            1
#define AD5686_DC_DIS           0
#define AD5686_RB_EN            1
#define AD5686_RB_DIS           0

#define MAX_RESOLUTION  16     // Maximum resolution of the supported devices

#define CMD_MASK        0xFF   // Mask for Command bits
#define ADDR_MASK       0xFF   // Mask for Address bits
#define CMD_OFFSET      4      // Offset for Command
#define MSB_MASK        0xFF00 // Most significant byte of the data word
#define MSB_OFFSET      8
#define LSB_MASK        0x00FF // Least significant byte of the data word
#define LSB_OFFSET      0
#define PKT_LENGTH      3      // SPI packet length in byte

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Supported devices */
typedef enum {
	ID_AD5684R,
	ID_AD5685R,
	ID_AD5686R,
	ID_AD5694R,
	ID_AD5695R,
	ID_AD5696R,
} ad5686_type;

typedef enum {
	SPI,
	I2C,
} comm_type;

typedef struct {
	uint8_t		resolution;
	comm_type	communication;
} ad5686_chip_info;

typedef struct {
	/* I2C */
	i2c_device	i2c_dev;
	/* SPI */
	spi_device	spi_dev;
	/* GPIO */
	gpio_device	gpio_dev;
	int8_t		gpio_reset;
	int8_t		gpio_ldac;
	/* Device Settings */
	ad5686_type	act_device;
} ad5686_dev;

typedef struct {
	/* I2C */
	i2c_type	i2c_type;
	uint32_t	i2c_id;
	uint32_t	i2c_max_speed_hz;
	uint8_t	i2c_slave_address;
	/* SPI */
	spi_type	spi_type;
	uint32_t	spi_id;
	uint32_t	spi_max_speed_hz;
	spi_mode	spi_mode;
	uint8_t		spi_chip_select;
	/* GPIO */
	gpio_type	gpio_type;
	uint32_t	gpio_id;
	int8_t		gpio_reset;
	int8_t		gpio_ldac;
	/* Device Settings */
	ad5686_type	act_device;
} ad5686_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize SPI and Initial Values for AD5449 Board. */
int32_t ad5686_init(ad5686_dev **device,
		    ad5686_init_param init_param);

/* Write to input register */
uint16_t ad5686_set_shift_reg(ad5686_dev *dev,
			      uint8_t command,
			      uint8_t address,
			      uint16_t data);

/* Write to Input Register n (dependent on LDAC) */
void ad5686_write_register(ad5686_dev *dev,
			   uint8_t address,
			   uint16_t data);

/* Update DAC Register n with contents of Input Register n */
void ad5686_update_register(ad5686_dev *dev,
			    uint8_t address);

/* Write to and update DAC channel n */
void ad5686_write_update_register(ad5686_dev *dev,
				  uint8_t address,
				  uint16_t data);

/* Read back Input Register n */
uint16_t ad5686_read_back_register(ad5686_dev *dev,
				   uint8_t address);

/* Power down / power up DAC */
void ad5686_power_mode(ad5686_dev *dev,
		       uint8_t address,
		       uint8_t mode);

/* Set up LDAC mask register */
void ad5686_ldac_mask(ad5686_dev *dev,
		      uint8_t ldac_mask);

/* Software reset (power-on reset) */
void ad5686_software_reset(ad5686_dev *dev);

/* Write to Internal reference setup register */
void ad5686_internal_reference(ad5686_dev *dev,
			       uint8_t value);

/* Set up DCEN register (daisy-chain enable) */
void ad5686_daisy_chain_en(ad5686_dev *dev,
			   uint8_t value);

/* Set up readback register (readback enable) */
void ad5686_read_back_en(ad5686_dev *dev,
			 uint8_t value);
