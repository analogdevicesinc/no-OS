/***************************************************************************//**
 *   @file   ad405x.c
 *   @brief  Implementation of AD405X Driver.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include "ad405x.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
//static uint8_t shadow_reg_val[5] = {0, 0, 0, 0, 0};
static const uint8_t ad405x_part_id[] = {
	[ID_AD4055] = GET_AD405X_RESET_VAL(AD4055_PARTID),
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Reads from the device.
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be read and returned in read_data.
 * @param read_data    - The read data buffer
 *
 * @return ret         - Result of the reading procedure.
*******************************************************************************/
int ad405x_read_device_data(struct ad405x_dev *dev, uint8_t base_address,
			     uint16_t size, uint8_t *read_data)
{
	int ret;

	if (dev->comm_type == AD405X_SPI_COMM) {
		dev->comm_buff[0] = AD405X_SPI_READ | (base_address << 1);
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, dev->comm_buff,
					       1 + size);
		for (uint16_t idx = 0; idx < size; idx++)
			read_data[idx] = dev->comm_buff[idx+1];
	} else {
		ret = no_os_i3c_priv_write(dev->com_desc.i3c_desc, &base_address, 1);
		if (ret)
			return ret;
		ret = no_os_i3c_priv_read(dev->com_desc.i3c_desc, read_data, size);
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes to the device
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be written. It is the size
 *                       of the write_data buffer.
 * @param write_data   - The data which is going to be written.
 *
 * @return ret         - Result of the writing procedure.
*******************************************************************************/
int ad405x_write_device_data(struct ad405x_dev *dev, uint8_t base_address,
			      uint16_t size, uint8_t *write_data)
{
	int ret;

	for (uint16_t idx = 0; idx < size; idx++)
		dev->comm_buff[1+idx] = write_data[idx];

	if (dev->comm_type == AD405X_SPI_COMM) {
		dev->comm_buff[0] = AD405X_SPI_WRITE | (base_address << 1);
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, dev->comm_buff,
					       size + 1);
	} else {
		dev->comm_buff[0] = base_address;
		ret = no_os_i3c_priv_write(dev->com_desc.i3c_desc, dev->comm_buff, size + 1);
	}

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the AD405X
 *        part is present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int ad405x_init(struct ad405x_dev **device,
		 struct ad405x_init_param init_param)
{
	struct ad405x_dev *dev;
	int ret;
	uint8_t reg_value;

	switch (init_param.dev_type) {
	case ID_AD4055:
		break;
	default:
		return -EINVAL;
	}

	dev = (struct ad405x_dev *)no_os_calloc(1, sizeof(*dev));

	if (!dev)
		return -ENOMEM;

	dev->comm_type = init_param.comm_type;

	if (dev->comm_type == AD405X_SPI_COMM) {
		ret = no_os_spi_init(&dev->com_desc.spi_desc, &(init_param.comm_init.spi_init));
		if (ret)
			goto error_dev;
	} else {
		// Attach initialized i3c peripheral to ad405x_dev.comm_desc
		ret = no_os_i3c_init_driver(&dev->com_desc.i3c_desc, &init_param.comm_init.i3c_init);
		if (ret)
			goto error_dev;
	}

	dev->dev_type = init_param.dev_type;

	// TODO do peripheral initialization transfers
	//ret = ad405x_read_device_data(dev, AD405X_ADDR(AD405X_DEVID_AD),
	//			       GET_AD405X_TRANSF_LEN(AD405X_DEVID_AD), &reg_value);
	//if (ret || (reg_value != GET_AD405X_RESET_VAL(AD405X_DEVID_AD)))
	//	goto error_com;

	ret = ad405x_read_device_data(dev, AD405X_ADDR(AD4055_PARTID),
				       GET_AD405X_TRANSF_LEN(AD4055_PARTID),&reg_value);
	if (ret || reg_value != ad405x_part_id[dev->dev_type])
		goto error_com;
	// Get shadow register values
	//ret = ad405x_read_device_data(dev,
	//			       AD405X_ADDR(AD405X_SHADOW_REGISTER_BASE_ADDR),
	//			       GET_AD405X_TRANSF_LEN(AD405X_SHADOW_REGISTER_BASE_ADDR),
	//			       &shadow_reg_val[0]);
	if (ret)
		goto error_com;

	// TODO set defaults, e.g.
	//dev->op_mode = GET_AD405X_RESET_VAL(AD405X_POWER_CTL);

	*device = dev;

	return ret;
error_com:
	if (dev->comm_type == AD405X_SPI_COMM)
		no_os_spi_remove(dev->com_desc.spi_desc);
	else
		no_os_i3c_remove_driver(&dev->com_desc.i3c_desc);
	no_os_free(dev);
	return -1;
error_dev:
	no_os_free(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad405x_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int ad405x_remove(struct ad405x_dev *dev)
{
	int ret;

	if (dev->comm_type == AD405X_SPI_COMM)
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	else
		ret = no_os_i3c_remove_driver(&dev->com_desc.i3c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Performs a soft reset of the device.
 *
 * @param dev  - The device structure.
 *
 * @return ret - Result of the soft reset procedure.
*******************************************************************************/
int ad405x_soft_reset(struct ad405x_dev *dev)
{
	//uint8_t register_values[5];
	int ret;
	uint8_t nb_of_retries = 255;
	uint8_t data = AD405X_RESET_CODE;
	union ad405x_sts_reg_flags flags;

	// Perform soft reset
	ret = ad405x_write_device_data(dev, AD405X_ADDR(AD405X_RESET),
					GET_AD405X_TRANSF_LEN(AD405X_RESET), &data);
	if (ret)
		return ret;

	// After soft reset, the data in the shadow registers will be valid only after NVM is not busy anymore
	ret = ad405x_get_sts_reg(dev, &flags);
	while (flags.fields.NVM_BUSY && nb_of_retries) {
		ret = ad405x_get_sts_reg(dev, &flags);
		nb_of_retries--;
	}
	if ((!nb_of_retries) || ret)
		return -EAGAIN;

	// TODO set readings after soft reset
	//no_os_mdelay(1);

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw output data.
 *
 * @param dev   - The device structure.
 * @param raw   - Raw ADC data.
 *
 * @return ret  - Result of the reading procedure.
*******************************************************************************/
int ad405x_get_raw(struct ad405x_dev *dev, uint32_t *raw)
{
	uint8_t array_raw[GET_AD405X_TRANSF_LEN(AD405X_DATA)] = {0};
	int ret;

	ret = ad405x_read_device_data(dev,AD405X_ADDR(AD405X_DATA),
				       GET_AD405X_TRANSF_LEN(AD405X_DATA),array_raw);
	return ret;
}

/***************************************************************************//**
 * @brief Reads the status register value.
 *
 * @param dev          - The device structure.
 * @param status_flags - Register value.
 *
 * @return ret         - Result of the reading procedure.
*******************************************************************************/
int ad405x_get_sts_reg(struct ad405x_dev *dev,
			union ad405x_sts_reg_flags *status_flags)
{
	int ret;
	uint8_t reg_value;

	ret = ad405x_read_device_data(dev, AD405X_ADDR(AD405X_STATUS),
				       GET_AD405X_TRANSF_LEN(AD405X_STATUS), &reg_value);

	if (!ret)
		status_flags->value = reg_value;

	return ret;
}
