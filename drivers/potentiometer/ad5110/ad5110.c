/**************************************************************************//**
*   @file   AD5110.c
*   @brief  Implementation of AD5110 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad5110.h"
#include "no_os_alloc.h"

/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
int8_t ad5110_init(struct ad5110_dev **device,
		   struct ad5110_init_param init_param)
{
	struct ad5110_dev *dev;
	int8_t status;

	dev = (struct ad5110_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->ad5110_dev_addr = init_param.ad5110_dev_addr;
	status = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5110_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad5110_remove(struct ad5110_dev *dev)
{
	int32_t ret;

	ret = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Write the content of serial register data to RDAC.
 *
 * @param dev        - The device structure.
 * @param rdac_value - Value of the serial register.
 *
 * @return none.
*******************************************************************************/
void ad5110_write_rdac(struct ad5110_dev *dev,
		       uint8_t rdac_value)
{
	uint8_t data_buffer[2];

	data_buffer[0] = CMD_WR_RDAC;
	data_buffer[1] = rdac_value;
	no_os_i2c_write(dev->i2c_desc,
			data_buffer,
			2,
			1);
}

/***************************************************************************//**
 * @brief Read the content of RDAC register.
 *
 * @param dev - The device structure.
 *
 * @return rdacValue - Value read from register.
*******************************************************************************/
uint8_t ad5110_read_rdac(struct ad5110_dev *dev)
{
	uint8_t data_buffer[2];


	data_buffer[0] = CMD_RD_RDAC;
	data_buffer[1] = 0;
	no_os_i2c_write(dev->i2c_desc,
			data_buffer,
			2,
			1);

	no_os_i2c_read(dev->i2c_desc,
		       data_buffer,
		       1,
		       1);

	return data_buffer[0];
}

/***************************************************************************//**
 * @brief Write the content of RDAC register to EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return none.
*******************************************************************************/
void ad5110_write_rdac_eeprom(struct ad5110_dev *dev)
{
	uint8_t data_buffer[2];


	data_buffer[0] = CMD_WR_RDAC_EEPROM;
	data_buffer[1] = 0;
	no_os_i2c_write(dev->i2c_desc,
			data_buffer,
			2,
			1);
}

/***************************************************************************//**
 * @brief Read wiper position from EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return wiperValue - Value read from EEPROM.
*******************************************************************************/
uint8_t ad5110_read_wiper(struct ad5110_dev *dev)
{
	uint8_t data_buffer[2];


	data_buffer[0] = CMD_RD_EEPROM;
	data_buffer[1] = WIPER_POSITION;
	no_os_i2c_write(dev->i2c_desc,
			data_buffer,
			2,
			1);

	no_os_i2c_read(dev->i2c_desc,
		       data_buffer,
		       1,
		       1);

	return data_buffer[0];
}

/***************************************************************************//**
 * @brief Read resistor tolerance from EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return resTolerance - Value read from EEPROM.
*******************************************************************************/
uint8_t ad5110_read_res_tolerance(struct ad5110_dev *dev)
{
	uint8_t data_buffer[2];


	data_buffer[0] = CMD_RD_EEPROM;
	data_buffer[1] = RESISTOR_TOLERANCE;
	no_os_i2c_write(dev->i2c_desc,
			data_buffer,
			2,
			1);

	no_os_i2c_read(dev->i2c_desc,
		       data_buffer,
		       1,
		       1);

	return data_buffer[0];
}

/***************************************************************************//**
 * @brief Software reset; makes a refresh of RDAC register with EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return none.
*******************************************************************************/
void ad5110_reset(struct ad5110_dev *dev)
{
	uint8_t data_buffer[2];


	data_buffer[0] = CMD_RESET;
	data_buffer[1] = 0;
	no_os_i2c_write(dev->i2c_desc,
			data_buffer,
			2,
			1);
}

/***************************************************************************//**
 * @brief Software shutdown.
 *
 * @param dev   - The device structure.
 * @param value - the value written to shut down register.
 *              Example: ShutDownOff - the device is on.
 *                       ShutDownOn  - the device is shut down.
 *
 * @return none.
*******************************************************************************/
void ad5110_shut_down(struct ad5110_dev *dev,
		      uint8_t value)
{
	uint8_t data_buffer[2];


	data_buffer[0] = CMD_SHUT_DOWN;
	data_buffer[1] = value;
	no_os_i2c_write(dev->i2c_desc,
			data_buffer,
			2,
			1);
}
