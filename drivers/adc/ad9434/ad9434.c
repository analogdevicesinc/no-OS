/***************************************************************************//**
* @file ad9434.c
* @brief Implementation of AD9434 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "ad9434.h"
#include "no_os_alloc.h"

#define DCO_DEBUG

/***************************************************************************//**
* @brief ad9434_spi_read
*******************************************************************************/
int32_t ad9434_spi_read(struct ad9434_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
* @brief ad9434_spi_write
*******************************************************************************/
int32_t ad9434_spi_write(struct ad9434_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

	return ret;
}

/***************************************************************************//**
* @brief ad9434_setup
*******************************************************************************/
int32_t ad9434_outputmode_set(struct ad9434_dev *dev,
			      uint8_t mode)
{
	int32_t ret;

	ret = ad9434_spi_write(dev, AD9434_REG_OUTPUT_MODE, mode);
	if (ret < 0)
		return ret;
	ret = ad9434_spi_write(dev, AD9434_REG_TEST_IO, TESTMODE_OFF);
	if (ret < 0)
		return ret;

	return ad9434_spi_write(dev, AD9434_REG_TRANSFER, TRANSFER_SYNC);
}

/***************************************************************************//**
* @brief ad9434_testmode_set
*******************************************************************************/
int32_t ad9434_testmode_set(struct ad9434_dev *dev,
			    uint8_t mode)
{

	ad9434_spi_write(dev, AD9434_REG_TEST_IO, 0x10);
	ad9434_spi_write(dev, AD9434_REG_TRANSFER, TRANSFER_SYNC);
	no_os_mdelay(1);
	ad9434_spi_write(dev, AD9434_REG_TEST_IO, 0x0);
	ad9434_spi_write(dev, AD9434_REG_TRANSFER, TRANSFER_SYNC);
	ad9434_spi_write(dev, AD9434_REG_TEST_IO, mode);


	return ad9434_spi_write(dev, AD9434_REG_TRANSFER, TRANSFER_SYNC);
}

/***************************************************************************//**
* @brief ad9434_data_delay
*******************************************************************************/
int32_t ad9434_data_delay(struct ad9434_dev *dev, int16_t delay)
{
	int32_t ret = 0;

	ret |= ad9434_spi_write(dev, AD9434_REG_OUTPUT_DELAY, delay);
	ret |= ad9434_spi_write(dev, AD9434_REG_TRANSFER, TRANSFER_SYNC);

	return ret;
}

/***************************************************************************//**
* @brief ad9434_setup
*******************************************************************************/
int32_t ad9434_setup(struct ad9434_dev **device,
		     struct ad9434_init_param init_param)
{
	uint8_t chip_id;
	int32_t ret = 0;
	struct ad9434_dev *dev;

	dev = (struct ad9434_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	ret |= ad9434_spi_read(dev, AD9434_REG_CHIP_ID, &chip_id);
	if(chip_id != AD9434_CHIP_ID) {
		printf("Error: Invalid CHIP ID (0x%x).\n", chip_id);
		return -1;
	}

	ret |= ad9434_outputmode_set(dev, AD9434_DEF_OUTPUT_MODE);

	*device = dev;

	printf("AD9434 successfully initialized.\n");

	return ret;
}

/***************************************************************************//**
* @brief ad9434_remove
*******************************************************************************/
int32_t ad9434_remove(struct ad9434_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
