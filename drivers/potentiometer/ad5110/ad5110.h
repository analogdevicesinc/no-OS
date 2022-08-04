/**************************************************************************//**
*   @file   ad5110.h
*   @brief  Header file of ad5110 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
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
#ifndef _ad5110_H_
#define _ad5110_H_

/*****************************************************************************/
/****************************** Include Files ********************************/
/*****************************************************************************/
#include <stdint.h>
#include "no_os_i2c.h"

/******************************************************************************/
/************************* Input shift register *******************************/
/******************************************************************************/

/*!< Command position in transmitted bytes */
#define COMMAND                 8
/*!< Available Commands */
#define CMD_NOP                 0
#define CMD_WR_RDAC_EEPROM      1
#define CMD_WR_RDAC             2
#define CMD_SHUT_DOWN           3
#define CMD_RESET               4
#define CMD_RD_RDAC             5
#define CMD_RD_EEPROM           6
/*!< Shutdown modes */
#define SHUT_DOWN_OFF           0
#define SHUT_DOWN_ON            1
/*!< Read modes */
#define WIPER_POSITION          0
#define RESISTOR_TOLERANCE      1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct ad5110_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* Device Settings */
	uint8_t ad5110_dev_addr;
};

struct ad5110_init_param {
	/* I2C */
	struct no_os_i2c_init_param	i2c_init;
	/* Device Settings */
	uint8_t ad5110_dev_addr;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*!< Initializes the communication with the device. */
int8_t ad5110_init(struct ad5110_dev **device,
		   struct ad5110_init_param init_param);

/*!< Free the resources allocated by ad5110_init(). */
int32_t ad5110_remove(struct ad5110_dev *dev);

/*!< Write the content of serial register data to RDAC. */
void ad5110_write_rdac(struct ad5110_dev *dev,
		       uint8_t rdac_value);

/*!< Read the content of RDAC register. */
uint8_t ad5110_read_rdac(struct ad5110_dev *dev);

/*!< Write the content of RDAC register to EEPROM. */
void ad5110_write_rdac_eeprom(struct ad5110_dev *dev);

/*!< Read wiper position from EEPROM. */
uint8_t ad5110_read_wiper(struct ad5110_dev *dev);

/*!< Read resistor tolerance from EEPROM. */
uint8_t ad5110_read_res_tolerance(struct ad5110_dev *dev);

/*!< Software reset; makes a refresh of RDAC register with EEPROM. */
void ad5110_reset(struct ad5110_dev *dev);

/*!< Software shutdown. */
void ad5110_shut_down(struct ad5110_dev *dev,
		      uint8_t value);

#endif  // _ad5110_H_
