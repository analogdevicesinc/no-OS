/***************************************************************************//**
 *   @file   ad7799.c
 *   @brief  Implementation of AD7798/AD7799 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "ad7799.h"
#include "no_os_alloc.h"

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
static const uint8_t ad7798_reg_size[] = {
	[AD7799_REG_COMM] = AD7799_REG_SIZE_1B,
	[AD7799_REG_MODE] = AD7799_REG_SIZE_2B,
	[AD7799_REG_CONF] = AD7799_REG_SIZE_2B,
	[AD7799_REG_DATA] = AD7799_REG_SIZE_2B,
	[AD7799_REG_ID] = AD7799_REG_SIZE_1B,
	[AD7799_REG_IO] = AD7799_REG_SIZE_1B,
	[AD7799_REG_OFFSET] = AD7799_REG_SIZE_2B,
	[AD7799_REG_FULLSCALE] = AD7799_REG_SIZE_2B
};

static const uint8_t ad7799_reg_size[] = {
	[AD7799_REG_COMM] = AD7799_REG_SIZE_1B,
	[AD7799_REG_MODE] = AD7799_REG_SIZE_2B,
	[AD7799_REG_CONF] = AD7799_REG_SIZE_2B,
	[AD7799_REG_DATA] = AD7799_REG_SIZE_3B,
	[AD7799_REG_ID] = AD7799_REG_SIZE_1B,
	[AD7799_REG_IO] = AD7799_REG_SIZE_1B,
	[AD7799_REG_OFFSET] = AD7799_REG_SIZE_3B,
	[AD7799_REG_FULLSCALE] = AD7799_REG_SIZE_3B
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param device - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_read(struct ad7799_dev *device, uint8_t reg_addr,
		    uint32_t *reg_data)
{
	int32_t ret;
	uint8_t i;
	uint8_t buff[4];
	uint8_t buff_size;

	*reg_data = 0;

	buff_size = device->reg_size[reg_addr];

	buff[0] = AD7799_COMM_READ | AD7799_COMM_ADDR(reg_addr);

	memset((buff + 1), 0, buff_size + 1);

	ret = no_os_spi_write_and_read(device->spi_desc, buff, buff_size + 1);
	if(ret)
		return -1;

	for(i = 1; i < buff_size + 1 ; i++)
		*reg_data = (*reg_data << 8) | buff[i];

	return ret;
}

/**
 * @brief Write device register.
 * @param device - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_write(struct ad7799_dev *device, uint8_t reg_addr,
		     uint32_t reg_data)
{
	int32_t ret;
	uint8_t i;
	uint8_t buff[4];
	uint8_t buff_size;

	buff_size = device->reg_size[reg_addr];
	buff[0] = AD7799_COMM_WRITE | AD7799_COMM_ADDR(reg_addr);

	for (i = 1; i < buff_size + 1 ; i++)
		buff[i] = reg_data >> ((buff_size - i) * 8);

	ret = no_os_spi_write_and_read(device->spi_desc, buff, buff_size + 1);
	if(ret)
		return -1;

	return ret;
}

/**
 * @brief Software reset of the device.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_reset(struct ad7799_dev *device)
{
	uint8_t reset_data[4] = {
		AD7799_RESET_DATA,
		AD7799_RESET_DATA,
		AD7799_RESET_DATA,
		AD7799_RESET_DATA
	};

	return no_os_spi_write_and_read(device->spi_desc, reset_data, 4);
}

/**
 * @brief Set the device mode.
 * @param device - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_set_mode(struct ad7799_dev *device, uint8_t mode)
{
	int32_t ret;
	uint32_t reg_data = 0;

	ret = ad7799_read(device, AD7799_REG_MODE, &reg_data);
	if (ret)
		return -1;

	reg_data &= ~AD7799_MODE_SEL(AD7799_REG_MASK);
	reg_data |= AD7799_MODE_SEL(mode);

	ret = ad7799_write(device, AD7799_REG_MODE, reg_data);
	if (ret)
		return -1;

	ret = ad7799_dev_ready(device);
	if (ret)
		return -1;

	return 0;
}

/**
 * @brief Select the ADC channel.
 * @param device - The device structure.
 * @param  ch - The channel number.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_set_channel(struct ad7799_dev *device, uint8_t ch)
{
	int32_t ret;
	uint32_t reg_data = 0;

	ret = ad7799_read(device, AD7799_REG_CONF, &reg_data);
	if (ret)
		return -1;

	reg_data &= ~AD7799_CONF_CHAN(AD7799_REG_MASK);
	reg_data |= AD7799_CONF_CHAN(ch);

	return ad7799_write(device, AD7799_REG_CONF, reg_data);
}

/**
 * @brief Read specific ADC channel.
 * @param device - The device structure.
 * @param ch - The ADC channel.
 * @param reg_data - The content of the data register.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_get_channel(struct ad7799_dev *device, uint8_t ch,
			   uint32_t *reg_data)
{
	int32_t ret;

	ret = ad7799_set_channel(device, ch);
	if (ret)
		return -1;

	ret = ad7799_set_mode(device, AD7799_MODE_SINGLE);
	if (ret)
		return -1;

	ret = ad7799_dev_ready(device);
	if (ret)
		return -1;

	ret = ad7799_read(device, AD7799_REG_DATA, reg_data);
	if (ret)
		return -1;

	return 0;
}

/**
 * @brief Read data from specific ADC channel with specified precision.
 * @param device - The device structure.
 * @param ch - The ADC channel.
 * @param data_scaled - The content of the data in mV/uV.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_read_channel(struct ad7799_dev *device, uint8_t ch,
			    int32_t *data_scaled)
{
	int32_t ret;
	uint32_t data, temp;

	uint32_t vref_scaled = device->vref_mv;

	if (device->precision)
		vref_scaled *= 1000;

	ret = ad7799_get_channel(device, ch, &data);
	if(ret)
		return ret;

	if (device->polarity) { // AD7799_UNIPOLAR
		temp = (1 << (device->reg_size[AD7799_REG_DATA] * 8));
		data = data * vref_scaled / temp;
	} else { // AD7799_BIPOLAR
		temp = 1 << ((device->reg_size[AD7799_REG_DATA] * 8) - 1);

		if(data >= temp)
			data = ((data - temp) * vref_scaled) / (temp - 1);
		else
			data = -(((temp - data) * vref_scaled) / (temp - 1));
	}

	*data_scaled = data;

	return ret;
}

/**
 * @brief Set the ADC gain.
 * @param device - The device structure.
 * @param  gain - the channel number.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_set_gain(struct ad7799_dev *device, uint8_t gain)
{
	int32_t ret;
	uint32_t reg_data = 0;

	ret = ad7799_read(device, AD7799_REG_CONF, &reg_data);
	if (ret)
		return -1;

	reg_data &= ~AD7799_CONF_GAIN(AD7799_REG_MASK);
	reg_data |= AD7799_CONF_GAIN(gain);

	return ad7799_write(device, AD7799_REG_CONF, reg_data);
}

/**
 * @brief Get the ADC gain
 * @param device - The device structure.
 * @param  gain - the gain value from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_get_gain(struct ad7799_dev *device, uint8_t *gain)
{
	int32_t ret;
	uint32_t reg_data = 0;

	ret = ad7799_read(device, AD7799_REG_CONF, &reg_data);
	if (ret)
		return -1;

	reg_data &= AD7799_CONF_GAIN(AD7799_REG_MASK);
	*gain = reg_data >> 8;

	return 0;
}

/**
 * @brief Enable or disable the reference detect function.
 * @param device - The device structure.
 * @param  ref_en - 1 reference detect enable.
 * 				  - 0 reference detect disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_set_refdet(struct ad7799_dev *device, uint8_t ref_en)
{
	int32_t ret;
	uint32_t reg_data = 0;

	ret = ad7799_read(device, AD7799_REG_CONF, &reg_data);
	if (ret)
		return -1;

	reg_data &= ~AD7799_CONF_REFDET(AD7799_REG_MASK);
	reg_data |= AD7799_CONF_REFDET(ref_en);

	return ad7799_write(device, AD7799_REG_CONF, reg_data);
}

/**
 * @brief Set ADC polarity.
 * @param device - The device structure.
 * @param polarity - set the device polarity:
 * 					0 - Bipolar coding
 * 					1 - Unipolar coding
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_set_polarity(struct ad7799_dev *device, uint8_t polarity)
{
	int32_t ret;
	uint32_t reg_data = 0;

	ret = ad7799_read(device, AD7799_REG_CONF, &reg_data);
	if (ret)
		return -1;

	reg_data &= ~AD7799_CONF_POLARITY(AD7799_REG_MASK);
	reg_data |= AD7799_CONF_POLARITY(polarity);

	return ad7799_write(device, AD7799_REG_CONF, reg_data);
}

/**
 * @brief Read the /RDY bit of status register and check the status
 * of the device
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_dev_ready(struct ad7799_dev *device)
{
	int32_t ret;
	uint32_t data;
	uint32_t timeout = AD7799_TIMEOUT;

	while (timeout > 0) {
		ret = ad7799_read(device, AD7799_REG_STAT, &data);
		if (ret)
			return -1;

		if (!(data & AD7799_STAT_RDY))
			return 0;

		timeout--;
	}

	return -1;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_init(struct ad7799_dev **device,
		    const struct ad7799_init_param *init_param)
{
	struct ad7799_dev *dev;
	int32_t ret;
	uint32_t chip_id = 0;


	dev = (struct ad7799_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	dev->chip_type = init_param->chip_type;
	dev->polarity = init_param->polarity;
	dev->gain = init_param->gain;
	dev->vref_mv = init_param->vref_mv;
	dev->precision = init_param->precision;

	switch(dev->chip_type) {
	case ID_AD7798:
		dev->reg_size = ad7798_reg_size;
		break;
	case ID_AD7799:
		dev->reg_size = ad7799_reg_size;
		break;
	default:
		no_os_free(dev);
		return -1;
	}

	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret) {
		no_os_free(dev);
		return -1;
	}

	ret = ad7799_reset(dev);
	if (ret)
		return -1;

	/* Check Chip ID */
	ret = ad7799_read(dev, AD7799_REG_ID, &chip_id);
	if (ret)
		return -1;

	switch(dev->chip_type) {
	case ID_AD7798:
		if ((chip_id & AD7799_ID_MASK) != ID_AD7798) {
			printf("Invalid AD7798 Chip ID");
			return -1;
		}
		break;
	case ID_AD7799:
		if ((chip_id & AD7799_ID_MASK) != ID_AD7799) {
			printf("Invalid AD7799 Chip ID");
			return -1;
		}
		break;
	default:
		printf("Invalid AD7798 Chip ID");
		return -1;
	}

	/* Initially set gain to 1 */
	ret = ad7799_set_gain(dev, dev->gain);
	if (ret)
		return -1;

	/* Enable unipolar coding */
	ret = ad7799_set_polarity(dev, dev->polarity);
	if (ret)
		return -1;

	*device = dev;

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7799_remove(struct ad7799_dev *device)
{
	int32_t ret;

	ret = no_os_spi_remove(device->spi_desc);
	no_os_free(device);

	return ret;
}
