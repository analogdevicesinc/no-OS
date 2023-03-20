/***************************************************************************//**
 *   @file   adt7420.c
 *   @brief  Implementation of ADT7420 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012, 2019, 2021(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "adt7420.h"

const struct adt7420_chip_info chip_info[] = {
	[ID_ADT7410] = {
		.resolution = 16,
		.communication = I2C,
	},
	[ID_ADT7420] = {
		.resolution = 16,
		.communication = I2C,
	},
	[ID_ADT7422] = {
		.resolution = 16,
		.communication = I2C,
	},
	[ID_ADT7310] = {
		.resolution = 16,
		.communication = SPI,
	},
	[ID_ADT7320] = {
		.resolution = 16,
		.communication = SPI,
	}
};

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param data - Pointer to the register value
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
int adt7420_reg_read(struct adt7420_dev *dev,
		     uint16_t register_address, uint16_t *data)
{
	if (adt7420_is_spi(dev))
		return adt7420_spi_reg_read(dev, register_address, data);
	else
		return adt7420_i2c_reg_read(dev, register_address, data);
}

/***************************************************************************//**
 * @brief Sets the value of a register SPI/I2C.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param mask		   - Bit Mask of the bit to be written
 * @param value		   - Value of the bit
 *
 * @return 0 in case of success, -1 otherwise.
*******************************************************************************/
int adt7420_reg_update_bits(struct adt7420_dev *dev,
			    uint16_t register_address, uint8_t mask, uint8_t value)
{

	uint16_t regval;

	if (adt7420_reg_read(dev, register_address, &regval))
		return -1;

	regval &= ~mask;
	regval |= value;

	return adt7420_reg_write(dev, register_address, regval);
}

/***************************************************************************//**
 * @brief Write a value of a register via SPI
 * @param dev - The device structure.
 * @param register_address - Address of the register.
 * @param data - Pointer to the register value
 * @return 0 in case of success, -1 otherwise.
*******************************************************************************/
int adt7420_spi_reg_write(struct adt7420_dev *dev, uint16_t register_address,
			  uint32_t data)
{
	uint8_t data_buffer[3] = { 0, 0, 0 };
	uint8_t num_bytes;

	if (no_os_field_get(ADT7320_L16, register_address)) {
		num_bytes = 3;
		data_buffer[1] = (data >> 8);
		data_buffer[2] = data & 0xFF;
	} else {
		num_bytes = 2;
		data_buffer[1] = data;
	}

	/*Form the SPI register read command byte. Bits [5:3] of command byte
	 holds the register address.*/
	data_buffer[0] = (no_os_field_get(ADT7320_ADDR_MSK,
					  register_address) << 3) & ADT7320_WRITE_MASK_CMD;

	return no_os_spi_write_and_read(dev->spi_desc, data_buffer, num_bytes);

}

/***************************************************************************//**
 * @brief Write a value of a register via I2C
 * @param dev - The device structure.
 * @param register_address - Address of the register.
 * @param data - Pointer to the register value
 * @return 0 in case of success, -1 otherwise.
*******************************************************************************/
int adt7420_i2c_reg_write(struct adt7420_dev *dev, uint16_t register_address,
			  uint32_t data)
{
	uint8_t data_buffer[3] = { 0, 0, 0 };
	uint8_t num_bytes;

	if (no_os_field_get(ADT7320_L16, register_address)) {
		num_bytes = 3;
		data_buffer[2] = data & 0xFF;
		data_buffer[1] = (data >> 8);
	} else {
		num_bytes = 2;
		data_buffer[1] = data;
	}

	data_buffer[0] = register_address;

	return no_os_i2c_write(dev->i2c_desc, data_buffer, num_bytes,
			       1); //no repeat start

}

/***************************************************************************//**
 * @brief Sets the value of a register SPI/I2C.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param data             - Data to be written in input register.
 *
 * @return 0 in case of success, -1 otherwise.
*******************************************************************************/
int adt7420_reg_write(struct adt7420_dev *dev,
		      uint16_t register_address,
		      uint32_t data)
{
	if (adt7420_is_spi(dev))
		return adt7420_spi_reg_write(dev, register_address, data);
	else
		return adt7420_i2c_reg_write(dev, register_address, data);
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the device is
 *        present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - The result of the initialization procedure.
 *                  Example: -1 - I2C peripheral was not initialized or the
 *                                device is not present.
 *                            0 - I2C peripheral was initialized and the
 *                                device is present.
*******************************************************************************/
int32_t adt7420_init(struct adt7420_dev **device,
		     struct adt7420_init_param init_param)
{
	struct adt7420_dev *dev;
	int32_t status;
	uint16_t device_connected_check = 0;

	dev = (struct adt7420_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->active_device = init_param.active_device;

	if (adt7420_is_spi(dev))
		status = no_os_spi_init(&dev->spi_desc, &init_param.interface_init.spi_init);
	else
		status = no_os_i2c_init(&dev->i2c_desc, &init_param.interface_init.i2c_init);

	if (status != -1) {
		/* Device Settings */
		dev->resolution_setting = init_param.resolution_setting;

		/* Reset device to default values to ensure all internal circuitry is properly initialised*/
		status = adt7420_reset(dev);
		if (status < 0)
			goto adt7420_init_error;

		/*Register read to ensure that next read will be valid - acts as 200us delay while
		  device resets*/
		if (adt7420_is_spi(dev))
			status = adt7420_reg_read(dev, ADT7320_REG_ID, &device_connected_check);
		else
			status = adt7420_reg_read(dev, ADT7420_REG_ID, &device_connected_check);

		if (status)
			return status;
		device_connected_check >>= 4; // Manufacturer ID

		if (device_connected_check != ADT7xxx_ID) // AD7xxx ID Check
			status = -1;
		else
			status = 0;

		*device = dev;

		return status;
	}

	return status;

adt7420_init_error:
	adt7420_remove(dev);
	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adt7420_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t adt7420_remove(struct adt7420_dev *dev)
{
	int32_t ret;

	if (!(adt7420_is_spi(dev)))
		ret = no_os_i2c_remove(dev->i2c_desc);
	else
		ret = no_os_spi_remove(dev->spi_desc);
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Sets the operational mode for ADT7420/ADT7320.
 *
 * @param dev  - The device structure.
 * @param mode - Operation mode.
 *               Example: ADT7420_OP_MODE_CONT_CONV - continuous conversion;
 *                        ADT7420_OP_MODE_ONE_SHOT  - one shot;
 *                        ADT7420_OP_MODE_1_SPS     - 1 SPS mode;
 *                        ADT7420_OP_MODE_SHUTDOWN  - shutdown.
 *
 * @return None.
*******************************************************************************/
int adt7420_set_operation_mode(struct adt7420_dev *dev,
			       uint8_t mode)
{
	int status;

	if (adt7420_is_spi(dev)) {
		status = adt7420_reg_update_bits(dev, ADT7320_REG_CONFIG,
						 ADT7420_CONFIG_OP_MODE(ADT7420_OP_MODE_SHUTDOWN), ADT7420_CONFIG_OP_MODE(mode));
	} else {
		status = adt7420_reg_update_bits(dev, ADT7420_REG_CONFIG,
						 ADT7420_CONFIG_OP_MODE(ADT7420_OP_MODE_SHUTDOWN), ADT7420_CONFIG_OP_MODE(mode));
	}

	return status;
}

/***************************************************************************//**
 * @brief Sets the resolution for ADT7420/ADT7320.
 *
 * @param dev        - The device structure.
 * @param resolution - Resolution.
 *                     Example: 0 - 13-bit resolution;
 *                              1 - 16-bit resolution.
 *
 * @return None.
*******************************************************************************/
int adt7420_set_resolution(struct adt7420_dev *dev,
			   uint8_t resolution)
{
	int status;

	if (adt7420_is_spi(dev)) {
		status = adt7420_reg_update_bits(dev, ADT7320_REG_CONFIG,
						 ADT7420_CONFIG_RESOLUTION, (resolution * ADT7420_CONFIG_RESOLUTION));
	} else {
		status = adt7420_reg_update_bits(dev, ADT7420_REG_CONFIG,
						 ADT7420_CONFIG_RESOLUTION, (resolution * ADT7420_CONFIG_RESOLUTION));
	}

	dev->resolution_setting = resolution;

	return status;
}

/***************************************************************************//**
 * @brief Resets the SPI or I2C inteface for the ADT7420/ADT7320
 *
 * @param dev        - The device structure.
 *
 * @return 0 in case of Success, -1 otherwise.
*******************************************************************************/
int32_t adt7420_reset(struct adt7420_dev *dev)
{
	if (adt7420_is_spi(dev)) {
		uint8_t data_buffer[] = { 0xFF, 0xFF, 0xFF, 0xFF };
		if (no_os_spi_write_and_read(dev->spi_desc,
					     data_buffer,
					     sizeof(data_buffer)))
			return -1;
	} else {
		uint8_t register_address = ADT7420_REG_RESET;
		if (no_os_i2c_write(dev->i2c_desc,
				    &register_address,
				    1,
				    1)) {
			//no repeat start
			return -1;
		}
	}
	no_os_mdelay(ADT7420_RESET_DELAY); /* device restart */
	dev->resolution_setting = 0;
	return 0;
}

/***************************************************************************//**
 * @brief Reads the temperature data and converts it to Celsius degrees.
 *
 * @param dev - The device structure.
 *
 * @return temperature - Temperature in degrees Celsius.
*******************************************************************************/
float adt7420_get_temperature(struct adt7420_dev *dev)
{
	uint16_t temp = 0;
	float temp_c = 0;

	if (adt7420_is_spi(dev))
		adt7420_reg_read(dev, ADT7320_REG_TEMP, &temp);
	else {
		adt7420_reg_read(dev, ADT7420_REG_TEMP_MSB, &temp);
	}

	if (dev->resolution_setting) {
		if (temp & ADT7420_16BIT_SIGN)
			/*! Negative temperature */
			temp_c = (float)((int32_t)temp - ADT7420_16BIT_NEG) / ADT7420_16BIT_DIV;
		else
			/*! Positive temperature */
			temp_c = (float)temp / ADT7420_16BIT_DIV;
	} else {
		temp >>= 3;
		if (temp & ADT7420_13BIT_SIGN)
			/*! Negative temperature */
			temp_c = (float)((int32_t)temp - ADT7420_13BIT_NEG) / ADT7420_13BIT_DIV;
		else
			/*! Positive temperature */
			temp_c = (float)temp / ADT7420_13BIT_DIV;
	}

	return temp_c;
}


/***************************************************************************//**
 * @brief Reads the value of a register SPI interface device.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param data - Pointer to the register value
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
int adt7420_spi_reg_read(struct adt7420_dev *dev,
			 uint16_t register_address, uint16_t *data)
{
	uint8_t data_buffer[3] = { 0, 0, 0 };
	uint8_t num_bytes;

	if (no_os_field_get(ADT7320_L16, register_address))
		num_bytes = 3;
	else
		num_bytes = 2;

	data_buffer[0] = ADT7320_READ_CMD  | (no_os_field_get(ADT7320_ADDR_MSK,
					      register_address) << 3);

	if (no_os_spi_write_and_read(dev->spi_desc, data_buffer, num_bytes))
		return -1;

	if (num_bytes == 2)
		*data = data_buffer[1];
	else
		*data = data_buffer[1] << 8 | data_buffer[2];

	return 0;
}

/***************************************************************************//**
 * @brief Reads the value of a register I2C interface device.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param data - Pointer to the register value
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
int adt7420_i2c_reg_read(struct adt7420_dev *dev, uint16_t register_address,
			 uint16_t *data)
{
	uint8_t data_buffer[3] = { 0, 0 };
	uint8_t num_bytes;

	if (no_os_field_get(ADT7320_L16, register_address))
		num_bytes = 2;
	else
		num_bytes = 1;

	if (no_os_i2c_write(dev->i2c_desc, &register_address, 1,
			    0)) //add a repeat start
		return -1;
	if (no_os_i2c_read(dev->i2c_desc, data_buffer, num_bytes, 1))
		return -1;

	if (num_bytes == 1)
		*data = data_buffer[0];
	else
		*data = no_os_get_unaligned_be16(data_buffer);

	return 0;
}

/***************************************************************************//**
 * @brief Check if the interface of the selected device is SPI
 *
 * @param dev              - The device structure.
 *
 * @return True in case of an SPI interface, False Otherwise
*******************************************************************************/
bool adt7420_is_spi(struct adt7420_dev *dev)
{
	return (chip_info[dev->active_device].communication == SPI);
}
