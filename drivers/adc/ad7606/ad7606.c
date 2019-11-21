/***************************************************************************//**
 *   @file   ad7606.c
 *   @brief  Implementation of ad7606 Driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ad7606.h"

static const struct ad7606_chip_info ad7606_chip_info_tbl[] = {
	[ID_AD7605_4] = {
		.num_channels = 4,
		.has_oversampling = false,
	},
	[ID_AD7606_4] = {
		.num_channels = 4,
		.has_oversampling = true,
	},
	[ID_AD7606_6] = {
		.num_channels = 6,
		.has_oversampling = true,
	},
	[ID_AD7606_8] = {
		.num_channels = 8,
		.has_oversampling = true,
	},
	[ID_AD7606B] = {
		.num_channels = 8,
		.has_oversampling = true,
		.has_registers = true,
	},
};

int32_t ad7606_spi_reg_read(struct ad7606_dev *dev,
			    uint8_t reg_addr,
			    uint8_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = AD7606_RD_FLAG_MSK(reg_addr);
	buf[1] = 0x00;
	ret = spi_write_and_read(dev->spi_desc, buf, ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	buf[0] = AD7606_RD_FLAG_MSK(reg_addr);
	buf[1] = 0x00;
	ret = spi_write_and_read(dev->spi_desc, buf, ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	*reg_data = buf[1];

	return ret;
}

int32_t ad7606_spi_reg_write(struct ad7606_dev *dev,
			     uint8_t reg_addr,
			     uint8_t reg_data)
{
	uint8_t buf[2];

	buf[0] = AD7606_WR_FLAG_MSK(reg_addr);
	buf[1] = reg_data;

	return spi_write_and_read(dev->spi_desc, buf, ARRAY_SIZE(buf));
}

static int ad7606_spi_write_mask(struct ad7606_dev *dev,
				 uint32_t addr,
				 uint32_t mask,
				 uint32_t val)
{
	uint8_t reg_data;
	int ret;

	ret = ad7606_spi_reg_read(dev, addr, &reg_data);
	if (ret < 0)
		return ret;

	reg_data &= ~mask;
	reg_data |= val;

	return ad7606_spi_reg_write(dev, addr, reg_data);
}

int32_t ad7606_spi_read_bulk(struct ad7606_dev *dev)
{
	uint8_t size;
	int32_t ret;

	size = ad7606_chip_info_tbl[dev->device_id].num_channels * 2;
	ret = gpio_set_value(dev->gpio_convst, 0);
	if (ret < 0)
		return ret;

	ret = gpio_set_value(dev->gpio_convst, 1);
	if (ret < 0)
		return ret;

	return spi_write_and_read(dev->spi_desc, dev->data, size);
}

int32_t ad7606_spi_read_samples(struct ad7606_dev *dev,
				uint8_t channel,
				uint16_t *adc_data)
{
	int32_t ret;

	ret = ad7606_spi_read_bulk(dev);
	if (ret < 0)
		return ret;

	*adc_data = (dev->data[channel * 2] << 8 | dev->data[channel * 2 + 1]);

	return 0;
}

int32_t ad7606_reset(struct ad7606_dev *dev)
{
	int32_t ret;

	ret = gpio_set_value(dev->gpio_reset, 1);
	if (ret < 0)
		return ret;

	mdelay(1);

	return gpio_set_value(dev->gpio_reset, 0);
}

int32_t ad7606_request_gpios(struct ad7606_dev *dev,
			     struct ad7606_init_param *init_param)
{
	int32_t ret;

	ret = gpio_get(&dev->gpio_reset, &init_param->gpio_reset);
	if (ret < 0)
		return ret;

	ret = gpio_get(&dev->gpio_convst, &init_param->gpio_convst);
	if (ret < 0)
		return ret;

	ret = gpio_get(&dev->gpio_busy, &init_param->gpio_busy);
	if (ret < 0)
		return ret;

	ret = gpio_get(&dev->gpio_range, &init_param->gpio_range);
	if (ret < 0)
		return ret;

	if (!ad7606_chip_info_tbl[dev->device_id].has_oversampling)
		return 0;

	ret = gpio_get(&dev->gpio_range, &init_param->gpio_os0);
	if (ret < 0)
		return ret;

	ret = gpio_get(&dev->gpio_range, &init_param->gpio_os1);
	if (ret < 0)
		return ret;

	return gpio_get(&dev->gpio_range, &init_param->gpio_os2);
}

int32_t ad7606_set_os_ratio(struct ad7606_dev *dev,
			    enum ad7606_osr osr)
{
	int32_t ret;

	if (dev->sw_mode_en) {
		ret = ad7606_spi_reg_write(dev, AD7606_OS_MODE, osr);
		if (ret < 0)
			return ret;
	} else {
		/* In hardware mode, OSR 128 and 256 are not avaialable */
		if (osr > AD7606_OSR_64)
			osr = AD7606_OSR_64;

		ret = gpio_set_value(dev->gpio_os0, ((osr & 0x01) >> 0));
		if (ret < 0)
			return ret;

		ret = gpio_set_value(dev->gpio_os1, ((osr & 0x02) >> 1));
		if (ret < 0)
			return ret;

		ret = gpio_set_value(dev->gpio_os2, ((osr & 0x04) >> 2));
		if (ret < 0)
			return ret;
	}

	return 0;
}

int32_t ad7606_set_ch_range(struct ad7606_dev *dev, uint8_t ch,
			    enum ad7606_range range)
{
	uint8_t value;
	int32_t ret;

	if (dev->sw_mode_en) {
		ret = ad7606_spi_write_mask(dev, AD7606_RANGE_CH_ADDR(ch),
					    AD7606_RANGE_CH_MSK(ch),
					    AD7606_RANGE_CH_MODE(ch, range));
	} else {
		value = 0;
		if (range > AD7606_RANGE_5V)
			value = 1;

		ret = gpio_set_value(dev->gpio_range, value);
	}

	return ret;
}

int32_t ad7606_init(struct ad7606_dev **device,
		    struct ad7606_init_param *init_param)
{
	struct ad7606_dev *dev;
	uint8_t nr_ch;
	int32_t i, ret;

	dev = (struct ad7606_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	ret = spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret < 0)
		goto error;

	dev->device_id = init_param->device_id;
	if (ad7606_chip_info_tbl[dev->device_id].has_registers)
		dev->sw_mode_en = init_param->sw_mode_en;

	ret = ad7606_request_gpios(dev, init_param);
	if (ret < 0)
		goto error;

	ret = ad7606_reset(dev);
	if (ret < 0)
		goto error;

	/* Set the range */
	if (dev->sw_mode_en) {
		nr_ch = ad7606_chip_info_tbl[dev->device_id].num_channels;
		for (i = 0; i < nr_ch; i++) {
			ret = ad7606_set_ch_range(dev, i, init_param->range);
			if (ret < 0)
				goto error;
		}
	} else {
		ret = ad7606_set_ch_range(dev, 0, init_param->range);
		if (ret < 0)
			goto error;
	}

	ret = gpio_set_value(dev->gpio_convst, 1);
	if (ret < 0)
		goto error;

	if (ad7606_chip_info_tbl[dev->device_id].has_oversampling)
		ad7606_set_os_ratio(dev, init_param->osr);

	*device = dev;

	printf("ad7606 successfully initialized\n");

	return 0;
error:
	printf("ad7606 initialization failed\n");
	ad7606_remove(dev);
	return ret;
}

int32_t ad7606_remove(struct ad7606_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}
