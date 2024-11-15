/**************************************************************************//**
*   @file   ad5110.h
*   @brief  Header file of ad5110 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
