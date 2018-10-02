/***************************************************************************//**
 *   @file   ad5683.h
 *   @brief  AD4110 Driver header file.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

#ifndef _AD5683_H_
#define _AD5683_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define FAILURE                    -1
#define AD5683_CMD_NOP             0  // No operation.
#define AD5683_CMD_WR_IN_REG       1  // Write Input Register.
#define AD5683_CMD_UPDATE_DAC      2  // Update DAC Register.
#define AD5683_CMD_WR_DAC_REG      3  // Write DAC and Input Register.
#define AD5683_CMD_WR_CTRL_REG     4  // Write Control Register.
#define AD5683_CMD_RB_IN_REG       5  // Readback input register.

#define MAX_RESOLUTION             16 // Maximum resolution of the supported devices

/* Power-down operation modes masks */
#define AD5683_PWRM_NORMAL         0	// Power Mode Normal
#define AD5683_PWRM_1K             1	// Power-down mode 1kOhm to GND
#define AD5683_PWRM_100K           2	// Power-down mode 100kOhm to GND
#define AD5683_PWRM_THREESTATE     3	// Three-State

#define AD5683_DC_EN               1
#define AD5683_DC_DIS              0

#define AD5683_INTREF_EN           1
#define AD5683_INTREF_DIS          0

#define AD5683_RB_EN               1
#define AD5683_RB_DIS              0

#define AD5683_RB_EN               1
#define AD5683_RB_DIS              0

#define AD5683_GB_2VREF            1
#define AD5683_GB_VREF             0

#define AD5683_SW_RESET_EN         1

#define PKT_LENGTH                 3       // SPI packet length in byte
#define CMD_MASK                   0xF     // Mask for Command bits
#define CMD_OFFSET                 4       // Offset for Command bits
#define MSB_MASK                   0xF     // Mask for MSB
#define MSB_OFFSET                 12	   // Offset for MSB
#define MIDB_MASK                  0xFF	   // Mask for middle bits
#define MIDB_OFFSET                4	   // Offset for middle bits
#define LSB_MASK                   0xF     // Mask for LSB
#define LSB_OFFSET                 4       // Offset for MSB
#define RB_DATA_MASK               0xFFFF0 // Mask for read back bits

/*************************** Write Control Register Bits **********************/

#define AD5683_CTRL_DCEN(x)        (((((x) & 0x1) << 0) << 10) & 0xFC00)
#define AD5683_CTRL_GM(x)          (((((x) & 0x1) << 1) << 10) & 0xFC00)
#define AD5683_CTRL_INT_REF(x)     (((((x) & 0x1) << 2) << 10) & 0xFC00)
#define AD5683_CTRL_PWRM(x)        (((((x) & 0x3) << 3) << 10) & 0xFC00)
#define AD5683_SW_RESET(x)         (((((x) & 0x1) << 5) << 10) & 0xFC00)

/******************************************************************************/
/**************************** Variable Declarations ***************************/
/******************************************************************************/

/* Supported devices */
enum ad5683_type {
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

struct ad5683_chip_info {
	uint8_t		resolution;
	enum comm_type	communication;
};

struct ad5683_dev {
	/* I2C */
	i2c_desc	*i2c_desc;
	/* SPI */
	spi_desc	*spi_desc;
	/* GPIO */
	gpio_desc	*gpio_reset;
	gpio_desc	*gpio_ldac;
	/* Device Settings */
	enum ad5683_type	act_device;
};

struct ad5683_init_param {
	/* I2C */
	i2c_init_param	i2c_init;
	/* SPI */
	spi_init_param	spi_init;
	/* GPIO */
	int8_t		gpio_reset;
	int8_t		gpio_ldac;
	/* Device Settings */
	enum ad5683_type	act_device;
};

/******************************************************************************/
/**************************** Function Declarations ***************************/
/******************************************************************************/

/* Initialize SPI and Initial Values for AD5683 Board. */
int32_t ad5683_init(struct ad5683_dev **device,
		    struct ad5683_init_param init_param);

/* Free the resources allocated by ad5683_init(). */
int32_t ad5683_remove(struct ad5683_dev *dev);

/* Write to input shift register */
uint32_t ad5683_set_shift_reg(struct ad5683_dev *dev, uint8_t command,
			      uint16_t data);

/* Write to Input Register */
int32_t ad5683_write_register(struct ad5683_dev *dev, uint16_t data);

/* Update DAC Register with contents of Input Register */
int32_t ad5683_update_register(struct ad5683_dev *dev);

/* Write DAC and Input Register */
int32_t ad5683_write_dac_input_register(struct ad5683_dev *dev, uint16_t data);

/* Read back Input Register */
uint32_t ad5683_read_back_register(struct ad5683_dev *dev);

/* Set Power-down mode. */
int32_t ad5683_power_mode(struct ad5683_dev *dev, uint8_t mode);

/* Software reset  */
int32_t ad5683_software_reset(struct ad5683_dev *dev);

/* Set Internal reference */
int32_t ad5683_internal_reference(struct ad5683_dev *dev, uint8_t value);

/* Set DCEN (daisy-chain enable) */
int32_t ad5683_daisy_chain_en(struct ad5683_dev *dev, uint8_t value);

/* Set Gain mode */
int32_t ad5683_gain_mode(struct ad5683_dev *dev, uint8_t value);

#endif /* _AD5683_H_ */
