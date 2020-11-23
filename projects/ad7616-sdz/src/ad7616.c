/***************************************************************************//**
 *   @file   ad7616.c
 *   @brief  Implementation of AD7616 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <sleep.h>
#include "platform_drivers.h"
#include "ad7616_core.h"
#include "ad7616.h"

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_read(ad7616_dev *dev,
					uint8_t reg_addr,
					uint16_t *reg_data)
{
	if (dev->interface == AD7616_SERIAL)
		return ad7616_spi_read(dev, reg_addr, reg_data);
	else
		return ad7616_par_read(dev, reg_addr, reg_data);
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_write(ad7616_dev *dev,
					 uint8_t reg_addr,
					 uint16_t reg_data)
{
	if (dev->interface == AD7616_SERIAL)
		return ad7616_spi_write(dev, reg_addr, reg_data);
	else
		return ad7616_par_write(dev, reg_addr, reg_data);
}

/**
 * Read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_read_mask(ad7616_dev *dev,
						 uint8_t reg_addr,
						 uint16_t mask,
						 uint16_t *data)
{
	uint16_t reg_data;
	int32_t ret;

	if (dev->interface == AD7616_SERIAL)
		ret = ad7616_spi_read(dev, reg_addr, &reg_data);
	else
		ret = ad7616_par_read(dev, reg_addr, &reg_data);
	*data = (reg_data & mask);

	return ret;
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_write_mask(ad7616_dev *dev,
						  uint8_t reg_addr,
						  uint16_t mask,
						  uint16_t data)
{
	uint16_t reg_data;
	int32_t ret;

	if (dev->interface == AD7616_SERIAL)
		ret = ad7616_spi_read(dev, reg_addr, &reg_data);
	else
		ret = ad7616_par_read(dev, reg_addr, &reg_data);
	reg_data &= ~mask;
	reg_data |= data;
	if (dev->interface == AD7616_SERIAL)
		ret |= ad7616_spi_write(dev, reg_addr, reg_data);
	else
		ret |= ad7616_par_write(dev, reg_addr, reg_data);

	return ret;
}

/**
 * SPI read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_spi_read(ad7616_dev *dev,
						uint8_t reg_addr,
						uint16_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = 0x00 | ((reg_addr & 0x3F) << 1);
	buf[1] = 0x00;
	ret = spi_write_and_read(&dev->spi_dev, buf, 2);

	buf[0] = 0x00 | ((reg_addr & 0x3F) << 1);
	buf[1] = 0x00;
	ret = spi_write_and_read(&dev->spi_dev, buf, 2);
	*reg_data = ((buf[0] & 0x01) << 8) | buf[1];

	return ret;
}

/**
 * SPI write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_spi_write(ad7616_dev *dev,
						 uint8_t reg_addr,
						 uint16_t reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = 0x80 | ((reg_addr & 0x3F) << 1) | ((reg_data & 0x100) >> 8);
	buf[1] = (reg_data & 0xFF);
	ret = spi_write_and_read(&dev->spi_dev, buf, 2);

	return ret;
}

/**
 * PAR read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_par_read(ad7616_dev *dev,
						uint8_t reg_addr,
						uint16_t *reg_data)
{
	uint32_t read;

	ad7616_core_write(*dev->core, AD7616_REG_UP_WRITE_DATA,
			0x0000 | ((reg_addr & 0x3F) << 9));
	usleep(50);
	ad7616_core_read(*dev->core, AD7616_REG_UP_READ_DATA, &read);
	*reg_data = read & 0xFF;
	mdelay(1);

	return 0;
}

/**
 * PAR write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_par_write(ad7616_dev *dev,
						 uint8_t reg_addr,
						 uint16_t reg_data)
{
	ad7616_core_write(*dev->core, AD7616_REG_UP_WRITE_DATA,
			0x8000 | ((reg_addr & 0x3F) << 9) | (reg_data & 0xFF));
	mdelay(1);

	return 0;
}

/**
 * Perform a full reset of the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_reset(ad7616_dev *dev)
{
	int32_t ret;

	ret = gpio_set_value(&dev->gpio_dev, dev->gpio_reset, GPIO_LOW);
	/* Low pulse width for a full reset should be at least 1200 ns */
	mdelay(20);
	ret |= gpio_set_value(&dev->gpio_dev, dev->gpio_reset, GPIO_HIGH);
	/* 15 ms are required to completely reconfigure the device */
	mdelay(150);

	return ret;
}

/**
 * Set the analog input range for the selected analog input channel.
 * @param dev - The device structure.
 * @param ch - The channel number.
 * 			   Accepted values: AD7616_VA0
 * 								AD7616_VA1
 * 								AD7616_VA2
 * 								AD7616_VA3
 * 								AD7616_VA4
 * 								AD7616_VA5
 * 								AD7616_VA6
 * 								AD7616_VA7
 * 								AD7616_VB0
 * 								AD7616_VB1
 * 								AD7616_VB2
 * 								AD7616_VB3
 * 								AD7616_VB4
 * 								AD7616_VB5
 * 								AD7616_VB6
 * 								AD7616_VB7
 * @param range - The analog input range.
 * 				  Accepted values: AD7616_2V5
 * 								   AD7616_5V
 * 								   AD7616_10V
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_set_range(ad7616_dev *dev,
						 ad7616_ch ch,
						 ad7616_range range)
{
	uint8_t	reg_addr;
	uint8_t	mask;
	uint8_t	data;
	uint8_t i;
	int32_t ret;

	if (dev->mode == AD7616_SW) {
		if (ch <= AD7616_VA7) {
			dev->va[ch] = range;
			if (ch <= AD7616_VA3) {
				reg_addr = AD7616_REG_INPUT_RANGE_A1;
				mask = AD7616_INPUT_RANGE(ch, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch, range);
			} else {
				reg_addr = AD7616_REG_INPUT_RANGE_A2;
				mask = AD7616_INPUT_RANGE(ch - AD7616_VA4, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch - AD7616_VA4, range);
			}
		} else {
			dev->vb[ch - AD7616_VB0] = range;
			if (ch <= AD7616_VB3) {
				reg_addr = AD7616_REG_INPUT_RANGE_B1;
				mask = AD7616_INPUT_RANGE(ch - AD7616_VB0, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch - AD7616_VB0, range);
			} else {
				reg_addr = AD7616_REG_INPUT_RANGE_B2;
				mask = AD7616_INPUT_RANGE(ch - AD7616_VB4, AD7616_10V);
				data = AD7616_INPUT_RANGE(ch - AD7616_VB4, range);
			}
		}
		ret = ad7616_write_mask(dev, reg_addr, mask, data);
	} else {
		for (i = 0; i <= AD7616_VA7; i++) {
			dev->va[i] = range;
			dev->vb[i] = range;
		}
		ret = gpio_set_value(&dev->gpio_dev,
				dev->gpio_hw_rngsel0, ((range & 0x01) >> 0));
		ret |= gpio_set_value(&dev->gpio_dev,
				dev->gpio_hw_rngsel1, ((range & 0x02) >> 1));
	}

	return ret;
}

/**
 * Set the operation mode (software or hardware).
 * @param dev - The device structure.
 * @param mode - The operation mode.
 * 				 Accepted values: AD7616_SW
 * 								  AD7616_HW
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_set_mode(ad7616_dev *dev,
						ad7616_mode mode)
{
	uint8_t i;
	int32_t ret = 0;

	dev->mode = mode;
	for (i = 0; i <= AD7616_VA7; i++) {
		ret |= ad7616_set_range(dev, i, dev->va[i]);
		ret |= ad7616_set_range(dev, i + AD7616_VB0, dev->vb[i]);
	}

	return ret;
}

/**
 * Set the oversampling ratio.
 * @param dev - The device structure.
 * @param osr - The oversampling ratio.
 * 				Accepted values: AD7616_OSR_0
 * 								 AD7616_OSR_2
 * 								 AD7616_OSR_4
 * 								 AD7616_OSR_8
 * 								 AD7616_OSR_16
 * 								 AD7616_OSR_32
 * 								 AD7616_OSR_64
 * 								 AD7616_OSR_128
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_set_oversampling_ratio(ad7616_dev *dev,
									  ad7616_osr osr)
{
	int32_t ret = 0;

	if (dev->mode == AD7616_SW) {
		ret = ad7616_write_mask(dev, AD7616_REG_CONFIG,
				AD7616_OS(0x7), AD7616_OS(osr));
	} else {
		ret = gpio_set_value(&dev->gpio_dev,
				dev->gpio_os0, ((osr & 0x01) >> 0));
		ret |= gpio_set_value(&dev->gpio_dev,
				dev->gpio_os1, ((osr & 0x02) >> 1));
		ret |= gpio_set_value(&dev->gpio_dev,
				dev->gpio_os2, ((osr & 0x04) >> 2));
	}

	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7616_setup(ad7616_dev **device,
					 adc_core *core,
					 ad7616_init_param init_param)
{
	ad7616_dev *dev;
	uint8_t i;
	uint32_t if_type;
	int32_t ret = 0;

	dev = (ad7616_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	dev->core = core;
	ad7616_core_read(*dev->core, AD7616_REG_UP_IF_TYPE, &if_type);
	if (if_type)
		dev->interface = AD7616_PARALLEL;
	else
		dev->interface = AD7616_SERIAL;

	dev->spi_dev.chip_select = init_param.spi_chip_select;
	dev->spi_dev.mode = init_param.spi_mode;
	dev->spi_dev.device_id = init_param.spi_device_id;
	dev->spi_dev.type = init_param.spi_type;
	if (dev->interface == AD7616_SERIAL)
		ret |= spi_init(&dev->spi_dev);

	dev->gpio_dev.device_id = init_param.gpio_device_id;
	dev->gpio_dev.type = init_param.gpio_type;
	ret |= gpio_init(&dev->gpio_dev);

	dev->gpio_reset = init_param.gpio_reset;
	if (dev->gpio_reset >= 0) {
		ret |= gpio_set_direction(&dev->gpio_dev,
						dev->gpio_reset, GPIO_OUT);
		ret |= gpio_set_value(&dev->gpio_dev,
						dev->gpio_reset,
						GPIO_HIGH);
	}

	dev->mode = init_param.mode;
	for (i = 0; i <= AD7616_VA7; i++) {
		dev->va[i] = init_param.va[i];
		dev->vb[i] = init_param.vb[i];
	}
	ret |= ad7616_set_mode(dev, dev->mode);

	dev->osr = init_param.osr;
	ret |= ad7616_set_oversampling_ratio(dev, dev->osr);

	*device = dev;

	if (!ret)
		printf("AD7616 successfully initialized\n");

	return ret;
}
