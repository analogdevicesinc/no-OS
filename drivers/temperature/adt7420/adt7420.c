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
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_error.h"
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
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
uint16_t adt7420_get_register_value(struct adt7420_dev *dev,
				    uint8_t register_address)
{
	if (adt7420_is_spi(dev))
		return adt7420_spi_get_register_value(dev, register_address);
	else
		return adt7420_i2c_get_register_value(dev, register_address);
}

/***************************************************************************//**
 * @brief Sets the value of a register SPI/I2C.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param num_data_bytes   - Number of bytes.
 * @param data             - Data to be written in input register.
 * @param mask		   - Bit Mask of the bit to be written
 * @param value		   - Value of the bit
 *
 * @return SUCCESS in case of success, FAILURE otherwise.
*******************************************************************************/
int32_t set_register_value(struct adt7420_dev *dev,
			   uint8_t register_address,
			   uint8_t num_data_bytes,
			   uint8_t *data, uint8_t mask, uint8_t value)
{

	uint8_t data_buffer[3] = {0, 0, 0};

	data[0] &= ~mask;
	data[0] |= value;
	data_buffer[1] = data[0];
	data_buffer[2] = data[1];

	if (adt7420_is_spi(dev)) {
		/*Form the SPI register read command byte. Bits [5:3] of command byte
		 holds the register address.*/
		data_buffer[0] = (register_address << 3) & ADT7320_WRITE_MASK_CMD;
		if (spi_write_and_read(dev->spi_desc, data_buffer, num_data_bytes) != SUCCESS)
			return FAILURE;
	} else {
		data_buffer[0] = register_address;

		if (i2c_write(dev->i2c_desc,
			      data_buffer,
			      num_data_bytes,
			      1) != SUCCESS)
			//no repeat start
			return FAILURE;
	}
	return SUCCESS;
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
	uint8_t device_connected_check = 0;

	dev = (struct adt7420_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->active_device = init_param.active_device;

	if (adt7420_is_spi(dev))
		status = spi_init(&dev->spi_desc, &init_param.interface_init.spi_init);
	else
		status = i2c_init(&dev->i2c_desc, &init_param.interface_init.i2c_init);

	if (status != FAILURE) {
		/* Device Settings */
		dev->resolution_setting = init_param.resolution_setting;

		/* Reset device to default values to ensure all internal circuitry is properly initialised*/
		status = adt7420_reset(dev);
		if (status < 0)
			goto adt7420_init_error;

		/*Register read to ensure that next read will be valid - acts as 200us delay while
		  device resets*/
		if (adt7420_is_spi(dev))
			device_connected_check = adt7420_get_register_value(dev, ADT7320_REG_ID);
		else
			device_connected_check = adt7420_get_register_value(dev, ADT7420_REG_ID);

		device_connected_check >>= 4; // Manufacturer ID

		if(device_connected_check != ADT7xxx_ID) // AD7xxx ID Check
			status = FAILURE;
		else
			status = SUCCESS;

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
		ret = i2c_remove(dev->i2c_desc);
	else
		ret = spi_remove(dev->spi_desc);
	free(dev);

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
void adt7420_set_operation_mode(struct adt7420_dev *dev,
				uint8_t mode)
{
	uint8_t register_value [2] = { 0, 0 };
	uint8_t device_reg_address;

	if (adt7420_is_spi(dev)) {
		device_reg_address = ADT7320_REG_CONFIG;
		register_value[0]  = adt7420_get_register_value(dev, device_reg_address);
	} else {
		device_reg_address = ADT7420_REG_CONFIG;
		register_value[0]  = adt7420_get_register_value(dev, device_reg_address);
	}

	set_register_value(dev, device_reg_address, 2, register_value,
			   ADT7420_CONFIG_OP_MODE(ADT7420_OP_MODE_SHUTDOWN), ADT7420_CONFIG_OP_MODE(mode));
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
void adt7420_set_resolution(struct adt7420_dev *dev,
			    uint8_t resolution)
{
	uint8_t register_value[1] = { 0 };
	uint8_t device_reg_address;

	if (adt7420_is_spi(dev)) {
		device_reg_address = ADT7320_REG_CONFIG;
		register_value[0]  = adt7420_get_register_value(dev, device_reg_address);
	} else {
		device_reg_address = ADT7420_REG_CONFIG;
		register_value[0]  = adt7420_get_register_value(dev, device_reg_address);
	}

	set_register_value(dev, device_reg_address, 2, register_value,
			   ADT7420_CONFIG_RESOLUTION, (resolution * ADT7420_CONFIG_RESOLUTION));
	dev->resolution_setting = resolution;
}

/***************************************************************************//**
 * @brief Resets the SPI or I2C inteface for the ADT7420/ADT7320
 *
 * @param dev        - The device structure.
 *
 * @return SUCCESS in case of Success, FAILURE otherwise.
*******************************************************************************/
int32_t adt7420_reset(struct adt7420_dev *dev)
{
	uint8_t data_buffer[] = { 0xFF, 0xFF, 0xFF, 0xFF };

	if (adt7420_is_spi(dev)) {
		if (spi_write_and_read(dev->spi_desc,
				       data_buffer,
				       sizeof(data_buffer)) != SUCCESS)
			return FAILURE;
	} else {
		uint8_t register_address = ADT7420_REG_RESET;
		if (i2c_write(dev->i2c_desc,
			      &register_address,
			      1,
			      1) != SUCCESS) {
			//no repeat start
			return FAILURE;
		}
	}
	dev->resolution_setting = 0;
	return SUCCESS;
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
		temp = adt7420_get_register_value(dev, ADT7320_REG_TEMP);
	else {
		temp  = adt7420_get_register_value(dev, ADT7420_REG_TEMP_MSB) << 8;
		temp |= adt7420_get_register_value(dev, ADT7420_REG_TEMP_LSB);
	}

	if(dev->resolution_setting) {
		if(temp & 0x8000)
			/*! Negative temperature */
			temp_c = (float)((int32_t)temp - 65536) / 128;
		else
			/*! Positive temperature */
			temp_c = (float)temp / 128;
	} else {
		temp >>= 3;
		if(temp & 0x1000)
			/*! Negative temperature */
			temp_c = (float)((int32_t)temp - 8192) / 16;
		else
			/*! Positive temperature */
			temp_c = (float)temp / 16;
	}

	return temp_c;
}


/**************************************************************************//**
 * @brief Write to input shift register SPI interface.
 *
 * @param dev               - The device structure.
 * @param register_address  - Command control bits.
 * @param data              - Data to be written in input register.
 *
 * @return  read_back_data  - value read from register.
******************************************************************************/
uint16_t set_shift_reg(struct adt7420_dev *dev,
		       uint8_t register_address,
		       uint8_t *data)
{
	uint8_t data_buffer[3] = { 0, 0, 0 };
	uint16_t read_back_data = 0;
	uint8_t num_bytes;

	switch (register_address) {
	case ADT7320_REG_STATUS:
	case ADT7320_REG_CONFIG:
	case ADT7320_REG_ID:
	case ADT7320_REG_HIST:
		num_bytes = 2;
		break;
	default:
		num_bytes = 3;
		break;
	}

	data_buffer[0] = data[0];
	data_buffer[1] = data[1];
	spi_write_and_read(dev->spi_desc, data_buffer, num_bytes);

	if (num_bytes == 2)
		read_back_data = data_buffer[1];
	else
		read_back_data = data_buffer[1] << 8 | data_buffer[2];

	return read_back_data;
}

/***************************************************************************//**
 * @brief Reads the value of a register SPI interface device.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
uint16_t adt7420_spi_get_register_value(struct adt7420_dev *dev,
					uint8_t register_address)
{
	uint8_t data[2] = { 0, 0xFF };
	/* Form the SPI register read command byte. Bits [5:3] of command byte
		 holds the register address.*/
	data[0] = ADT7320_READ_CMD  | (register_address << 3);

	return set_shift_reg(dev, register_address, data);
}

/***************************************************************************//**
 * @brief Reads the value of a register I2C interface device.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
uint16_t adt7420_i2c_get_register_value(struct adt7420_dev *dev,
					uint8_t register_address)
{
	uint8_t register_value = 0;
	uint8_t data[2] = { 0, 0xFF };
	data[0] = register_address;

	i2c_write(dev->i2c_desc,
		  &register_address,
		  1,
		  0); //add a repeat start
	i2c_read(dev->i2c_desc,
		 &register_value,
		 1,
		 1);

	return register_value;
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
