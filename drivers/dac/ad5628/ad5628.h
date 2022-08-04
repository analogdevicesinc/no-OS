/***************************************************************************//**
 *   @file   AD5628.h
 *   @brief  Header file of AD5628 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
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

#ifndef __AD5628_H__
#define __AD5628_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/******************************** AD5628 **************************************/
/******************************************************************************/
/* AD5628 Input Register */
#define AD5628_CMD(x)              ((0x000F & (int32_t)(x)) << 24)
#define AD5628_ADDR(x)             ((0x000F & (int32_t)(x)) << 20)
#define AD5628_DATA_BITS(x)        ((0x0FFF & (int32_t)(x)) << 8)

/* Command Definitions (AD5628_COMMAND(x) options) */
#define AD5628_CMD_WRITE_INPUT_N             0 // Write to Input Register n.
#define AD5628_CMD_UPDATE_DAC_N              1 // Update DAC Register n.
#define AD5628_CMD_WRITE_INPUT_N_UPDATE_ALL  2 // Write to Input Register n, update all.
#define AD5628_CMD_WRITE_INPUT_N_UPDATE_N    3 // Write to and update DAC Channel n.
#define AD5628_CMD_POWERDOWN                 4 // Power down/power up DAC.
#define AD5628_CMD_LOAD_CLEAR_CODE           5 // Load clear code register.
#define AD5628_CMD_LOAD_LDAC_REG             6 // Load LDAC register.
#define AD5628_CMD_RESET                     7 // Reset (power-on reset)
#define AD5628_CMD_SET_INT_REF               8 // Set up internal REF register.

/* Address Commands (AD5628_ADDRESS(x) options) */
#define AD5628_ADDR_DAC_A      0x00 //DAC A
#define AD5628_ADDR_DAC_B      0x01 //DAC B
#define AD5628_ADDR_DAC_C      0x02 //DAC C
#define AD5628_ADDR_DAC_D      0x03 //DAC D
#define AD5628_ADDR_DAC_E      0x04 //DAC E
#define AD5628_ADDR_DAC_F      0x05 //DAC F
#define AD5628_ADDR_DAC_G      0x06 //DAC G
#define AD5628_ADDR_DAC_H      0x07 //DAC H
#define AD5628_ADDR_DAC_ALL    0x0F //All DACs

/* Internal Reference Register */
#define AD5628_INT_REF_OFF     0
#define AD5628_INT_REF_ON      1

/* Power-Down Modes of Operation */
#define AD5628_POWER_MODE(x)      ((0x03 & (uint16_t) (x)) << 8)

#define AD5628_PWRDN_NONE         0 // Normal operation
#define AD5628_PWRDN_1K           1 // 1 KOhm to GND    (Power-down mode)
#define AD5628_PWRDN_100K         2 // 100 KOhm to GND  (Power-down mode)
#define AD5628_PWRDN_3STATE       3 // Three-state      (Power-down mode)

/* Clear Code Function */
#define AD5628_CODE_0X0000        0
#define AD5628_CODE_0X8000        1
#define AD5628_CODE_0XFFFF        2
#define AD5628_CODE_NOP           3

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad5628_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

struct ad5628_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Resets the device and performs several initializations. */
int32_t ad5628_init(struct ad5628_dev **device,
		    struct ad5628_init_param init_param);

/*! Free the resources allocated by AD5628_Init(). */
int32_t ad5628_remove(struct ad5628_dev *dev);

/*! Sets the device in a specific power mode. */
void ad5628_power_mode(struct ad5628_dev *dev,
		       uint8_t pwr_mode,
		       uint8_t channel);

/*! Resets the device. */
void ad5628_reset(struct ad5628_dev *dev);

/*! Writes a 32-bit data-word to the Input Register of the device. */
void ad5628_set_input_register(struct ad5628_dev *dev,
			       uint32_t register_value);

#endif /* __AD5628_H__ */
