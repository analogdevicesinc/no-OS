/***************************************************************************//**
 *   @file   AD799x.c
 *   @brief  Implementation of AD799x Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad799x.h"    // AD799x definitions.
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes I2C.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status       - Initialization status.
 *                        Example: -1 - Initialization failed;
 *                                  0 - Initialization succeeded.
*******************************************************************************/
int8_t ad799x_init(struct ad799x_dev **device,
		   struct ad799x_init_param init_param)
{
	struct ad799x_dev *dev;
	int8_t status = -1;

	dev = (struct ad799x_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* Initialize I2C peripheral. */
	status = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);

	/* Determine the number of bits available for a conversion. */
	switch(init_param.part_number) {
	case AD7991:
		dev->bits_number = 12;
		break;
	case AD7995:
		dev->bits_number = 10;
		break;
	case AD7999:
		dev->bits_number = 8;
		break;
	}

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad799x_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad799x_remove(struct ad799x_dev *dev)
{
	int32_t ret;

	ret = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Writes data into the Configuration Register.
 *
 * @param dev            - The device structure.
 * @param register_value - Data value to write.
 *
 * @return None.
*******************************************************************************/
void ad799x_set_configuration_reg(struct ad799x_dev *dev,
				  uint8_t register_value)
{
	no_os_i2c_write(dev->i2c_desc, &register_value, 1, 1);
}

/***************************************************************************//**
 * @brief Reads the High byte and the Low byte of the conversion.
 *
 * @param dev        - The device structure.
 * @param conv_value - It is used to store the conversion value.
 * @param channel    - Stores the channel number for the current conversion.
 *
 * @return none.
*******************************************************************************/
void ad799x_get_conversion_result(struct ad799x_dev *dev,
				  int16_t* conv_value,
				  int8_t* channel)
{
	uint8_t rx_data[2] = {0, 0};
	int16_t conv_word = 0;

	no_os_i2c_read(dev->i2c_desc, rx_data, 2, 1);
	conv_word = (rx_data[0] << 8) + rx_data[1];
	*channel = (conv_word & 0x3000) >> 12;
	*conv_value = (conv_word & 0x0FFF) >> (12 - dev->bits_number);
}

/***************************************************************************//**
 * @brief Converts a raw sample to volts.
 *
 * @param dev		 - The device structure.
 * @param raw_sample - The data sample.
 * @param v_ref      - The value of the voltage reference used by the device.
 *
 * @return voltage  - The result of the conversion expressed as volts.
*******************************************************************************/
float ad799x_convert_to_volts(struct ad799x_dev *dev,
			      int16_t raw_sample,
			      float v_ref)
{
	float voltage = 0;

	voltage = v_ref * (float)raw_sample / (1 << dev->bits_number);

	return voltage;
}
